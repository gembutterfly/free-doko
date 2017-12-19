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
   ** genscher: -> result
   **
   ** @param	-
   **
   ** @return	pointer to the new teammate,
   **		NULL if the genscher is not to be announced
   **
   ** @author	Diether Knof
   **
   ** @version	0.5.4
   **
   ** @todo	all
   **
   **/
  Player const*
    UI_Text::genscher_partner()
    {
      return NULL;
    } // Player const* UI_Text::genscher_partner()

  /**
   **
   ** genscher: Information of the new team
   **
   ** @param	player	genscher player
   ** @param	partner	partner of the gensher player
   **
   ** @return	-
   **
   ** @author	Diether Knof
   **
   ** @version	0.5.4
   **
   ** @todo	all
   **
   **/
  void
    UI_Text::genscher(Player const& player,
                      Player const& partner)
    {
      return ;
    } // void UI_Text::genscher(Player const& player, Player const& partner)

} // namespace UI_TEXT_NS

#endif // #ifdef USE_UI_TEXT
