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

#ifndef GTKMM_PARTY_POINTS_HEADER
#define GTKMM_PARTY_POINTS_HEADER

#include "base.h"
#include "widgets/sticky_dialog.h"

#include "../../basistypes.h"

class Party;
class GameSummary;

#include <gtkmm/treestore.h>
namespace Gtk {
class Notebook;
class TreeView;
class CellRenderer;
class CheckButton;
class RadioButton;
class StockButton;
} // namespace Gtk
class Player;

namespace UI_GTKMM_NS {
class GameSummary;

/**
 ** @brief	the party points window
 **
 ** Note: the first row in each round
 ** either shows the current game (expanded round)
 ** or shows the point sum of the round (collapsed)
 ** The bottom row (sum) is a second table.
 **
 ** @todo	bold number for the startplayer
 ** @todo	lines between the columns
 **/
class PartyPoints : public Base, public Gtk::StickyDialog {
  class GameOverview;
  friend class GameOverview;
  class Graph;

  class PartyPointsModel : public Gtk::TreeModel::ColumnRecord {
    public:
      PartyPointsModel(unsigned playerno);

      Gtk::TreeModelColumn<Glib::ustring> empty;
      Gtk::TreeModelColumn<unsigned> round;
      Gtk::TreeModelColumn<Glib::ustring> round_str;
      Gtk::TreeModelColumn<Glib::ustring> round_color;
      Gtk::TreeModelColumn<unsigned> gameno;
      Gtk::TreeModelColumn<Glib::ustring> gameno_str;
      vector<Gtk::TreeModelColumn<Glib::ustring> > playerpoints;
      Gtk::TreeModelColumn<int> gamepoints;
      Gtk::TreeModelColumn<Glib::ustring> gamepoints_str;
      Gtk::TreeModelColumn<int> bock_multiplier;
      Gtk::TreeModelColumn<Glib::ustring> bock_multiplier_str;
#ifdef POSTPONED
      Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf> > gametype;
#else
      Gtk::TreeModelColumn<Glib::ustring> gametype;
#endif
  }; // class PartyPointsModel : public Gtk::TreeModel::ColumnRecord

  public:
  // constructor
  PartyPoints(Base* parent);
  // destructor
  ~PartyPoints();

  // the party is opened
  void party_open();
  // a round is started
  void party_start_round(unsigned round);
  // the party is finished
  void party_finish();
  // the party is closed
  void party_close();
  // a game is started
  void game_start();
  // a game is finished
  void game_finished();

  // the name of a player has changed
  void name_changed(Player const& player);
  // update the translations
  void language_update();

  protected:
  // clear the selection
  void clear_selection();

  private:
  // initialize the window
  void init();
  public:
  // recalc all points
  void recalc_all();
  // recreate the table
  void recreate_all();
  // add a player
  void player_added(Player const& player);
  private:
  // add a game
  void add_game(unsigned gameno, bool update = true);
  // set/update the values of the game
  void set_points(unsigned gameno);
  // update the round
  void update_round(unsigned round);
  // update the sum
  void update_sum();
  // update the duty soil
  void update_duty_soli();
  // update the size of the columns
  void update_player_columns_size();

  // add future bock multipliers
  void add_future_bock_multipliers();
  // add a row for the game
  void add_row(unsigned gameno);
  // another row is selected
  void row_selection_changed();
  // the column size of the sum table is changed
  void sum_columns_size_update();
  // the game overview button is toggled
  void show_game_overview_toggled();
  // the collapse rows is changed
  void collaps_setting_changed();

  // a row is collapsed or expanded
  void row_collapsed_or_expanded(Gtk::TreeModel::iterator const& iter,
                                 Gtk::TreeModel::Path const& path);
  // set the cell color
  void set_cell_color(Gtk::CellRenderer* cell_renderer,
                      Gtk::TreeModel::iterator const& iter,
                      unsigned column);
  // the color for a game type
  static Gdk::Color color(::GameSummary const& game_summary);

  // a key has been pressed
  bool on_key_press_event(GdkEventKey* key);

  private: // elements
  public:
  Gtk::Notebook* notebook = nullptr;
  private:
  //TrickpointsModel party_points_model;
  PartyPointsModel party_points_model;
  Glib::RefPtr<Gtk::TreeStore> party_points_list;
  Gtk::TreeView* party_points_treeview = nullptr;
  vector<Gtk::TreeModel::Row> round_rows;
  vector<Gtk::TreeModel::Row> game_rows;

  Gtk::TreeView* party_points_sum_treeview = nullptr;

  Glib::RefPtr<Gtk::TreeStore> duty_soli_list;
  Gtk::TreeModel::Row duty_free_soli_row;
  Gtk::TreeModel::Row duty_color_soli_row;
  Gtk::TreeModel::Row duty_picture_soli_row;
  Gtk::TreeView* duty_soli_treeview = nullptr;

  protected:
  Gtk::CheckButton* show_game_overview_button = nullptr;
  private:
  Gtk::CheckButton* add_up_points = nullptr;

  Gtk::RadioButton* collapse_rounds = nullptr;
  Gtk::RadioButton* expand_rounds = nullptr;

  Gtk::StockButton* close_button = nullptr;

  std::unique_ptr<GameOverview> game_overview;
  Graph* graph = nullptr;

  Gtk::Label* remaining_rounds_label = nullptr;
  Gtk::Label* remaining_points_label = nullptr;

  private: // unused
  PartyPoints() = delete;
  PartyPoints(PartyPoints const&) = delete;
  PartyPoints& operator=(PartyPoints const&) = delete;
}; // class PartyPoints : public Base, public Gtk::StickyDialog

} // namespace UI_GTKMM_NS

#endif // #ifdef GTKMM_PARTY_POINTS_HEADER

#endif // #ifdef USE_UI_GTKMM
