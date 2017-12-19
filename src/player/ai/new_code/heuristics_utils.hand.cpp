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
 ** estimates the minimum number of copies of a card a player has,
 ** using information from card counting and card weighting
 **
 ** @param      hi        heuristic interface
 ** @param      player    the player to check
 ** @param      card      the card to check
 **
 ** @return     the number of 'card' cards of the player
 **
 ** @author     Tassilo Kirsch
 **
 ** @version    0.7.6
 **/
unsigned
estimate_numberof(HeuristicInterface const& hi, Player const& player, Card const& card)
{
  using namespace CARD_SIGNALS;

  // cards of the analysing player
  unsigned own_cards = hi.hand().numberof(card);
  if (player.no() == hi.no())
    return own_cards;

  if (hi.value(Aiconfig::HANDS_KNOWN))
    return player.hand().numberof(card);

  int remaining = hi.cards_information().remaining(card) - own_cards;
  if (!remaining || !hi.handofplayer(player).contains(card))
    return 0;

  // must have cards of the player to check
  int player_musthave = hi.cards_information().of_player(player).must_have(card);
  remaining -= player_musthave;
  if (!remaining)
    return player_musthave;

  // the weightings for the card
  vector<int> w;

  // player's weighting
  w.push_back(hi.cards_information().of_player(player).weighting(card));

  // if the player to check or the analysing player have a must have card,
  // the weightings need adjustment later on
  int musthave_player = own_cards ? 4 : (player_musthave ? 0 : -1);

  // get the other players' weightings
  for (unsigned p = 0; p < hi.game().playerno(); ++p)
    if ((p != hi.no()) && (p != player.no())) {

      int musthave = hi.cards_information().of_player(p).must_have(card);

      if (musthave) {
        remaining -= musthave;
        if (!remaining)
          // all cards are known
          return player_musthave;
        if (hi.cards_information().of_player(p).unknown(card))
          musthave_player = w.size();
        else
          musthave_player = hi.game().playerno();
      }

      if (hi.cards_information().of_player(p).unknown(card))
        // the other player has an unknown card, take its weighting into account
        w.push_back(hi.cards_information().of_player(p).weighting(card));
    }

  // process the weightings of all players who are still in the mix
  // to calculate the effective weighting for the player's card(s)
  vector<pair<int,int> > effective_w = process_weightings(w, remaining, musthave_player, false);

  // in a normal game, a player will most likely not have two club queens
  if (   (card == Card::CLUB_QUEEN)
      && (hi.game().type() == GAMETYPE::NORMAL)
      && (remaining == 2 - player_musthave)
     )
    if (remaining == 1)
      effective_w[0].first = MIN_WEIGHTING;
    else
      effective_w[0].second = MIN_WEIGHTING;

  // estimated number of cards
  int n = player_musthave;

  if (effective_w[0].first == MAX_WEIGHTING)
    ++n;
  else if ((effective_w[0].first > MIN_WEIGHTING) && (effective_w[0].first >= SINGLE_CARD_WEIGHT)) {
    bool has_card = true;
    for (unsigned i = 1; i < w.size(); ++i)
      if (effective_w[0].first < effective_w[i].first + (3-remaining) * SINGLE_CARD_WEIGHT) {
        has_card = false;
        break;
      }
    n += has_card;
  }

  if (remaining > 1)
    if (effective_w[0].second == MAX_WEIGHTING)
      ++n;
    else if ((effective_w[0].second > MIN_WEIGHTING)  && (effective_w[0].second >= SINGLE_CARD_WEIGHT)) {
      bool has_card = true;
      for (unsigned i = 1; i < w.size(); ++i)
        if (effective_w[0].second < effective_w[i].second + SINGLE_CARD_WEIGHT) {
          has_card = false;
          break;
        }
      n += has_card;
    }

  return n;

} // unsigned estimate_numberof(...)


