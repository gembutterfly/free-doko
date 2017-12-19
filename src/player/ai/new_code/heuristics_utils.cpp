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

namespace HEURISTICS_UTILS {




/**
 ** get the number of "no-trump" cards per color
 **
 ** @param     trick   the current trick
 ** @param     hi      heuristic interface
 **
 ** @return    number of cards per miscolor
 **
 ** @author    Borg Enders
 **
 ** @version   ???
 **/
unsigned
number_of_no_trumps_per_color()
{
  unsigned i;
  unsigned notrump_per_color=0;

  // calculate number of no trump cards per color
  // only one color can be trump
  // if spade is trump caculate with club otherwise with spade
  if( Card( Card::SPADE, Card::ACE ).istrump( ::party.game() ) )
  {
    for ( i = 0;  i < 2 * ::party.rule()(Rule::NUMBER_OF_CARD_VALUES); i += 2 )
      if( !Card( Card::InttoColor(Card::CLUB), Card::InttoValue(i) ).istrump(
                                                                             ::party.game() )
        )
        notrump_per_color+=2;
  } else { // if( Card( Card::SPADE, Card::ACE ).istrump( ::party.game() ) )
    for ( i = 0;  i < 2 * ::party.rule()(Rule::NUMBER_OF_CARD_VALUES); i += 2 )
      if( !Card( Card::InttoColor( Card::SPADE ),
                Card::InttoValue(i) ).istrump( ::party.game() )
        )
        notrump_per_color+=2;
  } // else

  return notrump_per_color;
} // unsigned number_of_no_trumps_per_color()













#if 0
/**
 ** finds lowest trump card on Hand h
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
lowest_trump_card( Trick const& t, Hand const& h )
{
  Card c;
  HandCards const ha = h.validcards( t );
  unsigned i;

  // find any card that's allowed for this trick
  for( i = 0; i < ha.cardsnumber(); i++ )
  {
    if( ha.card( i ).istrump() )
    {
      c = ha.card( i );
      break;
    }
  } // for ( i < ha.cardsnumber() )

  if (c.is_empty())
    return Card::EMPTY;

  // find a better card
  for( i = 0; i < ha.cardsnumber(); i++ )
  {
    if(   ha.card(i).less( c )
       && ha.card(i).istrump()
      )
      c = ha.card( i );
  } // for ( i < ha.cardsnumber() )

  return c;
} // Card lowest_trump_card( Trick t, Hand h )


/**
 ** finds highest trump card on Hand h
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
highest_trump_card( Trick const& t, Hand h )
{
  Card c;
  HandCards const ha = h.validcards( t );
  unsigned i;

  // find any card that's allowed for this trick
  for( i = 0; i < ha.cardsnumber(); i++ )
  {
    if( ha.card( i ).istrump() )
    {
      c = ha.card( i );
      break;
    }
  } // for ( i < ha.cardsnumber() )

  if (c.is_empty())
    return Card::EMPTY;

  // find a better card
  for( i = 0; i < ha.cardsnumber(); i++ )
  {
    if(   c.less( ha.card(i) )
       && ha.card(i).istrump()
      )
      c = ha.card( i );
  } // for ( i < ha.cardsnumber() )

  return c;
} // Card highest_trump_card( Trick t, Hand h )
#endif


/**
 ** -> result
 **
 ** @param        old_card        best card till now
 ** @param        test_card        card to test
 ** @param        hi                hi (with the configuration)
 **
 ** @return        whether the card 'test_card' is to preferred to 'old_card'
 **                that is has more points
 **
 ** @author        Borg Enders
 **
 ** @version        0.6.9
 **/
bool
better_points_optimize(HandCard const& old_card,
                       HandCard const& test_card,
                       HeuristicInterface const& hi )
{
  return (  (  hi.game().trick_current_no()
             >= hi.value(Aiconfig::FIRST_TRICK_FOR_TRUMP_POINTS_OPTIMIZATION)
            )
          &&   ( old_card.value() < test_card.value() )
          || ( test_card.less( old_card ) && old_card.value() == test_card.value() )
         );
} // bool better_points_optimize(HandCard old_card, HandCard test_card, Ai hi)


/**
 ** -> return
 **
 ** @param     hi                      heuristic interface
 ** @param     card                    card to check
 ** @param     ignore_identical_card   ignore a second dolle that's higher than the first one
 **
 ** @return    the number of the other players' cards that are higher than card
 **
 ** @author    Tassilo Kirsch
 **
 ** @version   0.7.6
 **/
unsigned
numberof_higher_cards_others(HeuristicInterface const& hi, Card const& card, bool ignore_identical_card) // = false
{
  unsigned n = 0;

  vector<Card> const& highercards = hi.game().higher_cards(card);
  for (vector<Card>::const_iterator c = highercards.begin();
       c != highercards.end();
       ++c)
    if (!ignore_identical_card || (*c != card))
      n += hi.cards_information().remaining_others(*c);

  return n;
} // numberof_higher_cards_others(...)


/**
 ** higher_card is a better card if there is at least one card left on the other hands
 ** that is lower than higher_card but higher than lower_card
 **
 ** @param     hi             heuristic interface
 ** @param     higher_card    the highe card to check
 ** @param     lower_card     the lower card
 **
 ** @return    true if higher card is a better card than lower card
 **
 ** @author    Tassilo Kirsch
 **
 ** @version   0.7.6
 **/
bool
is_better_card(HeuristicInterface const& hi, Card const& higher_card, Card const& lower_card)
{
  if (!hi.game().less(lower_card, higher_card))
    return false;

  vector<Card> between = hi.game().cards_between(lower_card, higher_card);
  // we also have to check the lower card itself
  between.push_front(lower_card);
  for (vector<Card>::const_iterator c = between.begin();
       c != between.end();
       ++c)
    if (hi.cards_information().remaining_others(*c))
      return true;

  return false;
} // is_better_card(...)


} // namespace HEURISTICS_UTILS

