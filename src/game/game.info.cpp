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
 *********************************************************************/

#include "constants.h"

#include "game.h"

#include "../player/player.h"
#include "../party/rule.h"
#include "../card/trick.h"

/**
 ** -> result
 **
 ** @param       -
 **
 ** @return      number of cards
 **
 ** @author      Diether Knof
 **
 ** @version     0.7.6
 **/
unsigned
Game::cards_no() const
{
  return (this->different_cards_no()
          * this->rule()(Rule::NUMBER_OF_SAME_CARDS));
} // unsigned Game::cards_no() const

/**
 ** -> result
 **
 ** @param       -
 **
 ** @return      number of different cards
 **
 ** @author      Diether Knof
 **
 ** @version     0.7.6
 **/
unsigned
Game::different_cards_no() const
{
  return this->rule()(Rule::NUMBER_OF_CARDS);
} // unsigned Game::different_cards_no() const

/**
 ** -> result
 **
 ** @param       tcolor	tcolor
 **
 ** @return      number of cards of the tcolor
 **
 ** @author      Diether Knof
 **
 ** @version     0.7.5
 **/
unsigned
Game::cards_no(Card::TColor const& tcolor) const
{
  if (tcolor == Card::UNKNOWNCARDCOLOR)
    return this->cards_no();
  else
    return (this->different_cards_no(tcolor)
            * this->rule()(Rule::NUMBER_OF_SAME_CARDS));
} // unsigned Game::cards_no(Card::TColor tcolor) const

/**
 ** -> result
 **
 ** @param       tcolor	tcolor
 **
 ** @return      number of cards of the tcolor
 **
 ** @author      Diether Knof
 **
 ** @version     0.7.5
 **/
unsigned
Game::numberof(Card::TColor const& tcolor) const
{
  return this->cards_no(tcolor);
} // unsigned Game::numberof(Card::TColor tcolor) const

/**
 ** -> result
 **
 ** @param       tcolor	tcolor
 **
 ** @return      number of different cards of the tcolor
 **
 ** @author      Borg Enders
 ** @author      Diether Knof
 **
 ** @version     0.7.0
 **/
unsigned
Game::different_cards_no(Card::TColor const& tcolor) const
{
  if (tcolor == Card::UNKNOWNCARDCOLOR)
    return this->cards_no();
  if (tcolor == Card::TRUMP)
    return (this->trumps_no() / this->rule()(Rule::NUMBER_OF_SAME_CARDS));
  if (tcolor == this->trumpcolor())
    return 0;

  switch (this->type()) {
  case GAMETYPE::NORMAL:
  case GAMETYPE::FOX_HIGHEST_TRUMP:
  case GAMETYPE::POVERTY:
  case GAMETYPE::GENSCHER:
  case GAMETYPE::MARRIAGE:
  case GAMETYPE::MARRIAGE_SOLO:
  case GAMETYPE::MARRIAGE_SILENT:
  case GAMETYPE::SOLO_DIAMOND:
  case GAMETYPE::SOLO_HEART:
  case GAMETYPE::SOLO_SPADE:
  case GAMETYPE::SOLO_CLUB:
    {
      unsigned number = this->rule()(Rule::NUMBER_OF_CARD_VALUES) - 2;
      if ( ( tcolor == Card::HEART )
          && this->rule()(Rule::DOLLEN))
        number -= 1;
      return number;
    }
  case GAMETYPE::SOLO_JACK:
  case GAMETYPE::SOLO_QUEEN:
  case GAMETYPE::SOLO_KING:
    return ( this->rule()(Rule::NUMBER_OF_CARD_VALUES) - 1 );

  case GAMETYPE::SOLO_QUEEN_JACK:
  case GAMETYPE::SOLO_KING_JACK:
  case GAMETYPE::SOLO_KING_QUEEN:
    return ( this->rule()(Rule::NUMBER_OF_CARD_VALUES) - 2 );

  case GAMETYPE::SOLO_KOEHLER:
    return ( this->rule()(Rule::NUMBER_OF_CARD_VALUES) - 3 );

  case GAMETYPE::SOLO_MEATLESS:
    return this->rule()(Rule::NUMBER_OF_CARD_VALUES);

  case GAMETYPE::THROWN_NINES:
  case GAMETYPE::THROWN_KINGS:
    return this->rule()(Rule::NUMBER_OF_CARD_COLORS);

  case GAMETYPE::THROWN_NINES_AND_KINGS:
    return (2 * this->rule()(Rule::NUMBER_OF_CARD_COLORS));

  case GAMETYPE::THROWN_RICHNESS:
    return 0;
  } // this->type()

  return 0;
} // unsigned Game::different_cards_no(Card::TColor tcolor) const

/**
 ** -> result
 **
 ** @param       -
 **
 ** @return      number of trumps in the game
 **
 ** @author      Borg Enders
 ** @author      Diether Knof
 **
 ** @version     0.4.4
 **/
