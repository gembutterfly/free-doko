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

#include "menu.h"

#include "ui.h"
#include "translations.h"
#include "main_window.h"
#include "party_settings.h"
#include "rules.h"
#include "players.h"
#include "players_db.h"
#include "game_debug.h"
#include "preferences.h"
#include "table.h"
#include "party_points.h"
#include "bug_report.h"
#include "bug_report_replay.h"
#include "help.h"
#include "license.h"
#include "changelog.h"
#include "support.h"
#include "about.h"
#ifdef USE_NETWORK
#include "chatter.h"
#include "network.h"
#include "network_log.h"
#endif

#include "../../basistypes.h"
#include "../../party/rule.h"
#include "../../game/game.h"
#include "../../misc/setting.h"
#include "../../misc/setting.theme.h"
#include "../../utils/string.h"

#include <gtkmm/alignment.h>
#include <gtkmm/menu.h>
#include <gtkmm/menuitem.h>
#include <gtkmm/stock.h>
#include <gtkmm/checkmenuitem.h>
#include <gtkmm/imagemenuitem.h>
#include <gtkmm/separatormenuitem.h>
#include <gtkmm/notebook.h>
#include <gtkmm/filechooserdialog.h>
namespace UI_GTKMM_NS {

  /**
   ** Constructor
   **
   ** @param     main_window   the corresponding main window
   **
   ** @return    -
   **/
  Menu::Menu(MainWindow* const main_window) :
    Base(main_window),
    MenuBar(),
    main_window(main_window)
    {
      this->init();
      this->signal_realize().connect(sigc::mem_fun(*this, &Menu::language_update));

      return ;
    } // Menu::Menu(MainWindow* main_window)

  /**
   ** Destruktor
   **
   ** @param     -
   **
   ** @return    -
   **/
  Menu::~Menu()
  {
    for (auto& item : this->themes_menu->get_children())
      delete static_cast<Setting::Theme*>(item->get_data("theme"));
    return ;
  } // Menu::~Menu()

