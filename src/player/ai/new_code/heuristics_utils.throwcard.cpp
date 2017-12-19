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
 ** -> result
 **
 ** @param     trick            current trick
 ** @param     hi               heuristic interface
 ** @param     forceRemaining   only return cards which may create ruff chances
 ** @param     forceSingleton   only return singletons
 ** @param     forceFirstRun    only return singletons from first run suits
 **
 ** @return    smartest plain suit card to throw
 **
 ** @author    Borg Enders
 ** @author    Diether Knof
 ** @author    Tassilo Kirsch
 **
 ** @version   0.7.6
 **/
Card
choose_best_fehl( Trick const& trick,
                  HeuristicInterface const& hi,
                  bool const forceRemaining, // = true
                  bool const forceSingleton, // = true
                  bool const forceFirstRun   // = false
                )
{

  // priorities:
  // #1 singleton below ten in a first run suit
  // #2 singleton ten in a first run suit
  // #3 (!forceFirstRun) singleton below ten in a suit in which the partner isn't void
  // #4 (!forceFirstRun && !forceSingleton) lower card from a doubleton with king as highest card in a suit in which the partner isn't void
  // #5 (!forceFirstRun && !forceSingleton) lowest card from any suit longer than 2 in which the partner isn't void
  // #6 (!forceFirstRun) last remaining non-trump if it's a nine or king
  // special case no trump on hand: lowest card below ten from the longest color

  // only useful if we don't have to follow suit
  if (has_to_follow_suit(trick, hi.hand()))
    return Card::EMPTY;

  Game const& game = hi.game();

  Player const* partner = hi.team_information().guessed_partner();
  vector<bool> hastrump(game.playerno(), false);
  bool other_players_out_of_trump = false;

  if (!game.undetermined_marriage()) {
    other_players_out_of_trump = true;
    for (unsigned p = 0; p < game.playerno(); ++p) {
      if (p != hi.no()) {
        hastrump[p] = hi.handofplayer(p).hastrump();
        if (hastrump[p])
          other_players_out_of_trump = false;
      }
    }
  }
  hastrump[hi.no()] = hi.hand().hastrump();

  // there should be at least as many cards remaining as there are other players
  unsigned min_remaining = (forceRemaining && hastrump[hi.no()])
                           ? game.playerno() - 1
                           : 0;

  // the infos we need for each suit
  struct SuitInfo {
    unsigned remaining;
    unsigned len;
    Card::Value lowest; // exists if len > 1
    Card::Value highest; // exists if len > 1
  };
  SuitInfo suit_info[] = { {0}, {0}, {0}, {0} };

  // number of plain suit cards
  unsigned fehl = 0;
  // last plain suit card on hand
  Card last_fehl;

  Card::TColor starttcolor = trick.starttcolor();
  bool has_to_follow_suit = !trick.isempty() && hi.hand().existstcolor(starttcolor);

  // iterate hand's cards only once to calc all neccessary values
  for( unsigned i = 0; i < hi.hand().cardsnumber(); i++ )
  {
    HandCard c = hi.hand().card(i);
    if (   c.istrump()
        || (   has_to_follow_suit
            && (c.color() == starttcolor)
           )
       )
      continue;
    SuitInfo& suit = suit_info[c.color()];
    if (c.value() > suit.highest)
      suit.highest = c.value();
    if (!suit.len || (c.value() < suit.lowest))
      suit.lowest = c.value();
    suit.len++;
    last_fehl = c;
    fehl++;
  }

  // no plain suit card found
  if (!fehl)
    return Card::EMPTY;

  // sort the suits by
  // remaining others (descending) (only if we have trump), lowest card (ascending), length (ascending)
  Card::Color sorted[4];
  int suits = 0;
  for (int c = Card::DIAMOND; c <= Card::CLUB; c++) {
    Card::Color color = Card::InttoColor(c);
    SuitInfo& suit = suit_info[c];
    if (!suit.len) // this will also skip trump
      continue;

    bool partner_is_void = false;
    if (!other_players_out_of_trump) {
      if (!GAMETYPE::is_solo(game.type())) {
        // if our partner is out of a suit, throwing it becomes a lower priority, unless
        // an opponent not sitting directly in front of our partner can also ruff
        partner_is_void = partner && hi.cards_information().of_player(*partner).does_not_have(color);
        if (partner_is_void) {
          for (unsigned p = 0; p < game.playerno(); ++p) {
            if ((p == hi.no()) || (p == partner->no()) || (p + 1 == partner->no()) || !hastrump[p])
              continue;
            if (hi.cards_information().of_player(p).does_not_have(color)) {
              partner_is_void = false;
              break;
            }
          }
        }
      } else if (hi.team() != TEAM::RE) {
        // we're a contra player in a solo, a suit gets a lower priority if one of our partners is
        // void while the soloplayer may still have the suit or the partner sitting right behind
        // the soloplayer is out of the suit
        unsigned soloplayer = game.soloplayer().no();
        for (unsigned i = 1; i <= game.playerno(); ++i) {
          unsigned p = (soloplayer + i) % game.playerno();
          if ((p != hi.no()) && hastrump[p]) {
            if (hi.cards_information().of_player(p).does_not_have(color)) {
              partner_is_void = (p != soloplayer);
              if (p = soloplayer + 1)
                break;
            }
          }
        }
      }
    } // if (!other_players_out_of_trump)

    if (partner_is_void)
      continue;
    if (hastrump[hi.no()]) {
      suit.remaining = hi.cards_information().remaining_others(color);
      if (suit.remaining < min_remaining)
        continue;
    } else {
      // no trump on hand: best fehl is below ten and throwing it doesn't leave a single ten or ace
      if ((suit.lowest >= Card::TEN) || ((suit.len == 2) && (suit.highest >= Card::TEN)))
        continue;
    }
    int i = 0;
    while ( (i < suits) &&
            ((suit.remaining < suit_info[sorted[i]].remaining)
            || ((suit.remaining == suit_info[sorted[i]].remaining)
               && ((suit.lowest > suit_info[sorted[i]].lowest)
                  || ((suit.lowest == suit_info[sorted[i]].lowest)
                     && (suit.len > suit_info[sorted[i]].len)))))
          ) // sorry, that's a monster
      i++;
    for (int j = suits; j > i; j--) {
      sorted[j] = sorted [j-1];
    }
    sorted[i] = color;
    suits++;
  }

  // we have all the info needed, now choose the best card

  // no valid card found
  if (!suits)
    return Card::EMPTY;

  // special case no trump on hand: lowest card below ten from the longest color
  if (!hastrump[hi.no()])
    return Card(sorted[suits-1], suit_info[sorted[suits-1]].lowest);

  // #1 singleton below ten in a first run suit
  for (int i = 0; i < suits; i++) {
    SuitInfo& suit = suit_info[sorted[i]];
    if ( (suit.len == 1)
         && (hi.color_runs(sorted[i]) == 0)
         && (suit.lowest < Card::TEN)
       )
      return Card(sorted[i], suit.lowest);
  }

  // #2 singleton ten in a first run suit
  // the chance to ruff a first run is worth throwing a ten
  for (int i = 0; i < suits; i++) {
    SuitInfo& suit = suit_info[sorted[i]];
    if ( (suit.len == 1)
         && (hi.color_runs(sorted[i]) == 0)
         && (suit.lowest < Card::ACE)
       )
      return Card(sorted[i], suit.lowest);
  }

  if (forceFirstRun)
    return Card::EMPTY;

  // #3 singleton below ten in a suit in which the partner isn't void
  for (int i = 0; i < suits; i++) {
    SuitInfo& suit = suit_info[sorted[i]];
    if ( (suit.len == 1)
         && (suit.lowest < Card::TEN)
       )
      return Card(sorted[i], suit.lowest);
  }

  if (!forceSingleton) {

    // #4 lower card from a doubleton with king as highest card in a suit in which the partner isn't void
    for (int i = 0; i < suits; i++) {
      SuitInfo& suit = suit_info[sorted[i]];
      if ( (suit.len == 2)
           && (suit.highest < Card::TEN)
         )
        return Card(sorted[i], suit.lowest);
    }

    // #5 lowest card from any suit longer than 2 in which the partner isn't void
    for (int i = 0; i < suits; i++) {
      SuitInfo& suit = suit_info[sorted[i]];
      if ( (suit.len > 2)
           && (suit.lowest < Card::TEN)
         )
        return Card(sorted[i], suit.lowest);
    }
  } // if (!forceSingleton)

  // #6 last remaining non-trump if it's a nine or king
  if ((fehl == 1) && (last_fehl.value() < Card::TEN))
    return last_fehl;
  else
    return Card::EMPTY;

} // Card choose_best_fehl()



