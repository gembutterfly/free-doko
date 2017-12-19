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

#define MEATLESS_LEAD_TRICK_CONTRA_DEBUG

/**
 ** play a pfund if own team will win the trick
 **
 ** @param      t         current trick
 ** @param      hi        heuristic interface
 **
 ** @return     card to play, 'Card::EMPTY' if the heuristic does not match
 **
 ** @author     Tassilo Kirsch
 **
 ** @version    0.7.6
 **/
Card
Heuristics::choose_pfund_meatless( Trick const& t, HeuristicInterface const& hi )
{
  // @heuristic::name   choose pfund meatless
  // @heuristic::idea   if own team wins the trick, we can play a pfund or signal a winner card

  using namespace CARD_SIGNALS;

  // leave it to other heuristics if...
  if (   // I'm leading this trick
         t.isstartcard()
      || // not a meatless solo
         (hi.game().type() != GAMETYPE::SOLO_MEATLESS)
         // I'm the solo player
      || (hi.no() == hi.game().soloplayer().no())
     )
    return Card::EMPTY;


  // check if my team can win this trick
  bool can_win = false;
  if (hi.teamofplayer(t.winnerplayer()) == hi.team())
    can_win = !oppositeTeamCanWinTrick(t, hi);
  else
    // solo player is winning so far, only partners are sitting behind
    for (unsigned p = t.actcardno() + 1; p < hi.game().playerno(); ++p)
      if (player_has_higher_card(hi, t.player_of_card(p), t.winnercard())) {
        can_win = true;
        break;
      }
  if (!can_win)
    return Card::EMPTY;


  if (hi.hand().existstcolor(t.startcolor())) {

    // I have to follow suit

    Card highest = hi.hand().highest_card(t.startcolor());
    Card lowest = hi.hand().lowest_card(t.startcolor());
    if (t.winnercard().less(lowest))
      // I'll have to take the trick, leave it to other heuristics
      return Card::EMPTY;

    // estimate minimum number of suit cards still to be played in the trick
    unsigned players_follow = 0;
    for (unsigned p = t.actcardno() + 1; p < hi.game().playerno(); ++p)
      if (hi.cards_information().of_player(t.player_of_card(p)).must_have(t.startcolor()))
        ++players_follow;
    players_follow = min(players_follow, static_cast<unsigned>(t.winnerplayer() == hi.game().soloplayer()));

    // maximum number of suit cards remaining after the trick
    unsigned remaining_after = hi.cards_information().remaining_others(t.startcolor()) - players_follow;

    // play highest pfund if all other players are out of the suit
    if (   !remaining_after
        || (   (remaining_after == 1)
            && !t.has_played(hi.game().soloplayer())
           )
       )
      return highest;

    Card highest_others = hi.cards_information().highest_remaining_card_of_others(t.startcolor());
    if (t.is_jabbed(highest)) {

      // I can win the trick, check if I want to

      // number of my guaranteed winners of the current suit
      unsigned winners = hi.hand().numberof_ge(highest_others);

      // if I can win an additional trick or get all remaining tricks in this suit,
      // I'll take the trick with my highest card
      if (   (winners > 1)
          || (winners > remaining_after)
         )
        return highest;

      // if a partner sitting behind has signalled a winner, take the trick
      bool behind_soloplayer = false;
      for (unsigned p = t.actcardno() + 1; p < hi.game().playerno(); ++p) {
        Player const& player = t.player_of_card(p);
        if (player == hi.game().soloplayer())
          behind_soloplayer = true;
        else if (   estimate_numberof(hi, player, highest_others)
                 && (   !behind_soloplayer
                     || !hi.handofplayer(hi.game().soloplayer()).contains(highest_others)
                    )
                )
          return highest;
      }
    }

    // de-block my partner's suit
    if (   (t.winnerplayer() != hi.game().soloplayer())
        && (t.winnerplayer() == t.startplayer())
        && !t.winnercard().less(highest)
        && !hi.game().less(highest, highest_others)
           // don't de-block if we've signalled a winner
        && (weighting_to_signal(hi.cards_information().of_player(hi.no()).weighting(highest)) < SIGNAL_ONE)
       )
      return highest;

    unsigned own_cards = hi.hand().numberof(t.startcolor());

    // bring home an ace if it's endangered
    if (   (own_cards == 2)
        && (highest.value() == Card::ACE)
        && (highest_others == highest)
       )
      return highest;

    // bring home a ten if it's not a stopper after the trick
    if (highest.value() == Card::TEN) {
      unsigned aces_and_tens = hi.cards_information().played(Card(t.startcolor(), Card::ACE))
                               + hi.cards_information().played(Card(t.startcolor(), Card::TEN));
      unsigned own_tens = hi.hand().numberof(Card(t.startcolor(), Card::TEN));
      if (5 - aces_and_tens - own_tens >= own_cards)
        return highest;
    }

    // finally, pfund our second best card
    Card best_pfund = Card::EMPTY;
    Card highest_pfund = Card::EMPTY;
    for (HandCards::const_iterator c = hi.hand().cards().begin();
         c != hi.hand().cards().end();
         ++c)
    {
      if (   c->less(highest_others) // don't pfund a winner
          && best_pfund.less(*c)
         )
        best_pfund = *c;
      if (   c->less(highest)
          && highest_pfund.less(*c)
         )
        highest_pfund = *c;
    }
    if (best_pfund.is_empty())
      return highest_pfund;
    return best_pfund;

  } else {

    // I don't have to follow suit

    // * show double aces (or ace plus all remaining tens) by playing the ace
    // * show winner (ace or ten) by playing a low card from its suit
    // * highest card that's not a likely winner

    // card to signal our partners a suit in which we have a winner
    // priority: save suit (solo player can't win) - best suit (most winners) - longest suit - highest card
    Card marker = Card::EMPTY;
    unsigned marker_nuts = 0;
    unsigned marker_winners = 0;
    unsigned marker_len = 0;

    // if we don't play a signal, we'll play a pfund
    Card pfund = Card::EMPTY;

    // now analyse the suits
    for (vector<Card::Color>::const_iterator
         c = hi.game().rule().card_colors().begin();
         c != hi.game().rule().card_colors().end();
         ++c)
    {
      if (*c == t.startcolor())
        continue;

      unsigned len = hi.hand().numberof(*c);
      if (!len)
        continue;

      unsigned remaining = hi.cards_information().remaining_others(*c);
      if (!remaining) {
        Card card = hi.hand().highest_card(*c);
        if (pfund.is_empty() || (pfund.value() < card.value()))
          pfund = card;
        continue;
      }

      Card highest_others = hi.cards_information().highest_remaining_card_of_others(*c);
      Card ace = Card(*c, Card::ACE);
      Card ten = Card(*c, Card::TEN);

      unsigned winners = hi.hand().numberof_ge(highest_others);
      unsigned nuts = hi.hand().numberof_g(highest_others);
      unsigned aces = hi.hand().numberof(ace);
      unsigned tens = hi.hand().numberof(ten);

      // we signal aces and tens only
      if (   (aces || tens)
          && winners
          && (nuts >= marker_nuts)
          && (   (winners > marker_winners)
              || ((winners == marker_winners) && (len >= marker_len))
             )
         )
      {
        // signalling a winner only makes sense if our partners can play the suit for us
        bool do_mark = false;
        Card mark = Card::EMPTY;
        for (unsigned p = 0; p < hi.game().playerno(); ++p)
          if (   (p != hi.no())
              && (p != hi.game().soloplayer().no())
              && hi.handofplayer(p).contains(*c)
             )
          {
            do_mark = true;
            break;
          }

        // make shure that we've not already marked this suit
        if (   do_mark
            && (   (aces && (weighting_to_signal(hi.cards_information().of_player(hi.no()).weighting(ace)) >= SIGNAL_ONE))
                || (!aces && (weighting_to_signal(hi.cards_information().of_player(hi.no()).weighting(ten)) >= SIGNAL_ONE))
               )
           )
          do_mark = false;

        if (do_mark) {

          // use an ace as a signal only if we have a second winner that's better than the highest remaining card
          if (aces && (hi.hand().numberof_g(highest_others) > 1))
            mark = Card(*c, Card::ACE);
          else {
            for (HandCards::const_iterator card = hi.hand().cards().begin();
                 card != hi.hand().cards().end();
                 ++card)
              if (  (card->color() == *c)
                  && (card->value() < Card::KING)
                  && mark.less(*card)
                 )
                mark = *card;
          }

          if (   !mark.is_empty()
              && (   marker.is_empty()
                  || (nuts && !marker_nuts)
                  || (winners > marker_winners)
                  || (len > marker_len)
                  || (mark.value() > marker.value())
                 )
             )
            marker = mark;
          else
            do_mark = false;
        }

        if (!do_mark) {
          if (pfund.is_empty() || (pfund.value() < Card::KING))
            if (hi.hand().contains(Card(*c, Card::KING)))
              pfund = Card(*c, Card::KING);
        }
        continue;

      } // check playing a signal

      // highest possible pfund already found
      if (!pfund.is_empty() && (pfund.value() == Card::TEN))
        continue;

      // don't pfund a stopper
      unsigned aces_and_tens = hi.cards_information().played(Card(*c, Card::ACE))
                               + hi.cards_information().played(Card(*c, Card::TEN));
      if (len > 4 - aces_and_tens- aces - tens)
        continue;

      // find highest pfund
      Card card = hi.hand().highest_card(*c);
      if (pfund.is_empty() || (pfund.value() < card.value()))
        pfund = card;

    }
    if (!marker.is_empty())
      return marker;
    if (!pfund.is_empty())
      return pfund;

    // last resort - we couldn't find a good pfund
    return choose_pfund_card(t, hi);

  }

  return Card::EMPTY;
} // Card Heuristics::choose_pfund_meatless( Trick t, HeuristicInterface hi )



