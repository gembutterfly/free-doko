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

#include "bug_report.h"

#include "../../os/bug_report.h"
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
    UI_Text::BugReport::BugReport(UI_Text* const ui) :
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
    UI_Text::BugReport::~BugReport()
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
   **/
    bool
      UI_Text::BugReport::interpret_line()
      {
	// whether the line has been interpreted
	bool interpreted = false;

	if (this->ui->iskeyword("bug report")) {
	  this->create();
	  interpreted = true;
	} // if (this->ui->iskeyword("help"))

	return interpreted;
      } // bool UI_Text::BugReport::interpret_line()

  /**
   **
   ** create a bug report
   **
   ** @param	-
   **
   ** @return	the file in which the bug report is written
   **
   ** @version	0.5.4
   **
   ** @author	Diether Knof
   **
   **/
    string
      UI_Text::BugReport::create()
      {
	this->ui->ostr() << ::translation("Creating bug-report.") << '\n'
	  << ::translation("Please type your message.") << '\n';

	string line; // the read line
	string message; // the message
	while (this->ui->istr().good()) {
	  std::getline(this->ui->istr(), line);
	  if (line == ".")
	    break;

	  message += line;
	  message += "\n";
	} // while (this->ui->istr().good())

	string const filename = ::bug_report->report(message);

	this->ui->ostr() << ::translation("The bug report is written into the file '%sfile%'.", filename.c_str())
	  << '\n';

	return filename;
      } // string UI_Text::BugReport::create()

} // namespace UI_TEXT_NS

#endif // #ifdef USE_UI_TEXT