  /**
   ** creates all subelements
   **
   ** @param     -
   **
   ** @return    -
   **/
  void
    Menu::init()
    {
      //*(Gtk::manage(new Gtk::Image(Gtk::Stock::QUIT, Gtk::ICON_SIZE_MENU)))

      { // party
        this->party_menu = Gtk::manage(new Gtk::Menu());

        this->party = Gtk::manage(new Gtk::ImageMenuItem("party"));
        this->ui->translations->add(*(dynamic_cast<Gtk::Label*>(this->party->get_child())),
                                    ::translation("Menu::party"));

#if 0
        this->new_party = Gtk::manage(new Gtk::ImageMenuItem(*(Gtk::manage(new Gtk::Image(Gtk::Stock::NEW, Gtk::ICON_SIZE_MENU))),
                                                             "new party"));
#endif
        this->new_party = Gtk::manage(new Gtk::ImageMenuItem("new party"));
        this->ui->translations->add(*(dynamic_cast<Gtk::Label*>(this->new_party->get_child())),
                                    ::translation("Menu::Item::new party"));

        this->load_party = Gtk::manage(new Gtk::ImageMenuItem(*(Gtk::manage(new Gtk::Image(Gtk::Stock::OPEN, Gtk::ICON_SIZE_MENU))),
                                                              "load party"));
        this->ui->translations->add(*(dynamic_cast<Gtk::Label*>(this->load_party->get_child())),
                                    ::translation("Menu::Item::load party")
                                    + "...");

        this->save_party = Gtk::manage(new Gtk::ImageMenuItem(*(Gtk::manage(new Gtk::Image(Gtk::Stock::SAVE_AS, Gtk::ICON_SIZE_MENU))),
                                                              "save party"));
        this->ui->translations->add(*(dynamic_cast<Gtk::Label*>(this->save_party->get_child())),
                                    ::translation("Menu::Item::save party")
                                    + "...");

        this->end_party = Gtk::manage(new Gtk::ImageMenuItem(*(Gtk::manage(new Gtk::Image(Gtk::Stock::STOP, Gtk::ICON_SIZE_MENU))),
                                                             "end party"));
        this->ui->translations->add(*(dynamic_cast<Gtk::Label*>(this->end_party->get_child())),
                                    ::translation("Menu::Item::end party"));

        this->quit = Gtk::manage(new Gtk::ImageMenuItem(*(Gtk::manage(new Gtk::Image(Gtk::Stock::QUIT, Gtk::ICON_SIZE_MENU))),
                                                        "quit"));
        this->ui->translations->add(*(dynamic_cast<Gtk::Label*>(this->quit->get_child())),
                                    ::translation("Menu::Item::quit"));
      } // party

      { // information

        this->information_menu = Gtk::manage(new Gtk::Menu());

        this->information = Gtk::manage(new Gtk::ImageMenuItem("information"));
        this->ui->translations->add(*(dynamic_cast<Gtk::Label*>(this->information->get_child())),
                                    ::translation("Menu::information"));

        this->card_suggestion = Gtk::manage(new Gtk::ImageMenuItem("card suggestion"));
        this->ui->translations->add(*(dynamic_cast<Gtk::Label*>(this->card_suggestion->get_child())),
                                    ::translation("Menu::Item::card suggestion")
                                    + "...");

        this->last_trick = Gtk::manage(new Gtk::ImageMenuItem("last trick"));
        this->ui->translations->add(*(dynamic_cast<Gtk::Label*>(this->last_trick->get_child())),
                                    ::translation("Menu::Item::last trick")
                                    + "...");

        this->game_debug = Gtk::manage(new Gtk::ImageMenuItem("game debug"));
        this->ui->translations->add(*(dynamic_cast<Gtk::Label*>(this->game_debug->get_child())),
                                    ::translation("Menu::Item::game debug")
                                    + "...");

        this->bug_report_replay = Gtk::manage(new Gtk::ImageMenuItem("bug report replay"));
        this->ui->translations->add(*(dynamic_cast<Gtk::Label*>(this->bug_report_replay->get_child())),
                                    ::translation("Menu::Item::bug report replay")
                                    + "...");

        this->rules = Gtk::manage(new Gtk::ImageMenuItem(*(Gtk::manage(new Gtk::Image(Gtk::Stock::PROPERTIES, Gtk::ICON_SIZE_MENU))),
                                                         "rules"));
        this->ui->translations->add(*(dynamic_cast<Gtk::Label*>(this->rules->get_child())),
                                    ::translation("Menu::Item::rules")
                                    + "...");

        this->party_points = Gtk::manage(new Gtk::ImageMenuItem("party points"));
        this->ui->translations->add(*(dynamic_cast<Gtk::Label*>(this->party_points->get_child())),
                                    ::translation("Menu::Item::party points")
                                    + "...");

        this->players = Gtk::manage(new Gtk::ImageMenuItem("players"));
        this->ui->translations->add(*(dynamic_cast<Gtk::Label*>(this->players->get_child())),
                                    ::translation("Menu::Item::players")
                                    + "...");

        this->players_db = Gtk::manage(new Gtk::ImageMenuItem("players database"));
        this->ui->translations->add(*(dynamic_cast<Gtk::Label*>(this->players_db->get_child())),
                                    ::translation("Menu::Item::players database")
                                    + "...");
      } // information

      { // announcements
        this->announcements = Gtk::manage(new Gtk::MenuItem("announcements"));
        this->ui->translations->add(*(dynamic_cast<Gtk::Label*>(this->announcements->get_child())),
                                    ::translation("Menu::announcements"));
      } // announcements


      { // preferences
        this->preferences_menu = Gtk::manage(new Gtk::Menu());

        this->preferences = Gtk::manage(new Gtk::MenuItem("preferences"));
        this->ui->translations->add(*(dynamic_cast<Gtk::Label*>(this->preferences->get_child())),
                                    ::translation("Menu::preferences"));

        this->emphasize_valid_cards
          = Gtk::manage(new Gtk::CheckMenuItem(::name(Setting::EMPHASIZE_VALID_CARDS)));
        this->ui->translations->add(*(dynamic_cast<Gtk::Label*>(this->emphasize_valid_cards->get_child())),
                                    ::translation(Setting::EMPHASIZE_VALID_CARDS));

        this->show_all_hands
          = Gtk::manage(new Gtk::CheckMenuItem(::name(Setting::SHOW_ALL_HANDS)));
        this->ui->translations->add(*(dynamic_cast<Gtk::Label*>(this->show_all_hands->get_child())),
                                    ::translation(Setting::SHOW_ALL_HANDS));

        this->cards_order = Gtk::manage(new Gtk::ImageMenuItem(::name(Setting::CARDS_ORDER)));
        this->ui->translations->add(*(dynamic_cast<Gtk::Label*>(this->cards_order->get_child())),
                                    ::translation(Setting::CARDS_ORDER));
        { // themes
          this->themes_menu = Gtk::manage(new Gtk::Menu());
          this->themes_item = Gtk::manage(new Gtk::MenuItem("themes"));
          this->ui->translations->add(*(dynamic_cast<Gtk::Label*>(this->themes_item->get_child())),
                                      ::translation("Menu::Item::preferences::themes"));
          for (auto const& theme : ::setting.themes()) {
            auto item = Gtk::manage(new Gtk::MenuItem("theme"));
            this->ui->translations->add(*(dynamic_cast<Gtk::Label*>(item->get_child())),
                                        theme->name());
            item->set_data("theme", new Setting::Theme(*theme));
            this->themes_menu->append(*item);
            item->signal_activate().connect(sigc::bind<Setting::Theme const* const>(sigc::mem_fun1(*this, &Menu::apply_theme_event), static_cast<Setting::Theme const*>(item->get_data("theme"))));
          } // for (theme \in ::setting.themes())
        } // themes

        this->preferences_preferences = Gtk::manage(new Gtk::ImageMenuItem(*(Gtk::manage(new Gtk::Image(Gtk::Stock::PREFERENCES, Gtk::ICON_SIZE_MENU))),
                                                                           "preferences"));
        this->ui->translations->add(*(dynamic_cast<Gtk::Label*>(this->preferences_preferences->get_child())),
                                    ::translation("Menu::Item::preferences")
                                    + "...");
      } // preferences


      { // network
#ifdef USE_NETWORK
        this->network_menu = Gtk::manage(new Gtk::Menu());

        this->network = Gtk::manage(new Gtk::MenuItem("network"));
        this->ui->translations->add(*(dynamic_cast<Gtk::Label*>(this->network->get_child())),
                                    ::translation("Menu::network"));

        this->chatter = Gtk::manage(new Gtk::ImageMenuItem("Menu::Item::chatter"));
        this->ui->translations->add(*(dynamic_cast<Gtk::Label*>(this->chatter->get_child())),
                                    ::translation("Menu::Item::chatter")
                                    + "...");

        this->network_log = Gtk::manage(new Gtk::ImageMenuItem("Menu::Item::network log"));
        this->ui->translations->add(*(dynamic_cast<Gtk::Label*>(this->network_log->get_child())),
                                    ::translation("Menu::Item::network log")
                                    + "...");
        this->network_settings = Gtk::manage(new Gtk::ImageMenuItem("Menu::Item::network settings"));
        this->ui->translations->add(*(dynamic_cast<Gtk::Label*>(this->network_settings->get_child())),
                                    ::translation("Menu::Item::network settings")
                                    + "...");
#endif
      } // network


      { // bug report
        this->bug_report = Gtk::manage(new Gtk::ImageMenuItem("bug report"));
        this->ui->translations->add(*(dynamic_cast<Gtk::Label*>(this->bug_report->get_child())),
                                    ::translation("Menu::Item::bug report"));
      } // bug report

      { // help
        this->help_menu = Gtk::manage(new Gtk::Menu());

        this->help = Gtk::manage(new Gtk::ImageMenuItem("help"));
        this->ui->translations->add(*(dynamic_cast<Gtk::Label*>(this->help->get_child())),
                                    ::translation("Menu::help"));

        this->help_index = Gtk::manage(new Gtk::ImageMenuItem(*(Gtk::manage(new Gtk::Image(Gtk::Stock::HELP, Gtk::ICON_SIZE_MENU))),
                                                              "help"));
        this->ui->translations->add(*(dynamic_cast<Gtk::Label*>(this->help_index->get_child())),
                                    ::translation("Menu::Item::help index")
                                    + "...");

        this->license = Gtk::manage(new Gtk::ImageMenuItem("license"));
        this->ui->translations->add(*(dynamic_cast<Gtk::Label*>(this->license->get_child())),
                                    ::translation("Menu::Item::license")
                                    + "...");

        this->changelog = Gtk::manage(new Gtk::ImageMenuItem("changelog"));
        this->ui->translations->add(*(dynamic_cast<Gtk::Label*>(this->changelog->get_child())),
                                    ::translation("Menu::Item::changelog")
                                    + "...");

        this->support = Gtk::manage(new Gtk::ImageMenuItem("support"));
        this->ui->translations->add(*(dynamic_cast<Gtk::Label*>(this->support->get_child())),
                                    ::translation("Menu::Item::support")
                                    + "...");

        this->about = Gtk::manage(new Gtk::ImageMenuItem("about"));
        this->ui->translations->add(*(dynamic_cast<Gtk::Label*>(this->about->get_child())),
                                    ::translation("Menu::Item::about")
                                    + "...");

        this->generate_error = Gtk::manage(new Gtk::ImageMenuItem(*(Gtk::manage(new Gtk::Image(Gtk::Stock::DIALOG_ERROR, Gtk::ICON_SIZE_MENU))),
                                                                  "generate error"));
        this->ui->translations->add(*(dynamic_cast<Gtk::Label*>(this->generate_error->get_child())),
                                    ::translation("Menu::Item::generate error"));
      } // help

      this->append(*this->party);
      this->append(*this->information);
      this->append(*this->announcements);
      this->append(*this->preferences);
#ifdef USE_NETWORK
      this->append(*this->network);
#endif
      // ToDo: make the bug report be right
      // the following does not work (I think because the 'help' menu is 
      // on the right, also
      //this->bug_report->set_right_justified();
      this->append(*this->bug_report);
      this->append(*this->help);
      this->help->set_right_justified();

      // the party menu
      this->party->set_submenu(*this->party_menu);

      this->party_menu->append(*this->new_party);
      this->party_menu->append(*this->load_party);
      this->party_menu->append(*this->save_party);
      this->party_menu->append(*this->end_party);
      this->party_menu->append(*Gtk::manage(new Gtk::SeparatorMenuItem()));
      this->party_menu->append(*this->quit);
      // the information menu
      this->information->set_submenu(*this->information_menu);
      this->information_menu->append(*this->card_suggestion);
      this->information_menu->append(*this->last_trick);
#ifndef RELEASE
      this->information_menu->append(*this->game_debug);
      this->information_menu->append(*this->bug_report_replay);
#endif
      this->information_menu->append(*Gtk::manage(new Gtk::SeparatorMenuItem()));
      this->information_menu->append(*this->rules);
      this->information_menu->append(*this->party_points);
      this->information_menu->append(*Gtk::manage(new Gtk::SeparatorMenuItem()));
      this->information_menu->append(*this->players);
      this->information_menu->append(*this->players_db);
      // the preferences menu
      this->preferences->set_submenu(*this->preferences_menu);
      this->preferences_menu->append(*this->emphasize_valid_cards);
      this->preferences_menu->append(*this->show_all_hands);
      this->preferences_menu->append(*this->cards_order);
      this->preferences_menu->append(*this->themes_item);
      this->themes_item->set_submenu(*this->themes_menu);
      this->preferences_menu->append(*this->preferences_preferences);
#ifdef USE_NETWORK
      // the network menu
      this->network->set_submenu(*this->network_menu);
      this->network_menu->append(*this->chatter);
      this->network_menu->append(*this->network_log);
      this->network_menu->append(*this->network_settings);
#endif
      // the help menu
      this->help->set_submenu(*this->help_menu);
      this->help_menu->append(*this->help_index);
      this->help_menu->append(*this->license);
      this->help_menu->append(*this->changelog);
      this->help_menu->append(*this->support);
      this->help_menu->append(*this->about);
#ifndef RELEASE
      this->help_menu->append(*Gtk::manage(new Gtk::SeparatorMenuItem()));
      this->help_menu->append(*this->generate_error);
#endif

      this->card_suggestion->set_sensitive(false);
      this->last_trick->set_sensitive(false);
      this->game_debug->set_sensitive(false);
      this->information->set_sensitive(false);

      this->emphasize_valid_cards->set_active(::setting.value(Setting::EMPHASIZE_VALID_CARDS));
      this->show_all_hands->set_active(::setting.value(Setting::SHOW_ALL_HANDS));


      { // load party file chooser
        this->load_party_file_chooser = new Gtk::FileChooserDialog("load party",
                                                                   Gtk::FILE_CHOOSER_ACTION_OPEN);
        this->ui->translations->add(*this->load_party_file_chooser,
                                    ::translation("~load party"));

        auto cancel_button
          = this->load_party_file_chooser->add_button(Gtk::Stock::CANCEL,
                                                      Gtk::RESPONSE_CANCEL);
        auto ok_button
          = this->load_party_file_chooser->add_button(Gtk::Stock::OPEN,
                                                      Gtk::RESPONSE_OK);


        { // signals
          this->load_party_file_chooser->signal_file_activated().connect(sigc::mem_fun(*this, &Menu::load_party_event));
          this->load_party_file_chooser->signal_file_activated().connect(sigc::mem_fun(*this->load_party_file_chooser, &Widget::hide));

          ok_button->signal_clicked().connect(sigc::mem_fun(*this, &Menu::load_party_event));
          ok_button->signal_clicked().connect(sigc::mem_fun(*this->load_party_file_chooser, &Widget::hide));
          cancel_button->signal_clicked().connect(sigc::mem_fun(*this->load_party_file_chooser, &Widget::hide));
        } // signals
        {
          string dir = (::setting(Setting::PRIVATE_DATA_DIRECTORY)
                        + "/"
                        + ::setting(Setting::PARTIES_DIRECTORY)
                        + "/"
                       );
#ifdef WINDOWS
          DK::Utils::String::replace_all(dir, "/", "\\");
#endif
          this->load_party_file_chooser->set_current_folder(dir);
        }
      } // load party file chooser
      { // save party file chooser
        this->save_party_file_chooser = new Gtk::FileChooserDialog("save party",
                                                                   Gtk::FILE_CHOOSER_ACTION_SAVE);
        this->ui->translations->add(*this->save_party_file_chooser,
                                    ::translation("~save party"));

        auto cancel_button
          = this->save_party_file_chooser->add_button(Gtk::Stock::CANCEL,
                                                      Gtk::RESPONSE_CANCEL);
        auto ok_button
          = this->save_party_file_chooser->add_button(Gtk::Stock::SAVE,
                                                      Gtk::RESPONSE_OK);


        { // signals
          this->save_party_file_chooser->signal_file_activated().connect(sigc::mem_fun(*this, &Menu::save_party_event));
          this->save_party_file_chooser->signal_file_activated().connect(sigc::mem_fun(*this->save_party_file_chooser, &Widget::hide));

          ok_button->signal_clicked().connect(sigc::mem_fun(*this, &Menu::save_party_event));
          ok_button->signal_clicked().connect(sigc::mem_fun(*this->save_party_file_chooser, &Widget::hide));
          cancel_button->signal_clicked().connect(sigc::mem_fun(*this->save_party_file_chooser, &Widget::hide));
        } // signals
        {
          string dir = (::setting(Setting::PRIVATE_DATA_DIRECTORY)
                        + "/"
                        + ::setting(Setting::PARTIES_DIRECTORY)
                        + "/"
                       );
#ifdef WINDOWS
          DK::Utils::String::replace_all(dir, "/", "\\");
#endif
          this->save_party_file_chooser->set_current_folder(dir);
        }
      } // save party file chooser

      this->show_all();
      this->bug_report_replay->hide();

      return ;
    } // void Menu::init()

