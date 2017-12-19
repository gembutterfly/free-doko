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

#ifndef GTKMM_GAME_DEBUG_HEADER
#define GTKMM_GAME_DEBUG_HEADER

#include "base.h"
#include "widgets/sticky_dialog.h"
#include "../../os/os.h"

#include "../../party/party.h"
#include "../../party/rule.h"

#include <stack>
using std::stack;

#include <gtkmm/treemodel.h>
namespace Gtk {
class Label;
class TreeView;
class ListStore;
class TreeStore;
} // namespace Gtk

class Trick;

namespace UI_GTKMM_NS {

/**
 ** @brief	the summary of a game
 **
 ** Note, this class gets the information directly over the OS,
 ** the functions are not called by UI_GTKMM.
 **/
class GameDebug : public Base, public Gtk::StickyDialog, public OS {

  /**
   ** @brief	the columns of the player info table
   **/
  struct PlayerInfoModel : public Gtk::TreeModel::ColumnRecord {
    PlayerInfoModel();

    Gtk::TreeModelColumn<Glib::ustring> name;
    Gtk::TreeModelColumn<Glib::ustring> team;
    Gtk::TreeModelColumn<Glib::ustring> swines;
    Gtk::TreeModelColumn<Glib::ustring> announcement;
    Gtk::TreeModelColumn<unsigned> points;
    Gtk::TreeModelColumn<unsigned> tricks;
  }; // struct PlayerInfoModel : public Gtk::TreeModel::ColumnRecord

  /**
   ** @brief	the columns of the team info table
   **/
  struct TeamInfoModel : public Gtk::TreeModel::ColumnRecord {
    TeamInfoModel();

    Gtk::TreeModelColumn<Glib::ustring> name;
    vector<Gtk::TreeModelColumn<Glib::ustring> > known_team;
  }; // struct TeamInfoModel : public Gtk::TreeModel::ColumnRecord

  /**
   ** @brief	the columns of the tricks table
   **/
  struct TricksModel : public Gtk::TreeModel::ColumnRecord {
    TricksModel();

    Gtk::TreeModelColumn<Glib::ustring> trickno;
    Gtk::TreeModelColumn<Glib::ustring> playerno;
    Gtk::TreeModelColumn<Glib::ustring> card;
    Gtk::TreeModelColumn<Glib::ustring> heuristic;
    Gtk::TreeModelColumn<bool> isvirtual;
  }; // struct TricksModel : public Gtk::TreeModel::ColumnRecord

  public:
  // constructor
  GameDebug(Base* parent);
  // destructor
  ~GameDebug();

  // the game is opened
  void game_open(Game const& game);
  // the cards is started
  void game_start();

  // the trick is opened
  void trick_open(::Trick const& trick);
  // the card is played
  void card_played(HandCard const& card);

  // the ai tests a card
  void ai_test_card(Card const& card, unsigned playerno);
  // the ai has weighted teh card
  void ai_card_weighting(int weighting);
  // the ai has played a card in a virtual game
  void virtual_card_played(HandCard const& card);

  // an announcement is made
  void announcement_made(Announcement const& announcement,
                         Player const& player);
  // the player has swines
  void swines_announced(Player const& player);
  // the player has hyperswines
  void hyperswines_announced(Player const& player);

  // the marriage partner has found a bride
  void marriage(Player const& bridegroom, Player const& bride);
  // a player has announced a genscher
  void genscher(Player const& genscher, Player const& partner);

  private:
  // initialize the window
  void init();

  // update the information
  void update_info();

  public:
  // the name of the player has changed
  void name_changed(Player const& player);

  private:
  // update the language
  void language_update();

  // event: a key has been pressed
  bool on_key_press_event(GdkEventKey* key);

  private:
  Gtk::Label* seed = nullptr;
  Gtk::Label* gametype = nullptr;
  Gtk::Label* soloplayer = nullptr;

  PlayerInfoModel player_info_model;
  Glib::RefPtr<Gtk::ListStore> player_info_list;
  Gtk::TreeView* player_info_treeview = nullptr;
  vector<Gtk::TreeModel::Row> player_info_rows;

  TeamInfoModel team_info_model;
  Glib::RefPtr<Gtk::ListStore> team_info_list;
  Gtk::TreeView* team_info_treeview = nullptr;
  vector<Gtk::TreeModel::Row> team_info_rows;

  TricksModel tricks_model;
  Glib::RefPtr<Gtk::TreeStore> tricks_store;
  Gtk::TreeView* tricks_treeview = nullptr;
  vector<Gtk::TreeModel::Row> tricks_rows;
  stack<Gtk::TreeModel::Row> ai_test_rows;

  private: // unused
  GameDebug() = delete;
  GameDebug(GameDebug const&) = delete;
  GameDebug& operator=(GameDebug const&) = delete;
}; // class GameDebug : public Base, public Gtk::StickyDialog, public OS

} // namespace UI_GTKMM_NS

#endif // #ifdef GTKMM_GAME_DEBUG_HEADER

#endif // #ifdef USE_UI_GTKMM
