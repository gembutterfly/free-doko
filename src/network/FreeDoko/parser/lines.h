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

#ifndef CLASS_NETWORK_CONNECTION_INTERPRETER_FREEDOKO_PARSER_LINES_HEADER
#define CLASS_NETWORK_CONNECTION_INTERPRETER_FREEDOKO_PARSER_LINES_HEADER

#include "parser.h"

namespace Network {
  namespace FreeDoko {
    namespace Parser {

    /**
     ** base parser for parsing (nested) single lines
     **
     ** @author   Diether Knof
     **/
    class Lines : public Base {
      public:
	// constructor
	Lines(Connection::Interpreter& interpreter);
	// destructor
	~Lines();

	// interpret the given text
	bool interpret(string const& text);
	// interpret the given line
	virtual bool interpret_line(string const& line) = 0;

        // the parsing is finished
        virtual void finish();

      private:
	// the depth in the blocks
	unsigned depth;

      private: // unused
	Lines();
	Lines(Lines const&);
	Lines& operator=(Lines const&);
    }; // class Network::Interpreter::FreeDoko::Parser::Lines : public Base

    } // namespace Parser
  } // namespace FreeDoko
} // namespace Network

#endif // #ifndef CLASS_NETWORK_CONNECTION_INTERPRETER_FREEDOKO_PARSER_LINES_HEADER

#endif // #ifdef USE_NETWORK
