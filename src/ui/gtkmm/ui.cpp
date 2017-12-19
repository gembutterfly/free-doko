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
 *********************************************************************/

#include "constants.h"

#ifdef USE_UI_GTKMM

#include "ui.h"
#include "translations.h"
#include "cards.h"
#include "icons.h"
#include "first_run.h"
#include "program_updated.h"
#include "splash_screen.h"
#include "main_window.h"
#include "table.h"
#include "game_finished.h"
#include "game_review.h"
#include "party_points.h"
#include "menu.h"
#include "party_summary.h"
#include "party_settings.h"
#include "players_db.h"
#include "game_debug.h"
#include "chatter.h"
#include "help.h"
#include "license.h"
#include "changelog.h"
#include "support.h"
#include "about.h"
#include "preferences.h"
#ifdef USE_NETWORK
#include "network.h"
#include "network_log.h"
#ifdef USE_NETWORK_DOKOLOUNGE
#include "dokolounge/lounge.h"
#endif
#endif
#include "bug_report.h"
#include "bug_report_replay.h"
#include "error.h"


#include "../help.h"
#include "../status_message.h"
#include "../../basistypes.h"
#include "../../party/party.h"
#include "../../game/game.h"
#include "../../card/trick.h"
#include "../../player/human/human.h"
#include "../../misc/setting.h"
#ifndef RELEASE
#include "../../game/gameplay.h"
#include "../../player/ai/cards_information.h"
#include "../../player/ai/team_information.h"
#endif

#include "widgets/stock_button.h"

#include <gtkmm/main.h>
#include <gtkmm/messagedialog.h>
#include <gtkmm/stock.h>
#include <gtkmm/label.h>
#include <gtkmm/button.h>
#include <gtkmm/textview.h>
#include <gtkmm/imagemenuitem.h>
#include <gtkmm/checkmenuitem.h>
#include <gdk/gdkkeysyms.h>

