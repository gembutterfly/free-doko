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

#include "server.h"

#include "listener.h"
#include "connection.h"
#include "sender.h"

#include "gnet/addresses.h"

#include "../ui/ui.h"
#include "../ui/ui.wrap.h"

#include "../utils/string.h"

Network::Server* server = NULL;

namespace Network {

  unsigned const Server::FREEDOKO_STANDARD_PORT      = 2442;

  /**
   ** constructor
   **
   ** @param     -
   **
   ** @return    -
   **
   ** @author    Diether Knof
   **
   ** @version   0.7.2
   **/
  Server::Server() :
    OS(OS_TYPE::SERVER),
    listeners_(),
    connections_()
  {
    return ;
  } // Server::Server()

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
  Server::~Server()
  {
    this->clear();
  } // Server::~Server()

  /**
   ** the ui has changed
   **
   ** @param     ui   new ui
   **
   ** @return    -
   **
   ** @author    Diether Knof
   **
   ** @version   0.7.13
   **/
  void
  Server::ui_changed(UI& ui) const
  {
    for (vector<Listener*>::const_iterator l = this->listeners().begin();
l != this->listeners().end();
++l)
      ui.network_listener_new(**l);
    for (vector<Connection*>::const_iterator c = this->connections().begin();
         c != this->connections().end();
         ++c) {
      ui.network_traffic(**c, TRAFFIC_CONNECTION, "connection (port " + DK::Utils::String::to_string((*c)->port()) + ")");
      ui.network_connection_new(**c);
    }
    return ;
  } // void Server::ui_changed(UI& ui) const

  /**
   ** -> result
   **
   ** @param     -
   **
   ** @return    the local addresses
   **
   ** @author    Diether Knof
   **
   ** @version   0.7.2
   **/
  vector<string>
    Server::local_addresses() const
    {
      return GNet::local_addresses();
    } // vector<string> Server::local_addresses() const

  /**
   ** -> result
   **
   ** @param     -
   **
   ** @return    whether the server is empty, t.i. no listener and no connection
   **
   ** @author    Diether Knof
   **
   ** @version   0.7.2
   **/
  bool
    Server::empty() const
    {
      return (this->listeners().empty() && this->connections().empty());
    } // bool Server::empty() const

  /**
   ** -> result
   **
   ** @param     -
   **
   ** @return    the listeners
   **
   ** @author    Diether Knof
   **
   ** @version   0.7.2
   **/
  vector<Listener*>&
    Server::listeners()
    {
      return this->listeners_;
    } // vector<Listener*>& Server::listeners()

  /**
   ** -> result
   **
   ** @param     -
   **
   ** @return    the connections
   **
   ** @author    Diether Knof
   **
   ** @version   0.7.2
   **/
  vector<Connection*>&
    Server::connections()
    {
      return this->connections_;
    } // vector<Connection*>& Server::connections()

  /**
   ** -> result
   **
   ** @param     -
   **
   ** @return    whether there is a connection to a parent
   **
   ** @author    Diether Knof
   **
   ** @version   0.7.2
   **/
  bool
    Server::has_parent_connection() const
    {
      for (vector<Connection*>::const_iterator c = this->connections().begin();
           c != this->connections().end();
           ++c)
        if ((*c)->relation() == Connection::PARENT)
          return true;

      return false;
    } // bool Server::has_parent_connection() const

  /**
   ** -> result
   **
   ** @param     -
   **
   ** @return    the number of child connections
   **
   ** @author    Diether Knof
   **
   ** @version   0.7.2
   **/
  unsigned
    Server::child_connections_no() const
    {
      unsigned n = 0;
      for (vector<Connection*>::const_iterator c = this->connections().begin();
           c != this->connections().end();
           ++c)
        if ((*c)->relation() == Connection::CHILD)
          n += 1;

      return n;
    } // unsigned Server::child_connections_no() const

  /**
   ** creates a new listener
   **
   ** @param     port   port to listen on
   **
   ** @return    pointer to the listener, NULL if failed
   **
   ** @author    Diether Knof
   **
   ** @version   0.7.2
   **/
  Listener*
    Server::create_listener(unsigned const port)
    {
      Listener* listener = Listener::new_(*this, port);
      if (listener == NULL)
        return NULL;

      this->new_listener(listener);

      return listener;
    } // Listener* Server::create_listener(unsigned port)

  /**
   ** there is a new listener
   **
   ** @param     listener   new listener
   **
   ** @return    -
   **
   ** @author    Diether Knof
   **
   ** @version   0.7.2
   **/
  void
    Server::new_listener(Listener* listener)
    {
      this->listeners().push_back(listener);
      ::ui->network_listener_new(*listener);

      return ;
    } // void Server::new_listener(Listener* listener)

  /**
   ** delete the listener
   **
   ** @param     listener   listener to delete
   **
   ** @return    -
   **
   ** @author    Diether Knof
   **
   ** @version   0.7.2
   **/
  void
    Server::delete_listener(Listener* listener)
    {
      vector<Listener*>::iterator c = find(this->listeners().begin(),
                                           this->listeners().end(),
                                           listener);
      DEBUG_ASSERTION((c != this->listeners().end()),
                      "Server::delete_listener(listener):\n"
                      "  listener on port " << listener->port()
                      << " not found");

      this->listeners().erase(c);
      ::ui->network_listener_deleted(*listener);

      delete listener;
      return ;
    } // void Server::delete_listener(Listener* listener)