unsigned
Game::trumps_no() const
{
  // number of trumps
  unsigned trumps = 0;

  switch (this->type()) {
  case GAMETYPE::NORMAL:
  case GAMETYPE::FOX_HIGHEST_TRUMP:
  case GAMETYPE::POVERTY:
  case GAMETYPE::GENSCHER:
  case GAMETYPE::MARRIAGE:
  case GAMETYPE::MARRIAGE_SOLO:
  case GAMETYPE::MARRIAGE_SILENT:
  case GAMETYPE::SOLO_DIAMOND:
  case GAMETYPE::SOLO_HEART:
  case GAMETYPE::SOLO_SPADE:
  case GAMETYPE::SOLO_CLUB:
    trumps = 2 * this->rule()(Rule::NUMBER_OF_CARD_COLORS); // jack and queen
    trumps += (this->rule()(Rule::NUMBER_OF_CARD_VALUES)
               - 2); // rest trump color
    if (this->rule()(Rule::DOLLEN)
        && (this->type() != GAMETYPE::SOLO_HEART))
      trumps += 1;
    break;
  case GAMETYPE::SOLO_JACK:
  case GAMETYPE::SOLO_QUEEN:
  case GAMETYPE::SOLO_KING:
    trumps = this->rule()(Rule::NUMBER_OF_CARD_COLORS);
    break;
  case GAMETYPE::SOLO_QUEEN_JACK:
  case GAMETYPE::SOLO_KING_JACK:
  case GAMETYPE::SOLO_KING_QUEEN:
    trumps = 2 * this->rule()(Rule::NUMBER_OF_CARD_COLORS);
    break;
  case GAMETYPE::SOLO_KOEHLER:
    trumps = 3 * this->rule()(Rule::NUMBER_OF_CARD_COLORS);
    break;
  case GAMETYPE::SOLO_MEATLESS:
    trumps = 0;
    break;
  case GAMETYPE::THROWN_NINES:
  case GAMETYPE::THROWN_KINGS:
    trumps = this->rule()(Rule::NUMBER_OF_CARD_COLORS);
    break;

  case GAMETYPE::THROWN_NINES_AND_KINGS:
    trumps = 2 * this->rule()(Rule::NUMBER_OF_CARD_COLORS);
    break;

  case GAMETYPE::THROWN_RICHNESS:
    trumps = (this->rule()(Rule::NUMBER_OF_CARDS)
              / this->rule()(Rule::NUMBER_OF_SAME_CARDS));
    break;
  } // this->type()
  trumps *= this->rule()(Rule::NUMBER_OF_SAME_CARDS);

  return trumps;
} // unsigned Game::trumps_no() const

/**
 **
 ** -> result
 **
 ** @param       -
 **
 ** @return      number of played trumps
 **
 ** @version     0.4.4
 **
 ** @author      Borg Enders
 ** @author      Diether Knof
 **
 ** @todo        perhaps use a variable,
 **              so that this function does not take much time
 **
 */
unsigned
Game::trumps_played_no() const
{
  // number of trumps
  unsigned trumps = 0;
  for (vector<Trick*>::const_iterator trick = this->tricks().begin();
       trick != this->tricks().end();
       trick++)
    for (unsigned c = 0; c < (*trick)->actcardno(); c++)
      if ((*trick)->card(c).istrump())
        trumps += 1;

  return trumps;
} // unsigned Game::trumps_played_no() const

/**
 **
 ** -> result
 **
 ** @param       -
 **
 ** @return      number of played cards
 **
 ** @version     0.6.8
 **
 ** @author      Diether Knof
 **
 **/
unsigned
Game::played_cards_no() const
{
  if (this->tricks().empty())
    return 0;

  if (::game_status == GAMESTATUS::GAME_FINISHED)
    return (this->trickno()
            * this->rule()(Rule::NUMBER_OF_PLAYERS_IN_GAME));

  return (this->real_trick_current_no()
          * this->rule()(Rule::NUMBER_OF_PLAYERS_IN_GAME)
          + this->trick_current().actcardno());
} // unsigned Game::played_cards_no() const

/**
 ** -> result
 **
 ** @param       -
 **
 ** @return      number of cards remaining to be played
 **
 ** @author      Diether Knof
 **
 ** @version     0.7.8
 **/
unsigned
Game::remaining_cards_no() const
{
  return (this->trickno()
          * this->rule()(Rule::NUMBER_OF_PLAYERS_IN_GAME)
          * - this->played_cards_no());
} // unsigned Game::remaining_cards_no() const

/**
 **
 ** -> result
 **
 ** @param       card	card
 **
 ** @return      number of played cards 'card'
 **
 ** @version     0.6.8
 **
 ** @author      Diether Knof
 **
 ** @todo        perhaps use a variable,
 **              so that this function does not take much time
 **
 **/
unsigned
Game::played_no(Card const& card) const
{
  // number of trumps
  unsigned no = 0;
  for (vector<Trick*>::const_iterator trick = this->tricks().begin();
       trick != this->tricks().end();
       trick++)
    for (HandCards::const_iterator c = (*trick)->cards().begin();
         c != (*trick)->cards().end();
         ++c)
      if (*c == card)
        no += 1;

  return no;
} // unsigned Game::played_no(Card card) const

/**
 ** -> result
 **
 ** @param       -
 **
 ** @return      the trumpcolor
 **
 ** @author      Borg Enders
 ** @author      Diether Knof
 **
 ** @version     0.4.4
 **/