/**
 ** finds lowest trump card on Hand h, but gives only fuchs or ten back
 ** if there are better cards to keep
 **
 ** @param     trick     trick to check
 ** @param     h         hand to choose the card from
 ** @param     hi        heuristic interface
 **
 ** @return    the card to play ( or 'Card()' if no card was found )
 **
 ** @author    Borg Enders
 **
 ** @version   Beta
 **/
Card
lowest_best_trump_card( Trick const& t, Hand const& h, HeuristicInterface const& hi )
{
  // @heuristic::name   ?  lowest best trump  ?
  // @heuristic::idea   find smallest best trump card (avoiding for example a fox in most situations)

  HandCard c( h );
  HandCards const ha = hi.hand().validcards( t );
  unsigned i;

  // find any card that's allowed for this trick
  for( i = 0; i < ha.cardsnumber(); i++ )
  {
    if ( ha.card( i ).istrump() )
    {
      c = ha.card( i );
      break;
    }
  } // for( i < ha.cardsnumber() )

  if (c.is_empty())
    return Card::EMPTY;

  // find a better card
  for( i = 0; i < ha.cardsnumber(); i++ )
  {
    if(   ha.card( i ).less( c )
       && ha.card( i ).istrump()
       && !ha.card( i ).isfox()
       && (   ha.card(i).value() != Card::TEN
           || c.isfox()
          )
      )
      c = ha.card( i );
  } // for( i = 0; i < ha.cardsnumber(); i++ )

  if( !c.less( hi.trump_card_limit() ) )
    // maybe a diamond ten isn't all that bad
  {
    for( i = 0; i < ha.cardsnumber(); i++ )
    {
      if(   ha.card( i ).less( c )
         && ha.card( i ).istrump()
         && !ha.card( i ).isfox()
        )
        c = ha.card( i );
    }
  } // if( !c.less( hi.trump_card_limit() ) )

  if( !c.less( hi.trump_card_limit() ) )
    // maybe a diamond Ace isn't all that bad
  {
    for( i = 0; i < ha.cardsnumber(); i++ )
    {
      if(   ha.card( i ).less( c )
         && ha.card(i).istrump()
        )
        c = ha.card( i );
    }
  } // if( !c.less( hi.trump_card_limit() ) )

  return c;
} // Card lowest_best_trump_card( Trick t, Hand h, HeuristicInterface hi )


