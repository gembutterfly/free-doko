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
#include "party_summary.h"
#include "party_settings.h"
#include "players_db.h"
#include "players.h"
#include "rules.h"
#include "table.h"
#include "party_points.h"
#ifdef USE_NETWORK
#include "network.h"
#endif

#include "../../basistypes.h"
#include "../../party/party.h"

#include <gtkmm/main.h>

namespace UI_GTKMM_NS {

/**
 ** start a new party
 **
 ** @param     -
 **
 ** @return    -
 **/
void
  UI_GTKMM::start_new_party()
  {
    this->thrower(GAMESTATUS::PARTY_NEW, __FILE__, __LINE__);
    this->main_quit();

    return ;
  } // void UI_GTKMM::start_new_party()

/**
 ** end a party
 **
 ** @param     -
 **
 ** @return    -
 **/
void
  UI_GTKMM::end_party()
  {
    this->thrower(GAMESTATUS::PARTY_FINISHED, __FILE__, __LINE__);
    this->main_quit();

    return ;
  } // void UI_GTKMM::end_party()

/**
 ** the party is opened
 **
 ** @param     party   party that is opened
 **
 ** @return    -
 **/
void
  UI_GTKMM::party_open(Party& party)
  {
    this->thrower.inc_depth();

    this->UI::party_open(party);

    this->main_window->menu->party_open();
    this->table->party_open();
    this->players_db->party_open();
#ifdef USE_NETWORK
    this->network->party_open();
#endif

    this->thrower.dec_depth();
  } // void UI_GTKMM::party_open()

/**
 ** gets and sets the settings of the party:
 **   names, types and configuration of the players,
 **   rules,
 **   starting seed
 **   startplayer
 **
 ** @param     -
 **
 ** @return    -
 **/
void
  UI_GTKMM::party_get_settings()
  {
    this->thrower.inc_depth();

    if (!(FAST_PLAY & FAST_NS::PARTY_START))
      this->party_settings->get();

    this->thrower.dec_depth();
  } // void UI_GTKMM::party_get_settings()

/**
 ** the party is loaded
 **
 ** @param     -
 **
 ** @return    -
 **/
void
  UI_GTKMM::party_loaded()
  {
    this->thrower.inc_depth();

    if (this->table
        && this->table->party_points_)
      this->table->party_points_->recreate_all();
    if (!(FAST_PLAY & FAST_NS::PARTY_START))
      this->party_summary->party_loaded();

    this->thrower.dec_depth();
  } // void UI_GTKMM::party_loaded()

/**
 ** the seed of the party has changed
 **
 ** @param     -
 **
 ** @return    -
 **/
void
  UI_GTKMM::party_seed_changed()
  {
    this->thrower.inc_depth();

    if (this->party_settings)
      this->party_settings->update();

    this->thrower.dec_depth();
  } // void UI_GTKMM::party_seed_changed()

/**
 ** the startplayer of the party has changed
 **
 ** @param     -
 **
 ** @return    -
 **/
void
  UI_GTKMM::party_startplayer_changed()
  {
    this->thrower.inc_depth();

    if (this->party_settings)
      this->party_settings->update();

    this->thrower.dec_depth();
  } // void UI_GTKMM::party_startplayer_changed()

/**
 ** the party is started:
 ** the gametype and announcements of the other players are shown
 **
 ** @param     -
 **
 ** @return    -
 **/
void
  UI_GTKMM::party_start()
  { 
    this->thrower.inc_depth();

    // the order is important:
    // first the party points window is created,
    // then the menu item can send signals to it
    this->table->party_start();
    this->main_window->menu->party_start();
    this->party_settings->players->sensitivity_update();
    this->party_settings->rules->sensitivity_update();

    this->thrower.dec_depth();
  } // void UI_GTKMM::party_start()

/**
 ** a new round is started in the party
 **
 ** @param     round   number of the round
 **
 ** @return    -
 **/
void
  UI_GTKMM::party_start_round(unsigned const round)
  { 
    this->thrower.inc_depth();

    this->table->party_points_->party_start_round(round);

    this->thrower.dec_depth();
  } // void UI_GTKMM::party_start_round(unsigned const round)

/**
 ** the party is finished
 **
 ** @param     -
 **
 ** @return    -
 **/
void
  UI_GTKMM::party_finish()
  {
    this->thrower.inc_depth();

    if (!(FAST_PLAY & FAST_NS::PARTY_FINISHED)) {
      this->main_window->menu->party_finish();
      this->table->party_finish();
    }

    this->thrower.dec_depth();
  } // void UI_GTKMM::party_finish()

/**
 ** the party is closed
 **
 ** @param     -
 **
 ** @return    -
 **/
void
  UI_GTKMM::party_close()
  {
    this->thrower.inc_depth();

    this->UI::party_close();

    this->players_db->party_close();
    this->main_window->menu->party_close();
    this->table->party_close();

    this->thrower.dec_depth();
  } // void UI_GTKMM::party_close()

} // namespace UI_GTKMM_NS

#endif // #ifdef USE_UI_GTKMM
