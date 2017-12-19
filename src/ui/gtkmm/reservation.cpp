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

#include "reservation.h"
#include "table.h"
#include "name.h"
#include "icongroup.h"
#include "ui.h"
#include "main_window.h"
#include "utils.h"

#include "widgets/stock_button.h"
#include "../../party/rule.h"
#include "../../game/game.h"
#include "../../player/player.h"
#include "../../basistypes.h"

#include "../../os/bug_report_replay.h"

#include "../../utils/string.h"

#include <gtkmm/stock.h>
#include <gtkmm/notebook.h>
#include <gtkmm/alignment.h>
#include <gtkmm/radiobutton.h>
#include <gtkmm/table.h>
#include <gtkmm/main.h>
namespace UI_GTKMM_NS {

  /**
   ** constructor
   **
   ** @param     table      the table
   ** @param     position   the corresponding position
   **
   ** @return    -
   **/
  Reservation::Reservation(Table* const table, Position const position) :
    HTIN(table, position),
    Gtk::StickyDialog("Reservation", *parent->ui->main_window, false)
  {
    this->ui->add_window(*this);

    this->signal_realize().connect(sigc::mem_fun(*this,
                                                 &Reservation::init));

    return ;
  } // Reservation::Reservation(Table* table, Position position)

  /**
   ** destructor
   **
   ** @param     -
   **
   ** @return    -
   **/
  Reservation::~Reservation()
  { }

  /**
   ** -> result
   **
   ** @param     -
   **
   ** @return    the corresponding player
   **/
  Player&
    Reservation::player()
    {
      return this->ui->game().player(this->ui->game().no(this->table().player(this->position())));
    } // Player& Reservation::player()

  /**
   ** -> result
   **
   ** @param     -
   **
   ** @return    the corresponding game
   **/
  Game&
    Reservation::game()
    {
      return this->ui->game();
    } // Game& Reservation::game()

