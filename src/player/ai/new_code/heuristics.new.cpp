/**********************************************************************
 *
 *   FreeDoko a Doppelkopf-Game
 *
 *   Copyright (C) 2001-2017 by Diether Knof and Borg Enders
 *
 *   This program is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU General Public License as
 *   published by the Free Software Foundation; either version 2 of
 *   the License, or (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *   You can find this license in the file 'gpl.txt'.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 *   MA  02111-1307  USA
 *
 *  Contact:
 *    Diether Knof dknof@gmx.de
 *    Borg Enders  borg@borgsoft.de
 *
 ********************************************************************/

//// Datei existiert vorerst nur, um euch meine Heuristiken zu zeigen, ohne an heuristics.cpp etwas zu ändern.
//// Man könnte die Datei aber durchaus bestehen lassen und für neue, experimentelle Heuristiken nutzen.

#include "constants.h"

#include "heuristics.h"
#include "heuristics_utils.h"

#include "HeuristicInterface.h"
#include "cards_information.h"
#include "cards_information.of_player.h"
#include "team_information.h"
#include "../../game/game.h"
#include "../../party/party.h"
#include "../../party/rule.h"
#include "../../card/trick.h"

using namespace HEURISTICS_UTILS;

#define SECONDRUN_DEBUG
#define SECONDPOSITION_DEBUG


#if defined (SECONDRUN_DEBUG) || defined (SECONDPOSITION_DEBUG)
// returns the name of the card, especially a short 'EMPTY' for an empty card
string
card_name(Card const& card)
{
  if (card.is_empty())
    return "EMPTY";
  else
    return name(card);
} // card_name

#endif


/**
 ** Finds a suitable card for a ruff chance in the 2nd (or later) run of a suit.
 ** The goal is to neither waste a high trump nor a chance to bring home a precious
 ** trump nor a chance to play a plain suit pfund. For details, see below.
 **
 ** @param     trick     trick to check
 ** @param     hi        heuristic interface
 **
 ** @return    the card to play (or Card::EMPTY if no card was found)
 **
 ** @author    Tassilo Kirsch
 **
 ** @version   0.7.6
 **/
