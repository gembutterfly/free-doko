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

#include "../actor.h"

#include "../../party/party.h"
#include "../../party/rule.h"
#include "../../player/network/network_player.h"
#include "../../game/gameplay_actions.h"

namespace Network {

  /**
   ** check whether a pending game action is still pending. If not, delete it.
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
    Connection::Actor::check_pending_game_action()
    {
      // do not use this->pending_action() here, because this would lead to a recursion
      if (this->pending_action_ == NULL)
        return ;

      switch (this->pending_action_->type) {
      case GameplayAction::RESERVATION:
        if (::game_status != GAMESTATUS::GAME_RESERVATION)
          this->remove_pending_action();
	break;

      case GameplayAction::MARRIAGE:
	// nothing to do
	break;

      case GameplayAction::GENSCHER:
        if (this->game().type() == GAMETYPE::GENSCHER)
          this->remove_pending_action();
	break;

      case GameplayAction::POVERTY_SHIFT:
        // ToDo
	break;

      case GameplayAction::POVERTY_ACCEPTED:
        // ToDo
	break;

      case GameplayAction::POVERTY_RETURNED:
        // ToDo
	break;

      case GameplayAction::POVERTY_DENIED:
        // ToDo
	break;

      case GameplayAction::POVERTY_DENIED_BY_ALL:
	// nothing to do
	break;

      case GameplayAction::ANNOUNCEMENT: {
        GameplayAction::Announcement const& action
          = dynamic_cast<GameplayAction::Announcement const&>(*this->pending_action_);
        if (this->game().announcement_of_player(action.player)
            >= action.announcement)
          this->remove_pending_action();
      } break;

      case GameplayAction::SWINES:
        if (this->game().swines_announced())
          this->remove_pending_action();
	break;

      case GameplayAction::HYPERSWINES:
        if (this->game().hyperswines_announced())
          this->remove_pending_action();
	break;

      case GameplayAction::CARD_PLAYED: {
        GameplayAction::CardPlayed const& action
          = dynamic_cast<GameplayAction::CardPlayed const&>(*this->pending_action_);
        Player const& player = this->game().player(action.player);
        if (   this->game().trick_current().has_played(player)
            && (this->game().trick_current().card_of_player(player)
                == action.card) ) {
          this->remove_pending_action();
        }
      } break;

      case GameplayAction::TRICK_OPEN:
        break;

      case GameplayAction::TRICK_FULL:
        // ToDo: check the trick
        break;

      case GameplayAction::TRICK_TAKEN:
        if (   (::game_status == GAMESTATUS::GAME_FINISHED)
            || this->game().trick_current().isempty()) {
          this->remove_pending_action();
        }
        break;

      case GameplayAction::CHECK:
        // shall not happen
        DEBUG_ASSERTION(false,
                        "Gameplay action 'Check' should not be processed here");
        break;
      case GameplayAction::PRINT:
        // shall not happen
        DEBUG_ASSERTION(false,
                        "Gameplay action 'Print' should not be processed here");
        break;
      } // switch (action.type)

#if 0
      if (this->pending_action_)
        CLOG << *this->pending_action_ << endl;
#endif
      return ;
    } // void Connection::Actor::check_pending_game_action()

  /**
   ** a gameplay action has happened
   ** remove a pending action
   **
   ** @param     action   gameplay action
   **
   ** @return    -
   **
   ** @author    Diether Knof
   **
   ** @version   0.7.13
   **
   ** @todo      check pending actions for equality
   **/
  void
    Connection::Actor::check_gameplay_action(GameplayAction const& action)
    {
      if (!this->pending_action())
        return ;

      //CLOG << *this->pending_action() << endl;
      //CLOG << action << endl;
      //if (action.type == GameplayAction::TRICK_TAKEN) {
      //CLOG << this->pending_action()->type << endl;
      //}
      //if (this->pending_action()->type == GameplayAction::TRICK_TAKEN) {
      //CLOG << action.type << endl;
      //exit(0);
      //}
#ifdef POSTPONED
      if (*this->pending_action() == action)
#endif
        if (this->pending_action()->type == action.type)
          this->remove_pending_action();

      return ;
    } // void Connection::Actor::check_gameplay_action(GameplayAction action)