Card::Color
Game::trumpcolor() const
{
  switch (this->type()) {
  case GAMETYPE::NORMAL:
  case GAMETYPE::POVERTY:
  case GAMETYPE::FOX_HIGHEST_TRUMP:
  case GAMETYPE::GENSCHER:
  case GAMETYPE::MARRIAGE:
  case GAMETYPE::MARRIAGE_SOLO:
  case GAMETYPE::MARRIAGE_SILENT:
  case GAMETYPE::SOLO_DIAMOND:
    return Card::DIAMOND;
  case GAMETYPE::SOLO_HEART:
    return Card::HEART;
  case GAMETYPE::SOLO_SPADE:
    return Card::SPADE;
  case GAMETYPE::SOLO_CLUB:
    return Card::CLUB;
  case GAMETYPE::SOLO_JACK:
  case GAMETYPE::SOLO_QUEEN:
  case GAMETYPE::SOLO_KING:
  case GAMETYPE::SOLO_QUEEN_JACK:
  case GAMETYPE::SOLO_KING_JACK:
  case GAMETYPE::SOLO_KING_QUEEN:
  case GAMETYPE::SOLO_KOEHLER:
  case GAMETYPE::SOLO_MEATLESS:
  case GAMETYPE::THROWN_NINES:
  case GAMETYPE::THROWN_KINGS:
  case GAMETYPE::THROWN_NINES_AND_KINGS:
  case GAMETYPE::THROWN_RICHNESS:
    return Card::NOCARDCOLOR;
  } // switch(this->type())

  DEBUG_ASSERTION(false,
                  "Game::trumpcolor()\n"
                  "  after return");
  return Card::NOCARDCOLOR;
} // Card::Color Game::trumpcolor() const

/**
 ** -> result
 **
 ** @param       tcolor   tcolor
 **
 ** @return      the cards of the tcolor
 **
 ** @author      Diether Knof
 **
 ** @version     0.7.6
 **/
vector<Card> const&
Game::cards(Card::TColor const& tcolor) const
{
  DEBUG_ASSERTION(tcolor <= Card::TRUMP,
                  "Game::cards(tcolor)\n"
                  "  tcolor = " << tcolor << " is not valid.");

  return this->cards_[tcolor];
} // vector<Card> Game::cards(Card::TColor tcolor) const

/**
 ** -> result
 **
 ** @param       -
 **
 ** @return      the trumps
 **
 ** @author      Diether Knof
 **
 ** @version     0.7.6
 **/
vector<Card> const&
Game::trumps() const
{
  return this->cards(Card::TRUMP);
} // vector<Card> Game::trumps() const

/**
 ** -> result
 **
 ** @note	if both cards are equal, the first (this) is not less than
 **		the second (card)
 **		(but the dollen -- see rules)
 **
 ** @param	card_a   first card
 ** @param	card_b   second card
 **
 ** @return	whether 'card_a' is less than 'card_b'
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.4
 **/
