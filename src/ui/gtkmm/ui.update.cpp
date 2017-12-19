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
#include "translations.h"
#include "splash_screen.h"
#include "cards.h"
#include "icons.h"
#include "party_summary.h"
#include "party_settings.h"
#include "players_db.h"
#include "game_debug.h"
#include "rules.h"
#include "table.h"
#include "game_finished.h"
#include "game_summary.h"
#include "party_points.h"
#include "party_finished.h"
#include "hand.h"
#include "icongroup.h"
#include "license.h"
#include "main_window.h"
#ifdef USE_NETWORK
#include "network.h"
#endif

#include "../../basistypes.h"
#include "../../party/party.h"
#include "../../misc/setting.h"

#include <gtkmm/label.h>
#include <gtkmm/button.h>
#include <gtkmm/textview.h>

namespace UI_GTKMM_NS {

/**
 ** updates all language elements
 **
 ** @param     -
 **
 ** @return    -
 **/
void
  UI_GTKMM::language_changed()
  {
    this->thrower.inc_depth();

    this->translations->language_update();

    for (auto p : this->parts)
      p->language_update();

    this->thrower.dec_depth();
  } // void UI_GTKMM::language_changed()

/**
 ** updates all cards
 **
 ** @param     -
 **
 ** @return    -
 **/
void
  UI_GTKMM::cards_changed()
  {
    this->thrower.inc_depth();

    if (this->main_window)
      this->main_window->set_minimal_size();

    if (this->cards)
      this->cards->update_managed();

    if (this->icons)
      this->icons->update_managed();

    if (this->license)
      this->license->license_update();

    for (auto p : this->parts)
      p->cards_update();

    if (this->table)
      this->table->mouse_cursor_update();

    this->thrower.dec_depth();
  } // void UI_GTKMM::cards_changed()

/**
 ** updates all cards back
 **
 ** @param     -
 **
 ** @return    -
 **/
void
  UI_GTKMM::cards_back_changed()
  {
    this->thrower.inc_depth();

    for (auto p : this->parts)
      p->cards_back_update();

    if (this->table)
      this->table->mouse_cursor_update();

    this->thrower.dec_depth();
  } // void UI_GTKMM::cards_back_changed()

/**
 ** updates all icons
 **
 ** @param     -
 **
 ** @return    -
 **/
void
  UI_GTKMM::icons_changed()
  {
    this->thrower.inc_depth();

    this->icons->update_managed();

    if (this->table)
      this->table->icons_update();

    if (this->license)
      this->license->license_update();

    this->thrower.dec_depth();
  } // void UI_GTKMM::icons_changed()

/**
 ** redraw all (of the playfield)
 **
 ** @param     -
 **
 ** @return    -
 **/
void
  UI_GTKMM::redraw_all()
  { 
    this->thrower.inc_depth();

    if (this->table) {
      this->table->draw_all();
      this->table->mouse_cursor_update();
    }

    this->thrower.dec_depth();
  } // void UI_GTKMM::redraw_all()

/**
 ** update the gametype icon
 **
 ** @param     -
 **
 ** @return    -
 **/
void
  UI_GTKMM::gametype_changed()
  { 
    this->thrower.inc_depth();

    if (this->table)
      this->table->draw_all();

    this->thrower.dec_depth();
  } // void UI_GTKMM::gametype_changed()

/**
 ** the players 'player_a' and 'player_b' have been switched
 **
 ** @param     player_a   first player
 ** @param     player_b   second player
 **
 ** @return    -
 **/
void
  UI_GTKMM::players_switched(Player const& player_a, Player const& player_b)
  {
    this->thrower.inc_depth();

    if (this->party_settings)
      this->party_settings->players_switched(player_a, player_b);

    this->thrower.dec_depth();
  } // void UI_GTKMM::players_switched(Player player_a, Player player_b)

/**
 ** update the 'player'
 **
 ** @param     player   the player
 **
 ** @return    -
 **/
void
  UI_GTKMM::player_changed(Player const& player)
  {
    this->thrower.inc_depth();

    if (this->party_settings)
      this->party_settings->player_update(player);
    this->name_changed(player);
    this->voice_changed(player);

    this->thrower.dec_depth();
  } // void UI_GTKMM::player_changed(Player player)

/**
 ** add the 'player'
 **
 ** @param     player   the player
 **
 ** @return    -
 **/
void
  UI_GTKMM::player_added(Player const& player)
  {
    this->thrower.inc_depth();

    if (this->table)
      this->table->party_points_->player_added(player);
    this->name_changed(player);
    this->voice_changed(player);

    this->thrower.dec_depth();
  } // void UI_GTKMM::player_added(Player player)

/**
 ** sparse players have been removed
 **
 ** @param     -
 **
 ** @return    -
 **/
void
  UI_GTKMM::sparse_players_removed()
  {
    this->thrower.inc_depth();

    if (this->table)
      this->table->party_points_->recreate_all();

    this->thrower.dec_depth();
  } // void UI_GTKMM::sparse_players_removed()

/**
 ** update the name of 'player'
 **
 ** @param     player   the player
 **
 ** @return    -
 **/
void
  UI_GTKMM::name_changed(Player const& player)
  {
    this->thrower.inc_depth();

#ifdef WORKAROUND
    { // name: change to utf8
      auto const name = UI_GTKMM::to_utf8(player.name());
      if (name != player.name()) {
        const_cast<Player&>(player).set_name(name);
        return ;
      }
    } // name: change to utf8
#endif

    if (this->players_db)
      this->players_db->name_changed(player);
    if (this->party_settings)
      this->party_settings->name_update(player);
    if (this->party_summary)
      this->party_summary->name_update(player);
    if (this->table) {
      this->table->name_changed(player);
      this->table->mouse_cursor_update();
    }
    if (this->game_debug)
      this->game_debug->name_changed(player);

    this->thrower.dec_depth();
  } // void UI_GTKMM::name_changed(Player player)

/**
 ** update the voice of 'player'
 **
 ** @param     player   the player
 **
 ** @return    -
 **/
void
  UI_GTKMM::voice_changed(Player const& player)
  {
    this->thrower.inc_depth();

    if (this->party_settings)
      this->party_settings->voice_update(player);

    this->thrower.dec_depth();
  } // void UI_GTKMM::voice_changed(Player player)

/**
 ** update the hand of 'player'
 **
 ** @param     player   the player
 **
 ** @return    -
 **/
void
  UI_GTKMM::hand_changed(Player const& player)
  { 
    this->thrower.inc_depth();

    if (this->table) {
      this->table->hand(player).draw();
      this->table->mouse_cursor_update();
    }

    this->thrower.dec_depth();
  } // void UI_GTKMM::hand_changed(Player player)

/**
 ** update the team icon of 'player'
 **
 ** @param     player   the player
 **
 ** @return    -
 **/
void
  UI_GTKMM::teaminfo_changed(Player const& player)
  { 
    this->thrower.inc_depth();

    if (this->table) {
      this->table->icongroup(player).draw_team();
      this->table->mouse_cursor_update();
    }

    this->thrower.dec_depth();
  } // void UI_GTKMM::teaminfo_changed(Player player)

/**
 ** update the 'aiconfig'
 **
 ** @param     aiconfig   the aiconfig
 **
 ** @return    -
 **/
void
  UI_GTKMM::aiconfig_changed(Aiconfig const& aiconfig)
  {
    this->thrower.inc_depth();

    if (this->party_settings)
      this->party_settings->aiconfig_update(aiconfig);

    this->thrower.dec_depth();
  } // void UI_GTKMM::aiconfig_changed(Aiconfig aiconfig)

/**
 ** update the rule
 **
 ** @param     type   	the rule, that has changed
 ** @param     old_value   old value
 **
 ** @return    -
 **/
void
  UI_GTKMM::rule_changed(int const type, void const* const old_value)
  {
    this->thrower.inc_depth();

    if (this->party_settings) {
      this->party_settings->rules_update();
      this->party_settings->rules->update(type);
    }
#ifdef USE_NETWORK
    if (this->party_settings)
      this->network->rule_update(type);
#endif

    switch (type) {
    case Rule::COUNTING: {
      if (this->table
          && this->table->game_finished_
          && this->table->game_finished_->game_summary)
        this->table->game_finished_->game_summary->update();

      if (this->table
          && this->table->party_points_)
        this->table->party_points_->recalc_all();

      if (this->table
          && this->table->party_finished_)
        this->table->party_finished_->redraw_points_graph();

      if (this->table
          && (::game_status == GAMESTATUS::PARTY_FINISHED))
        this->table->draw_all();
    }
      break;

    case Rule::WITH_NINES: {
      if (this->party_settings)
        this->party_settings->update();
    }
      break;

    case Rule::NUMBER_OF_DUTY_SOLI:
    case Rule::NUMBER_OF_DUTY_COLOR_SOLI:
    case Rule::NUMBER_OF_DUTY_PICTURE_SOLI:
      if (this->table
          && this->table->party_points_)
        this->table->party_points_->recreate_all();

    default:
      break;
    } // switch (type)

#ifndef RELEASE
    this->table->draw_all();
#endif

    this->thrower.dec_depth();
  } // void UI_GTKMM::rule_changed(int type, void const* old_value)

/**
 ** update the setting
 **
 ** @param     type   	the setting, that has changed
 ** @param     old_value   old value
 **
 ** @return    -
 **/
void
  UI_GTKMM::setting_changed(int const type, void const* const old_value)
  {
    this->thrower.inc_depth();

    switch(type) {
    case Setting::SHOW_AI_INFORMATION_HANDS:
    case Setting::SHOW_AI_INFORMATION_TEAMS:
      if (this->table)
        this->table->draw_all();
      break;

    case Setting::ANNOUNCE_IN_TABLE:
      this->redraw_all();
      break;

    case Setting::NAME:
#ifdef WORKAROUND
      { // convert latin to utf8
        auto const name = UI_GTKMM::to_utf8(::setting(Setting::NAME));
        if (name != ::setting(Setting::NAME)) {
          ::setting.set(Setting::NAME, name);
    this->thrower.dec_depth();
          return ;
        }
      } // convert latin to utf8
#endif
      for (unsigned p = 0; p < ::party.playerno(); p++)
        this->name_changed(::party.player(p));
      break;

    case Setting::CARDSET:
      if (this->cards)
        this->cards->load_all();
      if (this->icons)
        this->icons->load();

      if (this->cards)
        this->cards_changed();
      break;

    case Setting::CARDS_BACK:
      if (this->cards) {
        this->cards->load_back();
        this->cards_changed();
      }
      break;

    case Setting::ICONSET:
      if (this->icons)
        this->icons->load();

      if (this->cards)
        this->cards_changed();
      break;

    case Setting::ORIGINAL_CARDS_SIZE:
      if (::setting(Setting::ORIGINAL_CARDS_SIZE) == true)
        ::setting.set(Setting::CARDS_HEIGHT, this->cards->height_original());
    case Setting::CARDS_HEIGHT:
      if (this->cards)
        this->cards->update_scaling();
      if (this->icons)
        this->icons->update_scaling();

      this->cards_changed();
      break;

    case Setting::SHOW_KNOWN_TEAMS_IN_GAME:
    case Setting::SHOW_SOLOPLAYER_IN_GAME:
      if (this->table)
        this->table->draw_all();
      break;

    case Setting::ADDITIONAL_PARTY_SETTINGS:
      if (this->party_settings)
        this->party_settings->update();
      break;

    default:
      break;
    } // switch(type)

    for (auto p : this->parts)
      p->setting_update(type);

    this->thrower.dec_depth();
  } // void UI_GTKMM::setting_changed(int type, void const* old_value)

/**
 ** the status message has changed
 **
 ** @param     status_message   the status message
 **
 ** @return    -
 **/
void
  UI_GTKMM::status_message_changed(Translator::Translation const& status_message)
  {
    this->thrower.inc_depth();

    if (this->splash_screen)
      this->splash_screen->status_message_changed(status_message);

    this->thrower.dec_depth();
  } // void UI_GTKMM::status_message_changed(Translator::Translation status_message)

/**
 ** the program is busy
 **
 ** @param     -
 **
 ** @return    -
 **/
void
  UI_GTKMM::set_busy()
  {
    this->thrower.inc_depth();

    this->UI::set_busy();
    this->table->mouse_cursor_update();

    this->thrower.dec_depth();
  } // void UI_GTKMM::set_busy()

/**
 ** the program is not busy any more
 **
 ** @param     -
 **
 ** @return    -
 **/
void
  UI_GTKMM::set_not_busy()
  {
    this->thrower.inc_depth();

    this->UI::set_not_busy();
    this->table->mouse_cursor_update();

    this->thrower.dec_depth();
  } // void UI_GTKMM::set_not_busy()

/**
 ** the progress has changed
 **
 ** @param     progress   the progress
 **
 ** @return    -
 **/
void
  UI_GTKMM::progress_changed(double const progress)
  {
    this->thrower.inc_depth();

    if (this->splash_screen)
      this->splash_screen->progress_changed(progress);

    if (this->table)
      this->table->progress_changed(progress);

    this->thrower.dec_depth();
  } // void UI_GTKMM::progress_changed(double progress)

/**
 ** the progress is finished
 **
 ** @param     -
 **
 ** @return    -
 **/
void
  UI_GTKMM::progress_finished()
  {
    this->thrower.inc_depth();

    if (this->table)
      this->table->progress_finished();

    this->thrower.dec_depth();
  } // void UI_GTKMM::progress_finished()

} // namespace UI_GTKMM_NS

#endif // #ifdef USE_UI_GTKMM
