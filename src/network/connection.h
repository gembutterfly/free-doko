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

#ifndef CLASS_NETWORK_CONNECTION_HEADER
#define CLASS_NETWORK_CONNECTION_HEADER

#include "types.h"

#include "server.h"
#include "../basistypes.h"

#include "../misc/translation.h"

//#include "../player/network/network_player.h"
class NetworkPlayer;
class GameplayAction;

#include "../utils/time.h"
using DK::Utils::Time;

class Player;

namespace Network {

  /**
   ** base class for a network connection
   **
   ** @author	Diether Knof
   **
   ** @todo	split into 'sender' and 'receiver'
   ** @todo	resent messages without 'received' signal
   **/
  class Connection {
    public:
      class Interpreter; // interprets the in- and outgoing messages
      class Actor; // acts according to the incoming messages
    public:
    enum Relation {
      ORPHAN,
      PARENT,
      CHILD,
      SIBLING
    }; // enum Type
    enum Type {
      UNSET,
      PLAYER,
      SHADOW,	// observer of one player
      OBSERVER	// observes the whole game (all hands)
    }; // enum Type

    public:
      // create a new connection
      static Connection* new_(Server& server,
			      string const& address, unsigned const port,
                              InterpreterType const type);
    protected:
      // constructor
      Connection(Server& server);
      // constructor
      Connection(Server& server,
                 InterpreterType const type);
      // constructor
      Connection(Server& server,
                 string const& address, unsigned const port,
                 InterpreterType const type);
    public:
      // destructor
      virtual ~Connection();

      // close this connection
      void close();

      // check for network traffic
      virtual void update() { }

      // sends 'text'
      // child classes shall increase 'incoming_bytes'
      virtual void send(string const& text);


      // set the name
      void set_name(string const& name);

      // whether the connection manages the given player
      bool manages_player(Player const& player) const;
      // returns the managed player 'playerno' or NULL if not managed
      NetworkPlayer* managed_player(unsigned const playerno);

      // create a network player
      void create_network_player();
      // sets the player to a network player
      void set_to_network_player(Player const& player);
      // sets the player to a local player
      void set_to_local_player(Player const& player);

    protected:
      // received
      void received(string const& text);

    public:
      // the connected players
      vector<NetworkPlayer*> players;
    private:
      // the players which are replaced by network players
      vector<Player*> players_bak;

    private:
      // the corresponding server
      PRIV_VAR_P_RW(Server, server);

      // the interpreter
      PRIV_VAR_P_RW(Interpreter, interpreter);
      // the actor
      PRIV_VAR_P_RW(Actor, actor);

      // the relation of the connection
      PROT_VAR_RW(Relation, relation);
      // the type of the connection
      PROT_VAR_RW(Type, type);
      // the name of the connection
      PRIV_VAR_R(string, name);
      // the address of the connected computer
      PROT_VAR_R(string, address);
      // the port on the connected computer
      PROT_VAR_R(unsigned, port);
      // whether the connection is accepted
      PRIV_VAR_R(bool, accepted);

      // size of incoming traffic
      PROT_VAR_R(unsigned, incoming_bytes);
      // size of outgoing traffic
      PROT_VAR_R(unsigned, outgoing_bytes);

    private: // unused
      Connection();
      Connection(Connection const&);
      Connection& operator=(Connection const&);
  }; // class Connection

} // namespace Network

// the name of the relation
string name(Network::Connection::Relation const& relation);
// the name of the type
string name(Network::Connection::Type const& type);

// output of the names
WRITE_NAME(Network::Connection::Relation)
WRITE_NAME(Network::Connection::Type)

  inline
  Translator::Translation translation(Network::Connection::Relation const&
                                      relation)
{ return Translator::Translation(::translator,
                                 "Network::Connection::Relation::"
                                 + ::name(relation));
}
inline
Translator::Translation translation(Network::Connection::Type const& type)
{ return Translator::Translation(::translator,
                                 "Network::Connection::Type::"
                                 + ::name(type));
}

#endif // #ifndef CLASS_NETWORK_CONNECTION_HEADER

#endif // #ifdef USE_NETWORK
