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

#include "seed.h"

#include "../game/game.h"

#include <ctime>

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
  Seed::Seed(ostream* ostr) :
    OS(OS_TYPE::SEED),
  ostr(ostr)
  {
    return ;
  } // Seed::Seed()

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
  Seed::~Seed()
  {
    return ;
  } // Seed::~Seed()

  /**
   **
   ** the game is opened
   ** show the seed and the startplayerno
   **
   ** @param	game	game that is opened
   **
   ** @return	-
   ** 
   ** @version	0.6.7
   **
   ** @author	Diether Knof
   **
   **/
  void
    Seed::game_open(Game const& game)
    {
      this->OS::game_open(game);

      time_t const time_now = time(NULL);
      *(this->ostr) << "seed: " << setw(8) << this->game().seed()
	<< " (" << this->game().no(this->game().startplayer()) << ")"
	<< "\t"
	<< ctime(&time_now)
	<< flush;

      return ;
    } // void Seed::game_open(Game const& game)

  /**
   **
   ** the game is opened
   ** show the gametype (and the soloplayernumber)
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
    Seed::game_start()
    {
      *this->ostr << "  " << this->game().type();

      if (is_solo(this->game().type()))
	*this->ostr << "\t"
	  << "(soloplayer: "
	  << this->game().no(this->game().soloplayer())
	  << ")";
      *this->ostr << "\n" << flush;

      return ;
    } // void Seed::game_start(Game& game)

} // namespace OS_NS
