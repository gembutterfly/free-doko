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

#include "party_settings.h"
#include "players.h"
#include "rules.h"

#include "ui.h"
#include "translations.h"
#include "main_window.h"
#include "bug_report.h"
#ifdef USE_NETWORK
#include "network.h"
#ifdef USE_NETWORK_DOKOLOUNGE
#include "dokolounge/lounge.h"
#endif
#endif
#include "first_run.h"
#include "program_updated.h"

#include "../../party/party.h"
#include "../../player/player.h"
#include "../../player/aiconfig.h"
#ifdef USE_NETWORK
#include "../../network/server.h"
#endif
#include "../../os/bug_report_replay.h"
#include "../../misc/setting.h"

#include "widgets/stock_button.h"
#include <gtkmm/stock.h>
#include <gtkmm/radiobutton.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm/alignment.h>
#include <gtkmm/frame.h>
#include <gtkmm/main.h>
#include <gtkmm/table.h>
#include <gtkmm/filechooserdialog.h>
namespace UI_GTKMM_NS {

  /**
   ** constructor
   **
   ** @param     parent   the parent object
   **
   ** @return    -
   **/
  PartySettings::PartySettings(Base* const parent) :
    Base(parent),
    StickyDialog("PartySettings", *(parent->ui->main_window), false)
  {
    this->ui->add_window(*this);

    this->init();
#if 0
    this->signal_realize().connect(sigc::mem_fun(*this,
					      &PartySettings::language_update));
#endif

    this->signal_show().connect(sigc::mem_fun(*this, &PartySettings::update));
    this->signal_key_press_event().connect(sigc::mem_fun(*this,
						      &PartySettings::key_press));

    this->ui->bug_report->set_dnd_destination(*this);

    return ;
  } // PartySettings::PartySettings(Base* parent)

  /**
   ** destruktor
   **
   ** @param     -
   **
   ** @return    -
   **/
  PartySettings::~PartySettings()
  {
    delete this->players;
    delete this->rules;

    return ;
  } // PartySettings::~PartySettings()

