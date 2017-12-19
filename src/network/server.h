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

#ifndef CLASS_NETWORK_SERVER_HEADER
#define CLASS_NETWORK_SERVER_HEADER

#include "../os/os.h"
class UI;

namespace Network {

  class Connection;
  class Listener;

  /**
   ** base class for a network server
   **
   ** @author	Diether Knof
   **/
  class Server : public OS {
    friend class Listener;
    friend class Connection;
    public:
    static unsigned const FREEDOKO_STANDARD_PORT;

    public:
    // standard constructor
    Server();
    // destructor
    virtual ~Server();

    // the ui has changed -- update the corresponding information
    void ui_changed(UI& ui) const;

    // the names of the addresses of this server
    vector<string> local_addresses() const;

    // whether the server is empty
    bool empty() const;
    // the listeners
    vector<Listener*>& listeners();
    // the connections
    vector<Connection*>& connections();

    // whether there is a connection to a parent
    bool has_parent_connection() const;
    // the number of child connections
    unsigned child_connections_no() const;

    public:

    // creates a listener on the port 'port'
    Listener* create_listener(unsigned const port);
    // the listener is created
    void new_listener(Listener* const listener);
    // the listener is deleted
    void delete_listener(Listener* const listener);

    // creates a new connection
    Connection* create_connection(string const& address, unsigned const port,
                                 InterpreterType const type);
    // the connection is created
    void new_connection(Connection* const connection);
    // the connection is deleted
    void delete_connection(Connection* const connection);


    // the open ports
    vector<unsigned> ports() const;

    // removes all listeners and connections
    void clear();


    //////
    // OS
    //////

    // quit the program
    void quit();

    // use for check on network traffic
    void update();
    // synchronize with the parent/childs
    void synchronize_game();


    //
    // gameplay
    //

    void gameplay_action(GameplayAction const& action);

    // the party is started
    void party_start();
    // a new round is started in the party
    void party_start_round(unsigned const round);
    // the party is finished
    void party_finish();


    // the parts of a game
    void game_open(Game const& game);
    void game_cards_distributed();
    void game_start();
    void game_finish();



    // 'player' has chatted 'message'
    void chat(string const& player, string const& message,
              bool const self = true);




    // the two players were switched
    void players_switched(Player const& player_a, Player const& player_b);
    // the player has changed
    void player_changed(Player const& player);
    // the name of the player has changed
    void name_changed(Player const& player);

    // a rule has changed
    void rule_changed(int const type, void const* const old_value);
    // the settings have changed (here only the name is important)
    void setting_changed(int const type, void const* const old_value);

    private:
    // the listeners
    PRIV_VAR_R(vector<Listener*>, listeners);
    // the connections to the server
    PRIV_VAR_R(vector<Connection*>, connections);

    private: // unused
    Server(Server const&);
    Server& operator=(Server const&);
  }; // class Server : public OS

} // namespace Network

extern Network::Server* server;

#endif // #ifndef CLASS_NETWORK_SERVER_HEADER

#endif // #ifdef USE_NETWORK