  /**
   ** create all subelements
   **
   ** @param     -
   **
   ** @return    -
   **/
  void
    Reservation::init()
    {
      this->set_icon(this->ui->icon);
      if (this->game().number_of_humans() <= 1)
        this->ui->translations->add(*this,
                                    ::translation("~reservation"));
      else
        this->ui->translations->add(*this,
                                    ::translation("~reservation (%sname%)",
                                                  this->player().name()));

      this->set_position(Gtk::WIN_POS_CENTER_ON_PARENT);

      this->announce_button
        = Gtk::manage(new Gtk::StockButton(Gtk::Stock::OK,
                                           "reservation announce"));
      this->ui->translations->add(*this->announce_button,
                                  ::translation("reservation announce"));
      this->add_action_widget(*this->announce_button, Gtk::RESPONSE_ACCEPT);

      this->bock_label = Gtk::manage(new Gtk::Label("Bock"));
#ifdef POSTPONED
      // does not work reliable
      {
        Pango::AttrList attributes;
        auto attribute
          = Pango::AttrShape::create_attr_weight(Pango::WEIGHT_BOLD);
        attributes.insert(attribute);
        this->bock_label->set_attributes(attributes);
      }
#endif
      this->ui->translations->add(*this->bock_label,
                                  ::translation("bock: %umultiplier%", 1u));


      this->notebook = Gtk::manage(new Gtk::Notebook());
      this->notebook->set_tab_pos(Gtk::POS_LEFT);
      this->notebook->signal_switch_page().connect(sigc::mem_fun(*this,
                                                                 &Reservation::switch_page_event));

      this->swines_button = Gtk::manage(new Gtk::CheckButton("swines"));
      this->ui->translations->add(*this->swines_button,
                                  ::translation("swines"));
      this->hyperswines_button = Gtk::manage(new Gtk::CheckButton("hyperswines"));
      this->ui->translations->add(*this->hyperswines_button,
                                  ::translation("hyperswines"));
      this->solo_swines_button = Gtk::manage(new Gtk::CheckButton("swines"));
      this->ui->translations->add(*this->solo_swines_button,
                                  ::translation("swines"));
      this->solo_hyperswines_button = Gtk::manage(new Gtk::CheckButton("hyperswines"));
      this->ui->translations->add(*this->solo_hyperswines_button,
                                  ::translation("hyperswines"));

      this->announce_button->grab_default();

      { // bock information
        auto alignment
          = Gtk::manage(new Gtk::Alignment(Gtk::ALIGN_CENTER,
                                           Gtk::ALIGN_CENTER,
                                           0, 0));
        alignment->set_border_width(1 EX);
        alignment->add(*this->bock_label);
        this->get_vbox()->add(*alignment);
      } // bock information
      { // the notebook
        this->get_vbox()->add(*this->notebook);

        { // create the buttons
          Gtk::RadioButton::Group gametype_group;
          for (int g = GAMETYPE::FIRST; g <= GAMETYPE::LAST; g++) {
            this->gametype_buttons.push_back(Gtk::manage(new Gtk::RadioButton(gametype_group, "type")));
            this->gametype_buttons.back()->set_data("gametype", new GameType(GameType(g)));
            this->ui->translations->add(*this->gametype_buttons.back(),
                                        ::translation(static_cast<GameType>(g)));
            this->gametype_buttons.back()->signal_toggled().connect(sigc::bind<int const>(sigc::mem_fun(*this, &Reservation::gametype_changed), g));
          } // for (g \in GAMETYPE)

          Gtk::RadioButton::Group marriage_selector_group;
          for (int m = MARRIAGE_SELECTOR::FIRST;
               m <= MARRIAGE_SELECTOR::LAST;
               m++) {
            this->marriage_selector_buttons.push_back(Gtk::manage(new Gtk::RadioButton(marriage_selector_group, "selector")));
            this->marriage_selector_buttons.back()->set_data("selector", new MarriageSelector(MarriageSelector(m)));
            this->ui->translations->add(*(this->marriage_selector_buttons.back()),
                                        ::translation(static_cast<MarriageSelector>(m)));
            this->marriage_selector_buttons.back()->signal_toggled().connect(sigc::bind<int const>(sigc::mem_fun(*this, &Reservation::marriage_selector_changed), m));
          } // for (m \in MARRIAGE_SELECTOR)
        } // create the buttons

        { // set the pages of the notebook
          { // General
            auto alignment
              = Gtk::manage(new Gtk::Alignment(Gtk::ALIGN_CENTER,
                                               Gtk::ALIGN_CENTER,
                                               0, 0));
            alignment->set_border_width(2 EX);
            auto label = Gtk::manage(new Gtk::Label("general"));
            this->ui->translations->add(*label,
                                        ::translation("GameType::Group::general"));
            this->notebook->append_page(*alignment, *label);

            auto hbox = Gtk::manage(new Gtk::HBox(false, 1 EX));
            alignment->add(*hbox);

            Gtk::VBox* vbox = nullptr;
            { // the swines
              vbox = Gtk::manage(new Gtk::VBox(false, 10));
              hbox->add(*vbox);

              {
                auto alignment
                  = Gtk::manage(new Gtk::Alignment(Gtk::ALIGN_START,
                                                   Gtk::ALIGN_CENTER,
                                                   0, 0));
                alignment->add(*this->swines_button);
                vbox->add(*alignment);
              }
              {
                auto alignment
                  = Gtk::manage(new Gtk::Alignment(Gtk::ALIGN_START,
                                                   Gtk::ALIGN_CENTER,
                                                   0, 0));
                vbox->add(*this->hyperswines_button);
                vbox->add(*alignment);
              }
            } // the swines
            { // gametypes
              vbox = Gtk::manage(new Gtk::VBox(false, 10));
              hbox->add(*vbox);

              vbox->add(*this->gametype_buttons[GAMETYPE::NORMAL]);
              vbox->add(*this->gametype_buttons[GAMETYPE::THROWN_NINES]);
              vbox->add(*this->gametype_buttons[GAMETYPE::THROWN_KINGS]);
              vbox->add(*this->gametype_buttons[GAMETYPE::THROWN_NINES_AND_KINGS]);
              vbox->add(*this->gametype_buttons[GAMETYPE::THROWN_RICHNESS]);
              vbox->add(*this->gametype_buttons[GAMETYPE::FOX_HIGHEST_TRUMP]);
              vbox->add(*this->gametype_buttons[GAMETYPE::POVERTY]);
            } // gametypes
            { // marriage selectors
              vbox = Gtk::manage(new Gtk::VBox(false, 10));
              hbox->add(*vbox);

              vbox->add(*this->gametype_buttons[GAMETYPE::MARRIAGE]);
              vbox->add(*indent(*this->marriage_selector_buttons[MARRIAGE_SELECTOR::FIRST_FOREIGN]));
              vbox->add(*indent(*this->marriage_selector_buttons[MARRIAGE_SELECTOR::FIRST_TRUMP]));
              vbox->add(*indent(*this->marriage_selector_buttons[MARRIAGE_SELECTOR::FIRST_COLOR]));
              vbox->add(*indent(*this->marriage_selector_buttons[MARRIAGE_SELECTOR::FIRST_CLUB]));
              vbox->add(*indent(*this->marriage_selector_buttons[MARRIAGE_SELECTOR::FIRST_SPADE]));
              vbox->add(*indent(*this->marriage_selector_buttons[MARRIAGE_SELECTOR::FIRST_HEART]));
            } // marriage selectors
          } // General
          { // Solo
            auto alignment
              = Gtk::manage(new Gtk::Alignment(Gtk::ALIGN_CENTER,
                                               Gtk::ALIGN_CENTER,
                                               0, 0));
            alignment->set_border_width(2 EX);
            auto label = Gtk::manage(new Gtk::Label("solo"));
            this->ui->translations->add(*label,
                                        ::translation("GameType::Group::solo"));
            this->notebook->append_page(*alignment, *label);

            auto table = Gtk::manage(new Gtk::Table(6, 8));
            table->set_row_spacings(10);
            table->set_col_spacings(10);
            alignment->add(*table);

            table->attach(*this->gametype_buttons[GAMETYPE::SOLO_CLUB],
                          0, 2, 0, 1);
            table->attach(*this->gametype_buttons[GAMETYPE::SOLO_SPADE],
                          0, 2, 1, 2);
            table->attach(*this->gametype_buttons[GAMETYPE::SOLO_HEART],
                          0, 2, 2, 3);
            table->attach(*this->gametype_buttons[GAMETYPE::SOLO_DIAMOND],
                          0, 2, 3, 4);
            table->attach(*this->gametype_buttons[GAMETYPE::SOLO_MEATLESS],
                          2, 4, 0, 1);
            table->attach(*this->gametype_buttons[GAMETYPE::SOLO_JACK],
                          2, 4, 1, 2);
            table->attach(*this->gametype_buttons[GAMETYPE::SOLO_QUEEN],
                          2, 4, 2, 3);
            table->attach(*this->gametype_buttons[GAMETYPE::SOLO_KING],
                          2, 4, 3, 4);
            table->attach(*this->gametype_buttons[GAMETYPE::SOLO_QUEEN_JACK],
                          4, 6, 0, 1);
            table->attach(*this->gametype_buttons[GAMETYPE::SOLO_KING_JACK],
                          4, 6, 1, 2);
            table->attach(*this->gametype_buttons[GAMETYPE::SOLO_KING_QUEEN],
                          4, 6, 2, 3);
            table->attach(*this->gametype_buttons[GAMETYPE::SOLO_KOEHLER],
                          4, 6, 3, 4);
            table->attach(*this->solo_swines_button,
                          1, 3, 5, 6);
            table->attach(*this->solo_hyperswines_button,
                          3, 5, 5, 6);

            {
              auto alignment
                = Gtk::manage(new Gtk::Alignment(Gtk::ALIGN_CENTER,
                                                 Gtk::ALIGN_CENTER,
                                                 0, 0));
              this->offer_duty_solo_button
                = Gtk::manage(new Gtk::CheckButton);
              this->ui->translations->add(*this->offer_duty_solo_button,
                                          ::translation("offer duty solo"));

              alignment->add(*this->offer_duty_solo_button);
              table->attach(*alignment,
                            0, 6, 7, 8);
            }
          } // Solo
        } // set the pages of the notebook
      } // the notebook
      { // tournament info
        auto table = Gtk::manage(new Gtk::Table(1, 2));
        { // remaining games/rounds/points
          auto alignment = Gtk::manage(new Gtk::Alignment(Gtk::ALIGN_CENTER,
                                                                     Gtk::ALIGN_CENTER,
                                                                     0, 0));
          auto table2 = Gtk::manage(new Gtk::Table(3, 1));

          this->remaining_rounds_label
            = Gtk::manage(new Gtk::Label);
          this->ui->translations->add(*this->remaining_rounds_label,
                                      ::translation("remaining rounds") + ":");
          this->remaining_rounds_number
            = Gtk::manage(new Gtk::Label);
          this->remaining_rounds_number->set_label("0");
          this->remaining_games_label
            = Gtk::manage(new Gtk::Label);
          this->ui->translations->add(*this->remaining_games_label,
                                      ::translation("remaining games") + ":");
          this->remaining_games_number
            = Gtk::manage(new Gtk::Label);
          this->remaining_games_number->set_label("0");
          this->remaining_points_label
            = Gtk::manage(new Gtk::Label);
          this->ui->translations->add(*this->remaining_points_label,
                                      ::translation("remaining points") + ":");
          this->remaining_points_number
            = Gtk::manage(new Gtk::Label);
          this->remaining_points_number->set_label("0");

          this->remaining_rounds_label->set_alignment(0, 0);
          this->remaining_rounds_number->set_alignment(1, 0);
          this->remaining_games_label->set_alignment(0, 0);
          this->remaining_games_number->set_alignment(1, 0);
          this->remaining_points_label->set_alignment(0, 0);
          this->remaining_points_number->set_alignment(1, 0);
          table2->attach(*this->remaining_rounds_label,
                         0, 1, 0, 1);
          table2->attach(*this->remaining_rounds_number,
                         1, 2, 0, 1);
          table2->attach(*this->remaining_games_label,
                         0, 1, 1, 2);
          table2->attach(*this->remaining_games_number,
                         1, 2, 1, 2);
          table2->attach(*this->remaining_points_label,
                         0, 1, 2, 3);
          table2->attach(*this->remaining_points_number,
                         1, 2, 2, 3);

          table2->set_row_spacings(1 EX);
          table2->set_col_spacings(static_cast<int>(0.5 EM));
          table2->set_homogeneous(false);

          alignment->add(*table2);
          table->attach(*alignment, 0, 1, 0, 1);
        } // remaining games/rounds/points
        { // duty soli
          auto alignment = Gtk::manage(new Gtk::Alignment(Gtk::ALIGN_CENTER,
                                                                     Gtk::ALIGN_CENTER,
                                                                     0, 0));
          auto table2 = Gtk::manage(new Gtk::Table(3, 1));

          this->duty_free_soli_label
            = Gtk::manage(new Gtk::Label);
          this->ui->translations->add(*this->duty_free_soli_label,
                                      ::translation("duty free soli") + ":");
          this->duty_free_soli_number
            = Gtk::manage(new Gtk::Label);
          this->duty_free_soli_number->set_label("0");
          this->duty_color_soli_label
            = Gtk::manage(new Gtk::Label);
          this->ui->translations->add(*this->duty_color_soli_label,
                                      ::translation("duty color soli") + ":");
          this->duty_color_soli_number
            = Gtk::manage(new Gtk::Label);
          this->duty_color_soli_number->set_label("0");
          this->duty_picture_soli_label
            = Gtk::manage(new Gtk::Label);
          this->ui->translations->add(*this->duty_picture_soli_label,
                                      ::translation("duty picture soli") + ":");
          this->duty_picture_soli_number
            = Gtk::manage(new Gtk::Label);
          this->duty_picture_soli_number->set_label("0");

          this->duty_free_soli_label->set_alignment(0, 0);
          this->duty_free_soli_number->set_alignment(1, 0);
          this->duty_color_soli_label->set_alignment(0, 0);
          this->duty_color_soli_number->set_alignment(1, 0);
          this->duty_picture_soli_label->set_alignment(0, 0);
          this->duty_picture_soli_number->set_alignment(1, 0);
          table2->attach(*this->duty_free_soli_label,
                         0, 1, 0, 1);
          table2->attach(*this->duty_free_soli_number,
                         1, 2, 0, 1);
          table2->attach(*this->duty_color_soli_label,
                         0, 1, 1, 2);
          table2->attach(*this->duty_color_soli_number,
                         1, 2, 1, 2);
          table2->attach(*this->duty_picture_soli_label,
                         0, 1, 2, 3);
          table2->attach(*this->duty_picture_soli_number,
                         1, 2, 2, 3);

          table2->set_row_spacings(1 EX);
          table2->set_col_spacings(static_cast<int>(0.5 EM));
          table2->set_homogeneous(false);

          alignment->add(*table2);
          table->attach(*alignment, 1, 2, 0, 1);
        } // duty soli
        table->set_border_width(1 EX);
        this->get_vbox()->add(*table);
      } // tournament info

      { // signals
        this->announce_button->signal_clicked().connect(sigc::mem_fun(*this,
                                                                      &Reservation::announce)
                                                       );
        this->swines_button->signal_toggled().connect(sigc::mem_fun(*this, &Reservation::swines_changed));
        this->solo_swines_button->signal_toggled().connect(sigc::mem_fun(*this, &Reservation::swines_changed));
        this->hyperswines_button->signal_toggled().connect(sigc::mem_fun(*this, &Reservation::swines_changed));
        this->solo_hyperswines_button->signal_toggled().connect(sigc::mem_fun(*this, &Reservation::swines_changed));
      } // signals

      this->show_all_children();

      return ;
    } // void Reservation::init()

