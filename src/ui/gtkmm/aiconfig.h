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

#pragma once

#ifdef USE_UI_GTKMM

#include "base.h"
#include "player.h"
#include "../../player/aiconfig.h"

#include "widgets/label_type_selector.h"
#include <sigc++/trackable.h>
#include <gtkmm/liststore.h>
namespace Gtk {
class VBox;
class Notebook;
class StockButton;
class Label;
class CheckButton;
class LabelSpinButton;
class SpinButton;
class ComboBoxText;
class MenuItem;
class FileChooserDialog;
class TreeView;
class TextView;
class Widget;
class FileMenu;
class LabelCardSelector;
} // namespace Gtk

class Player;
class Aiconfig;
namespace UI_GTKMM_NS {

/**
 ** @brief	the ai configuration of one player
 **/
class Players::Player::AiConfig : public Base, public sigc::trackable {
  public:
    friend class Players;
    friend class Player;

    /**
     ** The Model for a heuristic
     **/
    class HeuristicsModel : public Gtk::TreeModel::ColumnRecord {
      public:
        HeuristicsModel()
        {
          this->add(this->active);
          this->add(this->heuristic);
          this->add(this->heuristic_name);
          this->add(this->gametype_group);
          this->add(this->playertype_group);
        }

        Gtk::TreeModelColumn<bool> active;
        Gtk::TreeModelColumn<int> heuristic;
        Gtk::TreeModelColumn<Glib::ustring> heuristic_name;
        Gtk::TreeModelColumn<int> gametype_group;
        Gtk::TreeModelColumn<int> playertype_group;
    }; // class HeuristicsModel : public Gtk::TreeModel::ColumnRecord

  public:
    // constructor
    AiConfig(Player* player);
    // destructor
    ~AiConfig();

    // the aiconfig
    ::Aiconfig& aiconfig();
    // the aiconfig
    ::Aiconfig const& aiconfig_const() const;

    // an aitype has been changed by the user
    void change(int type);

    // sets the ai to the hardcoded
    void hardcoded();
    // creates a menu with the players to adopt from
    void adopt();
    // adopt the aiconfig from a player
    void adopt_aiconfig(unsigned playerno);
    // make all other aiconfigs like this
    void allot();
    // creates a menu with the files in the aiconfig directories
    void load();
    // load the aiconfig from a file
    void load_file(string filename);
    // save the aiconfig
    void save();

    // update the sensitivity
    void sensitivity_update();
    // update all widgets
    void update();
    // update the name of the player
    void name_update();
    // update the description of the difficulty
    void difficulty_description_update();
    // update the translations
    void language_update();

  public:
    // initialize the aiconfig
    void init();

  private:
    // create a container with the aiconfig
    void create_container();
    // add a list for the heuristic
    void
      add_heuristic_treeview(Gtk::Notebook* notebook,
                             HeuristicsMap::GametypeGroup gametype_group,
                             HeuristicsMap::PlayerTypeGroup playertype_group,
                             Translator::Translation const& label_text);
    // add a list for the heuristic (re and contra)
    void
      add_heuristic_treeviews_re_contra(Gtk::Notebook* notebook,
                                        HeuristicsMap::GametypeGroup gametype_group);
    // add a list for the heuristic (special, re and contra)
    void
      add_heuristic_treeviews_special_re_contra(Gtk::Notebook* notebook,
                                                HeuristicsMap::GametypeGroup const
                                                gametype_group);
    // create a treeview for the heuristics
    Gtk::Widget*
      create_heuristic_treeview(HeuristicsMap::GametypeGroup gametype_group,
                                HeuristicsMap::PlayerTypeGroup playertype_group);
    // initialize a heuristics treeview
    void
      init_heuristic_treeview(Gtk::Box* const box,
                              HeuristicsMap::GametypeGroup const
                              gametype_group,
                              HeuristicsMap::PlayerTypeGroup const
                              playertype_group);


    // the difficulty has been changed
    void difficulty_changed();
    // an aitype has changed
    void aitype_changed(unsigned const trickno);
    // a rating has changed
    void rating_changed(unsigned const trickno);
    // a future limit has changed
    void future_limit_changed(unsigned const trickno);
    // a flag aitype as previous has changed
    void aitype_as_previous_changed(unsigned const trickno);

    // the sorting of a heuristics has changed
    void heuristics_row_changed_event(Gtk::TreeModel::Path const& path,
                                      Gtk::TreeModel::iterator const& row);
    // a heuristics row has been deleted
    void heuristics_row_deleted_event(Gtk::TreeModel::Path const& path,
                                      Glib::RefPtr<Gtk::ListStore> store);

    // update the toggled state of the 'default' button of 'key'
    void update_heuristics_default_button_state(HeuristicsMap::Key const& key);
    // the 'default' button has been toggled
    void heuristics_default_button_toggled_event(HeuristicsMap::Key const key);
    // update the up/down-buttons of a heuristic
    void update_heuristic_up_down_buttons(HeuristicsMap::Key const& key);
    // update the description of a heuristic
    void update_heuristic_description(HeuristicsMap::Key const& key);

    // a heuristic has been moved up
    void heuristic_move_up_event(HeuristicsMap::Key const key);
    // a heuristic has been moved down
    void heuristic_move_down_event(HeuristicsMap::Key const key);

  private:
    Player* const player = nullptr;

  protected:
    Gtk::VBox* container = nullptr;
    Gtk::Notebook* notebook = nullptr;
    vector<Gtk::Notebook*> notebooks;

    Gtk::Container* difficulty_container = nullptr;
    Gtk::Label* difficulty_label = nullptr;
    Gtk::StockButton* difficulty_button = nullptr;
    Gtk::LabelTypeSelector< ::Aiconfig::Difficulty>* difficulty_selector = nullptr;
    Gtk::StockButton* adopt_button = nullptr;
    Gtk::StockButton* allot_button = nullptr;
    Gtk::StockButton* load_button = nullptr;
    Gtk::StockButton* save_button = nullptr;
    Gtk::FileChooserDialog* save_file_chooser = nullptr;

    vector<Gtk::ComboBoxText*> aitype;
    vector<Gtk::ComboBoxText*> rating;
    vector<Gtk::SpinButton*> future_limit;
    vector<Gtk::CheckButton*> aitype_as_previous;
    vector<Gtk::CheckButton*> type_bool;
    vector<Gtk::LabelSpinButton*> type_int;
    vector<Gtk::LabelCardSelector*> type_card;


    std::map<HeuristicsMap::Key, Gtk::ToggleButton*> heuristics_default_button;
    HeuristicsModel* heuristics_model = nullptr;
    std::map<HeuristicsMap::Key, Glib::RefPtr<Gtk::ListStore> > heuristics_list;
    std::map<HeuristicsMap::Key, Gtk::TreeView*> heuristics_treeview;
    std::map<HeuristicsMap::Key, Gtk::StockButton*> heuristic_up_button;
    std::map<HeuristicsMap::Key, Gtk::StockButton*> heuristic_down_button;
    std::map<HeuristicsMap::Key, Gtk::TextView*> heuristic_description;
    bool heuristics_signals_active = false;

  private: // unused
    AiConfig() = delete;
    AiConfig(AiConfig const&) = delete;
    AiConfig& operator=(AiConfig const&) = delete;
}; // class Players::Player::AiConfig : public Base, public Gtk::Object

} // namespace UI_GTKMM_NS

#endif // #ifdef USE_UI_GTKMM
