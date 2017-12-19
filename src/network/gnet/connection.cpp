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

#include "connection.h"
#include "../server.h"
#include "../types.h"
#include "../exceptions.h"

#include "../../ui/ui.h"

#include <cstring>

namespace Network {
  namespace GNet {

    /**
     ** constructor
     **
     ** @param     server    the corresponding server
     ** @param     address   address to connect to
     ** @param     port      port to connect to
     ** @param     type      interpreter type
     **
     ** @return    -
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.12
     **/
    Connection::Connection(::Network::Server& server,
                           string const& address, unsigned const port,
                           InterpreterType const type) :
      Network::Connection(server, address, port, type),
      socket(),
      iochannel(NULL),
      watch(0)
    {
      this->socket = gnet_tcp_socket_connect(address.c_str(), port);
      if (!this->socket)
        throw ConnectionFailureException();

      this->set_relation(PARENT);

      this->gnet_init();
    } // Connection::Connection(Server& server, string address, unsigned port, InterpreterType type)

    /**
     ** constructor
     **
     ** @param		server		corresponding server
     ** @param		client_socket	socket of the client
     **
     ** @return		-
     **
     ** @author		Diether Knof
     **
     ** @version	0.7.0
     **/
    Connection::Connection(::Network::Server& server,
                           GTcpSocket* client_socket) :
      Network::Connection(server),
      socket(client_socket),
      iochannel(NULL),
      watch(0)
    {
      if (client_socket == NULL) {
        cerr << "Connection error" << endl;
        exit (EXIT_FAILURE);
      }

      this->set_relation(CHILD);

      this->gnet_init();
    } // Connection::Connection()

    /**
     ** destructor
     **
     ** @param	-
     **
     ** @return	-
     **
     ** @author	Diether Knof
     **
     ** @version	0.7.0
     **/
    Connection::~Connection()
    {
      g_source_remove(this->watch);
      g_io_channel_shutdown(this->iochannel, false, NULL);
      gnet_tcp_socket_delete(this->socket);
    } // Connection::~Connection()

    /**
     ** init the gnet part
     **
     ** @param		-
     **
     ** @return		-
     **
     ** @author		Diether Knof
     **
     ** @version	0.7.0
     **/
    void
      Connection::gnet_init()
      {
        { // get some information of the client
          // internet address of the client
          GInetAddr*
            client_address = gnet_tcp_socket_get_remote_inetaddr(this->socket);
          if (client_address == NULL) {
            cerr << "Error getting the client address" << endl;
            exit(EXIT_FAILURE);
          }

          { // address of the connected computer
            //gchar* name = gnet_inetaddr_get_canonical_name(client_address);
            gchar* name = gnet_inetaddr_get_name(client_address);
            this->address_ = name;
            this->port_ = gnet_inetaddr_get_port(client_address);

            // first set the name to the address
            this->set_name(this->address());

            g_free(name);
            gnet_inetaddr_delete(client_address);
          }
        } // get some information of the client

        // create an io channel to the client
        this->iochannel = gnet_tcp_socket_get_io_channel(this->socket);
        if (this->iochannel == NULL) {
          cerr << "Error getting the client iochannel" << endl;
          exit(EXIT_FAILURE);
        }

        this->watch = 
          g_io_add_watch(this->iochannel,
                         static_cast<GIOCondition>(G_IO_IN
                                                   //| G_IO_OUT
                                                   | G_IO_ERR
                                                   | G_IO_HUP
                                                   | G_IO_NVAL), // watch flags
                         async_client_iofunc, this);

        if (this->name().empty()) {
          cerr << "Error getting the client name" << endl;
#ifdef POSTPONED
          exit(EXIT_FAILURE);
#endif
        }
        if (this->port() == 0) {
          cerr << "Error getting the client port" << endl;
#ifdef POSTPONED
          exit(EXIT_FAILURE);
#endif
        }

        return ;
      } // void Connection::gnet_init()

    /**
     ** check for network traffic
     **
     ** @param     -
     **
     ** @return    -
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.9
     **/
    void
      Connection::update()
      {
        g_main_context_iteration(NULL, false);
        return ;
      } // void Connection::update()