  /**
   ** show the reservation for selecting before it is the turn of the player
   **
   ** @param     -
   **
   ** @return    -
   **/
  void
    Reservation::show_for_selection()
    {
      if (this->announced)
        return ;

      this->realize();

      this->notebook->set_current_page(0);

      this->set_default();
      this->sensitivity_update();

      { // adjust some translations because of changed points
        this->remaining_rounds_number->set_label(DK::Utils::String::to_string(this->ui->party().remaining_rounds()));
        this->remaining_games_number->set_label(DK::Utils::String::to_string(this->ui->party().remaining_games()));
        this->remaining_points_number->set_label(DK::Utils::String::to_string(this->ui->party().remaining_points()));

        this->duty_free_soli_number->set_label(DK::Utils::String::to_string(this->player().remaining_duty_free_soli()));
        this->duty_color_soli_number->set_label(DK::Utils::String::to_string(this->player().remaining_duty_color_soli()));
        this->duty_picture_soli_number->set_label(DK::Utils::String::to_string(this->player().remaining_duty_picture_soli()));


        this->ui->translations->change(*this->bock_label,
                                       ::translation("bock: %umultiplier%",
                                                     this->ui->party().current_bock_multiplier()));
        this->ui->translations->change(*this->gametype_buttons[GAMETYPE::THROWN_NINES],
                                       ::translation("GameType::%tnumber% nines",
                                                     ::translation(DK::Utils::String::to_string(this->game().rule()(Rule::MIN_NUMBER_OF_THROWING_NINES)))));
        this->ui->translations->change(*this->gametype_buttons[GAMETYPE::THROWN_KINGS],
                                       ::translation("GameType::%tnumber% kings",
                                                     ::translation(DK::Utils::String::to_string(this->game().rule()(Rule::MIN_NUMBER_OF_THROWING_KINGS)))));
        this->ui->translations->change(*this->gametype_buttons[GAMETYPE::THROWN_NINES_AND_KINGS],
                                       ::translation("GameType::%tnumber% nines and kings",
                                                     ::translation(DK::Utils::String::to_string(this->game().rule()(Rule::MIN_NUMBER_OF_THROWING_NINES_AND_KINGS)))));
        this->ui->translations->change(*this->gametype_buttons[GAMETYPE::THROWN_RICHNESS],
                                       ::translation("GameType::%upoints% richness",
                                                     this->game().rule()(Rule::MIN_RICHNESS_FOR_THROWING)));

        if (this->player().hand().numberofclubqueens()
            == this->game().rule()(Rule::NUMBER_OF_SAME_CARDS))
          this->ui->translations->change(*this->gametype_buttons[GAMETYPE::NORMAL],
                                         ::translation(::GAMETYPE::MARRIAGE_SILENT));
        else
          this->ui->translations->change(*this->gametype_buttons[GAMETYPE::NORMAL],
                                         ::translation(::GAMETYPE::NORMAL));
      } // adjust some translations
      if (this->ui->party().current_bock_multiplier() != 1)
        this->bock_label->show();
      else
        this->bock_label->hide();

      this->show();

      return ;
    } // void Reservation::show_for_selection()

