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

#include "constants.h"

#ifdef USE_UI_GTKMM

#include "ui.h"

#include "main_window.h"
#include "menu.h"
#include "table.h"
#include "name.h"
#include "hand.h"
#include "trick.h"
#include "poverty.h"
#include "reservation.h"
#include "players_db.h"

#include "../../party/party.h"
#include "../../party/rule.h"
#include "../../game/game.h"
#include "../../game/gameplay_actions.h"
#include "../../card/trick.h"
#include "../../player/player.h"
#include "../../os/bug_report_replay.h"
#include "../../basistypes.h"

namespace UI_GTKMM_NS {

/**
 ** the game is opened
 **
 ** @param     game   the game that is opened
 **
 ** @return    -
 **/
void
  UI_GTKMM::game_open(Game& game)
  {
    this->thrower.inc_depth();

    this->UI::game_open(game);
    this->table->game_open();

    this->thrower.dec_depth();
  } // void UI_GTKMM::game_open(Game& game)

/**
 ** the cards are distributed
 **
 ** @param     -
 **
 ** @return    -
 **/
void
  UI_GTKMM::game_cards_distributed()
  {
    this->thrower.inc_depth();

    this->table->game_cards_distributed();

    this->thrower.dec_depth();
  } // void UI_GTKMM::game_cards_distributed()

/**
 ** ask 'player' for a reservation
 **
 ** @param     player   player who is asked
 **
 ** @return    -
 **/
void
  UI_GTKMM::reservation_ask(Player const& player)
  {
    this->thrower.inc_depth();

    this->table->reservation_ask(player);

    this->thrower.dec_depth();
  } // void UI_GTKMM::reservation_ask(Player player)

/**
 ** Gets the reservation of the given player:
 ** pops up a window
 ** This is the first time, the hands can (should) be shown.
 **
 ** @param     player   player, whose reservation schould be get
 **
 ** @return    reservation of the player
 **/
::Reservation
  UI_GTKMM::reservation_get(Player const& player)
  {
    this->thrower.inc_depth();

    auto const result = this->table->reservation_get(player);

    this->thrower.dec_depth();
    return result;
  } // ::Reservation UI_GTKMM::reservation_get(player)

/**
 ** 'player' has a poverty and shifts cards
 **
 ** @param     player   the player who has the poverty
 **
 ** @return    -
 **/
void
  UI_GTKMM::poverty_shifting(Player const& player)
  {
    this->thrower.inc_depth();

    this->table->draw_all();

    this->thrower.dec_depth();
  } // void UI_GTKMM::poverty_shifting(Player player)

/**
 ** 'player' shifts 'cardno' cards
 **
 ** @param     player   the player who shifts the cards
 ** @param     cardno   the number of cards that are shifted
 **
 ** @return    -
 **/
void
  UI_GTKMM::poverty_shift(Player const& player, unsigned const cardno)
  { 
    this->thrower.inc_depth();

    this->table->poverty().shift(player, cardno);

    this->thrower.dec_depth();
  } // void UI_GTKMM::poverty_shift(Player player, unsigned cardno)

/**
 ** ask 'player' whether to accept the poverty
 **
 ** @param     player   the player who is asked
 ** @param     cardno   the number of cards that are shifted
 **
 ** @return    -
 **/
void
  UI_GTKMM::poverty_ask(Player const& player, unsigned const cardno)
  { 
    this->thrower.inc_depth();

    this->table->poverty().ask(player, cardno);

    this->thrower.dec_depth();
  } // void UI_GTKMM::poverty_ask(Player player, unsigned cardno)

/**
 ** 'player' denied to take the shifted cards
 **
 ** @param     player   the player who has denied to take the cards
 **
 ** @return    -
 **/
void
  UI_GTKMM::poverty_take_denied(Player const& player)
  { 
    this->thrower.inc_depth();

    this->table->poverty().take_denied(player);

    this->thrower.dec_depth();
  } // void UI_GTKMM::poverty_take_denied(Player player)

/**
 ** all players have denied to take the cards
 **
 ** @param     -
 **
 ** @return    -
 **
 ** @todo	show a window
 **/
void
  UI_GTKMM::poverty_take_denied_by_all()
  { 
    this->thrower.inc_depth();

    this->table->poverty().take_denied_by_all();

    this->thrower.dec_depth();
  } // void UI_GTKMM::poverty_take_denied_by_all()

/**
 ** 'player' accepts to take the shifted cards
 ** and returns 'cardno' cards with 'trumpno' trumps
 **
 ** @param     player   the player who has denied to take the cards
 ** @param     cardno   number of cards that are given back
 ** @param     trumpno   number of trumps of the cards
 **
 ** @return    -
 **/
void
  UI_GTKMM::poverty_take_accepted(Player const& player,
                                  unsigned const cardno,
                                  unsigned const trumpno)
  { 
    this->thrower.inc_depth();

    this->table->poverty().take_accepted(player, cardno, trumpno);

    this->thrower.dec_depth();
  } // void UI_GTKMM::poverty_take_accepted(Player player, unsigned cardno, unsigned trumpno)

/**
 ** returns which cards the player shifts
 **
 ** @param     player   the player who shifts the cards
 **
 ** @return    the cards that are to be shifted
 **/
HandCards
  UI_GTKMM::poverty_shift(Player& player)
  { 
    this->thrower.inc_depth();

    auto const result = this->table->poverty().shift(player);

    this->thrower.dec_depth();
    return result;
  } // vector<HAndCard> UI_GTKMM::poverty_shift(Player& player)

/**
 ** returns whether 'player' accepts the shifted cards
 **
 ** @param     player   the player who shifts the cards
 ** @param     cardno   the number of shifted cards
 **
 ** @return    whether to accept the cards
 **/
bool
  UI_GTKMM::poverty_take_accept(Player& player, unsigned cardno)
  { 
    this->thrower.inc_depth();

    auto const result = this->table->poverty().take_accept(player, cardno);

    this->thrower.dec_depth();
    return result;
  } // void UI_GTKMM::poverty_take_accept(Player& player, unsigned cardno)

/**
 ** changes the cards from the poverty-player
 **
 ** @param     player   the player who has accepted the cards
 ** @param     cards   the cards that are given to the player
 **
 ** @return    the cards that are returned to the poverty-player
 **/
HandCards
  UI_GTKMM::poverty_cards_change(Player& player,
                                 HandCards const& cards)
  { 
    this->thrower.inc_depth();

    auto const result = this->table->poverty().cards_change(player, cards);

    this->thrower.dec_depth();
    return result;
  } // void UI_GTKMM::poverty_cards_change(Player& player, HandCards cards)

/**
 ** the poverty player 'player' gets 'cards'
 **
 ** @param     player   the player gets the cards
 ** @param     cards   the cards that are given to the player
 **
 ** @return    -
 **/
void
  UI_GTKMM::poverty_cards_get_back(Player& player,
                                   HandCards const& cards)
  { 
    this->thrower.inc_depth();

    this->table->poverty().cards_get_back(player, cards);

    this->thrower.dec_depth();
  } // void UI_GTKMM::poverty_cards_get_back(Player& player, HandCards cards)

/**
 ** the game is redistributed
 **
 ** @param     -
 **
 ** @return    -
 **/
void
  UI_GTKMM::game_redistribute()
  {
    this->thrower.inc_depth();

    this->table->game_redistribute();

    this->thrower.dec_depth();
  } // void UI_GTKMM::game_redistribute()

/**
 ** the game is started:
 ** the gametype and announcements of the other players are shown
 **
 ** @param     -
 **
 ** @return    -
 **/
void
  UI_GTKMM::game_start()
  {
    this->thrower.inc_depth();

    this->main_window->menu->game_start();
    this->table->game_start();

    this->thrower.dec_depth();
  } // void UI_GTKMM::game_start()

/**
 ** a new trick is opened
 **
 ** @param     trick   trick that is opened
 **
 ** @return    -
 **/
void
  UI_GTKMM::trick_open(::Trick const& trick)
  {
    this->thrower.inc_depth();

    this->UI::trick_open(trick);
    this->main_window->menu->trick_open();
    this->table->draw_all();

    this->thrower.dec_depth();
  } // void UI_GTKMM::trick_open(::Trick trick)

/**
 ** the trick is full.
 ** show the 'full trick'-window or wait
 **
 ** @param     -
 **
 ** @return    -
 **/
void
  UI_GTKMM::trick_full()
  {
    this->thrower.inc_depth();

    this->main_window->menu->announcements_update();
    this->table->trick_full();

    this->thrower.dec_depth();
  } // void UI_GTKMM::trick_full()

/**
 ** move the trick in the pile of the winnerplayer
 **
 ** @param     -
 **
 ** @return    -
 **/
void
  UI_GTKMM::trick_move_in_pile()
  {
    this->thrower.inc_depth();

    this->table->trick_move_in_pile();

    this->thrower.dec_depth();
  } // void UI_GTKMM::trick_move_in_pile()

/**
 ** the trick is closed
 **
 ** @param     -
 **
 ** @return    -
 **/
void
  UI_GTKMM::trick_close()
  {
    this->thrower.inc_depth();

    this->UI::trick_close();

    this->thrower.dec_depth();
  } // void UI_GTKMM::trick_close()

/**
 ** a gameplay action
 **
 ** @param     action   the action
 **
 ** @return    -
 **/
void
  UI_GTKMM::gameplay_action(GameplayAction const& action)
  {
    this->thrower.inc_depth();

    this->table->gameplay_action(action);

    switch (action.type) {
    case GameplayAction::ANNOUNCEMENT: {
      auto const& announcement
        = static_cast<GameplayAction::Announcement const&>(action).announcement;
      auto const& player
        = this->game().player(static_cast<GameplayAction::Announcement const&>(action).player);
      this->main_window->menu->announcements_update();
      this->table->announcement_made(announcement, player);
      break;
    }

    case GameplayAction::SWINES: {
      auto const& player
        = this->game().player(static_cast<GameplayAction::Swines const&>(action).player);
      this->main_window->menu->announcements_update();
      this->table->swines_announced(player);
    }
      break;

    case GameplayAction::HYPERSWINES: {
      auto const& player
        = this->game().player(static_cast<GameplayAction::Hyperswines const&>(action).player);
      this->main_window->menu->announcements_update();
      this->table->hyperswines_announced(player);
      break;
    }

    case GameplayAction::MARRIAGE: {
      auto const& bridegroom
        = this->game().player(static_cast<GameplayAction::Marriage const&>(action).player);
      auto const& bride
        = this->game().player(static_cast<GameplayAction::Marriage const&>(action).bride);
      this->main_window->menu->announcements_update();
      this->main_window->menu->language_update();
      this->table->marriage(bridegroom, bride);
      break;
    }

    case GameplayAction::GENSCHER: {
      auto const& genscher
        = this->game().player(static_cast<GameplayAction::Genscher const&>(action).player);
      auto const& partner
        = this->game().player(static_cast<GameplayAction::Genscher const&>(action).partner);
      this->main_window->menu->announcements_update();
      this->table->genscher(genscher, partner);
      break;
    }

    case GameplayAction::RESERVATION:
      // nothing to do
      break;
    case GameplayAction::CARD_PLAYED:
      // much to do, see this->card_played(HandCard const& card)
      break;

    case GameplayAction::POVERTY_SHIFT:
    case GameplayAction::POVERTY_ACCEPTED:
    case GameplayAction::POVERTY_RETURNED:
    case GameplayAction::POVERTY_DENIED:
    case GameplayAction::POVERTY_DENIED_BY_ALL:
    case GameplayAction::TRICK_OPEN:
    case GameplayAction::TRICK_FULL:
    case GameplayAction::TRICK_TAKEN:
    case GameplayAction::CHECK:
    case GameplayAction::PRINT:
      // ToDo
      break;
    } // switch (action.type)

    this->thrower.dec_depth();
  } // void UI_GTKMM::gameplay_action(GameplayAction action)

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    selected partner of the player
 **		(nullptr if the genscher is not announced)
 **
 **/
Player const*
  UI_GTKMM::genscher_partner()
  {
    this->thrower.inc_depth();

    auto const result = this->table->select_genscher_partner(::party.game().player_current());

    this->thrower.dec_depth();
    return result;
  } // Player const* UI_GTKMM::genscher_partner()

/**
 ** let the user select a card for the given player
 **
 ** @param     player   the player who is to play a card
 **
 ** @return    the selected card
 **/
HandCard
  UI_GTKMM::card_get(Player const& player)
  {
    this->thrower.inc_depth();

    this->table->card_get(player);

    this->main_window->menu->card_get();
    const_cast<Player&>(player).hand().forget_request();

    try {
      if (   (   ::setting(Setting::AUTOMATIC_CARD_SUGGESTION)
              && this->game().rule()(Rule::CARD_HINT))
          || (   ::bug_report_replay
              && (player.type() == Player::HUMAN) )
         )
        this->table->show_card_suggestion(false);

      do {
        while (   !this->ui->thrower
               && (::game_status == GAMESTATUS::GAME_PLAY))
          ::ui->wait();
        if (this->ui->thrower) {
          try {
            this->thrower.throw_it();
          } catch (Card const& card) {
            DEBUG_ASSERTION((card == player.hand().requested_card()),
                            "UI_GTKMM::card_get(player):\n"
                            "  did not catch the requested card:\n"
                            "  " << card << " != " << player.hand().requested_card()
                           );
          } // try
        } // if (this->ui->thrower)

        if (player.hand().requested_card().is_empty()) {
          const_cast<Player&>(player).hand().forget_request();
          continue;
        }
        // the player cannot play an invalid card
        if (!player.game().trick_current().isvalid(player.hand().requested_card(),
                                                   player.hand())) {
          const_cast<Player&>(player).hand().forget_request();
          continue;
        }

      } while (player.hand().requested_position() == UINT_MAX);
    } catch (...) {
      this->table->card_got();
      this->main_window->menu->card_got();
      throw;
    } // catch(...)

    this->table->card_got();
    this->main_window->menu->card_got();

    this->thrower.dec_depth();
    return player.hand().requested_card();
  } // HandCard UI_GTKMM::card_get(Player player)

/**
 ** the given player has played 'card.
 ** remove the card from the hand
 **
 ** @param     card   the played card
 **
 ** @return    -
 **/
void
  UI_GTKMM::card_played(HandCard const& card)
  { 
    this->thrower.inc_depth();

    auto const& player = card.player();

    // for the sensitivity of the swines button
    this->main_window->menu->announcements_update();

    // for updating the heuristics
    this->players_db->update_db();

    this->table->hand(player).draw();
    this->table->name(player).draw();
    this->table->trick().draw();
    this->table->name(player.game().player_current()).draw();

    if (::setting(Setting::SHOW_ALL_HANDS)
        || (::game_status == GAMESTATUS::GAME_FINISHED)
        || (::game_status == GAMESTATUS::GAME_REDISTRIBUTE)
        || !::setting(Setting::SHOW_AI_INFORMATION_HANDS)
        || (player.game().number_of_humans() != 1)
       )
      this->table->card_played(player);
    else
      this->table->draw_all();

    this->update();

    this->thrower.dec_depth();
  } // void UI_GTKMM::card_player(HandCard card)

/**
 ** the game is finished:
 ** the winner and the points of the game are shown.
 **
 ** @param     -
 **
 ** @return    -
 **/
void
  UI_GTKMM::game_finish()
  { 
    this->thrower.inc_depth();

    this->players_db->update_db();
    this->main_window->menu->game_finish();
    this->table->game_finished();

    this->thrower.dec_depth();
  } // void UI_GTKMM::game_finish()

/**
 ** the game is closed
 **
 ** @param     -
 **
 ** @return    -
 **/
void
  UI_GTKMM::game_close()
  {
    this->thrower.inc_depth();

    this->UI::game_close();

    this->main_window->menu->game_close();
    this->table->game_close();

    this->thrower.dec_depth();
  } // void UI_GTKMM::game_close()

} // namespace UI_GTKMM_NS

#endif // #ifdef USE_UI_GTKMM
