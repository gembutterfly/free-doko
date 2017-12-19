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
#include "hand_cards.h"

#include "../player/player.h"
#include "../game/game.h"
#include "../party/rule.h"

/**
 **
 ** constructor
 **
 ** @param	cards	cards of the hand
 **
 ** @return	-
 **
 ** @version	0.6.8
 **
 ** @author	Diether Knof
 **
 **/
HandCards::HandCards(vector<HandCard> const& cards) :
  vector<HandCard>(cards)
{
#ifndef DEBUG_NO_ASSERTS
  // test whether all cards are from the same player
  Player const* const player = &cards[0].player();
  for (HandCards::const_iterator c = this->begin();
       c != this->end();
       ++c)
    DEBUG_ASSERTION((player == &c->player()),
		    "HandCards::HandCards(cards):\n"
		    "  the cards belong to different players: "
		    << player->game().no(*player) << " and " << player->game().no(c->player()));
#endif
  return ;
} // HandCards::HandCards(vector<HandCard> const& cards)

/**
 ** constructor
 **
 ** @param	cards	cards of the hand
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.3
 **/
HandCards::HandCards(vector<Card> const& cards) :
  vector<HandCard>()
{
  for (vector<Card>::const_iterator c = cards.begin();
       c != cards.end();
       ++c)
    this->push_back(HandCard(*c));
  return ;
} // HandCards::HandCards(vector<Card> cards)

/**
 ** constructor
 **
 ** @param     cards   cards of the hand
 ** @param     hand    corresponding hand
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7,3
 **/
HandCards::HandCards(Hand const& hand,
		     vector<Card> const& cards) :
  vector<HandCard>(cards.size())
{
  for (unsigned i = 0; i < cards.size(); ++i)
    (*this)[i] = HandCard(hand, cards[i]);

  return ;
} // HandCards::HandCards(vector<HandCard> const& cards)

/**
 **
 ** constructor
 **
 ** @param	n	number of cards
 **
 ** @return	-
 **
 ** @version	0.6.8
 **
 ** @author	Diether Knof
 **
 **/
HandCards::HandCards(unsigned const n) :
  vector<HandCard>(n)
{ }

/**
 **
 ** reads the cards from the input stream
 **
 ** @param	istr	input stream
 **
 ** @return	-
 **
 ** @version	0.6.8
 **
 ** @author	Diether Knof
 **
 **/
HandCards::HandCards(istream& istr) :
  vector<HandCard>()
{
  while (istr.good()) {
#ifndef OUTDATED
    // DK: 0.6.8
    while (std::isdigit(istr.peek()))
      istr.get();
#endif
    string line;
    std::getline(istr, line);
    if (!line.empty() && (*line.rbegin() == '\r'))
      line.erase(line.end() - 1);
    if (line.empty())
      break;

    this->push_back(HandCard(Card(line)));
  } // while (this->istr->good())
} // HandCards::HandCards(istream& istr)

/**
 **
 ** constructor
 **
 ** @param	-
 **
 ** @return	-
 **
 ** @version	0.6.8
 **
 ** @author	Diether Knof
 **
 **/
HandCards::HandCards() :
  vector<HandCard>()
{ }

/**
 **
 ** copy constructor
 **
 ** @param	cards	cards to copy
 **
 ** @return	-
 **
 ** @version	0.6.8
 **
 ** @author	Diether Knof
 **
 **/
HandCards::HandCards(HandCards const& cards) :
  vector<HandCard>(cards)
{ }

/**
 **
 ** copy operator
 **
 ** @param	cards	cards to copy
 **
 ** @return	this cards
 **
 ** @version	0.6.8
 **
 ** @author	Diether Knof
 **
 **/
HandCards&
HandCards::operator=(HandCards const& cards)
{
  static_cast<vector<HandCard>&>(*this) = cards;

  return *this;
} // HandCards::HandCards(HandCards const& cards) :