  /**
   ** create all subelements
   **
   ** @param     -
   **
   ** @return    -
   **/
  void
    PartySettings::init()
    {
#ifdef POSTPONED
      auto& party = this->ui->party();
#else
      auto& party = ::party;
#endif

      this->set_icon(this->ui->icon);
      this->ui->translations->add(*this, ::translation("~party settings"));

      this->set_position(Gtk::WIN_POS_CENTER_ON_PARENT);

      this->players = new Players(this);
      this->rules = new Rules(this);

      this->load_bug_report_button
	= Gtk::manage(new Gtk::StockButton(Gtk::Stock::OPEN,
					   "load bug report"));
      this->ui->translations->add(*this->load_bug_report_button->label,
				  ::translation("load bug report"));
#ifndef RELEASE
      this->add_action_widget(*this->load_bug_report_button,
			      Gtk::RESPONSE_NONE);
#endif
      this->start_party_button
	= Gtk::manage(new Gtk::StockButton(Gtk::Stock::OK,
					   "start party"));
      this->ui->translations->add(*this->start_party_button->label,
				  ::translation("start party"));
      this->add_action_widget(*this->start_party_button, Gtk::RESPONSE_CLOSE);
      this->close_button
	= Gtk::manage(new Gtk::StockButton(Gtk::Stock::CLOSE,
					   "close"));
      this->ui->translations->add(*(this->close_button->label),
				  ::translation("close"));
      this->add_action_widget(*(this->close_button), Gtk::RESPONSE_CLOSE);

      { // seed
        this->seed_value = Gtk::manage(new Gtk::SpinButton(1, 0));
        this->seed_value->set_value(0);
        this->seed_value->set_range(0, PSEUDO_RAND_MAX - 1);
        this->seed_value->set_increments(1, 1000);
        this->seed_random = Gtk::manage(new Gtk::CheckButton("random"));
        this->ui->translations->add(*(static_cast<Gtk::Label*>(this->seed_random->get_child())),
                                    ::translation("random"));
      } // seed
      { // duration
        this->rule_number_of_rounds_limited
          = Gtk::manage(new Gtk::CheckButton("number of rounds limited"));
        this->ui->translations->add(*this->rule_number_of_rounds_limited,
                                    ::translation(Rule::NUMBER_OF_ROUNDS) + ":");
        this->rule_number_of_rounds
          = Gtk::manage(new Gtk::SpinButton());
        this->rule_number_of_rounds->set_increments(1, 10);
        this->rule_points_limited
          = Gtk::manage(new Gtk::CheckButton("points limited"));
        this->ui->translations->add(*this->rule_points_limited,
                                    ::translation(Rule::POINTS) + ":");
        this->rule_points
          = Gtk::manage(new Gtk::SpinButton());
        this->rule_points->set_increments(1, 10);
      } // duration
      { // startplayer
        this->startplayer_random = Gtk::manage(new Gtk::RadioButton("random"));
        this->ui->translations->add(*(static_cast<Gtk::Label*>(this->startplayer_random->get_child())),
                                    ::translation("random"));
      } // startplayer

      this->configure_players
        = Gtk::manage(new Gtk::StockButton("configure players"));
      this->ui->translations->add(*this->configure_players,
                                  ::translation("players"));
      this->configure_rules
        = Gtk::manage(new Gtk::StockButton(Gtk::Stock::PROPERTIES,
                                           "configure rules"));
      this->ui->translations->add(*this->configure_rules,
                                  ::translation("rules"));
#ifdef USE_NETWORK
#if GTKMM_VERSION_GE(2,8)
      this->configure_network
        = Gtk::manage(new Gtk::StockButton(Gtk::Stock::NETWORK,
                                           "configure network"));
#else
      this->configure_network
        = Gtk::manage(new Gtk::StockButton("configure network"));
#endif
      this->ui->translations->add(*this->configure_network,
                                  ::translation("network"));
#ifdef USE_NETWORK_DOKOLOUNGE
      this->dokolounge
        = Gtk::manage(new Gtk::StockButton("DokoLounge"));
      this->ui->translations->add(*this->dokolounge,
                                  ::translation("DokoLounge"));
#endif
#endif


      { // create the startplayer buttons
        auto startplayer_group
          = this->startplayer_random->get_group();
        for (unsigned p = 0; p < party.playerno(); p++)
          this->startplayer.push_back(Gtk::manage(new Gtk::RadioButton(startplayer_group,
                                                                       party.player(p).name()))
                                     );
        { // the switch buttons
#ifdef WORKAROUND
        // MS-Windows does not show the arrows
#include "arrows/arrow_lu.xpm"
#include "arrows/arrow_ur.xpm"
#include "arrows/arrow_rd.xpm"
#include "arrows/arrow_dl.xpm"
          this->switch_players_buttons.push_back(Gtk::manage(new Gtk::Button()));
          this->switch_players_buttons.back()->add(*Gtk::manage(new Gtk::Image(Gdk::Pixbuf::create_from_xpm_data(arrow_lu_xpm))));
          this->switch_players_buttons.push_back(Gtk::manage(new Gtk::Button()));
          this->switch_players_buttons.back()->add(*Gtk::manage(new Gtk::Image(Gdk::Pixbuf::create_from_xpm_data(arrow_ur_xpm))));
          this->switch_players_buttons.push_back(Gtk::manage(new Gtk::Button()));
          this->switch_players_buttons.back()->add(*Gtk::manage(new Gtk::Image(Gdk::Pixbuf::create_from_xpm_data(arrow_rd_xpm))));
          this->switch_players_buttons.push_back(Gtk::manage(new Gtk::Button()));
          this->switch_players_buttons.back()->add(*Gtk::manage(new Gtk::Image(Gdk::Pixbuf::create_from_xpm_data(arrow_dl_xpm))));

          for (auto& b : this->switch_players_buttons)
            b->set_relief(Gtk::RELIEF_NONE);
#else
        this->switch_players_buttons.push_back(Gtk::manage(new Gtk::Button(Glib::ustring(1, static_cast<gunichar>(0x2196))))); // up left
        this->switch_players_buttons.push_back(Gtk::manage(new Gtk::Button(Glib::ustring(1, static_cast<gunichar>(0x2197))))); // up right
        this->switch_players_buttons.push_back(Gtk::manage(new Gtk::Button(Glib::ustring(1, static_cast<gunichar>(0x2199))))); // down right
        this->switch_players_buttons.push_back(Gtk::manage(new Gtk::Button(Glib::ustring(1, static_cast<gunichar>(0x2198))))); // down left
#endif
        } // the switch buttons
      } // create the startplayer buttons
      { // Layout
        auto hbox = Gtk::manage(new Gtk::HBox(false, 1 EM));
        hbox->set_border_width(1 EM);
        this->get_vbox()->add(*hbox);
        { // actions
          { // configure
            auto alignment
              = Gtk::manage(new Gtk::Alignment(Gtk::ALIGN_CENTER,
                                               Gtk::ALIGN_CENTER,
                                               1, 0));
            hbox->pack_end(*alignment, false, true);

            auto vbox = Gtk::manage(new Gtk::VBox(true, 1 EX));
            alignment->add(*vbox);

            vbox->add(*this->configure_rules);
            vbox->add(*this->configure_players);
#ifdef USE_NETWORK
            vbox->add(*Gtk::manage(new Gtk::Alignment(Gtk::ALIGN_CENTER,
                                                      Gtk::ALIGN_CENTER,
                                                      1, 0)));
            vbox->add(*this->configure_network);
#ifdef USE_NETWORK_DOKOLOUNGE
            vbox->add(*this->dokolounge);
#endif
#endif
          } // configure

        } // actions

        { // settings
          auto alignment
            = Gtk::manage(new Gtk::Alignment(Gtk::ALIGN_CENTER,
                                             Gtk::ALIGN_CENTER,
                                             0, 0));
          hbox->pack_start(*alignment, true, true);

          auto vbox = Gtk::manage(new Gtk::VBox(false, 1 EX));
          alignment->add(*vbox);

          { // seed
            this->seed_frame = Gtk::manage(new Gtk::Frame("card distribution"));
            this->ui->translations->add(*(dynamic_cast<Gtk::Label*>(this->seed_frame->get_label_widget())),
                                        ::translation("card distribution"));
            vbox->add(*this->seed_frame);

            auto alignment
              = Gtk::manage(new Gtk::Alignment(Gtk::ALIGN_CENTER,
                                               Gtk::ALIGN_CENTER,
                                               0, 0));
            seed_frame->add(*alignment);
            auto table = Gtk::manage(new Gtk::Table(2, 2, false));
            alignment->add(*table);

            table->set_border_width(2 EX);
            table->set_spacings(1 EX);

            auto label = Gtk::manage(new Gtk::Label("seed"));
            this->ui->translations->add(*label,
                                        ::translation("seed") + ":");
            table->attach(*label, 0, 1, 0, 1);
            table->attach(*this->seed_value, 1, 2, 0, 1);
            table->attach(*this->seed_random, 1, 2, 1, 2);
          } // seed

          { // duration
            auto duration_frame = Gtk::manage(new Gtk::Frame("game duration"));
            this->ui->translations->add(*dynamic_cast<Gtk::Label*>(duration_frame->get_label_widget()),
                                        ::translation("game duration"));
            vbox->add(*duration_frame);
            auto alignment
              = Gtk::manage(new Gtk::Alignment(Gtk::ALIGN_CENTER,
                                               Gtk::ALIGN_CENTER,
                                               0, 0));
            duration_frame->add(*alignment);

            auto table = Gtk::manage(new Gtk::Table(2, 2, false));
            alignment->add(*table);
            table->set_border_width(2 EX);
            table->set_spacings(1 EX);

            table->attach(*this->rule_number_of_rounds_limited, 0, 1, 0, 1);
            table->attach(*this->rule_number_of_rounds, 1, 2, 0, 1);
            table->attach(*this->rule_points_limited, 0, 1, 1, 2);
            table->attach(*this->rule_points, 1, 2, 1, 2);
          } // duration

          { // startplayer
            this->startplayer_frame
              = Gtk::manage(new Gtk::Frame("~startplayer"));
            this->ui->translations->add(*(dynamic_cast<Gtk::Label*>(this->startplayer_frame->get_label_widget())),
                                        ::translation("startplayer"));
            vbox->add(*this->startplayer_frame);

            auto startplayer_table
              = Gtk::manage(new Gtk::Table(3, 3, true));
            this->startplayer_frame->add(*startplayer_table);
            startplayer_table->set_border_width(2 EX);
            startplayer_table->set_spacings(1 EX);
            startplayer_table->attach(*this->startplayer_random, 1, 2, 1, 2);
            startplayer_table->attach(*this->startplayer[0], 1, 2, 2, 3);
            startplayer_table->attach(*this->startplayer[1], 0, 1, 1, 2);
            startplayer_table->attach(*this->startplayer[2], 1, 2, 0, 1);
            startplayer_table->attach(*this->startplayer[3], 2, 3, 1, 2);
            startplayer_table->attach(*this->switch_players_buttons[0],
                                      0, 1, 2, 3);
            startplayer_table->attach(*this->switch_players_buttons[1],
                                      0, 1, 0, 1);
            startplayer_table->attach(*this->switch_players_buttons[2],
                                      2, 3, 0, 1);
            startplayer_table->attach(*this->switch_players_buttons[3],
                                      2, 3, 2, 3);
          } // startplayer
        } // settings

      } // Layout

      { // signals
        this->load_bug_report_button->signal_clicked().connect(sigc::mem_fun(*this,
                                                                             &PartySettings::load_bug_report)
                                                              );
        this->start_party_button->signal_clicked().connect(sigc::mem_fun(*this,
                                                                         &PartySettings::start_party_event)
                                                          );
        this->close_button->signal_clicked().connect(sigc::mem_fun(*this,
                                                                   &Gtk::Widget::hide)
                                                    );

        this->seed_value->signal_value_changed().connect(sigc::mem_fun(*this, &PartySettings::seed_change_event));
        this->seed_random->signal_toggled().connect(sigc::mem_fun(*this, &PartySettings::seed_change_event));
        this->seed_random->signal_toggled().connect(sigc::mem_fun(*this,
                                                                  &PartySettings::seed_value_sensitivity_update)
                                                   );

        this->rule_number_of_rounds_limited->signal_toggled().connect(sigc::bind<int const>(sigc::mem_fun(*this, &PartySettings::rule_change), Rule::NUMBER_OF_ROUNDS_LIMITED));
        this->rule_number_of_rounds->signal_value_changed().connect(sigc::bind<int const>(sigc::mem_fun(*this, &PartySettings::rule_change), Rule::NUMBER_OF_ROUNDS));
        this->rule_points_limited->signal_toggled().connect(sigc::bind<int const>(sigc::mem_fun(*this, &PartySettings::rule_change), Rule::POINTS_LIMITED));
        this->rule_points->signal_value_changed().connect(sigc::bind<int const>(sigc::mem_fun(*this, &PartySettings::rule_change), Rule::POINTS));

        this->startplayer_random->signal_toggled().connect(sigc::mem_fun(*this, &PartySettings::startplayer_change_event));
        for (auto& p : this->startplayer)
          p->signal_toggled().connect(sigc::mem_fun(*this, &PartySettings::startplayer_change_event));

        for (unsigned p = 0; p < this->switch_players_buttons.size(); ++p)
          this->switch_players_buttons[p]->signal_clicked().connect(sigc::bind<unsigned const>(sigc::mem_fun(*this, &PartySettings::switch_players_event), p));

        this->signal_hide().connect(sigc::mem_fun(*this->ui->bug_report->load_file_chooser,
                                                  &Gtk::Widget::hide));

        this->configure_players->signal_clicked().connect(sigc::mem_fun0(*this->players,
                                                                         &Gtk::Window::present)
                                                         );
        this->configure_rules->signal_clicked().connect(sigc::mem_fun0(*this->rules,
                                                                       &Gtk::Window::present)
                                                       );
#ifdef USE_NETWORK
        this->configure_network->signal_clicked().connect(sigc::mem_fun0(*this->ui->network,
                                                                         &Gtk::Window::present)
                                                         );
#ifndef OUTDATED
        // remove, when the network is (mostly) stable
        this->configure_network->signal_clicked().connect(sigc::mem_fun(*this,
                                                                        &PartySettings::show_network_unstable_warning)
                                                         );
#endif

#ifdef USE_NETWORK_DOKOLOUNGE
        this->dokolounge->signal_clicked().connect(sigc::mem_fun0(*this->ui->lounge,
                                                                  &Gtk::Window::present)
                                                  );
#endif
#endif
      } // signals

      this->show_all_children();
      this->sensitivity_update();
      this->update();

      return ;
    } // void PartySettings::init()