bool
Game::less(Card const& card_a, Card const& card_b) const
{
  if (card_b.is_empty())
    return false;
  if (card_a.is_empty())
    return true;

  Card const& a = card_a;
  Card const& b = card_b;
  Game const& game = *this;
  Rule const& rule = this->rule();

  // both cards are not trump
  if (!a.istrump(game) && !b.istrump(game)) {
    if (a.color() == b.color()) // same color
      return (a.value() < b.value());
    else // different color
      return false;
  }

  // a is trump but b not
  if (a.istrump(game) && !b.istrump(game))
    return false;

  // a is not trump but b is
  if (!a.istrump(game) && b.istrump(game))
    return true;


  // The difficult case: both cards are trump

  switch(game.type()) {
    using namespace GAMETYPE;
  case NORMAL:
  case POVERTY:
  case FOX_HIGHEST_TRUMP:
  case GENSCHER:
  case MARRIAGE:
  case MARRIAGE_SOLO:
  case MARRIAGE_SILENT:
  case SOLO_DIAMOND:
  case SOLO_CLUB:
  case SOLO_HEART:
  case SOLO_SPADE:

    // Hyperswines
    if (rule(Rule::HYPERSWINES)) {
      if (this->hyperswines_announced()) {
        if (a == this->hyperswine())
          return false;
        if (b == this->hyperswine())
          return true;
      } // if (this->hyperswines_announced())
    } // if (rule(Rule::HYPERSWINES))

    // Swines
    if (rule(Rule::SWINES)) {
      if (this->swines_announced()) {
        if (a == this->swine())
          return false;
        if (b == this->swine())
          return true;
      } // if (this->swines_announced())
    } // if (rule(Rule::SWINES))

    // Dollen
    if (rule(Rule::DOLLEN)) {
      if (a == Card::DOLLE) {
        if (b == Card::DOLLE) { // both cards are dollen
          if ((rule(Rule::DOLLEN_FIRST_OVER_SECOND_WITH_SWINES)
               && game.swines_owner()) )
            return false;
          if (rule(Rule::DOLLEN_CONTRARY_IN_LAST_TRICK)
              && game.is_last_trick()) {
            // we are in the last trick
            // and the rule 'DOLLEN_CONTRARY_IN_LAST_TRICK' is valid
            if (rule(Rule::DOLLEN_SECOND_OVER_FIRST)) {
              return false;
            } else { // if !(rule(Rule::DOLLEN_SECOND_OVER_FIRST)
              return true;
            } // if !(rule(Rule::DOLLEN_SECOND_OVER_FIRST)
          } else { // if !(game.is_last_trick())
            if (rule(Rule::DOLLEN_SECOND_OVER_FIRST)) {
              return true;
            } else { // if !(rule(Rule::DOLLEN_SECOND_OVER_FIRST)
              return false;
            } // if !(rule(Rule::DOLLEN_SECOND_OVER_FIRST)
          } // if !(game.is_last_trick())
        } else { // if !(b == Card::DOLLE)
          return false;
        } // if !(b == Card::DOLLE)
      } else { // if !(a == Card::DOLLE)
        if (b == Card::DOLLE)
          return true;
      } // if !(a == Card::DOLLE)
    } // if (rule.dollen)

    if (a.value() == Card::QUEEN) { // 'a' is a QUEEN
      if (b.value() == Card::QUEEN) // both cards are QUEENs
        return (a.color() < b.color());
      else // 'b' is lesser than a QUEEN (JACK or DIAMOND)
        return false;
    }

    if (a.value() == Card::JACK) { // 'a' is a JACK
      if (b.value() == Card::QUEEN) // 'b' is a QUEEN
        return true;
      else if (b.value() == Card::JACK) // 'a' and 'b' are JACKs
        return (a.color() < b.color());
      else // 'b' is a DIAMOND/CLUB/SPADE/HEART
        return false;
    }

    // 'a' is a simple ace/ten/king/nine
    if ( (b.value() == Card::QUEEN) ||
        (b.value() == Card::JACK) ) // 'b' is higher than an ace/ten/king/nine
      return true;

    // both cards are simple ace/ten/king/nine
    return (a.value() < b.value());
  case SOLO_JACK:
  case SOLO_QUEEN:
  case SOLO_KING:
    return (a.color() < b.color());
  case SOLO_QUEEN_JACK:
    if (a.value() != b.value()) // one card is queen, one is jack
      return (a.value() == Card::JACK);
    // both card are JACK/QUEEN
    return (a.color() < b.color());
  case SOLO_KING_JACK:
    if (a.value() != b.value()) // one card is king, one is jack
      return (a.value() == Card::JACK);
    // both card are JACK/KING
    return (a.color() < b.color());
  case SOLO_KING_QUEEN:
    if (a.value() != b.value()) // one card is king, one is queen
      return (a.value() == Card::QUEEN);
    // both card are QUEEN/KING
    return (a.color() < b.color());
  case SOLO_KOEHLER:
    if (a.value() == b.value()) // both cards are the same value
      return (a.color() < b.color());
    // both cards have different values
    if (a.value() == Card::JACK)
      return true;
    if (b.value() == Card::JACK)
      return false;
    // one card is queen, one is king
    // -- hey, that is like in 'SOLO_KING_QUEEN', take that code
    return (a.value() == Card::QUEEN);
  case SOLO_MEATLESS:
    // cannot be, because no card is trump
    DEBUG_ASSERTION(false,
                    "  SOLO_MEATLESS with trump ");

    break;
  case THROWN_NINES:
    return (a.color() < b.color());
  case THROWN_KINGS:
    return (a.color() < b.color());
  case THROWN_NINES_AND_KINGS:
    if (   (a.value() == Card::NINE)
        && (b.value() == Card::KING) )
      return true;
    if (   (a.value() == Card::KING)
        && (b.value() == Card::NINE) )
      return false;
    return (a.color() < b.color());

  case THROWN_RICHNESS:
    return (   (a.value() < b.value())
            || (   (a.value() == b.value())
                && (a.color() < b.color())));
  } // switch(game.type())

  DEBUG_ASSERTION(false,
                  "Card::less(card, game):\n"
                  "  after 'switch(game.type())");

  return false;
} // bool Game::less(Card card_a, Card card_b) const

/**
 ** -> result
 **
 ** @param     card   card
 **
 ** @return    the next lower card than 'card'
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.9
 **
 ** @todo      use code that is more performant
 **/
Card
Game::next_lower_card(Card const& card) const
{
  vector<Card> lower_cards = this->lower_cards(card);
  if (lower_cards.empty())
    return Card::EMPTY;

  return lower_cards.front();
} // Card Game::next_lower_card(Card card) const

/**
 ** -> result
 **
 ** @param     card   card
 **
 ** @return    the next higher card than 'card'
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.9
 **
 ** @todo      use code that is more performant
 **/
Card
Game::next_higher_card(Card const& card) const
{
  vector<Card> higher_cards = this->higher_cards(card);
  if (higher_cards.empty())
    return Card::EMPTY;

  return higher_cards.front();
} // Card Game::next_higher_card(Card card) const

/**
 ** -> result
 **
 ** @param     card   card
 ** @param     n      number of cards to get higher
 **
 ** @return    the next higher card in n steps than 'card'
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.12
 **
 ** @todo      use code that is more performant
 **/
