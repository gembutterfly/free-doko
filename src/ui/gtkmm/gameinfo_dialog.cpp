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

#include "gameinfo_dialog.h"
#include "gameinfo.h"
#include "gameinfo.announcement.h"
#include "gameinfo.swines.h"
#include "table.h"
#include "ui.h"
#include "translations.h"
#include "main_window.h"

#include "../../party/party.h"
#include "../../game/game.h"
#include "../../party/rule.h"
#include "../../player/human/human.h"

#include "widgets/stock_button.h"
#include <gtkmm/separator.h>
#include <gtkmm/stock.h>
#include <gtkmm/main.h>

#ifdef USE_NETWORK
#include "../../network/server.h"
#endif

namespace UI_GTKMM_NS {

  /**
   ** constructor
   **
   ** @param     table   the table
   **
   ** @return    -
   **/
  GameInfoDialog::GameInfoDialog(Table* const table) :
    Base(table),
    Gtk::StickyDialog("GameInfoDialog", *table->ui->main_window, false)
  {
    this->ui->add_window(*this);

    this->signal_realize().connect(sigc::mem_fun(*this,
                                                 &GameInfoDialog::init));

    return ;
  } // GameInfoDialog::GameInfoDialog(Table* table)

  /**
   ** destruktor
   **
   ** @param     -
   **
   ** @return    -
   **/
  GameInfoDialog::~GameInfoDialog()
  {
    this->clear();
    return ;
  } // GameInfoDialog::~GameInfoDialog()

  /**
   ** init the dialog
   **
   ** @param     -
   **
   ** @return    -
   **/
  void
    GameInfoDialog::init()
    {
      this->ui->translations->add(*this, ::translation("game information"));

      this->set_icon(this->ui->icon);

      this->set_skip_taskbar_hint();

      { // action area
        { // hyperswines announce
          this->hyperswines_announce_button
            = Gtk::manage(new Gtk::StockButton(Gtk::Stock::CLOSE,
                                               "hyperswines"));
          this->ui->translations->add(*this->hyperswines_announce_button,
                                      ::translation("Menu::Item::hyperswines announcement"));
          this->add_action_widget(*this->hyperswines_announce_button,
                                  Gtk::RESPONSE_NONE);

          this->hyperswines_announce_button->signal_clicked().connect(sigc::mem_fun(*this, &GameInfoDialog::announce_hyperswines));
          this->hyperswines_announce_button->signal_clicked().connect(sigc::mem_fun(*this, &GameInfoDialog::close));

          // the button is only shown when the announcement is valid, see add_info().
          this->hyperswines_announce_button->show_all_children();
        } // hyperswines announce
        { // announcement reply button
          this->announcement_reply_button
            = Gtk::manage(new Gtk::StockButton(Gtk::Stock::CLOSE,
                                               "announcement reply"));
          this->ui->translations->add(*this->announcement_reply_button,
                                      ::translation(""));
          this->add_action_widget(*this->announcement_reply_button, Gtk::RESPONSE_CLOSE);

          this->announcement_reply_button->grab_default();

          this->announcement_reply_button->signal_clicked().connect(sigc::mem_fun(*this, &GameInfoDialog::announcement_reply));
          this->announcement_reply_button->signal_clicked().connect(sigc::mem_fun(*this, &GameInfoDialog::close));

          // the button is only shown when the reply is valid, see add_info().
          this->announcement_reply_button->show_all_children();
        } // announcement reply button
        { // close button
          this->close_button
            = Gtk::manage(new Gtk::StockButton(Gtk::Stock::CLOSE, "close"));
          this->ui->translations->add(*this->close_button,
                                      ::translation("close"));
          this->add_action_widget(*this->close_button, Gtk::RESPONSE_CLOSE);

          this->close_button->grab_default();

          this->close_button->signal_clicked().connect(sigc::mem_fun(*this, &GameInfoDialog::close));

          this->close_button->show_all();
        } // close button
      } // action area

      { // move to the top left corner of the table
        int x, y;
        this->table().get_window()->get_origin(x, y);
        this->move(x, y);
      } // move to the top left corner of the table

      return ;
    } // void GameInfoDialog::init()

  /**
   ** add the information and manage it
   **
   ** @param     info   game information to add
   **
   ** @return    -
   **/
  void
    GameInfoDialog::add_info(Information* const info)
    {
      this->infos.push_back(info);

      if (this->infos.empty()) {
        this->ui->translations->change(*this, info->title);
      } else {
        this->get_vbox()->add(*Gtk::manage(new Gtk::HSeparator()));
        this->ui->translations->change(*this,
                                       ::translation("game information"));
      }

      this->get_vbox()->add(*info);
      info->show();

      auto const& game = this->ui->game();

      { // check for announcement reply
        if (   (game.number_of_humans() == 1)
            && !this->ui->game().rule()(Rule::KNOCKING)
            && (!this->announcement_reply_button->is_visible())
            && (dynamic_cast<Information::Announcement*>(info)) ) {
          auto const& announcement_info
            = static_cast<Information::Announcement const&>(*info);
          if (   (game.team(*announcement_info.player)
                  != game.team(*game.human_player()))
              && !ANNOUNCEMENT::is_reply(announcement_info.announcement) ) {
            auto const reply
              = game.announcement_valid(ANNOUNCEMENT::REPLY,
                                        *game.human_player());
            if (reply) {
              this->ui->translations->change(*this->announcement_reply_button,
                                             (   (reply == ANNOUNCEMENT::NO120)
                                              || (reply == ANNOUNCEMENT::REPLY))
                                             ? ::translation(game.human_player()->team())
                                             : ::translation(reply));
              this->announcement_reply_button->show();
            } // if (reply)
          } // if (can reply)
        } // if (is announcement)
      } // check for announcement reply

      { // check for hyperswines announcement
        if (   (game.number_of_humans() == 1)
            && (!this->hyperswines_announce_button->is_visible())
            && (dynamic_cast<Information::Swines*>(info)) ) {
          if (game.hyperswines_announcement_valid(*game.human_player())) {
            this->hyperswines_announce_button->show();
          } // if (reply valid
        } // if (is announcement)
      } // check for hyperswines announcement

      return ;
    } // void GameInfoDialog::add_info(Information* info)