  /**
   ** get the party settings
   **
   ** @param     -
   **
   ** @return    -
   **/
  void
    PartySettings::get()
    {
      auto& party = this->ui->party();
      this->update();

      this->players->create_backup();
      this->rules->create_backup();

      this->start_party_button->show();
      this->start_party_button->grab_default();
      this->start_party_button->grab_focus();

      this->present();

      while (   !this->ui->thrower
             && this->is_visible()
             && (::game_status == GAMESTATUS::PARTY_NEW)) {
        ::ui->wait();
        if (::bug_report_replay
            && ::bug_report_replay->auto_start_party())
          break;
      }

      if (::game_status == GAMESTATUS::PARTY_NEW) {
#ifdef NETWORK
        if (!::server->has_parent_connection()) {
#endif
          // set the seed
          if (this->seed_random->get_active())
            party.set_random_seed();
          else
            party.set_seed(this->seed_value->get_value_as_int());

          // set the startplayer
          if (this->startplayer_random->get_active())
            party.set_random_startplayer();
          else
            for (unsigned p = 0; p < party.playerno(); ++p)
              if (this->startplayer[p]->get_active()) {
                party.set_startplayer(p);
                break;
              }

          this->players->create_backup();
          this->rules->create_backup();
#ifdef NETWORK
        }
#endif
      } // if (::game_status == GAMESTATUS::PARTY_NEW)

      this->hide();

      return ;
    } // void PartySettings::get()