    /**
     ** sends the text over the connection
     **
     ** @param	text	text to send
     **
     ** @return	-
     **
     ** @author	Diether Knof
     **
     ** @version	0.7.2
     **/
    void
      Connection::send(string const& text)
      {
        if (!this->accepted())
          return ;

        GIOError error = G_IO_ERROR_NONE; // error code
        gsize bytes_written = 0; // written bytes

        { // Write the data into the stream
          gchar* buffer = new gchar[text.size()];
          if (buffer == NULL) {
            cerr << "Memory problems.\n" << endl;
            exit(EXIT_FAILURE);
          }
          strncpy(buffer, text.c_str(), text.size());
#ifndef OUTDATED
          error = gnet_io_channel_writen(this->iochannel,
                                         buffer,
                                         text.size(),
                                         &bytes_written);
#else
          {
            GError* error = NULL;
            GIOStatus const status
              = g_io_channel_write_chars(this->iochannel,
                                         buffer,
                                         text.size(),
                                         &bytes_written,
                                         &error);
          }
#endif

          delete[] buffer;
          buffer = NULL;
        } // Write the data into the stream

        if (error != G_IO_ERROR_NONE) {
          cerr << "Client write error "
            << "(" << this->name() << ":" << this->port() << ")"
            << " : " << error << endl;
          return ;
        }

        if (bytes_written != text.size()) {
          cerr << "Client write problem "
            << "(" << this->name() << ":" << this->port() << ")"
            << " : not all bytes written "
            << "(" << bytes_written << " out of " << text.size() << ")"
            << endl;
          return ;
        }

        this->Network::Connection::send(text);

        return ;
      } // void Connection::send(string const& text)

    /**
     ** traffic on the network
     **
     ** @param     condition   traffic condition
     **
     ** @return    whether the source shall not be removed
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.0
     **
     ** @todo      closing of socket
     **/
    bool
      Connection::traffic(GIOCondition condition)
      {
        // Check for socket error
        if (condition & (G_IO_ERR | G_IO_HUP | G_IO_NVAL)) {
          cerr << "Client socket error "
            << "(" << this->name() << ":" << this->port() << ")"
            << ": " << condition
            << endl;
#ifndef WORKAROUND
          // error 25
          return true;
#endif
          return false;
        }
#ifdef WORKAROUND
        // see BugReport 5 (under MS-Windows)
        if (condition == 0)
          return false;
#endif

        // Check for data to be read (or if the socket was closed)
        if (condition & G_IO_IN) {
#ifdef OUTDATED
          gchar* buffer = NULL; // buffer
          gsize bytes_read = 0; // number of read bytes

          // error code
          GIOError error
            = gnet_io_channel_readline_strdup(this->iochannel,
                                              &buffer, &bytes_read);

          // Check for socket error
          if (error != G_IO_ERROR_NONE) {
            cerr << "Client read error "
              << "(" << this->name() << ":" << this->port() << ")"
              << " : " << error
              << endl;
            return false;
          }
#endif
          gchar buffer[1000];
          gsize bytes_read = 0; // number of read bytes
          GError* error = NULL;

          if (g_io_channel_read_chars(this->iochannel,
                                      buffer, 1000, &bytes_read,
                                      &error)
              != G_IO_STATUS_NORMAL) {
            cerr << "AClient read error "
              << "(" << this->name() << ":" << this->port() << ")"
              << " : " << (error
                           ? static_cast<char*>(error->message)
                           : "no error message")
              << endl;
            cerr << "closing connection" << endl;
            this->server().delete_connection(this);
            return false;
          }

          // Check if we read 0 bytes, which means the connection is closed
          if (bytes_read == 0) {
            cerr << "Connection closed "
              << "(" << this->name() << ":" << this->port() << ")" << endl;
            this->close();
            return true;
          }

          // so we read something

          string text(buffer, bytes_read);

#ifdef NETWORK_LOG_ON_STDOUT
          cout << "< " << text << endl;
#endif
          this->received(text);
        } else { // if !(condition & G_IO_IN)
          DEBUG_ASSERTION(false,
                          "Connection::traffic():\n"
                          "  unknown condition '" << condition << "'");
        } // if !(condition & G_IO_IN)

        return true;
      } // bool Connection::traffic(GIOChannel* iochannel, GIOCondition condition)

    /**
     ** traffic on the network
     **
     ** @param	iochannel	IO-channel
     ** @param	condition	condition
     ** @param	connection_ptr	pointer to the connection
     **
     ** @return	whether the data was accepted
     **
     ** @author	Diether Knof
     **
     ** @version	0.7.2
     **/
    gboolean
      Connection::async_client_iofunc(GIOChannel* iochannel,
                                      GIOCondition condition, 
                                      gpointer connection_ptr)
      {
        DEBUG_ASSERTION((static_cast<Connection*>(connection_ptr)->iochannel
                         == iochannel),
                        "Connection::async_client_iofunc():\n"
                        "  the iochannels are not the same");
        return static_cast<Connection*>(connection_ptr)->traffic(condition);
      }

  } // namespace GNet
} // namespace Network

#endif // #ifdef USE_NETWORK