/**
 **
 ** destructor
 **
 ** @param	-
 **
 ** @return	-
 **
 ** @version	0.6.8
 **
 ** @author	Diether Knof
 **
 **/
HandCards::~HandCards()
{
} // HandCards::~Hand()

/**
 **
 ** converts the vector in a vector of simple cards
 **
 ** @param	-
 **
 ** @return	this cards
 **
 ** @version	0.6.8
 **
 ** @author	Diether Knof
 **
 **/
HandCards::operator vector<Card>() const
{
  vector<Card> cards;

  for (HandCards::const_iterator c = this->begin();
       c != this->end();
       ++c)
    cards.push_back(*c);

  return cards;
} // HandCards::vector<Card>() const

/**
 **
 ** -> result
 **
 ** @param	-
 **
 ** @return	number of cards
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.8
 **
 **/
unsigned
HandCards::cardsnumber() const
{
  return this->size();
} // unsigned HandCards::cardsnumber() const

/**
 **
 ** -> result
 **
 ** @param	i	number of card
 **
 ** @return	card i of the hand (with remaining cards)
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 **
 ** @version	0.6.8
 **
 **/
HandCard const&
HandCards::card(unsigned const i) const
{
  DEBUG_ASSERTION((i < this->size()),
		  "HandCards::card(i):\n"
		  "  invalid value 'i' = " << i
		  << " (max = " << this->size() << ")\n"
		  "HandCards: "
		  << *this);

  return (*this)[i];
} // HandCard HandCards::card(unsigned i) const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	the player this hand belongs to
 **/
Player const&
HandCards::player() const
{
  return (*this)[0].player();
} // Player HandCards::player() const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	the corresponding game
 **/
Game const&
HandCards::game() const
{
  return this->player().game();
} // Game HandCards::game() const

/**
 ** add the card
 **
 ** @param	card   card to add
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.9
 **/
void
HandCards::add(HandCard const& card)
{
  this->push_back(card);
  return ;
} // void HandCards::add(HandCard card)

/**
 ** add the cards
 **
 ** @param	cards   cards to add
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.9
 **/
void
HandCards::add(HandCards const& cards)
{
  this->insert(this->end(), cards.begin(), cards.end());
  return ;
} // void HandCards::add(HandCards cards)

/**
 ** removes 'card' from the vector
 **
 ** @param	card	card to remove
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.12
 **/
void
HandCards::remove(Card const& card)
{
  HandCards::iterator d;
  for (d = this->begin();
       card != *d;
       ++d) {
    DEBUG_ASSERTION((d != this->end()),
                    "HandCards::remove(card):\n"
                    "  card '" << card << "' not found in the cards.\n"
                    << *this);
  }
  this->erase(d);

  return ;
} // void HandCards::remove(Card card)

/**
 ** removes 'cards' from the vector
 **
 ** @param	cards	cards to remove
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.8
 **/
void
HandCards::remove(HandCards const& cards)
{
  for (HandCards::const_iterator c = cards.begin();
       c != cards.end();
       ++c) {
    HandCards::iterator d;
    for (d = this->begin();
         *c != *d;
         ++d) {
      DEBUG_ASSERTION((d != this->end()),
                      "HandCards::remove(cards):\n"
                      "  card '" << *c << "' not found in the cards.\n"
                      << *this);
    }
    this->erase(d);
  } // for (c \in cards)

  return ;
} // void HandCards::remove(HandCards cards)

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	the highest value of the cards
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
Card::Value
HandCards::highest_value() const
{
  if (this->cardsnumber() == 0)
    return Card::NOCARDVALUE;

  Card::Value value = this->begin()->value();
  for (HandCards::const_iterator c = this->begin() + 1;
       c != this->end();
       ++c) {
    if (c->value() > value)
      value = c->value();
  } // for (c)

  return value;
} // Card::Value HandCards::highest_value() const