  /**
   ** show the window.
   ** Shows/hides the action buttons
   **
   ** @param     -
   **
   ** @return    -
   **/
  void
    PartySettings::on_show()
    {
#ifdef GLIBMM_DEFAULT_SIGNAL_HANDLERS_ENABLED
      this->Dialog::on_show();
#endif

      if (this->ui->first_run_window)
        this->ui->first_run_window->raise();
      if (this->ui->program_updated_window)
        this->ui->program_updated_window->raise();

      this->update();

      return ;
    } // void PartySettings::show()

  /**
   ** load the bug report
   **
   ** @param     -
   **
   ** @return    -
   **/
  void
    PartySettings::load_bug_report()
    {
      this->ui->bug_report->load_file_chooser->present();

      return ;
    } // void PartySettings::load_bug_report()

  /**
   ** update the sensitivity of all elements
   **
   ** @param     -
   **
   ** @return    -
   **/
  void
    PartySettings::sensitivity_update()
    {
      auto sensitive = (::game_status == GAMESTATUS::PARTY_NEW);
#ifdef USE_NETWORK
      if (   ::server
          && ::server->has_parent_connection())
        sensitive = false;
#endif
      if (sensitive)
        this->seed_value_sensitivity_update();
      else
        this->seed_value->set_sensitive(false);

      this->seed_random->set_sensitive(sensitive);

      this->rule_number_of_rounds_limited->set_sensitive(sensitive);
      this->rule_number_of_rounds->set_sensitive(sensitive);
      this->rule_points_limited->set_sensitive(sensitive);
      this->rule_points->set_sensitive(sensitive);

      for (auto& p : this->startplayer)
        p->set_sensitive(sensitive);
      this->startplayer_random->set_sensitive(sensitive);
      for (auto p : this->switch_players_buttons)
        p->set_sensitive(sensitive);

      this->players->sensitivity_update();
      this->rules->sensitivity_update();

      this->load_bug_report_button->set_sensitive(sensitive);
      this->start_party_button->set_sensitive(sensitive);

      if (::setting(Setting::ADDITIONAL_PARTY_SETTINGS)) {
        this->seed_frame->show();
        this->startplayer_frame->show();
      } else {
        this->seed_frame->hide();
        this->startplayer_frame->hide();
      }

      return ;
    } // void PartySettings::sensitivity_update()