  /**
   ** creates a new connection
   **
   ** @param     address   address to connect to
   ** @param     port      port to connect to
   ** @param     type      type of the interpreter
   **
   ** @return    pointer to the connection, NULL if failed
   **
   ** @author    Diether Knof
   **
   ** @version   0.7.12
   **/
  Connection*
    Server::create_connection(string const& address, unsigned const port,
                              InterpreterType const type)

    {
      Connection* connection = Connection::new_(*this, address, port, type);
      if (connection == NULL)
        return NULL;

      connection->set_relation(Connection::PARENT);
      connection->set_type(Connection::PLAYER);
      this->new_connection(connection);

      return connection;
    } // Connection* Server::create_connection(string address, unsigned port, InterpreterType type)

  /**
   ** a new connection is added
   **
   ** @param     connection   new connection
   **                         the memory is managed by this class
   **
   ** @return    -
   **
   ** @author    Diether Knof
   **
   ** @version   0.7.0
   **/
  void
    Server::new_connection(Connection* const connection)
    {
      this->connections().push_back(connection);
      ::ui->network_traffic(*connection, TRAFFIC_CONNECTION, "connection (port " + DK::Utils::String::to_string(connection->port()) + ")");
      ::ui->network_connection_new(*connection);

      if (connection->relation() == Connection::ORPHAN)
        connection->set_relation(Connection::CHILD);
      if (connection->type() == Connection::UNSET)
        connection->set_type(Connection::PLAYER);
      connection->interpreter().sender().connection_up();

      return ;
    } // void Server::new_connection(Connection* const connection)

  /**
   ** a connection is deleted
   **
   ** @param     connection   connection to delete
   **
   ** @return    -
   **
   ** @author    Diether Knof
   **
   ** @version   0.7.0
   **/
  void
    Server::delete_connection(Connection* const connection)
    {
      vector<Connection*>::iterator c = find(this->connections().begin(),
                                             this->connections().end(),
                                             connection);
      DEBUG_ASSERTION((c != this->connections().end()),
                      "Server::delete_connection(connection):\n"
                      "  connection " << connection->name()
                      << ":" << connection->port() << " not found");

      this->connections().erase(c);
      ::ui->network_traffic(*connection, TRAFFIC_DISCONNECTION,
                            "disconnection (port " + DK::Utils::String::to_string(connection->port()) + ")");
      ::ui->network_connection_deleted(*connection);

      delete connection;

      return ;
    } // void Server::delete_connection(Connection* const connection)

  /**
   ** -> result
   **
   ** @param     -
   **
   ** @return    the open ports
   **
   ** @author    Diether Knof
   **
   ** @version   0.7.2
   **/
  vector<unsigned>
    Server::ports() const
    {
      vector<unsigned> ports;

      for (vector<Listener*>::const_iterator l = this->listeners().begin();
           l != this->listeners().end();
           ++l)
        ports.push_back((*l)->port());

      return ports;
    } // vector<unsigned> Server::ports() const

  /**
   ** removes all listeners and connections
   **
   ** @param     -
   **
   ** @return    -
   **
   ** @author    Diether Knof
   **
   ** @version   0.7.2
   **/
  void
    Server::clear()
    {
      while (!this->connections().empty()) {
        this->connections().front()->close();
        this->delete_connection(this->connections().front());
      }

      while (!this->listeners().empty()) {
        this->delete_listener(this->listeners().front());
      }

      return ;
    } // void Server::clear()

  /**
   ** the program is quit: clear the network
   **
   ** @param     -
   **
   ** @return    -
   **
   ** @author    Diether Knof
   **
   ** @version   0.7.12
   **/
  void
    Server::quit()
    {
      this->clear();
    } // void Server::quit()

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
    Server::update()
    {
      for (vector<Listener*>::iterator l = this->listeners().begin();
           l != this->listeners().end();
           ++l)
        (*l)->update();
      for (vector<Connection*>::const_iterator c = this->connections().begin();
           c != this->connections().end();
           ++c)
        (*c)->update();

      return ;
    } // void Server::update()

  /**
   ** synchronize the game with the parent/childs
   **
   ** @param     -
   **
   ** @return    -
   **
   ** @author    Diether Knof
   **
   ** @version   0.7.5
   **/
  void
    Server::synchronize_game()
    {
      for (vector<Connection*>::iterator c = this->connections().begin();
           c != this->connections().end();
           ++c)
        (*c)->interpreter().sender().synchronize_game();

      return ;
    } // void Server::synchronize_game()

  /**
   ** 'player' has chatted 'message'
   **
   ** @param     player    the name of the player who has chatted
   ** @param     message   the chat message
   ** @param     self      whether this player has chattet
   **                      (else the chatter comes from the network)
   **                      default: true
   **
   ** @return    -
   **
   ** @author    Diether Knof
   **
   ** @version   0.7.2
   **/
  void
    Server::chat(string const& player, string const& message, bool const self)
    {
      if (self) {
        for (vector<Connection*>::iterator c = this->connections().begin();
             c != this->connections().end();
             ++c)
          (*c)->interpreter().sender().chat(message);
      } // if (self)

      return ;
    } // void Server::chat(string player, string message, bool self = true)

} // namespace Network

#endif // #ifdef USE_NETWORK