Card
Game::next_higher_card(Card const& card, unsigned const n) const
{
  if (n == 0)
    return card;
  else if (n == 1)
    return this->next_higher_card(card);
  else // if (n > 1)
    return this->next_higher_card(card, n-1);
} // Card Game::next_higher_card(Card card, unsigned n) const

/**
 ** -> result
 **
 ** @param     card   card
 **
 ** @return    all cards lower than 'card' (in ascending order)
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
vector<Card>
Game::lower_cards(Card const& card) const
{
  if (card.istrump(*this)) {
    vector<Card>::const_reverse_iterator c;
    for (c = this->trumps().rbegin(); c != this->trumps().rend(); ++c)
      if (*c == card)
        break;
    return vector<Card>(++c, this->trumps().rend());
  } else { // if !(card.istrump(*this))
    vector<Card> cards;
    for (vector<Card::Value>::const_iterator
         v =  this->rule().card_values().begin();
         v !=  this->rule().card_values().end();
         ++v) {
      Card const c(card.color(), *v);
      if (   !c.istrump(*this)
          && this->less(c, card))
        cards.insert(cards.begin(), c);
    } // for (v \in card values)

    return cards;
  } // if !(card.istrump(*this))
} // vector<Card> Game::lower_cards(Card card) const

/**
 ** -> result
 **
 ** @param     card   card
 **
 ** @return    all cards higher than 'card' (in ascending order)
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
vector<Card>
Game::higher_cards(Card const& card) const
{
  if (card.istrump(*this)) {
    vector<Card>::const_iterator c;
    for (c = this->trumps().begin(); c != this->trumps().end(); ++c)
      if (this->less(card, *c))
        break;
    return vector<Card>(c, this->trumps().end());

  } else { // if !(card.istrump(*this))
    vector<Card> cards;
    for (vector<Card::Value>::const_iterator
         v =  this->rule().card_values().begin();
         v !=  this->rule().card_values().end();
         ++v) {
      Card const c(card.color(), *v);
      if (   !c.istrump(*this)
          && this->less(card, c))
        cards.push_back(c);
    } // for (v \in card values)

    cards.insert(cards.end(), this->trumps().begin(), this->trumps().end());
    return cards;
  } // if !(card.istrump(*this))
} // vector<Card> Game::higher_cards(Card card) const

/**
 ** -> result
 **
 ** @param     card_a   the first card
 ** @param     card_b   the second card
 **
 ** @return    the cards between 'card_a' and 'card_b' (in ascending order)
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **
 ** @todo      all
 **/
vector<Card>
Game::cards_between(Card const& card_a, Card const& card_b) const
{
  // the cards between
  vector<Card> cards = this->higher_cards(card_a);

  // search the first card not less than 'card_b'
  vector<Card>::iterator c;
  for (c = cards.begin(); c != cards.end(); ++c)
    if (   (*c == card_b)
        || (!this->less(*c, card_b)) )
      break;

  return vector<Card>(cards.begin(), c);
} // vector<Card> Game::cards_between(Card card_a, Card card_b) const

/**
 ** -> result
 **
 ** @param     card   card
 **
 ** @return    all trumps lower than 'card' (in ascending order)
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.6
 **/
vector<Card>
Game::lower_trumps(Card const& card) const
{
  if (!card.istrump(*this))
    return vector<Card>();

  vector<Card>::const_iterator c;
  for (c = this->trumps().begin(); c != this->trumps().end(); ++c)
    if (!this->less(*c, card))
      break;
  return vector<Card>(this->trumps().begin(), c);
} // vector<Card> Game::lower_trumps(Card card) const


/**
 ** -> result
 **
 ** @param       team    Team
 **
 ** @return      whether the team has got a trick
 **
 ** @author      Borg Enders
 ** @author      Diether Knof
 **
 ** @version     0.4.4
 **/
bool
Game::hastrick(const TEAM::Team& team) const
{
  return (this->numberoftricks_of_team(team) > 0);
} // bool Game::hastrick(const TEAM::Team& team) const

/**
 ** -> result
 **
 ** @param       player   the player
 **
 ** @return      whether the player has got a trick
 **
 ** @author      Borg Enders
 ** @author      Diether Knof
 **
 ** @version     0.4.4
 **/
bool
Game::hastrick(Player const& player) const
{
  return (this->numberoftricks_of_player(player) > 0);
} // bool Game::hastrick(Player player) const

/**
 **
 ** -> result
 **
 ** @param       player  player
 **
 ** @return      number of tricks, the player has won so far
 **              (with the current trick)
 **
 ** @version     0.4.4
 **
 ** @author      Borg Enders
 ** @author      Diether Knof
 **
 */
unsigned
Game::numberoftricks_of_player(Player const& player) const
{
  unsigned number = 0;

  vector<Trick*>::const_iterator trick;
  for (trick = this->tricks().begin();
       trick != this->tricks().end();
       trick++)
    if (((*trick)->winnerplayer() == player)
        && (*trick)->intrickpile())
      number += 1;

  return number;
} // unsigned Game::numberoftricks_of_player(Player const& player) const

