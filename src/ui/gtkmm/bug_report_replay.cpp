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

#include "bug_report_replay.h"
#include "gameplay_actions.h"

#include "ui.h"
#include "thrower.h"
#include "translations.h"
#include "bug_report.h"
#include "main_window.h"
#include "cards.h"

#include "../ui.wrap.h"
#include "../../os/bug_report_replay.h"
#include "../../player/player.h"
#include "../../player/aiconfig.h"

#include "../../utils/string.h"

#include "gameplay_actions.h"
#include "game_summary.h"
#include "widgets/stock_button.h"
#include <gtkmm/notebook.h>
#include <gtkmm/label.h>
#include <gtkmm/textview.h>
#include <gtkmm/stock.h>
#include <gtkmm/alignment.h>
#include <gtkmm/scrolledwindow.h>

namespace UI_GTKMM_NS {

/**
 ** Constructor
 **
 ** @param     parent   the parent widget
 **
 ** @return    -
 **/
BugReportReplay::BugReportReplay(Base* const parent) :
  Base(parent),
  StickyDialog("bug report replay", false),
  OS(OS_TYPE::UNNAMED)
{
  this->ui->add_window(*this);

  this->actions       = Gtk::manage(new GameplayActions(this));
  this->human_actions = Gtk::manage(new GameplayActions(this));
  this->game_summary  = Gtk::manage(new GameSummary(this));

  this->signal_realize().connect(sigc::mem_fun(*this, &BugReportReplay::init));

  this->ui->bug_report->set_dnd_destination(*this);

  static_cast<UI_Wrap*>(::ui)->append(this);

  return ;
} // BugReportReplay::BugReportReplay(Base* parent)

/**
 ** Destruktor
 **
 ** @param     -
 **
 ** @return    -
 **/
BugReportReplay::~BugReportReplay()
{
  static_cast<UI_Wrap*>(::ui)->remove(this);
  return ;
} // BugReportReplay::~BugReportReplay()

/**
 ** create all subelements
 **
 ** @param     -
 **
 ** @return    -
 **/
void
BugReportReplay::init()
{
  this->ui->translations->add(*this, ::translation("FreeDoko %ttitle%",
                                                   ::translation("~bug report replay")));

  this->set_icon(this->ui->icon);

  { // action area
    auto end_button
      = Gtk::manage(new Gtk::Button("end bug report"));
    this->ui->translations->add(*end_button,
                                ::translation("BugReport::end"));

    end_button->signal_clicked().connect(sigc::mem_fun(*this,
                                                       &BugReportReplay::end_bug_report));

    auto close_button
      = Gtk::manage(new Gtk::StockButton(Gtk::Stock::CLOSE, "close"));
    this->ui->translations->add(*close_button,
                                ::translation("close"));
    this->add_action_widget(*close_button, Gtk::RESPONSE_CLOSE);

    close_button->grab_default();
    close_button->signal_clicked().connect(sigc::mem_fun(*this,
                                                         &Gtk::Widget::hide));
  } // action area

  { // informations
    this->version = Gtk::manage(new Gtk::Label(""));
    this->ui->translations->add(*this->version, ::translation(""));
    this->version->set_justify(Gtk::JUSTIFY_LEFT);
    this->compiled = Gtk::manage(new Gtk::Label(""));
    this->ui->translations->add(*this->compiled, ::translation(""));
    this->compiled->set_justify(Gtk::JUSTIFY_LEFT);
    this->system = Gtk::manage(new Gtk::Label(""));
    this->ui->translations->add(*this->system, ::translation(""));
    this->system->set_justify(Gtk::JUSTIFY_LEFT);
    this->time = Gtk::manage(new Gtk::Label(""));
    this->ui->translations->add(*this->time, ::translation(""));
    this->time->set_justify(Gtk::JUSTIFY_LEFT);
    this->language = Gtk::manage(new Gtk::Label(""));
    this->ui->translations->add(*this->language, ::translation(""));
    this->language->set_justify(Gtk::JUSTIFY_LEFT);
    this->trick = Gtk::manage(new Gtk::Label(""));
    this->ui->translations->add(*this->trick, ::translation(""));
    this->trick->set_justify(Gtk::JUSTIFY_LEFT);
    this->reset_ais_button
      = Gtk::manage(new Gtk::StockButton(Gtk::Stock::CLOSE, "reset ais"));
    this->ui->translations->add(*this->reset_ais_button,
                                ::translation("reset ais"));
    this->message = Gtk::manage(new Gtk::TextView());
    this->message->get_buffer()->set_text("");
    this->message->set_wrap_mode(Gtk::WRAP_WORD);
#ifdef RELEASE
    this->message->set_editable(false);
    this->message->set_cursor_visible(false);
#else
    // Note: the changes in the message box are not saved.
#endif

    this->seed = Gtk::manage(new Gtk::Label(""));
    this->ui->translations->add(*this->seed, ::translation(""));
    this->seed->set_justify(Gtk::JUSTIFY_LEFT);

    this->startplayer = Gtk::manage(new Gtk::Label(""));
    this->ui->translations->add(*this->startplayer, ::translation(""));
    this->startplayer->set_justify(Gtk::JUSTIFY_LEFT);

    this->soloplayer = Gtk::manage(new Gtk::Label(""));
    this->ui->translations->add(*this->soloplayer, ::translation(""));
    this->soloplayer->set_justify(Gtk::JUSTIFY_LEFT);

    // hide the 'player' column
    this->human_actions->get_column(1)->set_visible(false);
  } // informations
  { // vbox
    this->notebook = Gtk::manage(new Gtk::Notebook());
    { // general
      auto label = Gtk::manage(new Gtk::Label("general"));
      this->ui->translations->add(*label, ::translation("BugReportReplay::Group::general"));

      auto general_box = Gtk::manage(new Gtk::VBox(false, 1 EM));
      general_box->set_border_width(1 EM);

      { // game box
        auto alignment
          = Gtk::manage(new Gtk::Alignment(Gtk::ALIGN_CENTER,
                                           Gtk::ALIGN_CENTER,
                                           0, 0));
        auto game_box = Gtk::manage(new Gtk::VBox(false, 1 EX));

        game_box->pack_start(*this->version, Gtk::PACK_EXPAND_WIDGET);
        game_box->pack_start(*this->system, Gtk::PACK_EXPAND_WIDGET);
        game_box->pack_start(*this->seed, Gtk::PACK_EXPAND_WIDGET);
        game_box->pack_start(*this->startplayer, Gtk::PACK_EXPAND_WIDGET);
        game_box->pack_start(*this->soloplayer, Gtk::PACK_EXPAND_WIDGET);
        game_box->pack_start(*this->trick, Gtk::PACK_EXPAND_WIDGET);
        game_box->pack_start(*this->reset_ais_button, Gtk::PACK_EXPAND_WIDGET);

        alignment->add(*game_box);
        general_box->pack_start(*alignment, false, true);
      } // game box
      { // message
        general_box->add(*this->message);
      } // message

      this->notebook->append_page(*general_box, *label);
    } // general
    { // actions
      auto label = Gtk::manage(new Gtk::Label("actions"));
      this->ui->translations->add(*label, ::translation("BugReportReplay::Group::actions"));

      auto vbox = Gtk::manage(new Gtk::VBox(false, 1 EX));

      auto scrolled_window = Gtk::manage(new Gtk::ScrolledWindow);
      scrolled_window->set_policy(Gtk::POLICY_AUTOMATIC,
                                  Gtk::POLICY_AUTOMATIC);

      this->actions->get_selection()->set_mode(Gtk::SELECTION_SINGLE);
      scrolled_window->add(*this->actions);

      vbox->add(*scrolled_window);

      this->auto_actions_button
        = Gtk::manage(new Gtk::Button("auto actions"));
      this->ui->translations->add(*this->auto_actions_button,
                                  ::translation("auto actions"));
      vbox->pack_end(*this->auto_actions_button, Gtk::PACK_SHRINK);

      this->notebook->append_page(*vbox, *label);
    } // actions
    { // human actions
      auto label = Gtk::manage(new Gtk::Label("human actions"));
      this->ui->translations->add(*label, ::translation("BugReportReplay::Group::human actions"));
      auto scrolled_window = Gtk::manage(new Gtk::ScrolledWindow);
      scrolled_window->set_policy(Gtk::POLICY_AUTOMATIC,
                                  Gtk::POLICY_AUTOMATIC);

      scrolled_window->add(*this->human_actions);

      this->notebook->append_page(*scrolled_window, *label);
    } // human actions
    { // game summary
      auto label = Gtk::manage(new Gtk::Label("game summary"));
      this->ui->translations->add(*label, ::translation("BugReportReplay::Group::game summary"));

      this->notebook->append_page(*this->game_summary, *label);
    } // game summary

    this->get_vbox()->pack_start(*this->notebook, true, true);
  } // vbox

  { // signals
    reset_ais_button->signal_clicked().connect(sigc::mem_fun(*this, &BugReportReplay::reset_ais));
    this->actions->get_selection()->signal_changed().connect(sigc::mem_fun(*this, &BugReportReplay::update_auto_actions_button));
    this->auto_actions_button->signal_clicked().connect(sigc::mem_fun(*this, &BugReportReplay::set_auto_actions));
    this->update_auto_actions_button();
  } // signals

  this->show_all_children();


  {
    Gdk::Geometry geometry;
    geometry.min_width = 3 * this->ui->cards->height();
    geometry.min_height = 4 * this->ui->cards->height();

    this->set_geometry_hints(*this, geometry, Gdk::HINT_MIN_SIZE);
  }

  return ;
} // void BugReportReplay::init()

/**
 ** a bug report replay is opened
 **
 ** @param     bug_report_replay   the bug report replay that is opened
 **
 ** @return    -
 **/
void
BugReportReplay::bug_report_replay_open(OS_NS::BugReportReplay const&
                                        bug_report_replay)
{
  DEBUG_ASSERTION((&bug_report_replay == ::bug_report_replay),
                  "BugReportReplay::bug_report_replay_open(bug_report_replay)\n"
                  "  the bug report replay "
                  " = " << &bug_report_replay
                  << " != " << ::bug_report_replay
                  << " = global one");
  this->bug_report_replay = ::bug_report_replay;

  if (!this->get_realized())
    this->realize();

  this->actions->set_actions(bug_report_replay.actions());
  this->actions->set_discrepancies(bug_report_replay.actions_discrepancies());
  this->human_actions->set_actions(bug_report_replay.human_actions());
  this->human_actions->set_discrepancies(bug_report_replay.human_actions_discrepancies());
  if (bug_report_replay.game_summary()) {
    this->game_summary->set_game_summary(*bug_report_replay.game_summary());
    this->game_summary->show();
  } else {
    this->game_summary->hide();
  }
  // ToDo: hide the last page if there is no game summary in the bug report

  this->update_info();
  this->notebook->set_current_page(0);

  if (!(FAST_PLAY & FAST_NS::HIDE_BUG_REPORT_WINDOW))
    this->show();

  return ;
} // void BugReportReplay::bug_report_replay_open(OS_NS::BugReportReplay bug_report_replay)

/**
 ** the bug report replay is closed
 **
 ** @param     -
 **
 ** @return    -
 **/
void
BugReportReplay::bug_report_replay_close()
{
  this->actions->remove_actions();
  this->human_actions->remove_actions();
  this->bug_report_replay = nullptr;
  this->hide();

  return ;
} // BugReportReplay::bug_report_replay_close()

/**
 ** update the information
 **
 ** @param     party   party that is opened
 **
 ** @return    -
 **/
void
BugReportReplay::party_open(Party const& party)
{
  this->OS::party_open(party);
  this->update_info();
  return ;
} // void BugReportReplay::party_open(Party party)

/**
 ** update the information
 **
 ** @param     -
 **
 ** @return    -
 **/
void
BugReportReplay::party_get_settings()
{
  this->update_info();
  return ;
} // void BugReportReplay::party_get_settings()

/**
 ** update the information
 **
 ** @param     game   game that is opened
 **
 ** @return    -
 **/
void
BugReportReplay::game_open(Game const& game)
{
  this->OS::game_open(game);
  this->update_info();
  return ;
} // void BugReportReplay::game_open(Game game)

/**
 ** update the information
 **
 ** @param     -
 **
 ** @return    -
 **/
void
BugReportReplay::game_redistribute()
{
  this->update_info();
  return ;
} // void BugReportReplay::game_redistribute()

/**
 ** there happened a gameplay action
 **
 ** @param     action   gameplay action
 **
 ** @return    -
 **/
void
BugReportReplay::gameplay_action(GameplayAction const& action)
{
  this->OS::gameplay_action(action);
  if (!this->bug_report_replay)
    return ;

  this->actions->set_current_action_no(this->bug_report_replay->current_action_no());
  this->human_actions->set_current_action_no(this->bug_report_replay->current_human_action_no());
  this->update_auto_actions_button();

  return ;
} // void BugReportReplay::gameplay_action(GameplayAction action)

/**
 ** updates the information
 **
 ** @param     -
 **
 ** @return    -
 **/
void
BugReportReplay::update_info()
{
  if (!this->bug_report_replay)
    return ;

  DEBUG_ASSERTION((this->bug_report_replay == ::bug_report_replay),
                  "BugReportReplay::update_info():\n"
                  "  this->bug_report_replay != ::bug_report_replay:\n"
                  "  " << this->bug_report_replay
                  << " != " << ::bug_report_replay);

  if (::game_status == GAMESTATUS::PROGRAMSTART)
    return ;
#if 0
  if (!&this->party())
    return ;
#endif


#ifdef WORKAROUND
  // This function can be called before this party is set
  // (by ::bug_report_replay). So we take a party here that always exists.
  // An alternitive would be to check, whether this party is set, but
  // 'OS::party_' is private.
  Party const& party = ::party;
#else
  Party const& party = this->party();
#endif

  this->ui->translations->change(*this->version,
                                 ::translation("Version: %sversion%",
                                               static_cast<string const>(this->bug_report_replay->version())));
  this->ui->translations->change(*this->compiled,
                                 ::translation("Compiled: %stime%",
                                               this->bug_report_replay->compiled()));
  this->ui->translations->change(*this->system,
                                 ::translation("System: %ssystem%",
                                               this->bug_report_replay->system()));
  this->ui->translations->change(*this->time,
                                 ::translation("Time: %stime%",
                                               this->bug_report_replay->time()));
  this->ui->translations->change(*this->language,
                                 ::translation("Language: %slanguage%",
                                               this->bug_report_replay->language()));
  this->ui->translations->change(*this->trick,
                                 ::translation("Trick: %utrick%",
                                               this->bug_report_replay->trickno()));

#ifdef WORKAROUND
  // message: change iso to utf8
  this->message->get_buffer()->set_text(UI_GTKMM::to_utf8(this->bug_report_replay->message()));
#else
  this->message->get_buffer()->set_text(this->bug_report_replay->message());
#endif


  this->seed->set_label(DK::Utils::String::to_string(this->bug_report_replay->seed()));

  this->startplayer->set_label(DK::Utils::String::to_string(this->bug_report_replay->startplayer_no()));
  this->ui->translations->change(*this->seed,
                                 ::translation("Seed: %useed%",
                                               this->bug_report_replay->seed()));

  if (this->bug_report_replay->startplayer_no() != UINT_MAX)
    this->ui->translations->change(*this->startplayer,
                                   ::translation("Startplayer: %ustartplayer%",
                                                 this->bug_report_replay->startplayer_no())
                                   + " (" + party.player(this->bug_report_replay->startplayer_no()).name() + ")"
                                  );
  else
    this->ui->translations->change(*this->startplayer,
                                   ::translation("Startplayer: %ustartplayer%",
                                                 this->bug_report_replay->startplayer_no())
                                   + " (-)"
                                  );

  if (this->bug_report_replay->soloplayer_no() != UINT_MAX)
    this->ui->translations->change(*this->soloplayer,
                                   ::translation("Soloplayer: %usoloplayer%",
                                                 this->bug_report_replay->soloplayer_no())
                                   + " (" + party.player(this->bug_report_replay->soloplayer_no()).name() + ")"
                                  );
  else
    this->ui->translations->change(*this->soloplayer,
                                   ::translation("Soloplayer: -"));

  this->actions->update();
  this->human_actions->update();

  if (this->bug_report_replay->game_summary())
    this->game_summary->update();

  this->update_actions_past();

  return ;
} // void BugReportReplay::update_info()

/**
 ** update the showing of the actions already made
 **
 ** @param     -
 **
 ** @return    -
 **
 ** @todo      all
 **/
void
BugReportReplay::update_actions_past()
{
  if (!this->get_realized())
    return ;

#if 0
  for (auto& row : this->actions->get_model()->children()) {
    for (auto col : this->actions->get_columns()) {
      // ToDo
    } // for (col \in this->actsion->get_columns())
  } // for (row \in this->actions)
#endif

  return ;
} // void BugReportReplay::update_actions_past()

/**
 ** update the auto actions button
 **
 ** @param     -
 **
 ** @return    -
 **/
void
BugReportReplay::update_auto_actions_button()
{
  if (!this->get_realized())
    return ;

  auto selection = this->actions->get_selection();

  if (selection->get_selected()) {
    auto row = *(selection->get_selected());

    unsigned const actionno = row[this->actions->model.no];

    if (actionno > this->bug_report_replay->current_action_no()) {
      this->auto_actions_button->set_sensitive(true);
    } else {
      this->auto_actions_button->set_sensitive(false);
    }
  } else {	// if !(selection->get_selected())
    this->auto_actions_button->set_sensitive(false);
  }	// if !(selection->get_selected())

  return ;
} // void BugReportReplay::update_auto_actions_button()

/**
 ** automatic execute till the selected action
 **
 ** @param     -
 **
 ** @return    -
 **/
void
BugReportReplay::set_auto_actions()
{
  auto selection = this->actions->get_selection();

  DEBUG_ASSERTION(selection->get_selected(),
                  "BugReportReplay::set_auto_actions()\n"
                  "  no action selected");
  auto row = *(selection->get_selected());

  unsigned const actionno = row[this->actions->model.no];

  this->bug_report_replay->set_auto_action_end(actionno);
  if (   (::game_status == GAMESTATUS::GAME_PLAY)
      && (this->ui->game().player_current().type() == Player::HUMAN) ) {
    auto& player = this->ui->game().player_current();
    player.hand().request_card(player.card_get());
    this->ui->thrower(player.hand().requested_card(), __FILE__, __LINE__);
  } // if (human has to play a card)

  return ;
} // void BugReportReplay::set_auto_actions()

/**
 ** end the bug report
 **
 ** @param     -
 **
 ** @return    -
 **/
void
BugReportReplay::end_bug_report()
{
  delete this->bug_report_replay;

  return ;
} // void BugReportReplay::end_bug_report()

/**
 ** reset the ais
 **
 ** @param     -
 **
 ** @return    -
 **/
void
BugReportReplay::reset_ais()
{
  unsigned n = 0;
  for (auto p : this->ui->party().players()) {
    if (dynamic_cast<Aiconfig*>(p))
      dynamic_cast<Aiconfig*>(p)->reset_to_hardcoded(n);
    ++n;
  }

  return ;
} // void BugReportReplay::reset_ais()

/**
 ** the name of 'player' has changed
 **
 ** @param     player   player whose name has changed
 **
 ** @return    -
 **/
void
BugReportReplay::name_changed(Player const& player)
{
  if (!this->get_realized())
    return ;

  if (!this->bug_report_replay)
    return ;

  if (::game_status & GAMESTATUS::GAME) {
    if (this->bug_report_replay->startplayer_no()
        == player.game().no(player))
      this->ui->translations->change(*this->startplayer,
                                     ::translation("Startplayer: %ustartplayer%",
                                                   player.game().no(player))
                                     + " (" + player.name() + ")"
                                    );

    if (this->bug_report_replay->soloplayer_no() == player.game().no(player))
      this->ui->translations->change(*this->soloplayer,
                                     ::translation("Soloplayer: %usoloplayer%",
                                                   player.game().no(player))
                                     + " (" + player.name() + ")"
                                    );
  } // if (::game_status & GAMESTATUS::GAME)

  this->actions->name_changed(player);
  this->human_actions->name_changed(player);
  this->game_summary->name_changed(player);

  return ;
} // void BugReportReplay::name_changed(Player player)

} // namespace UI_GTKMM_NS

#endif // #ifdef USE_UI_GTKMM