/**
 ** conservative estimate of the player's highest card,
 ** using information from card counting and card weighting
 **
 ** @param      hi        heuristic interface
 ** @param      player    the player to check
 ** @param      tcolor    the suit to check
 **
 ** @return     the player's highest card of suit tcolor
 **
 ** @author     Tassilo Kirsch
 **
 ** @version    0.7.6
 **/
HandCard const&
estimate_highest_card(HeuristicInterface const& hi, Player const& player)
{
  // shortcuts
  if ((player.no() == hi.no()) || hi.value(Aiconfig::HANDS_KNOWN))
    return player.hand().highest_card();

  // now find the highest card using estimate_numberof
  HandCard const* highest_card = &HandCard::EMPTY;
  HandCard const* lowest_card = &HandCard::EMPTY;
  for (HandCards::const_iterator c = hi.handofplayer(player).cards().begin();
       c != hi.handofplayer(player).cards().end();
       ++c)
    if (   highest_card->less(*c)
        && estimate_numberof(hi, player, *c)
       )
      highest_card = &(*c);
    else if (lowest_card->is_empty() || c->less(*lowest_card))
      lowest_card = &(*c);

  if (!highest_card->is_empty())
    return *highest_card;
  else
    return *lowest_card;
} // HandCard const& estimate_highest_card(HeuristicInterface const& hi, Player const& player, Card::TColor const tcolor)


/**
 ** conservative estimate of the player's highest card of suit tcolor,
 ** using information from card counting and card weighting
 **
 ** @param      hi        heuristic interface
 ** @param      player    the player to check
 ** @param      tcolor    the suit to check
 **
 ** @return     the player's highest card of suit tcolor
 **
 ** @author     Tassilo Kirsch
 **
 ** @version    0.7.6
 **/
HandCard const&
estimate_highest_card(HeuristicInterface const& hi, Player const& player, Card::TColor const tcolor)
{
  // shortcuts
  if ((player.no() == hi.no()) || hi.value(Aiconfig::HANDS_KNOWN))
    return player.hand().highest_card(tcolor);

  // we check does_not_have first, because it's faster than numberof
  if (   hi.cards_information().of_player(player).does_not_have(tcolor)
      || !hi.handofplayer(player).numberof(tcolor)
     )
    return HandCard::EMPTY;

  // now find the highest card using estimate_numberof
  HandCard const* highest_card = &hi.handofplayer(player).lowest_card(tcolor);
  for (HandCards::const_iterator c = hi.handofplayer(player).cards().begin();
       c != hi.handofplayer(player).cards().end();
       ++c)
    if (   (c->tcolor() == tcolor)
        && highest_card->less(*c)
        && estimate_numberof(hi, player, *c)
       )
      highest_card = &(*c);

  return *highest_card;
} // HandCard const& estimate_highest_card(HeuristicInterface const& hi, Player const& player, Card::TColor const tcolor)


/**
 ** checks if a player has a higher card than 'card' using information from card counting and card weighting
 **
 ** @param      hi        heuristic interface
 ** @param      player    the player to check
 ** @param      card      the card to check
 **
 ** @return     true if there's a high probability that the player has a higher card
 **
 ** @author     Tassilo Kirsch
 **
 ** @version    0.7.6
 **/
bool
player_has_higher_card(HeuristicInterface const& hi, Player const& player, Card const& card)
{
  // shortcuts
  if ((player.no() == hi.no()) || hi.value(Aiconfig::HANDS_KNOWN))
    return player.hand().higher_card_exists(card);

  if (!hi.handofplayer(player).higher_card_exists(card))
    return false;

  // check higher cards
  vector<Card> const& highercards = hi.game().higher_cards(card);
  for (vector<Card>::const_iterator c = highercards.begin();
       c != highercards.end();
       ++c)
    if (estimate_numberof(hi, player, *c))
      return true;

  return false;
} // bool player_has_higher_card(...)


/**
 ** Returns the highest card of an opponent's hand. This is identical to
 ** hand.highest_card() except for a re player in a normal game who doesn't
 ** know his partner but should ignore club queens in opponents' hands.
 **
 **
 ** @param     hi      Heuristic interface
 ** @param     hand    opponent's hand to check
 **
 ** @return the highest card of hand
 **
 ** @author Tassilo Kirsch
 **
 ** @version  0.7.6
 **/