/**
 ** -> result
 **
 ** @param	tcolor   tcolor to check
 **
 ** @return	the highest value of the given tcolor
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
Card::Value
HandCards::highest_value(Card::TColor const& tcolor) const
{
  if (this->cardsnumber() == 0)
    return Card::NOCARDVALUE;

  Card::Value value = Card::NINE;
  for (HandCards::const_iterator c = this->begin();
       c != this->end();
       ++c) {
    if (   (c->value() > value)
        && (c->tcolor() == tcolor) )
      value = c->value();
  } // for (c)

  return value;
} // Card::Value HandCards::highest_value(Card::TColor tcolor) const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	the lowest value of the cards
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
Card::Value
HandCards::lowest_value() const
{
  if (this->empty())
    return Card::NOCARDVALUE;

  Card::Value value = this->begin()->value();
  for (HandCards::const_iterator c = this->begin() + 1;
       c != this->end();
       ++c) {
    if (c->value() < value)
      value = c->value();
  } // for (c)

  return value;
} // Card::Value HandCards::lowest_value() const

/**
 ** -> result
 **
 ** @param	tcolor   tcolor to check
 **
 ** @return	the lowest value of the given tcolor
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
Card::Value
HandCards::lowest_value(Card::TColor const& tcolor) const
{
  if (this->empty())
    return Card::NOCARDVALUE;

  Card::Value value = Card::ACE;
  for (HandCards::const_iterator c = this->begin();
       c != this->end();
       ++c) {
    if (   (c->value() < value)
        && (c->tcolor() == tcolor) )
      value = c->value();
  } // for (c)

  return value;
} // Card::Value HandCards::lowest_value(Card::TColor tcolor) const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	the highest card of the cards
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.5
 **/
HandCard const&
HandCards::highest_card() const
{
  if (this->empty())
    return HandCard::EMPTY;

  HandCards::const_iterator highest_card = this->begin();

  for (HandCards::const_iterator c = this->begin() + 1;
       c != this->end();
       ++c) {
    if (highest_card->less(*c))
      highest_card = c;
  } // for (c)

  return *highest_card;
} // HandCard HandCards::highest_card() const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	the highest trump of the cards
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.5
 **/
HandCard const&
HandCards::highest_trump() const
{
  return this->highest_card(Card::TRUMP);
} // HandCard HandCards::highest_trump() const

/**
 ** -> result
 **
 ** @param	card_limit   card limit
 **
 ** @return	the highest trump till the card limit
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.8
 **/
HandCard const&
HandCards::highest_trump_till(Card const& card_limit) const
{
  return this->highest_card_till(Card::TRUMP, card_limit);
} // HandCard HandCards::highest_trump_till(Card card_limit) const

/**
 ** -> result
 **
 ** @param	tcolor   tcolor to check the cards of
 **
 ** @return	the highest card of the given tcolor
 **		(Card() if no card of the color is present)
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.2
 **/
HandCard const&
HandCards::highest_card(Card::TColor const& tcolor) const
{
  if (this->numberof(tcolor) == 0)
    return HandCard::EMPTY;

  HandCards::const_iterator highest_card = this->begin();
  for (highest_card = this->begin();
       highest_card != this->end();
       ++highest_card)
    if (highest_card->tcolor() == tcolor)
      break;

  if (highest_card == this->end())
    return HandCard::EMPTY;

  for (HandCards::const_iterator c = highest_card + 1;
       c != this->end();
       ++c) {
    if (   (c->tcolor() == tcolor)
        && (highest_card->less(*c)) )
      highest_card = c;
  } // for (c)

  return *highest_card;
} // HandCard HandCards::highest_card(Card::TColor tcolor) const

/**
 ** -> result
 **
 ** @param	tcolor       tcolor
 ** @param	card_limit   card limit
 **
 ** @return	the highest card of the given tcolor equal/less the card limit
 **		(Card() if no card is found)
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.8
 **/
