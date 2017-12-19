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

#include "party_points.h"
#include "game_overview.h"
#include "party_points.graph.h"

#include "ui.h"
#include "translations.h"
#include "cards.h"
#include "icons.h"
#include "bug_report.h"

#include "../../party/party.h"
#include "../../party/rule.h"
#include "../../game/game_summary.h"
#include "../../misc/setting.h"

#include "widgets/stock_button.h"
#include <gtkmm/stock.h>
#include <gtkmm/notebook.h>
#include <gtkmm/treeview.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/alignment.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/radiobutton.h>
#include <gtkmm/label.h>
#include <gtkmm/main.h>
#include <gdk/gdkkeysyms.h>


//#define OFFSET_ROUND	0
//#define OFFSET_GAMENO	(OFFSET_ROUND + 1)
//#define OFFSET_PLAYER	(OFFSET_GAMENO + 1)
#define OFFSET_PLAYER	1
#ifdef POSTPONED
#define OFFSET_GAMEPOINTS	(OFFSET_PLAYER + this->ui->party().playerno())
#else
#define OFFSET_GAMEPOINTS	(OFFSET_PLAYER + ::party.playerno())
#endif
#define OFFSET_BOCK_MULTIPLIER	(OFFSET_GAMEPOINTS + 1)
#define OFFSET_GAMETYPE	        (OFFSET_BOCK_MULTIPLIER + 1)
namespace UI_GTKMM_NS {

/**
 ** Constructor for the model
 **
 ** @param     playerno   number of players in the party
 **
 ** @return    -
 **
 ** @todo	the row can be deselected
 **/
PartyPoints::PartyPointsModel::PartyPointsModel(unsigned const playerno) :
  playerpoints(playerno)
{
  this->add(this->empty);
  this->add(this->round);
  this->add(this->round_str);
  this->add(this->round_color);
  this->add(this->gameno);
  this->add(this->gameno_str);
  for (auto& p : this->playerpoints)
    this->add(p);
  this->add(this->gamepoints_str);
  this->add(this->bock_multiplier_str);
  this->add(this->gametype);

  return ;
} // PartyPoints::PartyPointsModel::PartyPointsModel(unsigned const playerno)

/**
 ** constructor
 **
 ** @param     parent   the parent object
 **
 ** @return    -
 **/
PartyPoints::PartyPoints(Base* const parent) :
  Base(parent),
  StickyDialog("Party points", false),
  party_points_model(1000) 
{
  this->ui->add_window(*this);

  this->signal_realize().connect(sigc::mem_fun(*this, &PartyPoints::init));
#ifndef GLIBMM_DEFAULT_SIGNAL_HANDLERS_ENABLED
  this->signal_key_press_event().connect(sigc::mem_fun(*this, &PartyPoints::on_key_press_event));
#endif

#ifdef WORKAROUND
  // remove when 'GamePoints' doesn't need a pointer to Game any more
  this->realize();
#endif

  return ;
} // PartyPoints::PartyPoints(Base* parent)

/**
 ** Destruktor
 **
 ** @param     -
 **
 ** @return    -
 **/
PartyPoints::~PartyPoints()
{ }

/**
 ** create all subelements
 **
 ** @param     -
 **
 ** @return    -
 **/
void
PartyPoints::init()
{
  this->ui->translations->add(*this, ::translation("~party points"));

  this->set_icon(this->ui->icon);

  this->game_overview = std::make_unique<GameOverview>(this);
  this->signal_show().connect(sigc::mem_fun(*this,
                                            &PartyPoints::show_game_overview_toggled));

  { // notebook
    this->notebook = Gtk::manage(new Gtk::Notebook());

    { // table
      auto table_vbox = Gtk::manage(new Gtk::VBox(false));
      table_vbox->set_border_width(1 EX);
      table_vbox->set_spacing(1 EX);

      { // the points table
        // of the sum treeview only the header is neeeded
        // (workaround since there exists no foot for a treeview)
        this->duty_soli_list
          = Gtk::TreeStore::create(this->party_points_model);
        this->party_points_sum_treeview
          = Gtk::manage(new Gtk::TreeView(this->duty_soli_list));
        this->duty_soli_treeview = this->party_points_sum_treeview;

        this->party_points_list
          = Gtk::TreeStore::create(this->party_points_model);
        this->party_points_treeview
          = Gtk::manage(new Gtk::TreeView(this->party_points_list));
        this->party_points_treeview->get_selection()->set_mode(Gtk::SELECTION_SINGLE);

        this->party_points_treeview->append_column("",
                                                   this->party_points_model.empty);
        this->party_points_sum_treeview->append_column("",
                                                       this->party_points_model.empty);
        this->party_points_sum_treeview->get_column(0)->set_alignment(Gtk::ALIGN_END);

        for (unsigned p = 0; p < ::party.playerno(); p++) {
          this->party_points_treeview->append_column(::party.player(p).name(),
                                                     this->party_points_model.playerpoints[p]);
          this->party_points_treeview->get_column_cell_renderer(p + OFFSET_PLAYER)->set_property("xalign", 0.5);
          this->party_points_treeview->get_column(p + OFFSET_PLAYER)->set_alignment(Gtk::ALIGN_CENTER);
          this->party_points_sum_treeview->append_column("0",
                                                         this->party_points_model.playerpoints[p]);
          this->party_points_sum_treeview->get_column(p + OFFSET_PLAYER)->set_resizable(false);
          this->duty_soli_treeview->get_column(p + OFFSET_PLAYER)->set_alignment(Gtk::ALIGN_CENTER);
        } // for (p < ::party.playerno())
        this->party_points_treeview->append_column("game points",
                                                   this->party_points_model.gamepoints_str);
        this->party_points_treeview->get_column_cell_renderer(OFFSET_GAMEPOINTS)->set_property("xalign", 0.5);
        this->party_points_sum_treeview->append_column("",
                                                       this->party_points_model.gamepoints_str);
        this->party_points_sum_treeview->get_column(OFFSET_GAMEPOINTS)->set_alignment(Gtk::ALIGN_CENTER);

        this->party_points_treeview->append_column("bock",
                                                   this->party_points_model.bock_multiplier_str);
        this->party_points_treeview->get_column_cell_renderer(OFFSET_BOCK_MULTIPLIER)->set_property("xalign", 0.5);
        this->party_points_sum_treeview->append_column("",
                                                       this->party_points_model.bock_multiplier_str);
        this->party_points_sum_treeview->get_column(OFFSET_BOCK_MULTIPLIER)->set_alignment(Gtk::ALIGN_CENTER);

        this->party_points_treeview->append_column("game type",
                                                   this->party_points_model.gametype);
        this->party_points_sum_treeview->append_column("",
                                                       this->party_points_model.gametype);


#ifdef LINUX
        this->party_points_treeview->set_rules_hint(true);
#endif
        for (unsigned c = 0;
             c < this->party_points_treeview->get_columns().size();
             ++c) {
          auto column = this->party_points_treeview->get_column(c);
#ifdef GLIBMM_PROPERTIES_ENABLED
          column->property_width().signal_changed().connect(sigc::mem_fun(*this, &PartyPoints::update_player_columns_size));
          //column->property_width().signal_changed().connect(sigc::mem_fun(*this, &PartyPoints::sum_columns_size_update));
#else
          column->connect_property_changed("width", sigc::mem_fun(*this, &PartyPoints::update_player_columns_size));
#endif
#ifdef WINDOWS
          column->add_attribute(*column->get_first_cell(),
                                "background", 3);
#endif
          column->set_cell_data_func(*column->get_first_cell(),
                                     sigc::bind<unsigned>(sigc::mem_fun(*this, &PartyPoints::set_cell_color), c));
        }

      } // the points table

      { // point list
        auto table_box = Gtk::manage(new Gtk::VBox(false));
        auto alignment
          = Gtk::manage(new Gtk::Alignment(Gtk::ALIGN_CENTER,
                                           Gtk::ALIGN_CENTER,
                                           0, 1));
        table_vbox->pack_start(*alignment);
        alignment->add(*table_box);
        { // the scrolled window
          auto scrolled_window = Gtk::manage(new Gtk::ScrolledWindow);
          scrolled_window->set_policy(Gtk::POLICY_NEVER,
                                      Gtk::POLICY_AUTOMATIC);
          scrolled_window->add(*(this->party_points_treeview));
          scrolled_window->set_hadjustment(this->party_points_treeview->get_hadjustment());
          scrolled_window->set_vadjustment(this->party_points_treeview->get_vadjustment());

          table_box->pack_start(*scrolled_window, true, true);
        } // the scrolled window
        table_box->pack_end(*this->party_points_sum_treeview, false, true);
      } // point list

      { // duty soli
        // add lines for duty soli
        {
          this->duty_free_soli_row = *this->duty_soli_list->append();
          this->duty_free_soli_row[this->party_points_model.gametype]
            = "duty free soli";
        }
        {
          this->duty_color_soli_row = *this->duty_soli_list->append();
          this->duty_color_soli_row[this->party_points_model.gametype]
            = "duty color soli";
        }
        {
          this->duty_picture_soli_row = *this->duty_soli_list->append();
          this->duty_picture_soli_row[this->party_points_model.gametype]
            = "duty picture soli";
        }

      } // duty soli

      { // the buttons
        { // show game overview
          auto alignment
            = Gtk::manage(new Gtk::Alignment(Gtk::ALIGN_CENTER,
                                             Gtk::ALIGN_CENTER,
                                             0, 0));
          this->show_game_overview_button
            = Gtk::manage(new Gtk::CheckButton("show game overview"));
          this->ui->translations->add(*this->show_game_overview_button,
                                      ::translation("show game overview"));
          alignment->add(*(this->show_game_overview_button));
          table_vbox->pack_end(*alignment, Gtk::PACK_SHRINK);
          this->show_game_overview_button->signal_toggled().connect(sigc::mem_fun(*this, &PartyPoints::show_game_overview_toggled));
        } // show game overview
        { // add up points
          auto alignment
            = Gtk::manage(new Gtk::Alignment(Gtk::ALIGN_CENTER,
                                             Gtk::ALIGN_CENTER,
                                             0, 0));
          this->add_up_points
            = Gtk::manage(new Gtk::CheckButton("add up points"));
          this->ui->translations->add(*this->add_up_points,
                                      ::translation("add up points"));
          alignment->add(*this->add_up_points);
          table_vbox->pack_end(*alignment, Gtk::PACK_SHRINK);
          this->add_up_points->signal_toggled().connect(sigc::mem_fun(*this, &PartyPoints::recalc_all));
        } // add up points

        { // collapse rounds
          auto vbox = Gtk::manage(new Gtk::HBox(true));
          Gtk::RadioButton::Group group;
          table_vbox->pack_end(*vbox, Gtk::PACK_SHRINK);
          {
            auto alignment
              = Gtk::manage(new Gtk::Alignment(Gtk::ALIGN_CENTER,
                                               Gtk::ALIGN_CENTER,
                                               0, 0));
            vbox->add(*alignment);
            this->collapse_rounds
              = Gtk::manage(new Gtk::RadioButton(group, "collapse rounds"));
            this->ui->translations->add(*this->collapse_rounds,
                                        ::translation("collapse rounds"));
            alignment->add(*this->collapse_rounds);
            this->collapse_rounds->signal_toggled().connect(sigc::mem_fun(*this, &PartyPoints::collaps_setting_changed));
          }
          {
            auto alignment
              = Gtk::manage(new Gtk::Alignment(Gtk::ALIGN_CENTER,
                                               Gtk::ALIGN_CENTER,
                                               0, 0));
            vbox->add(*alignment);
            this->expand_rounds
              = Gtk::manage(new Gtk::RadioButton(group, "expand rounds"));
            this->ui->translations->add(*this->expand_rounds,
                                        ::translation("expand rounds"));
            alignment->add(*this->expand_rounds);
            this->expand_rounds->signal_toggled().connect(sigc::mem_fun(*this, &PartyPoints::collaps_setting_changed));
          }
        } // collapse rounds
      } // the buttons

      auto label = Gtk::manage(new Gtk::Label("table"));
      this->ui->translations->add(*label, ::translation("table"));

      this->notebook->append_page(*table_vbox, *label);
    } // table
    { // graph
      this->graph = Gtk::manage(new Graph(this));

      auto label = Gtk::manage(new Gtk::Label("graph"));
      this->ui->translations->add(*label, ::translation("graph"));

      this->notebook->append_page(*this->graph, *label);
    } // graph

    this->get_vbox()->pack_start(*this->notebook);
  } // notebook

  { // remaining rounds / points
    this->remaining_rounds_label
      = Gtk::manage(new Gtk::Label("remaining rounds"));
    this->ui->translations->add(*this->remaining_rounds_label,
                                ::translation("remaining rounds: %urounds%",
                                              0u));
    this->remaining_points_label
      = Gtk::manage(new Gtk::Label("remaining points"));
    this->ui->translations->add(*this->remaining_points_label,
                                ::translation("remaining points: %ipoints%",
                                              0));

    auto box = Gtk::manage(new Gtk::HBox(true));
    box->pack_start(*this->remaining_rounds_label, Gtk::PACK_SHRINK, 1 EM);
    box->pack_end(*this->remaining_points_label, Gtk::PACK_SHRINK, 1 EM);

    this->get_vbox()->pack_start(*box, false, true);
  } // remaining rounds / points

  { // action area
    this->close_button
      = Gtk::manage(new Gtk::StockButton(Gtk::Stock::CLOSE, "close"));
    this->ui->translations->add(*this->close_button,
                                ::translation("close"));
    this->add_action_widget(*this->close_button, Gtk::RESPONSE_CLOSE);

    this->close_button->grab_default();
    this->close_button->signal_clicked().connect(sigc::mem_fun(*this, &Gtk::Widget::hide));
  } // action area

  this->show_all_children();

  { // signals
    this->signal_hide().connect(sigc::bind<bool>(sigc::mem_fun(*this->show_game_overview_button,
                                                               &Gtk::ToggleButton::set_active),
                                                 false));

    this->party_points_treeview->get_selection()->signal_changed().connect(sigc::mem_fun(*this, &PartyPoints::row_selection_changed));

    this->signal_show().connect(sigc::mem_fun(*this,
                                              &PartyPoints::update_player_columns_size));
    this->signal_show().connect(sigc::mem_fun(*this, &PartyPoints::language_update));
    this->party_points_treeview->signal_row_collapsed().connect(sigc::mem_fun(*this, &PartyPoints::row_collapsed_or_expanded));
    this->party_points_treeview->signal_row_expanded().connect(sigc::mem_fun(*this, &PartyPoints::row_collapsed_or_expanded));
  } // signals

  this->update_player_columns_size();

  this->language_update();

  this->set_default_size(0, 4 * this->ui->cards->height());

  this->show_game_overview_button->set_sensitive(false);

  if (::game_status > GAMESTATUS::PARTY_NEW)
    this->recreate_all();

  return ;
} // void PartyPoints::init()

/**
 ** the party is opened
 **
 ** @param     -
 **
 ** @return    -
 **/
void
PartyPoints::party_open()
{
  if (!this->get_realized())
    return ;

  this->recreate_all();

  if (this->ui->party().rule()(Rule::NUMBER_OF_ROUNDS_LIMITED))
    this->remaining_rounds_label->show();
  else
    this->remaining_rounds_label->hide();
  if (this->ui->party().rule()(Rule::POINTS_LIMITED))
    this->remaining_points_label->show();
  else
    this->remaining_points_label->hide();

  this->party_points_treeview->get_column(OFFSET_BOCK_MULTIPLIER)->set_visible(party.rule()(Rule::BOCK));
  this->party_points_sum_treeview->get_column(OFFSET_BOCK_MULTIPLIER)->set_visible(party.rule()(Rule::BOCK));

  return ;
} // void PartyPoints::party_open()

/**
 ** a round of the party is started
 **
 ** @param     round   number of the round
 **
 ** @return    -
 **/
void
PartyPoints::party_start_round(unsigned const round)
{
  if (!this->get_realized())
    return ;

  this->update_sum();
  this->update_duty_soli();

  if (this->collapse_rounds->get_active())
    if (round > 0)
      this->party_points_treeview->collapse_row(this->party_points_list->get_path(this->round_rows[round - 1]));

  return ;
} // void PartyPoints::party_start_round(unsigned round)

/**
 ** the party is finished
 **
 ** @param     -
 **
 ** @return    -
 **/
void
PartyPoints::party_finish()
{
  if (!this->get_realized())
    return ;

  this->graph->draw_all();
  this->update_sum();
  this->update_duty_soli();

  if (!this->round_rows.empty())
    this->party_points_treeview->collapse_row(this->party_points_list->get_path(this->round_rows.back()));

  return ;
} // void PartyPoints::party_finish()

/**
 ** the party is closed
 **
 ** @param     -
 **
 ** @return    -
 **/
void
PartyPoints::party_close()
{
  if (!this->get_realized())
    return ;

  this->party_points_list->clear();
  this->round_rows.clear();
  this->game_rows.clear();
  this->graph->draw_all();
  this->game_overview->set_gameno(UINT_MAX);

  return ;
} // void PartyPoints::party_close()

/**
 ** the game has started:
 ** redraw the graph (because of solo games)
 **
 ** @param     -
 **
 ** @return    -
 **/
void
PartyPoints::game_start()
{
  if (!this->get_realized())
    return ;

  this->graph->draw_all();

  return ;
} // void PartyPoints::game_start()

/**
 ** the game has finished:
 ** add a new row with the current game
 **
 ** @param     -
 **
 ** @return    -
 **/
void
PartyPoints::game_finished()
{
  if (!this->get_realized())
    return ;

  this->add_game(this->ui->party().gameno());
  this->add_future_bock_multipliers();
  this->party_points_treeview->scroll_to_row(Gtk::TreePath(this->game_rows[this->ui->party().gameno()]));

  return ;
} // void PartyPoints::game_finished()

/**
 ** the name of 'player' has changed
 **
 ** @param     player   the player with the changed name
 **
 ** @return    -
 **/
void
PartyPoints::name_changed(Player const& player)
{
  if (!this->get_realized())
    return ;

  this->party_points_treeview->get_column(::party.no(player) + OFFSET_PLAYER)->set_title(" " + player.name() + " ");
  this->game_overview->name_changed(player);

  this->party_points_treeview->columns_autosize();

  return ;
} // void PartyPoints::name_changed(Player player)

/**
 ** recalculates the party points
 **
 ** @param     -
 **
 ** @return    -
 **/
void
PartyPoints::recalc_all()
{
  if (!this->get_realized())
    return ;

  if (::game_status <= GAMESTATUS::PARTY_NEW)
    return ;

  for (unsigned g = 0; g < this->game_rows.size(); g++)
    this->set_points(g);

  this->update_sum();
  this->update_duty_soli();

  this->graph->draw_all();

  return ;
} // void PartyPoints::recalc_all()

/**
 ** recreates the party points
 **
 ** @param     -
 **
 ** @return    -
 **
 ** @todo	 round limits
 ** @todo	 added bock rounds
 ** @todo	 recreate the columns
 **/
void
PartyPoints::recreate_all()
{
  if (!this->get_realized())
    return ;
  if (::game_status < GAMESTATUS::PARTY_INITIAL_LOADED)
    return ;

  this->party_points_list->clear();
  this->round_rows.clear();
  this->game_rows.clear();

  auto const& party = this->ui->party();
  unsigned r = 0; // round startgame
  for (unsigned g = 0; g < party.finished_games(); ++g) {
    if (g == party.round_startgame(r)) {
      this->party_start_round(r);
      r += 1;
    }
    this->add_game(g, false);
  } // for (g < party.finished_games())

  this->add_future_bock_multipliers();

  if (this->duty_free_soli_row)
    this->duty_soli_list->erase(this->duty_free_soli_row);
  if (this->duty_color_soli_row)
    this->duty_soli_list->erase(this->duty_color_soli_row);
  if (this->duty_picture_soli_row)
    this->duty_soli_list->erase(this->duty_picture_soli_row);
  if (::party.rule()(Rule::NUMBER_OF_DUTY_SOLI) > 0)
    this->duty_free_soli_row = *this->duty_soli_list->append();
  else
    this->duty_free_soli_row = Gtk::TreeRow();
  if (::party.rule()(Rule::NUMBER_OF_DUTY_COLOR_SOLI) > 0)
    this->duty_color_soli_row = *this->duty_soli_list->append();
  else
    this->duty_color_soli_row = Gtk::TreeRow();
  if (::party.rule()(Rule::NUMBER_OF_DUTY_PICTURE_SOLI) > 0)
    this->duty_picture_soli_row = *this->duty_soli_list->append();
  else
    this->duty_picture_soli_row = Gtk::TreeRow();

  this->update_sum();
  this->update_duty_soli();

  this->party_points_treeview->get_column(OFFSET_BOCK_MULTIPLIER)->set_visible(party.rule()(Rule::BOCK));
  this->party_points_sum_treeview->get_column(OFFSET_BOCK_MULTIPLIER)->set_visible(party.rule()(Rule::BOCK));


  this->language_update();

  this->graph->draw_all();

  return ;
} // void PartyPoints::recreate_all()

/**
 ** add a column for the player
 **
 ** @param     player   new player
 **
 ** @return    -
 **/
void
PartyPoints::player_added(Player const& player)
{
  auto const p = ::party.no(player);
  //this->party_points_model.playerpoints.resize(::party.playerno());
  this->party_points_treeview->insert_column(player.name(),
                                             this->party_points_model.playerpoints[p],
                                             p + OFFSET_PLAYER);
  this->party_points_treeview->get_column_cell_renderer(p + OFFSET_PLAYER)->set_property("xalign", 0.5);
  this->party_points_treeview->get_column(p + OFFSET_PLAYER)->set_alignment(Gtk::ALIGN_CENTER);
  this->party_points_sum_treeview->insert_column("0",
                                                 this->party_points_model.playerpoints[p],
                                                 p);
  this->party_points_sum_treeview->get_column(p + OFFSET_PLAYER)->set_resizable(false);
  this->duty_soli_treeview->get_column(p + OFFSET_PLAYER)->set_alignment(Gtk::ALIGN_CENTER);
  return ;
} // void PartyPoints::player_added(Player player)

/**
 ** add the row for the game 'gameno'
 **
 ** @param     gameno   number of the game to be added
 ** @param     update   update the sum, ...
 **
 ** @return    -
 **/
void
PartyPoints::add_game(unsigned const gameno, bool const update)
{
  if (!this->get_realized())
    return ;

  this->add_row(gameno);
  this->set_points(gameno);
  if (update) {
    this->update_sum();
    this->party_points_treeview->expand_to_path(this->party_points_list->get_path(this->game_rows[gameno]));
    this->update_duty_soli();
    this->graph->draw_all();
  }

  return ;
} // void PartyPoints::add_game(unsigned gameno, bool update = false)

/**
 ** update the values of 'gameno'
 **
 ** @param     gameno   game number
 **
 ** @return    -
 **/
void
PartyPoints::set_points(unsigned const gameno)
{
  if (!this->get_realized())
    return ;

  DEBUG_ASSERTION((gameno < this->game_rows.size()),
                  "PartyPoints::set_points(gameno)\n"
                  "  gameno = " << gameno << " >= "
                  << this->game_rows.size() << " = number of rows");


  auto const& party = this->ui->party();
  auto& row = this->game_rows[gameno];

  if (gameno >= party.game_summaries().size()) {
    row[this->party_points_model.bock_multiplier_str]
      = (  (party.bock_multiplier(gameno) != 1)
         ? std::to_string(party.bock_multiplier(gameno))
         : "");
    if (party.rule()(Rule::NUMBER_OF_ROUNDS_LIMITED)
        && (gameno == party.round_startgame(party.rule()(Rule::NUMBER_OF_ROUNDS))) )
      row[this->party_points_model.gametype]
        =::translation("duty soli round").translation();
    else
      row[this->party_points_model.gametype]
        = "";
    return ;
  }

  auto const& game_summary = party.game_summary(gameno);

  if (   party.starts_new_round(gameno)
      && row.children()) {
    if (!this->party_points_treeview->row_expanded(this->party_points_list->get_path(row))) {
      this->update_round(row[this->party_points_model.round]);
      return ;
    }
  }

  row[this->party_points_model.round] = party.round_of_game(gameno);
  if (party.round_of_game(gameno) % 2 == 0)
    row[this->party_points_model.round_color] = "#F8F8F8";
  else
    row[this->party_points_model.round_color] = "#F0F0F0";
  row[this->party_points_model.gameno] = gameno;
  row[this->party_points_model.gameno_str]
    = std::to_string(gameno);
  row[this->party_points_model.bock_multiplier_str]
    = (   (game_summary.bock_multiplier() != 1)
       ? std::to_string(game_summary.bock_multiplier())
       : "");
  if (this->add_up_points->get_active()) {
    row[this->party_points_model.gamepoints_str]
      = std::to_string(party.pointsum_till_game(gameno));
  } else {
    row[this->party_points_model.gamepoints_str]
      = std::to_string(game_summary.points());
  }
#ifdef POSTPONED
  if (game_summary.game_type() == GAMETYPE::NORMAL)
    ;
  //row[this->party_points_model.gametype] = nullptr;
  else {
    Glib::RefPtr<Gdk::Pixbuf> pixbuf
      = this->ui->icons->icon(game_points.game_type());
    int const height = 20;
    row[this->party_points_model.gametype]
      = pixbuf->scale_simple(pixbuf->get_width() * height / pixbuf->get_height(),
                             height, Gdk::INTERP_TILES);
  }
#else
  if (game_summary.game_type() == GAMETYPE::NORMAL) {
    row[this->party_points_model.gametype] = "";
  } else {
    row[this->party_points_model.gametype]
      = (::translation(game_summary.game_type()).translation()
         + (game_summary.is_duty_solo() ? " *" : ""));
  }
#endif
  for (unsigned p = 0; p < party.playerno(); p++) {
    // ToDo: p is here the player in the party, not in the game
    if (game_summary.player_get_points(party.player(p))) {
      if (this->add_up_points->get_active())
        row[this->party_points_model.playerpoints[p]]
          = std::to_string(party.pointsum_till_game(gameno, p));
      else
        row[this->party_points_model.playerpoints[p]]
          = std::to_string(party.game_summary(gameno).points(party.player(p)));
    } else {
      row[this->party_points_model.playerpoints[p]]
        = "";
    }
  } // for (p < party.playerno())

  return ;
} // void PartyPoints::set_points(unsigned const gameno)

/**
 ** add rows for future bock multipliers
 **
 ** @param     gameno   game number
 **
 ** @return    -
 **
 ** @todo      stop, when the round limit is reached
 **/
void
PartyPoints::add_future_bock_multipliers()
{
  if (!this->get_realized())
    return ;

  auto const& party = this->ui->party();
  if (party.bock_multipliers().empty()
      || (   (party.bock_multipliers().size() == 1)
          && (party.current_bock_multiplier() == 1) ) )
    return ;

  // first add the missing rows
  for (unsigned g = 0;
       g < party.bock_multipliers().size();
       ++g) {
    this->add_row(party.finished_games() + g);
  }

  // second set the multipliers
  for (unsigned g = 0;
       g < party.bock_multipliers().size();
       ++g) {
    auto const gameno = party.finished_games() + g;
    auto& row = this->game_rows[gameno];
    row[this->party_points_model.round]
      = party.round_of_game(gameno);
    if (   party.rule()(Rule::NUMBER_OF_ROUNDS_LIMITED)
        && (gameno == party.round_startgame(party.rule()(Rule::NUMBER_OF_ROUNDS))) )
      row[this->party_points_model.gametype]
        = ::translation("duty soli round").translation();
    row[this->party_points_model.gameno]
      = gameno;
    row[this->party_points_model.bock_multiplier_str]
      = (   (party.bock_multiplier(gameno) == 1)
         ? ""
         : std::to_string(party.bock_multiplier(gameno)));
  } // for (g)

  return ;
} // void PartyPoints::add_future_bock_multipliers()

/**
 ** add a row for the game 'gameno' if there is none till now
 **
 ** @param     gameno   number of the game to add a row for
 **
 ** @return    -
 **/
void
PartyPoints::add_row(unsigned const gameno)
{
  if (!this->get_realized())
    return ;

  if (this->game_rows.size() <= gameno) {
    if (this->ui->party().starts_new_round(gameno)) {
      this->round_rows.push_back(*this->party_points_list->append());
      this->game_rows.push_back(this->round_rows.back());
    } else {
      this->game_rows.push_back(*this->party_points_list->append(this->round_rows.back().children()));
      if (this->round_rows.back()[this->party_points_model.gameno] + 1
          >= gameno)
        this->party_points_treeview->expand_to_path(this->party_points_list->get_path(this->round_rows.back()));
    }
  } else if (   (this->ui->party().finished_games() > gameno)
             && this->ui->party().game_summary(gameno).startplayer_stays()
            ) {
    // when a solo game has been played, add a row
    if (this->party_points_list->iter_depth(this->game_rows[gameno]) == 0) {
      if (this->game_rows[gameno].children().empty()) {
        this->game_rows.insert(this->game_rows.begin() + gameno + 1,
                               *this->party_points_list->append(this->game_rows[gameno].children()));
      } else {
        this->game_rows.insert(this->game_rows.begin() + gameno + 1,
                               *this->party_points_list->insert(this->game_rows[gameno+1]));
      }
    } else {
      this->game_rows.insert(this->game_rows.begin() + gameno,
                             *this->party_points_list->insert(this->game_rows[gameno]));
    }
    // update the future rows
    for (unsigned g = gameno; g < this->game_rows.size(); ++g)
      this->game_rows[g][this->party_points_model.gameno] = g;
  }
  this->game_rows[gameno][this->party_points_model.round]
    = party.round_of_game(gameno);

#ifndef RELEASE
#ifdef DKNOF
  if ( (this->party_points_list->iter_depth(this->game_rows[gameno]) == 0)
      != this->ui->party().starts_new_round(gameno) ) {
    DEBUG_ASSERTION(false,
                    "PartyPoints::add_row(" << gameno << ")\n"
                    "  depth of row = " << this->party_points_list->iter_depth(this->game_rows[gameno]) << " but game " << gameno << " starts new round = " << this->ui->party().starts_new_round(gameno) << '\n'
                    << "  roundno(" << gameno << ") = " << this->ui->party().round_of_game(gameno) << '\n'
                    << "  startgame of round = " << this->ui->party().round_startgame(this->ui->party().round_of_game(gameno)));
  }
#endif
#endif

  return ;
} // void PartyPoints::add_row(unsigned gameno)

/**
 ** a row has been collapsed/expanded
 **
 ** @param     iterator
 ** @param     path
 **
 ** @return    -
 **/
void
PartyPoints::row_collapsed_or_expanded(Gtk::TreeModel::iterator const& iterator,
                                       Gtk::TreeModel::Path const& path)
{
  if (!this->get_realized())
    return ;

  // ToDo: verify that it is a round-row
  auto row = *this->party_points_treeview->get_model()->get_iter(path);

  if (this->party_points_treeview->row_expanded(path))
    this->set_points(row[this->party_points_model.gameno]);
  else
    this->update_round(row[this->party_points_model.round]);

  return ;
} // void PartyPoints::row_collapsed_or_expanded(Gtk::TreeModel::iterator iterator, Gtk::TreeModel::Path path)

/**
 ** update the values of 'round'
 **
 ** @param     round   round which is to be updated
 **
 ** @return    -
 **
 ** @todo      future games
 **/
void
PartyPoints::update_round(unsigned const round)
{
  if (!this->get_realized())
    return ;

  DEBUG_ASSERTION((this->round_rows.size() > round),
                  "PartyPoints::update_round(round):\n"
                  "  round too great: round = " << round
                  << " >= this->round_rows.size() = "
                  << this->round_rows.size());
  if (::game_status <= GAMESTATUS::PARTY_NEW)
    return ;

  auto const& party = this->ui->party();
  auto& row = this->round_rows[round];

  if (this->party_points_treeview->row_expanded(this->party_points_list->get_path(row))
      || row.children().empty())
    return ;

  row[this->party_points_model.gameno_str] = "";
  if (party.bock_multiplier(party.round_startgame(round)) > 1)
    row[this->party_points_model.bock_multiplier_str]
      = std::to_string(party.bock_multiplier(party.round_startgame(round)));
  else
    row[this->party_points_model.bock_multiplier_str] = "";
  row[this->party_points_model.gametype]
    = (  (party.duty_soli_round() == round)
       ? ::translation("duty soli round").translation()
       : "");
  if (round > party.round_of_game(party.finished_games() - 1)) {
    for (unsigned p = 0; p < party.playerno(); ++p)
      row[this->party_points_model.playerpoints[p]]
        = "";
    row[this->party_points_model.gamepoints_str]
      = "";
  } else { // if !(round > party.roundno())
    if (this->add_up_points->get_active()) {
      for (unsigned p = 0; p < party.playerno(); ++p)
        row[this->party_points_model.playerpoints[p]]
          = std::to_string(party.pointsum_till_round(round, p));
      row[this->party_points_model.gamepoints_str]
        = std::to_string(party.pointsum_till_round(round));
    } else {
      for (unsigned p = 0; p < party.playerno(); ++p)
        row[this->party_points_model.playerpoints[p]]
          = std::to_string(party.pointsum_in_round(round, p));
      row[this->party_points_model.gamepoints_str]
        = std::to_string(party.pointsum_in_round(round));
    }
  }// if !(round > party.roundno())

  return ;
} // void PartyPoints::update_round(unsigned round)

/**
 ** update the sum
 **
 ** @param     -
 **
 ** @return    -
 **/
void
PartyPoints::update_sum()
{
  if (!this->get_realized())
    return ;

  if (::game_status <= GAMESTATUS::PARTY_NEW) {
    for (unsigned p = 0; p < party.playerno(); ++p)
      this->party_points_sum_treeview->get_column(p + OFFSET_PLAYER)->set_title("");
    this->party_points_sum_treeview->get_column(OFFSET_GAMEPOINTS)->set_title("");
    this->party_points_sum_treeview->get_column(OFFSET_BOCK_MULTIPLIER)->set_title("");
    this->party_points_sum_treeview->get_column(0)->set_title("");
    return ;
  } // if (::game_status <= GAMESTATUS::PARTY_NEW)

  auto const& party = this->ui->party();

  // udpate the sum
  for (unsigned p = 0; p < party.playerno(); ++p)
    this->party_points_sum_treeview->get_column(p + OFFSET_PLAYER)->set_title(std::to_string(party.pointsum(p)));
  this->party_points_sum_treeview->get_column(OFFSET_GAMEPOINTS)->set_title(std::to_string(party.pointsum()));
  // ToDo: write the number of bock triggers
  this->party_points_sum_treeview->get_column(0)->set_title(" " + std::to_string(party.gameno() + 1) + " ");

  this->sum_columns_size_update();


  if (party.rule()(Rule::NUMBER_OF_ROUNDS_LIMITED))
    this->ui->translations->change(*this->remaining_rounds_label,
                                   ::translation("remaining rounds: %urounds%",
                                                 party.remaining_rounds())
                                  );
  if (party.rule()(Rule::POINTS_LIMITED))
    this->ui->translations->change(*this->remaining_points_label,
                                   ::translation("remaining points: %ipoints%",
                                                 ( (party.remaining_points() > 0)
                                                  ? party.remaining_points()
                                                  : 0) ) );
  if (   party.rule()(Rule::NUMBER_OF_ROUNDS_LIMITED)
      || party.rule()(Rule::POINTS_LIMITED))
    this->remaining_rounds_label->get_parent()->set_border_width(1 EX);
  else
    this->remaining_rounds_label->get_parent()->set_border_width(0);

  return ;
} // void PartyPoints::update_sum()

/**
 ** update the duty soli
 **
 ** @param     -
 **
 ** @return    -
 **/
void
PartyPoints::update_duty_soli()
{
  if (!this->get_realized())
    return ;

  auto const& party = this->ui->party();

  if (this->duty_free_soli_row)
    this->duty_free_soli_row[this->party_points_model.gametype]
      = ::translation("duty free soli").translation();
  if (this->duty_color_soli_row)
    this->duty_color_soli_row[this->party_points_model.gametype]
      = ::translation("duty color soli").translation();
  if (this->duty_picture_soli_row)
    this->duty_picture_soli_row[this->party_points_model.gametype]
      = ::translation("duty picture soli").translation();

  for (unsigned p = 0; p < party.playerno(); ++p) {
    if (this->duty_free_soli_row)
      this->duty_free_soli_row[this->party_points_model.playerpoints[p]]
        = std::to_string(party.remaining_duty_free_soli(p));
    if (this->duty_color_soli_row)
      this->duty_color_soli_row[this->party_points_model.playerpoints[p]]
        = std::to_string(party.remaining_duty_color_soli(p));
    if (this->duty_picture_soli_row)
      this->duty_picture_soli_row[this->party_points_model.playerpoints[p]]
        = std::to_string(party.remaining_duty_picture_soli(p));
  } // for (p < playerno())

  // ToDo: show/hide the rows according to the rule

  return ;
} // void PartyPoints::update_duty_soli()

/**
 ** update the column size of the players
 **
 ** @param     -
 **
 ** @return    -
 **
 ** @todo	understand why this works
 **/
void
PartyPoints::update_player_columns_size()
{
  if (!this->is_visible())
    return ;

  auto const& party = this->ui->party();

  for (unsigned c = OFFSET_PLAYER;
       c < OFFSET_PLAYER + party.playerno();
       ++c) {
    this->party_points_treeview->get_column(c)->set_min_width(-1);
  } // for (c)
  {
    int width = -1;
    for (unsigned c = OFFSET_PLAYER;
         c < OFFSET_PLAYER + party.playerno();
         ++c) {
      width = max(width, 
                  this->party_points_treeview->get_column(c)->get_width());
    } // for (c)
    for (unsigned c = OFFSET_PLAYER;
         c < OFFSET_PLAYER + party.playerno();
         ++c) {
      if (this->party_points_treeview->get_column(c)->get_min_width()
          < width)
        this->party_points_treeview->get_column(c)->set_min_width(width);
    } // for (c)
  }

  this->sum_columns_size_update();

  return ;
} // void PartyPoints::update_player_columns_size()

/**
 ** the row selection has changed:
 ** show the belonging game summary
 **
 ** @param     -
 **
 ** @return    -
 **/
void
PartyPoints::row_selection_changed()
{
  auto selection = this->party_points_treeview->get_selection();

  this->show_game_overview_button->set_sensitive(selection->get_selected_rows().size() == 1);

  if (selection->get_selected()) {
    auto row = *selection->get_selected();

    auto const gameno = row[this->party_points_model.gameno];
#ifdef DKNOF
    cout << "selected game: " << gameno
      << " (" << this->game_rows[gameno][this->party_points_model.gameno] << ")"
      << " - round: " << this->ui->party().round_of_game(gameno)
      << " (startgame: " << this->ui->party().round_startgame(this->ui->party().round_of_game(gameno)) << ")"
      << endl;
#endif

    // ToDo: check, if the current game is finished
    if (gameno < ::party.finished_games()) {
      this->game_overview->set_gameno(gameno);
      this->show_game_overview_button->set_sensitive(true);
    } else {
      this->game_overview->set_gameno(UINT_MAX);
      this->game_overview->hide();
      this->show_game_overview_button->set_sensitive(false);
    }
  } else {	// if !(selection->get_selected())
    this->game_overview->set_gameno(UINT_MAX);
    this->game_overview->hide();
    this->show_game_overview_button->set_sensitive(false);
  }	// if !(selection->get_selected())

  return ;
} // void PartyPoints::row_selection_changed()

/**
 ** update the column size of the sum treeview
 **
 ** @param     -
 **
 ** @return    -
 **/
void
PartyPoints::sum_columns_size_update()
{
  if (!this->get_realized())
    return ;

  for (unsigned c = 0; c < this->party_points_treeview->get_columns().size(); ++c)
    if (this->party_points_treeview->get_column(c)->get_visible()) {
      auto const width
        = this->party_points_treeview->get_column(c)->get_width();
#ifdef WORKAROUND
      this->party_points_sum_treeview->get_column(c)->set_min_width(width);
      this->party_points_sum_treeview->get_column(c)->set_max_width(width);
#else
      // does not work
      this->party_points_sum_treeview->get_column(c)->set_fixed_width(width);
      this->party_points_sum_treeview->get_column(c)->set_sizing(Gtk::TREE_VIEW_COLUMN_FIXED);
#endif
    }

  return ;
} // void PartyPoints::sum_columns_size_update()

/**
 ** clear the selection
 **
 ** @param     -
 **
 ** @return    -
 **/
void
PartyPoints::clear_selection()
{
  if (!this->get_realized())
    return ;

  this->party_points_treeview->get_selection()->unselect_all();

  return ;
} // void PartyPoints::clear_selection()

/**
 ** toggle the showing of the 'game overview' window
 **
 ** @param     -
 **
 ** @return    -
 **/
void
PartyPoints::show_game_overview_toggled()
{
  if (!this->get_realized())
    return ;

  if (this->game_overview) {
    if (this->show_game_overview_button->get_active()) {
      this->row_selection_changed();

      if (this->game_overview->gameno()
          < this->ui->party().finished_games())
        this->game_overview->present();
      else
        this->show_game_overview_button->set_active(false);
#ifdef WORKAROUND
      // 'game_overview' cannot get the gameno in the initialisation
      if (this->game_overview->gameno() == UINT_MAX)
        this->row_selection_changed();
#endif
    } else {
      this->game_overview->hide();
    }
  } // if (this->game_overview)

  return ;
} // void PartyPoints::show_game_overview_toggled()

/**
 ** toggle the collappsing of the rounds
 **
 ** @param     -
 **
 ** @return    -
 **/
void
PartyPoints::collaps_setting_changed()
{
  if (!this->get_realized())
    return ;

  if (this->collapse_rounds->get_active())
    this->party_points_treeview->collapse_all();
  else
    this->party_points_treeview->expand_all();

  return ;
} // void PartyPoints::collaps_setting_changed()

/**
 ** updates all language elements
 **
 ** @param     -
 **
 ** @return    -
 **/
void
PartyPoints::language_update()
{
  if (!this->get_realized())
    return ;

  if (::game_status <= GAMESTATUS::PARTY_NEW)
    return ;

  this->party_points_treeview->get_column(OFFSET_GAMETYPE)->set_title(::translation("gametype").translation());
  this->party_points_treeview->get_column(OFFSET_BOCK_MULTIPLIER)->set_title(::translation("bock").translation());
  this->party_points_treeview->get_column(OFFSET_GAMEPOINTS)->set_title(::translation("game points").translation());

  for (auto row : this->game_rows) {
    auto const gameno = row[this->party_points_model.gameno];
    // ToDo: check, whether the current game is finished
    if (gameno >= this->ui->party().finished_games()) {
      if (   party.rule()(Rule::NUMBER_OF_ROUNDS_LIMITED)
          && (gameno == party.round_startgame(party.rule()(Rule::NUMBER_OF_ROUNDS))) )
        row[this->party_points_model.gametype]
          =::translation("duty soli round").translation();
      else
        row[this->party_points_model.gametype] = "";
    } else { // if !(gameno >= this->ui->party().finished_games())
      auto const& game_summary
        = this->ui->party().game_summary(gameno);
      if (game_summary.game_type() == GAMETYPE::NORMAL)
        row[this->party_points_model.gametype] = "";
      else
        row[this->party_points_model.gametype]
          = (::translation(game_summary.game_type()).translation()
             + (game_summary.is_duty_solo() ? " *" : ""));
    } // if !(gameno >= this->ui->party().finished_games())
  } // for (row : this->game_rows)
  for (auto row : this->round_rows)
    this->update_round(row[this->party_points_model.round]);

  { // duty soli
    if (this->duty_free_soli_row)
      this->duty_free_soli_row[this->party_points_model.gametype]
        = ::translation("duty free soli").translation();
    if (this->duty_color_soli_row)
      this->duty_color_soli_row[this->party_points_model.gametype]
        = ::translation("duty color soli").translation();
    if (this->duty_picture_soli_row)
      this->duty_picture_soli_row[this->party_points_model.gametype]
        = ::translation("duty picture soli").translation();
  } // duty soli

  return ;
} // void PartyPoints::language_update()

/**
 ** changes the color of the cell at 'iterator'
 **
 ** @param     cell_renderer   cell renderer to change
 ** @param     iterator   row
 ** @param     column   	the column
 **
 ** @return    -
 **/
void
PartyPoints::set_cell_color(Gtk::CellRenderer* cell_renderer,
                            Gtk::TreeModel::iterator const& iterator,
                            unsigned const column)
{
  if (!this->get_realized())
    return ;

  auto const row = *iterator;

  Gdk::Color color; 

  auto const gameno = row[this->party_points_model.gameno];
  auto const& party = this->ui->party();
  if (gameno >= party.finished_games())
    return ;

  auto const& game_summary = party.game_summary(gameno);

#ifdef POSTPONED
  if (this->party_points_treeview->row_expanded(*iterator))
#endif
    if (   (game_summary.soloplayer_no() != UINT_MAX)
        && (   (column == OFFSET_GAMETYPE)
            || (column == OFFSET_PLAYER + game_summary.party_player(game_summary.soloplayer_no())) )
       )
      color = this->color(game_summary);

  cell_renderer->set_property("foreground_gdk", color);

#ifdef POSTPONED
  // the whole column is made italic

  // make the name of the startplayer bold
  if (   (column >= OFFSET_PLAYER)
      && (column < OFFSET_PLAYER + party.playerno())
      && (game_summary.soloplayer_no() != UINT_MAX)
      && (column - OFFSET_PLAYER == game_summary.party_player(game_summary.soloplayer_no()))) {
    cell_renderer->set_property("style", Pango::STYLE_ITALIC);
  } // if (column of the soloplayer)
#endif

  return ;
} // void PartyPoints::set_cell_color(Gtk::CellRenderer* cell_renderer, Gtk::TreeModel::iterator iterator, unsigned column)

/**
 ** -> result
 **
 ** @param     game_summary   game summary
 **
 ** @return    (background) color for the game summary
 **/
Gdk::Color
PartyPoints::color(::GameSummary const& game_summary)
{
  Gdk::Color color;

  switch (game_summary.game_type()) {
  case GAMETYPE::NORMAL:
    color.set("black");
    break;
  case GAMETYPE::THROWN_NINES:
  case GAMETYPE::THROWN_KINGS:
  case GAMETYPE::THROWN_NINES_AND_KINGS:
  case GAMETYPE::THROWN_RICHNESS:
  case GAMETYPE::FOX_HIGHEST_TRUMP:
    color.set("#ffff00");
    break;
  case GAMETYPE::POVERTY:
    color.set("#B8860B");
    break;
  case GAMETYPE::GENSCHER:
    color.set("#7FFFD4");
    break;
  case GAMETYPE::MARRIAGE:
    color.set("#1E90FF");
    break;
  case GAMETYPE::MARRIAGE_SOLO:
  case GAMETYPE::MARRIAGE_SILENT:
    color.set("#00BFFF");
    break;
  case GAMETYPE::SOLO_JACK:
  case GAMETYPE::SOLO_QUEEN:
  case GAMETYPE::SOLO_KING:
  case GAMETYPE::SOLO_QUEEN_JACK:
  case GAMETYPE::SOLO_KING_JACK:
  case GAMETYPE::SOLO_KING_QUEEN:
  case GAMETYPE::SOLO_KOEHLER:
  case GAMETYPE::SOLO_CLUB:
  case GAMETYPE::SOLO_SPADE:
  case GAMETYPE::SOLO_HEART:
  case GAMETYPE::SOLO_DIAMOND:
  case GAMETYPE::SOLO_MEATLESS:
    if (game_summary.is_duty_solo())
      color.set("#FF0000");
    else
      color.set("#ffaf00");

    break;
  } // switch (game_type)

  return color;
} // static Gdk::Color PartyPoints::color(::GameSummary game_summary)

/**
 ** a key has been pressed
 ** C-o: output of the points on 'stdout'
 **
 ** @param     key   the key
 **
 ** @return    whether the key was managed
 **
 ** @todo     print the party points
 **/
bool
PartyPoints::on_key_press_event(GdkEventKey* const key)
{
  bool managed = false;

  if ((key->state & ~GDK_SHIFT_MASK) == GDK_CONTROL_MASK) {
    switch (key->keyval) {
    case GDK_KEY_o: // ouput of the party points
      // ToDo
      managed = true;
      break;
    } // switch (key->keyval)
  } // if (key->state == GDK_CONTROL_MASK)

  return (managed
          || this->StickyDialog::on_key_press_event(key)
          || this->ui->key_press(key));
} // bool PartyPoints::on_key_press_event(GdkEventKey* key)

} // namespace UI_GTKMM_NS

#endif // #ifdef USE_UI_GTKMM