Card
Heuristics::choose_for_second_run(Trick const& t, HeuristicInterface const& hi )
{
  // @heuristic::name   choose for second run
  // @heuristic::idea   neither waste a high trump nor a chance to play a pfund when we can ruff the 2nd (or later) run of a suit

  // #1 - check conditions

  Card::Color const startcolor = t.startcolor();

  // leave it to other heuristics if...
  if(    // trick hasn't been started yet
         t.isempty()
         // it's a trump trick
      || t.startcard().istrump()
         // d'oh
      || (hi.game().type() == GAMETYPE::SOLO_MEATLESS)
         // it's the first run
      || !hi.color_runs(startcolor)
         // no opponents sitting behind
      || checkAllMyTeam(t, hi)
         // there's no chance to win the trick
      || !hi.hand().higher_card_exists(t.winnercard())
         // we have to follow suit
      || hi.hand().contains(startcolor)
    )
    return Card::EMPTY;

  Player const* partner = hi.team_information().guessed_partner();
  bool partner_backhand =  is_teammate(hi, t.lastplayer());

#ifdef SECONDRUN_DEBUG
  if (!hi.game().isvirtual() && partner_backhand)
    cout << "*2nd run (backhand): "
         << hi.cards_information().of_player(t.lastplayer()).can_have(startcolor)
         << ":"
         << hi.cards_information().of_player(t.lastplayer()).can_have(Card::TRUMP)
         << endl;
#endif

  // if a partner who can ruff sits in backhand, leave it to other heuristics
  if (   partner_backhand
      && hi.cards_information().of_player(t.lastplayer()).does_not_have(startcolor)
      && !hi.cards_information().of_player(t.lastplayer()).does_not_have(Card::TRUMP)
     )
    return Card::EMPTY;

  bool partner_wins_so_far = is_teammate(hi, t.winnerplayer());

  // if a partner has played a nut card, leave it to other heuristics
  if (partner_wins_so_far && cardWillWinTheTrick(t, hi, t.winnercard()))
    return Card::EMPTY;

  bool partner_ruffed = partner_wins_so_far  && t.winnercard().istrump();

  // here's the game plan:

  // possible actions (in general):

  // * play the nut trump
  // This is the lowest trump that guarantees the trick and will be played if the trick
  // is worth it and there's a high risk losing it otherwise.

  // * play the best trump (if we don't have the nuts)
  // This is the lowest of our highest trumps. It can't guarantee the trick, but our
  // opponents will be forced to draw a high trump of their own if they want to win.
  // Will be played if the trick is worth it and there's a high risk losing it otherwise.

  // * play a stopper
  // This is a rather low trump (usually our lowest jack in a normal game), played to
  // prevent the opponents from winning the trick with a precious trump of their own.
  // Played if the trick is worth winning but not worth a high trump and if the risk
  // is too high for playing...

  // * play the most precious trump
  // This is a low trump with a high value, like a fox or a ten. Played whenever we have
  // the balls to do so. The higher the risk of losing the trick, the lower the value
  // of the most precious trump we'll play, i.e. we start with a fox and end with a king.

  // * play a pfund
  // This is a low trump with a high value or a plain suit singleton creating a chance to
  // ruff or a high value plain suit card. Played when we think that our partner is going
  // to win the trick with one exception: if we sit right behind our partner in a normal
  // game or right in front of the soloplayer in a solo, we will try to win the trick to
  // lead the next one with the partner in backhand or the soloplayer in position 2.

  // * play a loser
  // This is a low trump with a low value or a plain suit singleton creating a chance to
  // ruff or a low value plain suit card. Played when we think or know that our team
  // can't win the trick or if the trick isn't worth playing a higher card.

  // All we need to do now, is to decide which of these actions fits best. Unfortunately,
  // that's not as trivial as one might think. The general idea is to estimate the risk of
  // losing the trick and then act accordingly, while taking other conditions (e.g. trump
  // strength, doppelkopf-risk, trick points, game points) into account.


  bool solo =    GAMETYPE::is_solo(hi.game().type())
              || ((hi.game().marriage_selector() == MARRIAGE_SELECTOR::SILENT) && (hi.team() == TEAM::RE));

  bool partner_known = partner || solo;

  // rough prediction of the points of the trick, worst case for winning/losing
  unsigned predpoints_win = t.points();
  unsigned predpoints_lose = predpoints_win;

  // minimum suit cards of an opponent sitting behind us
  unsigned min_opp_cards = 13; // if there are 13 cards in a suit, we're playing bridge
  // number of opponents sitting behind us
  unsigned opponents_behind = 0;
  // if a partner sits behind
  bool partner_behind = false;
  // if a possible partner sits behind
  bool possible_partner_behind = !partner_known;
  // soloplayer sits behind and may ruff
  bool solo_alert = false;

  // detection of the highest opposite card which may lead to a doppelkopf
  Card highest_doppelkopf_card = Card::EMPTY;

  Card const& swine = hi.game().swine();
  Card const& dolle = hi.game().dolle();
  vector<Card> doko_cards;

  if (GAMETYPE::is_normal(hi.game().type())) {
    if (!swine.is_empty())
      doko_cards.push_back(swine);
    if (!dolle.is_empty())
      doko_cards.push_back(dolle);
    if (swine.is_empty())
      doko_cards.push_back(Card::FOX);
    doko_cards.push_back(Card(Card::DIAMOND, Card::TEN));
  }

  // analyse the players sitting behind
  for (unsigned i = t.actcardno() + 1; i < hi.game().playerno(); ++i) {
    Player const& p = t.player_of_card(i);
    Hand const& hp = hi.handofplayer(p);
    unsigned n = hp.numberof(startcolor);
    bool own_team = is_teammate(hi, p);
    if (own_team) {
      partner_behind = true;

      if (n) {
        predpoints_win  += hp.highest_value(startcolor);
        predpoints_lose += hp.lowest_value(startcolor);
      } else {
        // partner will play highest plain suit or lowest trump if he has trump only
        Card::Value trumpmin = Card::NOCARDVALUE;
        Card::Value plainmax = Card::NOCARDVALUE;
        for (unsigned c = 0; c < hp.cardsnumber(); ++c) {
          HandCard const& hc = hp.card(c);
          if (hc.istrump()) {
            if ((trumpmin == Card::NOCARDVALUE) || (hc.value() < trumpmin))
              trumpmin = hc.value();
          } else {
            if ((plainmax == Card::NOCARDVALUE) || (hc.value() > plainmax))
              plainmax = hc.value();
          }
        }
        if (plainmax == Card::NOCARDVALUE)
          predpoints_win += trumpmin;
        else
          predpoints_win += plainmax;
        predpoints_lose += lowest_free_card(hp).points();
      }
    } // if (own_team)
    else {

      if (is_possible_teammate(hi, p))
        possible_partner_behind = true;

      if (n && (t.is_jabbed(hp.highest_card(startcolor))))
        partner_wins_so_far = false;

      // opponents who can't ruff will be ignored
      unsigned musthave = hi.cards_information().of_player(p).must_have(startcolor);
      if (   !hi.cards_information().of_player(p).does_not_have(Card::TRUMP)
          && !musthave
         )
      {
        ++opponents_behind;
        if (n < min_opp_cards)
          min_opp_cards = n;
        if (solo && (p == hi.game().soloplayer()))
          solo_alert = true;

        // find the highest opposite card which may lead to a doppelkopf
        if (GAMETYPE::is_normal(hi.game().type())) {
          for (vector<Card>::const_iterator dc = doko_cards.begin();
               dc != doko_cards.end();
               ++dc
              )
          {
            if (!hi.game().less(highest_doppelkopf_card, *dc))
              break;
            if (hp.contains(*dc))
              highest_doppelkopf_card = *dc;
          }
        }
      } // if (may ruff)

      if (musthave || (n > 1)) {
        predpoints_win  += hp.lowest_value(startcolor);
        predpoints_lose += hp.highest_value(startcolor);
      } else {
        predpoints_win += lowest_free_card(hp).points();
        predpoints_lose += hp.highest_value();
      }
    } // if (!own_team)
  } // for (unsigned i = t.actcardno() + 1; i < hi.game().playerno(); ++i)


  min_opp_cards = min(hi.cards_information().remaining(startcolor) / (t.remainingcardno()-1), min_opp_cards);

  // we will spare a single charlie if we have more trumps than plain suit cards
  unsigned trumps = hi.hand().numberoftrumps();
  unsigned plains = hi.hand().cardsnumber() - trumps;

  unsigned most_precious_flags = ((trumps > plains) ? PT_SPARE_CHARLIE : 0) | PT_PLAY_TO_WIN;
  // our most precious trump
  HandCard most_precious = HandCard(hi.hand(), most_precious_trump(hi, t, most_precious_flags));

  Player const& player  = hi.game().player(hi.no());

  // let partner lead (i.e. don't play for position) if he may have a first run ace
  bool let_partner_lead = false;
  if (!solo && partner) {
    for (vector<Card::Color>::const_iterator
         c = hi.game().rule().card_colors().begin();
         c != hi.game().rule().card_colors().end();
         ++c)
    {
      if (*c == hi.game().trumpcolor())
        continue;
      if (   !hi.color_runs(*c)
          && hi.handofplayer(*partner).contains(Card(*c, Card::ACE))
         )
      {
        let_partner_lead = true;
        break;
      }
    }
  }

  // check if winning the trick will provide the optimum position for the next trick
  bool play_for_position =    (   !solo // partner in backhand
                               && partner
                               && (hi.game().player_following(*partner) == player)
                               && !let_partner_lead
                              )
                           || (   solo // soloplayer in position 2
                               && (hi.team() != TEAM::RE)
                               && (hi.game().player_following(player) == hi.game().soloplayer())
                              );

#ifdef SECONDRUN_DEBUG
  if (!hi.game().isvirtual())
    cout << "*2nd run (no): " << predpoints_win << ":" << predpoints_lose
         << " PT:" << card_name(most_precious)
         << " DT:" << card_name(highest_doppelkopf_card)
         << " Opp:" << opponents_behind << "(" << min_opp_cards << ")"
         << " Par:" << partner_behind << "(" << partner_backhand << ")"
         << endl;
#endif

  // no risk
  if (!opponents_behind) {
    if (!partner_wins_so_far || play_for_position)
      return most_precious;
    else
      return choose_pfund_card( t, hi );
  }

  // from now on: spare fox if it's not endangered
  if (most_precious.isfox() && !has_endangered_fox(t, hi)) {
    most_precious_flags |= PT_SPARE_FOX;
    most_precious = HandCard(hi.hand(), most_precious_trump(hi, t, most_precious_flags));
  }

#ifdef SECONDRUN_DEBUG
  if (!hi.game().isvirtual())
    cout << "*2nd run (low): PT:" << card_name(most_precious)
         << endl;
#endif

  // low risk
  if (   (opponents_behind == 1)
      && (min_opp_cards >= 2)
      && (   partner_backhand
          || is_teammate(hi, t.startplayer(), true)
          || partner_ruffed
         )
      && !solo_alert
     )
  {
    if (!partner_wins_so_far) {
      if (!most_precious.is_empty())
        return most_precious;
      else
        // most_precious can only be empty if it was a fox before
        return Card::FOX;
    } else {
      if (play_for_position && !most_precious.is_empty())
        return most_precious;
      else
        return choose_pfund_card( t, hi );
    }
  }

  // trump which guarantees the trick
  HandCard const& nuttrump = HandCard(hi.hand(), lowest_highest_trump(t, hi, true, trumps > plains));

  // from now on: doppelkopf prevention
  bool doppelkopf_alert =    GAMETYPE::is_normal(hi.game().type())
                          && !partner_behind
                          && (t.points() >= 10 * t.actcardno());

  // to prevent a doppelkopf, make shure that most_precious either has a value below ten
  // or cannot be overtrumped by a card with a value >= ten. If we don't have such a trump,
  // the only allowed alternative is a trump ten.

  bool alternative_valid = false;

  if (   doppelkopf_alert
      && !most_precious.is_empty()
      && (most_precious.value() >= Card::TEN)
      && most_precious.less(highest_doppelkopf_card)
     )
  {
    if (!most_precious.isswine() && !most_precious.isdolle()) {
      if (!most_precious.isfox())
        alternative_valid = true;
      // sparing fox, charlie and tens
      most_precious = HandCard(hi.hand(), most_precious_trump(hi, t, most_precious_flags | PT_SPARE_FOX | PT_SPARE_TEN));
    }
    if (!most_precious.is_empty() && (most_precious.value() >= Card::TEN)) {
      if (most_precious.less(highest_doppelkopf_card))
        if (!nuttrump.is_empty())
          most_precious = nuttrump;
        else
          most_precious = HandCard::EMPTY;
    }
  }

  // card to play if we let the trick pass
  Card card_to_lose;
  if (!plains) {
    card_to_lose = lowest_trump_card(hi.hand());
    if (card_to_lose.is_empty())
      card_to_lose = hi.hand().lowest_trump();
  } else {
    card_to_lose = choose_best_fehl(t, hi, false, false); //  all ruff chances
    if (card_to_lose.is_empty() || (doppelkopf_alert && (card_to_lose.value() >= Card::TEN))) {
      Card plain_suit_loser = lowest_free_card(hi.hand());
      if (   (plain_suit_loser.value() < Card::TEN)
          || card_to_lose.is_empty()
         )
        card_to_lose = plain_suit_loser;
      if (   !GAMETYPE::is_single_picture_solo(hi.game().type())
          && (card_to_lose.value() >= Card::TEN)
         )
      {
        Card trump_loser = lowest_trump_card(hi.hand());
        if (   !trump_loser.is_empty()
            && (trump_loser.value() < card_to_lose.value())
            && hi.game().less(trump_loser, hi.trump_card_limit())
           )
          card_to_lose = trump_loser;
      }
    }
  }


#ifdef SECONDRUN_DEBUG
  if (!hi.game().isvirtual())
    cout << "*2nd run (fair): "
         << " PT:" << card_name(most_precious) << " (" << alternative_valid << ")"
         << " DoKo:" << doppelkopf_alert
         << " Nut:" << card_name(nuttrump)
         << " Lose:" << card_name(card_to_lose)
         << endl;
#endif

  HandCard card_to_play = HandCard::EMPTY;

  // fair risk
  if (   (opponents_behind == 1)
      && (min_opp_cards >= 2)
     )
  {
    if (!partner_wins_so_far) {
      if (!most_precious.is_empty())
        return most_precious;
      else if (alternative_valid)
        return Card(Card::DIAMOND, Card::TEN);
      else
        return card_to_lose;
    } else {
      if (play_for_position && !most_precious.is_empty())
        return most_precious;
      if (doppelkopf_alert)
        card_to_play = smartest_plain_suit_pfund(hi, false, true, false);
      if (!card_to_play.is_empty())
        return card_to_play;
      else
        return choose_pfund_card( t, hi );
    }
  }

  // from now on, we may have to use high trumps, see if it's necessary
  unsigned winning = hi.game().points_needed_to_win(hi.team());
  unsigned losing  = hi.game().points_needed_to_win(TEAM::opposite(hi.team()));
  unsigned points_own;
  unsigned points_opp;
  estimate_team_points(hi, points_own, points_opp);

  // we may lose the game if we lose this trick
  bool urgency =    (points_opp < losing)
                 && (points_opp + predpoints_lose + card_to_lose.points() >= losing);
  // if we have strong trumps, we should use them
  bool strength = false;

  // our highest trump
  HandCard best_trump = !nuttrump.is_empty()
                        ? nuttrump
                        : HandCard(hi.hand(), lowest_highest_trump(t, hi, false, trumps > plains));

  // card to use for tricks with many points or the chance to win the game
  Card bargain_trump = Card::EMPTY;

  if (!best_trump.is_empty()) {

    // no need to play the highest trump if it doesn't make a difference
    // (this cannot happen if we have the nut trump)
    if (   nuttrump.is_empty()
        && partner_ruffed
        && (numberof_higher_cards_others(hi, best_trump) ==
            numberof_higher_cards_others(hi, t.winnercard()))
       )
      best_trump = Card::EMPTY;
    else {

      unsigned own_strength = hi.hand().numberof_ge(best_trump);
      unsigned partner_strength = trump_tricks_guaranteed_by_partner(hi);
#ifdef SECONDRUN_DEBUG
      if (!hi.game().isvirtual())
        cout << "*2nd run (strength): "
             << " Best:" << card_name(best_trump)
             << " Strength: " << own_strength << " + " << partner_strength
             << " (" << hi.game().tricks_remaining_no() << ")"
             << endl;
#endif
      if (own_strength + partner_strength >= hi.game().tricks_remaining_no())
        return best_trump;
      strength = (own_strength > 1) && (own_strength + partner_strength > 2);

      // see if the trick is a bargain
      bool bargain = predpoints_win >= 20; // *value*
      if (   bargain
          || (   (points_own < winning)
              && (points_own + predpoints_win + best_trump.points() >= winning)
             )
         )
      {
        if ((hi.game().trumpcolor() == Card::NOCARDCOLOR) || strength)
          bargain_trump = best_trump;
        else {
          if (Card::CLUB_QUEEN.less(best_trump)) {
            if (predpoints_win > hi.value(Aiconfig::LIMITDOLLE))
              bargain_trump = best_trump;
            else if (bargain || (points_own + predpoints_win + Card::CLUB_QUEEN.points() >= winning)) {
              bargain_trump = highest_queen(hi.hand());
              if (!t.is_jabbed(bargain_trump))
                bargain_trump = best_trump;
            }
          }
          if (predpoints_win <= hi.value(Aiconfig::LIMITQUEEN))
            bargain_trump = Card::EMPTY;
        }
      }
    } // if (partner played best trump)
  } // if (!best_trump.is_empty())

  // card used to prevent that the trick can be won by a low but valuable trump
  // or, in a picture solo, card to draw a high trump from the opponents
  Card stopper = Card::EMPTY;

  // no need for a stopper if the partner has already played one
  if (   !partner_ruffed
      || t.winnercard().less(hi.lowest_trump_card_limit())
     )
  {
    switch (hi.game().type()) {
    case GAMETYPE::NORMAL:
    case GAMETYPE::POVERTY:
    case GAMETYPE::FOX_HIGHEST_TRUMP:
    case GAMETYPE::GENSCHER:
    case GAMETYPE::MARRIAGE:
    case GAMETYPE::MARRIAGE_SOLO:
    case GAMETYPE::MARRIAGE_SILENT:
    case GAMETYPE::SOLO_DIAMOND:
    case GAMETYPE::SOLO_HEART:
    case GAMETYPE::SOLO_SPADE:
    case GAMETYPE::SOLO_CLUB:
      stopper = lowest_winner(t, hi, trumps > plains, hi.lowest_trump_card_limit(), hi.trump_card_limit());
      break;

    case GAMETYPE::SOLO_JACK:
    case GAMETYPE::SOLO_QUEEN:
    case GAMETYPE::SOLO_KING:
      stopper = lowest_winner(t, hi, false, Card::EMPTY, hi.trump_card_limit());
      break;

    case GAMETYPE::SOLO_QUEEN_JACK:
      stopper = lowest_winner(t, hi, false, Card(Card::SPADE, Card::JACK), Card(Card::HEART, Card::QUEEN));
      break;

    case GAMETYPE::SOLO_KING_JACK:
      stopper = lowest_winner(t, hi, false, Card(Card::SPADE, Card::JACK), Card(Card::HEART, Card::KING));
      break;

    case GAMETYPE::SOLO_KING_QUEEN:
      stopper = lowest_winner(t, hi, false, Card(Card::SPADE, Card::QUEEN), Card(Card::HEART, Card::KING));
      break;

    case GAMETYPE::SOLO_KOEHLER:
      stopper = lowest_winner(t, hi, false, Card(Card::SPADE, Card::QUEEN), Card(Card::HEART, Card::KING));
      if (stopper.is_empty()) {
        stopper = highest_card(hi.hand(), Card::JACK);
        if (!stopper.is_empty() && !t.is_jabbed(stopper))
          stopper = Card::EMPTY;
      }
      break;

    default: // should never happen
      break;
    } // switch(hi.game().type())
  } // if (partner didn't play a stopper)

#ifdef SECONDRUN_DEBUG
  if (!hi.game().isvirtual())
    cout << "*2nd run (risk): " << points_own << ":" << points_opp
         << " Best:" << card_name(best_trump)
         << " Bargain:" << card_name(bargain_trump)
         << " Stop:" << card_name(stopper)
         << " U:" << urgency
         << " S:" << strength
         << endl;
#endif

  // medium risk (not possible if partner has ruffed)
  if (min_opp_cards >= 2)
  {
    if (urgency)
      if (!best_trump.is_empty())
        return best_trump;
    if (!bargain_trump.is_empty())
      return bargain_trump;

    if (!partner_wins_so_far) {
      if (!stopper.is_empty())
        return stopper;
      if (!most_precious.is_empty())
        return most_precious;
      else
        return card_to_lose;
    } else {
      if (play_for_position && !stopper.is_empty())
        return stopper;
      if (doppelkopf_alert)
        card_to_play = smartest_plain_suit_pfund(hi, false, true, false);
      if (!card_to_play.is_empty())
        return card_to_play;
      else
        return card_to_lose;
    }
  }

  // from now on: spare fox unconditionally
  if (most_precious.isfox() && (most_precious != nuttrump)) {
    most_precious_flags |= PT_SPARE_FOX;
    most_precious = HandCard(hi.hand(), most_precious_trump(hi, t, most_precious_flags));
  }

  // from now on, play a club queen as a high trump to reveal our team,
  // to prevent being overtrumped by our own partner
  if (   !partner_knows_me(hi)
      && possible_partner_behind
      && (hi.team() == TEAM::RE)
      && t.is_jabbed(Card::CLUB_QUEEN)
     )
  {
    if (nuttrump.is_empty()) {
      best_trump = HandCard(hi.hand(), Card::CLUB_QUEEN);
      bargain_trump = best_trump;
    }
    if (   stopper.is_empty()
        || hi.game().less(Card(Card::DIAMOND, Card::QUEEN), stopper)
       )
      stopper = Card::CLUB_QUEEN;
  }

  // From now on, there's a high risk that we're losing the trick if we're not using
  // the nut trump. But if we're losing the trick, we're not in the lead for the next
  // one, thus creating a risk to lose our nut trump if it has become our last trump
  // and other higher or equal cards are still in the game. In this situation, it's
  // better to play the nut trump if we're going to play trump at all.
  if (   !nuttrump.is_empty()
      && (trumps == 2)
      && (   numberof_higher_cards_others(hi, nuttrump, true)
          || (   hi.cards_information().remaining_others(nuttrump)
                 // only neccessary if the first is higher than the second dolle
              && (!nuttrump.isdolle() || !hi.game().less(Card::DOLLE, Card::DOLLE))
             )
         )
      && // check if overtrumping our other trump will make our nut trump the highest card left
         (   most_precious.is_empty()
          || !most_precious.less(nuttrump)
          || (numberof_higher_cards_others(hi, most_precious, false) > 1)
         )
     )
  {
    best_trump = nuttrump;
    bargain_trump = nuttrump;
    stopper = nuttrump;
  }

#ifdef SECONDRUN_DEBUG
  if (!hi.game().isvirtual())
    cout << "*2nd run (high): "
         << " PT:" << card_name(best_trump)
         << " Known:" << partner_known << "(" << possible_partner_behind << ")"
         << " Stop:" << card_name(stopper)
         << " T:" << hi.game().teaminfo(hi.no())
         << endl;
#endif

  // high risk
  if (min_opp_cards) {
    if (urgency || strength)
      if (!best_trump.is_empty())
        return best_trump;
    if (!bargain_trump.is_empty())
      return bargain_trump;
    if (!stopper.is_empty())
      return stopper;

    if (!partner_wins_so_far) {
      if (!most_precious.is_empty())
        return most_precious;
      else
        return card_to_lose;
    } else {

      if (doppelkopf_alert)
        card_to_play = smartest_plain_suit_pfund(hi, false, true, false);
      if (!card_to_play.is_empty())
        return card_to_play;
      else
        return card_to_lose;
    }
  }

  // from now on: spare trump ten unconditionally
  if ((most_precious.value() >= Card::TEN) && (most_precious != nuttrump))
    most_precious = HandCard(hi.hand(), most_precious_trump(hi, t, most_precious_flags | PT_SPARE_TEN));

  // alert risk, i.e. an opponent behind us will ruff (finally)
  if (urgency || strength)
    if (!best_trump.is_empty())
      return best_trump;
  if (!bargain_trump.is_empty())
    return bargain_trump;
  if (!stopper.is_empty())
    return stopper;
  if (   play_for_position
      && partner_ruffed
      && !best_trump.is_empty()
     )
    return best_trump;
  if (!most_precious.is_empty())
    return most_precious;

  return card_to_lose;

} // Card Heuristics::choose_for_second_run(Trick t, HeuristicInformation hi)