/**
 **
 ** -> result
 **
 ** @param       player  player
 ** @param       trickno last trick to view
 **
 ** @return      number of tricks, the player has won til the trick 'trickno'
 **
 ** @version     0.6.7
 **
 ** @author      Borg Enders
 ** @author      Diether Knof
 **
 */
unsigned
Game::numberoftricks_of_player(Player const& player,
                               unsigned trickno) const
{
  unsigned number = 0;

  if (trickno != UINT_MAX)
    trickno += 1;
  for (vector<Trick*>::const_iterator
       trick = this->tricks().begin();
       ((trickno > 0)
        && (trick != this->tricks().end()));
       trick++, trickno--)
    if (((*trick)->winnerplayer() == player)
        && (*trick)->intrickpile())
      number += 1;

  return number;
} // unsigned Game::numberoftricks_of_player(Player const& player, unsigned trickno) const

/**
 ** -> result
 **
 ** @param       team    team
 **
 ** @return      number of tricks, the team has won so far
 **              (without the current trick)
 **
 ** @version     0.4.4
 **
 ** @author      Borg Enders
 ** @author      Diether Knof
 **/
unsigned
Game::numberoftricks_of_team(Team const& team) const
{
  unsigned number = 0;

  for (vector<Player*>::const_iterator player = this->players_begin();
       player != this->players_end();
       player++)
    if ((*player)->team() == team)
      number += this->numberoftricks_of_player(**player);

  return number;
} // unsigned Game::numberoftricks_of_team(Team const& team) const

/**
 **
 ** -> result
 **
 ** @param       player  player
 **
 ** @return      points, the player has got
 **
 ** @version     0.6.7
 **
 ** @author      Borg Enders
 ** @author      Diether Knof
 **
 */
unsigned
Game::points_of_player(Player const& player) const
{
  unsigned points = 0;

  vector<Trick*>::const_iterator trick;
  for (trick = this->tricks().begin();
       trick != this->tricks().end();
       trick++)
    if (((*trick)->winnerplayer() == player)
        && ((*trick)->intrickpile()))
      points += (*trick)->points();

  return points;
} // unsigned Game::points_of_player(Player const& player) const

/**
 ** -> result
 **
 ** @param       playerno   number of the player
 **
 ** @return      points, the player has got
 **
 ** @author      Diether Knof
 **
 ** @version     0.7.4
 **/
unsigned
Game::points_of_player(unsigned const playerno) const
{
  return this->points_of_player(this->player(playerno));
} // unsigned Game::points_of_player(unsigned playerno) const

/**
 **
 ** -> result
 **
 ** @param       player  player
 ** @param       trickno last trick to view
 **
 ** @return      points, the player has got til trickno
 **
 ** @version     0.6,7
 **
 ** @author      Borg Enders
 ** @author      Diether Knof
 **
 */
unsigned
Game::points_of_player(Player const& player, unsigned trickno) const
{
  unsigned points = 0;

  if (trickno != UINT_MAX)
    trickno += 1;
  for (vector<Trick*>::const_iterator
       trick = this->tricks().begin();
       ((trickno > 0)
        && (trick != this->tricks().end()));
       trick++, trickno--)
    if (((*trick)->winnerplayer() == player)
        && (*trick)->intrickpile())
      points += (*trick)->points();

  return points;
} // unsigned Game::points_of_player(Player const& player, unsigned trickno) const

/**
 ** -> result
 **
 ** @param       playerno   number of the player
 ** @param       trickno    last trick to view
 **
 ** @return      points, the player has got till trickno
 **
 ** @author      Diether Knof
 **
 ** @version     0.7.4
 **/
unsigned
Game::points_of_player(unsigned const playerno, unsigned const trickno) const
{
  return this->points_of_player(this->player(playerno), trickno);
} // unsigned Game::points_of_player(unsigned playerno, unsigned trickno) const

/**
 ** -> result
 ** uses hidden information
 **
 ** @param       team    team
 **
 ** @return      points, the team has got
 **
 ** @version     0.4.4
 **
 ** @author      Borg Enders
 ** @author      Diether Knof
 **
 **/
unsigned
Game::points_of_team(Team const& team) const
{
  unsigned points = 0;

  for (vector<Player*>::const_iterator player = this->players_begin();
       player != this->players_end();
       player++)
    if ((*player)->team() == team)
      points += this->points_of_player(**player);

  return points;
} // unsigned Game::points_of_team(Team const& team) const

/**
 ** -> result
 **
 ** @param       -
 **
 ** @return      the points played so far in the game
 **
 ** @author      Diether Knof
 **
 ** @version     0.7.10
 **/
unsigned
Game::played_points() const
{
  unsigned points = 0;

  for (vector<Player*>::const_iterator player = this->players_begin();
       player != this->players_end();
       player++)
    points += this->points_of_player(**player);
  if (::game_status < GAMESTATUS::GAME_FINISHED)
    points += this->trick_current().points();

  return points;
} // unsigned Game::played_points() const

/**
 ** -> result
 **
 ** @param       -
 **
 ** @return      the remaining points to be played
 **
 ** @author      Diether Knof
 **
 ** @version     0.7.10
 **/