/**
 ** -> result
 **
 ** @param     trick   current trick
 ** @param     hand    hand of the player
 **
 ** @return    lowest card to play in the trick
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.5
 **/
Card
lowest_card(Trick const& trick, Hand const& hand)
{
  if (trick.isstartcard())
    return lowest_free_card(hand);

  if (!hand.contains(trick.startcard().tcolor()))
    return lowest_free_card(hand);

  if (trick.startcard().istrump())
    return lowest_trump_card(hand);
  else
    return lowest_color_card(trick.startcard().tcolor(), hand);
} // Card lowest_card(Trick trick, Hand hand)


/**
 ** -> result
 **
 ** @param     hand    hand of the player
 **
 ** @return    lowest trump to play
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.5
 **/
Card
lowest_trump_card(Hand const& hand)
{
  DEBUG_ASSERTION(hand.contains(Card::TRUMP),
                  "lowest_trump_card(hand)\n"
                  "  hand does not contain any trump");

  HandCard const& lowest_trump = hand.lowest_trump();
  Game const& game = hand.game();

  if (   !game.is_special(lowest_trump)
      && (lowest_trump.value() < Card::TEN) )
    return lowest_trump;

  DEBUG_ASSERTION(!GAMETYPE::is_picture_solo(game.type()),
                  "lowest_trump_card(hand)\n"
                  "  picture solo: " << game.type());


  vector<Card> const& trumps = game.trumps();
  // search lowest trump
  for (vector<Card>::const_iterator t = trumps.begin();
       t != trumps.end();
       ++t)
    if (hand.contains(*t))
      if (   !game.is_special(HandCard(hand, *t))
          && (t->value() < Card::TEN))
        return *t;

  return Card::EMPTY;
} // Card lowest_trump_card(Hand hand)


