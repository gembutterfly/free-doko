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

#ifndef CLASS_NETWORK_GNET_CONNECTION_HEADER
#define CLASS_NETWORK_GNET_CONNECTION_HEADER

#include <gnet.h>
#include <glib.h>
//#include <glibmm/iochannel.h>
#include "../connection.h"


namespace Network {
  namespace GNet {

    /**
     ** class for connections -- programmed with the gnet library
     **
     ** @author	Diether Knof
     **/
    class Connection : public Network::Connection {
      public:
	static gboolean async_client_iofunc(GIOChannel* iochannel,
					    GIOCondition condition, 
					    gpointer data);
      public:
	// constructor
	Connection(::Network::Server& server,
		   string const& address, unsigned const port,
                   InterpreterType const type);
	// constructor
	Connection(::Network::Server& server, GTcpSocket* client_socket);
	// destructor
	~Connection();

        
        // connect
        bool connect(string const& address, unsigned const port);

        // check for network traffic
        void update();

        // sends 'text' over the connection
        void send(string const& text);

      private:
        // there was traffic on the connection
        bool traffic(GIOCondition condition);

        // create the gnet part of the connection
        void gnet_init();

      private: // gnet part
        GTcpSocket* socket;
      public:
        GIOChannel* iochannel;
        guint watch;

      private: // unused
        Connection();
        Connection(Connection const&);
        Connection& operator=(Connection const&);
    }; // class Connection : public Network::Connection

  } // namespace GNet

} // namespace Network

#endif // #ifndef CLASS_NETWORK_GNET_CONNECTION_HEADE

#endif // #ifdef USE_NETWORK
