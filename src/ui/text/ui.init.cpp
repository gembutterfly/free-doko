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

#include "general.h"
#include "help.h"
#include "bug_report.h"
#include "settings.h"
#include "party.h"

namespace UI_TEXT_NS {

  /**
   **
   ** Constructor
   **
   ** @param	-
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
  UI_Text::UI_Text() :
    UI(UI_TYPE::TEXT),
    istr_(&cin),
    ostr_(&cout),
    prompt("FreeDoko"),
    line(),
    line_name(),
    line_value(),
    general(new General(this)),
    help(new Help(this)),
    bug_report(new BugReport(this)),
    settings(new Settings(this)),
    party(new Party(this))
  { }

  /**
   **
   ** Destruktor
   **
   ** @param	-
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
  UI_Text::~UI_Text()
  {
    delete this->general;
    delete this->help;

    return ;
  } // UI_Text::~UI_Text()


  /**
   **
   ** Initialises the UI with the command line arguments.
   ** Nothing to do
   **
   ** @param	argc	number of arguments
   ** @param	argv	arguments
   **
   ** @return	-
   ** 
   ** @author	Diether Knof
   **
   ** @version	0.5.4
   **
   **/
  void
    UI_Text::init(int& argc, char**& argv)
    {
#ifndef POSTPONED
      cerr << "Warning:\n"
        << "  This interface (text) is still under developement.\n"
        << "  There are many parts that do not work, yet.\n"
        << endl;
#endif

      return ;
    } // UI_Text::init(argc, argv)

} // namespace UI_TEXT_NS

#endif // #ifdef USE_UI_TEXT