/**
 ** -> result
 **
 ** @param     tcolor   tcolor to serve
 ** @param     hand     hand of the player
 **
 ** @return    lowest card to play the tcolor
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.5
 **/
Card
lowest_color_card(Card::TColor const& tcolor, Hand const& hand)
{
  DEBUG_ASSERTION((tcolor != Card::TRUMP),
                  "lowest_color_card(tcolor, hand)\n"
                  "  tcolor ist trump");
  DEBUG_ASSERTION(hand.contains(tcolor),
                  "lowest_color_card(tcolor, hand)\n"
                  "  hand does not contain the color " << tcolor);

  return hand.lowest_card(tcolor);
} // Card lowest_color_card(Card::TColor tcolor, Hand hand)


/**
 ** -> result
 **
 ** @param     hand     hand of the player
 **
 ** @return    lowest card to play (free color selection)
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.5
 **
 ** @todo      replace with Heuristics::choose_best_prefiltered_card()
 **/
Card
lowest_free_card(Hand const& hand)
{
  if (hand.numberoftrumps() == hand.cardsnumber())
    return lowest_trump_card(hand);

  vector<Card::Color> const& card_colors = hand.game().rule().card_colors();
  Card::Color trumpcolor = hand.game().trumpcolor();

  // search the lowest value
  Card::Value lowest_value = Card::ACE;
  for (HandCards::const_iterator c = hand.cards().begin();
       c != hand.cards().end();
       ++c)
    if (!c->istrump() && (c->value() < lowest_value))
      lowest_value = c->value();

  if (lowest_value < Card::ACE) {
    // search single card with lowest value
    for (vector<Card::Color>::const_iterator
         c = card_colors.begin();
         c != card_colors.end();
         ++c)
      if (   (trumpcolor != *c)
          && (hand.numberof(*c) == 1)
          && (hand.contains(Card(*c, lowest_value)))) {
        return Card(*c, lowest_value);
      }
  } // if (lowest_value < Card::ACE)

  if (lowest_value < Card::TEN) {
    // search lowest card of the smallest color without ACE and TEN
    Card::Color best_color = Card::NOCARDCOLOR;
    for (vector<Card::Color>::const_iterator
         c = card_colors.begin();
         c != card_colors.end();
         ++c)
      if (   (trumpcolor != *c)
          && (hand.numberof(Card(*c, Card::ACE)) == 0)
          && (hand.numberof(Card(*c, Card::TEN)) == 0))
        if (   (hand.numberof(*c) > 0)
            && (    (best_color == Card::NOCARDCOLOR)
                || (hand.numberof(*c) < hand.numberof(best_color)) ) )
          best_color = *c;
    if (best_color != Card::NOCARDCOLOR) {
      return hand.lowest_card(best_color);
    }
  } // if (lowest_value < Card::TEN)

  if (lowest_value == Card::ACE) {
    // search double ace
    for (vector<Card::Color>::const_iterator
         c = card_colors.begin();
         c != card_colors.end();
         ++c)
      if ((trumpcolor != *c) && (hand.numberof(*c) == 2)) {
        return Card(*c, lowest_value);
      }
  } // if (lowest_value == Card::ACE)

  if (lowest_value == Card::TEN) {
    // search only tens
    for (vector<Card::Color>::const_iterator
         c = card_colors.begin();
         c != card_colors.end();
         ++c)
      if (   (trumpcolor != *c)
          && hand.contains(Card(*c, lowest_value))
          && (hand.numberof(*c)
              == hand.numberof(Card(*c, lowest_value))) ) {
        return Card(*c, lowest_value);
      }

    // search biggest color
    Card::Color best_color = Card::NOCARDCOLOR;
    for (vector<Card::Color>::const_iterator
         c = card_colors.begin();
         c != card_colors.end();
         ++c) {
      if ((trumpcolor != *c) && hand.contains(Card(*c, lowest_value)))
        if (   (best_color == Card::NOCARDCOLOR)
            || (hand.numberof(*c) > hand.numberof(best_color)))
          best_color = *c;
    } // for (c \in card_colors())
    if (best_color != Card::NOCARDCOLOR) {
      return hand.lowest_card(best_color);
    }
  } // if (lowest_value == Card::TEN)

  { // search lowest card of the biggest color
    Card::Color best_color = Card::NOCARDCOLOR;
    unsigned best_cards_no = 2;
    for (vector<Card::Color>::const_iterator
         c = card_colors.begin();
         c != card_colors.end();
         ++c)
      if ((trumpcolor != *c) && (hand.numberof(*c) > best_cards_no)) {
        best_color = *c;
        best_cards_no = hand.numberof(*c);
      }
    if (best_color != Card::NOCARDCOLOR) {
      return hand.lowest_card(best_color);
    }
  } // search lowest card of the biggest color

  { // search lowest card
    Card::Color best_color = Card::NOCARDCOLOR;
    for (vector<Card::Color>::const_iterator
         c = card_colors.begin();
         c != card_colors.end();
         ++c)
      if (   (trumpcolor != *c)
          && hand.contains(Card(*c, lowest_value))
          && (   (best_color == Card::NOCARDCOLOR)
              || (hand.numberof(*c) > hand.numberof(best_color))))
        best_color = *c;
    if (best_color != Card::NOCARDCOLOR) {
      return hand.lowest_card(best_color);
    }
  } // search lowest card


  DEBUG_ASSERTION(false,
                  "Heuristics::serve_color_trick(trick, hi)\n"
                  "   no play lowest color: no card to play found");
  return Card::EMPTY;
} // Card lowest_free_card(Hand hand)