unsigned
Game::remaining_points() const
{
  return (240 - this->played_points());
} // unsigned Game::remaining_points() const

/**
 ** -> result
 **
 ** @param       team   team to check
 **
 ** @return      the points needed for 'team' to win the game
 **
 ** @author      Diether Knof
 **
 ** @version     0.7.10
 **/
unsigned
Game::needed_points_to_win(Team const& team) const
{
  Announcement const highest_bid_re  
    = this->announcement_of_team(TEAM::RE);
  Announcement const highest_bid_contra
    = this->announcement_of_team(TEAM::CONTRA);

  switch (team) {
  case TEAM::RE:
    if (   (highest_bid_re == ANNOUNCEMENT::NOANNOUNCEMENT)
        && (highest_bid_contra == ANNOUNCEMENT::NOANNOUNCEMENT) )
      return 121;
    else if (highest_bid_re == ANNOUNCEMENT::NOANNOUNCEMENT)
      return point_limit_for_opposite_team(highest_bid_contra);
    else 
      return max(ANNOUNCEMENT::needed_points(highest_bid_re),
                      ANNOUNCEMENT::point_limit_for_opposite_team(highest_bid_contra));
  case TEAM::CONTRA:
    if (   (highest_bid_contra == ANNOUNCEMENT::NOANNOUNCEMENT)
        && (highest_bid_re == ANNOUNCEMENT::NOANNOUNCEMENT) )
      return 120;
    else if (   (highest_bid_contra == ANNOUNCEMENT::NO120)
             && (highest_bid_re == ANNOUNCEMENT::NO120) )
      return 120;
    else if (highest_bid_contra == ANNOUNCEMENT::NOANNOUNCEMENT)
      return point_limit_for_opposite_team(highest_bid_re);
    else 
      return max(ANNOUNCEMENT::needed_points(highest_bid_contra),
                      ANNOUNCEMENT::point_limit_for_opposite_team(highest_bid_re));
  default:
    return 0;
  } // switch(team)
} // unsigned Game::needed_points_to_win(Team const& team) const

/**
 ** -> result
 **
 ** @param       -
 **
 ** @return      the winner team
 **
 ** @author      Borg Enders
 ** @author      Diether Knof
 **
 ** @version     0.7.6
 **/
