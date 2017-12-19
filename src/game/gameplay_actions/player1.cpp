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

#include "player1.h"

#include "../../misc/translations.h"
#include "../../utils/string.h"

/**
 ** constructor
 **
 ** @param	type       action type
 ** @param	playerno   player who has initiated the action
 **
 ** @return	-
 **
 ** @author	Diether Knof
 ** 
 ** @version	0.7.3
 **/
GameplayAction::Player1::Player1(Type const& type, unsigned const playerno) :
 GameplayAction(type, playerno)
{ }

/**
 ** constructor
 **
 ** @param	type   action type
 ** @param	line   line with the data
 **
 ** @return	action corresponding to the line
 **		(to be freed)
 **
 ** @author	Diether Knof
 ** 
 ** @version	0.7.3
 **/
GameplayAction::Player1::Player1(Type const& type, string const& line) :
  GameplayAction(type, UINT_MAX)
{
  // ex: swines = 0
  string const pre = ::name(this->type) + " =";
  if (string(line, 0, pre.length()) != pre)
    return ;

  istringstream istr(string(line, pre.length()));
  istr >> this->player;

  return ;
} // GameplayAction::Player1::Player1(Type type, string line)


/**
 ** copy constructor
 **
 ** @param	action   action to copy
 **
 ** @return	-
 **
 ** @author	Diether Knof
 ** 
 ** @version	0.7.3
 **/
GameplayAction::Player1::Player1(Player1 const& action) :
 GameplayAction(action)
{ }

/**
 ** copy constructor
 **
 ** @param	action   action to copy
 **
 ** @return	this action
 **
 ** @author	Diether Knof
 ** 
 ** @version	0.7.3
 **/
GameplayAction::Player1&
GameplayAction::Player1::operator=(Player1 const& action)
{
  this->GameplayAction::operator=(action);
  return *this;
} // Player1& Player1::operator=(Player1 action)

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
GameplayAction::Player1::~Player1()
{ }

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
GameplayAction::Player1::write(ostream& ostr) const
{
  ostr << this->type;
  ostr << " = " << this->player;

  return ;
} // void write(ostream& ostr) const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	a translation representing the data (without the player number)
 ** 
 ** @author	Diether Knof
 **
 ** @version	0.7.3
 **/
Translator::Translation
GameplayAction::Player1::data_translation() const
{
  return ::translation("");
} // virtual Translator::Translation GameplayAction::Player1::data_translation() const