/**
 ** -> result
 **
 ** @param     hi       heuristic interface
 ** @param     hand     hand of the player
 ** @param     cards    cards to choose from
 **
 ** @return    lowest card to play from 'cards'
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.5
 **/
Card
choose_best_prefiltered_card(HeuristicInterface const& hi,
                             Hand const& hand,
                             HandCards const& cards)
{
  if (cards.cardsnumber() == 0)
    return Card::EMPTY;
  if (cards.cardsnumber() == 1)
    return cards[0];

  if (cards.numberoftrumps() < cards.cardsnumber())
    return choose_best_prefiltered_color_card(hi, hand, cards);
  else
    return choose_best_prefiltered_trump_card(hi, hand, cards);
} // Card choose_best_prefiltered_card(HeuristicsInterface hi, Hand hand, HandCards cards)


/**
 ** -> result
 **
 ** @param     hi       heuristic interface
 ** @param     hand     hand of the player
 ** @param     cards    cards to choose from
 **
 ** @return    lowest card to play from 'cards',
 **            Card::EMPTY, if none is found
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.5
 **/
Card
choose_best_prefiltered_color_card(HeuristicInterface const& hi,
                                   Hand const& hand,
                                   HandCards const& cards)
{
  DEBUG_ASSERTION(cards.numberoftrumps() < cards.cardsnumber(),
                  "Heuristics::choose_best_prefilterd_color_card(hand, cards)\n"
                  "  'cards' only contains trumps");
  if (cards.cardsnumber() == 0)
    return Card::EMPTY;
  if (cards.cardsnumber() == 1)
    return cards[0];

  // search the lowest value
  Card::Value lowest_value = Card::ACE;
  for (vector<HandCard>::const_iterator c = cards.begin();
       c != cards.end();
       ++c)
    if (   !c->istrump()
        && (c->value() < lowest_value) )
      lowest_value = c->value();

  // the card colors with the lowest value and not trump
  vector<Card::Color> card_colors;
  for (vector<Card::Color>::const_iterator
       c = hand.game().rule().card_colors().begin();
       c != hand.game().rule().card_colors().end();
       ++c)
    if (!Card(*c, lowest_value).istrump(hand.game()))
      card_colors.push_back(*c);

  if (card_colors.empty())
    return Card::EMPTY;


  if (lowest_value < Card::ACE) {
    // search single card with lowest value
    for (vector<Card::Color>::const_iterator
         c = card_colors.begin();
         c != card_colors.end();
         ++c)
      if (   (hand.numberof(*c) == 1)
          && (cards.contains(*c, lowest_value))) {
        // ToDo: consider two colors with the same number of cards
        return Card(*c, lowest_value);
      }
  } // if (lowest_value < Card::ACE)

  if (lowest_value < Card::TEN) {
    // search lowest card of the smallest color without ACE and TEN
    Card::Color best_color = Card::NOCARDCOLOR;
    for (vector<Card::Color>::const_iterator
         c = card_colors.begin();
         c != card_colors.end();
         ++c)
      if (   cards.existstcolor(*c)
          && (hand.numberof(Card(*c, Card::ACE)) == 0)
          && (hand.numberof(Card(*c, Card::TEN)) == 0))
        if (   (best_color == Card::NOCARDCOLOR)
            || (cards.numberof(*c) < hand.numberof(best_color)))
          // ToDo: improve selection -- card value comparison, compare remaining cards of the color, compare jabbing, ...
          best_color = *c;
    if (best_color != Card::NOCARDCOLOR) {
      return cards.lowest_card(best_color);
    }
  } // if (lowest_value < Card::TEN)

  if (lowest_value == Card::ACE) {
    // search double ace
    for (vector<Card::Color>::const_iterator
         c = card_colors.begin();
         c != card_colors.end();
         ++c)
      if (   (hand.numberof(*c) == 2)
          && cards.contains(*c, Card::ACE) ) {
        return Card(*c, lowest_value);
      }
  } // if (lowest_value == Card::ACE)
  if (lowest_value == Card::TEN) {
    // search only tens
    for (vector<Card::Color>::const_iterator
         c = card_colors.begin();
         c != card_colors.end();
         ++c)
      if (   cards.contains(*c, lowest_value)
          && (hand.numberof(*c)
              == hand.numberof(*c, lowest_value)) ) {
        return Card(*c, lowest_value);
      }

    // search biggest color
    Card::Color best_color = Card::NOCARDCOLOR;
    for (vector<Card::Color>::const_iterator
         c = card_colors.begin();
         c != card_colors.end();
         ++c) {
      if (cards.contains(Card(*c, lowest_value)))
        if (   (best_color == Card::NOCARDCOLOR)
            || (hand.numberof(*c) > hand.numberof(best_color)))
          best_color = *c;
    } // for (c \in card_colors())
    // ToDo: compare ace ace ten, ace ten ten
    if (best_color != Card::NOCARDCOLOR) {
      return Card(best_color, lowest_value);
    }
  } // if (lowest_value == Card::TEN)

  { // search lowest card of the biggest color with min 3 cards
    Card::Color best_color = Card::NOCARDCOLOR;
    unsigned best_cards_no = 2;
    for (vector<Card::Color>::const_iterator
         c = card_colors.begin();
         c != card_colors.end();
         ++c)
      if (   (hand.numberof(*c) > best_cards_no)
          && (cards.existstcolor(*c)) ) {
        best_color = *c;
        best_cards_no = hand.numberof(*c);
      }
    if (   (best_color != Card::NOCARDCOLOR)
        && cards.contains(hand.lowest_card(best_color)) ) {
      // ToDo: take smallest card from cards
      return hand.lowest_card(best_color);
    }
  } // search lowest card of the biggest color

  { // search lowest card of the largest color
    Card::Color best_color = Card::NOCARDCOLOR;
    for (vector<Card::Color>::const_iterator
         c = card_colors.begin();
         c != card_colors.end();
         ++c)
      if (   cards.contains(Card(*c, lowest_value))
          && (   (best_color == Card::NOCARDCOLOR)
              || (hand.numberof(*c) > hand.numberof(best_color))))
        best_color = *c;
    if (best_color != Card::NOCARDCOLOR) {
      return cards.lowest_card(best_color);
    }
  } // search lowest card

  return Card::EMPTY;
} // static Card choose_best_prefiltered_color_card(HeuristicsInterface hi, Hand hand, HandCards cards)


