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

#ifdef USE_UI_TEXT

#ifndef UI_TEXT_HELP_HEADER
#define UI_TEXT_HELP_HEADER

#include "ui.h"

namespace UI_TEXT_NS {

  /**
   **
   ** @brief	interface for the help
   **
   ** @author	Diether Knof
   **
   **/
  class UI_Text::Help {
    public:
      Help(UI_Text* const ui);

      virtual ~Help();

      // interprets the read line
      virtual bool interpret_line();

    private:
      UI_Text* const ui;

    private: // unused
      Help();
      Help(Help const& ui);
      Help& operator=(Help const& ui);
  }; // class UI_Text::Help

} // namespace UI_TEXT_NS
#endif // #ifdef UI_TEXT_HELP_HEADER

#endif // #ifdef USE_UI_TEXT