  /**
   ** set the signals
   **
   ** @param     -
   **
   ** @return    -
   **/
  void
    Menu::set_signals()
    {
      { // the signals
        this->new_party->signal_activate().connect(sigc::mem_fun(*this->main_window,
                                                                 &MainWindow::start_new_party_event)
                                                  );
        this->load_party->signal_activate().connect(sigc::mem_fun0(*this->load_party_file_chooser,
                                                                   &Gtk::Window::present)
                                                   );
        this->save_party->signal_activate().connect(sigc::mem_fun0(*this->save_party_file_chooser,
                                                                   &Gtk::Window::present)
                                                   );
        this->end_party->signal_activate().connect(sigc::mem_fun(*this->main_window,
                                                                 &MainWindow::end_party_event)
                                                  );
        this->quit->signal_activate().connect(sigc::mem_fun(*this->main_window,
                                                            &MainWindow::quit_program_event)
                                             );


        this->card_suggestion->signal_activate().connect(sigc::bind<bool const>(sigc::mem_fun(*(this->ui->table), &Table::show_card_suggestion), true));
        this->last_trick->signal_activate().connect(sigc::mem_fun(*this->ui->table,
                                                                  &Table::show_last_trick));

#ifndef RELEASE
        this->game_debug->signal_activate().connect(sigc::mem_fun0(*this->ui->game_debug,
                                                                   &Gtk::Window::present));
        //this->bug_report_replay->signal_activate().connect(sigc::mem_fun(*this->ui->bug_report_replay, &Gtk::Window::present));
#endif

        this->rules->signal_activate().connect(sigc::mem_fun0(*this->ui->party_settings->rules, &Gtk::Window::present));
        this->players->signal_activate().connect(sigc::mem_fun0(*this->ui->party_settings->players, &Gtk::Window::present));
        this->players_db->signal_activate().connect(sigc::mem_fun0(*this->ui->players_db, &Gtk::Window::present));


        this->emphasize_valid_cards->signal_toggled().connect(sigc::mem_fun(*this, &Menu::emphasize_valid_cards_set)
                                                             );
        this->show_all_hands->signal_toggled().connect(sigc::mem_fun(*this, &Menu::show_all_hands_set)
                                                      );
        this->cards_order->signal_activate().connect(sigc::mem_fun(*this->ui->preferences, &Preferences::show_cards_order));
        this->themes_item->signal_activate().connect(sigc::mem_fun(*this, &Menu::update_themes_sensitivity));
        this->preferences_preferences->signal_activate().connect(sigc::mem_fun0(*this->ui->preferences, &Gtk::Window::present));

#ifdef USE_NETWORK
        this->chatter->signal_activate().connect(sigc::mem_fun0(*this->ui->chatter, &Gtk::Window::present));
        this->network_log->signal_activate().connect(sigc::mem_fun0(*this->ui->network_log, &Gtk::Window::present));
        this->network_settings->signal_activate().connect(sigc::mem_fun0(*this->ui->network, &Gtk::Window::present));
#endif

        this->bug_report->signal_activate().connect(sigc::mem_fun(*this->ui->bug_report, &BugReport::create_report));


        this->help_index->signal_activate().connect(sigc::mem_fun0(*this->ui->help,
                                                                   &Help::show_manual));
        this->license->signal_activate().connect(sigc::mem_fun0(*this->ui->license,
                                                                &Gtk::Window::present));
        this->changelog->signal_activate().connect(sigc::mem_fun0(*(this->ui->changelog),
                                                                  &Gtk::Window::present));
        this->about->signal_activate().connect(sigc::mem_fun0(*(this->ui->about),
                                                              &Gtk::Window::present));
        this->support->signal_activate().connect(sigc::mem_fun0(*(this->ui->support),
                                                                &Gtk::Window::present));
        this->generate_error->signal_activate().connect(sigc::mem_fun(*(this->main_window),
                                                                      &MainWindow::generate_error_event));
      } // the signals

      { // keys

        // I have here some notes, whenever I differ from the HIG ('GNOME Human Interface Guidelines (1.0)', see http://developer.gnome.org/projects/gup/hig/)

        this->new_party->add_accelerator("activate",
                                         this->main_window->get_accel_group(),
                                         GDK_KEY_n,
                                         Gdk::CONTROL_MASK,
                                         Gtk::ACCEL_VISIBLE);
        // Note:
        // The HIG says 'C-o' for 'open a new document', but 'open' says to open in a new window (see HIG, the file menu). Here I do not want to open another FreeDoko window but to replace the existing party.
        // So I use 'C-l' (load) in contrast to 'C-s' (save).
        this->load_party->add_accelerator("activate",
                                          this->main_window->get_accel_group(),
                                          GDK_KEY_l,
                                          Gdk::CONTROL_MASK,
                                          Gtk::ACCEL_VISIBLE);
        this->save_party->add_accelerator("activate",
                                          this->main_window->get_accel_group(),
                                          GDK_KEY_s,
                                          Gdk::CONTROL_MASK,
                                          Gtk::ACCEL_VISIBLE);
        this->quit->add_accelerator("activate",
                                    this->main_window->get_accel_group(),
                                    GDK_KEY_q,
                                    Gdk::CONTROL_MASK,
                                    Gtk::ACCEL_VISIBLE);
#if 0
        this->card_suggestion->add_accelerator("activate",
                                               this->main_window->get_accel_group(),
                                               GDK_KEY_h,
                                               Gdk::ModifierType(0),
                                               Gtk::ACCEL_VISIBLE);
        this->last_trick->add_accelerator("activate",
                                          this->main_window->get_accel_group(),
                                          GDK_KEY_l,
                                          Gdk::ModifierType(0),
                                          Gtk::ACCEL_VISIBLE);
        this->rules->add_accelerator("activate",
                                     this->main_window->get_accel_group(),
                                     GDK_KEY_r,
                                     Gdk::ModifierType(0),
                                     Gtk::ACCEL_VISIBLE);
        this->party_points->add_accelerator("activate",
                                            this->main_window->get_accel_group(),
                                            GDK_KEY_p,
                                            Gdk::ModifierType(0),
                                            Gtk::ACCEL_VISIBLE);
#ifndef WORKAROUND
        // does not work
        this->players->add_accelerator("activate",
                                       this->main_window->get_accel_group(),
                                       GDK_KEY_P,
                                       Gdk::ModifierType(0),
                                       Gtk::ACCEL_VISIBLE);
#endif
#ifndef RELEASE
        this->game_debug->add_accelerator("activate",
                                          this->main_window->get_accel_group(),
                                          GDK_KEY_i,
                                          Gdk::ModifierType(0),
                                          Gtk::ACCEL_VISIBLE);
#endif
#endif
        // HIG: C-a should select all
        this->show_all_hands->add_accelerator("activate",
                                              this->main_window->get_accel_group(),
                                              GDK_KEY_a,
                                              Gdk::CONTROL_MASK,
                                              Gtk::ACCEL_VISIBLE);
        // HIG: C-o should open a new document / party
        this->cards_order->add_accelerator("activate",
                                           this->main_window->get_accel_group(),
                                           GDK_KEY_o,
                                           Gdk::CONTROL_MASK,
                                           Gtk::ACCEL_VISIBLE);
#ifndef USE_NETWORK
        // does conflict with chat
        this->emphasize_valid_cards->add_accelerator("activate",
                                                     this->main_window->get_accel_group(),
                                                     GDK_KEY_period,
                                                     Gdk::ModifierType(0),
                                                     Gtk::ACCEL_VISIBLE);
#endif

#ifdef USE_NETWORK
#if 0
        this->chatter->add_accelerator("activate",
                                       this->main_window->get_accel_group(),
                                       GDK_KEY_c,
                                       Gdk::ModifierType(0),
                                       Gtk::ACCEL_VISIBLE);
#endif
        this->network_settings->add_accelerator("activate",
                                                this->main_window->get_accel_group(),
                                                GDK_KEY_F3,
                                                Gdk::ModifierType(0),
                                                Gtk::ACCEL_VISIBLE);
#endif
        // Note:
        // The HIG uses F2 for 'Rename' (p.e. a filename in the file manager).
        // Since I did not find some shortcut for preferences I use F2 because
        // it is next to F1 for help.
        // Later I will use S-F2 for context sensitive preferences
        // as S-F1 for context sensitive help.
        this->preferences_preferences->add_accelerator("activate",
                                                       this->main_window->get_accel_group(),
                                                       GDK_KEY_F2,
                                                       Gdk::ModifierType(0),
                                                       Gtk::ACCEL_VISIBLE);
        this->help_index->add_accelerator("activate",
                                          this->main_window->get_accel_group(),
                                          GDK_KEY_F1,
                                          Gdk::ModifierType(0),
                                          Gtk::ACCEL_VISIBLE);
#if 0
        // any suggestions?
        // Ctrl-l is used for loading a party.
        // Further I do not think that Ctrl-Shift-something is good usability (which program (but emacs) does use such shortcuts?).
        this->license->add_accelerator("activate",
                                       this->main_window->get_accel_group(),
                                       GDK_KEY_l,
                                       Gdk::CONTROL_MASK,
                                       Gtk::ACCEL_VISIBLE);
#endif
      } // keys

      return ;
    } // void Menu::set_signals()