  /**
   ** gets a reservation
   **
   ** @param     -
   **
   ** @return    reservation
   **/
  ::Reservation
    Reservation::get()
    {
      if (!this->announced) {

        if (!this->is_visible())
          this->show_for_selection();

        do {
          if (!this->is_visible())
            this->present();

          while (!this->ui->thrower
                 && this->is_visible())
            ::ui->wait();
        } while (!this->announce_button->is_sensitive()
                 && !this->ui->thrower) ;
      } // if (!this->announced)

      this->game().set_type(GAMETYPE::NORMAL);

      if (!this->announced)
        this->announce();

      this->announced = false;

      return this->player().reservation();
    } // ::Reservation Reservation::get()

  /**
   ** nothing (dummy function)
   **
   ** @param     cr       drawing context
   ** @param     update   whether to update the area
   **
   ** @return    -
   **/
  void
    Reservation::draw(Cairo::RefPtr<::Cairo::Context> cr, bool const update)
    {
      // this function is needed because it is virtual in base class 'HTIN'
      return ;
    } // void Reservation::draw(bool update)

  /**
   ** set to default values
   **
   ** @param     -
   **
   ** @return    -
   **/
  void
    Reservation::set_default()
    {
      this->in_update = true;

      ::Reservation const& reservation
        = this->player().reservation_get_default();

      this->swines_button->set_active(reservation.swines);
      this->hyperswines_button->set_active(reservation.hyperswines);
      this->solo_swines_button->set_active(false);
      this->solo_hyperswines_button->set_active(false);
      this->offer_duty_solo_button->set_active(false);

      // special case: with swines announced the player does not have a poverty
      if (   (reservation.game_type == GAMETYPE::POVERTY)
          && (this->player().hand().numberofpovertycards()
              > this->game().rule()(Rule::MAX_NUMBER_OF_POVERTY_TRUMPS)) )
        this->swines_button->set_active(false);

      this->marriage_selector_buttons[reservation.marriage_selector]->set_active();
      this->gametype_buttons[reservation.game_type]->set_active();

      this->name().draw();
      this->table().name(this->game().startplayer()).draw();

      this->in_update = false;

      this->gametype_changed(reservation.game_type);

      { // switch to the game type from the bug report replay
        if (   ::bug_report_replay
            && !::bug_report_replay->finished()
            && (::bug_report_replay->soloplayer_no() == this->game().no(this->player()) )) {

          this->gametype_buttons[::bug_report_replay->game_type()]->set_active();
          if (::bug_report_replay->game_type() == GAMETYPE::MARRIAGE) {
            this->marriage_selector_buttons[::bug_report_replay->marriage_selector()]->set_active();
          }

          if (GAMETYPE::is_solo(::bug_report_replay->game_type()))
            this->notebook->set_current_page(1);
        }
      } // switch to the game type from the bug report replay

      this->swines_changed();

      return ;
    } // void Reservation::set_default()

