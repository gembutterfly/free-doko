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

#include "../connection.h"

namespace Network {
  namespace FreeDoko {

    /**
     ** constructor
     **
     ** @param     connection   corresponding connection
     **
     ** @return    -
     **
     ** @author    -
     **
     ** @version   0.7.3
     **/
    Interpreter::Interpreter(Connection& connection) :
      Connection::Interpreter(connection,
			      *new Sender(*this), *new Receiver(*this) )
    { }

    /**
     ** destructor
     **
     ** @param     -
     **
     ** @return    -
     **
     ** @author    -
     **
     ** @version   0.7.3
     **/
    Interpreter::~Interpreter()
    {
      delete this->sender_;
      delete this->receiver_;
    } // Interpreter::~Interpreter()

    /**
     ** -> result
     **
     ** @param     -
     **
     ** @return    the corresponding sender
     **
     ** @author    -
     **
     ** @version   0.7.3
     **/
    Interpreter::Sender&
      Interpreter::sender()
      { return dynamic_cast<Sender&>(this->Connection::Interpreter::sender()); }

    /**
     ** -> result
     **
     ** @param     -
     **
     ** @return    the corresponding receiver
     **
     ** @author    -
     **
     ** @version   0.7.3
     **/
    Interpreter::Receiver&
      Interpreter::receiver()
      { return dynamic_cast<Receiver&>(this->Connection::Interpreter::receiver()); }

  } // namespace FreeDoko
} // namespace Network

#endif // #ifdef USE_NETWORK