  /**
   ** sets the setting 'emphasize valid cards'
   **
   ** @param     -
   **
   ** @return    -
   **/
  void
    Menu::emphasize_valid_cards_set()
    {
      ::setting.set(Setting::EMPHASIZE_VALID_CARDS,
                    this->emphasize_valid_cards->get_active());

      return ;
    } // void Menu::emphasize_valid_cards_set()

  /**
   ** sets the setting 'show all hands'
   **
   ** @param     -
   **
   ** @return    -
   **/
  void
    Menu::show_all_hands_set()
    {
      ::setting.set(Setting::SHOW_ALL_HANDS,
                    this->show_all_hands->get_active());

      return ;
    } // void Menu::show_all_hands_set()

  /**
   ** event: make the announcement 'announcement' for player 'playerno'
   **
   ** @param     playerno   the number of the player who announces
   ** @param     announcement   the announcement to make
   **
   ** @return    -
   **/
  void
    Menu::announcement_event(unsigned const playerno, int const announcement)
    {
      // note, that here it is not the playernumber in the game but in the party
      this->ui->game().make_announcement(Announcement(announcement),
                                         this->ui->game().player(playerno));

      return ;
    } // void Menu::announcement_event(unsigned playerno, int announcement)

  /**
   ** event: announce 'swines' for player 'playerno'
   **
   ** @param     playerno   the number of the player who announces swines
   **				if it is 'UINT_MAX' the player is determined
   **				automatically
   **
   ** @return    -
   **/
  void
    Menu::swines_announcement_event(unsigned const playerno)
    {
      // note, that here it is not the playernumber in the game but in the party
      if (playerno == UINT_MAX) {
        for (auto const& p : this->ui->game().players_const()) {
          if (this->ui->game().swines_announcement_valid(*p)) {
            this->ui->game().swines_announce(*p);
            break;
          } 
        } // for (p : this->ui->game().players())
      } else { // if !(playerno == UINT_MAX)
        this->ui->game().swines_announce(this->ui->game().player(playerno));
      } // if !(playerno == UINT_MAX)

      return ;
    } // void Menu::swines_announcement_event(unsigned playerno)

