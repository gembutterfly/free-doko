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

#include "game.h"

#include "../utils/string.h"

/**
 ** constructor
 **
 ** @param	-
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.3
 **/
Reservation::Reservation() :
  game_type(GAMETYPE::NORMAL),
  marriage_selector(MARRIAGE_SELECTOR::TEAM_SET),
  offer_duty_solo(false),
  swines(false),
  hyperswines(false)
{  }

/**
 ** constructor
 ** the parts of the reservation are split by ',' in 'line'
 **
 ** @param	line   line to read the reservation from
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.3
 **/
Reservation::Reservation(string const& line) :
  game_type(GAMETYPE::NORMAL),
  marriage_selector(MARRIAGE_SELECTOR::TEAM_SET),
  offer_duty_solo(false),
  swines(false),
  hyperswines(false)
{ 
  // syntax:
  //   'game type', 'marriage selector', swines, hyperswines

  // first split the line in the single parts
  vector<string> const parts(DK::Utils::String::split(line, ','));
  vector<string>::const_iterator p = parts.begin();

  this->game_type = GAMETYPE::from_name(*p);
  ++p;

  if (this->game_type == GAMETYPE::MARRIAGE) {
    this->marriage_selector = MARRIAGE_SELECTOR::from_name(*p);
    ++p;
  } // if (this->game_type == GAMETYPE::MARRIAGE)

  for ( ; p != parts.end(); ++p) {
    if ((*p) == "offer duty solo") {
      this->offer_duty_solo = true;
    } else if ((*p) == "swines") {
      this->swines = true;
    } else if ((*p) == "hyperswines"){ 
      this->swines = true;
    } else {
      DEBUG_ASSERTION(false,
		      "Reservation(line)\n"
		      "  unknown part '" << *p << "'");
    }
  } // for (p \in parts)

  return ;
} // Reservation::Reservation(string line)

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
Reservation::~Reservation()
{  }

/**
 ** write the reservation in the stream
 **
 ** @param	ostr          output stream to write into
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.3
 **/
void
Reservation::write(ostream& ostr) const
{
  ostr << "game type: " << this->game_type << '\n';
  if (this->game_type == GAMETYPE::MARRIAGE)
    ostr << "marriage selector: " << this->marriage_selector << '\n';
  if (this->offer_duty_solo)
    ostr << "offer duty solo\n";
  if (this->swines)
    ostr << "swines\n";
  if (this->hyperswines)
    ostr << "hyperswines\n";
  return ;
} // void Reservation::write(ostream& ostr) const

/**
 ** -> result
 **
 ** @param	reservation_a   first reservation
 ** @param	reservation_b   second reservation
 **
 ** @return	whether the reservations are equal
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.3
 **/
bool
operator==(Reservation const& reservation_a,
	   Reservation const& reservation_b)
{
  return (   (reservation_a.game_type
	      == reservation_b.game_type)
	  && (reservation_a.marriage_selector
	      == reservation_b.marriage_selector)
	  && (reservation_a.offer_duty_solo
	      == reservation_b.offer_duty_solo)
	  && (reservation_a.swines
	      == reservation_b.swines)
	  && (reservation_a.hyperswines
	      == reservation_b.hyperswines));
} // bool operator==(Reservation reservation_a, Reservation reservation_b)

/**
 ** write the reservation in the stream
 **
 ** @param	ostr          output stream to write into
 ** @param	reservation   reservation to write
 **
 ** @return	the output stream
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.3
 **/
ostream&
operator<<(ostream& ostr, Reservation const& reservation)
{
  reservation.write(ostr);
  return ostr;
} // ostream& operator<<(ostream& ostr, Reservation reservation)