Team
Game::winnerteam() const
{
  DEBUG_ASSERTION((this->tricks_remaining_no() == 0),
                  "Game::winnerteam():\n"
                  "  game is not finished, yet\n"
                  << "current no: " << this->trick_current_no() << '\n'
                  << "remaining no: " << this->tricks_remaining_no() << '\n');

  // the points the team has made
  unsigned const points_re     = this->points_of_team(TEAM::RE);
  unsigned const points_contra = this->points_of_team(TEAM::CONTRA);

  // The official rules (http://www.doko-verband.de/download/turnierspielregeln.pdf)
  // distinguish between two kinds of announcements:
  // "ANsagen" (re and contra (both as opening and reply), let's call them "calls") and
  // "ABsagen" (no90, no60 and so on, let's call them "bids").
  // This distinction is important when deciding on the winner team, because bids determine how
  // many points a party needs to win, while all calls are ignored with one exception:
  // if a game ends 120-120 and the only announcement was a contra call, the contra team
  // will lose (it wins all other 120-120 situations unless it has made a bid).
  //
  // FreeDoko follows this distinction only if strict_limit is false, it treats all announcements
  // as bids if strict_limit is true. This has a number of consequences:
  // * instead of a single reply announcement (the REPLY call), there are several reply bids:
  //   NO120_REPLY (treated as a NO121 bid)
  //    NO90_REPLY (treated as a NO151 bid)
  //    NO60_REPLY (treated as a NO181 bid)
  //    NO30_REPLY (treated as a NO211 bid)
  //     NO0_REPLY (treated as a bid for at least one trick)
  // * if NO120_REPLY was the highest bid of the re team (which implies a contra announcement),
  //   the re team will win a 120-120 game because it fulfilled its bid
  // * if both teams announced NO120 and the game ends 120-120, there will be no winner
  // * there'll be many more games without a winner because an opening re (or contra) will
  //   stand as a NO120 bid until the end. This way, even 120 points against a NO0 bid may
  //   not be sufficient to win the game.

  Announcement highest_bid_re     = this->announcement_of_team(TEAM::RE);
  Announcement highest_bid_contra = this->announcement_of_team(TEAM::CONTRA);

  // a re/contra (without strict limit) is seen as no announcement
  // a reply can only be made, when the other team has made an announcement
  if (!this->rule()(Rule::ANNOUNCEMENT_STRICT_LIMIT)) {
    // special case: only contra has announced 'contra'
    if (   (highest_bid_contra == ANNOUNCEMENT::NO120)
        && (highest_bid_re == ANNOUNCEMENT::NOANNOUNCEMENT) )
      if (this->points_of_team(TEAM::RE) >= 120)
        return TEAM::RE;
      else
        return TEAM::CONTRA;

    if (   (highest_bid_re == ANNOUNCEMENT::NO120)
        || (highest_bid_re == ANNOUNCEMENT::REPLY) )
      highest_bid_re = ANNOUNCEMENT::NOANNOUNCEMENT;
    if (   (highest_bid_contra == ANNOUNCEMENT::NO120)
        || (highest_bid_contra == ANNOUNCEMENT::REPLY) )
      highest_bid_contra = ANNOUNCEMENT::NOANNOUNCEMENT;
  } // if (!this->rule()(Rule::ANNOUNCEMENT_STRICT_LIMIT))

  // Now determine the winning team

  // Case #1: no party made a bid
  // * there's always a winner
  // * if no call was made or both teams made calls or only the re team made a call:
  // * Re-Team wins with 121 or more points [according to official rules article 7.1.2,1-3]
  // * Contra-Team wins with 120 or more points [7.1.3,1-3]
  // * if only the contra team made a call:
  // * Re-Team wins with 120 ore more points [7.1.2,4] (see above)
  // * Contra-Team wins with 121 or more points [7.1.3,4] (see above)

  if (   (highest_bid_re == ANNOUNCEMENT::NOANNOUNCEMENT)
      && (highest_bid_contra == ANNOUNCEMENT::NOANNOUNCEMENT) ) {
    if (points_re > 120)
      return TEAM::RE;
    else
      return TEAM::CONTRA;
  }

  // Case #2: at least one of the teams made a bid
  // * both teams are allowed to make bids but it's only possible for one team to fulfill its bid =>
  // * Re-Team wins if it fulfills its bid [7.1.2,5-6]
  // * Contra-Team wins if it fulfills its bid [7.1.3,5-6]

  // check if re fulfilled its bid
  if (   (   (highest_bid_re == ANNOUNCEMENT::NO120)  // call gets treated as bid if strict_limit is true
          && (points_contra < 120) )
      || (   (highest_bid_re == ANNOUNCEMENT::NO90)
          && (points_contra < 90 ) )
      || (   (highest_bid_re == ANNOUNCEMENT::NO60)
          && (points_contra < 60 ) )
      || (   (highest_bid_re == ANNOUNCEMENT::NO30)
          && (points_contra < 30 ) )
      || (   (highest_bid_re == ANNOUNCEMENT::NO0)
          && !this->hastrick( TEAM::CONTRA ) ) ) {
    return TEAM::RE;
  }

  // these bids are only possible if strict_limit is true
  if (   (   (highest_bid_re == ANNOUNCEMENT::NO120_REPLY)
          && (points_re >= 120 ))
      || (   (highest_bid_re == ANNOUNCEMENT::NO90_REPLY)
          && (points_re >= 90 ))
      || (   (highest_bid_re == ANNOUNCEMENT::NO60_REPLY)
          && (points_re >= 60 ))
      || (   (highest_bid_re == ANNOUNCEMENT::NO30_REPLY)
          && (points_re >= 30 ))
      || (   (highest_bid_re == ANNOUNCEMENT::NO0_REPLY)
          && this->hastrick(TEAM::RE) ) ) {
    return TEAM::RE;
  }

  // check if contra fulfilled its bid
  if (   (  (highest_bid_contra == ANNOUNCEMENT::NO120)  // call gets treated as bid if strict_limit is true
          && (points_re < 120 ))
      || (   (highest_bid_contra == ANNOUNCEMENT::NO90)
          && (points_re < 90 ))
      || (   (highest_bid_contra == ANNOUNCEMENT::NO60)
          && (points_re < 60 ))
      || (   (highest_bid_contra == ANNOUNCEMENT::NO30)
          && (points_re < 30 ))
      || (   (highest_bid_contra == ANNOUNCEMENT::NO0)
          && !this->hastrick( TEAM::RE ) ) ) {
    return TEAM::CONTRA;
  }

  // these bids are only possible if strict_limit is true
  if (   (   (highest_bid_contra == ANNOUNCEMENT::NO120_REPLY)
          && (points_contra >= 120 ))
      || (   (highest_bid_contra == ANNOUNCEMENT::NO90_REPLY)
          && (points_contra >= 90 ))
      || (   (highest_bid_contra == ANNOUNCEMENT::NO60_REPLY)
          && (points_contra >= 60 ))
      || (   (highest_bid_contra == ANNOUNCEMENT::NO30_REPLY)
          && (points_contra >= 30 ))
      || (   (highest_bid_contra == ANNOUNCEMENT::NO0_REPLY)
          && this->hastrick( TEAM::CONTRA ) ) ) {
    return TEAM::CONTRA;
  }

  // Case #3: at least one bid has been made, but no bid has been fulfilled
  // the case that no team has made an annoucement / bid is Case #1,
  // so here there exists at least one team with a bid
  // * Re-Team wins if it didn't make a bid [7.1.2,7-8]
  // * Contra-Team wins if it didn't make a bid [7.1.3,7-8]
  // * no winner if both teams made bids [7.1.4]

  if (!highest_bid_re)
    // contra has not fulfilled its announcement
    return TEAM::RE;
  else if (!highest_bid_contra)
    // re has not fulfilled its announcement
    return TEAM::CONTRA;
  else
    // no player has made enough points for his announcement
    return TEAM::NOTEAM;
} // Team Game::winnerteam() const
