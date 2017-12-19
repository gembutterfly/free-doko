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

#ifndef CLASS_NETWORK_CONNECTION_INTERPRETER_SENDER_HEADER
#define CLASS_NETWORK_CONNECTION_INTERPRETER_SENDER_HEADER

#include "interpreter.h"
#include "../game/game.h"

namespace Network {

  /**
   ** base class for an the outgoing messages
   **
   ** @author	Diether Knof
   **/
  class Connection::Interpreter::Sender {
    public:
      // constructor
      Sender(Interpreter& interpreter) :
	interpreter_(&interpreter)
      { }

      // destructor
      virtual ~Sender()
      { }

      // the corresponding connection
      Connection& connection()
      { return this->interpreter().connection(); }

      // the corresponding game
      Game const& game() { return this->connection().server().game(); }

      // the connection is up
      virtual void connection_up() = 0;

      // send a ping
      virtual void ping() = 0;
      // send a ping with a number
      virtual void ping(unsigned const number) = 0;
      // send a pong (reply to a ping)
      virtual void pong() = 0;
      // send a pong with a number
      virtual void pong(unsigned const number) = 0;

      // send the chat text
      virtual void chat(string const& text) = 0;

      // send the name
      virtual void name() = 0;

      // send the type of the player
      virtual void player_type(Player const& player) = 0;
      // send the name of the player
      virtual void player_name(Player const& player) = 0;
      // the two players were switched
      virtual void players_switched(unsigned const player_a,
				    unsigned const player_b) = 0;

      // send all rules
      virtual void rules() = 0;
      // send the specific rule
      virtual void rule(int const type) = 0;


      //
      // gameplay
      //

      // synchronize the game
      virtual void synchronize_game() = 0;

      virtual void gameplay_action(GameplayAction const& action) = 0;

      // the party is started
      virtual void party_start() = 0;
      // a new round is started in the party
      virtual void party_start_round(unsigned const round) = 0;
      // the party is finished
      virtual void party_finish() = 0;

      // open the game
      virtual void game_open(Game const& game) = 0;
      // distribute the cards
      virtual void game_distribute_cards(vector<Hand const*> hands) = 0;
      // start the game
      virtual void game_start() = 0;
      // finish the game
      virtual void game_finish() = 0;


    private:
      // the corresponding interpreter
      PRIV_VAR_P_RW(Interpreter, interpreter);

    private: // unused
      Sender();
      Sender(Sender const&);
      Sender& operator=(Sender const&);
  }; // class Connection::Interpreter::Sender

} // namespace Network

#endif // #ifndef CLASS_NETWORK_CONNECTION_INTERPRETER_SENDER_HEADER

#endif // #ifdef USE_NETWORK
