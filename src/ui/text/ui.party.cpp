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
#include "party.h"

#include "../../misc/translations.h"

namespace UI_TEXT_NS {

  /**
   **
   ** the party is opened
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
    UI_Text::party_open()
    {
#ifndef POSTPONED
      this->ostr() << "The party is opened." << endl;
#endif
      
      return ;
    } // void UI_Text::party_open()

  /**
   **
   ** gets and sets the settings of the party:
   **   names, types and configuration of the players,
   **   rules,
   **   starting seed
   **   startplayer
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
    UI_Text::party_get_settings()
    {
      this->party->get_settings();

      return ;
    } // void UI_Text::party_get_settings()

  /**
   **
   ** the party is started:
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
    UI_Text::party_start()
    { 
#ifndef POSTPONED
      this->ostr() << "The party is started." << endl;
#endif

      return ;
    } // void UI_Text::party_start()

  /**
   **
   ** the party is finished
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
    UI_Text::party_finish()
    {
#ifndef POSTPONED
      this->ostr() << "The party is finished." << endl;
#endif

      return ;
    } // void UI_Text::party_end()

  /**
   **
   ** the party is closed
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
    UI_Text::party_close()
    {
#ifndef POSTPONED
      this->ostr() << "The party is closed." << endl;
#endif

      return ;
    } // void UI_Text::party_close()

} // namespace UI_TEXT_NS

#endif // #ifdef USE_UI_TEXT
