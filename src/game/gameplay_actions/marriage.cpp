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

#include "marriage.h"

#include "../../misc/translations.h"
#include "../../utils/string.h"

/**
 ** constructor
 **
 ** @param	bridegroom	player with the marriage
 ** @param	bride		the partner
 **
 ** @return	-
 **
 ** @author	Diether Knof
 ** 
 ** @version	0.7.1
 **/
GameplayAction::Marriage::Marriage(unsigned const bridegroom,
				     unsigned const bride) :
  Player1(MARRIAGE, bridegroom),
  bride(bride)
{
} // Marriage::Marriage()

/**
 ** constructor
 **
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
GameplayAction::Marriage::Marriage(string line) :
  Player1(MARRIAGE, UINT_MAX),
  bride()
{
#ifndef OUTDATED
  // old format in the bug report:
  //   Played: Player 2: club ace
  if (string(line, 0, strlen("Marriage: ")) == "Marriage: ") {
    // ToDo

    return ;
  }
#endif

  // ex: marriage = 0, 1
  string const pre = ::name(this->type) + " =";
  if (string(line, 0, pre.length()) != pre)
    return ;

  istringstream istr(string(line, pre.length()));
  istr >> this->player;
  if (istr.get() != ',')
    return ;
  istr >> this->bride;

  return ;
} // GameplayAction::Marriage(string line)

/**
 ** copy constructor
 **
 ** @param	marriage	action to copy
 **
 ** @return	-
 **
 ** @author	Diether Knof
 ** 
 ** @version	0.7.1
 **/
GameplayAction::Marriage::Marriage(Marriage const& marriage) :
  Player1(marriage),
  bride(marriage.bride)
{ }

/**
 ** copy constructor
 **
 ** @param	marriage	action to copy
 **
 ** @return	this action
 **
 ** @author	Diether Knof
 ** 
 ** @version	0.7.1
 **/
GameplayAction::Marriage&
GameplayAction::Marriage::operator=(Marriage const& marriage)
{
  this->Player1::operator=(marriage);
  this->bride = marriage.bride;
  return *this;
} // Marriage& Marriage::operator=(Marriage marriage)

/**
 ** destructor
 **
 ** @param	-
 **
 ** @return	-
 **
 ** @author	Diether Knof
 ** 
 ** @version	0.7.1
 **/
GameplayAction::Marriage::~Marriage()
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
GameplayAction::Marriage::equal(GameplayAction const& action) const
{
  return (   (this->GameplayAction::equal(action))
	  && (this->bride == static_cast<GameplayAction::Marriage const&>(action).bride));
} // bool GameplayAction::Marriage::equal(GameplayAction action) const

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
 ** @version	0.7.1
 **/
void
GameplayAction::Marriage::write(ostream& ostr) const
{
  ostr << this->type;
  ostr << " = " << this->player << ", " << this->bride;

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
 ** @version	0.7.1
 **/
Translator::Translation
GameplayAction::Marriage::data_translation() const
{
  return (Translator::Translation(::translator)
	  + DK::Utils::String::to_string(this->bride));
} // virtual Translator::Translation GameplayAction::Marriage::data_translation() const
