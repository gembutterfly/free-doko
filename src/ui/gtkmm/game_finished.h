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

#ifndef GTKMM_GAME_FINISHED_HEADER
#define GTKMM_GAME_FINISHED_HEADER

#include "base.h"
#include "ui.h"
#include "widgets/sticky_dialog.h"

class Player;
namespace Gtk {
class StockButton;
class ToggleButton;
} // namespace Gtk

namespace UI_GTKMM_NS {
class GameSummary;
class GameReview;

/**
 ** @brief	the game finished window
 **
 ** @todo	when selecting a special point, jump to the trick
 ** @todo	when the party is finished, rename the 'next game' button
 **/
class GameFinished : public Base, public Gtk::StickyDialog {
  public:
    // constructor
    GameFinished(Base* parent);
    // destructor
    ~GameFinished() = default;

    // a game is finished
    void game_finished();
    // update a setting
    void setting_update(int type);

    // the name of a player has changed
    void name_changed(::Player const& player);

  private:
    // initialize the window
    void init();
    // the window is hidden
    void on_hide();
    // the game review is toggled
    void toggle_game_review();

  public:
    std::unique_ptr<GameReview> game_review;

  private:
    Gtk::StockButton* create_bug_report_button = nullptr;
  public:
    GameSummary* game_summary = nullptr;
  private:
    Gtk::StockButton* show_party_points = nullptr;
    Gtk::ToggleButton* show_game_review = nullptr;

    Gtk::StockButton* next_game_button = nullptr;

  private: // unused
    GameFinished() = delete;
    GameFinished(GameFinished const&) = delete;
    GameFinished& operator=(GameFinished const&) = delete;
}; // class GameFinished : public Base, public Gtk::StickyDialog

} // namespace UI_GTKMM_NS

#endif // #ifdef GTKMM_GAME_FINISHED_HEADER

#endif // #ifdef USE_UI_GTKMM
