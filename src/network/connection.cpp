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
#include "receiver.h"
#include "server.h"
#include "actor.h"
#include "types.h"
#include "exceptions.h"

#include "gnet/connection.h"
#include "file/connection.h"

#include "../party/party.h"
#include "../party/rule.h"
#include "../player/network/network_player.h"

#include "../misc/setting.h"
#include "../ui/ui.h"

#include "../utils/string.h"

namespace Network {

  /**
   ** constructor
   ** if port = 0, take 'address' as filename for the traffic
   **
   ** @param	server    the corresponding server
   ** @param	address   address to connect to
   ** @param	port      port to connect to
   ** @param	type      the interpreter type
   **
   ** @return	-
   **
   ** @author	Diether Knof
   **
   ** @version	0.7.12
   **/
  Connection*
    Connection::new_(Server& server, 
		     string const& address, unsigned const port,
                     InterpreterType const type)
    {
      try {
        if (port == 0) {
          return new File::Connection(server, address, 0, type);
        } else {
          return new GNet::Connection(server, address, port, type);
        }
      } catch (ConnectionFailureException const&) {
        return NULL;
      }
      return NULL;
    } // static Connection* Connection::new_(Server& server, string address, unsigned port, InterpreterType type)

  /**
   ** constructor
   **
   ** @param	server	the corresponding server
   **
   ** @return	-
   **
   ** @author	Diether Knof
   **
   ** @version	0.7.0
   **/
  Connection::Connection(Server& server) :
    players(),
    players_bak(4, static_cast<Player*>(NULL)),
    server_(&server),
    interpreter_(NULL),
    actor_(new Actor(*this)),
    relation_(ORPHAN),
    type_(UNSET),
    name_(),
    address_(),
    port_(Server::FREEDOKO_STANDARD_PORT),
#ifdef POSTPONED
    accepted_(false),
#else
    accepted_(true),
#endif
    incoming_bytes_(0),
    outgoing_bytes_(0)
    {
#ifndef OUTDATED
      this->interpreter_ = Interpreter::new_(*this, FREEDOKO);
#endif
      return ;
    } // Connection::Connection(Server server)

  /**
   ** constructor
   **
   ** @param	server    the corresponding server
   ** @param	address   address to connect to
   ** @param	port      port to connect to
   ** @param	type      interpreter type
   **
   ** @return	-
   **
   ** @author	Diether Knof
   **
   ** @version	0.7.12
   **
   ** @todo	select the interpreter automatically
   **/
  Connection::Connection(Server& server,
                         string const& address, unsigned const port,
                         InterpreterType const type) :
    players(),
    players_bak(4, static_cast<Player*>(NULL)),
    server_(&server),
    interpreter_(NULL),
    actor_(new Actor(*this)),
    relation_(ORPHAN),
    type_(UNSET),
    name_(),
    address_(address),
    port_(port),
#ifdef POSTPONED
    accepted_(false),
#else
    accepted_(true),
#endif
    incoming_bytes_(0),
    outgoing_bytes_(0)
    {

      this->interpreter_ = Interpreter::new_(*this, type);

      return ;
    } // Connection::Connection(Server server, string address, unsigned port, InterpreterType type)

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
   **
   ** @bug       const cast
   **/
  Connection::~Connection()
  {
    if (::game_status == GAMESTATUS::QUIT)
      return ;

    if (::game_status <= GAMESTATUS::PARTY_LOADED) {
      // replace the managed players with the backups

      for (unsigned p = 0; p < this->players.size(); ++p) {
        if (   this->players_bak[p]
            && (this->players[p]->type() == Player::NETWORK)) {
          Party& party = const_cast<Party&>(this->server().party());
          party.set_player(p, this->players_bak[p]);
        } else {
          delete this->players_bak[p];
        }
      } // for (p < this->players.size())
    } else { // if !(::game_status <= GAMESTATUS::PARTY_LOADED)

      // transform the player into an ai
      for (unsigned p = 0; p < this->players.size(); ++p) {
        if (   this->players_bak[p]
            && (this->players[p]->type() == Player::NETWORK)) {
          delete this->players_bak[p];
          this->players[p]->remove_connection();
        }
      } // for (p)
    } // if !(::game_status <= GAMESTATUS::PARTY_LOADED)

    delete this->interpreter_;
    delete this->actor_;
  } // Connection::~Connection()

