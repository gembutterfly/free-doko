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
#include "card.h"

#include "hand_card.h"
#include "../game/game.h"
#include "../party/rule.h"

Card const Card::EMPTY{};
Card const Card::UNKNOWN(Card::UNKNOWNCARDCOLOR, Card::UNKNOWNCARDVALUE);
Card const Card::FOX(Card::DIAMOND, Card::ACE);
Card const Card::DOLLE(Card::HEART, Card::TEN);
Card const Card::CHARLIE(Card::CLUB, Card::JACK);
Card const Card::CLUB_ACE(Card::CLUB, Card::ACE);
Card const Card::SPADE_ACE(Card::SPADE, Card::ACE);
Card const Card::HEART_ACE(Card::HEART, Card::ACE);
Card const Card::DIAMOND_ACE(Card::DIAMOND, Card::ACE);
Card const Card::CLUB_TEN(Card::CLUB, Card::TEN);
Card const Card::SPADE_TEN(Card::SPADE, Card::TEN);
Card const Card::HEART_TEN(Card::HEART, Card::TEN);
Card const Card::DIAMOND_TEN(Card::DIAMOND, Card::TEN);
Card const Card::CLUB_KING(Card::CLUB, Card::KING);
Card const Card::SPADE_KING(Card::SPADE, Card::KING);
Card const Card::HEART_KING(Card::HEART, Card::KING);
Card const Card::DIAMOND_KING(Card::DIAMOND, Card::KING);
Card const Card::CLUB_QUEEN(Card::CLUB, Card::QUEEN);
Card const Card::SPADE_QUEEN(Card::SPADE, Card::QUEEN);
Card const Card::HEART_QUEEN(Card::HEART, Card::QUEEN);
Card const Card::DIAMOND_QUEEN(Card::DIAMOND, Card::QUEEN);
Card const Card::CLUB_JACK(Card::CLUB, Card::JACK);
Card const Card::SPADE_JACK(Card::SPADE, Card::JACK);
Card const Card::HEART_JACK(Card::HEART, Card::JACK);
Card const Card::DIAMOND_JACK(Card::DIAMOND, Card::JACK);
Card const Card::CLUB_NINE(Card::CLUB, Card::NINE);
Card const Card::SPADE_NINE(Card::SPADE, Card::NINE);
Card const Card::HEART_NINE(Card::HEART, Card::NINE);
Card const Card::DIAMOND_NINE(Card::DIAMOND, Card::NINE);

// 4 colors * 6 values
int const Card::MAX_INT = 4 * 6;

/**
 ** constructor
 **
 ** @param     c   card color
 ** @param     v   card value
 **
 ** @return    -
 **/
Card::Card(Color const c, Value const v):
  color_(c), 
  value_(v)
{
  if (   (this->color() == NOCARDCOLOR)
      || (this->value() == NOCARDVALUE))
    this->color_ = NOCARDCOLOR, this->value_ = NOCARDVALUE;

  return ;
} // Card::Card(Color c, Value v)

/**
 ** constructor
 ** reads 'color' and 'value' from 'name'
 **
 ** @param	name	the card name
 **
 ** @return	-
 **/
Card::Card(string const& name)
{
  istringstream istr(name);
  this->read(istr);

  return ;
} // Card::Card(string name)

/**
 ** read the card from the stream
 **
 ** @param	istr	stream to read the card from
 **
 ** @return	stream
 **/
istream&
Card::read(istream& istr)
{
  return (istr >> this->color_ >> this->value_);
} // istrean& Card::read(istream& istr)

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    the color of the card
 **/
Card::Color
Card::color() const
{ return this->color_; }

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    the value of the card
 **/
Card::Value
Card::value() const
{ return this->value_; }

/**
 ** -> return
 **
 ** @param     -
 **
 ** @return    whether this card is empty
 **/
bool
Card::is_empty() const
{
  return (   (this->color() == NOCARDCOLOR)
          || (this->value() == NOCARDVALUE));
} // bool Card::is_empty() const

/**
 ** -> return
 **
 ** @param     -
 **
 ** @return    whether this card is not empty
 **/
Card::operator bool() const
{
  return !this->is_empty();
} // Card::operator bool() const

/**
 ** -> return
 **
 ** @param     -
 **
 ** @return    whether this card is unknown
 **/
bool
Card::is_unknown() const
{
  return (   (this->color() == UNKNOWNCARDCOLOR)
          || (this->value() == UNKNOWNCARDVALUE));
} // bool Card::is_unknown() const


/**
 ** -> result
 ** This value is used to use 'vector<.>' instead of 'map<Card, .>'
 ** because of performance reasons (~15% better when using in 'CardCounter')
 **
 ** @param     -
 **
 ** @return    integer value of the card
 **/
