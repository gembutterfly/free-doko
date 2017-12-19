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

#ifndef CLASS_NETWORK_CONNECTION_INTERPRETER_RECEIVER_HEADER
#define CLASS_NETWORK_CONNECTION_INTERPRETER_RECEIVER_HEADER

#include "interpreter.h"

namespace Network {

  /**
   ** base class for an the incoming messages
   **
   ** @author	Diether Knof
   **/
  class Connection::Interpreter::Receiver {
    public:
      // constructor
      Receiver(Interpreter& interpreter) :
	interpreter_(&interpreter)
      { }

      // destructor
      virtual ~Receiver()
      { }

      // the corresponding connection
      Connection& connection()
      { return this->interpreter().connection(); }

      // the corresponding actor
      Connection::Actor& actor()
      { return this->interpreter().connection().actor(); }

      // the text was received
      virtual bool received(string const& text) = 0;

    private:
      // the corresponding interpreter
      PRIV_VAR_P_RW(Interpreter, interpreter);

    private: // unused
      Receiver();
      Receiver(Receiver const&);
      Receiver& operator=(Receiver const&);
  }; // class Connection::Interpreter::Receiver

} // namespace Network

#endif // #ifndef CLASS_NETWORK_CONNECTION_INTERPRETER_RECEIVER_HEADER

#endif // #ifdef USE_NETWORK
