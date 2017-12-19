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

#ifndef GTKMM_PREFERENCES_HEADER
#define GTKMM_PREFERENCES_HEADER

#include "base.h"
#include "widgets/sticky_dialog.h"
#include <gtkmm/notebook.h>
namespace Gtk {
class Widget;
class Notebook;
class Label;
class Alignment;
class Button;
class StockButton;
class CheckButton;
class RadioButton;
class LabelSpinButton;
class Entry;
class Image;
class FontSelectionDialog;
class ColorSelectionDialog;
class Menu;
class FileMenu;
} // namespace Gtk

#include "../../misc/setting.cardsorder.h"
class Setting;

namespace UI_GTKMM_NS {

/**
 ** @brief	the settings dialog
 **
 ** @todo	update of the cardset in 'cards order'
 ** @todo	change the style of the fonts and colors buttons
 ** @todo	alpha-channel for the colors
 ** @todo	on-the-fly changing of the fonts
 **/
class Preferences : public Base, public Gtk::StickyDialog {
  public:
    // constructor
    Preferences(Base* parent);
    // destructor
    ~Preferences();

    // create an internal backup of the settings
    void create_backup();
    // the backup
    Setting const& backup() const;

    // reset the settings
    void reset();
    // save the settings
    void save();

    // update the sensitivity
    void sensitivity_update();
    // update all elements
    void update_all();
    // update the cards order
    void update_cards_order();
    // update a specific setting
    void setting_update(int type);
    // update a setting and all depended elements
    void update(int type, bool update_sensitivity = true);
    // a setting has been changed by the user
    void changed(int type);
    // the focus has left a setting
    bool focus_out_event(GdkEventFocus*, int type);

    // show the cards order
    void show_cards_order();
    // set the cards order direction
    void cards_order_direction_set(unsigned pos);
    // reorder a card color to the left
    void cards_order_pos_to_left(unsigned pos);
    // reorder a card color to the right
    void cards_order_pos_to_right(unsigned pos);
    // set the cards order as sorted
    void cards_order_set_sorted();
    // set the card as mixed
    void cards_order_mix();

    // create the language menu
    void language_menu_create(bool popup);
    // a language has been selected
    void language_selected(string language);
    // create the background menu
    void background_menu_popup();
    // a background has been selected
    void background_selected(string background);
    // a cardset has been selected
    void cardset_selected(string cardset);
    // a cards back has been selected
    void cards_back_selected(string cards_back);
    // an iconset has been selected
    void iconset_selected(string iconset);

    // a font has been selected
    void font_accepted(Gtk::FontSelectionDialog* font_selector);
    // no font has been selected
    void font_declined(Gtk::FontSelectionDialog* font_selector);
    // a color has been selected
    void color_accepted(Gtk::ColorSelectionDialog* color_selector);
    // no color has been selected
    void color_declined(Gtk::ColorSelectionDialog* color_selector);

  private:
    // initialize all elements
    void init();
    // initialize the cards order
    void init_cardsorder();

    // add a group box
    Gtk::VBox* add_group_vbox(string const& group_name);
    // add a group notebook
    Gtk::Notebook* add_group_notebook(string const& group_name);
    // add a group
    void add_group(string const& group_name, Gtk::Widget& widget);
    // add a subgroup
    Gtk::VBox* add_subgroup_vbox(Gtk::Notebook& subgroup_notebook,
                                 string const& subgroup_name);

#ifndef RELEASE
    // update the translation
    void update_translation();
#endif

    // a key has been pressed
    bool on_key_press_event(GdkEventKey* key);
    // a notebook page has been changed
    void group_notebook_page_changed(Widget* page, guint pageno);

  public:
    Gtk::StockButton* reset_button = nullptr;
    Gtk::StockButton* close_button = nullptr;

    Gtk::Notebook* group_notebook = nullptr;

    vector<Gtk::CheckButton*> type_bool;
    vector<Gtk::LabelSpinButton*> type_unsigned;
    vector<Gtk::Widget*> type_string;
    vector<Gtk::Label*> type_string_label;

    vector<Gtk::Image*> sorting_tcolor_image;
    vector<Gtk::RadioButton*> sorting_up_button;
    vector<Gtk::RadioButton*> sorting_down_button;
    Gtk::RadioButton* sorted_button = nullptr;
    Gtk::RadioButton* unsorted_button = nullptr;
    Gtk::Button* mix_cards_button = nullptr;

    Gtk::Menu* language_menu = nullptr;
    Gtk::FileMenu* background_menu = nullptr;
    Gtk::FileMenu* cardset_menu = nullptr;
    Gtk::FileMenu* cards_back_menu = nullptr;
    Gtk::FileMenu* iconset_menu = nullptr;
    vector<Gtk::FontSelectionDialog*> font_selector;
    vector<Gtk::ColorSelectionDialog*> color_selector;

#ifndef RELEASE
    Gtk::Button* update_translation_button = nullptr;
#endif

  private:
    Setting* backup_ = nullptr;

  private: // unused
    Preferences() = delete;
    Preferences(Preferences const&) = delete;
    Preferences& operator=(Preferences const&) = delete;
}; // class Preferences : public Base, public Gtk::StickyDialog

} // namespace UI_GTKMM_NS

#endif // #ifdef GTKMM_PREFERENCES_HEADER

#endif // #ifdef USE_UI_GTKMM
