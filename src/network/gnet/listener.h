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

#ifndef CLASS_NETWORK_GNET_LISTENER_HEADER
#define CLASS_NETWORK_GNET_LISTENER_HEADER

#include <gnet.h>
#include "../listener.h"

namespace Network {
  namespace GNet {

    class Listener : public Network::Listener {
      private:
	static void async_accept_connection(GTcpSocket* listener_socket,
					    GTcpSocket* client_socket,
					    gpointer listener_ptr);
      public:
	// constructor
	Listener(Server& server, unsigned const port);
	// destructor
	~Listener();

	// check for data on the connections
	void update();

	// create a connection
	void create_connection(GTcpSocket* client_socket);

      private: // gnet part

	// local internet address
	GInetAddr* gnet_local_addr;
	// listener
	GTcpSocket* gnet_server;
    }; // class Listener : public Network::Listener

  } // namespace GNet

} // namespace Network

#endif // #ifndef CLASS_NETWORK_GNET_LISTENER_HEADER

#endif // #ifdef USE_NETWORK
