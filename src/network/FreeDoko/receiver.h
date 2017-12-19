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

#ifndef CLASS_NETWORK_CONNECTION_INTERPRETER_FREEDOKO_RECEIVER_HEADER
#define CLASS_NETWORK_CONNECTION_INTERPRETER_FREEDOKO_RECEIVER_HEADER

#include "interpreter.h"
#include "../receiver.h"

namespace Network {
  namespace FreeDoko {

    /**
     ** receiver interpreter for FreeDoko
     **
     ** @author	Diether Knof
     **/
    class Interpreter::Receiver : public Connection::Interpreter::Receiver {
      private:
	// split the line in 'time' and 'message'
	static MessageType split_line(string const& line,
				      Time& time, string& message);

      public:
	// constructor
	Receiver(Interpreter& interpreter);
	// destructor
	~Receiver();

	// the corresponding sender
	Sender& sender();

	// the text was received
	bool received(string const& text);

      private:
	// the text was received
	bool received_line(string const& text);

	// act according to a simple message
	bool act_simple(string const& message);
	// act according to a special message
	bool act_special(string const& message);
	// act in the right order
	bool act_normal_in_order(Time const& time, unsigned const& counter,
				 string const& message);
	// act according to a normal message
	bool act_normal(string const& message);

      private:
	// a line was not send in total
	string pending_line;
	// a parser
	Network::FreeDoko::Parser::Base* parser;

      private: // unused
	Receiver();
	Receiver(Receiver const&);
	Receiver& operator=(Receiver const&);
    }; // class Interpreter::Receiver : public Connection::Interpreter::Receiver 

  } // namespace FreeDoko
} // namespace Network

#endif // #ifndef CLASS_NETWORK_CONNECTION_INTERPRETER_FREEDOKO_RECEIVER_HEADER

#endif // #ifdef USE_NETWORK
