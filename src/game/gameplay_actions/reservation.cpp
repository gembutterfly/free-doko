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

#include "reservation.h"

#include "../../misc/translations.h"
#include "../../utils/string.h"

/**
 ** constructor
 **
 ** @param	player         player
 ** @param	reservation    reservation of the player
 **
 ** @return	-
 **
 ** @author	Diether Knof
 ** 
 ** @version	0.7.3
 **/
GameplayAction::Reservation::Reservation(unsigned const player,
					 ::Reservation const& reservation) :
  Player1(RESERVATION, player),
  reservation(reservation)
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
GameplayAction::Reservation::Reservation(string line) :
  Player1(RESERVATION, UINT_MAX),
  reservation()
{
  // ex: reservation = 0, marriage, first color, hyperswines
  string const pre = ::name(this->type) + " =";
  if (string(line, 0, pre.length()) != pre)
    return ;

  istringstream istr(string(line, pre.length()));
  istr >> this->player;
  if (istr.get() != ',')
    return ;
  string reservation_text;
  getline(istr, reservation_text);
  this->reservation = ::Reservation(reservation_text);

  return ;
} // GameplayAction::Reservation(string line)

/**
 ** copy constructor
 **
 ** @param	reservation   action to copy
 **
 ** @return	-
 **
 ** @author	Diether Knof
 ** 
 ** @version	0.7.3
 **/
GameplayAction::Reservation::Reservation(Reservation const& reservation) :
  Player1(reservation),
  reservation(reservation.reservation)
{ }

/**
 ** copy constructor
 **
 ** @param	reservation   action to copy
 **
 ** @return	this action
 **
 ** @author	Diether Knof
 ** 
 ** @version	0.7.3
 **/
GameplayAction::Reservation&
GameplayAction::Reservation::operator=(Reservation const& reservation)
{
  this->Player1::operator=(reservation);
  this->reservation = reservation.reservation;
  return *this;
} // Reservation& Reservation::operator=(Reservation reservation)

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
GameplayAction::Reservation::~Reservation()
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
GameplayAction::Reservation::equal(GameplayAction const& action) const
{
  return (   (this->GameplayAction::equal(action))
	  && (this->reservation == static_cast<GameplayAction::Reservation const&>(action).reservation));
} // bool GameplayAction::Reservation::equal(GameplayAction action) const

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
GameplayAction::Reservation::write(ostream& ostr) const
{
  ostr << this->type;
  ostr << " = " << this->player;
  ostr << ", " << this->reservation.game_type;
  if (this->reservation.game_type == GAMETYPE::MARRIAGE)
    ostr << ", " << this->reservation.marriage_selector;
  if (this->reservation.swines)
    ostr << ", swines";
  if (this->reservation.hyperswines)
    ostr << ", hyperswines";

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
GameplayAction::Reservation::data_translation() const
{
  Translator::Translation translation(::translator);

  translation += DK::Utils::String::to_string(this->player);

  translation += ", " + ::translation(this->reservation.game_type);

  if (   (this->reservation.game_type == GAMETYPE::MARRIAGE)
      || (this->reservation.game_type == GAMETYPE::MARRIAGE))
    translation += ", " + ::translation(this->reservation.marriage_selector);

  if (this->reservation.swines)
    translation += ", " + ::translation("swines");
  if (this->reservation.hyperswines)
    translation += ", " + ::translation("hyperswines");

  return translation;
} // virtual Translator::Translation GameplayAction::Reservation::data_translation() const