  /**
   ** clear the window
   **
   ** @param     -
   **
   ** @return    -
   **/
  void
    GameInfoDialog::clear()
    {
      for (auto& info : this->infos) {
        this->get_vbox()->remove(*info);
        delete info;
      }

      this->infos.clear();

      if (this->announcement_reply_button)
        this->announcement_reply_button->hide();

      if (this->hyperswines_announce_button)
        this->hyperswines_announce_button->hide();

      return ;
    } // void GameinfDialog::clear()

  /**
   ** -> result
   **
   ** @param     -
   **
   ** @return    the table
   **/
  Table&
    GameInfoDialog::table()
    {
      return *dynamic_cast<Table*>(this->parent);
    } // Table& GameinfDialog::table()

  /**
   ** show the information and manage it
   **
   ** @param     info                  information to show
   ** @param     close_automatically   whether to close the window automatically
   ** @param     close_delay   	       the time (in tenth of seconds) to wait
   **                                  before closing the window
   **
   ** @return    -
   **
   ** @todo      manage multiple delays
   **/
  void
    GameInfoDialog::show(Information* const info,
                         bool const close_automatically,
                         unsigned const close_delay)
    {
      if (!this->get_realized())
        this->realize();

      this->add_info(info);

      this->present();

#ifdef WORKAROUND
      // @todo   manage multiple delays
      if (this->timeout_connection)
        this->timeout_connection.disconnect();

      if (close_automatically)
        this->timeout_connection
          = Glib::signal_timeout().connect(sigc::slot0<bool>(sigc::mem_fun(*this, &GameInfoDialog::on_timeout)),
                                           close_delay);
#endif

      if (this->is_blocking()) {
        while (   this->is_visible()
               && !this->ui->thrower)
          ::ui->wait();

        this->close();
      } // if (this->is_blocking)

      return ;
    } // void GameInfoDialog::show(Information* info, bool close_automatically, unsigned close_delay)

  /**
   ** show the information and close it on a click
   **
   ** @param     info   information to show
   **
   ** @return    -
   **/
  void
    GameInfoDialog::show(Information* const info)
    {
      this->show(info, false, 0);

      return ;
    } // void GameInfoDialog::show(Information* info)

  /**
   ** hide the window
   **
   ** @param     -
   **
   ** @return    -
   **/
  void
    GameInfoDialog::close()
    {
      if (this->timeout_connection)
        this->timeout_connection.disconnect();

      this->hide();
      this->clear();

      return ;
    } // void GameInfoDialog::close()

  /**
   ** update the announcement reply sensitivity
   **
   ** @param     -
   **
   ** @return    -
   **/
  void
    GameInfoDialog::update_announcement_reply()
    {
      if (!this->announcement_reply_button)
        return ;

      auto const& game = this->ui->game();
      if (   this->announcement_reply_button->is_visible()
          && !game.announcement_valid(ANNOUNCEMENT::REPLY,
                                      *game.human_player()) ) {
        this->announcement_reply_button->hide();
      } // if (reply invalid)

      return ;
    } // void GameInfoDialog::update_announcement_reply()

  /**
   ** the name of 'player' has changed
   **
   ** @param     player   player whose name has changed
   **
   ** @return    -
   **/
  void
    GameInfoDialog::name_changed(Player const& player)
    {
      for (auto& info : this->infos)
        info->name_changed(player);
    } // void GameInfoDialog::name_changed(Player player)

  /**
   ** the timeout event:
   ** close the window and exit the main loop
   **
   ** @param     -
   **
   ** @return    false (to stop the timeout event)
   **/
  bool
    GameInfoDialog::on_timeout()
    {
      this->close();

      return false;
    } // bool GameInfoDialog::on_timeout()

  /**
   ** reply to the last announcement
   **
   ** @param     -
   **
   ** @return    -
   **/
  void
    GameInfoDialog::announcement_reply()
    {
      auto& game = ::party.game();

      DEBUG_ASSERTION((game.number_of_humans() == 1),
                      "GameInfoDialog::announcement_reply()\n"
                      "  game has '" << game.number_of_humans() << "' humans, must be '1'");

      game.make_announcement(ANNOUNCEMENT::REPLY, *game.human_player());

      return ;
    } // void GameInfoDialog::announcement_reply()

  /**
   ** announce the hyperswines
   **
   ** @param     -
   **
   ** @return    -
   **/
  void
    GameInfoDialog::announce_hyperswines()
    {
      DEBUG_ASSERTION(this->ui->game().human_player(),
                      "GameInfoDialog::announce_hyperswines()\n"
                      "  no human player");
      this->ui->game().hyperswines_announce(*this->ui->game().human_player());

      return ;
    } // void GameInfoDialog::announce_hyperswines()

  /**
   ** -> result
   **
   ** @param     -
   **
   ** @return    whether the dialog is blocking the gameplay
   **/
  bool
    GameInfoDialog::is_blocking() const
    {
      for (auto& i : this->infos)
        if (i->is_blocking())
          return true;

      return false;
    } // bool GameInfoDialog::is_blocking() const

} // namespace UI_GTKMM_NS

#endif // #ifdef USE_UI_GTKMM