HandCard const&
highest_opp_card(HeuristicInterface const& hi, Hand const& hand)
{
  if (   (hi.team() != TEAM::RE)
      || GAMETYPE::is_solo(hi.game().type())
      || (hi.game().marriage_selector() == MARRIAGE_SELECTOR::SILENT)
      || ((hi.game().type() != GAMETYPE::NORMAL) && (hi.game().type() != GAMETYPE::MARRIAGE))
     )
    return hand.highest_card();

  HandCard const* highest = &HandCard::EMPTY;

  for (HandCards::const_iterator c = hand.cards().begin();
       c != hand.cards().end();
       ++c)
  {
    if ((*c != Card::CLUB_QUEEN) && highest->less(*c))
      highest = &(*c);
  }

  return *highest;
} // HandCard const& highest_opp_card(...)



/**
 ** Finds highest jack.
 ** The jack has to be higher or equal to the given min_jack. If no such jack can be found,
 ** the lowest queen lower or equal to the given max_queen will be returned. No queen will
 ** be returned if max_queen is NOCARDCOLOR. If a single charlie is the only jack on hand
 ** and if it may score an extrapoint and if spare_charlie is true, the charlie will be
 ** skipped, i.e. a valid lower jack or queen will be returned instead. If there are no
 ** valid replacements, the single charlie will be returned.
 **
 ** @param     h              the hand to search for a card
 ** @param     min_jack       the suit of the lowest valid jack
 ** @param     max_queen      the suit of the highest valid queen or NOCARDCOLOR if no queens are allowed
 ** @param     spare_charlie  spares a single charlie
 **
 ** @return    highest jack on hand or Card::EMPTY if no card was found
 **
 ** @author    Tassilo Kirsch
 **
 ** @version   0.7.6
 **/
Card
highest_jack(Hand const& h, Card::Color min_jack, Card::Color max_queen, bool spare_charlie) // = true
{
  // the lowest jack allowed
  Card min_card = Card((min_jack == Card::NOCARDCOLOR ? Card::DIAMOND : min_jack), Card::JACK);
  // the highest queen allowed
  Card max_card = max_queen == Card::NOCARDCOLOR ? Card::CHARLIE : Card(max_queen, Card::QUEEN);

  // the highest jack found so far
  Card highest = Card::EMPTY;
  // the lowest queen found so far
  Card lowest = Card::EMPTY;

  // sparing a single charlie only makes sense if the extrapoint is possible
  spare_charlie =    spare_charlie
                  && GAMETYPE::is_normal(h.game().type())
                  && h.game().rule()(Rule::EXTRAPOINT_CHARLIE);
  bool has_charlie = false;

  for (unsigned i = 0; i < h.cardsnumber(); ++i) {
    HandCard const& card = h.card(i);
    if (card.less(min_card) || max_card.less(card))
      continue;

    if (   (card.value() == Card::JACK)
        && (highest.is_empty() || highest.less(card))
       )
    {
      if (spare_charlie && (card.color() == Card::CLUB)) {
        // another charlie is fair game
        spare_charlie = false;
        has_charlie = true;
      } else
        highest = card;
    }
    if (   (card.value() == Card::QUEEN)
        && (lowest.is_empty() || card.less(lowest))
       )
      lowest = card;
  } // for (unsigned i = 0; i < h.cardsnumber(); ++i)

  if (!highest.is_empty())
    return highest;
  if (!lowest.is_empty())
    return lowest;
  if (has_charlie)
    return Card::CHARLIE;
  return Card::EMPTY;

} // Card highest_jack(...)


/**
 ** finds lowest card with value cv
 **
 ** @param     h       the hand to search for a card
 ** @param     cv      the value to search for
 **
 ** @return    lowest card with the value cv (or 'Card()' if no card was found)
 **
 ** @author    Borg Enders
 **
 ** @version   Beta
 **/
