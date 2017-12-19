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
#ifdef USE_NETWORK_DOKOLOUNGE

#ifndef CLASS_NETWORK_CONNECTION_INTERPRETER_DOKOLOUNGE_HEADER
#define CLASS_NETWORK_CONNECTION_INTERPRETER_DOKOLOUNGE_HEADER

#include "../interpreter.h"

namespace Network {
  namespace DokoLounge {

    /**
     ** interpreter for Dokolounge
     **
     ** @author	Diether Knof
     **/
    class Interpreter : public Connection::Interpreter {
      public:
        class Sender;
        class Receiver;

        static string const address;  // internet address of the server
        static unsigned const port;   // port of the server
        static string const version;  // version of the client

      public:
        // constructor
        Interpreter(Connection& connection);

        // destructor
        ~Interpreter();

        // close the connection
        void close_connection();

        // variables
        Sender& sender();
        Receiver& receiver();

      private: // unused
        Interpreter();
        Interpreter(Interpreter const&);
        Interpreter& operator=(Interpreter const&);
    }; // class Interpreter : public Connection::Interpreter

  } // namespace DokoLounge
} // namespace Network

#endif // #ifndef CLASS_NETWORK_CONNECTION_INTERPRETER_DOKOLOUNGE_HEADER

#endif // #ifdef USE_NETWORK_DOKOLOUNGE
#endif // #ifdef USE_NETWORK
