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
#include "../../card/card_signals.h"

namespace HEURISTICS_UTILS {


/**
 ** check if own team can still win this trick.
 **
 ** @param      trick   the current trick
 ** @param      hi      heuristic interface
 **
 ** @return     returns true if the own team can win the trick
 **
 ** @author     Borg Enders
 ** @author     Tassilo Kirsch
 **
 ** @version    0.7.6
 **/
bool
ownTeamCanWinTrick( Trick const &t, HeuristicInterface const& hi )
{
  DEBUG_ASSERTION((!t.isempty()),
                  "ownTeamCanWinTrick(...)\n"
                  "  trick is empty");

  bool opponents_behind = false;

   // take a look if a teammate sitting behind can win the trick
  for( unsigned i = t.actcardno() + 1; i < hi.game().playerno(); i++ ) {
    Player const& p = t.player_of_card(i);
    if (is_teammate(hi, p)) {
      // use conservative estimate
      HandCard highest = estimate_highest_card(hi, p, t.starttcolor());
      if (highest.is_empty())
        highest = estimate_highest_card(hi, p);
      if (cardWillWinTheTrick(t, hi, highest))
        return true;
    } else
      opponents_behind = true;
  }

  return    (t.winnerplayer().team() == hi.team())
         && (   !opponents_behind
             || cardWillWinTheTrick(t, hi, t.winnercard())
            );

} // bool ownTeamCanWinTrick ( Trick t, HeuristicInterface hi )


/**
 ** check if opposite team can still win this trick.
 **
 ** @param      trick   the current trick
 ** @param      hi      heuristic interface
 **
 ** @return     returns true if the opposite team can win the trick
 **
 ** @author     Borg Enders
 ** @author     Tassilo Kirsch
 **
 ** @version    0.7.6
 **/
bool
oppositeTeamCanWinTrick( Trick const& t, HeuristicInterface const& hi )
{

  HandCard winnercard = t.winnercard();

  // guess who will win the trick
  for (unsigned i = t.actcardno() + 1; i < hi.game().playerno(); ++i) {
    Player const& p = t.player_of_card(i);
    HandCard highest;
    if (is_teammate(hi, p)) {
      // use conservative estimate
      highest = estimate_highest_card(hi, p, t.starttcolor());
      if (highest.is_empty())
        highest = estimate_highest_card(hi, p);
      if (winnercard.less(highest))
        winnercard = highest;
    } else {
      // use worst case estimate
      highest = hi.handofplayer(p).highest_card(t.starttcolor());
      if (highest.is_empty())
        highest = hi.handofplayer(p).highest_card();
      if (cardWillWinTheTrick(t, hi, highest))
        return true;
    }
    if (winnercard.less(highest))
      winnercard = highest;
  }

  return !is_teammate(hi, winnercard.player());

} // bool oppositeTeamCanWinTrick ( Trick t, HeuristicInterface hi )


/**
 ** Check if a card played by the own team will win the trick
 **
 ** @param      trick   the current trick
 ** @param      hi      heuristic interface
 ** @param      card    the card to check
 **
 ** @return     returns true if the card will win the trick
 **
 ** @author     Florian Sundermann
 ** @author     Tassilo Kirsch
 **
 ** @version    0.7.6
 **/
bool
cardWillWinTheTrick(Trick const &t, HeuristicInterface const& hi, HandCard const& card)
{

  if (   // we need a card to check
         card.is_empty()
      || // the card has to win the trick so far
         (   !t.is_jabbed(card)
          && (card.player() != t.winnerplayer())
         )
         // it has to be a card from a teammate
      || !is_teammate(hi, card.player())
     )
    return false;

  Card::TColor starttcolor = (t.isempty()) ? card.tcolor() : t.starttcolor();
  Player const& player = hi.game().player(hi.no());

  // check if opponents still coming in the trick can win against our card
  for( unsigned i = t.actcardno(); i < hi.game().playerno(); i++ ) {
    Player const& p = t.player_of_card(i);
    if (    (p != player)
         && (p != card.player())
         && !is_teammate(hi, p)
       )
    {
      HandCard highest;
      if ((starttcolor != Card::TRUMP) && hi.cards_information().of_player(p).must_have(starttcolor)) {
        // opponent has to follow suit
        // shortcut: we're ruffing
        if (card.istrump())
          continue;
        highest = hi.handofplayer(p).highest_card(starttcolor);
      } else {
        // opponent can play trump
        highest = hi.handofplayer(p).highest_card();
      }
      if ( t.cardno_of_player(card.player()) < i ) {
        // opponent behind our card: opponent's card has to be higher
        if (card.less(highest))
          return false;
      } else {
        // opponent before our card: our card has to be higher
        if (!highest.less(card))
          return false;
      }
    }
  }
  return true;

} // bool cardWillWinTheTrick()


/**
 ** checks if there are any trumps in trick t
 **
 ** @param     trick   trick to check
 **
 ** @return    whether there is a trump card in the trick
 **
 ** @author    Borg Enders
 ** @author    Diether Knof
 **
 ** @version   0.7.5
 **/
bool
trumpInTrick( Trick const& trick )
{
  for (unsigned i = 0; i < trick.actcardno(); i++)
    if (trick.card(i).istrump())
      return true;

  return false;
} // bool trumpInTrick( Trick trick )


/**
 ** checks if the player of hand has to follow suit (incl. trump) in the given trick
 **
 ** @param     trick    trick to check
 ** @param     hand     player's hand
 **
 ** @return    true if the player has to follow suit
 **
 ** @author    Tassilo Kirsch
 **
 ** @version   0.7.6
 **/
bool
has_to_follow_suit(Trick const& trick, Hand const& hand)
{
  return    !trick.isempty()
         && hand.contains(trick.starttcolor());
} // has_to_follow_suit(...)


/**
 ** -> result
 **
 ** @param     trick   trick to check
 ** @param     hi      heuristic interface
 **
 ** @return    the highest card of the playes of the opposite team behind
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.6
 **
 ** @todo      check for 'must have' of the color of the trick
 **/
HandCard const&
highest_opposite_card_behind(Trick const& trick, HeuristicInterface const& hi)
{
  HandCard const* highest_card = &HandCard::EMPTY;

  // analyse players sitting behind
  for (unsigned i = trick.actcardno() + 1; i < hi.game().playerno(); i++) {
    Player const& p = trick.player_of_card(i);
    Card::TColor const starttcolor = trick.isempty()
                                     ? Card::TRUMP
                                     : trick.starttcolor();
    bool trumptrick = starttcolor == Card::TRUMP;

    if (   (hi.game().type() == GAMETYPE::MARRIAGE)
        && (hi.game().marriage_selector() != MARRIAGE_SELECTOR::TEAM_SET)) {
      if (::is_selector(starttcolor, hi.game().marriage_selector())) {
        if (   (hi.team() != TEAM::RE)
            && (hi.game().soloplayer() == p) ) {
          // in a selector trick, the re player is seen as of the own team
          continue;
        }
      }
    } else { // if (normal game)
      if (hi.team() == hi.teamofplayer(p))
        continue;
    }

    if (   (hi.game().type() == GAMETYPE::POVERTY)
        && (p == hi.game().soloplayer()) ) {
      if (!trumptrick) {
        HandCard const& c = hi.handofplayer(p).highest_card(starttcolor);
        if (highest_card->less(c))
          highest_card = &c;
        continue;
      }
    }
    // check for 'must have' of the color of the trick
    HandCard const& c = (trumptrick || !hi.cards_information().of_player(p).must_have(starttcolor))
                        ? highest_opp_card(hi, hi.handofplayer(p))
                        : hi.handofplayer(p).highest_card(starttcolor);
    if (highest_card->less(c))
      highest_card = &c;
  } // for (i < hi.game().playerno())

  return *highest_card;
} // HandCard const& highest_opposite_card_behind(Trick trick, HeuristicInterface hi)


/**
 ** -> result
 ** if a single charlie is lowest highest trump on hand and if it may score an extrapoint
 ** and if spare_charlie is true, a diamond or heart queen will be returned instead
 **
 ** @param     trick          trick
 ** @param     hi             heuristic interface
 ** @param     spare_charlie  spare a single charlie
 ** @param     to_win         if the card returned has to guarantee winning the trick
 **
 ** @return    the lowest trump which guarantees the trick (Card::EMPTY if there is no such trump) or the lowest of the player's highest trumps
 **
 ** @author    Diether Knof
 ** @author    Tassilo Kirsch
 **
 ** @version   0.7.6
 **
 ** @todo      test
 **/
Card
lowest_highest_trump(Trick const& trick,
                     HeuristicInterface const& hi,
                     bool to_win, // = true
                     bool spare_charlie) // = true

{
  Card lh_trump = hi.hand().highest_trump();
  if (lh_trump == Card::EMPTY)
    return Card::EMPTY;

  spare_charlie = (   spare_charlie
                   && GAMETYPE::is_normal(hi.game().type())
                   && hi.game().rule()(Rule::EXTRAPOINT_CHARLIE)
                  );
  bool has_charlie = false;

  HandCard const& highest_card_behind = to_win
                                        ? highest_opposite_card_behind(trick, hi)
                                        : HandCard::EMPTY;

  vector<Card>::const_reverse_iterator c;
  // first search the highest trump
  for (c = hi.game().trumps().rbegin();
       *c != lh_trump;
       ++c) {
    DEBUG_ASSERTION(c != hi.game().trumps().rend(),
                    "heuristics.cpp::lowest_highest_trump(trick, hi)\n"
                    "  lowest trump " << lh_trump
                    << " not found in the trumps");
  };
  lh_trump = Card::EMPTY;
  // now search for a lesser trump
  for ( ; c != hi.game().trumps().rend(); ++c) {

    if (   !trick.isempty()
        && !trick.winnercard().less(*c))
      break;

    if (   !highest_card_behind.is_empty()
        && c->less(highest_card_behind))
      break;

    // skip fox and ten when searching the highest trump
    if (   (   to_win
            || (c->value() < Card::TEN)
            || (*c == hi.game().dolle())
            || (HandCard(hi.hand(), *c).possible_swine())
           )
        && (hi.hand().contains(*c))
       ) {
      if (spare_charlie
          && (*c == Card::CHARLIE)
          && (hi.hand().numberof(*c) == 1)
         )
        has_charlie = true;
      else
        lh_trump = *c;
    }

    if (   !to_win
        && hi.cards_information().remaining_others(*c))
      break;

  } // for (c \in hi.game().trumps())

  if (   has_charlie
      && (   lh_trump.is_empty()
          || hi.game().less(hi.value(Aiconfig::CHARLIELIMIT_HIGH), lh_trump)
         )
     )
    return Card::CHARLIE;
  else
    return lh_trump;
} // Card lowest_highest_trump(...)


/**
 ** returns the lowest card within the given inclusive range that wins the given trick
 ** if spare_charlie is true, a higher card will be returned instead (not higner than
 ** CHARLIELIMIT_HIGH)
 **
 ** @param     t                the trick we're playing
 ** @param     hi               heuristic interface
 ** @param     spare_charlie    spare a single charlie
 ** @param     min_card         lowest allowed card (Card:EMPTY -> all cards allowed)
 ** @param     max_card         highest allowed card (Card:EMPTY -> all cards allowed)
 **
 ** @return    lowest winner card for the trick
 **
 ** @author    Tassilo Kirsch
 **
 ** @version   0.7.6
 **/
Card
lowest_winner(Trick const& t,
              HeuristicInterface const& hi,
              bool spare_charlie,    // = true
              Card const& min_card,  // = Card::EMPTY
              Card const& max_card   // = Card::EMPTY
             )
{
  // sparing a single charlie only makes sense if the extrapoint is possible
  spare_charlie =    spare_charlie
                  && !GAMETYPE::is_solo(hi.game().type())
                  && hi.game().rule()(Rule::EXTRAPOINT_CHARLIE)
                  && (hi.hand().numberof(Card::CHARLIE) == 1);

  bool has_charlie = false;
  HandCards const& ha = hi.hand().validcards(t);
  Card winner = Card::EMPTY;
  for (unsigned i = 0; i < ha.cardsnumber(); i++)
  {
    HandCard c = ha.card(i);
    if (   t.is_jabbed(c)
        && !c.less(min_card)
        && (max_card.is_empty() || !max_card.less(c))
        && (winner.is_empty() || c.less(winner))
       )
    {
      if (spare_charlie && (c == Card::CHARLIE))
        has_charlie = true;
      else
        winner = c;
    }
  }
  if (   has_charlie
      && (   winner.is_empty()
          || hi.game().less(hi.value(Aiconfig::CHARLIELIMIT_HIGH), winner)
         )
     )
    return Card::CHARLIE;
  if (!winner.is_empty())
    return winner;
  return Card::EMPTY;
} // Card lowest_winner(...)


/**
 ** Check if the card to be played is a plain suit trap: it's a trap if we know that the card
 ** can be ruffed by the other team, but our partner doesn't
 **
 ** @param     t             current trick
 ** @param     hi            heuristic interface
 ** @param     card_to_play  the card we want to play
 **
 ** @return    true if it's a trap
 **
 ** @author    Tassilo Kirsch
 **
 ** @version   0.7.6
 **/
bool
is_plain_suit_trap(Trick const& t, HeuristicInterface const& hi, Card const& card_to_play)
{
  // useless in a solo for the soloplayer, in a meatless solo and in an undetermined marriage
  if (   (hi.game().type() == GAMETYPE::SOLO_MEATLESS)
      || (GAMETYPE::is_solo(hi.game().type()) && (hi.team() == TEAM::RE))
      || hi.game().undetermined_marriage()
     )
    return false;

  // useful only when leading with plain suit
  if (!t.isempty() || card_to_play.istrump(hi.game()))
    return false;

  // the suit to check
  Card::Color const color = card_to_play.color();

  // it's no trap if the suit has already been ruffed by the other team
  // or if it's known to all that there aren't enough suit cards left for a full trick
  if (   hi.jabbedbyotherteam(color)
      || (hi.cards_information().remaining(color) < hi.game().playerno())
     )
    return false;


  if (GAMETYPE::is_solo(hi.game().type())) {

    // it's no trap if both partners sit behind the soloplayer or the soloplayer can't ruff
    if (   (t.cardno_of_player(hi.game().soloplayer()) == 1)
        || hi.cards_information().of_player(hi.game().soloplayer()).does_not_have(Card::TRUMP)
        || hi.cards_information().of_player(hi.game().soloplayer()).must_have(color)
       )
      return false;

    // it's no trap if a partner sits in backhand and can ruff
    if (   (is_teammate(hi, t.lastplayer()))
        && hi.cards_information().of_player(t.lastplayer()).does_not_have(color)
        && hi.cards_information().of_player(t.lastplayer()).must_have(Card::TRUMP)
       )
      return false;

    // it's a trap if we know that the soloplayer will ruff
    if (   hi.cards_information().of_player(hi.game().soloplayer()).does_not_have(color)
        && hi.cards_information().of_player(hi.game().soloplayer()).must_have(Card::TRUMP)
       )
      return true;

    unsigned void_players = 0;
    for (unsigned i = 1; i < hi.game().playerno(); ++i)
      void_players += hi.cards_information().of_player(t.player_of_card(i)).does_not_have(color);

    // it's a trap if only we know that there aren't enough suit cards left for a full trick
    return hi.cards_information().remaining_others(color) < hi.game().playerno() - 1 - void_players;

  } else {

    // it's no trap if a partner sits in backhand
    if (is_teammate(hi, t.lastplayer()))
      return false;

    Player const* partner = hi.team_information().guessed_partner();
    bool partner_is_void = false;

    if (partner) {

      bool cannot_ruff = true;
      bool will_ruff = false;

      for (unsigned i = t.cardno_of_player(*partner) + 1; i < hi.game().playerno(); i++) {
        CardsInformation::OfPlayer const& playerinfo = hi.cards_information().of_player(t.player_of_card(i));
        cannot_ruff = cannot_ruff
                      && (playerinfo.does_not_have(Card::TRUMP)
                         || playerinfo.must_have(color));
        will_ruff = will_ruff || playerinfo.does_not_have(color);
      }

      // it's no trap if the players behind our partner cannot ruff
      if (cannot_ruff)
        return false;

      // it's a trap if we know that the card is going to be ruffed but our partner might not
      if (will_ruff)
        return true;

      if (hi.cards_information().of_player(*partner).does_not_have(color))
        partner_is_void = true;
    }

    // it's a trap if only we know that there aren't enough suit cards left for a full trick
    return hi.cards_information().remaining_others(color) < hi.game().playerno() - 1 - partner_is_void;
  }

} // bool is_plain_suit_trap(...)



/**
 ** checks if a suit will be ruffed by a player of the opposite team who
 ** sits both behind the player and the last partner who can also ruff.
 **
 ** @param     hi      heuristic interface
 ** @param     t       trick to check
 ** @param     color   suit to check (only used if t is empty)
 **
 ** @return    true if the suit will be ruffed
 **
 ** @author    Tassilo Kirsch
 **
 ** @version   0.7.6
 **/
bool
suit_will_be_ruffed_by_other_team(Trick const& t, HeuristicInterface const& hi, Card::Color color) // = Card::NOCARDCOLOR
{
  if (!t.isempty())
    color = t.starttcolor();

  if ((color == Card::TRUMP) || (color == Card::NOCARDCOLOR))
    return false;

  for (unsigned p = hi.game().playerno()-1; p > t.actcardno(); --p) {
    Hand const& h = hi.handofplayer(t.player_of_card(p));
    if (h.hastrump() && !h.existstcolor(color))
      return !is_teammate(hi, t.player_of_card(p));
  }

  return false;
} // bool suit_will_be_ruffed_by_other_team


/**
 ** returns the highest opposite card behind which takes the trick and may lead to a doppelkopf
 ** will return Card::EMPTY if...
 ** ... nu such card exists
 ** ... a doppelkopf doesn't score an extrapoint
 ** ... there's already a card with a value below ten in the trick
 ** ... a partner sitting behind can prevent the doppelkopf
 **
 **
 ** @param     t       trick
 ** @param     hi      heuristic interface
 **
 ** @return    highest opposite card which may lead to a doppelkopf
 **
 ** @author    Tassilo Kirsch
 **
 ** @version   0.7.6
 **/
Card
doppelkopf_alert_card(Trick const& t, HeuristicInterface const& hi)
{
  if (   !GAMETYPE::is_normal(hi.game().type())
      || (t.points() < 10 * t.actcardno())
     )
    return Card::EMPTY;

  // detection of the highest opposite card which may lead to a doppelkopf
  Card highest_doppelkopf_card = Card::EMPTY;

  Card const& swine = hi.game().swine();
  Card const& dolle = hi.game().dolle();
  vector<Card> doko_cards;
  if (!swine.is_empty())
    doko_cards.push_back(swine);
  if (!dolle.is_empty())
    doko_cards.push_back(dolle);
  if (swine.is_empty())
    doko_cards.push_back(Card::FOX);
  doko_cards.push_back(Card(Card::DIAMOND, Card::TEN));

  // analyse the players sitting behind
  for (unsigned i = t.actcardno() + 1; i < hi.game().playerno(); ++i) {
    Player const& p = t.player_of_card(i);
    HandCards const& hp = hi.handofplayer(p).validcards(t);
    if (is_teammate(hi, p)) {
      if (hp.lowest_value() < Card::TEN)
        return Card::EMPTY;
    } else {
      // find the highest opposite card which may lead to a doppelkopf
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
  } // for (unsigned i = t.actcardno() + 1; i < hi.game().playerno(); ++i)

  if (   !is_teammate(hi, t.winnerplayer())
      || t.is_jabbed(highest_doppelkopf_card)
     )
    return highest_doppelkopf_card;

  return Card::EMPTY;
} // doppelkopf_alert_card


/**
 ** estimates the points for a trick in a worst case scenario
 **
 ** @param     t             the trick to be estimated
 ** @param     hi            heuristic interface
 ** @param     card_to_play  the card to be played by the current player
 ** @param     play_to_win   true: estimate worst case (minimum) points if the trick can be won
 ** @param     play_to_win   false: estimate worst case (maximum) points if the trick will be lost
 **
 ** @return    estimated points of the trick (0 if play_to_win cannot be fulfilled)
 **
 ** @author    Tassilo Kirsch
 **
 ** @version   alpha
 **/
unsigned
meatless_estimated_trick_points(Trick const& t, HeuristicInterface const& hi, HandCard const& card_to_play, bool const play_to_win)
{
  // shortcut #1: trick is already full
  if (t.isfull()) {
    // ignore card_to_play
    if (play_to_win != (hi.teamofplayer(t.winnerplayer()) == hi.team()))
      return 0; // invalid: wrong winning team
    else
      return t.points();
  }

  unsigned points = t.points() + card_to_play.points();

  // shortcut #2: we're going to play the last card of the trick
  if (t.islastcard()) {
    if (play_to_win != (t.is_jabbed(card_to_play) || (hi.teamofplayer(t.winnerplayer()) == hi.team())))
      return 0; // invalid: wrong winning team
    else
      return points;
  }


  Card::Color startcolor = (t.isempty()) ? card_to_play.color() : t.startcolor();
  HandCard winnercard = (t.isempty() || t.winnercard().less(card_to_play)) ? card_to_play : t.winnercard();

  CardCounter played;

  if (play_to_win) {

    // worst case trick points for a team if it wins the trick

    // check if the card to play (with minimum support from the own team) will win the trick
    HandCard mincard = card_to_play;
    if (hi.team() == TEAM::CONTRA) {
      if (winnercard.player() != hi.game().soloplayer())
        mincard = winnercard;
      for (unsigned i = t.actcardno() + 1; i < hi.game().playerno(); i++) {
        Player const& p = t.player_of_card(i);
        if ((p != hi.game().soloplayer()) && hi.cards_information().of_player(p).must_have(startcolor)) {
          HandCard found = hi.handofplayer(p).lowest_card(startcolor);
          if (mincard.less(found))
            mincard = found;
        }
      }
    }
    if (!cardWillWinTheTrick(t, hi, mincard))
      return 0; // invalid: team can't win the trick

    for (unsigned i = t.actcardno() + 1; i < hi.game().playerno(); i++) {
      Player const& p = t.player_of_card(i);
      Hand const& ha = hi.handofplayer(p);
      HandCard found = HandCard::EMPTY;
      for( unsigned c = 0; c < ha.cardsnumber(); c++ ) {
        HandCard card = ha.card(c);
        // is the card still available?
        if (played[card] + hi.hand().numberof(card) < hi.cards_information().remaining(card)) {
          // is it a valid card for the trick?
          if ( (card.color() == startcolor)
               || !hi.cards_information().of_player(p).must_have(startcolor) )
            // is it the lowest card found?
            if (found.is_empty() || (card.value() < found.value()))
              found = card;
        } // for( unsigned c = 0; c < ha.cardsnumber(); c++ )
      }
      played.inc(found);
      points += found.points();
    }
  } // if (play_to_win)
  else {
    if (hi.team() == TEAM::RE) {

      // worst case trick points if soloplayer loses the trick
      unsigned wonby = 0;

      if (winnercard.player() == hi.game().soloplayer()) {
        // find the opponent who takes the trick
        Card::Value highest_v = Card::NOCARDVALUE;
        for (unsigned i = t.actcardno() + 1; i < hi.game().playerno(); i++) {
          Player const& p = t.player_of_card(i);
          HandCard found = hi.handofplayer(p).highest_card(startcolor);
          if (found.is_empty())
            continue;
          if (winnercard.less(found)) {
            if (hi.cards_information().of_player(p).must_have(startcolor)) {
              wonby = i;
              break;
            } else {
              Card::Value v = hi.handofplayer(p).highest_value();
              if ((highest_v == Card::NOCARDVALUE) || (v > highest_v)) {
                highest_v = v;
                wonby = i;
              }
            }
          }
        }
        if (!wonby)
          return 0; // invalid: soloplayer can't lose the trick
      }

      for (unsigned i = t.actcardno() + 1; i < hi.game().playerno(); i++) {
        Player const& p = t.player_of_card(i);
        Hand const& ha = hi.handofplayer(p);
        HandCard found = HandCard::EMPTY;
        for( unsigned c = 0; c < ha.cardsnumber(); c++ ) {
          HandCard card = ha.card(c);
          // is the card still available?
          if (played[card] + hi.hand().numberof(card) < hi.cards_information().remaining(card)) {
            // is it a valid card for the trick?
            if ( (card.color() == startcolor)
                 || !(hi.cards_information().of_player(p).must_have(startcolor) || (wonby == i)) )
              // is it the highest card found?
              if (found.is_empty() || (found.value() < card.value()))
                found = card;
          } // for( unsigned c = 0; c < ha.cardsnumber(); c++ )
          played.inc(found);
          points += found.points();
        }
      }
    } // if (hi.team() == TEAM::RE)
    else {

      // worst case trick points if contra team loses the trick
      if (!t.has_played(hi.game().soloplayer())) {
        HandCard found = hi.handofplayer(hi.game().soloplayer()).highest_card(startcolor);
        if (!winnercard.less(found))
          return 0; // invalid: soloplayer can't win the trick
        played.inc(found);
        points += found.points();
        winnercard = found;
      } else if (winnercard.player() != hi.game().soloplayer()) {
        return 0; // invalid: soloplayer won't win the trick
      }
      for (unsigned i = t.actcardno() + 1; i < hi.game().playerno(); i++) {
        Player const& p = t.player_of_card(i);
        if (hi.game().soloplayer() != p) {
          HandCards const& ha = hi.handofplayer(p).validcards(t);
          HandCard found = HandCard::EMPTY;
          for( unsigned c = 0; c < ha.cardsnumber(); c++ ) {
            HandCard card = ha.card(c);
            // is the card still available?
            if (played[card] + hi.hand().numberof(card) < hi.cards_information().remaining(card))
              // is it a valid card for the trick?
              if ((card.color() == startcolor) || !hi.cards_information().of_player(p).must_have(startcolor))
                // does the card not win the trick?
                if ( (card.color() != startcolor)
                     || card.less(winnercard)
                     || ((i > t.cardno_of_player(hi.game().soloplayer())) && !winnercard.less(card))
                   )
                  // is it the highest card found?
                  if (found.is_empty() || found.less(card))
                    found = card;
          } // for( unsigned c = 0; c < ha.cardsnumber(); c++ )
          if (found.is_empty())
            return 0; // invalid: soloplayer won't win the trick
          played.inc(found);
          points += found.value();
        }
      } // for (unsigned i = t.actcardno() + 1; i < hi.game().playerno(); i++)


    } // if (hi.team() == TEAM::CONTRA)
  } // if (!play_to_win)

  return points;

} // unsigned meatless_estimated_trick_points()

} // namespace HEURISTICS_UTILS

