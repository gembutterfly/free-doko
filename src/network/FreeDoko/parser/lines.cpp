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

#ifdef USE_NETWORK

#include "constants.h"

#include "lines.h"
#include "../../server.h"

namespace Network {
  namespace FreeDoko {
    namespace Parser {

      /**
       ** constructor
       **
       ** @param     interpreter	corresponding interpreter
       **
       ** @return    -
       **
       ** @author    Diether Knof
       **
       ** @version   0.7.5
       **/
      Lines::Lines(Connection::Interpreter& interpreter) :
	Base(interpreter),
	depth(0)
      { }

      /**
       ** destructor
       **
       ** @param     -
       **
       ** @return    -
       **
       ** @author    Diether Knof
       **
       ** @version   0.7.5
       **/
      Lines::~Lines()
      { }

      /**
       ** interprets the text and acts accodingly
       **
       ** @param     text   text to interpret
       **
       ** @return    whether the text could be interpreted
       **
       ** @author    Diether Knof
       **
       ** @version   0.7.5
       **/
      bool
	Lines::interpret(string const& text)
	{
	  if (  (text == "")
	     || (text[0] == '#') ) {
	    return true; 
	  } else if (text == "{") {
	    depth += 1;
	    return true;
	  } else if (text == "}") {
	    if (depth == 0) {
              this->finish();
	      return false;
	    }
	    depth -= 1;
	    if (depth == 0)
              this->finish();

	    return true;
	  } // if (text == "...")

	  return this->interpret_line(text);
	} // bool Lines::interpret(string text)

      /**
       ** finish the parsing
       **
       ** @param     -
       **
       ** @return    -
       **
       ** @author    Diether Knof
       **
       ** @version   0.7.5
       **/
      void
        Lines::finish()
        {
          this->finished_ = true;
          return ;
        } // void Lines::finish()

    } // namespace Parser
  } // namespace FreeDoko
} // namespace Network

#endif // #ifdef USE_NETWORK