  /**
   ** update the sensitivity of 'seed_value'
   **
   ** @param     -
   **
   ** @return    -
   **/
  void
    PartySettings::seed_value_sensitivity_update()
    {
      switch (::game_status) {
      case GAMESTATUS::PARTY_NEW:
      case GAMESTATUS::PARTY_INITIAL_LOADED:
        this->seed_value->set_sensitive(!(this->seed_random->get_active()));
        break;
      default:
        this->seed_value->set_sensitive(false);
        break;
      } // switch (::game_status);

      return ;
    } // void PartySettings::seed_value_sensitivity_update()

  /**
   ** update the widgets
   **
   ** @param     -
   **
   ** @return    -
   **/
  void
    PartySettings::update()
    {
      if (!this->is_visible())
        return ;

      auto& party = this->ui->party();
      this->sensitivity_update();

      switch (::game_status) {
      case GAMESTATUS::PARTY_NEW:
#ifndef RELEASE
        this->load_bug_report_button->show();
#endif
        this->start_party_button->show();
        this->close_button->hide();

        this->start_party_button->grab_default();
        break;
      default:
        this->load_bug_report_button->hide();
        this->start_party_button->hide();
        this->close_button->show();

        this->close_button->grab_default();
        break;
      } // switch (::game_status)

      auto const random_seed = (party.seed_first() == UINT_MAX);
      if (!random_seed)
        this->seed_value->set_value(party.seed());
      this->seed_random->set_active(random_seed);

      this->rules_update();

      if (party.startplayer() == UINT_MAX)
        this->startplayer_random->set_active(true);
      else
        this->startplayer[party.startplayer()]->set_active(true);

      for (unsigned p = 0; p < party.playerno(); ++p) {
        this->name_update_local(party.player(p));
      }
      this->players->update();
      this->rules->update_all();

      return ;
    } // void PartySettings::update()

