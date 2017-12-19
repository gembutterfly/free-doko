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

#include "full_trick.h"
#include "trick_summary.h"
#include "ui.h"
#include "table.h"
#include "translations.h"
#include "cards.h"

#include "../../game/game.h"
#include "../../card/trick.h"
#include "../../misc/setting.h"

#include "widgets/stock_button.h"
#include <gtkmm/image.h>
#include <gtkmm/stock.h>
#include <gtkmm/main.h>

namespace UI_GTKMM_NS {

  /**
   ** constructor
   **
   ** @param     table   the table
   **
   ** @return    -
   **/
FullTrick::FullTrick(Table* const table) :
  Base(table),
  Gtk::StickyDialog("full trick", *table->ui->main_window, false),
  winnercard()
{
  this->set_gravity(Gdk::GRAVITY_STATIC);

  this->signal_realize().connect(sigc::mem_fun(*this, &FullTrick::init));

  return ;
} // FullTrick::FullTrick(Table* table)

/**
 ** destruktor
 **
 ** @param     -
 **
 ** @return    -
 **/
FullTrick::~FullTrick()
{
  delete this->trick_summary;

  return ;
} // FullTrick::~FullTrick()

/**
 ** create all subelements
 **
 ** @param     -
 **
 ** @return    -
 **/
void
FullTrick::init()
{
  this->ui->translations->add(*this,
                              ::translation("~full trick"));

  { // action area
    auto close_button
      = Gtk::manage(new Gtk::StockButton(Gtk::Stock::CLOSE, "close"));
    this->ui->translations->add(*close_button,
                                ::translation("close"));
    this->add_action_widget(*close_button, Gtk::RESPONSE_CLOSE);

    close_button->grab_default();

    close_button->signal_clicked().connect(sigc::mem_fun(*this, &FullTrick::close));
  } // action area

  { // vbox
    this->get_vbox()->set_spacing(1 EX);

    this->trick_summary = new TrickSummary(this, false);

    this->get_vbox()->add(*this->trick_summary);

    this->winnercard = this->ui->cards->new_managed_image();
    this->winnercard->set_padding(2 EM, 2 EX);
    this->trick_summary->pack_start(*this->winnercard);
  } // vbox

  this->show_all_children();
  this->trick_summary->update();

  { // move to the top left corner of the window
    int x, y;
    static_cast<Table*>(this->parent)->get_window()->get_origin(x, y);
    this->move(x, y);
  } // move to the top left conrner of the window
  return ;
} // void FullTrick::init()

/**
 ** show the window
 **
 ** @param     trick   trick to show
 **
 ** @return    -
 **/
void
FullTrick::show_information(::Trick const& trick)
{
  this->trick = &trick;
  if (!this->get_realized())
    this->realize();

  bool show = ::setting(Setting::SHOW_FULL_TRICK_WINDOW);
  if (!show
      && ::setting(Setting::SHOW_FULL_TRICK_WINDOW_IF_SPECIAL_POINTS)) {
    // test whether the teams of the player of and for are different
    auto const specialpoints = this->trick->specialpoints();
    for (auto const& sp : specialpoints) {
      if (    (sp.player_of_no == UINT_MAX)
          || ( (this->trick->game().teaminfo_for_humans(this->trick->game().player(sp.player_get_no)) != TEAM::RE)
              && (this->trick->game().teaminfo_for_humans(this->trick->game().player(sp.player_get_no)) != TEAM::CONTRA))
          || (this->trick->game().teaminfo_for_humans(this->trick->game().player(sp.player_get_no))
              != this->trick->game().teaminfo_for_humans(this->trick->game().player(sp.player_of_no)))) {
        show = true;
        break;
      } // if (trick shall be shown)
    } // for (sp)
  } // if (!show)

  if (show) {
    this->trick_summary->set_trick(*this->trick);
    this->ui->cards->change_managed(this->winnercard,
                                    this->trick->winnercard());

    this->present();
  }

  sigc::connection timeout_connection;
  if (::setting(Setting::CLOSE_FULL_TRICK_AUTOMATICALLY))
    timeout_connection
      = Glib::signal_timeout().connect(sigc::slot0<bool>(sigc::mem_fun(*this, &FullTrick::on_timeout)),
                                       ::setting(Setting::FULL_TRICK_CLOSE_DELAY));

  this->ui->table->mouse_cursor_update();

  while (   (::game_status == GAMESTATUS::GAME_FULL_TRICK)
         && !this->ui->thrower)
    ::ui->wait();

  if (::setting(Setting::CLOSE_FULL_TRICK_AUTOMATICALLY))
    timeout_connection.disconnect();

  if (show)
    this->hide();

  if (   (this->ui->thrower.exception_type() == Thrower::GAMESTATUS)
      && (this->ui->thrower.exception().game_status
          == GAMESTATUS::GAME_TRICK_TAKEN)) {
    this->ui->thrower.clear();
    ::game_status = GAMESTATUS::GAME_TRICK_TAKEN;
  }

  this->trick = nullptr;
  this->ui->cards->change_managed(this->winnercard, Card());

  return ;
} // void FullTrick::show_information(::Trick trick)

/**
 ** close the full trick
 **
 ** @param     -
 **
 ** @return    -
 **/
void
FullTrick::close()
{
  if (!this->trick)
    return ;

#ifdef WORKAROUND
  // problem: this exception is thrown two times
  // It seems that that one time is from the automatic closing
  // and the other from manual closing.
  if (this->ui->thrower.exception_type() == Thrower::GAMESTATUS)
    return ;
#endif
  this->ui->thrower(GAMESTATUS::GAME_TRICK_TAKEN, __FILE__, __LINE__);

  return ;
} // void FullTrick::close()

/**
 ** the name of 'player' has changed
 **
 ** @param     player   the player whose name has changed
 **
 ** @return    -
 **/
void
FullTrick::name_changed(Player const& player)
{
  if (!this->trick)
    return ;

  this->trick_summary->update();

  return ;
} // void FullTrick::name_changed(Player player)

/**
 ** the timeout event:
 ** close the window and exit the main loop
 **
 ** @param     -
 **
 ** @return    false (to stop the timeout event)
 **/
bool
FullTrick::on_timeout()
{
  this->close();

  return false;
} // bool FullTrick::on_timeout()

} // namespace UI_GTKMM_NS

#endif // #ifdef USE_UI_GTKMM
