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

#ifdef LINUX
// for 'int nanosleep()'
#include <ctime>
#endif // #ifdef LINUX

namespace UI_TEXT_NS {

  /**
   **
   ** -> result
   **
   ** @param	-
   **
   ** @return	output stream
   **
   ** @version	0.5.4
   **
   ** @author	Diether Knof
   **
   **/
  ostream&
    UI_Text::ostr()
    {
      return *this->ostr_;
    } // ostream& UI_Text::ostr()

  /**
   **
   ** -> result
   **
   ** @param	-
   **
   ** @return	input stream
   **
   ** @version	0.5.4
   **
   ** @author	Diether Knof
   **
   **/
  istream&
    UI_Text::istr()
    {
      return *this->istr_;
    } // istream& UI_Text::istr()

  /**
   **
   ** sleeps the given time
   ** (in miliseconds, UINT_MAX for infinity)
   **
   ** @param	sleep_usec	time to sleep
   **
   ** @return	-
   **
   ** @version	0.5.4
   **
   ** @author	Diether Knof
   **
   ** @todo	Windows
   **
   **/
  void
    UI_Text::sleep(unsigned const sleep_usec)
    {
#ifdef LINUX
      timespec const req = { 0, (sleep_usec * 1000) };
      nanosleep(&req, NULL);
#endif // #ifdef LINUX

      return ;
    } // void UI_Text::sleep(unsigned const sleep_usec)

  /**
   **
   ** load the page with the help
   **
   ** @param	page	the page to be loaded
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
    UI_Text::help_load(string const& page)
    { }

} // namespace UI_TEXT_NS

#endif // #ifdef USE_UI_TEXT