  /**
   ** event: announce 'hyperswines' for player 'playerno'
   **
   ** @param     playerno   the number of the player with hyperswines
   **				if it is 'UINT_MAX' the player is determined
   **				automatically
   **
   ** @return    -
   **/
  void
    Menu::hyperswines_announcement_event(unsigned const playerno)
    {
      // note, that here it is not the playernumber in the game but in the party
      if (playerno == UINT_MAX) {
        for (auto const& p : this->ui->game().players_const()) {
          if (this->ui->game().hyperswines_announcement_valid(*p)) {
            this->ui->game().hyperswines_announce(*p);
            break;
          } 
        } // for (p : this->ui->game().players())
      } else { // if !(playerno == UINT_MAX)
        this->ui->game().hyperswines_announce(this->ui->game().player(playerno));
      } // if !(playerno == UINT_MAX)

      return ;
    } // void Menu::hyperswines_announcement_event(unsigned playerno)

  /**
   ** the party is opened
   ** some sensitivity is set
   **
   ** @param     -
   **
   ** @return    -
   **/
  void
    Menu::party_open()
    {
      this->party_points_present_connection
        = this->party_points->signal_activate().connect(sigc::mem_fun0(*this->ui->table->party_points_,
                                                                       &Gtk::Window::present));

      switch (::game_status) {

      case GAMESTATUS::PARTY_NEW:
        this->party_points->set_sensitive(false);
        this->end_party->set_sensitive(false);
        break;

      case GAMESTATUS::PARTY_INITIAL_LOADED:
      case GAMESTATUS::PARTY_LOADED:
        this->party_points->set_sensitive(true);
        this->end_party->set_sensitive(true);
        break;

      default:
        DEBUG_ASSERTION(false,
                        "Menu::party_open():\n"
                        "  game status = '" << ::game_status << "'");
      } // switch (::game_status)

      this->information->set_sensitive(true);
      this->bug_report_replay->set_sensitive(true);
      this->rules->set_sensitive(true);
      this->players->set_sensitive(true);
      this->players_db->set_sensitive(true);

      this->card_suggestion->set_sensitive(false);
      this->last_trick->set_sensitive(false);
      this->game_debug->set_sensitive(false);
      this->announcements->set_sensitive(false);
      this->announcements->unset_submenu();

      return ;
    } // void Menu::party_open()

