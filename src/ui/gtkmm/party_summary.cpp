/**********************************************************************
 *
 *   FreeDoko a Doppelkopf-Party
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

#include "party_summary.h"

#include "ui.h"
#include "translations.h"
#include "main_window.h"
#include "program_updated.h"
#include "bug_report_replay.h"

#include "../../party/party.h"
#include "../../party/rule.h"
#include "../../os/bug_report_replay.h"

#include "widgets/stock_button.h"
#include <gtkmm/button.h>
#include <gtkmm/alignment.h>
#include <gtkmm/label.h>
#include <gtkmm/main.h>
#include <gtkmm/treeview.h>
#include <gtkmm/stock.h>

namespace UI_GTKMM_NS {

  /**
   ** constructor
   **
   ** @param     parent   the parent object
   **
   ** @return    -
   **/
  PartySummary::PartySummary(Base* const parent) :
    Base(parent),
    StickyDialog("PartySummary", *(parent->ui->main_window), false)
  {
    this->ui->add_window(*this);

    this->signal_realize().connect(sigc::mem_fun(*this, &PartySummary::init));
#ifndef GLIBMM_DEFAULT_SIGNAL_HANDLERS_ENABLED
    this->signal_show().connect(sigc::mem_fun(*this, &PartySummary::on_show));
#endif

    return ;
  } // PartySummary::PartySummary(Base* parent)

  /**
   ** destructor
   **
   ** @param     -
   **
   ** @return    -
   **/
  PartySummary::~PartySummary()
  { }

  /**
   ** create all subelements
   **
   ** @param     -
   **
   ** @return    -
   **/
  void
    PartySummary::init()
    {
      this->set_icon(this->ui->icon);
      this->ui->translations->add(*this,
				  ::translation("~party summary"));

      this->set_position(Gtk::WIN_POS_CENTER_ON_PARENT);


      this->get_vbox()->set_spacing(1 EX);

      { // action area
	this->new_party_button
	  = Gtk::manage(new Gtk::StockButton("new party"));
	this->ui->translations->add(*this->new_party_button->label,
				    ::translation("new party"));
	this->add_action_widget(*this->new_party_button, Gtk::RESPONSE_CLOSE);
	this->new_party_button->signal_clicked().connect(sigc::mem_fun(*this,
								    &PartySummary::new_party_event)
							);

	this->continue_party_button
	  = Gtk::manage(new Gtk::StockButton(Gtk::Stock::OK,
					     "continue party"));
	this->ui->translations->add(*this->continue_party_button->label,
				    ::translation("continue party"));
	this->add_action_widget(*this->continue_party_button, Gtk::RESPONSE_CLOSE);
	this->continue_party_button->signal_clicked().connect(sigc::mem_fun(*this,
									 &PartySummary::continue_party_event)
							     );
	this->continue_party_button->grab_default();
	this->continue_party_button->grab_focus();
      } // action area

      {
	this->get_vbox()->pack_start(*Gtk::manage(new Gtk::Alignment()));
      }
      {
	this->roundnumber = Gtk::manage(new Gtk::Label("Roundnumber:"));
	this->ui->translations->add(*this->roundnumber,
				    ::translation("Roundnumber: %uroundno%",
						 0u));
	this->get_vbox()->pack_start(*this->roundnumber, false, true);
      }
      {
	this->gamenumber = Gtk::manage(new Gtk::Label("Gamenumber:"));
	this->ui->translations->add(*this->gamenumber,
				    ::translation("Gamenumber: %ugameno%",
						 0u));
	this->get_vbox()->pack_start(*this->gamenumber, false, true);
      }
      {
	this->remaining_rounds = Gtk::manage(new Gtk::Label("Remaining rounds:"));
	this->ui->translations->add(*this->remaining_rounds,
				    ::translation("Remaining rounds: %urounds%",
						 0u));
	this->get_vbox()->pack_start(*this->remaining_rounds, false, true);
      }
      {
	this->remaining_points = Gtk::manage(new Gtk::Label("Remaining points:"));
	this->ui->translations->add(*this->remaining_points,
				    ::translation("Remaining points: %ipoints%",
						 0));
	this->get_vbox()->pack_start(*this->remaining_points, false, true);
      }
      {
	this->next_startplayer = Gtk::manage(new Gtk::Label("Next startplayer:"));
	this->ui->translations->add(*this->next_startplayer,
				    ::translation("Next startplayer: %sname%",
						 "Name"));
	this->get_vbox()->pack_end(*this->next_startplayer, false, true);
      }

      { // player
	this->player_model
	  = std::make_unique<PlayersSummaryModel>(this->ui->party().playerno());
	this->player_list
	  = Gtk::ListStore::create(*this->player_model);
	this->player_treeview
	  = Gtk::manage(new Gtk::TreeView(this->player_list));
	this->player_treeview->get_selection()->set_mode(Gtk::SELECTION_NONE);

	this->player_treeview->append_column("", this->player_model->name);
	for (unsigned p = 0; p < this->ui->party().playerno(); ++p) {
	  this->player_treeview->append_column("Name", this->player_model->player[p]);
	  this->player_treeview->get_column_cell_renderer(1 + p)->set_property("xalign", 0.5);
	}

	auto alignment
	  = Gtk::manage(new Gtk::Alignment(Gtk::ALIGN_CENTER,
					   Gtk::ALIGN_CENTER,
					   0, 0));
	alignment->add(*this->player_treeview);
	this->get_vbox()->pack_start(*alignment, false, true);
      } // player


      this->show_all_children();
      this->update();
      this->language_update();

      return ;
    } // void PartySummary::init()

  /**
   ** A party is loaded.
   ** Show this window.
   **
   ** @param     -
   **
   ** @return    -
   **/
  void
    PartySummary::party_loaded()
    {
      if (!this->get_realized())
	this->realize();

      this->update();

      if (::game_status == GAMESTATUS::PARTY_INITIAL_LOADED)
	this->new_party_button->show();
      else
	this->new_party_button->hide();

      this->present();
      if (this->ui->bug_report_replay->is_visible())
        this->ui->bug_report_replay->raise();

      while (   (this->is_visible())
             && !this->ui->thrower) {
        ::ui->wait();
        if (::bug_report_replay
            && ::bug_report_replay->auto_start_party())
          break;
      }

      this->hide();

      return ;
    } // void PartySummary::party_loaded()

  /**
   ** show the window.
   ** Shows/hides the action buttons
   **
   ** @param     -
   **
   ** @return    -
   **/
  void
    PartySummary::on_show()
    {
#ifdef GLIBMM_DEFAULT_SIGNAL_HANDLERS_ENABLED
      this->Dialog::on_show();
#endif

      if (this->ui->program_updated_window)
        this->ui->program_updated_window->raise();

      this->update();

      return ;
    } // void PartySummary::show()

  /**
   ** update (rewrite) all information
   **
   ** @param     -
   **
   ** @return    -
   **
   ** @todo	remaining points
   ** @todo	duty soli
   **/
  void
    PartySummary::update()
    {
      if (!this->get_realized())
        return ;

      if (   (::game_status != GAMESTATUS::PARTY_INITIAL_LOADED)
          && (::game_status != GAMESTATUS::PARTY_LOADED))
        return ;

      Party const& party = this->ui->party();

      this->ui->translations->change(*this->roundnumber,
                                     ::translation("Roundnumber: %uroundno%",
                                                   party.roundno()));
      this->ui->translations->change(*this->gamenumber,
                                     ::translation("Gamenumber: %ugameno%",
                                                   party.gameno()));
      if (party.rule()(Rule::NUMBER_OF_ROUNDS_LIMITED))
        this->ui->translations->change(*this->remaining_rounds,
                                       ::translation("Remaining rounds: %urounds%",
                                                     party.remaining_rounds()));
      else
        this->remaining_rounds->hide();
      if (party.rule()(Rule::POINTS_LIMITED))
        this->ui->translations->change(*this->remaining_points,
                                       ::translation("Remaining points: %ipoints%",
                                                     party.remaining_points()));
      else
        this->remaining_points->hide();

      this->ui->translations->change(*this->next_startplayer,
                                     ::translation("Next startplayer: %sname%",
                                                   party.player(party.startplayer()).name()));

      this->ui->translations->change(*this->continue_party_button->label,
                                     party.is_finished()
                                     ? ::translation("finish party")
                                     : ::translation("continue party"));

      { // player
        this->player_list->clear();

        for (unsigned p = 0; p < party.playerno(); ++p)
          this->player_treeview->get_column(1 + p)->set_title(party.player(p).name());

        auto points_row = *(this->player_list->append());
        points_row[this->player_model->name]
          = ::translation("points").translation();
        for (unsigned p = 0; p < party.playerno(); ++p)
          points_row[this->player_model->player[p]] = party.pointsum(p);

        if (party.rule()(Rule::NUMBER_OF_DUTY_SOLI)) {
          Gtk::TreeModel::Row duty_soli_row = *(this->player_list->append());
          duty_soli_row[this->player_model->name]
            = ::translation("duty soli").translation();
          for (unsigned p = 0; p < party.playerno(); ++p)
            duty_soli_row[this->player_model->player[p]]
              = party.remaining_duty_soli(p);
          // points_row[this->player_model->player[p]] = party.pointsum(p);
        } // if (party.rule()(Rule::NUMBER_OF_DUTY_SOLI))
      } // player

      return ;
    } // void PartySummary::update()

  /**
   ** the name of 'player' has changed
   **
   ** @param     player   the player with the changed name
   **
   ** @return    -
   **/
  void
    PartySummary::name_update(Player const& player)
    {
      if (!this->get_realized())
        return ;

      unsigned const p = this->ui->party().no(player);
      if (p + 1 >= this->player_treeview->get_columns().size())
        return ;

      this->player_treeview->get_column(p + 1)->set_title(player.name());

      return ;
    } // void PartySummary::name_update(Player const& player)

  /**
   ** updates all language elements
   **
   ** @param     -
   **
   ** @return    -
   **/
  void
    PartySummary::language_update()
    {
      if (!this->get_realized())
        return ;

      this->update();

      return ;
    } // void PartySummary::language_update()

  /**
   ** event, that the party shall be continued
   **
   ** @param     -
   **
   ** @return    -
   **/
  void
    PartySummary::continue_party_event()
    {
      this->hide();
      this->ui->main_quit();

      return ;
    } // void Party
  /**
   ** event, that a new party shall be started
   **
   ** @param     -
   **
   ** @return    -
   **/
  void
    PartySummary::new_party_event()
    {
      this->ui->thrower(GAMESTATUS::PARTY_NEW, __FILE__, __LINE__);

      this->hide();
      this->ui->main_quit();

      return ;
    } // void PartySummary::new_party_event()

} // namespace UI_GTKMM_NS

#endif // #ifdef USE_UI_GTKMM