  /**
   ** update the sensitivity
   **
   ** @param     -
   **
   ** @return    -
   **/
  void
    Reservation::sensitivity_update()
    {
      if (!this->get_realized())
        return ;

      if (::game_status != GAMESTATUS::GAME_RESERVATION)
        return ;

      if (this->in_update)
        return ;

      ::Player const& player = this->player();
      ::Game const& game = player.game();
      ::Rule const& rule = game.rule();
      ::Hand const& hand = player.hand();

      if (   rule(Rule::SWINES)
          && !rule(Rule::SWINE_ONLY_SECOND)) {
        this->swines_button->show();
        this->solo_swines_button->show();
      } else {
        this->swines_button->hide();
        this->solo_swines_button->hide();
      }
      if (!rule(Rule::SWINES_IN_SOLO))
        this->solo_swines_button->hide();
      this->swines_button->set_sensitive(game.swines_announcement_valid(player));
      this->solo_swines_button->set_sensitive(GAMETYPE::is_color_solo(game.type())
                                              && game.swines_announcement_valid(player));

      if (rule(Rule::HYPERSWINES)) {
        this->hyperswines_button->show();
        this->solo_hyperswines_button->show();
      } else {
        this->hyperswines_button->hide();
        this->solo_hyperswines_button->hide();
      }
      if (!rule(Rule::HYPERSWINES_IN_SOLO))
        this->solo_hyperswines_button->hide();
      this->hyperswines_button->set_sensitive(game.hyperswines_announcement_valid(player)
                                              && this->swines_button->get_active());
      this->solo_hyperswines_button->set_sensitive(GAMETYPE::is_color_solo(game.type())
                                                   && game.hyperswines_announcement_valid(player)
                                                   && this->solo_swines_button->get_active());
      if (::setting(Setting::ANNOUNCE_SWINES_AUTOMATICALLY)) {
        this->swines_button->set_sensitive(false);
        this->hyperswines_button->set_sensitive(false);
        this->solo_swines_button->set_sensitive(false);
        this->solo_hyperswines_button->set_sensitive(false);
      } // if (::setting(Setting::ANNOUNCE_SWINES_AUTOMATICALLY))

      for (auto widget : this->gametype_buttons) {
        if (rule(*(static_cast<GameType*>(widget->get_data("gametype")))))
          widget->show();
        else
          widget->hide();
      } // for (widget)

      if (rule(Rule::SOLO))
        this->notebook->get_nth_page(1)->show();
      else
        this->notebook->get_nth_page(1)->hide();

      for (auto widget : this->marriage_selector_buttons) {
        if (rule(*(static_cast<MarriageSelector*>(widget->get_data("selector")))))
          widget->show();
        else
          widget->hide();
      } // for (widget)

      this->gametype_buttons[GAMETYPE::THROWN_RICHNESS]->set_sensitive((hand.points() >= rule(Rule::MIN_RICHNESS_FOR_THROWING))
                                                                       && rule(GAMETYPE::THROWN_RICHNESS));
      this->gametype_buttons[GAMETYPE::THROWN_NINES_AND_KINGS]->set_sensitive((hand.numberofnines() + hand.numberofkings() >= rule(Rule::MIN_NUMBER_OF_THROWING_NINES_AND_KINGS))
                                                                              && rule(GAMETYPE::THROWN_NINES_AND_KINGS));
      this->gametype_buttons[GAMETYPE::THROWN_KINGS]->set_sensitive((hand.numberofkings() >= rule(Rule::MIN_NUMBER_OF_THROWING_KINGS))
                                                                    && rule(GAMETYPE::THROWN_KINGS));
      this->gametype_buttons[GAMETYPE::THROWN_NINES]->set_sensitive((hand.numberofnines() >= rule(Rule::MIN_NUMBER_OF_THROWING_NINES))
                                                                    && rule(GAMETYPE::THROWN_NINES));
      this->gametype_buttons[GAMETYPE::POVERTY]->set_sensitive(hand.has_poverty());

      { // look, whether there is a card that is higher than the fox
        unsigned c;
        for (c = 0; c < player.hand().cardsnumber(); c++)
          // check with diamond ten because of swines!
          if (   Card(Card::DIAMOND, Card::TEN).less(player.hand().card(c))
              && !player.hand().card(c).istrumpace())
            break;
        this->gametype_buttons[GAMETYPE::FOX_HIGHEST_TRUMP]->set_sensitive((c == player.hand().cardsnumber())
                                                                           && rule(GAMETYPE::POVERTY));
      } // look, whether there is a card that is higher than the fox
      this->gametype_buttons[GAMETYPE::MARRIAGE]->set_sensitive((hand.numberofclubqueens() == rule(Rule::NUMBER_OF_SAME_CARDS)));

      for (auto widget : this->marriage_selector_buttons) {
        auto const selected = this->gametype_buttons[GAMETYPE::MARRIAGE]->get_active();
        widget->set_sensitive(selected);
        //this->gametype_buttons[GAMETYPE::MARRIAGE]->sensitive());
      }

      { // duty soli
        if (   game.is_duty_solo()
            && (game.startplayer() == player)) {
          this->announce_button->set_sensitive(false);
          this->notebook->set_current_page(1);
          this->notebook->get_nth_page(0)->set_sensitive(false);
          for (auto widget : this->gametype_buttons) {
            auto const game_type
              = *static_cast<GameType*>(widget->get_data("gametype"));
            widget->set_sensitive(   (   is_solo(game_type)
                                      && player.remaining_duty_free_soli())
                                  || (   is_color_solo(game_type)
                                      && player.remaining_duty_color_soli())
                                  || (   is_picture_solo(game_type)
                                      && player.remaining_duty_picture_soli())
                                 );
            if (   widget->is_sensitive()
                && widget->get_active())
              this->announce_button->set_sensitive(true);

          } // for (widget)
        } // if (duty solo)

        if (rule(Rule::OFFER_DUTY_SOLO)
            && game.is_duty_solo()
            && (game.startplayer() == player))
          this->offer_duty_solo_button->show();
        else
          this->offer_duty_solo_button->hide();

        if (rule(Rule::NUMBER_OF_ROUNDS_LIMITED)) {
          this->remaining_rounds_label->show();
          this->remaining_rounds_number->show();
          this->remaining_games_label->show();
          this->remaining_games_number->show();
        } else {
          this->remaining_rounds_label->hide();
          this->remaining_rounds_number->hide();
          this->remaining_games_label->hide();
          this->remaining_games_number->hide();
        }
        if (rule(Rule::POINTS_LIMITED)) {
          this->remaining_points_label->show();
          this->remaining_points_number->show();
        } else {
          this->remaining_points_label->hide();
          this->remaining_points_number->hide();
        }
        if (  rule(Rule::NUMBER_OF_DUTY_SOLI)
            - rule(Rule::NUMBER_OF_DUTY_COLOR_SOLI)
            - rule(Rule::NUMBER_OF_DUTY_PICTURE_SOLI)
            > 0) {
          this->duty_free_soli_label->show();
          this->duty_free_soli_number->show();
        } else {
          this->duty_free_soli_label->hide();
          this->duty_free_soli_number->hide();
        }
        if (rule(Rule::NUMBER_OF_DUTY_COLOR_SOLI)) {
          this->duty_color_soli_label->show();
          this->duty_color_soli_number->show();
        } else {
          this->duty_color_soli_label->hide();
          this->duty_color_soli_number->hide();
        }
        if (rule(Rule::NUMBER_OF_DUTY_PICTURE_SOLI)) {
          this->duty_picture_soli_label->show();
          this->duty_picture_soli_number->show();
        } else {
          this->duty_picture_soli_label->hide();
          this->duty_picture_soli_number->hide();
        }
      } // duty soli

      return ;
    } // void Reservation::sensitivity_update()

