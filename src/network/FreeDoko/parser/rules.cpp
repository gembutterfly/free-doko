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

#include "rules.h"
#include "../../server.h"

#include "../../../party/party.h"
#include "../../../party/rule.h"

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
      Rules::Rules(Connection::Interpreter& interpreter) :
	Lines(interpreter)
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
      Rules::~Rules()
      { }

      /**
       ** interprets the line and set the rule accodingly
       **
       ** @param     line   line to interpret
       **
       ** @return    whether the text could be interpreted
       **
       ** @author    Diether Knof
       **
       ** @version   0.7.5
       **/
      bool
	Rules::interpret_line(string const& line)
	{
	  return this->actor().set_rule(line);
	} // bool Rules::interpret_line(string line)

    } // namespace Parser
  } // namespace FreeDoko
} // namespace Network

#endif // #ifdef USE_NETWORK