namespace UI_GTKMM_NS {

bool UI_GTKMM::sleeping = false;

/**
 ** constructor
 **
 ** @param     -
 **
 ** @return    -
 **/
UI_GTKMM::UI_GTKMM() :
  UI(UI_TYPE::GTKMM_DOKO),
  Base(this, this)
{
#if defined(DKNOF) && !defined(RELEASE)
  {
    int argc = 2;
    char** argv = new char*[2];
    argv[0] = new char[10];
    strcpy(argv[0], "FreeDoko");
    argv[1] = new char[20];
    strcpy(argv[1], "--g-fatal-warnings");
    this->kit = make_unique<Gtk::Main>(argc, argv);
    delete argv[1];
    delete argv[0];
    delete argv;
  }
#else
  this->kit = make_unique<Gtk::Main>();
#endif

  // count the number of objects
  this->base_objects_number = 30 + 1;
  this->base_objects_number += 5 * 4; // HTIN objects
#ifdef RELEASE
  this->base_objects_number += 2; // ?
#endif

#ifndef RELEASE
  this->base_objects_number += 1; // gameinfo
#endif
#ifndef RELEASE
  this->base_objects_number -= 2; // ?
#endif
#ifdef USE_NETWORK
  this->base_objects_number += 2; // network, log
#ifdef USE_NETWORK_DOKOLOUNGE
  this->base_objects_number += 8; // lounge, 2 * chat
#endif
#endif

  this->translations = make_unique<Translations>(this);

#ifdef GLIBMM_EXCEPTIONS_ENABLED
  { // the logo
    try {
      this->logo = Gdk::Pixbuf::create_from_file(::setting.path(Setting::LOGO_FILE));
    } catch (Glib::FileError file_error) {
      ::ui->error(::translation("Error::loading logo (%sfile%)",
                                (  ::setting.path(Setting::LOGO_FILE).empty()
                                 ? ::setting.value(Setting::LOGO_FILE)
                                 : ::setting.path(Setting::LOGO_FILE))
                               ).translation()
                  + "\n"
                  + ::translation("searched paths:").translation() + "\n"
                  + ::setting.data_directories_string("  "));
    } catch (Gdk::PixbufError pixbuf_error) {
      ::ui->error(::translation("Error::loading logo (%sfile%)",
                                (  ::setting.path(Setting::LOGO_FILE).empty()
                                 ? ::setting.value(Setting::LOGO_FILE)
                                 : ::setting.path(Setting::LOGO_FILE))
                               ).translation()
                  + "\n"
                  + ::translation("searched paths:").translation() + "\n"
                  + ::setting.data_directories_string("  "));
    } // try
  } // the logo
  { // the icon
    try {
      this->icon = Gdk::Pixbuf::create_from_file(::setting.path(Setting::ICON_FILE));
    } catch (Glib::FileError file_error) {
      ::ui->error(::translation("Error::loading icon (%sfile%)",
                                (  ::setting.path(Setting::ICON_FILE).empty()
                                 ? ::setting.value(Setting::ICON_FILE)
                                 : ::setting.path(Setting::ICON_FILE))
                               ).translation()
                  + "\n"
                  + ::translation("searched paths:").translation() + "\n"
                  + ::setting.data_directories_string("  "));
    } catch (Gdk::PixbufError pixbuf_error) {
      ::ui->error(::translation("Error::loading icon (%sfile%)",
                                (  ::setting.path(Setting::ICON_FILE).empty()
                                 ? ::setting.value(Setting::ICON_FILE)
                                 : ::setting.path(Setting::ICON_FILE))
                               ).translation()
                  + "\n"
                  + ::translation("searched paths:").translation() + "\n"
                  + ::setting.data_directories_string("  "));
    } // try
  } // the icon
#else
  std::auto_ptr<Glib::Error> error;
  this->logo = Gdk::Pixbuf::create_from_file(::setting.path(Setting::LOGO_FILE), error);
  if (error.get())
    ::ui->error(::translation("Error::loading logo (%sfile%)",
                              ::setting.path(Setting::LOGO_FILE)).translation()
                + "\n"
                + ::translation("searched paths:").translation() + "\n"
                + ::setting.data_directories_string("  "));
  this->icon = Gdk::Pixbuf::create_from_file(::setting.path(Setting::ICON_FILE), error);
  if (error.get())
    ::ui->error(::translation("Error::loading icon (%sfile%)",
                              ::setting.path(Setting::ICON_FILE)).translation()
                + "\n"
                + ::translation("searched paths:").translation() + "\n"
                + ::setting.data_directories_string("  "));
#endif
} // UI_GTKMM::UI_GTKMM()

/**
 ** destruktor
 **/
UI_GTKMM::~UI_GTKMM()
{
  if (this->kit && this->kit->level())
    Gtk::Main::quit();
} // UI_GTKMM::~UI_GTKMM()


/**
 ** Initialises the UI with the command line arguments.
 ** GTKMM removes the arguments it understands from the list.
 **
 ** @param     argc   number of arguments
 ** @param     argv   arguments
 **
 ** @return    -
 **/
void
UI_GTKMM::init(int& argc, char**& argv)
{
  this->thrower.inc_depth();

  this->set_max_progress(3);

  // I have always to construct the window for 'Icons'
  // (I need one window and now I take the root window from 'splash_screen')
  Gtk::Window::set_auto_startup_notification(false);
  this->help = make_unique<Help>(this);
  this->splash_screen = make_unique<SplashScreen>(this);
  if (::setting(Setting::SHOW_SPLASH_SCREEN)) {
    this->splash_screen->show();

    if (this->ui->first_run_window)
      this->ui->first_run_window->raise();
    if (this->ui->program_updated_window)
      this->ui->program_updated_window->raise();
  } // if (::setting(Setting::SHOW_SPLASH_SCREEN))

  StatusMessages sm(*this);

  sm.add(::translation("~loading the cards"));
  this->cards = make_unique<Cards>(this);
  sm.add(::translation("~loading the icons"));
  this->icons = make_unique<Icons>(this);
  sm.add(::translation("~initiating the GUI"));

  // there are some orders on initializing:
  //   bug_report < party_settings
  //   bug_report < bug_report_replay
  //   bug_report < main_window
  //   main_window < party_summary
  //   main_window < party_settings
  //   main_window < table
  //   main_window < network
  //   main_window < network_log
  //   network < party_settings
  sm.add(::translation("~initiating the GUI (license)"));
  this->license = make_unique<License>(this);
  sm.add(::translation("~initiating the GUI (changelog)"));
  this->changelog = make_unique<ChangeLog>(this);
  sm.add(::translation("~initiating the GUI (support)"));
  this->support = make_unique<Support>(this);
  sm.add(::translation("~initiating the GUI (about)"));
  this->about = make_unique<About>(this);

  sm.add(::translation("~initiating the GUI (preferences)"));
  this->preferences = make_unique<Preferences>(this);

  sm.add(::translation("~initiating the GUI (bug report)"));
  this->bug_report = make_unique<BugReport>(this);
  sm.add(::translation("~initiating the GUI (bug report replay)"));
  this->bug_report_replay = make_unique<BugReportReplay>(this);

  sm.add(::translation("~initiating the GUI (main window)"));
  this->main_window = make_unique<MainWindow>(this);
  sm.add(::translation("~initiating the GUI (table)"));
  this->table = make_unique<Table>(this);

#ifdef USE_NETWORK
  sm.add(::translation("~initiating the GUI (network)"));
  this->network = make_unique<Network>(this);
  sm.add(::translation("~initiating the GUI (network log)"));
  this->network_log = make_unique<NetworkLog>(this);
#ifdef USE_NETWORK_DOKOLOUNGE
  sm.add(::translation("~initiating the GUI (dokolounge)"));
  this->lounge = make_unique<DokoLounge::Lounge>(this);
#endif
#endif
  sm.add(::translation("~initiating the GUI (party summary)"));
  this->party_summary = make_unique<PartySummary>(this);
  sm.add(::translation("~initiating the GUI (party settings)"));
  this->party_settings = make_unique<PartySettings>(this);

  sm.add(::translation("~initiating the GUI (players db)"));
  this->players_db = make_unique<PlayersDB>(this);

  sm.add(::translation("~initiating the GUI (game debug)"));
  this->game_debug = make_unique<GameDebug>(this);

  sm.add(::translation("~initiating the GUI (chatter)"));
  this->chatter = make_unique<Chatter>(this);

  sm.add(::translation("~initiating the GUI (show all)"));
  this->main_window->menu->set_signals();

  this->language_update();

  { // set the size of the main window
    int const width_hint = static_cast<int>(7 * this->cards->height());
    int const height_hint = static_cast<int>(6 * this->cards->height());
    int const width = min(width_hint,
                          Gdk::Screen::get_default()->get_width() - 2 EM);
    int const height = min(height_hint,
                           Gdk::Screen::get_default()->get_height() - 2 EX);
    this->main_window->set_default_size(width, height);
    //this->main_window->set_default_size(width_hint - 1 EM, height_hint - 1 EX);
  } // set the size of the main window

  Gtk::Window::set_auto_startup_notification(true);

  this->main_window->show();
  this->splash_screen->hide();

  this->finish_progress();

  this->splash_screen.release();

#ifdef WORKAROUND
#ifdef WINDOWS
  // Under MS-Windows there are problems with special characters (like 'ü')
  // When the language is changed, the characters are shown as they should,
  // I wonder, why.
  {
    auto const language = ::setting(Setting::LANGUAGE);
    ::setting.set(Setting::LANGUAGE, "en");
    ::setting.set(Setting::LANGUAGE, language);
  }
#endif
#endif

  this->thrower.dec_depth();
} // UI_GTKMM::init(argc, argv)

/**
 ** -> result
 **
 ** @param     text   text to analys
 **
 ** @return    whether the text is an utf8 encoded text
 **/
bool
  UI_GTKMM::is_utf8(string const& text)
  {
    // see http://en.wikipedia.org/wiki/UTF-8
    for (size_t i = 0; i < text.size(); ++i) {
      char const& c = text[i];
      if ((c & 0x80) == 0) {
        // OK, ASCII character
      } else if ((c & 0xE0) == 0xC0 ) {
        if (i + 1 >= text.size())
          return false;
        if ((text[i + 1] & 0xC0) != 0x80)
          return false;
        i += 1;
      } else if ((c & 0xF0) == 0xE0 ) {
        if (i + 2 >= text.size())
          return false;
        if (   ((text[i + 1] & 0xC0) != 0x80)
            || ((text[i + 2] & 0xC0) != 0x80) )
          return false;
        i += 2;
      } else if ((c & 0xF8) == 0xF0 ) {
        if (i + 3 >= text.size())
          return false;
        if (   ((text[i + 1] & 0xC0) != 0x80)
            || ((text[i + 2] & 0xC0) != 0x80)
            || ((text[i + 3] & 0xC0) != 0x80) )
          return false;
        i += 3;
      } else {
        return false;
      } // if (c)
    } // for (i)

    return true;
  } // static bool UI_GTKMM::is_utf8(string text)

/**
 ** -> result
 **
 ** @param     text   text to convert
 **
 ** @return    utf8 format of 'text'
 **/
Glib::ustring
  UI_GTKMM::to_utf8(string const& text)
  {
    // I do not know any function to identify the encoding of a string.
    // So we decide according to the german umlauts latin1-characters.

    // searches for german umlauts
    if (UI_GTKMM::is_utf8(text))
      return text;

#ifdef GLIBMM_EXCEPTIONS_ENABLED
    try {
      return Glib::locale_to_utf8(text);
    } catch (Glib::ConvertError const error) {
      try {
        return Glib::convert(text, "utf8", "latin1");
      } catch (Glib::ConvertError const error) {
      } // try
    } // try
#else
    std::auto_ptr<Glib::Error> error;
    auto ret = Glib::locale_to_utf8(text, error);
    if (!error.get())
      return ret;
    ret = Glib::convert(text, "utf8", "latin1", error);
    if (!error.get())
      return ret;
#endif

    return text;
  } // static Glib::ustring UI_GTKMM::to_utf8(string text)


/**
 ** add a part to the list
 **
 ** @param     base   part to add
 **
 ** @return    -
 **/
void
  UI_GTKMM::add_part(Base& base)
  {
    this->parts.push_back(&base);
  } // void UI_GTKMM::add_part(Base& base)

/**
 ** add a window to the list
 **
 ** @param     window   window to add
 **
 ** @return    -
 **/
void
  UI_GTKMM::add_window(Gtk::Window& window)
  {
    this->windows.push_back(&window);
  } // void UI_GTKMM::add_window(Gtk::Window& window)

/**
 ** updates the UI
 **
 ** @param     -
 **
 ** @return    -
 **/
void
  UI_GTKMM::update()
  {
    this->thrower.inc_depth();

#ifdef CHECK_RUNTIME
    auto const ssp = ::runtime["ui update"].start_stop_proxy();
#endif
    while(this->kit->events_pending())
      this->kit->iteration();

    this->thrower.dec_depth();
  } // void UI_GTKMM::update()

/**
 ** sleeps the given time
 ** (in miliseconds, UINT_MAX for infinity)
 **
 ** @param     sleep_msec   time to sleep
 **
 ** @return    -
 **
 ** @todo	remove variable 'sleeping'
 **/
void
  UI_GTKMM::sleep(unsigned const sleep_msec)
  {
    this->thrower.inc_depth();

    auto timeout_connection
      = Glib::signal_timeout().connect(&UI_GTKMM::stop_sleeping,
                                       //sigc::mem_fun(*(this->kit),
                                       //&Gtk::Main::quit),
                                       sleep_msec);

    UI_GTKMM::sleeping = true;

    while (!this->thrower
           && UI_GTKMM::sleeping)
      Gtk::Main::iteration();

    timeout_connection.disconnect();

    this->thrower.dec_depth();
  } // void UI_GTKMM::sleep(unsigned sleep_msec)

/**
 ** quits the innermost main loop of gtk
 **
 ** @param     -
 **
 ** @return    -
 **/
void
  UI_GTKMM::main_quit()
  {
    if (this->kit->level() > 0)
      this->kit->quit();
  } // void UI_GTKMM::main_quit()

/**
 ** quits the innermost main loop of gtk
 **
 ** @param     -
 **
 ** @return    true
 **/
bool
  UI_GTKMM::stop_sleeping()
  {
    if (Gtk::Main::level() > 0)
      Gtk::Main::quit();

    UI_GTKMM::sleeping = false;

    return true;
  } // static bool UI_GTKMM::stop_sleeping()

/**
 ** quits the program
 **
 ** @param     -
 **
 ** @return    -
 **/
void
  UI_GTKMM::quit_program()
  {
    this->thrower(GAMESTATUS::QUIT, __FILE__, __LINE__);
    this->main_quit();
  } // void UI_GTKMM::quit_program()

/**
 ** a key has been pressed
 **
 ** @param     key   the key
 **
 ** @return    true (stop other handlers from being invoked for the event)
 **
 ** @todo      some shortcuts (search 'ToDo')
 **/
bool
  UI_GTKMM::key_press(GdkEventKey const* const key)
  {
#if 0
    struct GdkEventKey {
      GdkEventType type;
      GdkWindow *window;
      gint8 send_event;
      guint32 time;
      guint state;
      guint keyval;
      gint length;
      gchar *string;
      guint16 hardware_keycode;
      guint8 group;
    };
#endif

    if (this->table
        && this->table->in_game_review())
      if (!this->table->game_finished_->game_review->key_press(key))
        return true;

    if (key->state == GDK_SHIFT_MASK) {
      switch (key->keyval) {
      case GDK_KEY_Escape: // quit the program
        this->quit_program();
        break;
      } // switch (key->keyval)
    } // if (key->state == GDK_SHIFT_MASK)

    if (::game_status == GAMESTATUS::GAME_PLAY) {
      // play a card?
      Player& player = const_cast<Player&>(this->game().player_current());
      if (player.type() == Player::HUMAN) {
        SortedHand& hand = player.sorted_hand();
        if (!(key->state & ~(GDK_SHIFT_MASK | GDK_MOD1_MASK | GDK_MOD2_MASK))) {
          { // request a card to play
            unsigned requested_cardno = UINT_MAX;
            switch (key->keyval) {
            case GDK_KEY_1:
              requested_cardno = 0;
              break;
            case GDK_KEY_2:
              requested_cardno = 1;
              break;
            case GDK_KEY_3:
              requested_cardno = 2;
              break;
            case GDK_KEY_4:
              requested_cardno = 3;
              break;
            case GDK_KEY_5:
              requested_cardno = 4;
              break;
            case GDK_KEY_6:
              requested_cardno = 5;
              break;
            case GDK_KEY_7:
              requested_cardno = 6;
              break;
            case GDK_KEY_8:
              requested_cardno = 7;
              break;
            case GDK_KEY_9:
              requested_cardno = 8;
              break;
            case GDK_KEY_0:
            case GDK_KEY_a:
              requested_cardno = 9;
              break;
            case GDK_KEY_b:
              if (player.game().rule()(Rule::WITH_NINES))
                requested_cardno = 10;
              break;
            case GDK_KEY_c:
              if (player.game().rule()(Rule::WITH_NINES))
                requested_cardno = 11;
              break;
            } // switch (key->keyval)
            if (this->table->get_card()) {
              if (   (requested_cardno != UINT_MAX)
                  && !hand.played(requested_cardno)
                  && player.game().trick_current().isvalid(hand.card_all(requested_cardno)) ) {
                hand.request_position_all(requested_cardno);
                this->thrower(hand.requested_card(), __FILE__, __LINE__);
              }
            } // if (this->table->card_get())
          } // request a card to play
        } // if ((key->state & ~GDK_SHIFT_MASK) == 0)

      } // if (player.type() == Player::HUMAN)
    } // if (::game_status == GAMESTATUS::GAME_PLAY)

    if (!(key->state & ~(GDK_SHIFT_MASK | GDK_MOD1_MASK | GDK_MOD2_MASK))) {
      switch (key->keyval) {
      case GDK_KEY_question: // "?": help
        cout << "key codes:\n"
          << " ?: help on key codes\n"
          << " 1-0abc: play card\n"
          << " n: next announcement\n"
          << " m: announcement reply\n"
          << " h: card suggestion\n"
          << " l: show last trick\n"
          << " r: open rules window\n"
          << " p: open party points window (list page)\n"
          << " g: open party points window (graph page)\n"
          << " P: open players window\n"
#ifdef USE_NETWORK
          << " c: activate chatter\n"
          << " L: show network log\n"
#endif
          << " F1: open manual\n"
          << " F2: show preferences\n"
          << " Pause/Break: create bug report\n"
          << " C-c: exit pogram\n"
          << " S-Esc: exit program\n"
          << " C-f: toggle fullscreen\n"
          << " C-n: start new party\n"
          << " C-l: load party\n"
          << " C-s: save party\n"
          << " C-q: quit the program\n"
          << " C-r: redraw all\n"
          << " C-0: default card size\n"
          << " C-+: increase card size\n"
          << " C--: decrease card size\n"
#ifndef RELEASE
          << " i: show game information\n"
          << " C-g: print gameplay\n"
          << " e: print human cards information\n"
          << " v: print cards information of the player at the bottom (short form)\n"
          << " V: print cards information of the player at the bottom (long form)\n"
          << " t: print team information of the player at the bottom\n"
          << " T: print all team information of the player at the bottom\n"
#endif
          ;
        break;
      case GDK_KEY_Clear: // close windows
        // @todo    all
        break;
#ifdef USE_NETWORK
        // when network is activated, the menu takes care of the key
      case GDK_KEY_c: // show chat window
        // -> menu.cpp
        this->main_window->menu->chatter->activate();
        break;

      case GDK_KEY_L: // show network log
        this->network_log->present();
        break;
#endif

      case GDK_KEY_l: // show last trick
        // -> menu.cpp
        if (this->main_window->menu->last_trick->is_sensitive())
          this->main_window->menu->last_trick->activate();
        break;

#ifndef RELEASE
      case GDK_KEY_i: // show game information
        // -> menu.cpp
        if (this->main_window->menu->game_debug->is_sensitive())
          this->main_window->menu->game_debug->activate();
        break;
#endif

      case GDK_KEY_n: // next announcement
        if (   (::game_status >= GAMESTATUS::GAME_PLAY)
            && (::game_status < GAMESTATUS::GAME_FINISHED))
          if (this->game().number_of_humans() == 1) {
            this->game().make_announcement(this->game().human_player()->next_announcement(),
                                           *this->game().human_player()
                                          );
          } // if (only one human
        break;
      case GDK_KEY_m: // announcement reply
        if (   (::game_status >= GAMESTATUS::GAME_PLAY)
            && (::game_status < GAMESTATUS::GAME_FINISHED))
          if (this->game().number_of_humans() == 1)
            this->game().make_announcement(ANNOUNCEMENT::REPLY,
                                           *this->game().human_player()
                                          );
        break;
      case GDK_KEY_h: // card suggestion
        // -> menu.cpp
        if (this->main_window->menu->card_suggestion->is_sensitive())
          this->main_window->menu->card_suggestion->activate();
        break;
      case GDK_KEY_r: // show the rules
        // -> menu.cpp
        this->main_window->menu->rules->activate();
        break;
      case GDK_KEY_p: // show the party points
        // -> menu.cpp
        if (this->main_window->menu->party_points->is_sensitive()) {
          this->ui->table->party_points_->present();
          this->ui->table->party_points_->notebook->set_current_page(0);
        }
        break;
      case GDK_KEY_g: // show the party points graph
        if (this->main_window->menu->party_points->is_sensitive()) {
          this->ui->table->party_points_->present();
          this->ui->table->party_points_->notebook->set_current_page(1);
        }
        break;
      case GDK_KEY_P: // show the players
        // -> menu.cpp
        this->main_window->menu->players->activate();
        break;
#ifndef RELEASE
      case GDK_KEY_e: // print human cards information
        if (   (::game_status >= GAMESTATUS::GAME_POVERTY_SHIFT)
            && (::game_status < GAMESTATUS::GAME_FINISHED)
            && this->game().human_player()) {
          cout << '\n'
            << this->game().human_player()->name() << ": "
            << this->game().human_player()->cards_information() << '\n';
        }
        break;
      case GDK_KEY_v: // print the cards information of the player at the bottom
        if (   (::game_status >= GAMESTATUS::GAME_POVERTY_SHIFT)
            && (::game_status < GAMESTATUS::GAME_FINISHED) ) {
          auto ai = dynamic_cast<Ai const*>(&this->table->player(SOUTH));
          if (ai)
            cout << '\n'
              << ai->name() << ": "
              << ai->cards_information() << '\n';
        }
        break;
      case GDK_KEY_V: // print the cards information of the player at the bottom
        if (   (::game_status >= GAMESTATUS::GAME_POVERTY_SHIFT)
            && (::game_status < GAMESTATUS::GAME_FINISHED) ) {
          auto ai = dynamic_cast<Ai const*>(&this->table->player(SOUTH));
          if (ai)
            cout << '\n'
              << ai->name() << ": ";
          ai->cards_information().write(cout);
          cout << '\n';
        }
        break;
      case GDK_KEY_t: // print team information of the player at the bottom
        if (   (::game_status >= GAMESTATUS::GAME_POVERTY_SHIFT)
            && (::game_status < GAMESTATUS::GAME_FINISHED) ) {
          auto ai = dynamic_cast<Ai const*>(&this->table->player(SOUTH));
          if (ai)
            cout << '\n'
              << ai->name() << ": "
              << ai->team_information() << '\n';
        }
        break;
      case GDK_KEY_T: // print all team information
        if (   (::game_status >= GAMESTATUS::GAME_POVERTY_SHIFT)
            && (::game_status < GAMESTATUS::GAME_FINISHED) ) {
          for (auto p : this->game().players_const()) {
            if (!dynamic_cast<Ai*>(p)) {
              cout << '\n'
                << p->name() << ": no ai\n";
              continue;
            }
            cout << '\n'
              << p->name() << ": "
              << dynamic_cast<Ai*>(p)->team_information() << '\n';
          } // for (p)
        }
        break;
#endif // #ifndef RELEASE

      case GDK_KEY_F1: // show manual
        // -> menu.cpp
        this->main_window->menu->help->activate();
        break;
      case GDK_KEY_F2: // show preferences
        // -> menu.cpp
        this->main_window->menu->preferences->activate();
        break;
      case GDK_KEY_Pause: // create a bug report
      case GDK_KEY_Break: // create a bug report
        this->bug_report->create_report();
        break;
      case GDK_KEY_period: // toggle show valid cards
        // -> menu.cpp
        this->main_window->menu->show_all_hands->activate();
        break;
      case GDK_KEY_Escape:
        break;
      } // switch (key->keyval)
    } // if (key->state == 0)

    if (key->state & GDK_CONTROL_MASK) {
      switch (key->keyval) {
      case GDK_KEY_c: // exit the program
        exit(EXIT_SUCCESS);
        break;
      case GDK_KEY_f: // toggle fullscreen
        if (this->main_window->get_window()->get_state()
            & Gdk::WINDOW_STATE_FULLSCREEN)
          this->main_window->unfullscreen();
        else
          this->main_window->fullscreen();
        break;

#ifndef RELEASE
      case GDK_KEY_g: // print the gameplay
        cout << this->game().gameplay();
        break;
#endif

      case GDK_KEY_n: // start new party
        // -> menu.cpp
        this->main_window->menu->new_party->activate();
        break;
      case GDK_KEY_l: // load party
        // -> menu.cpp
        this->main_window->menu->load_party->activate();
        break;
      case GDK_KEY_s: // save party
        // -> menu.cpp
        this->main_window->menu->save_party->activate();
        break;
      case GDK_KEY_q: // quit the program
        // -> menu.cpp
        this->main_window->menu->quit->activate();
        break;
      case GDK_KEY_r: // redraw all
        this->redraw_all();
        break;
      case GDK_KEY_0: // default card size
        ::setting.set(Setting::ORIGINAL_CARDS_SIZE, true);
        break;
      case GDK_KEY_plus: // increase card size
        ::setting.set(Setting::CARDS_HEIGHT,
                      ::setting(Setting::CARDS_HEIGHT) + 1);
        ::setting.set(Setting::ORIGINAL_CARDS_SIZE, false);
        break;
      case GDK_KEY_minus: // decrease card size
        ::setting.set(Setting::CARDS_HEIGHT,
                      ::setting(Setting::CARDS_HEIGHT) - 1);
        ::setting.set(Setting::ORIGINAL_CARDS_SIZE, false);
        break;
      } // switch (key->keyval)
    } // if (key->state == GDK_CONTROL_MASK)

    return true;
  } // bool UI_GTKMM::key_press(GdkEventKey* key)

/**
 ** @param     page   the page to be loaded
 **
 ** @return    -
 **
 ** @todo	all
 **/
void
  UI_GTKMM::help_load(string const& page)
  {
  } // void UI_GTKMM::help_load(string page)

/**
 **
 ** @param     player    name of the player who has chatted
 ** @param     message   message
 ** @param     self      whether this player has chattet
 **			 (else the chatter comes from the network)
 **			 default: true
 **
 ** @return    -
 **/
void
  UI_GTKMM::chat(string const& player, string const& message,
                 bool const self)
  {
    this->chatter->chat(player, message);
  } // void UI_GTKMM::chat(string player, string message, bool self = true)

/**
 ** this is the first run of the program
 **
 ** @param     message   	message for the user
 **
 ** @return    -
 **/
void
  UI_GTKMM::first_run(Translator::Translation const& message)
  {
    DEBUG_ASSERTION(!this->first_run_window,
                    "UI_GTKMM::first_run(message)\n"
                    "  the window is already created.");

    this->first_run_window = make_unique<FirstRun>(this, message);

    this->first_run_window->show();
  } // void UI_GTKMM::first_run(Translator::Translation message)

/**
 ** the program is updated
 **
 ** @param     old_version   the old version
 **
 ** @return    -
 **/
void
  UI_GTKMM::program_updated(DK::Utils::Version const& old_version)
  {
    DEBUG_ASSERTION(!this->program_updated_window,
                    "UI_GTKMM::program_updated(old_version)\n"
                    "  the window is already created.");

    this->program_updated_window = make_unique<ProgramUpdated>(this, old_version);

    this->program_updated_window->show();
  } // void UI_GTKMM::program_updated(DK::Utils::Version old_version)

/**
 ** show the message in an information window
 ** Does not shows the message, if it has already been shown and is no error message and is not forced to be shown.
 **
 ** @param     message   	message to display
 ** @param     type   	the type of the message
 ** @param     force_show     whether to show the message, although it was already shown (default: false)
 **
 ** @return    -
 **
 ** @todo	remove memory leak
 ** @todo	add a parameter for a title
 ** @todo	put the windows on top of all windows
 **
 ** @bug	memory leak (the dialog is never deleted)
 **/
void
  UI_GTKMM::information(Translator::Translation const& message,
                        INFORMATION::Type const type,
                        bool const force_show)
  {
    // check, whether the message was already shown
    if (   !this->add_to_messages_shown(message, type)
        && (type != INFORMATION::PROBLEM)
        && !force_show)
      return ;

    auto message_type = Gtk::MESSAGE_INFO;
    switch (type) {
    case INFORMATION::NORMAL:
      message_type = Gtk::MESSAGE_INFO;
      break;
    case INFORMATION::WARNING:
      message_type = Gtk::MESSAGE_WARNING;
      break;
    case INFORMATION::PROBLEM:
      message_type = Gtk::MESSAGE_ERROR;
      break;
    } // switch(type)

    // memory leak: will not be destructed
    auto information
      = new Gtk::MessageDialog(message.translation(),
                               false,
                               message_type,
                               Gtk::BUTTONS_NONE,
                               false);
#ifdef OUTDATED
    // gtkmm 3: use Gtk::Dialog for using an image
#ifdef GLIBMM_EXCEPTIONS_ENABLED
    try {
      this->icon = Gdk::Pixbuf::create_from_file(::setting.path(Setting::ICON_FILE));
    } catch (Glib::FileError file_error) {
      ::ui->error(::translation("Error::loading icon (%sfile%)",
                                ::setting.path(Setting::ICON_FILE)).translation());
    } catch (Gdk::PixbufError pixbuf_error) {
      ::ui->error(::translation("Error::loading icon (%sfile%)",
                                ::setting.path(Setting::ICON_FILE)).translation());
    } // try
#else
    std::auto_ptr<Glib::Error> error;
    this->icon = Gdk::Pixbuf::create_from_file(::setting.path(Setting::ICON_FILE), error);
    if (error.get())
      ::ui->error(::translation("Error::loading icon (%sfile%)",
                                ::setting.path(Setting::ICON_FILE)).translation());
#endif
    if (this->icon) {
      auto image = Gtk::manage(new Gtk::Image(icon));
      information->set_image(*image);
    }
#endif

    // memory leak: no call of 'ui->remove_from_translation(*close_button)'
    auto close_button
      = Gtk::manage(new Gtk::StockButton(Gtk::Stock::CLOSE, "close"));
    this->translations->add(*close_button, ::translation("close"));
    information->add_action_widget(*close_button, Gtk::RESPONSE_CLOSE);
    close_button->show_all();

    close_button->signal_clicked().connect(sigc::mem_fun(*information,
                                                         &Gtk::Widget::hide)
                                          );

    if (   (type == INFORMATION::PROBLEM)
        || (::game_status == GAMESTATUS::PROGRAMSTART) ) {
      if (this->program_updated_window)
        information->set_transient_for(*this->program_updated_window);
      else if (this->main_window)
        information->set_transient_for(*this->main_window);
      information->set_modal();
      Gtk::Main::run(*information);
      delete information;
    } else {
      information->present();
    }
  } // void UI_GTKMM::information(Translator::Translation message, INFORMATION::Type type, bool force_show = false)

/**
 ** shown an error window
 ** and quit the program
 **
 ** @param     message   	message to display
 **
 ** @return    -
 **/
void
UI_GTKMM::error(string const& message)
{
  static bool already_running = false;

  if (already_running) {
    cerr << '\n'
      << ::translation("Press <Return> to exit.").translation() << endl;
    std::cin.get();
  } else { // if !(already_running)

    already_running = true;

    if (this->splash_screen)
      this->splash_screen->hide();

    Error error(message, this);
#ifdef RELEASE
    error.run();
#else
    error.present();
    while (error.is_visible()) {
      this->sleep(1000);
      this->thrower.clear();
    }
#endif
  } // if !(already_running)

#ifdef ASSERTION_GENERATES_SEGFAULT
  cerr << ::translation("Creating segmentation fault.").translation()
    << endl;
  SEGFAULT;
#endif // #ifdef ASSERTION_GENERATES_SEGFAULT

  ::exit(EXIT_FAILURE);
} // void UI_GTKMM::error(string const& message)

/**
 ** generate an error (for testings)
 **
 ** @param     error_message   error message
 **
 ** @return    -
 **/
void
UI_GTKMM::generate_error(string const error_message)
{
  ::generate_error(error_message);
} // void UI_GTKMM::generate_error(string error_message)

/**
 ** iconifies all windows
 **
 ** @param     -
 **
 ** @return    -
 **/
void 
UI_GTKMM::iconify_all_windows()
{
  for (auto& w : this->windows)
    w->iconify();
} // void UI_GTKMM::conify_all_windows()

/**
 ** deiconifies all windows
 **
 ** @param     -
 **
 ** @return    -
 **/
void
UI_GTKMM::deiconify_all_windows()
{
  for (auto& w : this->windows)
    w->deiconify();
} // void UI_GTKMM::deiconify_all_windows()

} // namespace UI_GTKMM_NS

#endif // #ifdef USE_UI_GTKMM