HandCard const&
HandCards::highest_card_till(Card::TColor const& tcolor,
                             Card const& card_limit) const
{
  if (this->numberof(tcolor) == 0)
    return HandCard::EMPTY;

  HandCards::const_iterator highest_card = this->begin();
  for (highest_card = this->begin();
       highest_card != this->end();
       ++highest_card)
    if (   (highest_card->tcolor() == tcolor)
        && (   (*highest_card == card_limit)
            || highest_card->less(card_limit)) )
      break;

  if (highest_card == this->end())
    return HandCard::EMPTY;

  for (HandCards::const_iterator c = highest_card + 1;
       c != this->end();
       ++c) {
    if (   (c->tcolor() == tcolor)
        && (highest_card->less(*c))
        && (   (*c == card_limit)
            || c->less(card_limit)) )
      highest_card = c;
  } // for (c)

  return *highest_card;
} // HandCard HandCards::highest_card_till(Card::TColor tcolor, Card card_limit) const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	the lowest trump of the cards
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.5
 **/
HandCard const&
HandCards::lowest_trump() const
{
  return this->lowest_card(Card::TRUMP);
} // HandCard HandCards::lowest_trump() const

/**
 ** -> result
 **
 ** @param	tcolor   tcolor to check the cards of
 **
 ** @return	the lowest card of the given tcolor
 **		(Card() if no card of the tcolor is present)
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.5
 **/
HandCard const&
HandCards::lowest_card(Card::TColor const& tcolor) const
{
  if (this->numberof(tcolor) == 0)
    return HandCard::EMPTY;

  HandCards::const_iterator lowest_card = this->begin();
  for (lowest_card = this->begin();
       lowest_card != this->end();
       ++lowest_card)
    if (lowest_card->tcolor() == tcolor)
      break;

  if (lowest_card == this->end())
    return HandCard::EMPTY;

  for (HandCards::const_iterator c = lowest_card + 1;
       c != this->end();
       ++c) {
    if (   (c->tcolor() == tcolor)
        && (c->less(*lowest_card)) )
      lowest_card = c;
  } // for (c)

  return *lowest_card;
} // HandCard HandCards::lowest_card(Card::TColor tcolor) const

/**
 ** -> result
 **
 ** @param	card   card to compare with
 **
 ** @return	whether there exists a higher card than 'card' on the hand
 **/
bool
HandCards::higher_card_exists(Card const& card) const
{
  for (auto& c : *this)
    if (card.less(c))
      return true;

  return false;
} // bool HandCards::higher_card_exists(Card card) const

/**
 ** -> result
 **
 ** @param	card   card to compare with
 **
 ** @return	whether there exists a lower card than 'card' on the hand of the same color
 **/
bool
HandCards::lower_card_exists(Card const& card) const
{
  for (auto& c : *this)
    if (   (c.tcolor() == card.tcolor(this->game()))
        && c.less(card))
      return true;

  return false;
} // bool HandCards::lower_card_exists(Card card) const

/**
 ** -> result
 **
 ** @param	card   card to compare with
 **
 ** @return	the number of higher cards then 'card'
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.8
 **/
unsigned
HandCards::higher_cards_no(Card const& card) const
{
  unsigned n = 0;

  for (HandCards::const_iterator c = this->begin();
       c != this->end();
       ++c)
    if (card.less(*c))
      n += 1;

  return n;
} // unsigned HandCards::higher_cards_no(Card card) const

/**
 ** -> result
 **
 ** @param	card   card to compare with
 **
 ** @return	the next jabbing card of 'card', EMPTY if no such exists
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.11
 **/
HandCard const&
HandCards::next_jabbing_card(Card const& card) const
{
  if (this->game().less(card, card))
    return this->same_or_higher_card(card);
  else
    return this->next_higher_card(card);
} // HandCard HandCards::next_jabbing_card(Card card) const

/**
 ** -> result
 **
 ** @param	card   card to compare with
 **
 ** @return	the next higher card then 'card', EMPTY if no such exists
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.9
 **/