  /**
   ** a gameplay action
   **
   ** @param     action   gameplay action
   **
   ** @return    -
   **
   ** @author    Diether Knof
   **
   ** @version   0.7.13
   **
   ** @todo      mostly all
   **/
  void
    Connection::Actor::gameplay_action(GameplayAction const& action)
    {
      //COUT << "actor: gameplay action: " << action << endl;

      if (this->pending_action()) {
        //CLOG << *this->pending_action() << endl;
        //CLOG << action << endl;
        if (   (action.type != GameplayAction::RESERVATION)
            && (action.type != GameplayAction::POVERTY_SHIFT)
            && (action.type != GameplayAction::POVERTY_ACCEPTED)
            && (action.type != GameplayAction::POVERTY_RETURNED)
            && (action.type != GameplayAction::POVERTY_DENIED)
            && (action.type != GameplayAction::POVERTY_DENIED_BY_ALL)
            && (action.type != GameplayAction::SWINES)
            && (action.type != GameplayAction::HYPERSWINES)) {
          CLOG << *this->pending_action() << endl;
          CLOG << action << endl;
          SEGFAULT;
        }
      }
      this->remove_pending_action();

      NetworkPlayer* player = NULL;
      if (   (dynamic_cast<GameplayAction::Player1 const*>(&action))
          && (action.type != GameplayAction::ANNOUNCEMENT) ) {
        player = dynamic_cast<NetworkPlayer*>(&::party.game().player(dynamic_cast<GameplayAction::Player1 const&>(action).player));
        DEBUG_ASSERTION(player,
                        "Connection::Actor::gameplay_action(action)\n"
                        "  player is no network player.\n"
                        "  action: " << action << "\n"
                        "  player type: " << ::party.game().player(dynamic_cast<GameplayAction::Player1 const&>(action).player).type());
        //player = this->connection().managed_player(dynamic_cast<GameplayAction::Player1 const&>(action).player);
        if (player == NULL)
          return ;
      } // if (dynamic_cast<GameplayAction::Player1*>(&action))


      this->pending_action_ = action.clone();
      switch (action.type) {
      case GameplayAction::RESERVATION:
        player->set_reservation(dynamic_cast<GameplayAction::Reservation const&>(action).reservation);
        break;

      case GameplayAction::MARRIAGE:
        // nothing to do
        break;

      case GameplayAction::GENSCHER:
        player->set_genscher_partner(dynamic_cast<GameplayAction::Genscher const&>(action).partner);
        break;

      case GameplayAction::POVERTY_SHIFT:
        player->set_poverty_cards_to_shift(dynamic_cast<GameplayAction::PovertyShift const&>(action).cards);
        break;

      case GameplayAction::POVERTY_ACCEPTED:
        player->set_poverty_accept(true);
        break;

      case GameplayAction::POVERTY_RETURNED:
        player->set_poverty_cards_to_shift(dynamic_cast<GameplayAction::PovertyReturned const&>(action).cards);
        break;

      case GameplayAction::POVERTY_DENIED:
        player->set_poverty_accept(false);
        break;

      case GameplayAction::POVERTY_DENIED_BY_ALL:
        // nothing to do
        break;

      case GameplayAction::ANNOUNCEMENT: {
        // different handling, see 'Game::make_announcement_from_network()'
        GameplayAction::Announcement const& announcement_action
          = dynamic_cast<GameplayAction::Announcement const&>(action);
        DEBUG_ASSERTION(this->game().announcement_valid(announcement_action.announcement, this->game().player(announcement_action.player)),
                        "announcement " << announcement_action.announcement << " not valid\n"
                        << action);
        this->game().make_announcement_from_network(announcement_action.announcement,
                                                    this->game().player(announcement_action.player));
      } break;

      case GameplayAction::SWINES:
        if (   this->game().swines_announced()
            && (*player == *this->game().swines_owner()))
          break;
        CLOG << ::name(::game_status) << endl;
        this->set_known_card(*player, this->game().swine(), 2);
        DEBUG_ASSERTION(this->game().swines_announcement_valid(*player),
                        "swines announcement not valid\n"
                        << action);
        player->set_swines_announcement();
        break;

      case GameplayAction::HYPERSWINES:
        this->set_known_card(*player, this->game().hyperswine(), 2);
        DEBUG_ASSERTION(this->game().hyperswines_announcement_valid(*player),
                        "hyperswines announcement not valid\n"
                        << action);
        player->set_hyperswines_announcement();
        break;

      case GameplayAction::CARD_PLAYED:
        player->set_card_to_play(dynamic_cast<GameplayAction::CardPlayed const&>(action).card);
        break;

      case GameplayAction::TRICK_OPEN:
        break;

      case GameplayAction::TRICK_FULL:
        // ToDo: check the trick
        break;

      case GameplayAction::TRICK_TAKEN:
        for (vector<NetworkPlayer*>::const_iterator
             p = this->connection().players.begin();
             p != this->connection().players.end();
             ++p)
          (*p)->set_trick_taken();
        if (this->game().number_of_humans() == 0)
          ::game_status = GAMESTATUS::GAME_TRICK_TAKEN;
        break;

      case GameplayAction::CHECK:
        // shall not happen
        DEBUG_ASSERTION(false,
                        "Gameplay action 'Check' should not be processed here");
        break;
      case GameplayAction::PRINT:
        // shall not happen
        DEBUG_ASSERTION(false,
                        "Gameplay action 'Print' should not be processed here");
        break;
      } // switch (action.type)

      return ;
    } // void Connection::Actor::gameplay_action(GameplayAction action)