  /**
   ** the party has started
   ** create the announcements menu
   ** make some information accessable
   **
   ** @param     -
   **
   ** @return    -
   **/
  void
    Menu::party_start()
    {
      this->end_party->set_sensitive(true);

      this->party_points->set_sensitive(true);

      { // create the announcements menu
        this->announcements_player.clear();
        this->announcements_player_menu.clear();
        this->announcements_announcement.clear();
        this->swines_separator.clear();
        this->swines_announcement.clear();
        this->hyperswines_announcement.clear();

        Gtk::Menu* menu = nullptr;
        if (this->ui->party().humanno() != 1) {
          menu  = Gtk::manage(new Gtk::Menu());
        } // if (this->ui->party().humanno() != 1)
        for (unsigned p = 0; p < this->ui->party().playerno(); p++) {
          this->announcements_player.push_back(Gtk::manage(new Gtk::MenuItem(this->ui->party().player(p).name())));

          this->announcements_player_menu.push_back(Gtk::manage(new Gtk::Menu()));
          this->announcements_announcement.push_back(vector<Gtk::MenuItem*>()); 

          this->announcements_announcement.back().push_back(Gtk::manage(new Gtk::MenuItem(::translation(ANNOUNCEMENT::REPLY).translation())));
          if (   this->ui->party().rule()(Rule::ANNOUNCEMENT_REPLY)
              && this->ui->party().rule()(Rule::ANNOUNCEMENT_STRICT_LIMIT)) {
            this->announcements_player_menu.back()->append(*this->announcements_announcement.back().back());
            this->announcements_announcement.back().back()->signal_activate().connect(sigc::bind<unsigned const, int const>(sigc::mem_fun(*this, &Menu::announcement_event), p, ANNOUNCEMENT::REPLY));
          }
          for (int a = ANNOUNCEMENT::NO120;
               a <= ANNOUNCEMENT::NO0;
               ++a) {
            this->announcements_announcement.back().push_back(Gtk::manage(new Gtk::MenuItem(::translation(static_cast<Announcement>(a)).translation())));
            this->announcements_player_menu.back()->append(*(this->announcements_announcement.back().back()));

            this->announcements_announcement.back().back()->signal_activate().connect(sigc::bind<unsigned const, int const>(sigc::mem_fun(*this, &Menu::announcement_event), p, a));

          } // for (a \in Announcement)

          { // swines separator
            this->swines_separator.push_back(Gtk::manage(new Gtk::SeparatorMenuItem()));
            this->announcements_player_menu.back()->append(*this->swines_separator.back());
          } // swines separator
          { // swines
            this->swines_announcement.push_back(Gtk::manage(new Gtk::MenuItem("swines")));
            this->ui->translations->add(*(dynamic_cast<Gtk::Label*>(this->swines_announcement.back()->get_child())),
                                        ::translation("Menu::Item::swines announcement"));
            this->announcements_player_menu.back()->append(*(this->swines_announcement.back()));

            this->swines_announcement.back()->signal_activate().connect(sigc::bind<unsigned const>(sigc::mem_fun(*this, &Menu::swines_announcement_event), p));
          } // swines
          { // hyperswines
            this->hyperswines_announcement.push_back(Gtk::manage(new Gtk::MenuItem("hyperswines")));
            this->ui->translations->add(*(dynamic_cast<Gtk::Label*>(this->hyperswines_announcement.back()->get_child())),
                                        ::translation("Menu::Item::hyperswines announcement"));
            this->announcements_player_menu.back()->append(*(this->hyperswines_announcement.back()));

            this->hyperswines_announcement.back()->signal_activate().connect(sigc::bind<unsigned const>(sigc::mem_fun(*this, &Menu::hyperswines_announcement_event), p));
          } // hyperswines

          this->announcements_player_menu.back()->show_all_children();
          if ((this->ui->party().humanno() == 1)
              && (this->ui->party().player(p).type() == Player::HUMAN))
            menu = this->announcements_player_menu.back();
          else
            this->announcements_player.back()->set_submenu(*(this->announcements_player_menu.back()));

          if ((this->ui->party().humanno() != 1)
              && (this->ui->party().player(p).type() == Player::HUMAN))
            menu->append(*(this->announcements_player.back()));
        } // for (p < this->ui->party().playerno())
        this->announcements->set_submenu(*menu);
        menu->show_all_children();
      } // create the announcements menu

      return ;
    } // void Menu::party_start()