HandCard const&
HandCards::next_higher_card(Card const& card) const
{
  HandCard const* higher_card = NULL;
  for (HandCards::const_iterator c = this->begin();
       c != this->end();
       ++c) {
    if (   card.less(*c)
        && (*c != card)
        && (   (higher_card == NULL)
            || c->less(*higher_card)) )
      higher_card = &*c;
  } // for (c)

  if (higher_card == NULL)
    return HandCard::EMPTY;
  return *higher_card;
} // HandCard HandCards::next_higher_card(Card card) const

/**
 ** -> result
 **
 ** @param	card   card to compare with
 **
 ** @return	'card' or the next higher card then 'card', EMPTY if no such exists
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.11
 **/
HandCard const&
HandCards::same_or_higher_card(Card const& card) const
{
  for (HandCards::const_iterator c = this->begin();
       c != this->end();
       ++c)
    if (*c == card)
      return *c;

  return this->next_higher_card(card);
} // HandCard HandCards::same_or_higher_card(Card card) const

/**
 ** -> result
 **
 ** @param	card   card to compare with
 **
 ** @return	the next lower card then 'card' (in the same color), EMPTY if no such exists
 **/
HandCard const&
HandCards::next_lower_card(Card const& card) const
{
  HandCard const* lower_card = nullptr;
  auto const tcolor = card.tcolor(this->game());
  for (auto const& c : *this) {
    if (   c.less(card)
        && (c != card)
        && (c.tcolor() == tcolor)
        && (   (lower_card == NULL)
            || lower_card->less(c)) )
      lower_card = &c;
  } // for (c)

  if (lower_card == NULL)
    return HandCard::EMPTY;
  return *lower_card;
} // HandCard HandCards::next_lower_card(Card card) const

/**
 ** -> result
 **
 ** @param	card   card to compare with
 **
 ** @return	'card' or the next lower card then 'card', EMPTY if no such exists
 **/
HandCard const&
HandCards::same_or_lower_card(Card const& card) const
{
  for (auto const& c : *this)
    if (c == card)
      return c;

  return this->next_lower_card(card);
} // HandCard HandCards::same_or_lower_card(Card card) const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	whether there is a trump still on the hand
 **/
bool
HandCards::hastrump() const
{
  for (auto const& c : *this)
    if (c.istrump())
      return true;

  return false;
} // bool HandCards::hastrump()

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	whether there is a color card still on the hand
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.5
 **/
bool
HandCards::hascolor() const
{
  for (HandCards::const_iterator c = this->begin();
       c != this->end();
       ++c)
    if (!c->istrump())
      return true;

  return false;
} // bool HandCards::hascolor()

/**
 **
 ** -> result
 **
 ** @param	value	value to count
 **
 ** @return	number of cards with the value 'value'
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 **
 ** @version	0.6.8
 **
 **/
unsigned
HandCards::numberof(Card::Value const& value) const
{
  unsigned number = 0;

  for (HandCards::const_iterator c = this->begin();
       c != this->end();
       ++c)
    if (c->value() == value)
      number += 1;

  return number;
} // unsigned HandCards::numberof(Card::Value const& value)

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	number of cards with value 10 or 11
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.12
 **/
unsigned
HandCards::numberofrichcards() const
{
  unsigned number = 0;

  for (HandCards::const_iterator c = this->begin();
       c != this->end();
       ++c)
    if (c->value() >= 10)
      number += 1;

  return number;
} // unsigned HandCards::numberofrichcards() const

/**
 **
 ** -> result
 **
 ** @param	tcolor	trump-color to count
 **
 ** @return	number of cards with the trump-color 'tcolor'
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 **
 ** @version	0.6.8
 **
 **/
unsigned
HandCards::numberof(Card::TColor const& tcolor) const
{
  unsigned number = 0;

  for (HandCards::const_iterator c = this->begin();
       c != this->end();
       ++c) {
    if (c->tcolor() == tcolor) {
      number += 1;
    }
  }

  return number;
} // unsigned HandCards::numberof(Card::TColor const& tcolor)

/**
 **
 ** -> result
 **
 ** @param	card	card to count
 **
 ** @return	number of cards 'card'
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 **
 ** @version	0.6.8
 **
 **/