/**
 ** Finds a suitable card when sitting in position 2 and the partner leads with trump
 **
 ** @param     trick     trick to check
 ** @param     hi        heuristic interface
 **
 ** @return    the card to play (or Card::EMPTY if no card was found)
 **
 ** @author    Tassilo Kirsch
 **
 ** @version   0.7.6
 **/
Card
Heuristics::second_position_partner_leads_trump(Trick const& t, HeuristicInterface const& hi)
{
  // @heuristic::name   second position trump
  // @heuristic::idea   with both opponents behind, we have to prevent them from winning too easily

  // check conditions

  // leave it to other heuristics if...
  if(    // we're not in position 2
         (t.actcardno() != 1)
         // it's not a trump trick
      || !t.startcard().istrump()
         // we're in a solo
      || GAMETYPE::is_solo(hi.game().type())
         // we're playing a silent marriage
      || ((hi.game().marriage_selector() == MARRIAGE_SELECTOR::SILENT) && (hi.team() == TEAM::RE))
         // we're in an undetermined marriage
      || hi.game().undetermined_marriage()
         // partner didn't lead the trick
      || !is_teammate(hi, t.startplayer())
         // we can't overtrump our partner
      || !hi.hand().higher_card_exists(t.winnercard())
    )
    return Card::EMPTY;

  // the game plan:
  // * we determine if we want to win the trick (see below for details)
  // * if we want to win and have a trump which guarantees the trick, play it
  // * if we want to win but our highest trump doesn't improve the trump
  //   played by our partner, we leave it to other heuristics
  // * if we want to win, we play our highest trump
  // * if we don't want to win and our partner has already played a good trump,
  //   we leave it to other heuristics
  // * if we don't want to win and our partner has played a rather low trump,
  //   we make it a little more difficult for our opponents to win the trick

  Player const& player = hi.game().player(hi.no());
  HandCard const& partnercard = t.startcard();

  // we will spare a single charlie if we have more trumps than plain suit cards
  unsigned trumps = hi.hand().numberoftrumps();

  // sparing a single charlie only makes sense if the extrapoint is possible
  bool spare_charlie =    (trumps > hi.hand().cardsnumber() / 2)
                       && hi.game().rule()(Rule::EXTRAPOINT_CHARLIE);

  // lowest trump which guarantees the trick
  HandCard const& nuttrump = HandCard(hi.hand(), lowest_highest_trump(t, hi, true, spare_charlie));
  // our highest trump
  HandCard best_trump = !nuttrump.is_empty()
                        ? nuttrump
                        : HandCard(hi.hand(), lowest_highest_trump(t, hi, false, spare_charlie));

  // the card to play if we're not trying to win the trick
  Card play_low = Card::EMPTY;
  if (partnercard.less(hi.trump_card_limit())) {
    // our partner played low, we should play high
    Card play_high = Card::EMPTY;
    for (unsigned i = 0; i < hi.hand().cardsnumber(); i++)
    {
      HandCard c = hi.hand().card(i);
      if (   !t.is_jabbed(c)
          || (spare_charlie && (c == Card::CHARLIE))
         )
        continue;
      if (!c.less(hi.trump_card_limit())) {
        if (play_high.is_empty() || c.less(play_high))
          play_high = c;
      } else if (play_low.less(c) && !c.less(hi.lowest_trump_card_limit()))
        play_low = c;
    }
    if (!play_high.is_empty())
      play_low = play_high;
  }
  if (play_low.is_empty()) {
    play_low = lowest_trump_card(hi.hand());
    if (play_low.is_empty())
      play_low = best_trump;
  }

#ifdef SECONDPOSITION_DEBUG
  if (!hi.game().isvirtual())
    cout << "*2nd pos: "
         << " Nut:" << card_name(nuttrump)
         << " Best:" << card_name(best_trump)
         << " Low:" << card_name(play_low)
         << endl;
#endif

  // do we want to win the trick?
  bool play_to_win = false;

  // the next trick
  Trick nexttrick(player);

  // try to win the trick to play partner_backhand_draw_trump next
  HandCard const& partners_best = hi.handofplayer(t.startplayer()).highest_trump();
  if (cardWillWinTheTrick(nexttrick, hi, partners_best)) {
    play_to_win = true;
#ifdef SECONDPOSITION_DEBUG
    if (!hi.game().isvirtual())
      cout << "*2nd pos: play for partner's best: " << card_name(partners_best)
           << endl;
#endif
  }

  if (!play_to_win) {
    // see if we have a first run single ace to play
    Card const& ace = Heuristics::choose_ace(nexttrick, hi);
    if (   !ace.is_empty()
        && (hi.hand().numberof(ace) < hi.game().rule()(Rule::NUMBER_OF_SAME_CARDS))
        && (hi.color_runs(ace.color()) == 0)
       )
    {
      play_to_win = true;
#ifdef SECONDPOSITION_DEBUG
      if (!hi.game().isvirtual())
        cout << "*2nd pos: play for ace: " << card_name(ace)
             << endl;
#endif
    }
  }

  if (!play_to_win) {
    // see if we have a plain suit our partner can ruff
    for (vector<Card::Color>::const_iterator
         c = hi.game().rule().card_colors().begin();
         c != hi.game().rule().card_colors().end();
         ++c)
    {
      if (*c == hi.game().trumpcolor())
        continue;
      unsigned n = hi.hand().numberof(*c);
      if (   n
          && (n < hi.cards_information().remaining(*c))
          && !hi.handofplayer(t.startplayer()).existstcolor(*c)
         )
      {
        play_to_win = true;
#ifdef SECONDPOSITION_DEBUG
        if (!hi.game().isvirtual())
          cout << "*2nd pos: play for plain suit ruff: " << *c
               << endl;
#endif
        break;
      }
    }
  }


  // Until now, we've checked if we want to win the trick because
  // we want to lead the next trick. From now on, we're checking if
  // we want to win the trick for the team. But if our team is already
  // winning. i.e. if our partner already played a nut card, we can
  // happiliy leave it to other heuristics

  if (!play_to_win && cardWillWinTheTrick(t, hi, t.startcard())) {
#ifdef SECONDPOSITION_DEBUG
      if (!hi.game().isvirtual())
        cout << "*2nd pos: nothing to do, partner played nut"
             << endl;
#endif
    return Card::EMPTY;
  }

  if (!play_to_win && !nuttrump.is_empty()) {
    // see if we can win all remaining tricks
    if (   hi.hand().numberof_ge(nuttrump)
           + trump_tricks_guaranteed_by_partner(hi)
        >= hi.game().tricks_remaining_no()
       )
    {
      play_to_win = true;
#ifdef SECONDPOSITION_DEBUG
      if (!hi.game().isvirtual())
        cout << "*2nd pos: play to win all tricks: "
             << hi.hand().numberof_ge(nuttrump)
             << " + "
             << trump_tricks_guaranteed_by_partner(hi)
             << " >= "
             << hi.game().tricks_remaining_no()
             << endl;
#endif
    }
  }

  if (!play_to_win) {

    // check if we might lose or can win the game with this trick
    unsigned winning = hi.game().points_needed_to_win(hi.team());
    unsigned losing  = hi.game().points_needed_to_win(TEAM::opposite(hi.team()));
    unsigned points_own;
    unsigned points_opp;
    estimate_team_points(hi, points_own, points_opp);

    if (play_low != nuttrump) {
      // rough prediction of a worst case scenario for losing the trick
      Trick predtrick = t;
      predtrick += HandCard(hi.hand(), play_low);
      unsigned predpoints = predtrick.points();

      unsigned maxwinner[] = {0, 0};
      unsigned maxpoints[] = {0, 0};

      // analyse the players sitting behind
      for (unsigned i = t.actcardno() + 1; i < hi.game().playerno(); ++i) {
        HandCards const& hp = hi.handofplayer(t.player_of_card(i)).validcards(predtrick);
        unsigned p = i - t.actcardno() - 1;
        for (unsigned c = 0; c < hp.cardsnumber(); ++c) {
          HandCard const& card = hp.card(c);
          if (predtrick.is_jabbed(card)) {
            if (!maxwinner[p] || (card.points() > maxwinner[p]))
              maxwinner[p] = card.points();
          }
          if (!maxpoints[p] || (card.points() > maxpoints[p]))
            maxpoints[p] = card.points();
        }
      } // for (unsigned i = t.actcardno() + 1; i < hi.game().playerno(); ++i)
      if (!maxwinner[0])
        predpoints += maxpoints[0] + maxwinner[1];
      else if (!maxwinner[1])
        predpoints += maxwinner[0] + maxpoints[1];
      else
        predpoints += max(maxpoints[0] + maxwinner[1], maxwinner[0] + maxpoints[1]);

      // we may lose the game if we lose this trick
      if (   (points_opp < losing)
          && (points_opp + predpoints >= losing)
         )
      {
        play_to_win = true;
#ifdef SECONDPOSITION_DEBUG
        if (!hi.game().isvirtual())
          cout << "*2nd pos: urgency: "
               << points_opp << " + " << predpoints << " >= " << losing
               << endl;
#endif
      }
    }

    if (!play_to_win) {
      // rough prediction of a worst case scenario for winning the trick
      Trick predtrick = t;
      predtrick += HandCard(hi.hand(), best_trump);
      unsigned predpoints = predtrick.points();

      // analyse the players sitting behind
      for (unsigned i = t.actcardno() + 1; i < hi.game().playerno(); ++i) {
        predpoints += hi.handofplayer(t.player_of_card(i)).validcards(predtrick).lowest_value();
      }

      // we may win the game if we win this trick
      if (   (points_own < winning)
          && (points_own + predpoints >= winning)
         )
      {
        play_to_win = true;
#ifdef SECONDPOSITION_DEBUG
        if (!hi.game().isvirtual())
          cout << "*2nd pos: opportunity: "
               << points_own << " + " << predpoints << " >= " << winning
               << endl;
#endif
      }
    }
  } // if (!play_to_win)


  // if it's worth it, play the nuts
  if (   !nuttrump.is_empty()
      && (   play_to_win
          || (trumps < 3) // *value*
          || (t.points() >= 10) // partner played a pfund
         )
     )
  {
     if (   (hi.team() == TEAM::RE)
        && nuttrump.less(Card::CLUB_QUEEN)
        && !partner_knows_me(hi)
       )
    {
#ifdef SECONDPOSITION_DEBUG
      if (!hi.game().isvirtual())
        cout << "*2nd pos: reveal identity with nuttrump"
             << endl;
#endif
      // let's reveal our identity
      return Card::CLUB_QUEEN;
    } else {
#ifdef SECONDPOSITION_DEBUG
      if (!hi.game().isvirtual())
        cout << "*2nd pos: play nuttrump"
             << endl;
#endif
      return nuttrump;
    }
  } // if (play nuttrump)

  // playing our highest trump doesn't make a difference - leave it to other heuristics
  // (this cannot happen if we have the nut trump)
  if (   nuttrump.is_empty()
      && !is_better_card(hi, best_trump, partnercard)
     )
  {
#ifdef SECONDPOSITION_DEBUG
    if (!hi.game().isvirtual())
      cout << "*2nd pos: cannot improve partner's card: "
           << numberof_higher_cards_others(hi, best_trump)
           << endl;
#endif
    return Card::EMPTY;
  }

  // if it's worth it, play our highest trump
  if (   !best_trump.is_empty()
      && (   play_to_win
          || (t.points() >= 10) // partner played a pfund
         )
     )
  {
    if (   (hi.team() == TEAM::RE)
        && best_trump.less(Card::CLUB_QUEEN)
        && !partner_knows_me(hi)
       )
    {
#ifdef SECONDPOSITION_DEBUG
      if (!hi.game().isvirtual())
        cout << "*2nd pos: reveal identity with best trump"
             << endl;
#endif
      // let's reveal our identity
      return Card::CLUB_QUEEN;
    } else {
#ifdef SECONDPOSITION_DEBUG
      if (!hi.game().isvirtual())
        cout << "*2nd pos: play best trump"
             << endl;
#endif
      return best_trump;
    }
  } // if (play best trump)

  if (!partnercard.less(hi.trump_card_limit())) {
#ifdef SECONDPOSITION_DEBUG
    if (!hi.game().isvirtual())
      cout << "*2nd pos: partner's trump is good enough"
           << endl;
#endif
    // partner already played high trump, leave it to other heuristics
    return Card::EMPTY;
  } else {
#ifdef SECONDPOSITION_DEBUG
    if (!hi.game().isvirtual())
      cout << "*2nd pos: stay low"
           << endl;
#endif
    // trick isn't worth a high trump, stay low
    return play_low;
  }

} // Card Heuristics::second_position_partner_leads_trump(Trick t, HeuristicInformation hi)

