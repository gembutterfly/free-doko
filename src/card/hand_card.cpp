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
#include "hand_card.h"

#include "trick.h"
#include "../player/player.h"
#include "../game/game.h"
#include "../party/rule.h"
#include "../party/party.h"
#include "../misc/setting.h"

// an empty hand card
HandCard const HandCard::EMPTY;

/**
 ** constructor
 **
 ** @param	-
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.8
 **/
HandCard::HandCard() :
  Card(),
  hand_(NULL)
{ }

/**
 ** constructor
 **
 ** @param	hand	hand this card belongs to
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.8
 **/
HandCard::HandCard(Hand const& hand) :
  Card(),
  hand_(&hand)
{ }

/**
 ** constructor
 **
 ** @param	card	card (without a hand)
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.8
 **/
HandCard::HandCard(Card const& card) :
  Card(card),
  hand_(NULL)
{ }

/**
 ** constructor
 **
 ** @param	hand	hand this card belongs to
 ** @param	card	card
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.8
 **/
HandCard::HandCard(Hand const& hand, Card const& card) :
  Card(card),
  hand_(&hand)
{ }

/**
 ** constructor
 **
 ** @param	hand	hand this card belongs to
 ** @param	color	color of the card
 ** @param	value	value of the card
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.8
 **/
HandCard::HandCard(Hand const& hand,
		   Card::Color const color,
		   Card::Value const value) :
  Card(color, value),
  hand_(&hand)
{ }

/**
 ** copy constructor
 **
 ** @param	card	card to copy
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.8
 **/
HandCard::HandCard(HandCard const& card) :
  Card(card),
  hand_(&card.hand())
{ }

/**
 ** copy operator
 **
 ** @param	card	card to copy
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.8
 **/
HandCard&
HandCard::operator=(HandCard const& card)
{
  static_cast<Card&>(*this) = card;
  this->hand_ = &card.hand();

  return *this;
} // HandCard::HandCard(HandCard card)

/**
 ** copy operator
 ** keeps the hand
 **
 ** @param	card	card to copy
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.8
 **/
HandCard&
HandCard::operator=(Card const& card)
{
  static_cast<Card&>(*this) = card;

  return *this;
} // HandCard::HandCard(Card card)

/**
 ** destructor
 **
 ** @param	-
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.8
 **/
HandCard::~HandCard()
{
} // HandCard::~HandCard()

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	the player this card belongs to
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.8
 **/
Player const&
HandCard::player() const
{
  return this->hand().player();
} // Player HandCard::player() const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	the corresponding game
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.8
 **/
Game const&
HandCard::game() const
{
  return this->player().game();
} // Game HandCard::game() const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	'TRUMP' is the card is a trump,
 **		else the color
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.8
 **/
Card::TColor
HandCard::tcolor() const
{
  return (this->istrump()
	  ? Card::TRUMP
	  : this->color());
} // Card::TColor HandCard::tcolor() const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	true if card is trump
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.8
 **/
bool
HandCard::istrump() const
{
  return this->Card::istrump(this->game());
} // bool HandCard::istrump() const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	true if card is a dolle
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.8
 **/
bool
HandCard::isdolle() const
{
  return this->Card::isdolle(this->game());
} // bool HandCard::isdolle() const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	true if card is a swine
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.8
 **/
bool
HandCard::isswine() const
{
  return (   this->istrumpace()
          && (this->game().swines_announced()));
} // bool HandCard::isswine()

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	true if card is a hyperswine
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.8
 **/
bool
HandCard::ishyperswine() const
{
  if (this->game().rule()(Rule::WITH_NINES))
    return (   this->istrumpnine()
            && (this->game().hyperswines_announced()));
  else
    return (   this->istrumpking()
            && (this->game().hyperswines_announced()));
} // bool HandCard::ishyperswine()

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	whether this card is or can be a (high) special card (dolle, swine, hyperswine)
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
bool
HandCard::is_special() const
{
  return (   this->isdolle()
          || this->isswine()
          || this->possible_swine()
          || this->ishyperswine()
          || this->possible_hyperswine()
          );
} // bool HandCard::is_special() const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	true if card can be a genscher card
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.8
 **/
bool
HandCard::possible_genscher() const
{
  return (!this->is_empty()
          && (   this->hand().has_possible_genscher()
              && this->istrumpking()) );
} // bool HandCard::possible_genscher()

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	true if the card can be or is a swine
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.8
 **/
