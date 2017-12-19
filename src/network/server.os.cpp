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
#include "connection.h"
#include "sender.h"

#include "../game/game.h"
#include "../game/gameplay_actions/player1.h"

#include "../misc/setting.h"

namespace Network {

  /**
   ** a gamplay action has happened
   **
   ** @param	action   gamplay action
   **
   ** @return	-
   **
   ** @author	Diether Knof
   **
   ** @version	0.7.3
   **/
  void
    Server::gameplay_action(GameplayAction const& action)
    {
      for (vector<Connection*>::iterator c = this->connections().begin();
	   c != this->connections().end();
	   ++c) {
	if (   ((*c)->relation() == Connection::CHILD)
	    || (   ((*c)->relation() == Connection::PARENT)
	     	&& (   !dynamic_cast<GameplayAction::Player1 const*>(&action)
		    || (this->game().player(dynamic_cast<GameplayAction::Player1 const&>(action).player).type()
		    != Player::NETWORK) )
	       ) )
	  (*c)->interpreter().sender().gameplay_action(action);
      }
      return ;
    } // void Server::gameplay_action(GameplayAction action)

  /**
   ** the party is started
   **
   ** @param	-
   **
   ** @return	-
   **
   ** @author	Diether Knof
   **
   ** @version	0.7.2
   **/
  void
    Server::party_start()
    {
      for (vector<Connection*>::iterator c = this->connections().begin();
	   c != this->connections().end();
	   ++c)
	if ((*c)->relation() == Connection::CHILD)
	  (*c)->interpreter().sender().party_start();
      return ;
    } // void Server::party_start()

  /**
   ** a new round is started
   ** do nothing
   **
   ** @param	roundno		number of round to start
   **
   ** @return	-
   **
   ** @author	Diether Knof
   **
   ** @version	0.7.2
   **/
  void
    Server::party_start_round(unsigned const roundno)
    {
      for (vector<Connection*>::iterator c = this->connections().begin();
	   c != this->connections().end();
	   ++c)
	if ((*c)->relation() == Connection::CHILD)
	  (*c)->interpreter().sender().party_start_round(roundno);
      return ;
    } // void Server::party_start_round(unsigned roundno)

  /**
   ** the party is finished
   **
   ** @param	-
   **
   ** @return	-
   **
   ** @author	Diether Knof
   **
   ** @version	0.7.2
   **/
  void
    Server::party_finish()
    {
      for (vector<Connection*>::iterator c = this->connections().begin();
	   c != this->connections().end();
	   ++c)
	if ((*c)->relation() == Connection::CHILD)
	  (*c)->interpreter().sender().party_finish();
      return ;
    } // void Server::party_finish()

  /**
   ** the game is opened
   **
   ** @param	-
   **
   ** @return	-
   **
   ** @author	Diether Knof
   **
   ** @version	0.7.3
   **/
  void
    Server::game_open(Game const& game)
    {
      this->OS::game_open(game);
      for (vector<Connection*>::iterator c = this->connections().begin();
	   c != this->connections().end();
	   ++c)
	if ((*c)->relation() == Connection::CHILD)
	  (*c)->interpreter().sender().game_open(game);
      return ;
    } // void Server::game_open(Game game)

  /**
   ** the cards of the game are distributed
   **
   ** @param	-
   **
   ** @return	-
   **
   ** @author	Diether Knof
   **
   ** @version	0.7.3
   **/
  void
    Server::game_cards_distributed()
    {
      vector<Hand const*> hands;
      for (vector<Player*>::const_iterator p = this->game().players_begin();
	   p != this->game().players_end();
	   ++p) {
	hands.push_back(&(*p)->hand());
      }

      for (vector<Connection*>::iterator c = this->connections().begin();
	   c != this->connections().end();
	   ++c)
	if ((*c)->relation() == Connection::CHILD)
	  (*c)->interpreter().sender().game_distribute_cards(hands);

      return ;
    } // void Server::game_cards_distributed()

  /**
   ** the game is started
   **
   ** @param	-
   **
   ** @return	-
   **
   ** @author	Diether Knof
   **
   ** @version	0.7.3
   **/
  void
    Server::game_start()
    {
      for (vector<Connection*>::iterator c = this->connections().begin();
	   c != this->connections().end();
	   ++c)
	if ((*c)->relation() == Connection::CHILD)
	  (*c)->interpreter().sender().game_start();
      return ;
    } // void Server::game_start()

  /**
   ** the game is finished
   **
   ** @param	-
   **
   ** @return	-
   **
   ** @author	Diether Knof
   **
   ** @version	0.7.3
   **/
  void
    Server::game_finish()
    {
      for (vector<Connection*>::iterator c = this->connections().begin();
	   c != this->connections().end();
	   ++c)
	if ((*c)->relation() == Connection::CHILD)
	  (*c)->interpreter().sender().game_finish();
      return ;
    } // void Server::game_finish()

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
    Server::players_switched(Player const& player_a, Player const& player_b)
    {
      for (vector<Connection*>::iterator c = this->connections().begin();
	   c != this->connections().end();
	   ++c)
	if ((*c)->relation() == Connection::CHILD)
	  (*c)->interpreter().sender().players_switched(this->party().no(player_a),
							this->party().no(player_b));
      return ;
    } // void Server::players_switched(Player player_a, Player player_b)

  /**
   ** the player has changed
   **
   ** @param	player   player who has changed
   **
   ** @return	-
   **
   ** @author	Diether Knof
   **
   ** @version	0.7.2
   **/
  void
    Server::player_changed(Player const& player)
    {
      this->name_changed(player);
      return ;
    } // void Server::player_changed(Player player)

  /**
   ** the name of the player 'player' has changed
   **
   ** @param	player   player whose name has changed
   **
   ** @return	-
   **
   ** @author	Diether Knof
   **
   ** @version	0.7.2
   **/
  void
    Server::name_changed(Player const& player)
    {
      for (vector<Connection*>::iterator c = this->connections().begin();
	   c != this->connections().end();
	   ++c)
	if (!(*c)->manages_player(player))
          (*c)->interpreter().sender().player_name(player);
      return ;
    } // void Server::name_changed(Player player)

  /**
   ** update the rule
   **
   ** @param	type		the rule, that has changed
   ** @param	old_value	old value
   **
   ** @return	-
   **
   ** @author	Diether Knof
   **
   ** @version	0.7.2
   **/
  void
    Server::rule_changed(int const type, void const* const old_value)
    {
      for (vector<Connection*>::iterator c = this->connections().begin();
	   c != this->connections().end();
	   ++c)
	if ((*c)->relation() == Connection::CHILD)
	  (*c)->interpreter().sender().rule(type);

      return ;
    } // void Server::rule_changed(int type, void const* old_value)

  /**
   ** check whether the name was changed
   **
   ** @param	type		the setting, that has changed
   ** @param	old_value	old value
   **
   ** @return	-
   **
   ** @author	Diether Knof
   **
   ** @version	0.7.0
   **/
  void
    Server::setting_changed(int const type, void const* const old_value)
    {
      if (type == Setting::NAME) {
	for (vector<Connection*>::iterator c = this->connections().begin();
	     c != this->connections().end();
	     ++c)
	  (*c)->interpreter().sender().name();
      } // if (type == Setting::NAME)
    } // void Server::setting_changed(int type, void* old_value)


} // namespace Network

#endif // #ifdef USE_NETWORK
