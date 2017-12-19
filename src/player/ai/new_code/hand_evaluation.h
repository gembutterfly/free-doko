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

#ifndef CLASS_HANDEVALUATION_HEADER
#define CLASS_HANDEVALUATION_HEADER

#include "../../basistypes.h"

class Ai;
class Reservation;

class HandEvaluation {
  public:

    // evaluates the hand of the given ai to determine if a reservation should be made
    static Reservation evaluate_reservation(Ai const& ai, bool const is_duty_solo);

    // evaluates the hand of the given ai to determine if an announcement should be made
    static Announcement evaluate_announcement(Ai const& ai);

    // evaluate the hand of the given ai to check if a poverty can be accepted
    static bool evaluate_accept_poverty(Ai const& ai, unsigned const cardno);
};


#endif // CLASS_HANDEVALUATION_HEADER