bool
HandCard::possible_swine() const
{
  return (!this->is_empty()
          && (this->isswine()
              || (   this->hand().has_swines()
                  && this->istrumpace())) );
} // bool HandCard::possible_swine()

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	true if the card can be or is a hyperswine
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.8
 **/
bool
HandCard::possible_hyperswine() const
{
  return (!this->is_empty()
          && (this->ishyperswine()
              || (this->hand().has_hyperswines()
                  && (this->game().rule()(Rule::WITH_NINES)
                      ? this->istrumpnine()
                      : this->istrumpking())
                 )
             ) );
} // bool HandCard::possible_hyperswine()

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	true if card is a fox
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.8
 **/
bool
HandCard::isfox() const
{
  return (GAMETYPE::is_normal(this->game().type())
          && this->istrumpace()
          && !this->isswine());
} // bool HandCard::isfox()

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	true if card is a trump ace
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.8
 **/
bool
HandCard::istrumpace() const
{
  return ((this->value() == ACE)
          && this->istrump());
} // bool HandCard::istrumpace()

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	true if card is a trump nine
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.8
 **/
bool
HandCard::istrumpnine() const
{
  return ((this->value() == NINE)
          && this->istrump());
} // bool HandCard::istrumpnine()

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	true if card is a trump king
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.8
 **/
bool
HandCard::istrumpking() const
{
  return ((this->value() == KING)
          && this->istrump());
} // bool HandCard::istrumpking() const

/**
 ** -> result
 **
 ** @note	if both cards are equal, the first (this) is not less than
 **		the second (card)
 **		(but the dollen -- see rules)
 ** @note	this tests also for possible hyperswines/swines
 **
 ** @param	card	the card to compare with
 **
 ** @return	true, if the card is less than 'card', else false
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.4
 **/
bool
HandCard::less(HandCard const& card) const
{
  if (this->is_empty())
    return true;
  if (card.is_empty())
    return false;

  { // hyperswines
    if (this->possible_hyperswine())
      return false;

    if (card.possible_hyperswine())
      return true;
  } // hyperswines
  { // swines
    if (this->possible_swine())
      return false;

    if (card.possible_swine())
      return true;
  } // swines

  return this->game().less(*this, card);
} // bool HandCard::less(Card card) const

/**
 ** -> result
 **
 ** @note	if both cards are equal, the first (this) is not less than
 **		the second (card)
 **		(but the dollen -- see rules)
 **
 ** @param	card	the card to compare with
 **
 ** @return	true, if the card is less than 'b', else false
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.4
 **/
bool
HandCard::less(Card const& card) const
{
  if (this->is_empty())
    return true;
  if (card.is_empty())
    return false;

  if (this->possible_hyperswine())
    return false;
  if (this->possible_swine())
    return false;

  return this->game().less(*this, card);
} // bool HandCard::less(Card card) const

/**
 ** -> result
 **
 ** @param	a	first card
 ** @param	b	second card
 **
 ** @return	whether the card 'a' is left (-1) or right (1) from card 'b'
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.8
 **/
int
HandCard::relative_position(HandCard const& a, HandCard const& b)
{
  return ::setting(Setting::CARDS_ORDER).relative_position(a, b);
} // static int HandCard::relative_position(HandCard a, HandCard b)

/**
 ** -> result
 **
 ** @param	trick	current trick
 **
 ** @return	whether it is valid to play the card in the trick
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.8
 **/
bool
HandCard::isvalid(Trick const& trick) const
{
  if (this->is_unknown())
    return true;

  Hand const& hand = this->hand();

  // test the card
  DEBUG_ASSERTION(!this->is_empty(),
                  "HandCard::isvalid(Trick):\n"
                  "  empty Card");

  // test whether the card is in the hand
  DEBUG_ASSERTION(hand.getpos(*this) != UINT_MAX,
                  "HandCard::isvalid(Trick):\n"
                  "  Card is not in the hand"
                  ": " << *this << "\n" << hand);

  // The first person can play all cards
  if (trick.isstartcard())
    return true;

  // if there is a card with the same tcolor on the hand as the startcard,
  // the card has to have the same tcolor
  if (hand.contains(trick.startcard().tcolor()))
    return (trick.startcard().tcolor() == this->tcolor());

  // the tcolor of the startcard is not in the hand
  // the player can play any card
  return true;
} // bool HandCard::isvalid(Trick trick) const