  /**
   ** update the rules (value, sensitivity)
   **
   ** @param     -
   **
   ** @return    -
   **/
  void
    PartySettings::rules_update()
    {
      auto const& rule = ::party.rule();

      this->rule_number_of_rounds_limited->set_active(rule(Rule::NUMBER_OF_ROUNDS_LIMITED));
      this->rule_number_of_rounds_limited->set_sensitive(rule.dependencies(Rule::NUMBER_OF_ROUNDS_LIMITED));
      this->rule_number_of_rounds->set_value(rule(Rule::NUMBER_OF_ROUNDS));
      this->rule_number_of_rounds->set_sensitive(rule(Rule::NUMBER_OF_ROUNDS_LIMITED));
      this->rule_number_of_rounds->set_value(rule(Rule::NUMBER_OF_ROUNDS));
      {
        auto const value = rule(Rule::NUMBER_OF_ROUNDS);
        this->rule_number_of_rounds->set_range(rule.min(Rule::NUMBER_OF_ROUNDS),
                                               rule.max(Rule::NUMBER_OF_ROUNDS));
        this->rule_number_of_rounds->set_value(value);
      }

      this->rule_points_limited->set_active(rule(Rule::POINTS_LIMITED));
      this->rule_points_limited->set_sensitive(rule.dependencies(Rule::POINTS_LIMITED));
      this->rule_points->set_sensitive(true);
      this->rule_points->set_value(rule(Rule::POINTS));
      this->rule_points->set_sensitive(rule(Rule::POINTS_LIMITED));
      {
        auto const value = rule(Rule::POINTS);
        this->rule_points->set_range(rule.min(Rule::POINTS),
                                     rule.max(Rule::POINTS));
        this->rule_points->set_value(value);
      }

      return ;
    } // void PartySettings::rules_update()