/**
 ** -> result
 **
 ** @param     hi       heuristic interface
 ** @param     hand     hand of the player
 ** @param     cards    cards to choose from
 **
 ** @return    lowest card to play from 'cards'
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.5
 **/
Card
choose_best_prefiltered_trump_card(HeuristicInterface const& hi,
                                   Hand const& hand,
                                   HandCards const& cards)
{
  DEBUG_ASSERTION(cards.numberoftrumps() == cards.cardsnumber(),
                  "Heuristics::choose_best_prefilterd_trump_card(hand, cards)\n"
                  "  'cards' does contain color cards");
  if (cards.cardsnumber() == 0)
    return Card::EMPTY;
  if (cards.cardsnumber() == 1)
    return cards[0];

  HandCard card(hand);

  // find any card that's allowed for this trick
  for(unsigned i = 0; i < cards.cardsnumber(); i++ )
  {
    if ( cards.card( i ).istrump() )
    {
      card = cards.card( i );
      break;
    }
  } // for( i < ha.cardsnumber() )

  if( card.is_empty())
    return Card::EMPTY;

  // find a better card
  for(unsigned i = 0; i < cards.cardsnumber(); i++ )
  {
    if(   cards.card( i ).less(card)
       && cards.card( i ).istrump()
       && !cards.card( i ).isfox()
       && (   cards.card(i).value() != Card::TEN
           || card.isfox()
          )
      )
      card = cards.card( i );
  } // for( i = 0; i < cards.cardsnumber(); i++ )

  if( !card.less( hi.trump_card_limit() ) )
    // maybe a diamond ten isn't all that bad
  {
    for(unsigned i = 0; i < cards.cardsnumber(); i++ )
    {
      if(   cards.card( i ).less( card )
         && cards.card( i ).istrump()
         && !cards.card( i ).isfox()
        )
        card = cards.card( i );
    }
  } // if( !card.less( cards.trump_card_limit() ) )

  if( !card.less( hi.trump_card_limit() ) )
    // maybe a diamond Ace isn't all that bad
  {
    for(unsigned i = 0; i < cards.cardsnumber(); i++ )
    {
      if(   cards.card( i ).less( card )
         && cards.card(i).istrump()
        )
        card = cards.card( i );
    }
  } // if( !c.less( hi.trump_card_limit() ) )

  return card;
} // Card choose_best_prefiltered_trump_card(HeuristicsInterface hi, Hand hand, HandCards cards)


/**
 ** finds lowest color card on Hand h, wihich is the best in a meatless solo
 **
 ** @param     trick     trick to check
 ** @param     h         hand to choose the card from
 **
 ** @return    the card to play ( or 'Card()' if no card was found )
 **
 ** @author    Borg Enders
 **
 ** @version   Beta
 **/
Card
lowest_color_card_meatless( Trick const& t, Hand const& h )
{
  HandCards const ha = h.validcards( t );
  if (ha.empty())
    return Card::EMPTY;

  Card card_to_play = ha[0];

  // find a better card
  for (vector<HandCard>::const_iterator c = ha.begin();
       c != ha.end();
       ++c)
  {
    if(   card_to_play.value() <= c->value()
       && (   h.numberof( c->color() ) >  h.numberof( card_to_play.color() )
           || (   !ha.contains(c->color(), Card::TEN)
               && !ha.contains(c->color(), Card::ACE)
              )
          )
      )
      card_to_play = *c;
  } // for ( i < ha.cardsnumber() )

  return card_to_play;
} // Card lowest_color_card_meatless( Trick t, Hand h )


} // namespace HEURISTICS_UTILS

