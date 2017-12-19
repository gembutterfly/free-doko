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

#include "party_points.h"

#include "../party/party.h"
#include "../game/game_summary.h"

namespace OS_NS {

  /**
   **
   ** constructor
   **
   ** @param	ostr	output stream
   **
   ** @return	-
   ** 
   ** @version	0.6.7
   **
   ** @author	Diether Knof
   **
   **/
  PartyPoints::PartyPoints(ostream* ostr) :
    OS(OS_TYPE::SEED),
  ostr(ostr)
  {
    return ;
  } // PartyPoints::PartyPoints()

  /**
   **
   ** destructor
   **
   ** @param	-
   **
   ** @return	-
   ** 
   ** @version	0.6.7
   **
   ** @author	Diether Knof
   **
   **/
  PartyPoints::~PartyPoints()
  {
    return ;
  } // PartyPoints::~PartyPoints()

  /**
   **
   ** the game is finished
   ** show the points
   **
   ** @param	-
   **
   ** @return	-
   ** 
   ** @version	0.6.7
   **
   ** @author	Diether Knof
   **
   **/
  void
    PartyPoints::game_finish()
    {
      GameSummary const& game_summary
	= this->party().last_game_summary();

      *(this->ostr) << setw(5) << ::party.gameno() << ": ";
      for (unsigned p = 0; p < ::party.playerno(); p++)
	*(this->ostr) << setw(8) << game_summary.points(p);
      *(this->ostr) << endl;

      return ;
    } // void PartyPoints::game_finish()

} // namespace OS_NS
