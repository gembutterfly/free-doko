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

#include "card_suggestion.h"
#include "table.h"
#include "ui.h"
#include "translations.h"
#include "cards.h"
#include "hand.h"

#include "../../party/party.h"
#include "../../game/game.h"
#include "../../player/player.h"
#include "../../player/ai/ai.h"
#include "../../os/bug_report_replay.h"

#include "widgets/stock_button.h"
#include <gtkmm/stock.h>
#include <gtkmm/alignment.h>
#include <gtkmm/box.h>
#include <gtkmm/eventbox.h>
#include <gtkmm/image.h>
#include <gtkmm/label.h>
#include <gtkmm/textview.h>
#include <gtkmm/main.h>
#include <gdk/gdkkeysyms.h>

namespace UI_GTKMM_NS {

/**
 ** constructor
 **
 ** @param     table   	the table
 **
 ** @return    -
 **/
CardSuggestion::CardSuggestion(Table* const table) :
  Base(table),
  Gtk::StickyDialog("card suggestion")
{
  this->signal_realize().connect(sigc::mem_fun(*this, &CardSuggestion::init));

  return ;
} // CardSuggestion::CardSuggestion(Table* table)

/**
 ** init the dialog
 **
 ** @param     -
 **
 ** @return    -
 **/
void
CardSuggestion::init()
{
  this->ui->translations->add(*this,
                              ::translation("~suggested card"));

  { // action area
    auto close_button
      = Gtk::manage(new Gtk::StockButton(Gtk::Stock::CLOSE, "close"));
    this->ui->translations->add(*close_button,
                                ::translation("close"));
    this->add_action_widget(*close_button, Gtk::RESPONSE_CLOSE);

    close_button->grab_default();

    close_button->signal_clicked().connect(sigc::mem_fun(*this, &CardSuggestion::hide));


    this->play_button
      = Gtk::manage(new Gtk::StockButton(Gtk::Stock::YES, "play card"));
    this->ui->translations->add(*(this->play_button),
                                ::translation("play card"));
    this->add_action_widget(*(this->play_button), Gtk::RESPONSE_NONE);
    this->play_button->signal_clicked().connect(sigc::mem_fun(*this, &CardSuggestion::play_card));

    this->play_button->add_accelerator("activate",
                                       this->get_accel_group(),
                                       GDK_KEY_t,
                                       Gdk::ModifierType(0),
                                       Gtk::AccelFlags(0));
  } // action area

  { // vbox
    this->get_vbox()->set_spacing(20);

    this->card_box = Gtk::manage(new Gtk::EventBox());
    this->card_box->signal_button_press_event().connect(sigc::mem_fun(*this, &CardSuggestion::play_card_signal));

    this->card_image = this->ui->cards->new_managed_image();
    this->card_box->add(*this->card_image);
    this->card_image->set_padding(2 EM, 2 EX);
    this->rationals_text = Gtk::manage(new Gtk::Label("Thinking, please wait."));
    this->ui->translations->add(*this->rationals_text,
                                ::translation("Thinking, please wait."));
    this->rationals_description = Gtk::manage(new Gtk::TextView);
    this->rationals_description->set_wrap_mode(Gtk::WRAP_WORD);
    this->ui->translations->add(*this->rationals_description,
                                ::translation("Thinking, please wait."));

    auto alignment
      = Gtk::manage(new Gtk::Alignment(Gtk::ALIGN_CENTER, Gtk::ALIGN_CENTER,
                                       0, 0));
    alignment->add(*this->card_box);
    this->get_vbox()->pack_start(*alignment, Gtk::PACK_SHRINK);
    this->get_vbox()->pack_start(*this->rationals_text, Gtk::PACK_SHRINK);
    this->get_vbox()->pack_start(*this->rationals_description);
  } // vbox

  this->show_all_children();

  return ;
} // void CardSuggestion::init()

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    the player for the suggestion,
 **		 that is the current player of the game
 **/
::Player const&
CardSuggestion::player()
{
  return this->ui->game().player_current();
} // ::Player& CardSuggestion::player()

/**
 ** show the window
 **
 ** @param     show_window   whether to show the window
 **
 ** @return    -
 **/
void
CardSuggestion::show_information(bool const show_window)
{
  if (!this->get_realized())
    this->realize();

  if (show_window)
    this->present();

  this->get_suggestion();

  return ;
} // void CardSuggestion::show_information(bool show_window)

/**
 ** play the suggested card
 **
 ** @param     -
 **
 ** @return    -
 **/
void
CardSuggestion::play_card()
{
  if (this->suggested_card.is_empty())
    return ;

  const_cast<Player&>(this->player()).hand().request_card(this->suggested_card);
  this->ui->thrower(this->suggested_card, __FILE__, __LINE__);

  return ;
} // void CardSuggestion::play_card()

/**
 ** a card was played
 **
 ** @param     -
 **
 ** @return    -
 **/
void
CardSuggestion::card_played()
{
  //this->hide();
  this->suggested_card = Card();
  this->ui->cards->change_managed(this->card_image, Card());
  this->ui->translations->change(*this->rationals_text, ::translation(""));
  this->ui->translations->change(*this->rationals_description,
                                 ::translation(""));
  this->play_button->set_sensitive(false);

  return ;
} // void CardSuggestion::card_played()

/**
 ** get the suggestion from the ai
 **
 ** @param     -
 **
 ** @return    -
 **/
void
CardSuggestion::get_suggestion()
{
  DEBUG_ASSERTION(dynamic_cast<Ai const*>(&this->player()),
                  "CardSuggestion::get_Suggestion():\n"
                  "  'this->player' cannot be cast to an ai.");

  if (this->thinking)
    return ;

  auto const& ai = dynamic_cast<::Ai const&>(this->player());
  if (this->suggested_card.is_empty()
      || (   ::bug_report_replay
          && (this->player().type() == Player::HUMAN))) {
    this->thinking = true;

    this->ui->translations->change(*this->rationals_text,
                                   ::translation("Thinking, please wait."));
    this->ui->translations->change(*this->rationals_description,
                                   ::translation(""));
    this->ui->cards->change_managed(this->card_image, Card());

    this->play_button->set_sensitive(false);

    auto const thrower_depth = this->ui->thrower.depth();
    this->ui->thrower.set_depth(0);
    try {
      this->suggested_card = const_cast<Ai&>(ai).card_suggestion();
      if (ai.hand().contains(this->suggested_card))
        const_cast<Ai&>(ai).hand().request_card(this->suggested_card);
    } catch (Card const& card) {
      this->ui->thrower(card, __FILE__, __LINE__);
    } catch (GameStatus const game_status) {
      this->ui->thrower(game_status, __FILE__, __LINE__);
    } catch (...) {
      this->thinking = false;
      this->ui->thrower.set_depth(thrower_depth);
      throw;
    } // try
    this->ui->thrower.set_depth(thrower_depth);

    this->thinking = false;
  } // if (!this->suggested_card)

  static_cast<Table*>(this->parent)->hand(ai).draw();

  this->ui->cards->change_managed(this->card_image, this->suggested_card);
  this->ui->translations->change(*this->rationals_text,
                                 ::translation(ai.lastHeuristic_));
  this->ui->translations->change(*this->rationals_description,
                                 ::translation("AiConfig::Heuristic::Description::" + ::name(ai.lastHeuristic_)));

  this->play_button->set_sensitive(true);

  return ;
} // void CardSuggestion::get_suggestion()

/**
 ** play the suggested card
 **
 ** @param     -
 **
 ** @return    whether the card can be played
 **/
bool
CardSuggestion::play_card_signal(GdkEventButton*)
{
  if (this->suggested_card.is_empty())
    return false;

  this->play_card();

  return true;
} // bool CardSuggestion::play_card_signal(GdkEventButton*)

/**
 ** the mouse was clicked on the table
 **
 ** @param     -
 **
 ** @return    -
 **/
void
CardSuggestion::mouse_click_on_table()
{
  return ;
} // void CardSuggestion::mouse_click_on_table()

} // namespace UI_GTKMM_NS

#endif // #ifdef USE_UI_GTKMM
