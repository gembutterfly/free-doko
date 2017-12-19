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
 ********************************************************************/

#ifdef USE_NETWORK

#include "constants.h"

#include "listener.h"
#include "connection.h"


namespace Network {
  namespace GNet {

    /**
     ** constructor
     **
     ** @param     server   the corresponding server
     ** @param     port     the port to listen at
     **
     ** @return    -
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.2
     **/
    Listener::Listener(Server& server, unsigned const port) :
      Network::Listener(server, port),
      gnet_local_addr(NULL),
      gnet_server(NULL)
    {
      gnet_init();

      // create a server
      this->gnet_server = gnet_tcp_socket_server_new_with_port(this->port());
      if (this->gnet_server == NULL) {
	throw ListenerFailureException();
      }
      // get the local internet address
      this->gnet_local_addr
	= gnet_tcp_socket_get_local_inetaddr(this->gnet_server);


      // Wait asyncy for incoming clients
      gnet_tcp_socket_server_accept_async(this->gnet_server,
					  Network::GNet::Listener::async_accept_connection,
					  this);

      return ;
    } // Listener::Listener()

    /**
     ** destructor
     **
     ** @param     -
     **
     ** @return    -
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.2
     **/
    Listener::~Listener()
    {
      if (this->gnet_server) {
	gnet_tcp_socket_server_accept_async_cancel(this->gnet_server);
	gnet_tcp_socket_delete(this->gnet_server);
	this->gnet_server = NULL;
      }
    } // Listener::~Listener()


#ifdef POSTPONED
    /**
     ** -> result
     **
     ** @param     -
     **
     ** @return    the names of the addresses of this computer
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.2
     **/
    vector<string>
      Listener::local_addresses() const
      {
	set<string> addresses;

	{
	  GList* interfaces = gnet_inetaddr_list_interfaces();
	  for (GList* i = g_list_first(interfaces);
	       i != NULL;
	       i = g_list_next(i)) {
	    GInetAddr* const address = static_cast<GInetAddr*>(i->data);
	    addresses.insert(gnet_inetaddr_get_canonical_name(address));
#ifndef WINDOWS
	    addresses.insert(gnet_inetaddr_get_name(address));
#endif
	  }
	  g_list_free(interfaces);
	}

	{ // localhost
	  GInetAddr* const address = gnet_inetaddr_get_host_addr();
	  if (address) {
	    addresses.insert(gnet_inetaddr_get_canonical_name(address));
#ifndef WINDOWS
	    addresses.insert(gnet_inetaddr_get_name(address));
#endif
	  }
	} // localhost

	addresses.erase(string());
	addresses.erase("127.0.0.1");
	addresses.erase("localhost");

	return vector<string>(addresses.begin(), addresses.end());
      } // vector<string> Listener::local_addresses() const
#endif

    /**
     ** check for a connection
     **
     ** @param     -
     **
     ** @return    -
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.0
     **/
    void
      Listener::update()
      {
	g_main_context_iteration(NULL, false);
	return ;
      } // void Listener::update()

    /**
     ** accept a connection
     **
     ** @param     server_socket   the server which gets the connection
     ** @param     client_socket   the client which ask for a connection
     ** @param     listener_ptr    pointer to the listener
     **
     ** @return    -
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.2
     **/
    void
      Listener::async_accept_connection(GTcpSocket* server_socket,
					GTcpSocket* client_socket,
					gpointer listener_ptr)
      {
	static_cast<Listener*>(listener_ptr)->create_connection(client_socket);

	return ;
      } // static void Listener::async_accept_connection(GTcpSocket* server_socket, GTcpSocket* client_socket, gpointer listener_ptr)

    /**
     ** create a connection
     **
     ** @param     client_socket   the client which ask for a connection
     **
     ** @return    -
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.2
     **/
    void
      Listener::create_connection(GTcpSocket* client_socket)
      {
	this->server->new_connection(new Connection(*this->server,
						    client_socket));

	return ;
      } // static void Listener::async_accept_connection(GTcpSocket* server_socket, GTcpSocket* client_socket, gpointer listener_ptr)

  } // namespace GNet

} // namespace Network

#endif // #ifdef USE_NETWORK
