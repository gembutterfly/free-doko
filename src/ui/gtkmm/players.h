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
 **********************************************************************/

#ifdef USE_UI_GTKMM

#ifndef GTKMM_PLAYERS_HEADER
#define GTKMM_PLAYERS_HEADER

#include "base.h"
#include "widgets/sticky_dialog.h"

#include "../../player/player.h"

class Player;
class Aiconfig;
#include <gtkmm/notebook.h>

namespace Gtk {
class StockButton;
class Table;
class ToggleButton;
class Entry;
}; // namespace Gtk
namespace UI_GTKMM_NS {

/**
 ** @brief	the window with all players
 **
 ** @todo	export into a window
 **/
class Players : public Base, public Gtk::StickyDialog {
  public:
    class Player;

  public:
    // constructor
    Players(Base* parent);
    // destructor
    ~Players();

    // the player settings
    Player& player(unsigned playerno);
    // the player settings
    Player& player(::Player const& player);

    // create a backup for all players
    void create_backup();
    // reset all players
    void reset();

    // update the sensitivity
    void sensitivity_update();
    // update all players
    void update();
    // two players have switched
    void players_switched(::Player const& player_a, ::Player const& player_b);
    // update a player
    void player_update(::Player const& player);
    // update a player name
    void name_update(::Player const& player);
    // update the voice of a player
    void voice_update(::Player const& player);
    // update the aiconfig of a player
    void aiconfig_update(::Aiconfig const& aiconfig);

#ifdef OUTDATED
    // event: a name has changed
    bool name_changed_event(GdkEventFocus*);
    // set a random name
    void random_name(unsigned playerno);
    // a team has changed
    void type_change(unsigned playerno, ::Player::Type type);
#endif

  private:
    // initialize all
    void init();

    // add a group
    Gtk::ToggleButton* add_group(string const& name,
                                 Gtk::Table* table, int& current_row,
                                 int depth,
                                 Gtk::ToggleButton* dependent_button);
    // add an aiconfig
    void add_aiconfig(int type,
                      Gtk::Table* table, int& current_row,
                      Gtk::ToggleButton* dependent_button);

    // signal: page switched
    void notebook_switch_page_signal(Widget*, guint pageno,
                                     Gtk::Notebook* notebook);

    // a key was pressed
    bool on_key_press_event(GdkEventKey* key);

  private:
    vector<std::unique_ptr<Player>> player_;
    Gtk::Notebook* players_notebook = nullptr;

  private: // unused
    Players() = delete;
    Players(Players const&) = delete;
    Players& operator=(Players const&) = delete;
}; // class Players : public Base, public Gtk::StickyDialog

} // namespace UI_GTKMM_NS

#endif // #ifdef GTKMM_PLAYERS_HEADER

#endif // #ifdef USE_UI_GTKMM
