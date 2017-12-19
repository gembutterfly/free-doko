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

#include "game_finished.h"
#include "game_summary.h"
#include "game_review.h"

#include "ui.h"
#include "translations.h"
#include "bug_report.h"
#include "main_window.h"
#include "table.h"
#include "party_points.h"

#include "../../party/party.h"
#include "../../misc/setting.h"

#include "widgets/stock_button.h"
#include <gtkmm/checkbutton.h>
#include <gtkmm/stock.h>
#include <gtkmm/label.h>
#include <gtkmm/alignment.h>
#include <gtkmm/main.h>

namespace UI_GTKMM_NS {

/**
 ** constructor
 **
 ** @param     parent   the parent widget
 **
 ** @return    -
 **/
GameFinished::GameFinished(Base* const parent) :
  Base(parent),
  StickyDialog("Game finished", *(parent->ui->main_window), false)
{
  this->ui->add_window(*this);

  // the game summary is created here, because else I would have first
  // to show this window and afterwards set the gameno.
  this->game_summary = Gtk::manage(new GameSummary(this));

  this->signal_realize().connect(sigc::mem_fun(*this, &GameFinished::init));
#ifndef GLIBMM_DEFAULT_SIGNAL_HANDLERS_ENABLED
  this->signal_hide().connect(sigc::mem_fun(*this, &GameFinished::on_hide));
#endif

  return ;
} // GameFinished::GameFinished(Base* parent)

/**
 ** create all subelements
 **
 ** @param     -
 **
 ** @return    -
 **/
void
GameFinished::init()
{
  this->ui->translations->add(*this, ::translation("~end of game"));

  this->set_icon(this->ui->icon);

  this->set_position(Gtk::WIN_POS_CENTER_ON_PARENT);

  this->get_vbox()->set_spacing(10);

  this->get_vbox()->pack_start(*this->game_summary);

  { // show party points
    this->show_party_points
      = Gtk::manage(new Gtk::StockButton("show party points"));
    this->ui->translations->add(*show_party_points->label,
                                ::translation("show party points"));

    auto alignment = Gtk::manage(new Gtk::Alignment(Gtk::ALIGN_CENTER, Gtk::ALIGN_CENTER, 0, 0));
    alignment->add(*this->show_party_points);
    this->get_vbox()->pack_end(*alignment, Gtk::PACK_SHRINK);
  } // show party points

  { // show game review
    this->show_game_review
      = Gtk::manage(new Gtk::CheckButton("show game review"));
    this->ui->translations->add(*show_game_review,
                                ::translation("show game review"));

    auto alignment
      = Gtk::manage(new Gtk::Alignment(Gtk::ALIGN_CENTER, Gtk::ALIGN_CENTER,
                                       0, 0));
    alignment->add(*this->show_game_review);
    this->get_vbox()->pack_end(*alignment, Gtk::PACK_SHRINK);
  } // show game review

  { // action area
    this->create_bug_report_button
      = Gtk::manage(new Gtk::StockButton("create bug report"));
    this->ui->translations->add(*create_bug_report_button->label,
                                ::translation("BugReport::create"));
    //this->add_action_widget(*this->create_bug_report_button, Gtk::RESPONSE_NONE);
    this->get_action_area()->add(*this->create_bug_report_button);
#ifdef WORKAROUND
    // for spacing
    auto alignment
      = Gtk::manage(new Gtk::Alignment(Gtk::ALIGN_CENTER,
                                       Gtk::ALIGN_CENTER,
                                       1, 0));
    this->get_action_area()->add(*alignment);
#endif

    this->next_game_button
      = Gtk::manage(new Gtk::StockButton(Gtk::Stock::OK, "next game"));
    this->ui->translations->add(*next_game_button->label,
                                ::translation("next game"));
    this->add_action_widget(*this->next_game_button, Gtk::RESPONSE_CLOSE);

    this->next_game_button->grab_default();
    this->next_game_button->grab_focus();
  } // action area

  this->show_all_children();
#ifdef WORKAROUND
  // for graphic bugs
  Gtk::Alignment* alignment
    = Gtk::manage(new Gtk::Alignment(Gtk::ALIGN_CENTER,
                                     Gtk::ALIGN_CENTER,
                                     0, 0));
  this->get_vbox()->add(*alignment);
  alignment->show();
#endif

  // signals
  this->create_bug_report_button->signal_clicked().connect(sigc::mem_fun(*(this->ui->bug_report),
                                                                         &BugReport::create_report));
  this->show_party_points->signal_clicked().connect(sigc::mem_fun0(*this->ui->table->party_points_, &Gtk::Window::present));
  this->show_game_review->signal_clicked().connect(sigc::mem_fun(*this, &GameFinished::toggle_game_review));
  this->next_game_button->signal_clicked().connect(sigc::mem_fun(*this, &GameFinished::hide));

  this->setting_update(Setting::SHOW_BUG_REPORT_BUTTON_IN_GAME_FINISHED_WINDOW);

  return ;
} // void GameFinished::init()

/**
 ** the game has finished:
 ** show the summary and let the player review the game
 **
 ** @param     -
 **
 ** @return    -
 **/
void
GameFinished::game_finished()
{
  this->realize();

  this->game_summary->set_gameno(::party.gameno());
  if (this->ui->party().is_last_game())
    this->ui->translations->change(*next_game_button->label,
                                   ::translation("end of party"));
  else
    this->ui->translations->change(*next_game_button->label,
                                   ::translation("next game"));

  this->present();

  while (   !this->ui->thrower
         && (::game_status == GAMESTATUS::GAME_FINISHED)
         && this->is_visible())
    ::ui->wait();

  this->show_game_review->set_active(false);

  this->hide();

  return ;
} // void GameFinished::game_finished()

/**
 ** a setting has changed
 ** SHOW_BUG_REPORT_BUTTON_IN_GAME_FINISHED_WINDOW:
 **	update whether to show the 'create bug report' button
 **
 ** @param     type   the type that has changed
 **
 ** @return    -
 **/
void
GameFinished::setting_update(int const type)
{
  if (!this->get_realized())
    return ;

  switch (type) {
  case Setting::SHOW_BUG_REPORT_BUTTON_IN_GAME_FINISHED_WINDOW:
    if (::setting(Setting::SHOW_BUG_REPORT_BUTTON_IN_GAME_FINISHED_WINDOW))
      this->create_bug_report_button->show();
    else
      this->create_bug_report_button->hide();
    break;
  default:
    break;
  } // switch(type)

  return ;
} // void GameFinished::setting_update(int type)

/**
 ** the name of 'player' has changed
 **
 ** @param     player   the player with the changed name
 **
 ** @return    -
 **/
void
GameFinished::name_changed(::Player const& player)
{
  if (!this->get_realized())
    return ;

  if (this->game_summary)
    this->game_summary->name_changed(player);

  return ;
} // void GameFinished::name_changed(Player const& player)

/**
 ** event: hide the window
 **
 ** @param     -
 **
 ** @return    -
 **/
void
GameFinished::on_hide()
{
#ifdef GLIBMM_DEFAULT_SIGNAL_HANDLERS_ENABLED
  this->Gtk::Dialog::on_hide();
#endif
  this->ui->main_quit();
  this->game_summary->set_gameno(UINT_MAX);

  return ;
} // void GameFinished::on_hide()

/**
 ** toggles the game review
 **
 ** @param     -
 **
 ** @return    -
 **/
void
GameFinished::toggle_game_review()
{
  if (this->show_game_review->get_active()) {
    DEBUG_ASSERTION(!this->game_review,
                    "GameFinished::toggle_game_review:\n"
                    "  already in game review");
    this->game_review = std::make_unique<GameReview>(this);
  } else {
    DEBUG_ASSERTION(this->game_review,
                    "GameFinished::toggle_game_review:\n"
                    "  not in game review");
    this->game_review.release();
  }
  this->ui->table->draw_all();

  return ;
} // void GameFinished::toggle_game_review()

} // namespace UI_GTKMM_NS

#endif // #ifdef USE_UI_GTKMM
