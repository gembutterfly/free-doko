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

#ifndef GTKMM_GAMEPLAY_ACTIONS_HEADER
#define GTKMM_GAMEPLAY_ACTIONS_HEADER

#include "base.h"

#include "../../game/gameplay_action.h"
#include "../../player/player.h"

#include <gtkmm/liststore.h>
#include <gtkmm/treeview.h>

namespace UI_GTKMM_NS {

/**
 ** A list store containing the gameplay actions from a vector
 **/
class GameplayActions : public Base, public Gtk::TreeView {
  public:
    // the colors for the action type
    static pair<string, string>
      colors(::GameplayAction::Type action_type,
             ::GameplayAction::Discrepancy discrepancy);

  public:
    /**
     ** @brief	the columns of game actions
     **/
    class GameplayActionModel : public Gtk::TreeModel::ColumnRecord {
      public:
        GameplayActionModel()
        {
          this->add(this->type);
          this->add(this->player);
          this->add(this->data);
          this->add(this->action);
          this->add(this->no);
        }

        Gtk::TreeModelColumn<Glib::ustring> type;
        Gtk::TreeModelColumn<Glib::ustring> player;
        Gtk::TreeModelColumn<Glib::ustring> data;
        Gtk::TreeModelColumn<GameplayAction const*> action;
        Gtk::TreeModelColumn<unsigned> no;
    }; // class GameplayActionModel : public Gtk::TreeModel::ColumnRecord

  public:
    // constructor
    GameplayActions(Base* parent);
    // destructor
    ~GameplayActions();

    // update the gameplay actions
    void update();

    // set the actions
    void set_actions(vector< ::GameplayAction const*> const& actions);
    // show the discrepancies
    void set_discrepancies(vector< ::GameplayAction::Discrepancy> const&
                           discrepancies);
    // remove all actions
    void remove_actions();
    // set the current action
    void set_current_action_no(unsigned current_action_no);

    // the name of the player has changed
    void name_changed(Player const& player);
    // update all translations
    void language_update();

    // set the cell color
    void set_cell_color(Gtk::CellRenderer* cell_renderer,
                        Gtk::TreeModel::iterator const& row);
  private:
    // initialize the list
    void init();

  private:
    // the actions
    vector<::GameplayAction const*> const* actions = nullptr;
    // the discrepancy
    vector<::GameplayAction::Discrepancy> const* discrepancies = nullptr;
    // the number of the current action
    unsigned current_action_no = 0;

  public:
    // the model
    GameplayActionModel model;
  private:
    // the list
    Glib::RefPtr<Gtk::ListStore> list;
}; // class GameplayActions

} // namespace UI_GTKMM_NS

#endif // #ifndef GTKMM_GAMEPLAY_ACTIONS_HEADER

#endif // #ifdef USE_UI_GTKMM