  /**
   ** the gametype has changed
   **
   ** @param     gametype_   changed game type
   **
   ** @return    -
   **/
  void
    Reservation::gametype_changed(int const gametype_)
    {
      if (!this->get_realized())
        return ;

      auto const gametype = static_cast<GameType>(gametype_);
      if (!this->gametype_buttons[gametype]->get_active())
        return ;
      if (this->in_update)
        return ;

      this->in_update = true;

      ::Player const& player = this->player();
      ::Game const& game = player.game();
      ::Rule const& rule = game.rule();

      if (gametype == GAMETYPE::MARRIAGE) {
        bool const selected = this->gametype_buttons[gametype]->get_active();
        for (auto widget : this->marriage_selector_buttons)
          widget->set_sensitive(selected);
      } // if (gametype == GAMETYPE::MARRIAGE)

      // '::party.game()' here, because the game has to be changed,
      // 'this->game()' is a const game
      //GameType const old_gametype = this->>game().type();
      this->game().set_type(static_cast<GameType>(gametype));

      { // update the swines
        ::Hand const& hand = player.hand();

        this->swines_button->set_sensitive(game.swines_announcement_valid(player));
        this->swines_button->set_active(this->swines_button->is_sensitive()
                                        && rule(Rule::SWINES_ANNOUNCEMENT_BEGIN)
                                        && ((game.type() != GAMETYPE::POVERTY)
                                            || (hand.numberoftrumps() <= rule(Rule::MAX_NUMBER_OF_POVERTY_TRUMPS)))
                                       );
        this->hyperswines_button->set_sensitive(game.hyperswines_announcement_valid(player)
                                                && this->swines_button->get_active());
        this->hyperswines_button->set_active(this->hyperswines_button->is_sensitive()
                                             && rule(Rule::HYPERSWINES_ANNOUNCEMENT_BEGIN));
        this->solo_swines_button->set_sensitive(game.swines_announcement_valid(player));
        this->solo_swines_button->set_active(this->solo_swines_button->is_sensitive()
                                             && rule(Rule::SWINES_ANNOUNCEMENT_BEGIN)
                                             && ((game.type() != GAMETYPE::POVERTY)
                                                 || (hand.numberoftrumps() <= rule(Rule::MAX_NUMBER_OF_POVERTY_TRUMPS)))
                                            );
        this->solo_hyperswines_button->set_sensitive(game.hyperswines_announcement_valid(player)
                                                     && this->solo_swines_button->get_active());
        this->solo_hyperswines_button->set_active(this->solo_hyperswines_button->is_sensitive()
                                                  && rule(Rule::HYPERSWINES_ANNOUNCEMENT_BEGIN));

        if (::setting(Setting::ANNOUNCE_SWINES_AUTOMATICALLY)) {
          this->swines_button->set_sensitive(false);
          this->hyperswines_button->set_sensitive(false);
          this->solo_swines_button->set_sensitive(false);
          this->solo_hyperswines_button->set_sensitive(false);
        } // if (::setting(Setting::ANNOUNCE_SWINES_AUTOMATICALLY))
      } // update the swines

      { // update the reservation of the player
        ::Reservation& reservation = this->player().reservation();

        reservation.game_type = static_cast<GameType>(gametype);
        if (   (player.hand().numberofclubqueens()
                == game.rule()(Rule::NUMBER_OF_SAME_CARDS))
            && (gametype == GAMETYPE::NORMAL))
          reservation.game_type = GAMETYPE::MARRIAGE_SILENT;
        for (auto widget : this->marriage_selector_buttons)
          if (widget->get_active())
            reservation.marriage_selector
              = *(static_cast<MarriageSelector*>(widget->get_data("selector")));
        // swines are updated in the following 'this->swines_changed()'
      } // update the reservation of the player

      this->icongroup().draw_team();
      this->name().draw();
      this->table().name(game.startplayer()).draw();

      if (    game.is_duty_solo()
          && (game.startplayer() == player))
        this->announce_button->set_sensitive(is_real_solo(gametype));

      this->in_update = false;

      this->swines_changed();

      this->game().players_hands_sort();

      return ;
    } // void Reservation::gametype_changed(int gametype)

