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
 **********************************************************************/

#ifndef CLASS_RESERVATION_HEADER
#define CLASS_RESERVATION_HEADER

#include "../basistypes.h"

class Reservation {
  public:
    // default constructor
    Reservation();
    // constructor
    Reservation(string const& line);
    // destructor
    ~Reservation();

    // write the reservation in the stream
    void write(ostream& ostr) const;


    // the game type
    GameType game_type;
    // the marriage selector
    MarriageSelector marriage_selector;
    // the offer the duty solo
    bool offer_duty_solo;
    // whether swines should be announced
    bool swines;
    // whether hyperswines should be announced
    bool hyperswines;
}; // class Reservation

// whether the two reservations are equal
bool operator==(Reservation const& reservation_a,
		Reservation const& reservation_b);
// write the reservation in the stream
ostream& operator<<(ostream& ostr, Reservation const& reservation);

#endif // #ifndef CLASS_RESERVATION_HEADER