unsigned
HandCards::numberof(Card const& card) const
{
  unsigned number = 0;

  for (HandCards::const_iterator c = this->begin();
       c != this->end();
       ++c)
    if (*c == card)
      number += 1;

  return number;
} // unsigned HandCards::numberof(Card const& card) const

/**
 **
 ** -> result
 ** Note: 'HandCards::numberof(Card)' does not cover the case (TRUMP, ACE)
 **
 ** @param	tcolor	trump-color of the card
 ** @param	value	value of the card
 **
 ** @return	number of cards with trump-color 'tcolor' and value 'value'
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 **
 ** @version	0.6.8
 **
 **/
unsigned
HandCards::numberof(Card::TColor const& tcolor, Card::Value const& value) const
{
  if (this->empty())
    return 0;

  if (tcolor == this->game().trumpcolor())
    return this->numberof(Card(tcolor, value));

  unsigned number = 0;

  for (HandCards::const_iterator c = this->begin();
       c != this->end();
       ++c) {
    if ( (c->tcolor() == tcolor)
        && (c->value() == value) ) {
      number += 1;
    }
  }

  return number;
} // unsigned HandCards::numberof(Card::TColor, Card::Value)

/**
 **
 ** -> result
 ** Note: 'HandCards::numberof(Card)' does not cover the case (TRUMP, ACE)
 **
 ** @param	color	color to count
 ** @param	game	game
 **
 ** @return	number of cards with color 'color' given the gametype 'gt'
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.8
 **
 **/
unsigned
HandCards::numberof(Card::Color const& color, Game const& game) const
{
  return this->numberof(color, game.type(), game.rule()(Rule::DOLLEN));
} // unsigned HandCards::numberof(Card::Color color, Game game) const


/**
 ** -> result
 ** 
 ** @param	card   card to search
 ** 
 ** @return	whether 'card' is contained in 'this'
 ** 
 ** @author	Diether Knof 
 ** 
 ** @version	0.7.4
 **/
bool
HandCards::contains(Card const& card) const
{
  return (this->numberof(card) > 0);
} // bool HandCards::contains(Card card) const

/**
 ** -> result
 ** 
 ** @param	color   card color
 ** @param	value   card vallue
 ** 
 ** @return	whether 'card' is contained in 'this'
 ** 
 ** @author	Diether Knof 
 ** 
 ** @version	0.7.4
 **/
bool
HandCards::contains(Card::Color const& color,
                    Card::Value const& value) const
{
  return this->contains(Card(color, value));
} // bool HandCards::contains(Card::Color color, Card::Value value) const

/**
 **
 ** -> result
 **
 ** @param	tcolor	trump-color to search for
 **
 ** @return	whether there is a card with trump-color 'tcolor' on the hand
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 **
 ** @version	0.6.8
 **
 **/
bool
HandCards::contains(Card::TColor const& tcolor) const
{
  for (HandCards::const_iterator c = this->begin();
       c != this->end();
       ++c)
    if (c->tcolor() == tcolor) {
      return true;
    }

  return false;
} // bool HandCards::contains(Card::TColor const& tcolor)

/**
 **
 ** -> result
 ** Note: 'HandCards::numberof(Card)' does not cover the case (TRUMP, ACE)
 **
 ** @param	color		color to count
 ** @param	gametype	the gametype
 ** @param	dollen		whether dollen are allowed
 **
 ** @return	number of cards with color 'color' given the gametype and the dollen rule
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 **
 ** @version	0.6.8
 **
 **/
unsigned
HandCards::numberof(Card::Color const& color,
                    GameType const gametype, bool const dollen) const
{
  unsigned number = 0;

  for (HandCards::const_iterator c = this->begin();
       c != this->end();
       ++c)
    if ((c->color() == color)
        && !c->Card::istrump(gametype, dollen))
      number += 1;

  return number;
} // unsigned HandCards::numberof(Card::Color color, GameType gametype, bool dollen) const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	number of trumps on the hand
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.8
 **/