  /**
   ** the rules have been changed
   **
   ** @param     type        the type of the rule that has changed
   **
   ** @return    -
   **/
  void
    PartySettings::rule_change(int const type)
    {
      auto& rule = ::party.rule();
      switch(type) {
      case Rule::NUMBER_OF_ROUNDS_LIMITED:
        rule.set(Rule::NUMBER_OF_ROUNDS_LIMITED,
                 this->rule_number_of_rounds_limited->get_active());
        break;
      case Rule::NUMBER_OF_ROUNDS:
        rule.set(Rule::NUMBER_OF_ROUNDS,
                 this->rule_number_of_rounds->get_value_as_int());
        break;
      case Rule::POINTS_LIMITED:
        rule.set(Rule::POINTS_LIMITED,
                 this->rule_points_limited->get_active());
        break;
      case Rule::POINTS:
        rule.set(Rule::POINTS,
                 this->rule_points->get_value_as_int());
        break;
      default:
        break;
      } // switch(type)

      return ;
    } // void PartySettings::rule_change(int type)

  /**
   ** the players 'player_a' and 'player_b' have been switched
   **
   ** @param     player_a   first player
   ** @param     player_b   second player
   **
   ** @return    -
   **/
  void
    PartySettings::players_switched(Player const& player_a,
                                    Player const& player_b)
    {
      this->name_update_local(player_a);
      this->name_update_local(player_b);

      this->players->players_switched(player_a, player_b);

      return ;
    } // void PartySettings::players_switched(Player player_a, Player player_b)

  /**
   ** update 'player'
   **
   ** @param     player   player to update
   **
   ** @return    -
   **/
  void
    PartySettings::player_update(Player const& player)
    {
      if (::game_status <= GAMESTATUS::PARTY_NEW)
        return ;

      this->name_update_local(player);
      this->players->player_update(player);

      return ;
    } // void PartySettings::player_update(Player player)

  /**
   ** update the name of 'player'
   **
   ** @param     player   player with the new name
   **
   ** @return    -
   **/
  void
    PartySettings::name_update(Player const& player)
    {
      this->name_update_local(player);
      this->players->name_update(player);
    } // void PartySettings::name_update(Player const& player)

  /**
   ** update the name of 'player'
   **
   ** @param     player   player with the new name
   **
   ** @return    -
   **/
  void
    PartySettings::name_update_local(Player const& player)
    {
      Pango::FontDescription fd;
      fd.set_weight(player.type() == Player::HUMAN
                    ? Pango::WEIGHT_BOLD
                    : Pango::WEIGHT_NORMAL);
      fd.set_style(player.type() == Player::NETWORK
                   ? Pango::STYLE_ITALIC
                   : Pango::STYLE_NORMAL);
      auto const playerno = this->ui->party().no(player);
      DEBUG_ASSERTION((playerno < UINT_MAX),
                      "PlayerSettings::name_update_local(player)\n"
                      "  player '" << player.name() << "' not found in the party");
      if (playerno >= 4)
        return ;

      this->startplayer[playerno]->get_child()->override_font(fd);

      this->startplayer[playerno]->set_label(player.name());

      return ;
    } // void PartySettings::name_update_local(Player player)

