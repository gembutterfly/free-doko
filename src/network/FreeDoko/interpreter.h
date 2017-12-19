/**********************************************************************
 *
 *   Interpreter a Doppelkopf-Game
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

#ifndef CLASS_NETWORK_CONNECTION_INTERPRETER_FREEDOKO_HEADER
#define CLASS_NETWORK_CONNECTION_INTERPRETER_FREEDOKO_HEADER

#include "../interpreter.h"

namespace Network {
  namespace FreeDoko {
    namespace Parser {
      // needed by 'receiver.h'
      class Base;
    }

  /**
   ** interpreter for Interpreter
   **
   ** @author	Diether Knof
   **/
  class Interpreter : public Connection::Interpreter {
    public:
      /**
       ** the type of the message
       **/
      enum MessageType {
	UNKNOWN, // unknown message (or error)
	SIMPLE,  // simple message:  just the message
	SPECIAL, // special message: '*' + time + ': ' + message
	NORMAL   // normal message:  time + ': ' + message
      }; // enum MessageType

    public:
      // packet
      struct Packet {
	Packet(Time const& time, unsigned const number, string const& message) :
	  time(time), number(number), message(message)
	{ }

	Time time;		// when the packet was sent (seconds)
	unsigned number;	// number of the packet
	string message;		// packet message
      };

    public:
      class Sender;
      class Receiver;

    public:
      // constructor
      Interpreter(Connection& connection);

      // destructor
      ~Interpreter();

      // variables
      Sender& sender();
      Receiver& receiver();

    private: // unused
      Interpreter();
      Interpreter(Interpreter const&);
      Interpreter& operator=(Interpreter const&);
  }; // class Interpreter : public Connection::Interpreter

  } // namespace FreeDoko
} // namespace Network

#endif // #ifndef CLASS_NETWORK_CONNECTION_INTERPRETER_FREEDOKO_HEADER

#endif // #ifdef USE_NETWORK