  /**
   ** the party is finished
   **
   ** @param     -
   **
   ** @return    -
   **/
  void
    Menu::party_finish()
    {
      this->end_party->set_sensitive(false);

      return ;
    } // void Menu::party_finish()

  /**
   ** the party has closed
   **
   ** @param     -
   **
   ** @return    -
   **
   ** @todo	better signal disconnection
   **/
  void
    Menu::party_close()
    {
      // ToDo: disconnection without extra variables
      this->party_points_present_connection.disconnect();

      this->party_points->set_sensitive(false);
      this->information->set_sensitive(false);
      this->last_trick->set_sensitive(false);
      this->game_debug->set_sensitive(false);

      return ;
    } // void Menu::party_close()

  /**
   ** the game has started
   ** make the announcements menu accessable
   **
   ** @param     -
   **
   ** @return    -
   **/
  void
    Menu::game_start()
    {
      this->announcements->set_sensitive(this->ui->game().number_of_humans() > 0);
      this->game_debug->set_sensitive(true);

      // to have 're'/'contra' instead of 'no 120'
      this->language_update();

      return ;
    } // void Menu::game_start()

  /**
   ** a new trick is opened
   **
   ** @param     -
   **
   ** @return    -
   **/
  void
    Menu::trick_open()
    {
      this->last_trick->set_sensitive(this->ui->game().real_trick_current_no() >= 1);
      this->announcements_update();

      return ;
    } // void Menu::trick_open()

  /**
   ** the (human) player has to play a card
   **
   ** @param     -
   **
   ** @return    -
   **/
  void
    Menu::card_get()
    {
      this->card_suggestion->set_sensitive(this->ui->game().rule()(Rule::CARD_HINT));

      return ;
    } // void Menu::card_get()

  /**
   ** the (human) player has played a card
   **
   ** @param     -
   **
   ** @return    -
   **/
  void
    Menu::card_got()
    {
      this->card_suggestion->set_sensitive(false);

      return ;
    } // void Menu::card_got()

  /**
   ** the game is finished
   **
   ** @param     -
   **
   ** @return    -
   **/
  void
    Menu::game_finish()
    {
      this->last_trick->set_sensitive(false);

      return ;
    } // void Menu::game_finish()

  /**
   ** the game is closed
   **
   ** @param     -
   **
   ** @return    -
   **/
  void
    Menu::game_close()
    {
      this->game_debug->set_sensitive(false);

      return ;
    } // void Menu::game_close()

