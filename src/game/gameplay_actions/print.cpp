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

#include "print.h"

#include "../../misc/translations.h"

#include "../../utils/string.h"

/**
 ** constructor
 **
 ** @param	-
 **
 ** @return	-
 **/
GameplayAction::Print::Print() :
 Pure(PRINT),
  info_()
{ }

/**
 ** constructor
 **
 ** @param	line   line with the data
 **
 ** @return	-
 **/
GameplayAction::Print::Print(string const& line) :
  Pure(PRINT),
  info_()
{
  string const pre = ::name(this->type) + " = ";
  if (string(line, 0, pre.length()) != pre)
    return ;

  this->info_ = string(line, pre.length());
  DK::Utils::String::remove_blanks(this->info_);
  
  return ;
} // GameplayAction::Print::Print(string const& line)

/**
 ** -> return
 **
 ** @param    -
 **
 ** @return   the info to print
 **/
string const&
GameplayAction::Print::info() const
{
  return this->info_;
} // string const& GameplayAction::Print::info() const

/**
 **
 ** writes the action in the output stream
 **
 ** @param	ostr	output stream
 **
 ** @return	-
 **/
void
GameplayAction::Print::write(ostream& ostr) const
{
  ostr << this->type;
  if (!this->info().empty())
    ostr << " " << this->info();

  return ;
} // void write(ostream& ostr) const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	a translation representing the data
 **/
Translator::Translation
GameplayAction::Print::data_translation() const
{
  return ::translation("%stext%", this->info());
} // Translator::Translation GameplayAction::Print::data_translation() const
