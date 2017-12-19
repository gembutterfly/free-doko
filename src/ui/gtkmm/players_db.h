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

#ifndef GTKMM_PLAYERS_DB_HEADER
#define GTKMM_PLAYERS_DB_HEADER

#include "base.h"
#include "widgets/sticky_dialog.h"

class Party;
class Player;
class PlayersDb;
class TLWcount;

#include <gtkmm/treemodel.h>
#include <gtkmm/treestore.h>
namespace Gtk {
class TreeView;
class StockButton;
};
namespace UI_GTKMM_NS {
/**
 ** @brief	window with the player db
 **
 ** @todo	only update the heuristics at the end of a game,
 **		so that the human player can get no additional information
 **		(p.e. with 'only valid card')
 **
 **/
class PlayersDB : public Base, public Gtk::StickyDialog {
  enum Statistic {
    STATISTIC_FIRST,
    TOTAL = STATISTIC_FIRST,
    WON,
    LOST,
    PERCENT_WON,
    STATISTIC_LAST = PERCENT_WON
  }; // enum Statistic
  // the name of the statisic
  static string name(Statistic statistic);

  // model for the data
  class PlayersDBModel : public Gtk::TreeModel::ColumnRecord {
    public:
      // constructor
      PlayersDBModel(unsigned playerno);

      Gtk::TreeModelColumn<Glib::ustring> type;
      vector<Gtk::TreeModelColumn<Glib::ustring> > statistic;
  }; // class PlayersDBModel : public Gtk::TreeModel::ColumnRecord

  public:
  // constructor
  PlayersDB(Base* parent);
  // destructor
  ~PlayersDB() = default;

  // the party is opened
  void party_open();
  // the party is closed
  void party_close();
  // the game is finished
  void game_finished();

  // update the database
  void update_db();
  // the name has changed
  void name_changed(Player const& player);
  // update the translation
  void language_update();

  private:
  // initialize the window
  void init();
  // recreate the database
  void recreate_db();
  // set the statistic presentation
  void set_statistic(Statistic statistic);

  // clear the database
  void clear_db();

  // returns a string representing the data
  Glib::ustring statistic_data(TLWcount const& tlwcount) const;
  // returns a string representing the data
  Glib::ustring heuristic_statistic_data(unsigned total,
                                         unsigned num) const;

  // a key has been pressed
  bool on_key_press_event(GdkEventKey* key);

  private:

  Statistic statistic = TOTAL;

  std::unique_ptr<PlayersDBModel> players_db_model;
  Glib::RefPtr<Gtk::TreeStore> players_db_list;
  Gtk::TreeView* players_db_treeview = nullptr;

  Gtk::TreeModel::Row ranking;
  Gtk::TreeModel::Row average_game_points;
  Gtk::TreeModel::Row average_game_trick_points;
  Gtk::TreeModel::Row games;
  Gtk::TreeModel::Row games_marriage;
  Gtk::TreeModel::Row games_poverty;
  Gtk::TreeModel::Row games_solo;
  Gtk::TreeModel::Row games_solo_color;
  Gtk::TreeModel::Row games_solo_picture;
  Gtk::TreeModel::Row games_solo_picture_single;
  Gtk::TreeModel::Row games_solo_picture_double;
  vector<Gtk::TreeModel::Row> game;
  Gtk::TreeModel::Row specialpoints;
  Gtk::TreeModel::Row specialpoints_winning;
  Gtk::TreeModel::Row specialpoints_announcement;
  vector<Gtk::TreeModel::Row> specialpoint;
  Gtk::TreeModel::Row heuristics;
  Gtk::TreeModel::Row heuristics_general;
  Gtk::TreeModel::Row heuristics_poverty;
  Gtk::TreeModel::Row heuristics_solo;
  Gtk::TreeModel::Row heuristics_solo_color;
  Gtk::TreeModel::Row heuristics_solo_picture;
  Gtk::TreeModel::Row heuristics_solo_meatless;
  vector<Gtk::TreeModel::Row> heuristic;

  private: // unused
  PlayersDB() = delete;
  PlayersDB(PlayersDB const&) = delete;
  PlayersDB& operator=(PlayersDB const&) = delete;
}; // class PlayersDB : public Base, public Gtk::StickyDialog

} // namespace UI_GTKMM_NS

#endif // #ifdef GTKMM_PLAYERS_DB_HEADER

#endif // #ifdef USE_UI_GTKMM