  /**
   ** sets the name
   **
   ** @param	name	new name
   **
   ** @return	-
   **
   ** @author	Diether Knof
   **
   ** @version	0.7.2
   **/
  void
    Connection::set_name(string const& name)
    {
      this->name_ = name;
      if (this->players.size() == 1)
        this->players[0]->set_name(name);
      ::ui->network_connection_changed(*this);

      return ;
    } // void Connection::set_name(string name)

  /**
   ** -> result
   **
   ** @param	player   player
   **
   ** @return	whether the connection manages the player 'player'
   **
   ** @author	Diether Knof
   **
   ** @version	0.7.2
   **/
  bool
    Connection::manages_player(Player const& player) const
    {
      return (std::find(this->players.begin(), this->players.end(),
                        &player)
              != this->players.end());
    } // bool Connection::manages_player(Player player) const

  /**
   ** -> result
   **
   ** @param	playerno   number of player (party)
   **
   ** @return	if the player 'playerno' is managed, a pointer to the player
   **           if unmanaged, the NULL-pointer
   **
   ** @author	Diether Knof
   **
   ** @version	0.7.3
   **/
  NetworkPlayer*
    Connection::managed_player(unsigned const playerno)
    {
      for (vector<NetworkPlayer*>::iterator p = this->players.begin();
           p != this->players.end();
           ++p)
        if (**p == ::party.player(playerno))
          return *p;

      return NULL;
    } // NetworkPlayer* Connection::managed_player(unsigned playerno)

  /**
   ** create a network player and bind it to the connection
   ** also creates a backup of the replaced player
   **
   ** @param	-
   **
   ** @return	-
   **
   ** @author	Diether Knof
   **
   ** @version	0.7.2
   **
   ** @bug	const cast
   **/
  void
    Connection::create_network_player()
    {
      // search an ai player in the party to replace
      Party& party = const_cast<Party&>(this->server().party());
      vector<Player*>::const_iterator p = party.players().begin();
      // first try the player on the north side
      ++p; ++p;
      if (!(   ((*p)->type(  ) == Player::AI)
            || ((*p)->type(  ) == Player::AI_DUMMY)
            || ((*p)->type(  ) == Player::AI_RANDOM) ) )
        for (p = party.players().begin(); p != party.players().end(); ++p)
          if (   ((*p)->type(  ) == Player::AI)
              || ((*p)->type(  ) == Player::AI_DUMMY)
              || ((*p)->type(  ) == Player::AI_RANDOM) )
            break;

      if (p == party.players().end()) {
        cerr << "Network::Connection::create_network_player()\n"
          << "  found no player to replace." << endl;
        // found no player to replace
        return ;
      }

      this->players.push_back(new NetworkPlayer(*this));
      this->players.back()->set_name(this->name());

      this->players_bak[party.no(**p)] = party.replace_player(party.no(**p),
                                                     this->players.back());

      return ;
    } // void Connection::create_network_player()

  /**
   ** sets 'player' to a network player, manged by this connection
   **
   ** @param	player   player which is set to a network player
   **
   ** @return	-
   **
   ** @author	Diether Knof
   **
   ** @version	0.7.2
   **
   ** @bug      const cast
   **/
  void
    Connection::set_to_network_player(Player const& player)
    {
      if (player.type() == Player::NETWORK)
        return ;

      Party& party = ::party;
      unsigned const playerno = party.no(player);

      if (player.type() == Player::HUMAN) {
        // search a player to set human in place of the now-network-player
        vector<Player*>::iterator p;
        for (p = party.players().begin(); p != party.players().end(); ++p)
          if (   ((*p)->type() != Player::NETWORK)
              && ((*p)->type() != Player::HUMAN)
              && (**p != player) )
            break;

        if (p != party.players().end()) {
          party.switch_players(playerno, party.no(**p));
        }
      } // if (player.type() == Player::HUMAN)
      // Note: It can be that 'player' is not anymore the player to set.

      this->players.push_back(new NetworkPlayer(*this));
      this->players.back()->set_name(party.player(playerno).name());
      if (playerno >= this->players_bak.size())
        this->players_bak.resize(playerno + 1);
      this->players_bak[playerno] = (party.replace_player(playerno,
                                                          this->players.back()));

      return ;
    } // void Connection::set_to_network_player(Player player)

