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

#include "genscher.h"

#include "../../misc/translations.h"
#include "../../utils/string.h"

/**
 ** constructor
 **
 ** @param	genscher	the player with the genscher
 ** @param	partner		the partner
 **
 ** @return	-
 **
 ** @author	Diether Knof
 ** 
 ** @version	0.7.1
 **/
GameplayAction::Genscher::Genscher(unsigned const genscher,
				   unsigned const partner) :
  Player1(GENSCHER, genscher),
  partner(partner)
{ }

/**
 ** constructor
 **
 ** @param	line	line with the data
 **
 ** @return	-
 **
 ** @author	Diether Knof
 ** 
 ** @version	0.7.3
 ** 
 ** @todo	old format
 **/
GameplayAction::Genscher::Genscher(string line) :
  Player1(GENSCHER, UINT_MAX),
  partner()
{
#ifndef OUTDATED
  // old format in the bug report:
  //   ?
  if (string(line, 0, strlen("Genscher: ")) == "Genscher: ") {
    // ToDo

    return ;
  }
#endif

  // ex: genscher = 0, 1
  string const pre = ::name(this->type) + " =";
  if (string(line, 0, pre.length()) != pre)
    return ;

  istringstream istr(string(line, pre.length()));
  istr >> this->player;
  if (istr.get() != ',')
    return ;
  istr >> this->partner;

  return ;
} // GameplayAction::Genscher(string line)

/**
 ** copy constructor
 **
 ** @param	genscher	action to copy
 **
 ** @return	action corresponding to the line
 **		(to be freed)
 **
 ** @author	Diether Knof
 ** 
 ** @version	0.7.1
 **/
GameplayAction::Genscher::Genscher(Genscher const& genscher) :
  Player1(genscher),
  partner(genscher.partner)
{ }

/**
 ** copy constructor
 **
 ** @param	genscher	action to copy
 **
 ** @return	this action
 **
 ** @author	Diether Knof
 ** 
 ** @version	0.7.1
 **/
GameplayAction::Genscher&
GameplayAction::Genscher::operator=(Genscher const& genscher)
{
  this->Player1::operator=(genscher);
  this->partner = genscher.partner;
  return *this;
} // Genscher& Genscher::operator=(Genscher genscher)

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
GameplayAction::Genscher::~Genscher()
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
GameplayAction::Genscher::equal(GameplayAction const& action) const
{
  return (   (this->GameplayAction::equal(action))
	  && (this->partner == static_cast<GameplayAction::Genscher const&>(action).partner));
} // bool GameplayAction::Genscher::equal(GameplayAction action) const

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
GameplayAction::Genscher::write(ostream& ostr) const
{
  ostr << this->type;
  ostr << " = " << this->player << ", " << this->partner;

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
GameplayAction::Genscher::data_translation() const
{
  return (Translator::Translation(::translator)
	  + DK::Utils::String::to_string(this->partner));
} // virtual Translator::Translation GameplayAction::Genscher::data_translation() const
