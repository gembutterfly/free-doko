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

#ifndef CLASS_NETWORK_CONNECTION_INTERPRETER_HEADER
#define CLASS_NETWORK_CONNECTION_INTERPRETER_HEADER

#include "connection.h"

namespace Network {
  /**
   ** base class for an interpreter for network traffic
   **
   ** @author	Diether Knof
   **/
  class Connection::Interpreter {

    public:
      class Sender;
      class Receiver;

    public:
      // create a new interpreter
      static Interpreter* new_(Connection& connection,
                               InterpreterType const type);

    public:
      // constructor
      Interpreter(Connection& connection, Sender& sender, Receiver& receiver) :
        connection_(&connection),
        sender_(&sender), receiver_(&receiver)
    { }

      // destructor
      virtual ~Interpreter()
      { }

      // close the connection
      virtual void close_connection() { }

    private:
      // the corresponding connection
      PRIV_VAR_P_RW(Connection, connection);

      // the interpreter for sending text
      PROT_VAR_P_RW(Sender, sender);
      // the interpreter for received text
      PROT_VAR_P_RW(Receiver, receiver);

    private: // unused
      Interpreter();
      Interpreter(Interpreter const&);
      Interpreter& operator=(Interpreter const&);
  }; // class Connection::Interpreter

} // namespace Network

#endif // #ifndef CLASS_NETWORK_CONNECTION_INTERPRETER_HEADER

#endif // #ifdef USE_NETWORK
