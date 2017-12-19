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

#include "check.h"

#include "../../misc/translations.h"

#include "../../utils/string.h"

/**
 ** constructor
 **
 ** @param	-
 **
 ** @return	-
 **
 ** @author	Diether Knof
 ** 
 ** @version	0.7.6
 **/
GameplayAction::Check::Check() :
 Pure(CHECK),
  comment_()
{ }

/**
 ** constructor
 **
 ** @param	line   line with the data
 **
 ** @return	-
 **
 ** @author	Diether Knof
 ** 
 ** @version	0.7.6
 **/
GameplayAction::Check::Check(string const& line) :
  Pure(CHECK),
  comment_()
{
  string const pre = ::name(this->type) + " =";
  if (string(line, 0, pre.length()) != pre)
    return ;

  this->comment_ = string(line, pre.length());
  DK::Utils::String::remove_blanks(this->comment_);
  
  return ;
} // GameplayAction::Check::Check(string const& line)

/**
 ** copy constructor
 **
 ** @param	action   action to copy
 **
 ** @return	-
 **
 ** @author	Diether Knof
 ** 
 ** @version	0.7.6
 **/
GameplayAction::Check::Check(Check const& action) :
 Pure(action),
  comment_(action.comment())
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
 ** @version	0.7.6
 **/
GameplayAction::Check&
GameplayAction::Check::operator=(Check const& action)
{
  this->Pure::operator=(action);
  this->comment_ = action.comment();
  return *this;
} // Check& Check::operator=(Check action)

/**
 ** destructor
 **
 ** @param	-
 **
 ** @return	-
 **
 ** @author	Diether Knof
 ** 
 ** @version	0.7.6
 **/
GameplayAction::Check::~Check()
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
 ** @version	0.7.6
 **/
void
GameplayAction::Check::write(ostream& ostr) const
{
  ostr << this->type;
  if (!this->comment().empty())
    ostr << " = " << this->comment();

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
 ** @version	0.7.6
 **/
Translator::Translation
GameplayAction::Check::data_translation() const
{
  return ::translation("%stext%", this->comment());
} // Translator::Translation GameplayAction::Check::data_translation() const