int
Card::to_int() const
{
  if (this->color() == NOCARDCOLOR)
    return 0;
  switch (this->value()) {
  case Card::NOCARDVALUE:
  case Card::UNKNOWNCARDVALUE:
    return 0;
  case Card::NINE:
    return (this->color() * 6 + 1);
  case Card::JACK:
    return (this->color() * 6 + 2);
  case Card::QUEEN:
    return (this->color() * 6 + 3);
  case Card::KING:
    return (this->color() * 6 + 4);
  case Card::TEN:
    return (this->color() * 6 + 5);
  case Card::ACE:
    return (this->color() * 6 + 6);
  }; // switch (this->value())

  return 0;
} // int Card::to_int() const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	ths points of the card
 **/
unsigned
Card::points() const
{
  // based on structure of enum type card values 
  // here can value be direct returned
  return this->value();
} // unsigned Card::points() const

/**
 ** -> result
 **
 ** @param	game	the game
 **
 ** @return	the tcolor of the card
 **/
Card::TColor
Card::tcolor(Game const& game) const
{
  if (this->istrump(game))
    return Card::TRUMP;
  else
    return this->color();
} // Card::TColor Card::tcolor(Game game) const

/**
 ** -> result
 **
 ** @param	game	the game
 **
 ** @return	true if card is trump in the game
 **/
bool
Card::istrump(Game const& game) const
{
  return this->istrump(game.type(), game.rule()(Rule::DOLLEN));
} // bool Card::istrump(Game game) const

/**
 ** -> result
 **
 ** @param	gametype	the gametype
 ** @param	dollen		whether dollen are allowed
 **
 ** @return	true if card is trump in the game
 **/
bool
Card::istrump(GameType const gametype, bool const dollen) const
{
  if (this->isdolle(gametype, dollen))
    return true;

  switch (gametype) {
  case GAMETYPE::THROWN_NINES:
    return (this->value() == NINE);
  case GAMETYPE::THROWN_KINGS:
    return (this->value() == KING);
  case GAMETYPE::THROWN_NINES_AND_KINGS:
    return (   (this->value() == NINE)
            || (this->value() == KING) );
  case GAMETYPE::THROWN_RICHNESS:
    return true;
  case GAMETYPE::NORMAL:
  case GAMETYPE::POVERTY:
  case GAMETYPE::FOX_HIGHEST_TRUMP:
  case GAMETYPE::GENSCHER:
  case GAMETYPE::MARRIAGE:
  case GAMETYPE::MARRIAGE_SOLO:
  case GAMETYPE::MARRIAGE_SILENT:
  case GAMETYPE::SOLO_DIAMOND:
    return (   (this->color() == DIAMOND)
            || (this->value() == JACK)
            || (this->value() == QUEEN) );
  case GAMETYPE::SOLO_JACK:
    return (this->value() == JACK);
  case GAMETYPE::SOLO_QUEEN:
    return (this->value() == QUEEN);
  case GAMETYPE::SOLO_KING:
    return (this->value() == KING);
  case GAMETYPE::SOLO_QUEEN_JACK:
    return (   (this->value() == JACK)
            || (this->value() == QUEEN) );
    break;
  case GAMETYPE::SOLO_KING_JACK:
    return (   (this->value() == JACK)
            || (this->value() == KING) );
  case GAMETYPE::SOLO_KING_QUEEN:
    return (   (this->value() == QUEEN)
            || (this->value() == KING) );
  case GAMETYPE::SOLO_KOEHLER:
    return (   (this->value() == JACK)
            || (this->value() == QUEEN)
            || (this->value() == KING) );
  case GAMETYPE::SOLO_CLUB:
    return (   (this->color() == CLUB)
            || (this->value() == JACK)
            || (this->value() == QUEEN) );
  case GAMETYPE::SOLO_HEART:
    return (   (this->color() == HEART)
            || (this->value() == JACK)
            || (this->value() == QUEEN) );
  case GAMETYPE::SOLO_SPADE:
    return (   (this->color() == SPADE)
            || (this->value() == JACK)
            || (this->value() == QUEEN) );
  case GAMETYPE::SOLO_MEATLESS:
    return false;
  } // switch(gametype)

  return false;
} // bool Card::istrump(GameType gametype, bool dollen) const

/**
 ** -> result
 **
 ** @param	game	the game
 **
 ** @return	true if card is a dolle
 **/
bool
Card::isdolle(Game const& game) const
{
  return this->isdolle(game.type(), game.rule()(Rule::DOLLEN));
} // bool Card::isdolle(Game game) const

/**
 ** -> result
 **
 ** @param	gametype	the gametype
 ** @param	dollen		whether dollen are allowed
 **
 ** @return	true if card is a dolle
 **/
