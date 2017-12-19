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

#ifndef CLASS_NETWORK_FILE_CONNECTION_HEADER
#define CLASS_NETWORK_FILE_CONNECTION_HEADER

#include "../connection.h"

namespace Network {
  namespace File {

    /**
     ** class for connections -- from a file
     **
     ** @author	Diether Knof
     **/
    class Connection : public Network::Connection {
      public:
	// constructor
	Connection(::Network::Server& server,
		   string const& filename, unsigned const port,
                   InterpreterType const type);
	// destructor
	~Connection();

        // check for network traffic
        void update();

      private:
        istream* istr;

      private: // unused
        Connection();
        Connection(Connection const&);
        Connection& operator=(Connection const&);
    }; // class Connection : public Network::Connection

  } // namespace File

} // namespace Network

#endif // #ifndef CLASS_NETWORK_FILE_CONNECTION_HEADE

#endif // #ifdef USE_NETWORK
