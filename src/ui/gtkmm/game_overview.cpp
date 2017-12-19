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

#include "game_overview.h"
#include "game_summary.h"
#include "main_window.h"
#include "ui.h"
#include "translations.h"

#include "../../party/party.h"
#include "../../game/game_summary.h"
#include "../../player/player.h"

#include "widgets/stock_button.h"
#include <gtkmm/stock.h>
#include <gtkmm/label.h>
#include <gtkmm/checkbutton.h>
#include <gdk/gdkkeysyms.h>

namespace UI_GTKMM_NS {

/**
 ** constructor
 **
 ** @param     party_points    the parent widget
 **
 ** @return    -
 **/
PartyPoints::GameOverview::GameOverview(PartyPoints* const party_points) :
  Base(party_points),
  StickyDialog("Game overview", *this->ui->main_window,
               // *dynamic_cast<PartyPoints*>(this->parent),
               false)
{
  this->ui->add_window(*this);

  this->set_transient_for(*dynamic_cast<PartyPoints*>(this->parent));

  // the game summary is created here, because else I would have first
  // to show this window and afterwards set the gameno.
  this->game_summary = Gtk::manage(new GameSummary(this));

  this->signal_realize().connect(sigc::mem_fun(*this, &GameOverview::init));

#ifndef GLIBMM_DEFAULT_SIGNAL_HANDLERS_ENABLED
  this->signal_key_press_event().connect(sigc::mem_fun(*this, &GameOverview::on_key_press_event));
#endif

  return ;
} // GameOverview::GameOverview(PartyPoints* party_points)

/**
 ** create all subelements
 **
 ** @param     -
 **
 ** @return    -
 **/
void
PartyPoints::GameOverview::init()
{
  this->ui->translations->add(*this, 
                              ::translation("Game overview: %ugame%",
                                            this->gameno_));

  this->set_icon(this->ui->icon);
  this->set_skip_taskbar_hint();

  this->get_vbox()->set_spacing(10);

  this->get_vbox()->pack_start(*this->game_summary);

  { // action area
    this->close_button
      = Gtk::manage(new Gtk::StockButton(Gtk::Stock::CLOSE, "close"));
    this->ui->translations->add(*(this->close_button),
                                ::translation("close"));
    this->add_action_widget(*(this->close_button), Gtk::RESPONSE_CLOSE);

    this->close_button->grab_default();
  } // action area

  // signals
  this->close_button->signal_clicked().connect(sigc::mem_fun(*this, &Gtk::Widget::hide));
  this->signal_hide().connect(sigc::bind<bool>(sigc::mem_fun(*(dynamic_cast<PartyPoints*>(this->parent)->show_game_overview_button),
                                                             &Gtk::ToggleButton::set_active),
                                               false));

  this->show_all_children();

  auto const gameno = this->gameno_;
  this->gameno_ = UINT_MAX;
  this->set_gameno(gameno);

  return ;
} // void GameOverview::init()

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    the gameno
 **/
unsigned
PartyPoints::GameOverview::gameno() const
{
  return this->gameno_;
} // unsigned GameOverview::gameno() const

/**
 ** the game has finished:
 ** show the summary and let the player review the game
 **
 ** @param     gameno   new gameno
 **		(UINT_MAX for no gameno)
 **
 ** @return    -
 **/
void
PartyPoints::GameOverview::set_gameno(unsigned const gameno)
{
  if (this->gameno() == gameno)
    return ;

  this->gameno_ = gameno;

  if (!this->get_realized())
    return ;

  if (this->gameno() == UINT_MAX) {
    this->hide();
    return ;
  }

  DEBUG_ASSERTION((this->gameno() <= ::party.gameno()),
                  "GameOverview::set_gameno():\n"
                  "  gameno = " << this->gameno()
                  << " > "
                  << ::party.gameno() << " = ::party.gameno()");

  this->game_summary->set_gameno(this->gameno());
  this->ui->translations->change(*this,
                                 ::translation("Game overview: %ugame%",
                                               this->gameno() + 1));

  return ;
} // void GameOverview::set_gameno(unsigned gameno)

/**
 ** the name of 'player' has changed
 **
 ** @param     player   the player with the changed name
 **
 ** @return    -
 **/
void
PartyPoints::GameOverview::name_changed(::Player const& player)
{
  if (!this->get_realized())
    return ;

  if (this->game_summary)
    this->game_summary->name_changed(player);

  return ;
} // void PartyPoints::GameOverview::name_changed(Player const& player)

/**
 ** a key has been pressed
 ** C-o: output of the game summary on 'stdout'
 **
 ** @param     key   the key
 **
 ** @return    whether the key was managed
 **/
bool
PartyPoints::GameOverview::on_key_press_event(GdkEventKey* const key)
{
  bool managed = false;

  if ((key->state & ~GDK_SHIFT_MASK) == GDK_CONTROL_MASK) {
    switch (key->keyval) {
    case GDK_KEY_o: // ouput of the game summary
      cout << this->ui->party().game_summary(this->gameno());
      managed = true;
      break;
    } // switch (key->keyval)
  } // if (key->state == GDK_CONTROL_MASK)

  return (managed
          || this->StickyDialog::on_key_press_event(key)
          || this->ui->key_press(key));
} // bool PartyPoints::GameOverview::on_key_press_event(GdkEventKey* key)

} // namespace UI_GTKMM_NS

#endif // #ifdef USE_UI_GTKMM