Card
lowest_card( Hand const& h, Card::Value const cv )
{
  Card c;

  unsigned i;

  for( i = 0; i < h.cardsnumber(); i++ )
  {
    // find any card
    if( h.card(i).value() == cv )
    {
      c = h.card(i);
      break;
    } // if ( h.card(i).value() == cv )
  } // for ( i < h.cardsnumber() )

  if (c.is_empty())
    return c;

  for( i = 0; i < h.cardsnumber(); i++ )
  {
    // find lowest card
    if(   h.card(i).value() == cv
       && h.card(i).less(c) )
      c = h.card(i);
  } // for ( i < h.cardsnumber() )

  return c;
} // Card lowest_card( Hand h, Card::Value cv )


/**
 ** finds highest card with value cv
 **
 ** @param     h       the hand to search for a card
 ** @param     cv      the value to search for
 **
 ** @return    highest card with the value cv (or 'Card()' if no card was found)
 **
 ** @author    Borg Enders
 **
 ** @version   Beta
 **/
Card
highest_card( Hand const& h, Card::Value const cv )
{
  Card c;

  unsigned i;

  for( i = 0; i < h.cardsnumber(); i++ )
  {
    // find any card
    if( h.card(i).value() == cv )
    {
      c = h.card(i);
      break;
    } // if ( h.card(i).value() == cv )
  } // for ( i < h.cardsnumber() )

  if (c.is_empty())
    return Card::EMPTY;

  for( i = 0; i < h.cardsnumber(); i++ )
  {
    // find highest card
    if(   h.card(i).value() == cv
       && !h.card(i).less(c) )
      c=h.card(i);
  } // for ( i < h.cardsnumber() )

  return c;
} // Card highest_card( Hand h, Card::Value cv )


/**
 ** finds highest queen
 **
 ** @param     h       the hand to search for a card
 **
 ** @return    highest queen on hand (or 'Card()' if no card was found)
 **
 ** @author    Borg Enders
 **
 ** @version   Beta
 **/
Card
highest_queen( Hand const& h )
{
  return highest_card( h, Card::QUEEN );
} // Card highest_queen( Hand h )


/**
 ** finds lowest queen
 **
 ** @param     h       the hand to search for a card
 **
 ** @return    lowest queen on hand (or 'Card()' if no card was found)
 **
 ** @author    Borg Enders
 **
 ** @version   Beta
 **/
Card
lowest_queen( Hand const& h )
{
  return lowest_card( h, Card::QUEEN );
} // Card lowest_queen ( Hand h )

/**
 ** finds lowest jack
 ** if a single charlie is the only jack on hand and if it may score an extrapoint and if
 ** spare_charlie is true, a diamond or heart queen will be returned instead
 **
 ** @param     h              the hand to search for a card
 ** @param     spare_charlie  spares a single charlie
 **
 ** @return    lowest jack on hand or Card::EMPTY if no card was found
 **
 ** @author    Borg Enders
 ** @author    Tassilo Kirsch
 **
 ** @version   0.7.6
 **/
Card
lowest_jack(HeuristicInterface const& hi, bool spare_charlie) // = true
{
  Card c = lowest_card(hi.hand(), Card::JACK);
  // sparing a single charlie only makes sense if the extrapoint is possible
  if (   (c == Card::CHARLIE)
      && spare_charlie
      && GAMETYPE::is_normal(hi.game().type())
      && hi.game().rule()(Rule::EXTRAPOINT_CHARLIE)
      && (hi.hand().numberof(Card::CHARLIE) < 2)
     )
  {
    vector<Card> const& highercards = hi.game().higher_cards(Card::CHARLIE);
    for (vector<Card>::const_iterator c = highercards.begin();
         c != highercards.end();
         ++c)
    {
      if (hi.game().less(hi.value(Aiconfig::CHARLIELIMIT_HIGH), *c))
        break;
      if (hi.hand().contains(*c))
        return *c;
    }
  }
  return c;
} // Card lowest_jack ( Hand h, HeuristicInterface hi )


} // namespace HEURISTICS_UTILS

