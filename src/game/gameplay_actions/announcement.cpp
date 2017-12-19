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

#include "announcement.h"

#include "../../misc/translations.h"

#include "../../utils/string.h"

/**
 ** constructor
 **
 ** @param	playerno	player who has made the announcement
 ** @param	announcement	announcement
 **
 ** @return	-
 **
 ** @author	Diether Knof
 ** 
 ** @version	0.7.1
 **/
GameplayAction::Announcement::Announcement(unsigned const playerno,
					     ::Announcement const&
					     announcement) :
  Player1(ANNOUNCEMENT, playerno),
  announcement(announcement)
{ }

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
 **/
GameplayAction::Announcement::Announcement(string line) :
  Player1(ANNOUNCEMENT, UINT_MAX),
  announcement(ANNOUNCEMENT::NOANNOUNCEMENT)
{
#ifndef OUTDATED
  // old format in the bug report:
  //   Announcement: player 1: no 120
  if (string(line, 0, strlen("Announcement: ")) == "Announcement: ") {
    // there is an announcement, look, which player has made it
    DK::Utils::String::word_first_remove_with_blanks(line);
    DK::Utils::String::word_first_remove_with_blanks(line);
    this->player = static_cast<unsigned>(atoi(line.c_str()));

    // read the announcement
    DK::Utils::String::word_first_remove_with_blanks(line);
    this->announcement = ANNOUNCEMENT::from_name(line);

    return ;
  }
#endif

  // ex: announcement = 0, no 90
  string const pre = ::name(this->type) + " =";
  if (string(line, 0, pre.length()) != pre)
    return ;

  istringstream istr(string(line, pre.length()));
  istr >> this->player;
  if (istr.get() != ',')
    return ;
  if (istr.get() != ' ')
    return ;
  string announcement_name;
  getline(istr, announcement_name);
  this->announcement = ANNOUNCEMENT::from_name(announcement_name);

  return ;
} // GameplayAction::Announcement(string line)

/**
 ** copy constructor
 **
 ** @param	announcement	action to copy
 **
 ** @return	-
 **
 ** @author	Diether Knof
 ** 
 ** @version	0.7.1
 **/
GameplayAction::Announcement::Announcement(Announcement const& announcement) :
  Player1(announcement),
  announcement(announcement.announcement)
{
} // Announcement::Announcement(Announcement announcement)

/**
 ** copy constructor
 **
 ** @param	announcement	action to copy
 **
 ** @return	this action
 **
 ** @author	Diether Knof
 ** 
 ** @version	0.7.1
 **/
GameplayAction::Announcement&
GameplayAction::Announcement::operator=(Announcement const& announcement)
{
  this->Player1::operator=(announcement);
  this->announcement = announcement.announcement;
  return *this;
} // Announcement& Announcement::operator=(Announcement announcement)

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
GameplayAction::Announcement::~Announcement()
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
GameplayAction::Announcement::equal(GameplayAction const& action) const
{
  return (   (this->GameplayAction::equal(action))
	  && (this->announcement == static_cast<GameplayAction::Announcement const&>(action).announcement));
} // bool GameplayAction::Announcement::equal(GameplayAction action) const

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
GameplayAction::Announcement::write(ostream& ostr) const
{
  ostr << this->type;
  ostr << " = " << this->player;
  ostr << ", " << this->announcement;

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
GameplayAction::Announcement::data_translation() const
{
  return (Translator::Translation(::translator)
	  + DK::Utils::String::to_string(this->player)
	  + ": "
	  + ::translation(this->announcement));
} // virtual Translator::Translation GameplayAction::Announcement::data_translation() const
