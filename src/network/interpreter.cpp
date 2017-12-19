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

#include "interpreter.h"
#include "sender.h"
#include "receiver.h"

#include "FreeDoko/interpreter.h"
#ifdef USE_NETWORK_DOKOLOUNGE
#include "DokoLounge/interpreter.h"
#endif

namespace Network {

    /**
     ** create an interpreter
     **
     ** @param     connection   corresponding connection
     ** @param     type         interpreter type
     **
     ** @return    -
     **
     ** @author    -
     **
     ** @version   0.7.3
     **/
  Connection::Interpreter*
    Connection::Interpreter::new_(Connection& connection,
                                  InterpreterType const type)
    {
      switch (type) {
      case FREEDOKO:
        return new FreeDoko::Interpreter(connection);
#ifdef USE_NETWORK_DOKOLOUNGE
      case DOKOLOUNGE:
        return new DokoLounge::Interpreter(connection);
#endif
      } // switch type

      return 0;
    } // Connection::Interpreter* Connection::Interpreter::new_(Connection& connection, InterpreterType type)

} // namespace Network

#endif // #ifdef USE_NETWORK