/**
 ** find the best card for the contra party to lead a trick in a meatless solo
 **
 ** @param     trick   current trick
 ** @param     hi      heuristic interface
 **
 ** @return    card to play, 'Card::EMPTY' if the heuristic does not match
 **
 ** @author    Tassilo Kirsch
 **
 ** @version   0.7.6
 **/
Card
Heuristics::meatless_lead_trick_contra(Trick const& trick, HeuristicInterface const& hi)
{
  // @heuristic::name   meatless lead trick contra
  // @heuristic::idea   see below

  // This heuristic tries to find the best card to lead a trick by one of the contra players
  // in a meatless solo. It's actually many "micro heuristics" in one, combined here because
  // they're all using the same set of info data (which therefore has to be calculated only once)
  // and the order in which they're called is highly important.

  // Some basic concepts:
  // * If the game can be won or lost by winning/losing the current trick, only winners will be played.
  // * If a partner has signalled possession of the highest card of a suit and it's the last remaining
  //   copy of the card or the solo player sits behind our partner, we will play our own winners first
  //   and then play our partner's suit
  // * We will prefer suits led by our partner and stay away from suits led by the solo player
  // * Since our partners will de-block, playing a first run ace without having the ten has a low priority

  // For the actual strategy, see comments below after "// choose card to play".

  // Finally, if no suitable card can be found, meatless_lead_trick_contra will work as a locking
  // heuristic and lock suits which have a stopper.


  // the player
  Player const& player = hi.game().player( hi.no() );

  // @heuristic::condition   first card to be played by contra party in a meatless solo
  if (   (hi.game().type() != GAMETYPE::SOLO_MEATLESS)
      || !trick.isstartcard()
      || player.isre()
     )
    return Card::EMPTY;

  // the infos we need for each suit
  struct SuitInfo {
    unsigned len;
    unsigned aces, tens, kings;
    Card::Value highest_below_ace; // exists if len > aces
    Card::Value highest_below_ten; // exists if len > aces + tens
    Card::Value best, lowest;      // exist if len > 0
    Card::Value secondbest;        // exists if len > 1
    unsigned winners;              // number of winners (if we're in the lead)
    bool stopper, stopper_remains; // if the suit has a stopper (TEN or KING), after the trick
    unsigned runs;                 // this should match color_runs
    unsigned played;               // number of suit cards already played
    unsigned aces_played, tens_played, kings_played;
    TEAM::Team played_by;          // which player/team led the suit's last trick
                                   // RE:           the solo player
                                   // UNKNOWN:      either no-one or contra after the solo player
                                   // CONTRA:       a partner
                                   // MAYBE_CONTRA: the player himself
    bool marked, save;             // if a partner has signalled a winner, if it's save to play
    bool soloplayer_is_void;
    Card::Value highest_remaining;
  };
  SuitInfo suit_info[] = { {0}, {0}, {0}, {0} };

  // get highest remaining card of the suit and init lowest card
  for (vector<Card::Color>::const_iterator
       c = hi.game().rule().card_colors().begin();
       c != hi.game().rule().card_colors().end();
       ++c)
  {
    SuitInfo& suit = suit_info[*c];
    suit.highest_remaining = hi.cards_information().highest_remaining_card_of_others(*c).value();
    suit.lowest = Card::NOCARDVALUE;
  }

  // iterate hand's cards only once to calc all neccessary values (instead of using numberof methods)
  for( unsigned i = 0; i < hi.hand().cardsnumber(); i++ )
  {
    HandCard c = hi.hand().card(i);
    SuitInfo& suit = suit_info[c.color()];

    suit.len++;
    if (c.value() > suit.best) {
      suit.secondbest = suit.best;
      suit.best = c.value();
    } else if (c.value() > suit.secondbest) {
      suit.secondbest = c.value();
    }
    if ((suit.lowest == Card::NOCARDVALUE) || (c.value() < suit.lowest))
      suit.lowest = c.value();
    if (c.value() == Card::ACE)
      suit.aces++;
    else {
      if (c.value() > suit.highest_below_ace)
        suit.highest_below_ace = c.value();
      if (c.value() == Card::TEN)
        suit.tens++;
      else {
        if (c.value() > suit.highest_below_ten)
          suit.highest_below_ten = c.value();
        if (c.value() == Card::KING)
          suit.kings++;
      }
    }
    suit.winners +=    (suit.highest_remaining == Card::NOCARDVALUE)
                    || (c.value() >= suit.highest_remaining);
  }

  // we consider all the following infos important to a meatless solo,
  // therefore we don't forget any tricks
  for (unsigned i = 0; i < hi.game().trick_current_no(); i++) {
    Trick const& t = hi.game().trick(i);
    Player const& startplayer = t.startcard().player();
    Card::Color const& startcolor = t.startcolor();
    TEAM::Team const startteam = hi.teamofplayer(startplayer);
    suit_info[t.startcolor()].runs++;

    for (unsigned cardno = 0; cardno < t.actcardno(); ++cardno) {
      HandCard const& c = t.card(cardno);
      SuitInfo& suit = suit_info[c.color()];

      suit.played++;
      if (c.value() == Card::ACE)
        suit.aces_played++;
      else if (c.value() == Card::TEN)
        suit.tens_played++;
      else if (c.value() == Card::KING)
        suit.kings_played++;

      if ((c.color() != startcolor) && (c.player() == hi.game().soloplayer()))
        suit_info[startcolor].soloplayer_is_void = true;

      if (cardno == 0) {
        if (startteam == TEAM::CONTRA) {
          if (suit.played_by == TEAM::RE)
            suit.played_by = TEAM::UNKNOWN;
          else if (startplayer == player)
            suit.played_by = TEAM::MAYBE_CONTRA;
          else
            suit.played_by = TEAM::CONTRA;
        } else {
          suit.played_by = TEAM::RE;
        }
      }
    }
  } // for (unsigned i = 0; i < hi.game().trick_current_no(); i++)


  // calculate additional information for each suit
  HandCard best_winner(hi.hand());
  HandCard best_loser(hi.hand());
  bool play_winner = false;

  for (vector<Card::Color>::const_iterator
       c = hi.game().rule().card_colors().begin();
       c != hi.game().rule().card_colors().end();
       ++c)
  {
    SuitInfo& suit = suit_info[*c];
    if (!suit.len)
      continue;

    // check if the suit has a stopper (after the trick)
    // * ten  + (4 - number of aces and tens already played - number of own aces and tens) cards
    // * king + (6 - number of aces, tens and kings already played - number of own aces, tens and kings) cards

    unsigned aces_and_tens = suit.aces_played + suit.tens_played + suit.aces + suit.tens;

    suit.stopper =    (   suit.tens
                       && (suit.len > 4 - aces_and_tens)
                      )
                   ||
                      (   suit.kings
                       && (suit.len > 6 - (aces_and_tens + suit.kings_played + suit.kings))
                      );

    suit.stopper_remains =    (     (suit.aces || (suit.tens == 2))
                                 && suit.tens
                                 && (suit.len > 5 - aces_and_tens)
                              )
                           ||
                              (     (suit.aces || suit.tens || (suit.kings == 2))
                                 && suit.kings
                                 && (suit.len > 7 - (aces_and_tens + suit.kings_played + suit.kings))
                              );


    // calculate best winner/loser
    if (suit.winners && best_winner.less(Card(*c, suit.best)))
      best_winner = Card(*c, suit.best);

    if (   (suit.winners < suit.len)
        && (best_loser.is_empty() || Card(*c, suit.lowest).less(best_loser))
       )
      best_loser = Card(*c, suit.lowest);

    // check if the suit has been marked by one of our partners
    bool behind_soloplayer = false;
    for (unsigned p = 1; p < hi.game().playerno(); ++p) {
      Player const& player = trick.player_of_card(p);
      if (player == hi.game().soloplayer())
        behind_soloplayer = true;
      else {
        Card const& highest =  Card(*c, suit.highest_remaining);
        // make shure that the solo player can't intercept
        suit.marked = estimate_numberof(hi, player, highest);
        suit.save =    suit.marked
                    && (   !behind_soloplayer
                        || !hi.handofplayer(hi.game().soloplayer()).contains(highest)
                       );
        play_winner = play_winner || suit.save;
      }
    }

  } // calculate stoppers and the best winner card

  // sort the suits by length
  Card::Color sorted[4];
  sorted[0] = Card::DIAMOND;
  for (int c = Card::HEART; c <= Card::CLUB; c++) {
    int i = 0;
    while ((i < c) && (suit_info[c].len <= suit_info[sorted[i]].len))
      i++;
    for (int j = c; j > i; j--) {
      sorted[j] = sorted [j-1];
    }
    sorted[i] = Card::InttoColor(c);
  }

  // check if we might lose or can win the game with this trick
  if (!play_winner && !best_winner.is_empty() && !best_loser.is_empty()) {

    unsigned winning = hi.game().points_needed_to_win(hi.team());
    unsigned losing  = hi.game().points_needed_to_win(TEAM::opposite(hi.team()));
    unsigned points_own;
    unsigned points_opp;
    estimate_team_points(hi, points_own, points_opp);

    unsigned predwin = meatless_estimated_trick_points(trick, hi, best_winner, true);
    unsigned predlose = meatless_estimated_trick_points(trick, hi, best_loser, false);

    play_winner =    // we may lose the game if we lose this trick
                     (   (points_opp < losing)
                      && (points_opp + predlose >= losing)
                     )
                     // we can win the game if we win this trick
                  || (   (points_own < winning)
                      && (points_own + predwin >= winning)
                     );
#ifdef MEATLESS_LEAD_TRICK_CONTRA_DEBUG
    if (!hi.game().isvirtual())
      cout << "*lead contra -"
           << " pred: " << predwin << ":" << predlose
           << " points: " << points_own << ":" << points_opp
           << " game: " << winning << ":" << losing
           << " win: " << (play_winner ? "yes" : "no")
           << endl;
#endif
  }


// define some macros 'cos this programmer is lazy
#ifdef MEATLESS_LEAD_TRICK_CONTRA_DEBUG

#define PLAY(val, msg) \
{ \
  if (!hi.game().isvirtual()) \
    cout << "* " << msg << endl; \
  return Card(sorted[i], val); \
}

#define PLAYCARD(msg) \
if (!card_to_play.is_empty()) { \
  if (!hi.game().isvirtual()) \
    cout << "* " << msg << endl; \
  return card_to_play; \
}

#else

#define PLAY(val, msg) \
return Card(sorted[i], val)

#define PLAYCARD(msg) \
if (!card_to_play.is_empty()) \
  return card_to_play; \

#endif

#define SETCARD(val) \
if (   card_to_play.is_empty() \
    || (   (suit.len == suit_info[card_to_play.color()].len) \
        && (val > card_to_play.value()) \
       ) \
   ) \
  card_to_play = Card(sorted[i], val)


  // choose card to play
  Card card_to_play = Card::EMPTY;

  // * highest card from the longest suit in which we'll get all remaining tricks
  for (int i = 0; i < 4; i++) {
    SuitInfo& suit = suit_info[sorted[i]];
    unsigned remaining = hi.game().cards_no(sorted[i]) - suit.played - suit.len;
    if (suit.len && (suit.winners >= remaining))
      SETCARD(suit.best);
  }
  PLAYCARD("highest card from the longest suit in which we'll get all remaining tricks");

  // * winner from the longest suit which holds another winner after the trick
  for (int i = 0; i < 4; i++) {
    SuitInfo& suit = suit_info[sorted[i]];
    if (suit.winners > 1)
      SETCARD(suit.best);
  }
  PLAYCARD("winner from the longest suit which holds another winner after the trick");

  // * winner from a suit in which a partner has signalled to have a winner
  // (pre- de-block parnter's suit, partner should not de-block)
  for (int i = 0; i < 4; i++) {
    SuitInfo& suit = suit_info[sorted[i]];
    if (suit.winners && suit.marked)
      SETCARD(suit.best);
  }
  PLAYCARD("winner from a suit in which a partner has signalled to have a winner");

  // * winner from the longest suit which still has a stopper after the trick
  for (int i = 0; i < 4; i++) {
    SuitInfo& suit = suit_info[sorted[i]];
    if (suit.winners && suit.stopper_remains)
      SETCARD(suit.best);
  }
  PLAYCARD("winner from the longest suit which still has a stopper after the trick");

  // * singleton ace from an already opened suit
  for (int i = 0; i < 4; i++) {
    SuitInfo& suit = suit_info[sorted[i]];
    if ((suit.len == 1) && suit.aces && suit.runs)
       PLAY(Card::ACE, "singleton ace from an already opened suit");
  }

  // * winner from a suit last opened by one of the partners
  for (int i = 0; i < 4; i++) {
    SuitInfo& suit = suit_info[sorted[i]];
    if (suit.winners && (suit.played_by == TEAM::CONTRA))
      SETCARD(suit.best);
  }
  PLAYCARD("winner from a suit last opened by one of the partners");

  // * winner below ace from a suit not opened yet
  for (int i = 0; i < 4; i++) {
    SuitInfo& suit = suit_info[sorted[i]];
    if (suit.winners && !suit.aces && !suit.runs)
      SETCARD(suit.best);
  }
  PLAYCARD("winner below ace from a suit not opened yet");

  // * endangered singleton winner below ace
  // endangered means that another copy of the same card is still in the game
  for (int i = 0; i < 4; i++) {
    SuitInfo& suit = suit_info[sorted[i]];
    if ((suit.len == 1) && suit.winners && !suit.aces && (suit.best == suit.highest_remaining))
      SETCARD(suit.best);
  }
  PLAYCARD("endangered singleton winner below ace");

  // * any winner below ace
  for (int i = 0; i < 4; i++) {
    SuitInfo& suit = suit_info[sorted[i]];
    if (suit.winners && !suit.aces)
      SETCARD(suit.best);
  }
  PLAYCARD("any winner below ace");

  if (!play_winner) {

    // * highest card from the longest suit in which the solo player is void
    for (int i = 0; i < 4; i++) {
      SuitInfo& suit = suit_info[sorted[i]];
      if (suit.len && suit.soloplayer_is_void && !suit.marked)
        SETCARD(suit.best);
    }
    PLAYCARD("highest card from the longest suit in which the solo player is void");

    // * highest card below ten from a partner's winner suit which may be intercepted
    for (int i = 3; i >= 0; --i) {
      SuitInfo& suit = suit_info[sorted[i]];
      if ((suit.len > suit.tens) && suit.marked)
        SETCARD(suit.highest_below_ten);
    }
    PLAYCARD("highest card below ten from a partner's winner suit which may be intercepted");

    // * highest card below ten from a suit last opened by one of the partners
    for (int i = 3; i >= 0; --i) {
      SuitInfo& suit = suit_info[sorted[i]];
      if ((suit.played_by == TEAM::CONTRA) && (suit.len > suit.tens))
        SETCARD(suit.highest_below_ten);
    }
    PLAYCARD("highest card below ten from a suit last opened by one of the partners");

    // * highest card below ace in longest suit with ace-x-x-x
    for (int i = 0; i < 4; i++) {
      SuitInfo& suit = suit_info[sorted[i]];
      if ((suit.len > 3) && suit.aces)
        SETCARD(suit.highest_below_ten);
    }
    PLAYCARD("highest card below ace in longest suit with ace-x-x-x");

    // * highest card below ten from longest x-x-x-x suit not opened by the solo player
    for (int i = 0; i < 4; i++) {
      SuitInfo& suit = suit_info[sorted[i]];
      if ((suit.len > 3) && !suit.aces && (suit.played_by != TEAM::RE))
        SETCARD(suit.highest_below_ten);
    }
    PLAYCARD("highest card below ten from longest x-x-x-x suit not opened by solo player");

    // * highest card below ten from longest x-x-x-x suit
    for (int i = 0; i < 4; i++) {
      SuitInfo& suit = suit_info[sorted[i]];
      if ((suit.len > 3) && !suit.aces)
        SETCARD(suit.highest_below_ten);
    }
    PLAYCARD("highest card below ten from longest x-x-x-x suit");

  } // if (!play_winner)

  // * ace from ace-10-x if suit hasn't been opened by the solo player
  for (int i = 0; i < 4; i++) {
    SuitInfo& suit = suit_info[sorted[i]];
    if ((suit.len > 2) && suit.aces && suit.tens && (suit.played_by != TEAM::RE))
      PLAY(Card::ACE, "ace from ace-10-x if suit hasn't been opened by the solo player");
  }

  // * ace from ace-10-x
  for (int i = 0; i < 4; i++) {
    SuitInfo& suit = suit_info[sorted[i]];
    if ((suit.len > 2) && suit.aces && suit.tens)
      PLAY(Card::ACE, "ace from ace-10-x");
  }

  if (!play_winner) {

    // * ten from the longest suit last opened by one of the partners
    for (int i = 0; i < 4; i++) {
      SuitInfo& suit = suit_info[sorted[i]];
      if (suit.tens && (suit.played_by == TEAM::CONTRA))
        PLAY(Card::TEN, "ten from the longest suit last opened by one of the partners");
    }

    // * lowest card from longest suit without aces or tens which hasn't been opened yet
    for (int i = 0; i < 4; i++) {
      SuitInfo& suit = suit_info[sorted[i]];
      if (suit.len && !suit.aces && !suit.tens && !suit.runs)
        PLAY(suit.lowest, "lowest card from longest unplayed suit without aces or tens");
    }

    // * highest card below ace in a suit with ace-x-x
    for (int i = 0; i < 4; i++) {
      SuitInfo& suit = suit_info[sorted[i]];
      if ((suit.len > 2) && suit.aces)
        SETCARD(suit.highest_below_ace);
    }
    PLAYCARD("highest card below ace in a suit with ace-x-x");

  } // if (!play_winner)


  // * ace from ace-10 if suit hasn't been opened by the solo player
  for (int i = 0; i < 4; i++) {
    SuitInfo& suit = suit_info[sorted[i]];
    if ((suit.len == 2) && suit.aces && suit.tens && (suit.played_by != TEAM::RE))
      PLAY(Card::ACE, "ace from ace-10 if suit hasn't been opened by the solo player");
  }

  // * ace from ace-10
  for (int i = 0; i < 4; i++) {
    SuitInfo& suit = suit_info[sorted[i]];
    if ((suit.len == 2) && suit.aces && suit.tens)
      PLAY(Card::ACE, "ace from ace-10");
  }

  // * ace from an already opened suit
  for (int i = 0; i < 4; i++) {
    SuitInfo& suit = suit_info[sorted[i]];
    if (suit.aces && suit.runs)
      PLAY(Card::ACE, "ace from an already opened suit");
  }

  // * singleton ace
  for (int i = 0; i < 4; i++) {
    SuitInfo& suit = suit_info[sorted[i]];
    if ((suit.len == 1) && suit.aces)
      PLAY(Card::ACE, "singleton ace");
  }

  // * any ace
  for (int i = 0; i < 4; i++) {
    SuitInfo& suit = suit_info[sorted[i]];
    if (suit.aces)
      PLAY(Card::ACE, "any ace");
  }

  // * highest card from the longest suit in which the solo player is void
  for (int i = 0; i < 4; i++) {
    SuitInfo& suit = suit_info[sorted[i]];
    if (suit.len && suit.soloplayer_is_void)
      SETCARD(suit.best);
  }
  PLAYCARD("highest card from the longest suit in which the solo player is void");

  // * highest card from a suit in which a partner has signalled to have a winner
  for (int i = 3; i >= 0; --i) {
    SuitInfo& suit = suit_info[sorted[i]];
    if (suit.save)
      SETCARD(suit.best);
  }
  PLAYCARD("highest card from a suit in which a partner has signalled to have a winner");


  // * lowest card from a suit which has a stopper left after the trick
  for (int i = 0; i < 4; i++) {
    SuitInfo& suit = suit_info[sorted[i]];
    if (suit.stopper_remains)
      PLAY(suit.lowest, "lowest card from a suit which has a stopper left after the trick");
  }

  // * we don't know what to play, but we know, what *not* to play - a suit with a stopper
  for (int i = 0; i < 4; i++) {
    SuitInfo& suit = suit_info[sorted[i]];
    if (suit.stopper) {
      bool locked = hi.hand().lock(sorted[i]);
#ifdef MEATLESS_LEAD_TRICK_CONTRA_DEBUG
      if (!hi.game().isvirtual())
        cout << "*lead contra - locking " << sorted[i] << (locked ? "(done)" : "(failed)") << endl;
#endif
    }
  }

#undef SETCARD
#undef PLAY
#undef PLAYCARD

  return Card::EMPTY;
} // Heuristics::meatless_lead_trick_contra