bool
Card::isdolle(GameType const gametype, bool const dollen) const
{
  if (!dollen)
    return false;

  if (*this != Card::DOLLE)
    return false;

  switch (gametype) {
  case GAMETYPE::NORMAL:
  case GAMETYPE::POVERTY:
  case GAMETYPE::GENSCHER:
  case GAMETYPE::MARRIAGE:
  case GAMETYPE::MARRIAGE_SOLO:
  case GAMETYPE::MARRIAGE_SILENT:
  case GAMETYPE::SOLO_CLUB:
  case GAMETYPE::SOLO_HEART:
  case GAMETYPE::SOLO_SPADE:
  case GAMETYPE::SOLO_DIAMOND:
    return true;
  case GAMETYPE::FOX_HIGHEST_TRUMP:
  case GAMETYPE::SOLO_MEATLESS:
  case GAMETYPE::SOLO_JACK:
  case GAMETYPE::SOLO_QUEEN:
  case GAMETYPE::SOLO_KING:
  case GAMETYPE::SOLO_QUEEN_JACK:
  case GAMETYPE::SOLO_KING_JACK:
  case GAMETYPE::SOLO_KING_QUEEN:
  case GAMETYPE::SOLO_KOEHLER:
  case GAMETYPE::THROWN_NINES:
  case GAMETYPE::THROWN_KINGS:
  case GAMETYPE::THROWN_NINES_AND_KINGS:
  case GAMETYPE::THROWN_RICHNESS:
    return false;
  } // switch (gametype)

  return false;
} // bool Card::isdolle(GameType gametype, bool dollen) const

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
 ** @todo       is this function still called?
 **/
bool
Card::less(HandCard const& card) const
{
  if (card.is_empty())
    return false;

  if (card.possible_hyperswine())
    return (*this != card);
  if (   card.game().hyperswines_announced()
      && (*this == card.game().hyperswine()))
      return false;
  if (card.possible_swine())
    return (*this != card);

  return card.game().less(*this, card);
} // bool HandCard::less(Card card) const

/**
 ** -> result
 **
 ** @param	a	first card
 ** @param	b	second card
 **
 ** @result	whether the two cards are equal
 **/
bool
operator==(Card const& a, Card const& b)
{
  return (   (a.value() == b.value())
          && (a.color() == b.color()));
} // bool operator==(Card a, Card b)

/**
 ** -> result
 **
 ** @param	a	first card
 ** @param	b	second card
 **
 ** @result	whether 'a' is lexicographical smaller than 'b'
 **/
bool
operator<(Card const& a, Card const& b)
{
  return (   (a.color() < b.color())
          || (   (a.color() == b.color())
              && (a.value() < b.value())));
} // bool operator<(Card a, Card b)

/**
 ** -> result
 **
 ** @param     tcolor              tcolor
 ** @param     marriage_selector   marriage selector
 **
 ** @result    whether the marriage is determined by a 'tcolor'-trick
 **/
bool
is_selector(Card::TColor const tcolor,
            MarriageSelector const marriage_selector)
{
  switch (marriage_selector) {
  case MARRIAGE_SELECTOR::TEAM_SET:
  case MARRIAGE_SELECTOR::SILENT:
    return false;
  case MARRIAGE_SELECTOR::FIRST_FOREIGN:
    return true;
  case MARRIAGE_SELECTOR::FIRST_TRUMP:
    return (tcolor == Card::TRUMP);
  case MARRIAGE_SELECTOR::FIRST_COLOR:
    return (tcolor != Card::TRUMP);
  case MARRIAGE_SELECTOR::FIRST_CLUB:
    return (tcolor == Card::CLUB);
  case MARRIAGE_SELECTOR::FIRST_SPADE:
    return (tcolor == Card::SPADE);
  case MARRIAGE_SELECTOR::FIRST_HEART:
    return (tcolor == Card::HEART);
  } // switch (marriage_selector)

  return false;
} // bool is_selector(Card::TColor tcolor, MarriageSelector marriage_selector)

/**
 ** -> result
 **
 ** @param     -
 **
 ** @result    the name of the card
 **/
string
name(Card const& card)
{
  if (card.is_empty())
    return "empty";
  else if (card.is_unknown())
    return "unknown";
  else
    return (name(card.color()) + " " + name(card.value()));
} // string name(Card const& card)

/**
 ** -> result
 **
 ** @param     -
 **
 ** @result    the name of the card
 **/
string
to_string(Card const& card)
{ 
  if (card.is_empty())
    return "empty";
  else if (card.is_unknown())
    return "unknown";
  else
    return (to_string(card.color()) + " " + to_string(card.value()));
} // string to_string(Card card)

/**
 ** writes the card in the output stream
 **
 ** @param	ostr	output stream
 ** @param	card	card
 **
 ** @return	output stream
 **/
ostream&
operator<<(ostream& ostr, Card const& card)
{
  return (ostr << ::name(card));
} // ostream& operator<<(ostream& ostr, Card const& card);

/**
 ** read the card out of the stream
 **
 ** @param	istr	input stream
 ** @param	card	card
 **
 ** @return	output stream
 **/
istream&
operator>>(istream& istr, Card& card)
{
  return card.read(istr);
} // istream& operator>>(istream& istr, Card& card);