  /**
   ** the marriage selector has changed
   **
   ** @param     marriage_selector_   changed marriage selector
   **
   ** @return    -
   **/
  void
    Reservation::marriage_selector_changed(int const marriage_selector_)
    {
      if (!this->get_realized())
        return ;

      this->player().reservation().marriage_selector
        = static_cast<MARRIAGE_SELECTOR::MarriageSelector>(marriage_selector_);
      this->icongroup().draw_team();

      return ;
    } // void Reservation::marriage_selector_changed(int marriage_selector_)

  /**
   ** the selection of swines/hyperswines has changed
   **
   ** @param     -
   **
   ** @return    -
   **
   ** @bug       the cards order is not updated (problem: information flow)
   **/
  void
    Reservation::swines_changed()
    {
      if (!this->get_realized())
        return ;

      if (this->in_update)
        return ;

      ::Reservation& reservation = this->player().reservation();

      bool differ = (   (reservation.swines
                         != (GAMETYPE::is_solo(this->game().type())
                             ? this->solo_swines_button->get_active()
                             : this->swines_button->get_active()))
                     || (reservation.hyperswines
                         != (GAMETYPE::is_solo(this->game().type())
                             ? this->solo_hyperswines_button->get_active()
                             : this->hyperswines_button->get_active())));
      if (!differ)
        return ;

      reservation.swines
        = (GAMETYPE::is_solo(this->game().type())
           ? this->solo_swines_button->get_active()
           : this->swines_button->get_active());
      reservation.hyperswines
        = (GAMETYPE::is_solo(this->game().type())
           ? this->solo_hyperswines_button->get_active()
           : this->hyperswines_button->get_active());

      this->sensitivity_update();
      this->icongroup().draw_swines();
      this->game().players_hands_sort();

      return ;
    } // void Reservation::swines_changed()

