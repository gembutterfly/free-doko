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
#ifdef USE_NETWORK_DOKOLOUNGE

#ifndef CLASS_NETWORK_CONNECTION_INTERPRETER_DOKOLOUNGE_SENDER_HEADER
#define CLASS_NETWORK_CONNECTION_INTERPRETER_DOKOLOUNGE_SENDER_HEADER

#include "interpreter.h"
#include "../sender.h"

namespace Network {
  namespace DokoLounge {

    /**
     ** class for an the outgoing messages for DokoLounge
     **
     ** @author	Diether Knof
     **/
    class Interpreter::Sender : public Connection::Interpreter::Sender {
      public:
	// constructor
	Sender(Interpreter& interpreter);
	// destructor
	~Sender();


        // the account
        string const& account() const;

        ///////////
        // special DokoLounge messages
        ///////////

        // create an account (name and password are taken from the settings)
        void create_account();
        // login (name and password are taken from the settings)
        void login();
        // logout
        void logout();

        // refresh
        void refresh();
        // refresh for all players (at the table)
        void refresh_all_players();

        // send the icon of the player
        void send_icon();

        // Klingel
        void klingel();
        // hilfe
        void hilfe();
        // Blog
        void blog();
        void blog_senden();
        // Pinnwand
        void pinnwand();
        //void pinnwand_refresh();
        //void pinnwand_senden();
        // Eigene Pinnwand
        void messages();
        // Rangliste
        //void rangliste();
        //void rangliste2();
        //void angstgegner();
        // Tisch
        //void tisch_neu();


        // Enter a table
        void enter_table(int const table);
        // Leave the table
        void leave_table();
        // follow a player
        void follow_player(string const& name);

        ///////////
        // common messages
        ///////////

        // the connection is up
        void connection_up();

        // send a ping
        void ping();
        // send a ping with a number
        void ping(unsigned const number);
        // send a pong (reply to a ping)
        void pong();
        // send a pong with a number
        void pong(unsigned const number);

        // send the chat text to the lounge
        void chat_lounge(string const& text);
        // send the chat text to the table
        void chat_table(string const& text);
        // send the chat text
        void chat(string const& text_);
        // send the chat text
        void chat(string const& location, string text);

        // send the name
        void name();

        // send the type of the player
        void player_type(Player const& player) {}
        // send the name of the player
        void player_name(Player const& player) {}
        // the position of two players has switched
        void players_switched(unsigned const player_a,
                              unsigned const player_b) {}

        // send all rules
        void rules() {}
        // send the specific rule
        void rule(int const type) {}



        //
        // gameplay
        //

        // synchronize the game
        void synchronize_game() {}

        // a gamplay action has happened
        void gameplay_action(GameplayAction const& action);

        // the party is started
        void party_start() {}
        // a new round is started in the party
        void party_start_round(unsigned const round) {}
        // the party is finished
        void party_finish() {}

        // open the game
        void game_open(Game const& game) {}
        // distribute the cards
        void game_distribute_cards(vector<Hand const*> hands) {}
        // start the game
        void game_start() {}
        // finish the game
        void game_finish() {}

      private:
        // sends a simple command
        void send_simple_command(string const& keyword);
        // send a normal command
        void send_command(string const& keyword, string const& text);
        // sends a named command
        void send_name_command(string const& keyword);
        void send_name_command(string const& keyword, string const& text);

        // sends a the text
        void send(string const& text);

        // command string
        string simple_command(string const& keyword) const;
        // command string
        string command(string const& keyword, string const& text) const;
        // command string whitwh name
        string name_command(string const& keyword) const;
        // command string
        string name_command(string const& keyword, string const& text) const;

      private: // unused
        Sender();
        Sender(Sender const&);
        Sender& operator=(Sender const&);
    }; // class Interpreter::Sender : public Connection::Interpreter::Sender

  } // namespace DokoLounge
} // namespace Network

#endif // #ifndef CLASS_NETWORK_CONNECTION_INTERPRETER_DOKOLOUNGE_SENDER_HEADER

#endif // #ifdef USE_NETWORK_DOKOLOUNGE
#endif // #ifdef USE_NETWORK