  /**
   ** updates the announcements, that is the sensitivity of them
   **
   ** @param     -
   **
   ** @return    -
   **/
  void
    Menu::announcements_update()
    {
      bool one_valid = false;
      for (unsigned p = 0; p < this->ui->game().number_of_players(); p++) {
        Player const& player = this->ui->game().player(p);
        bool valid = false;

        Announcement const reply
          = this->ui->game().announcement_valid(ANNOUNCEMENT::REPLY,
                                                player);
        valid |= reply;
        if (   reply
            && !(   (reply == ANNOUNCEMENT::NO120) 
                 || (reply == ANNOUNCEMENT::REPLY) )
           ) {
          dynamic_cast<Gtk::Label*>(this->announcements_announcement[p][0]->get_child())->set_text(::translation(reply).translation());
          this->announcements_announcement[p][0]->set_sensitive(true);
        } else { // if !(REPLY valid)
          dynamic_cast<Gtk::Label*>(this->announcements_announcement[p][0]->get_child())->set_text(::translation(ANNOUNCEMENT::REPLY).translation());
          this->announcements_announcement[p][0]->set_sensitive(false);
        } // if !(REPLY valid)

        for (int a = ANNOUNCEMENT::NO120;
             a <= ANNOUNCEMENT::NO0;
             ++a) {
          bool const v
            = (this->ui->game().announcement_valid(static_cast<Announcement>(a),
                                                   this->ui->game().player(p)));
          this->announcements_announcement[p][a - ANNOUNCEMENT::NO120 + 1]->set_sensitive(v);
          valid |= v;
        } // for (a \in Announcement)
        { // swines
          valid |= (player.game().swines_announcement_valid(player)
                    || player.game().hyperswines_announcement_valid(player));
          if (player.game().rule()(Rule::SWINES)) {
            this->swines_separator[p]->show();
            this->swines_announcement[p]->show();
          } else {
            this->swines_separator[p]->hide();
            this->swines_announcement[p]->hide();
          }
          if (player.game().rule()(Rule::HYPERSWINES))
            this->hyperswines_announcement[p]->show();
          else
            this->hyperswines_announcement[p]->hide();

          this->swines_announcement[p]->set_sensitive(player.game().swines_announcement_valid(player));
          this->hyperswines_announcement[p]->set_sensitive(player.game().hyperswines_announcement_valid(player));
        } // swines

        this->announcements_player[p]->set_sensitive(valid);
        valid &= ((this->ui->game().player(p).type() == Player::HUMAN));
        one_valid |= valid;
      } // for (p < this->ui->game().number_of_players())
      this->announcements->set_sensitive(one_valid);

      return ;
    } // void Menu::announcements_update()

  /**
   ** updates all language elements
   **
   ** @param     -
   **
   ** @return    -
   **
   ** @todo	'last trick' -> 'tricks' (when game is finished)
   ** @todo	change the name of the 'reply' item
   **/
  void
    Menu::language_update()
    {
      // update the announcements
      if (::game_status >= GAMESTATUS::GAME_INIT)
        for (unsigned p = 0; p < this->ui->game().number_of_players(); ++p) {
          dynamic_cast<Gtk::Label*>(this->announcements_announcement[p][0]->get_child()
                                   )->set_text(::translation(ANNOUNCEMENT::REPLY).translation());
          for (int a = ANNOUNCEMENT::NO120; a <= ANNOUNCEMENT::NO0; ++a)
            // if the team is 're' or 'contra'
            // take 're'/'contra' instead of 'no 120'
            if ( (a == ANNOUNCEMENT::NO120)
                && !this->ui->game().rule()(Rule::KNOCKING)
                && ((this->ui->game().player(p).team() == TEAM::RE)
                    || (this->ui->game().player(p).team() == TEAM::CONTRA)) )
              dynamic_cast<Gtk::Label*>(this->announcements_announcement[p][a - ANNOUNCEMENT::NO120 + 1]->get_child()
                                       )->set_text(::translation(this->ui->game().player(p).team()).translation());
            else
              dynamic_cast<Gtk::Label*>(this->announcements_announcement[p][a - ANNOUNCEMENT::NO120 + 1]->get_child()
                                       )->set_text(::translation(Announcement(a)).translation());
        } // for (p < this->ui->game().number_of_players())

      return ;
    } // void Menu::language_update()

  /**
   ** loads the party
   ** The filename is taken from 'load_party_file_chooser'.
   **
   ** @param     -
   **
   ** @return    -
   **/
  void
    Menu::load_party_event()
    {
      string const filename = this->load_party_file_chooser->get_filename();
      if (this->ui->party().load(filename))
        this->ui->thrower(GAMESTATUS::PARTY_LOADED,
                          __FILE__, __LINE__);

      return ;
    } // void Menu::load_party_event()

  /**
   ** saves the party
   ** The filename is taken from 'save_party_file_chooser'.
   **
   ** @param     -
   **
   ** @return    -
   **/
  void
    Menu::save_party_event()
    {
      auto const filename = this->save_party_file_chooser->get_filename();
      if (!this->ui->party().save(filename))
        this->ui->information(::translation("save party failed: %sfilename%",
                                            filename),
                              INFORMATION::PROBLEM);

      return ;
    } // void Menu::save_party_event()

  /**
   ** update the sensitivity of the themes
   **
   ** @param     -
   **
   ** @return    -
   **/
  void
    Menu::update_themes_sensitivity()
    {
      for (auto& item : this->themes_menu->get_children())
        if (item->get_data("theme"))
          item->set_sensitive(static_cast<Setting::Theme*>(item->get_data("theme"))->can_be_applied());
      return ;
    } // void Menu::update_theme_sensitivity()

  /**
   ** apply the theme
   **
   ** @param     theme   theme to apply
   **
   ** @return    -
   **/
  void
    Menu::apply_theme_event(Setting::Theme const* const theme)
    {
      if (theme)
        ::setting.set(*theme);
      return ;
    } // void Menu::save_party_event(Setting::Theme* theme)

} // namespace UI_GTKMM_NS

#endif // #ifdef USE_UI_GTKMM
