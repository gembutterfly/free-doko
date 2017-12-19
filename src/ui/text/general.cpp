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

#include "constants.h"

#ifdef USE_UI_TEXT

#include "general.h"

#include "../../basistypes.h"
#include "../../misc/translations.h"
#include "../../utils/string.h"

namespace UI_TEXT_NS {

  /**
   **
   ** constructor
   **
   ** @param	ui	pointer to the ui
   **
   ** @return	-
   **
   ** @version	0.5.4
   **
   ** @author	Diether Knof
   **
   **/
    UI_Text::General::General(UI_Text* const ui) :
      ui(ui)
    { }

  /**
   **
   ** destructor
   **
   ** @param	-
   **
   ** @return	-
   **
   ** @version	0.5.4
   **
   ** @author	Diether Knof
   **
   **/
    UI_Text::General::~General()
    { }

  /**
   **
   ** interprets the line read by the ui
   **
   ** @param	-
   **
   ** @return	whether the line was interpreted
   **
   ** @version	0.5.4
   **
   ** @author	Diether Knof
   **
   ** @todo	mostly all
   **
   **/
    bool
      UI_Text::General::interpret_line()
      {
	// whether the line has been interpreted
	bool interpreted = false;

	if ((this->ui->line.size() > 0)
	    && (this->ui->line[0] == '!')) {
	  // system call
	  (void)::system(string(this->ui->line, 1).c_str());
	  interpreted = true;
	} else if (this->ui->iskeyword("quit")) {
	  ::game_status = GAMESTATUS::QUIT;
	  interpreted = true;
#ifndef RELEASE
	} else if (DK::Utils::String::word_first(this->ui->line)
		   == "trans") {
	  string text = this->ui->line;
	  DK::Utils::String::word_first_remove_with_blanks(text);
	  COUT << ::translator.name() << ": " << ::translation(text) << endl;
	  interpreted = true;
#endif
	} // if (line == ...)

	return interpreted;
      } // bool UI_Text::General::interpret_line()

} // namespace UI_TEXT_NS

#endif // #ifdef USE_UI_TEXT