  /**
   ** open the game
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
    Connection::Actor::game_open()
    {
      this->game_players_.clear();
    } // void Connection::Actor::game_open()

  /**
   ** set the players
   **
   ** @param     players   the players
   **
   ** @return    -
   **
   ** @author    Diether Knof
   **
   ** @version   0.7.13
   **/
  void
    Connection::Actor::game_set_players(vector<Player*> const& players)
    {
      CLOG << players.size() << endl;
      for (unsigned p = 0; p < players.size(); ++p) {
        CLOG << players[p]->name() << endl;
      }
#if 1
      ::party.write_players_short(cout);
#endif
      if (&this->game_players_ != &players)
        this->game_players_ = players;
      if (   (::game_status & GAMESTATUS::GAME)
          && (::game_status <= GAMESTATUS::GAME_INIT) )
        this->game().set_players(players);

      return ;
    } // void Connection::Actor::game_set_players(vector<::Player*> players)

  /**
   ** set the startplayer
   **
   ** @param     startplayer   the startplayer
   **
   ** @return    -
   **
   ** @author    Diether Knof
   **
   ** @version   0.7.13
   **/
  void
    Connection::Actor::game_set_startplayer(unsigned const startplayer)
    {
      if (!(   (::game_status >= GAMESTATUS::GAME_NEW)
            && (::game_status <= GAMESTATUS::GAME_RESERVATION) ) )
        return ;

      this->game().set_startplayer(startplayer);
      CLOG << startplayer << endl;

      return ;
    } // void Connection::Actor::game_start_round(unsigned startplayer)

  /**
   ** set the hands
   **
   ** @param     hands   hands of the players
   **
   ** @return    -
   **
   ** @author    Diether Knof
   **
   ** @version   0.7.13
   **/
  void
    Connection::Actor::game_distribute_cards(vector<Hand*> const& hands)
    {
      DEBUG_ASSERTION((::game_status == GAMESTATUS::GAME_INIT),
                      "Connection::Actor::game_distribute_cards(hands)\n"
                      "::game_status = " << ::game_status << " != GAME_INIT");
      this->game().distributecards(hands);
      return ;
    } // void Connection::Actor::game_distribute_cards(vector<Hand*> hands)

  /**
   ** set the team
   **
   ** @param     playerno   player number
   ** @param     team       team of the player
   **
   ** @return    -
   **
   ** @author    Diether Knof
   **
   ** @version   0.7.13
   **
   ** @todo      all
   **/
  void
    Connection::Actor::set_team(unsigned const playerno, Team const team)
    {
      if (this->game().player(playerno).team() == team)
        return ;
      // todo all
      DEBUG_ASSERTION( (this->game().player(playerno).team() == TEAM::UNKNOWN),
                      "Connection::Actor::set_team(" << playerno << ", " << team << ")\n"
                      "  team already set: " << this->game().player(playerno).team());

      this->game().player(playerno).set_team(team);
      this->game().set_teaminfo(this->game().player(playerno), team);

      return ;
    } // void Connection::Actor::set_team(unsigned playerno, Team team);

  /**
   ** set known cards of the player
   **
   ** @param     player     player
   ** @param     card       card
   ** @param     n          number of card (default: 1)
   **
   ** @return    -
   **
   ** @author    Diether Knof
   **
   ** @version   0.7.13
   **/
  void
    Connection::Actor::set_known_card(Player& player,
                                      Card const& card, unsigned const n)
    {
      Hand& hand = player.hand();
      unsigned k = hand.numberofall(card);
      if (k >= n)
        return;
      DEBUG_ASSERTION(k + hand.numberofall(Card::UNKNOWN) >= n,
                      "Connection::Actor::set_known_card(" << player.name() << ", "
                      << card << ", " << n << ")\n"
                      "  not possible:\n"
                      << hand);
      hand.set_unknown(card, n - k);

      return ;
    } // void Connection::Actor::set_known_card(Player player, Team team, unsigned n = 1)

  /**
   ** set known cards of the player
   **
   ** @param     playerno   player number
   ** @param     card       card
   ** @param     n          number of card (default: 1)
   **
   ** @return    -
   **
   ** @author    Diether Knof
   **
   ** @version   0.7.13
   **/
  void
    Connection::Actor::set_known_card(unsigned const playerno,
                                      Card const& card, unsigned const n)
    {
      this->set_known_card(this->game().player(playerno), card, n);
      return ;
    } // void Connection::Actor::set_known_card(unsigned playerno, Team team, unsigned n = 1)

  /**
   ** start the game
   **
   ** @param     -
   **
   ** @return    -
   **
   ** @author    Diether Knof
   **
   ** @version   0.7.2
   **
   ** @todo      all
   **/
  void
    Connection::Actor::game_start()
    {
      if (this->game_players_.empty())
        this->game_players_ = const_cast<Game const&>(this->game()).players();
      else
        this->game_set_players(this->game_players_);
      return ;
    } // void Connection::Actor::game_start()

  /**
   ** finish the game
   **
   ** @param     -
   **
   ** @return    -
   **
   ** @author    Diether Knof
   **
   ** @version   0.7.2
   **
   ** @todo      all
   **/
  void
    Connection::Actor::game_finish()
    {
      return ;
    } // void Connection::Actor::game_finish()

  /**
   ** close the game
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
    Connection::Actor::game_close()
    {
      ::game_status = GAMESTATUS::GAME_CLOSED;
      return ;
    } // void Connection::Actor::game_close()

} // namespace Network

#endif // #ifdef USE_NETWORK