  /**
   ** sets 'player' to a local player, not manged by this connection anymore
   **
   ** @param	player   player which is set to a local player
   **
   ** @return	-
   **
   ** @author	Diether Knof
   **
   ** @version	0.7.2
   **
   ** @bug      const cast
   **/
  void
    Connection::set_to_local_player(Player const& player)
    {
      if (player.type() != Player::NETWORK)
        return ;

      Party& party = const_cast<Party&>(this->server().party());
      unsigned const playerno = party.no(player);

      // undo a switching
      unsigned p;
      for (p = 0; p < this->players.size(); ++p)
        if (party.no(*this->players[p]) == playerno)
          break;

      DEBUG_ASSERTION( (p < this->players.size()),
                      "Network::Connection::Actor::set_player_type()\n"
                      "  type = local\n"
                      "  playerno = " << playerno << "\n"
                      "  did not find managed network player.");

      delete party.replace_player(playerno, this->players_bak[p]);
      this->players.erase(this->players.begin() + p);
      this->players_bak.erase(this->players_bak.begin() + p);

      return ;
    } // void Connection::set_to_local_player(Player player)

  /**
   ** sends the text over the connection
   ** does nothing
   **
   ** @param     text   text to send
   **
   ** @return    -
   **
   ** @author    Diether Knof
   **
   ** @version   0.7.13
   **/
  void
    Connection::send(string const& text)
    {
      if (!this->accepted())
        return ;

      this->outgoing_bytes_ += text.size();

#ifdef NETWORK_LOG_ON_STDOUT
      cout << "> " << text << endl;
#endif

      ::ui->network_traffic(*this, TRAFFIC_OUTGOING, text);

      return ;
    } // void Connection::send(string const& text)


  /**
   ** a text has been received
   **
   ** @param	text	received text
   **
   ** @return	-
   **
   ** @author	Diether Knof
   **
   ** @version	0.7.0
   **/
  void
    Connection::received(string const& text)
    {
      this->incoming_bytes_ += text.size();

      if (!this->accepted())
        return ;

      ::ui->network_traffic(*this, TRAFFIC_INCOMING, text);
#ifdef NETWORK_LOG_ON_STDOUT
      cout << "network incoming: " << text << endl;
#endif

      DEBUG_ASSERTION(this->interpreter_,
                      "Connection::received(text)\n"
                      "  no interpreter");
      this->interpreter().receiver().received(text);

      return ;
    } // void Connection::received(string const& text)

  /**
   ** close this connection
   **
   ** @param	-
   **
   ** @return	-
   **
   ** @author	Diether Knof
   **
   ** @version	0.7.0
   **/
  void
    Connection::close()
    {
      this->interpreter().close_connection();
      this->server().delete_connection(this);

      this->relation_ = ORPHAN;
      this->type_ = UNSET;

      return ;
    } // void Connection::close()

} // namespace Network

/**
 ** -> result
 **
 ** @param	relation	connection relation
 **
 ** @return	name of the connection relation
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.2
 **/
string
name(Network::Connection::Relation const& relation)
{
  switch(relation) {
  case Network::Connection::ORPHAN:
    return "orphan";
  case Network::Connection::PARENT:
    return "parent";
  case Network::Connection::CHILD:
    return "child";
  case Network::Connection::SIBLING:
    return "sibling";
  } // switch(relation)

  return "";
} // string name(Network::Connection::Relation relation)

/**
 ** -> result
 **
 ** @param	type	connection type
 **
 ** @return	name of the connection type
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.2
 **/
string
name(Network::Connection::Type const& type)
{
  switch(type) {
  case Network::Connection::UNSET:
    return "unset";
  case Network::Connection::PLAYER:
    return "player";
  case Network::Connection::SHADOW:
    return "shadow";
  case Network::Connection::OBSERVER:
    return "observer";
  } // switch(type)

  return "";
} // string name(Network::Connection::Type type)

#endif // #ifdef USE_NETWORK
