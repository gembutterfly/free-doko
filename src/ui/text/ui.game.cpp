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

#ifdef USE_UI_TEXT

#include "ui.h"

namespace UI_TEXT_NS {

  /**
   **
   ** the game is opened
   **
   ** @param	-
   **
   ** @return	-
   **
   ** @version	0.5.4
   **
   ** @author	Diether Knof
   **
   ** @todo	all
   **
   **/
  void
    UI_Text::game_open()
    { }

  /**
   **
   ** Gets the reservation of the given player:
   ** pops up a window
   ** This is the first time, the hands can (should) be shown.
   **
   ** @param	player	player, whose reservation schould be get
   **
   ** @return	reservation of the player
   **
   ** @version	0.5.4
   **
   ** @author	Diether Knof
   **
   ** @todo	all
   **
   **/
  Reservation
    UI_Text::reservation_get(Player const& player)
    {
      return Reservation();
    } // Reservation UI_Text::reservation_get(player)


  /**
   **
   ** the game is started:
   ** the gametype and announcements of the other players are shown
   **
   ** @param	-
   **
   ** @return	-
   **
   ** @version	0.5.4
   **
   ** @author	Diether Knof
   **
   ** @todo	all
   **
   **/
  void
    UI_Text::game_start()
    { }

  /**
   **
   ** the game is finished:
   ** the winner and the points of the game are shown.
   **
   ** @param	-
   **
   ** @return	-
   **
   ** @version	0.5.4
   **
   ** @author	Diether Knof
   **
   ** @todo	all
   **
   **/
  void
    UI_Text::game_finish()
    {  }

  /**
   **
   ** the game is closed
   **
   ** @param	-
   **
   ** @return	-
   **
   ** @version	0.5.4
   **
   ** @author	Diether Knof
   **
   ** @todo	all
   **
   **/
  void
    UI_Text::game_close()
    { }

} // namespace UI_TEXT_NS

#endif // #ifdef USE_UI_TEXT
