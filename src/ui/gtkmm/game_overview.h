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

#ifndef GTKMM_GAME_OVERVIEW_HEADER
#define GTKMM_GAME_OVERVIEW_HEADER

#include "party_points.h"
#include "widgets/sticky_dialog.h"
class Player;

namespace Gtk {
class StockButton;
} // namespace Gtk

namespace UI_GTKMM_NS {

/**
 ** @brief	the game overwiew
 **/
class PartyPoints::GameOverview : public Base, public Gtk::StickyDialog {
  public:
    // constructor
    GameOverview(PartyPoints* party_points);
    // detructor
    ~GameOverview() = default;

    // the gameno
    unsigned gameno() const;
    // set the gameno
    void set_gameno(unsigned gameno);

    // a name has changed
    void name_changed(::Player const& player);

  private:
    // initialize the window
    void init();

    // a key has been pressed
    bool on_key_press_event(GdkEventKey* key);

  private:
    unsigned gameno_ = UINT_MAX;
    GameSummary* game_summary = nullptr;
    Gtk::StockButton* close_button = nullptr;

  private: // unused
    GameOverview() = delete;
    GameOverview(GameOverview const&) = delete;
    GameOverview& operator=(GameOverview const&) = delete;
}; // class GameOverview : public Base, public Gtk::StickyDialog

} // namespace UI_GTKMM_NS

#endif // #ifdef GTKMM_GAME_OVERVIEW_HEADER

#endif // #ifdef USE_UI_GTKMM
