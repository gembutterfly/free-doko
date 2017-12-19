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

#include "base_cards.h"

#include "../../misc/translations.h"
#include "../../utils/string.h"

/**
 ** constructor
 **
 ** @param	type       action type
 ** @param	playerno	player who has shifted the cards
 ** @param	cards		shifted cards
 **
 ** @return	-
 **
 ** @author	Diether Knof
 ** 
 ** @version	0.7.3
 **/
GameplayAction::BaseCards::BaseCards(Type const& type,
				     unsigned const playerno,
				     vector<Card> const& cards) :
  Player1(type, playerno),
  cards(cards)
{ }

/**
 ** constructor
 **
 ** @param	type       action type
 ** @param	line	line with the data
 **
 ** @return	action corresponding to the line
 **		(to be freed)
 **
 ** @author	Diether Knof
 ** 
 ** @version	0.7.3
 ** 
 ** @todo	old format
 **/
GameplayAction::BaseCards::BaseCards(Type const& type,string line) :
  Player1(type, UINT_MAX),
  cards()
{
#ifndef OUTDATED
  // old format in the bug report:
  //   Played: Player 2: club ace
  if (string(line, 0, strlen("")) == "Poverty Shift: ") {
    // ToDo

    return ;
  }
#endif

  string const pre = ::name(this->type) + " =";
  if (string(line, 0, pre.length()) != pre)
    return ;

  // first split the line in the single parts
  vector<string> const parts(DK::Utils::String::split(string(line, pre.length()),
						    ','));
  vector<string>::const_iterator p = parts.begin();

  if (p == parts.end()) {
      this->player = UINT_MAX;
      return ;
  }
  { // read the player
    if (*p == "-") {
      this->player = UINT_MAX;
      return ;
    }

    istringstream istr(*p);
    istr >> this->player;
    ++p;
  } // read the player

  { // read the cards
    for (; p != parts.end(); ++p)
      this->cards.push_back(Card(*p));
  } // read the cards

  return ;
} // GameplayAction::BaseCards(string line)

/**
 ** copy constructor
 **
 ** @param	poverty_shift	action to copy
 **
 ** @return	-
 **
 ** @author	Diether Knof
 ** 
 ** @version	0.7.3
 **/
GameplayAction::BaseCards::BaseCards(BaseCards const&
				     poverty_shift) :
  Player1(poverty_shift),
  cards(poverty_shift.cards)
{ }

/**
 ** copy constructor
 **
 ** @param	poverty_shift	action to copy
 **
 ** @return	this action
 **
 ** @author	Diether Knof
 ** 
 ** @version	0.7.3
 **/
GameplayAction::BaseCards&
GameplayAction::BaseCards::operator=(BaseCards const& poverty_shift)
{
  this->Player1::operator=(poverty_shift);
  this->cards = poverty_shift.cards;
  return *this;
} // BaseCards& BaseCards::operator=(BaseCards poverty_shift)

/**
 ** destructor
 **
 ** @param	-
 **
 ** @return	-
 **
 ** @author	Diether Knof
 ** 
 ** @version	0.7.3
 **/
GameplayAction::BaseCards::~BaseCards()
{ }

/**
 ** -> result
 **
 ** @param	action	action to compare with
 **
 ** @return	whether the action is equal to this
 **
 ** @author	Diether Knof
 ** 
 ** @version	0.7.3
 **/
bool
GameplayAction::BaseCards::equal(GameplayAction const& action) const
{
  if (!this->Player1::equal(action))
    return false;

  GameplayAction::BaseCards const& base_cards
    = static_cast<GameplayAction::BaseCards const&>(action);

  // compare the cards
  if (this->cards.size() != base_cards.cards.size())
    return false;

  for (unsigned i = 0; i < this->cards.size(); ++i)
    if (this->cards[i] != base_cards.cards[i])
      return false;

  return true;
} // bool GameplayAction::BaseCards::equal(GameplayAction action) const

/**
 **
 ** writes the action in the output stream
 **
 ** @param	ostr	output stream
 **
 ** @return	-
 **
 ** @author	Diether Knof
 ** 
 ** @version	0.7.3
 **/
void
GameplayAction::BaseCards::write(ostream& ostr) const
{
  ostr << this->type;
  ostr << " = " << this->player;
  for (vector<Card>::const_iterator c = this->cards.begin();
       c != this->cards.end();
       ++c)
    ostr << ", " << *c;

  return ;
} // void write(ostream& ostr) const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	a translation representing the data
 ** 
 ** @author	Diether Knof
 **
 ** @version	0.7.3
 **/
Translator::Translation
GameplayAction::BaseCards::data_translation() const
{
  vector<Card>::const_iterator c = this->cards.begin();
  if (c == this->cards.end())
    return ::translation("");

  Translator::Translation translation = ::translation(*c);
  for (++c; c != this->cards.end(); ++c)
    translation += ", " + ::translation(*c);

  return translation;
} // virtual Translator::Translation GameplayAction::BaseCards::data_translation() const