unsigned
HandCards::numberoftrumps() const
{
  return this->numberof(Card::TRUMP);
} // unsigned HandCards::numberoftrumps() const

/**
 ** -> result
 **
 ** @param	game_type   the game type
 **
 ** @return	number of trumps on the hand for the game type 'game_type'
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.8
 **/
unsigned
HandCards::numberoftrumps(GameType const game_type) const
{
  unsigned number = 0;

  for (HandCards::const_iterator c = this->begin();
       c != this->end();
       ++c)
    if (c->Card::istrump(game_type, this->game().rule()(Rule::DOLLEN)))
      number += 1;

  return number;
} // unsigned HandCards::numberoftrumps(GameType game_type) const

/**
 **
 ** -> result
 **
 ** @param	-
 **
 ** @return	number of trump aces on the hand
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.8
 **
 **/
unsigned
HandCards::numberoftrumpaces() const
{
  return this->numberof(Card::TRUMP, Card::ACE);
} // unsigned HandCards::numberoftrumpaces() const

/**
 **
 ** -> result
 **
 ** @param	game_type   game type
 **
 ** @return	number of trump aces on the hand
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.14
 **
 **/
unsigned
HandCards::numberoftrumpaces(GameType const game_type) const
{
  unsigned number = 0;

  for (HandCards::const_iterator c = this->begin();
       c != this->end();
       ++c)
    if (   c->Card::istrump(game_type, this->game().rule()(Rule::DOLLEN))
        && (c->value() == Card::ACE) )
      number += 1;

  return number;
} // unsigned HandCards::numberoftrumpaces(GameType game_type) const

/**
 **
 ** -> result
 **
 ** @param	-
 **
 ** @return	number of trump kings on the hand
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.8
 **
 **/
unsigned
HandCards::numberoftrumpkings() const
{
  return this->numberof(Card::TRUMP, Card::KING);
} // unsigned HandCards::numberoftrumpkings() const

/**
 **
 ** -> result
 **
 ** @param	-
 **
 ** @return	number of nines on the hand
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.8
 **
 **/
unsigned
HandCards::numberofnines() const
{
  return this->numberof(Card::NINE);
} // unsigned HandCards::numbernines()

/**
 **
 ** -> result
 **
 ** @param	-
 **
 ** @return	number of kings on the hand
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.8
 **
 **/
unsigned
HandCards::numberofkings() const
{
  return this->numberof(Card::KING);
} // unsigned HandCards::numberkings()

/**
 **
 ** -> result
 **
 ** @param	-
 **
 ** @return	number of club queens on the hand
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.8
 **
 **/
unsigned
HandCards::numberofclubqueens() const
{
  return this->numberof(Card::CLUB_QUEEN);
} // unsigned HandCards::numberofclubqueens()

/**
 **
 ** -> result
 **
 ** @param	-
 **
 ** @return	number of trump nines on the hand
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.8
 **
 **/
unsigned
HandCards::numberoftrumpnines() const
{
  return this->numberof(Card::TRUMP, Card::NINE);
} // unsigned HandCards::numberoftrumpnines() const

/**
 ** -> result
 **
 ** @param	cards	cards to compare with
 ** 
 ** @return	whether 'cards' is equal to 'this' (without order)
 **/
bool
HandCards::equal(HandCards const& cards) const
{
  for (auto const& c : cards)
    if (this->numberof(c) != cards.numberof(c))
      return false;

  return true;
} // bool HandCards::equal(HandCards cards) const

/**
 ** write the hand on 'ostr'
 **
 ** @param	ostr	stream to write the hand to
 ** @param	cards	cards to write
 **
 ** @return	output stream
 **/
ostream&
operator<<(ostream& ostr, HandCards const& cards)
{
  for (auto const& c : cards)
    ostr << c << '\n';

  return ostr;
} // ostream& operator<<(ostream&, HandCards)
