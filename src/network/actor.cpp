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

#include "constants.h"

#include "actor.h"
#include "server.h"
#include "interpreter.h"
#include "receiver.h"

#include "../party/party.h"
#include "../party/rule.h"
#include "../game/game.h"
#include "../game/gameplay_action.h"
#include "../misc/setting.h"
#ifdef USE_NETWORK_DOKOLOUNGE
#include "../misc/lounge.h"
#endif

#include "../ui/ui.wrap.h"

namespace Network {

  /**
   ** constructor
   **
   ** @param     connection   corresponding connection
   **
   ** @return    -
   **
   ** @author    Diether Knof
   **
   ** @version   0.7.13
   **/
  Connection::Actor::Actor(Connection& connection) :
    connection_(&connection),
    pending_action_(NULL),
    game_players_()
  { }


  /**
   ** destructor
   **
   ** @param     -
   **
   ** @return    -
   **
   ** @author    Diether Knof
   **
   ** @version   0.7.1
   **/
  Connection::Actor::~Actor()
  { }

  /**
   ** -> result
   **
   ** @param     -
   **
   ** @return    whether there are pending actions
   **
   ** @author    Diether Knof
   **
   ** @version   0.7.13
   **/
  GameplayAction const*
    Connection::Actor::pending_action()
    {
      if (this->pending_action_ == NULL)
        return NULL;
      // check whether the action was made, then delete the pending action
      this->check_pending_game_action();
      return this->pending_action_;
    } // GameplayAction const* Connection::Actor::pending_action()

  /**
   ** removes the pending action
   **
   ** @param     -
   **
   ** @return    -
   **
   ** @author    Diether Knof
   **
   ** @version   0.7.13
   **/
  void
    Connection::Actor::remove_pending_action()
    {
      if (this->pending_action_ == NULL)
        return; 
      delete this->pending_action_;
      this->pending_action_ = NULL;
      //this->connection().interpreter().receiver().received("");
      return ;
    } // void Connection::Actor::remove_pending_action()

  /**
   ** -> result
   **
   ** @param     -
   **
   ** @return    the party (writable)
   **
   ** @author    Diether Knof
   **
   ** @version   0.7.1
   **
   ** @bug       const cast
   **/
  Party&
    Connection::Actor::party()
    {
      return const_cast<Party&>(this->connection().server().party());
    } // Party& Connection::Actor::party()

  /**
   ** -> result
   **
   ** @param     -
   **
   ** @return    the party
   **
   ** @author    Diether Knof
   **
   ** @version   0.7.13
   **/
  Party const&
    Connection::Actor::party() const
    {
      return this->connection().server().party();
    } // Party const& Connection::Actor::party() const

  /**
   ** -> result
   **
   ** @param     -
   **
   ** @return    the game (writable)
   **
   ** @author    Diether Knof
   **
   ** @version   0.7.1
   **
   ** @bug       const cast
   **/
  Game&
    Connection::Actor::game()
    {
      return const_cast<Game&>(this->connection().server().game());
    } // Game& Connection::Actor::game()

  /**
   ** -> result
   **
   ** @param     -
   **
   ** @return    the game
   **
   ** @author    Diether Knof
   **
   ** @version   0.7.13
   **/
  Game const&
    Connection::Actor::game() const
    {
      return this->connection().server().game();
    } // Game const& Connection::Actor::game() const

  /**
   ** chat the text
   **
   ** @param     text   text to chat
   **
   ** @return    -
   **
   ** @author    Diether Knof
   **
   ** @version   0.7.1
   **/
  void
    Connection::Actor::chat(string const& text)
    {
      ::ui->chat(this->connection().name(), text, false);
      return ;
    } // void Connection::Actor::chat(string text)

  /**
   ** reset the party players
   ** first one is human, the rest ai
   **
   ** @param     -
   **
   ** @return    -
   **
   ** @author    Diether Knof
   **
   ** @version   0.7.13
   **/
  void
    Connection::Actor::reset_party_players()
    {
      this->party().remove_spare_players();
      // first one is human
      string const name = (
#ifdef USE_NETWORK_DOKOLOUNGE
                            ::lounge ? ::lounge->account() :
#endif
                            ::setting(Setting::NAME));
      if (this->party().player(0).type() != Player::HUMAN)
        this->party().replace_player(0, Player::create(Player::HUMAN, name));
      this->set_player_type(this->party().player(0), false);
      this->party().player(0).set_name(name);
      // remaining players are ai
      for (unsigned p = 1; p < this->party().playerno(); ++p) {
        this->set_player_type(this->party().player(p), true);
        this->set_player_name(this->party().player(p), "");
      }
      for (unsigned p = 0; p < this->party().playerno(); ++p)
        CLOG << p << ": " << this->party().player(p).name() << " -- " << this->party().player(p).type() << endl;
      return ;
    } // void Connection::Actor::reset_party_players()

  /**
   ** set the player type
   **
   ** @param     player    the player to set the type of
   ** @param     network   whether the player is a network player (or a local)
   **
   ** @return    whether the type was accepted
   **
   ** @author    Diether Knof
   **
   ** @version   0.7.13
   **/
  bool
    Connection::Actor::set_player_type(Player const& player,
                                       bool const network)
    {
      if (!(this->connection().relation() == Connection::PARENT))
        return false;

      if (network)
        this->connection().set_to_network_player(player);
      else
        this->connection().set_to_local_player(player);

      return true;
    } // bool Connection::Actor::set_player_type(Player player, bool network)

  /**
   ** set the player name
   **
   ** @param     player   the player to set the name of
   ** @param     name     name of the player
   **
   ** @return    whether the name was accepted
   **
   ** @author    Diether Knof
   **
   ** @version   0.7.2
   **/
  bool
    Connection::Actor::set_player_name(Player const& player,
                                       string const& name)
    {
      if (!this->connection().manages_player(player))
        return false;

      this->party().player(this->party().no(player)).set_name(name);

      return true;
    } // bool Connection::Actor::set_player_name(Player player, string name)

  /**
   ** the players are switched
   **
   ** @param	player_a   first player
   ** @param	player_b   second player
   **
   ** @return	-
   **
   ** @author	Diether Knof
   **
   ** @version	0.7.3
   **/
  void
    Connection::Actor::players_switched(unsigned const player_a,
                                        unsigned const player_b)
    {
      this->party().switch_players(player_a, player_b);

      return ;
    } // void Connection::Actor::players_switched(unsigned player_a, unsigned player_b)

  /**
   ** add a party player
   ** do not add him, if he is already in the party
   **
   ** @param     name     name of the player
   **
   ** @return    -
   **
   ** @author    Diether Knof
   **
   ** @version   0.7.13
   **/
  void
    Connection::Actor::add_party_player(string const& name)
    {
      if (this->party().player_by_name(name) == NULL) {
        Player* player = Player::create(Player::AI, name);
        this->party().add_player(player);
        this->connection().set_to_network_player(*player);
      }

      return ;
    } // void Connection::Actor::add_party_player(string const& name)

} // namespace Network

#endif // #ifdef USE_NETWORK
