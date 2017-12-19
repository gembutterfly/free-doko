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

#ifndef GTKMM_MENU_HEADER
#define GTKMM_MENU_HEADER

#include "base.h"

#include "../../basistypes.h"

#include "../../misc/setting.theme.h"

#include <gtkmm/menubar.h>
#include <sigc++/connection.h>
namespace Gtk {
class Menu;
class MenuItem;
class CheckMenuItem;
class ImageMenuItem;
class Label;
class FileChooserDialog;
} // namespace Gtk

namespace UI_GTKMM_NS {
class MainWindow;
class Preferences;

/**
 **
 ** @brief	the main window
 **
 ** party
 **   +- new party		C-n
 **   +- load party		C-l
 **   +- save party		C-s
 **   +- end party
 **   `- quit			C-q
 ** information
 **   +- card suggestion	h
 **   +- last trick		l
 **   +- rules		r
 **   +- players
 **   `- party points		p
 ** announcement
 **   `-  ...			n	m
 ** preferences
 **   +- show all hands
 **   +- show valid cards
 **   +- cards order
 **   `- preferences		F2
 ** network
 **   +- chatter		C
 **   +- log
 **   `- settings		F3
 ** bug report
 ** help
 **   +- help index		F1
 **   +- license
 **   +- cardset license
 **   +- changelog
 **   `- about
 ** 
 ** Note: the translation of the announcements are managed internally
 ** 
 **
 ** @author	Diether Knof
 **
 ** @todo	replace 'party_points_*_connection' (see sigc-doc)
 ** @todo	update the player names in the announcement
 **
 **/
class Menu : public Base, public Gtk::MenuBar {
  friend class Preferences; // for changing the preferences menu items
  friend class UI_GTKMM; // for shortcuts

  public:
  // constructor
  Menu(MainWindow* main_window);
  // destructor
  ~Menu();

  // set all signal
  void set_signals();
  // sets the setting 'show all hands'
  void show_all_hands_set();
  // sets the setting 'emphasize vaild cards'
  void emphasize_valid_cards_set();

  // a party is opened
  void party_open();
  // a party is started
  void party_start();
  // a game is started
  void game_start();
  // a trick is opened
  void trick_open();
  // a card is requested
  void card_get();
  // a card has been played
  void card_got();
  // the game is finished
  void game_finish();
  // the game is closed
  void game_close();
  // the party is finished
  void party_finish();
  // the party is closed
  void party_close();

  // update whether the announcements are valid
  void announcements_update();
  // update the translation
  void language_update();
  private:
  // initialize
  void init();

  public:
  // an announcement shall be made
  void announcement_event(unsigned playerno, int announcement);
  // a swines announcement shall be made
  void swines_announcement_event(unsigned playerno);
  // a hyperswines announcement shall be made
  void hyperswines_announcement_event(unsigned playerno);

  private:
  // the party shall be saved
  void save_party_event();
  // a party shall be loaded
  void load_party_event();

  // the theme list shall be updated
  void update_themes_sensitivity();
  // a theme as selected
  void apply_theme_event(Setting::Theme const* theme);

  private:
  MainWindow* main_window = nullptr;

  Gtk::Menu* party_menu = nullptr;
  Gtk::MenuItem* party = nullptr;
  Gtk::ImageMenuItem* new_party = nullptr;
  Gtk::ImageMenuItem* load_party = nullptr;
  Gtk::ImageMenuItem* save_party = nullptr;
  Gtk::ImageMenuItem* end_party = nullptr;
  Gtk::ImageMenuItem* quit = nullptr;

  Gtk::Menu* information_menu = nullptr;
  Gtk::MenuItem* information = nullptr;
  Gtk::ImageMenuItem* card_suggestion = nullptr;
  Gtk::ImageMenuItem* last_trick = nullptr;
  Gtk::ImageMenuItem* game_debug = nullptr;
  Gtk::ImageMenuItem* bug_report_replay = nullptr;
  Gtk::ImageMenuItem* rules = nullptr;
  Gtk::ImageMenuItem* party_points = nullptr;
  Gtk::ImageMenuItem* players = nullptr;
  Gtk::ImageMenuItem* players_db = nullptr;

  Gtk::MenuItem* announcements = nullptr;
  vector<Gtk::MenuItem*> announcements_player;
  vector<Gtk::Menu*> announcements_player_menu;
  vector<vector<Gtk::MenuItem*> > announcements_announcement;
  vector<Gtk::MenuItem*> swines_separator;
  vector<Gtk::MenuItem*> swines_announcement;
  vector<Gtk::MenuItem*> hyperswines_announcement;

  Gtk::Menu* preferences_menu = nullptr;
  Gtk::MenuItem* preferences = nullptr;
  protected:
  Gtk::CheckMenuItem* emphasize_valid_cards = nullptr;
  Gtk::CheckMenuItem* show_all_hands = nullptr;
  private:
  Gtk::ImageMenuItem* cards_order = nullptr;
  Gtk::Menu* themes_menu = nullptr;
  Gtk::MenuItem* themes_item = nullptr;
  vector<Gtk::ImageMenuItem*> themes;
  Gtk::ImageMenuItem* preferences_preferences = nullptr;

#ifdef USE_NETWORK
  Gtk::Menu* network_menu = nullptr;
  Gtk::MenuItem* network = nullptr;
  Gtk::ImageMenuItem* chatter = nullptr;
  Gtk::ImageMenuItem* network_log = nullptr;
  Gtk::ImageMenuItem* network_settings = nullptr;
#endif

  Gtk::ImageMenuItem* bug_report = nullptr;

  Gtk::Menu* help_menu = nullptr;
  Gtk::MenuItem* help = nullptr;
  Gtk::ImageMenuItem* help_index = nullptr;
  Gtk::ImageMenuItem* license = nullptr;
  Gtk::ImageMenuItem* changelog = nullptr;
  Gtk::ImageMenuItem* support = nullptr;
  Gtk::ImageMenuItem* about = nullptr;
  Gtk::ImageMenuItem* generate_error = nullptr;

  sigc::connection party_points_present_connection;

  Gtk::FileChooserDialog* load_party_file_chooser = nullptr;
  Gtk::FileChooserDialog* save_party_file_chooser = nullptr;

  private: // unused
  Menu() = delete;
  Menu(Menu const&) = delete;
  Menu& operator=(Menu const&) = delete;
}; // class Menu : public Base, public Gtk::MenuBar

} // namespace UI_GTKMM_NS

#endif // #ifdef GTKMM_MENU_HEADER

#endif // #ifdef USE_UI_GTKMM
