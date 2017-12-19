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

#ifndef GTKMM_RULES_HEADER
#define GTKMM_RULES_HEADER

#include "base.h"

#include "widgets/sticky_dialog.h"
#include <gtkmm/notebook.h>
namespace Gtk {
class Label;
class StockButton;
class RadioButton;
class CheckButton;
class LabelSpinButton;
class Box;
class Notebook;
class FileChooserDialog;
class Image;
} // namespace Gtk

class Rule;

namespace UI_GTKMM_NS {

/**
 ** @brief	the rules dialog
 **
 ** @todo	use a treeview
 **/
class Rules : public Base, public Gtk::StickyDialog {
  public:
    // constructor
    Rules(Base* parent);
    // destructor
    ~Rules();

    // create a backup
    void create_backup();
    // the backup
    Rule const& backup() const;

    // reset the rules
    void reset();
    // create a menu with the rules to load from
    void load();
    // load rules
    void load_file(string filename);
    // save rules
    void save();

    // update the sensitivity
    void sensitivity_update();
    // update a rule
    void update(int type, bool update_sensitivity = true);
    // a rule has been changed
    void changed(int type);

    // update all
    void update_all();

  private:
    // initialize the window
    void init();

    // adjust the announcement times to the game with nines
    void adjust_to_with_nines();
    // adjust the announcement times to the game without nines
    void adjust_to_without_nines();

    Gtk::Box* add_group(string const& name, Gtk::Image* image = nullptr);
    void group_notebook_page_changed(Widget* page, guint pageno);

    // a key was pressed
    bool on_key_press_event(GdkEventKey* key);

    // update the announcement text (because of knocking)
    void update_announcement_text();

  public:
    vector<Gtk::CheckButton*> type_bool;
    vector<Gtk::LabelSpinButton*> type_unsigned;
    vector<Gtk::RadioButton*> counting;

  private:
    Gtk::StockButton* reset_button = nullptr;

    Gtk::StockButton* load_button = nullptr;
    Gtk::StockButton* save_button = nullptr;
    Gtk::FileChooserDialog* save_file_chooser = nullptr;

    Gtk::Notebook* group_notebook = nullptr;

  private:
    std::unique_ptr<Rule> backup_;
    // whether the info dialog changes for the rule 'with nines' <-> 'without nines' was shown
    bool without_nines_info_shown = false;

  private: // unused
    Rules() = delete;
    Rules(Rules const&) = delete;
    Rules& operator=(Rules const&) = delete;
}; // class Rules : public Base, public Gtk::StickyDialog

} // namespace UI_GTKMM_NS

#endif // #ifdef GTKMM_RULES_HEADER

#endif // #ifdef USE_UI_GTKMM