  /**
   ** update in game: gametype, swines, hyperswines according to the selected reservation
   ** Used for the 'early' reservation, when the human can select a reservation before it is his turn. When a player before has announced, the game is resetted (gametype normal, no swines). So we have to adjust here, in order to have the cards order not changed.
   **
   ** @param     -
   **
   ** @return    -
   **/
  void
    Reservation::update_for_reservation()
    {
      this->game().set_type(this->player().reservation().game_type);
      this->swines_changed();

      return ;
    } // void Reservation::update_for_reservation()


  /**
   ** update the reservation of the player
   **
   ** @param     -
   **
   ** @return    -
   **/
  void
    Reservation::update_player_reservation()
    {
      if (!this->get_realized())
        return ;

      ::Reservation& reservation = this->player().reservation();

      for (auto widget : this->gametype_buttons)
        if (widget->get_active())
          reservation.game_type
            = *(static_cast<GameType*>(widget->get_data("gametype")));

      //reservation.game_type = static_cast<GameType>(gametype);
      for (auto widget : this->marriage_selector_buttons)
        if (widget->get_active())
          reservation.marriage_selector
            = *(static_cast<MarriageSelector*>(widget->get_data("selector")));

      reservation.swines
        = (GAMETYPE::is_solo(this->game().type())
           ? this->solo_swines_button->get_active()
           : this->swines_button->get_active());
      reservation.hyperswines
        = (GAMETYPE::is_solo(this->game().type())
           ? this->solo_hyperswines_button->get_active()
           : this->hyperswines_button->get_active());

      reservation.offer_duty_solo
        = this->offer_duty_solo_button->get_active();

      return ;
    } // void Reservation::update_player_reservation()


  /**
   ** page change
   ** if the user switches to the first page, activate the default reservation
   **
   ** @param     page     the new page
   ** @param     pageno   the number of the page
   **
   ** @return    -
   **/
  void
    Reservation::switch_page_event(Widget*, guint const pageno)
    {
#ifdef WORKAROUND
      // else the 'this->is_visible()' segfaults when starting a new tournament
      if (::game_status != GAMESTATUS::GAME_RESERVATION)
        return ;
#endif
      if (!this->is_visible())
        return ;

      if (::bug_report_replay) {
        if (   (pageno == 0)
            && GAMETYPE::is_solo(this->game().type())
            && !GAMETYPE::is_solo(::bug_report_replay->game_type()))
          this->set_default();
        if (   (pageno == 1)
            && !GAMETYPE::is_solo(this->game().type())
            && GAMETYPE::is_solo(::bug_report_replay->game_type()))
          this->set_default();
      } // if (::bug_report_replay)

      if (!::bug_report_replay) {
        if (pageno == 0) {
          if (GAMETYPE::is_solo(this->game().type())) {
            // reset the reservation
            this->game().set_type(GAMETYPE::NORMAL);
            if (!GAMETYPE::is_solo(this->player().reservation_get_default().game_type))
              this->set_default();
          }
        } // if (pageno == 0)
      } // if (!::bug_report_replay)

      return ;
    } // void Reservation::switch_page_event(Widget*, guint pageno)

  /**
   ** announce the reservation
   **
   ** @param     -
   **
   ** @return    -
   **/
  void
    Reservation::announce()
    {
      if (!this->announce_button->is_sensitive())
        return ;

      this->update_player_reservation();

      this->hide();
#ifdef WINDOWS
#ifdef WORKAROUND
      this->ui->main_window->raise();
#endif
#endif

      this->announced = true;

      return ;
    } // void Reservation::announce()

} // namespace UI_GTKMM_NS

#endif // #ifdef USE_UI_GTKMM