  /**
   ** update the voice of 'player'
   **
   ** @param     player   player with the new voice
   **
   ** @return    -
   **/
  void
    PartySettings::voice_update(Player const& player)
    {
      this->players->voice_update(player);

      return ;
    } // void PartySettings::voice_update(Player player)

  /**
   ** update the aiconfig
   **
   ** @param     aiconfig   changed aiconfig
   **
   ** @return    -
   **/
  void
    PartySettings::aiconfig_update(Aiconfig const& aiconfig)
    {
      this->players->aiconfig_update(aiconfig);

      return ;
    } // void PartySettings::aiconfig_update(Aiconfig aiconfig)

  /**
   ** a new party is started
   **
   ** @param     -
   **
   ** @return    -
   **/
  void
    PartySettings::start_party_event()
    {
      this->hide();
      ::game_status = GAMESTATUS::PARTY_NEW;

      return ;
    } // void PartySettings::start_party_event()

  /**
   ** the seed has changed
   **
   ** @param     -
   **
   ** @return    -
   **/
  void
    PartySettings::seed_change_event()
    {
      // set the seed
      if (this->seed_random->get_active())
        this->ui->party().set_random_seed();
      else
        this->ui->party().set_seed(this->seed_value->get_value_as_int());

      return ;
    } // void PartySettings::seed_change_event()

  /**
   ** the startplayer has changed
   **
   ** @param     -
   **
   ** @return    -
   **/
  void
    PartySettings::startplayer_change_event()
    {
      // set the startplayer
      if (this->startplayer_random->get_active())
        this->ui->party().set_random_startplayer();
      else
        for (unsigned p = 0; p < this->ui->party().playerno(); ++p)
          if (this->startplayer[p]->get_active()) {
            this->ui->party().set_startplayer(p);
            break;
          }

      return ;
    } // void PartySettings::startplayer_change_event()

  /**
   ** players shall be switched
   **
   ** @param     p   player to switch with the following
   **
   ** @return    -
   **/
  void
    PartySettings::switch_players_event(unsigned const p)
    {
      auto& party = ::party;

      party.switch_players(p, (p + 1) % party.playerno());

      return ;
    } // void PartySettings::switch_players_event(unsigned p)

  /**
   ** a key has been pressed
   **
   ** @param     key   the key
   **
   ** @return    whether the key was used
   **/
  bool
    PartySettings::key_press(GdkEventKey* const key)
    {
      bool used = false;

      if ((key->state & ~GDK_SHIFT_MASK) == 0) {
        switch (key->keyval) {
        case GDK_KEY_p: // show the players
          this->players->present();
          used = true;
          break;
        case GDK_KEY_r: // show the rules
          this->rules->present();
          used = true;
          break;
        } // switch (key->keyval)
      } // if ((key->state & ~GDK_SHIFT_MASK) == 0)

      return (used || this->ui->key_press(key));
    } // bool PartySettings::key_press(GdkEventKey* key)

#ifndef OUTDATED
  // remove, when the network is (mostly) stable
  /**
   ** show a warning, that the network is developement
   **
   ** @param     -
   **
   ** @return    -
   **/
  void
    PartySettings::show_network_unstable_warning()
    {
      static auto first_run = true;
#ifdef DKNOF
      first_run = false;
#endif

      if (!first_run)
        return ;
      first_run = false;

      this->ui->information(::translation("Warning::network unstable"),
                            INFORMATION::WARNING);

      return ;
    } // void PartySettings::show_network_unstable_warning()
#endif

} // namespace UI_GTKMM_NS

#endif // #ifdef USE_UI_GTKMM
