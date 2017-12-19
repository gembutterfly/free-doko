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

#include "game_summary.h"

#include "ui.h"
#include "translations.h"
#include "bug_report.h"

#include "../../party/party.h"
#include "../../party/rule.h"
#include "../../game/game_summary.h"
#include "../../utils/string.h"

#include <gtkmm/button.h>
#include <gtkmm/alignment.h>
#include <gtkmm/separator.h>
#include <gtkmm/label.h>
#include <gtkmm/main.h>
#include <gtkmm/treeview.h>
#include <gtkmm/scrolledwindow.h>

namespace UI_GTKMM_NS {

/**
 ** Constructor
 **
 ** @param     parent   parent object
 **
 ** @return    -
 **/
GameSummary::GameSummary(Base* const parent) :
  Base(parent),
  HBox()
{
  this->signal_realize().connect(sigc::mem_fun(*this, &GameSummary::init));
  return ;
} // GameSummary::GameSummary(Base* parent)

/**
 ** create all subelements
 **
 ** @param     -
 **
 ** @return    -
 **/
void
GameSummary::init()
{
  this->set_border_width(1 EM);
  this->set_spacing(1 EM);

  { // left column
    auto vbox = Gtk::manage(new Gtk::VBox());

    vbox->set_border_width(1 EX);
    vbox->set_spacing(1 EX);

    this->gamenumber = Gtk::manage(new Gtk::Label("Game:"));
    this->ui->translations->add(*this->gamenumber,
                                ::translation("~game %ugameno%",
                                              0u));
    vbox->pack_start(*this->gamenumber, false, true);

    this->seed = Gtk::manage(new Gtk::Label("Seed:"));
    this->ui->translations->add(*this->seed,
                                ::translation("~seed %useed%",
                                              0u));
    vbox->pack_start(*this->seed, false, true);

    this->startplayer = Gtk::manage(new Gtk::Label("Startplayer:"));
    this->ui->translations->add(*this->startplayer,
                                ::translation("~startplayer %sname%",
                                              ""));
    vbox->pack_start(*this->startplayer, false, true);

    this->winner = Gtk::manage(new Gtk::Label("Winner:"));
    this->ui->translations->add(*this->winner,
                                ::translation("Winner: no team"));
    vbox->pack_start(*this->winner, false, true);


    { // trickpoints
      this->trickpoints_list
        = Gtk::ListStore::create(this->trickpoints_model);
      this->trickpoints_treeview
        = Gtk::manage(new Gtk::TreeView(this->trickpoints_list));
      this->trickpoints_treeview->get_selection()->set_mode(Gtk::SELECTION_NONE);

      this->trickpoints_treeview->append_column("name",
                                                this->trickpoints_model.name);
      this->trickpoints_treeview->append_column("trick points",
                                                this->trickpoints_model.points);
      this->trickpoints_treeview->get_column_cell_renderer(1)->set_property("xalign", 0.5);
      this->trickpoints_treeview->append_column("team",
                                                this->trickpoints_model.team);
      this->trickpoints_treeview->append_column("game points",
                                                this->trickpoints_model.game_points);
      this->trickpoints_treeview->get_column_cell_renderer(3)->set_property("xalign", 0.5);
      this->trickpoints_treeview->append_column("party points",
                                                this->trickpoints_model.party_points);
      this->trickpoints_treeview->get_column_cell_renderer(4)->set_property("xalign", 0.5);

      auto alignment
        = Gtk::manage(new Gtk::Alignment(Gtk::ALIGN_CENTER,
                                         Gtk::ALIGN_CENTER,
                                         0, 0));
      alignment->add(*this->trickpoints_treeview);
      vbox->pack_start(*alignment, false, true);

      for (unsigned c = 0;
           c < this->trickpoints_treeview->get_columns().size();
           ++c) {
        auto column = this->trickpoints_treeview->get_column(c);
        column->set_cell_data_func(*column->get_first_cell(), sigc::mem_fun(*this, &GameSummary::set_points_cell_color));
      }
    } // trickpoints
    { // bock triggers
      this->bock_triggers_list
        = Gtk::ListStore::create(this->bock_triggers_model);
      this->bock_triggers_treeview
        = Gtk::manage(new Gtk::TreeView(this->bock_triggers_list));
      this->bock_triggers_treeview->get_selection()->set_mode(Gtk::SELECTION_NONE);

      this->bock_triggers_treeview->append_column("bock trigger",
                                                  this->bock_triggers_model.name);

      auto alignment
        = Gtk::manage(new Gtk::Alignment(Gtk::ALIGN_CENTER,
                                         Gtk::ALIGN_CENTER,
                                         0, 1));
      alignment->set_border_width(1 EX);
      alignment->add(*this->bock_triggers_treeview);
      vbox->pack_start(*alignment, false, true);
    } // bock triggers

    this->pack_start(*vbox, Gtk::PACK_SHRINK);
  } // left column

  { // right column
    auto vbox = Gtk::manage(new Gtk::VBox());

    vbox->set_border_width(1 EX);
    vbox->set_spacing(1 EX);

    { // game type
      this->type = Gtk::manage(new Gtk::Label("gametype:"));
      this->ui->translations->add(*this->type,
                                  ::translation("gametype: %ttype%",
                                                ::translation(GAMETYPE::NORMAL)));


      vbox->pack_start(*this->type, false, true);
    } // game type


    { // points description
      this->points_description_list
        = Gtk::ListStore::create(this->points_description_model);
      this->points_description_treeview
        = Gtk::manage(new Gtk::TreeView(this->points_description_list));

      // ToDo: selecting a special point shows the corresponding trick
      // in the game review
      this->points_description_treeview->get_selection()->set_mode(Gtk::SELECTION_NONE);
      this->points_description_treeview->append_column("team",
                                                       this->points_description_model.team);
      // ToDo: right-align the sign
      this->points_description_treeview->append_column("player",
                                                       this->points_description_model.player);
      this->points_description_treeview->append_column("points",
                                                       this->points_description_model.points);
      this->points_description_treeview->get_column_cell_renderer(2)->set_property("xalign", 0.5);
      this->points_description_treeview->append_column("description",
                                                       this->points_description_model.description);

#ifdef POSTPONED
      // ToDo: make the scrolled window show the first four lines of the table
      auto scrolled_window = Gtk::manage(new Gtk::ScrolledWindow);
      scrolled_window->set_policy(Gtk::POLICY_NEVER,
                                  Gtk::POLICY_AUTOMATIC);
      scrolled_window->add(*(this->points_description_treeview));
      vbox->pack_start(*scrolled_window, true, true);
#else
      auto alignment = Gtk::manage(new Gtk::Alignment(Gtk::ALIGN_CENTER,
                                                      Gtk::ALIGN_CENTER,
                                                      0, 1));
      alignment->add(*(this->points_description_treeview));
      vbox->pack_start(*alignment, true, true);
      //this->pack_start(*(this->points_description_treeview), true, true);
#endif
    } // point description
    //this->pack_start(*(this->points_description));

    this->pack_start(*vbox);
  } // right column

  this->language_update();
  this->show_all_children();
  if (this->gameno != UINT_MAX)
    this->show();

  this->update();
  this->language_update();

  return ;
} // void GameSummary::init()

/**
 ** set the gameno
 **
 ** @param     gameno   the new gameno
 **
 ** @return    -
 **/
void
GameSummary::set_gameno(unsigned const gameno)
{
  this->gameno = gameno;
  if (this->gameno == UINT_MAX)
    this->game_summary = nullptr;
  else
    this->game_summary = &this->ui->party().game_summary(this->gameno);
  this->update();

  return ;
} // void GameSummary::set_gameno(unsigned gameno)

/**
 ** set the game summary
 **
 ** @param     game_summary   new game summary
 **
 ** @return    -
 **/
void
GameSummary::set_game_summary(::GameSummary const& game_summary)
{
  this->gameno = UINT_MAX;
  this->game_summary = &game_summary;
  this->update();

  return ;
} // void GameSummary::set_game_summary(::GameSummary game_summary)

/**
 ** update (rewrite) all information
 **
 ** @param     -
 **
 ** @return    -
 **
 ** @todo	mark the winner gray
 ** @todo	if there is no 'player_of' don't show the column
 **/
void
GameSummary::update()
{
  if (!this->get_realized())
    return ;

  if (::game_status == GAMESTATUS::PROGRAMSTART)
    return ;
  if (!this->game_summary) {
    this->hide();
    return ;
  }

#ifdef WORKAROUND
  // why?
  this->show();
#endif

#ifdef TODO
#ifdef WORKAROUND
  // the party has not been opened, yet
  if (&party == nullptr)
    return ;
#endif
#endif
  Party const& party = this->ui->party();

  this->ui->translations->change(*this->gamenumber,
                                 ::translation("~game %ugameno%",
                                               this->gameno + 1));
  if (this->gameno == UINT_MAX)
    this->gamenumber->hide();
  else
    this->gamenumber->show();
  this->ui->translations->change(*this->seed,
                                 ::translation("~seed %useed%",
                                               this->game_summary->seed()));
  this->ui->translations->change(*this->startplayer,
                                 ::translation("~startplayer %sname%",
                                               this->ui->party().player(this->game_summary->party_player(this->game_summary->startplayer_no())).name()));

  this->ui->translations->change(*this->type,
                                 ::translation(( this->game_summary->is_duty_solo()
                                                ? "gametype: %ttype% (duty solo)"
                                                : "gametype: %ttype%"),
                                               ::translation(this->game_summary->game_type())));

  if (this->game_summary->winnerteam() == TEAM::NOTEAM)
    this->ui->translations->change(*this->winner,
                                   ::translation("Winner: no team"));
  else
    this->ui->translations->change(*this->winner,
                                   ::translation("Winner: team %twinnerteam% with %uwinnerpoints% points (%tlooserteam%: %ulooserpoints% points)",

                                                 ::translation(this->game_summary->winnerteam()),
                                                 this->game_summary->trick_points(this->game_summary->winnerteam()),
                                                 ::translation(::opposite(this->game_summary->winnerteam())),
                                                 this->game_summary->trick_points(::opposite(this->game_summary->winnerteam()))
                                                )
                                  );

  { // trickpoints
    this->trickpoints_list->clear();

    for (unsigned p = 0;
         p < this->game_summary->rule()(Rule::NUMBER_OF_PLAYERS_IN_GAME);
         p++) {
      auto row = *(this->trickpoints_list->append());
      row[this->trickpoints_model.name] = party.player(this->game_summary->party_player(p)).name();
      if (this->game_summary->winnerteam() == TEAM::NOTEAM)
        row[this->trickpoints_model.winner] = 0; // no winner
      else if (this->game_summary->team(p) == this->game_summary->winnerteam())
        row[this->trickpoints_model.winner] = 1; // winner
      else
        row[this->trickpoints_model.winner] = -1; // looser

      row[this->trickpoints_model.points] = this->game_summary->trick_points(p);
      row[this->trickpoints_model.team]
        = ::translation(this->game_summary->team(p)).translation();
      if (this->game_summary->player_get_points(p))
        if (this->game_summary->points(p) > 0)
          row[this->trickpoints_model.game_points]
            = "+" + DK::Utils::String::to_string(this->game_summary->points(p));
        else
          row[this->trickpoints_model.game_points]
            = DK::Utils::String::to_string(this->game_summary->points(p));
      else
        row[this->trickpoints_model.game_points]
          = "";

      row[this->trickpoints_model.party_points]
        = (this->gameno == UINT_MAX
           ? 0
           : party.pointsum_till_game(this->gameno, this->game_summary->party_player(p)));
    } // for (p < this->game_summary->rule(Rule::NUMBER_OF_PLAYERS_IN_GAME)
  } // trickpoints

  { // points description
    if (this->game_summary->specialpoints().empty()) {
      this->points_description_treeview->hide();
    } else { // if !(this->game_summary->specialpoints().empty())
      this->points_description_treeview->show();

      this->points_description_list->clear();

      for (unsigned i = 0;
           i < this->game_summary->specialpoints().size();
           i++) {
        auto const& specialpoint
          = this->game_summary->specialpoints()[i];
        auto row = *this->points_description_list->append();
        row[this->points_description_model.team]
          = (  (specialpoint.team == TEAM::NOTEAM)
             ? ""
             : ::translation(specialpoint.team).translation() );

        if (specialpoint.player_get_no == UINT_MAX) {
          if (specialpoint.player_of_no == UINT_MAX)
            row[this->points_description_model.player] = string();
          else
            row[this->points_description_model.player]
              = party.player(this->game_summary->party_player(specialpoint.player_of_no)).name();
        } else {
          row[this->points_description_model.player]
            = party.player(this->game_summary->party_player(specialpoint.player_get_no)).name();
        }

        if (   !party.rule()(Rule::KNOCKING)
            && (specialpoint.type == SPECIALPOINT::NO120_SAID) ) {
          if (specialpoint.team == TEAM::RE)
            row[this->points_description_model.description]
              = ::translation("SpecialPoint::re said").translation();
          else if (specialpoint.team == TEAM::CONTRA)
            row[this->points_description_model.description]
              = ::translation("SpecialPoint::contra said").translation();
        } else {
          row[this->points_description_model.description]
            = ::translation(specialpoint).translation();
        }


        { // show whether the points are added or subtracted
          bool add = true;
          if ((this->game_summary->winnerteam() == TEAM::RE)
              || (this->game_summary->winnerteam() == TEAM::CONTRA)) {
            add = (this->game_summary->specialpoints()[i].counting_team
                   == this->game_summary->winnerteam());
          } else { // if !(this->game_summary->winnerteam() == RE, CONTRA)
            if (specialpoint.counting_team == TEAM::RE)
              add = true;
            else if (specialpoint.counting_team == TEAM::CONTRA)
              add = false;
          } // if !(this->game_summary->winnerteam() != RE, CONTRA)
          if (add)
            row[this->points_description_model.points] = "+";
          else
            row[this->points_description_model.points] = "-";
        } // show whether the points are added or subtracted

        if (    (this->game_summary->winnerteam()
                 == TEAM::NOTEAM)
            && (specialpoint.counting_team
                == TEAM::NOTEAM))
          row[this->points_description_model.points] = " ";
        if (   (specialpoint.type
                == SPECIALPOINT::NO120_SAID)
            && (this->game_summary->rule()(Rule::ANNOUNCEMENT_RE_DOUBLES))
           ) {
          row[this->points_description_model.points]
            = "* " + DK::Utils::String::to_string(specialpoint.value());
        } else if (specialpoint.type
                   == SPECIALPOINT::SOLO) {
          // a solo
          row[this->points_description_model.points]
            = "* " + DK::Utils::String::to_string(specialpoint.value());
        } else if (specialpoint.type
                   == SPECIALPOINT::BOCK) {
          // a bock multiplicator
          row[this->points_description_model.points]
            = "* " + DK::Utils::String::to_string(specialpoint.value());
        } else {
          // all other special points
          row[this->points_description_model.points]
            = (row[this->points_description_model.points]
               + " " + DK::Utils::String::to_string(Value_of_Specialpoint(specialpoint.type))
              );

        }
      } // for (i < this->game_summary->specialpoints().size())
    } // if !(this->game_summary->specialpoints().empty())

  } // points description

  { // bock triggers
    if (this->game_summary->bock_triggers().empty()) {
      this->bock_triggers_treeview->hide();
    } else { // if !(this->game_summary->bock_triggers().empty())
      this->bock_triggers_treeview->show();

      this->bock_triggers_list->clear();

      for (unsigned i = 0;
           i < this->game_summary->bock_triggers().size();
           i++) {
        auto row = *(this->bock_triggers_list->append());
        row[this->bock_triggers_model.name]
          = ::translation(this->game_summary->bock_triggers()[i]).translation();
      } // for (i < this->game_summary->specialpoints().size())
    } // if !(this->game_summary->specialpoints().empty())

  } // bock triggers

  return ;
} // void GameSummary::update()

/**
 ** changes the color of the cell at 'iterator'
 ** mark the winner green and the loosers red
 **
 ** @param     cell_renderer   cell renderer to change
 ** @param     iterator   row
 **
 ** @return    -
 **/
void
GameSummary::set_points_cell_color(Gtk::CellRenderer* const cell_renderer,
                                   Gtk::TreeModel::iterator const& iterator)
{
  if (!this->get_realized())
    return ;

  auto const row = *iterator;

  int const winner = row[this->trickpoints_model.winner];
  if (winner > 0) // winner
    cell_renderer->property_cell_background() = "green4";
  else if (winner < 0) // looser
    cell_renderer->property_cell_background() = "red";
  else // no winner
    cell_renderer->property_cell_background() = "white";

  return ;
} // void GameSummary::set_points_cell_color(Gtk::CellRenderer* cell_renderer, Gtk::TreeModel::iterator iterator)


/**
 ** the name of 'player' has changed
 **
 ** @param     player   the player with the changed name
 **
 ** @return    -
 **/
void
GameSummary::name_changed(Player const& player)
{
  if (!this->get_realized())
    return ;

  this->update();

  return ;
} // void GameSummary::name_changed(Player const& player)

/**
 ** updates all language elements
 **
 ** @param     -
 **
 ** @return    -
 **/
void
GameSummary::language_update()
{
  if (!this->get_realized())
    return ;

  this->trickpoints_treeview->get_column(0)->set_title(::translation("name").translation());
  this->trickpoints_treeview->get_column(1)->set_title(::translation("trick points").translation());
  this->trickpoints_treeview->get_column(2)->set_title(::translation("team").translation());
  this->trickpoints_treeview->get_column(3)->set_title(::translation("game points").translation());
  this->trickpoints_treeview->get_column(4)->set_title(::translation("party points").translation());

  this->points_description_treeview->get_column(0)->set_title(::translation("team").translation());
  this->points_description_treeview->get_column(1)->set_title(::translation("player").translation());
#if 0
  this->points_description_treeview->get_column(2)->set_title(::translation("Sign").translation());
#endif
  this->points_description_treeview->get_column(2)->set_title(::translation("point(s)").translation());
  this->points_description_treeview->get_column(3)->set_title(::translation("description").translation());

  this->bock_triggers_treeview->get_column(0)->set_title(::translation("bock triggers").translation());

  this->update();

  return ;
} // void GameSummary::language_update()

} // namespace UI_GTKMM_NS

#endif // #ifdef USE_UI_GTKMM
