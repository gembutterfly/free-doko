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

#include "players_db.h"
#include "ui.h"
#include "translations.h"
#include "cards.h"

#include "widgets/stock_button.h"

#include "../../party/party.h"
#include "../../player/player.h"
#include "../../player/playersDb.h"
#include "../../player/ai/aiDb.h"

#include "../../utils/string.h"


#include <gtkmm/treeview.h>
#include <gtkmm/alignment.h>
#include <gtkmm/radiobutton.h>
#include <gtkmm/stock.h>
#include <gtkmm/scrolledwindow.h>
#include <gdk/gdkkeysyms.h>
namespace UI_GTKMM_NS {

/**
 ** -> result
 **
 ** @param     statistic   statistic
 **
 ** @return    name of 'statistic'
 **/
string
  PlayersDB::name(Statistic const statistic)
  {
    switch(statistic) {
    case TOTAL:
      return "statistic total";
    case WON:
      return "statistic won";
    case LOST:
      return "statistic lost";
    case PERCENT_WON:
      return "statistic percent won";
    } // switch(statistic)

    return "";
  } // static string PlayersDB::name(Statistic statistic)

/**
 ** Constructor for the model
 **
 ** @param     playerno   number of players in the party
 **
 ** @return    -
 **/
PlayersDB::PlayersDBModel::PlayersDBModel(unsigned const playerno) :
  Gtk::TreeModel::ColumnRecord(),
  type(),
  statistic(playerno)
{
  this->add(this->type);
  for (unsigned p = 0; p < playerno; p++)
    this->add(this->statistic[p]);

  return ;
} // PlayersDB::PlayersDBModel::PlayersDBModel(unsigned playerno)

/**
 ** constructor
 **
 ** @param     parent   the parent object
 **
 ** @return    -
 **/
PlayersDB::PlayersDB(Base* const parent) :
  Base(parent),
  StickyDialog("Players DB", false)
{
  this->ui->add_window(*this);

  this->set_default_size(0, 3 * this->ui->cards->height());
  this->signal_realize().connect(sigc::mem_fun(*this, &PlayersDB::init));

#ifndef GLIBMM_DEFAULT_SIGNAL_HANDLERS_ENABLED
  this->signal_key_press_event().connect(sigc::mem_fun(*this, &PlayersDB::on_key_press_event));
#endif

  return ;
} // PlayersDB::PlayersDB(Base* parent)

/**
 ** create all subelements
 **
 ** @param     -
 **
 ** @return    -
 **/
void
PlayersDB::init()
{
  this->set_icon(this->ui->icon);
  this->ui->translations->add(*this, ::translation("FreeDoko %ttitle%",
                                                   ::translation("~players database")));


  { // vbox
    auto hbox = Gtk::manage(new Gtk::HBox(false, 1 EM));
    { // statistic type
      auto alignment
        = Gtk::manage(new Gtk::Alignment(Gtk::ALIGN_START, Gtk::ALIGN_START,
                                         0, 0));
      alignment->set_border_width(1 EX);
      hbox->add(*alignment);
      auto vbox = Gtk::manage(new Gtk::VBox(false, 1 EX / 2));
      alignment->add(*vbox);

      Gtk::RadioButton::Group statistic_group;
      for (int statistic = STATISTIC_FIRST;
           statistic <= STATISTIC_LAST;
           ++statistic) {
        auto const radio_button
          = Gtk::manage(new Gtk::RadioButton(statistic_group, ""));
        this->ui->translations->add(*radio_button,
                                    ::translation(PlayersDB::name(static_cast<Statistic const>(statistic))));
        vbox->add(*radio_button);
        radio_button->signal_clicked().connect(sigc::bind<Statistic const>(sigc::mem_fun(*this, &PlayersDB::set_statistic), static_cast<Statistic const>(statistic)));
        if (statistic == PERCENT_WON)
          radio_button->set_active(true);

      } // for (statistic)
    } // statistic type

    { // the treeview

      // the points table
      this->players_db_model = std::make_unique<PlayersDBModel>(this->ui->party().playerno());

      this->players_db_list
        = Gtk::TreeStore::create(*this->players_db_model);
      this->players_db_treeview
        = Gtk::manage(new Gtk::TreeView(this->players_db_list));

      this->players_db_treeview->append_column("players database data type",
                                               this->players_db_model->type);
      this->players_db_treeview->get_column_cell_renderer(0)->set_property("xalign", 1);
      for (unsigned p = 0; p < this->ui->party().playerno(); p++) {
        this->players_db_treeview->append_column(this->ui->party().player(p).name(),
                                                 this->players_db_model->statistic[p]);
        this->players_db_treeview->get_column_cell_renderer(p + 1)->set_property("xalign", 0.5);
      } // for (p < this->ui->party().playerno())

      { // the scrolled window
        auto scrolled_window = Gtk::manage(new Gtk::ScrolledWindow);
        scrolled_window->set_policy(Gtk::POLICY_NEVER,
                                    Gtk::POLICY_AUTOMATIC);
        scrolled_window->add(*(this->players_db_treeview));

        hbox->pack_start(*scrolled_window);
      } // the scrolled window
    } // the treeview
    this->get_vbox()->pack_start(*hbox);
  } // vbox

  { // action area
    { // clear button
      auto clear_button
        = Gtk::manage(new Gtk::StockButton(Gtk::Stock::CLEAR, "clear database"));
      this->ui->translations->add(*clear_button,
                                  ::translation("clear database"));
      this->add_action_widget(*clear_button, Gtk::RESPONSE_NONE);
      clear_button->signal_clicked().connect(sigc::mem_fun(*this, &PlayersDB::clear_db));
    } // clear button

    { // close button
      auto close_button
        = Gtk::manage(new Gtk::StockButton(Gtk::Stock::CLOSE, "close"));
      this->ui->translations->add(*close_button,
                                  ::translation("close"));
      this->add_action_widget(*close_button, Gtk::RESPONSE_CLOSE);

      close_button->grab_default();

      close_button->signal_clicked().connect(sigc::mem_fun(*this, &Gtk::Widget::hide));

    } // close button
  } // action area

  this->show_all_children();

  this->signal_show().connect(sigc::mem_fun(*this,
                                            &PlayersDB::recreate_db));
  this->recreate_db();

  this->language_update();

  return ;
} // void PlayersDB::init()

/**
 ** the party is opened
 **
 ** @param     -
 **
 ** @return    -
 **/
void
PlayersDB::party_open()
{
  this->recreate_db();

  return ;
} // void PlayersDB::party_open()

/**
 ** the party is closed
 **
 ** @param     -
 **
 ** @return    -
 **/
void
PlayersDB::party_close()
{
  this->hide();

  return ;
} // void PlayersDB::party_close()

/**
 ** the name of 'player' has changed
 **
 ** @param     player   the player with the changed name
 **
 ** @return    -
 **/
void
PlayersDB::name_changed(Player const& player)
{
  if (!this->get_realized())
    return ;

  this->players_db_treeview->get_column(::party.no(player) + 1
                                       )->set_title(" " + player.name() + " ");

  return ;
} // void PlayersDB::name_changed(Player player)

/**
 ** recreates the table
 **
 ** @param     -
 **
 ** @return    -
 **/
void
PlayersDB::recreate_db()
{
  if (!this->get_realized())
    return ;

  if (!this->players_db_list)
    return ;


  this->players_db_list->clear();

  { // ranking
    this->ranking = *this->players_db_list->append();
  } // ranking

  { // average game points
    this->average_game_points = *this->players_db_list->append();
  } // average game points

  { // average game trick points
    this->average_game_trick_points = *this->players_db_list->append();
  } // average game trick points

  {
    for (unsigned p = 0; p < 4; ++p) {
      this->ranking[this->players_db_model->statistic[p]]
        = this->statistic_data(TLWcount());
    }
  }

  { // game type
    this->games = *this->players_db_list->append();
    this->games_marriage
      = *(this->players_db_list->append(this->games.children()));
    this->games_poverty
      = *(this->players_db_list->append(this->games.children()));
    this->games_solo
      = *(this->players_db_list->append(this->games.children()));
    this->games_solo_color
      = *(this->players_db_list->append(this->games_solo.children()));
    this->games_solo_picture
      = *(this->players_db_list->append(this->games_solo.children()));
    this->games_solo_picture_single
      = *(this->players_db_list->append(this->games_solo_picture.children()));
    this->games_solo_picture_double
      = *(this->players_db_list->append(this->games_solo_picture.children()));

    this->game.clear();
    for (int gt = GAMETYPE::FIRST; gt <= GAMETYPE::LAST; ++gt) {
      switch (static_cast<GameType>(gt)) {
      case GAMETYPE::NORMAL:
        this->game.push_back(*this->players_db_list->prepend(this->games.children()));
        break;
      case GAMETYPE::THROWN_NINES:
      case GAMETYPE::THROWN_KINGS:
      case GAMETYPE::THROWN_NINES_AND_KINGS:
      case GAMETYPE::THROWN_RICHNESS:
      case GAMETYPE::FOX_HIGHEST_TRUMP:
      case GAMETYPE::POVERTY:
        this->game.push_back(*this->players_db_list->append(this->games_poverty.children()));
        break;
      case GAMETYPE::GENSCHER:
      case GAMETYPE::MARRIAGE:
      case GAMETYPE::MARRIAGE_SOLO:
      case GAMETYPE::MARRIAGE_SILENT:
        this->game.push_back(*this->players_db_list->append(this->games_marriage.children()));
        break;
      case GAMETYPE::SOLO_JACK:
      case GAMETYPE::SOLO_QUEEN:
        this->game.push_back(*this->players_db_list->append(this->games_solo_picture_single.children()));
        break;
      case GAMETYPE::SOLO_KING:
      case GAMETYPE::SOLO_QUEEN_JACK:
      case GAMETYPE::SOLO_KING_JACK:
      case GAMETYPE::SOLO_KING_QUEEN:
        this->game.push_back(*this->players_db_list->append(this->games_solo_picture_double.children()));
        break;
      case GAMETYPE::SOLO_KOEHLER:
        this->game.push_back(*this->players_db_list->append(this->games_solo_picture.children()));
        break;
      case GAMETYPE::SOLO_CLUB:
      case GAMETYPE::SOLO_HEART:
      case GAMETYPE::SOLO_SPADE:
      case GAMETYPE::SOLO_DIAMOND:
        this->game.push_back(*this->players_db_list->append(this->games_solo_color.children()));
        break;
      case GAMETYPE::SOLO_MEATLESS:
        this->game.push_back(*this->players_db_list->append(this->games_solo.children()));
        break;
#if 0
      default:
        DEBUG_ASSERTION(false,
                        "UI_GTKMM::PlayersDB::recreate_db()\n"
                        "  game type '" << static_cast<GameType>(gt) << "' is not considered");
        break;
#endif
      } // switch(gt)
    } // for (gt \in GameType)
  } // game type

  { // special points
    this->specialpoints = *this->players_db_list->append();
    this->specialpoints_winning
      = *this->players_db_list->append(this->specialpoints.children());
    this->specialpoints_announcement
      = *this->players_db_list->append(this->specialpoints.children());

    this->specialpoint.clear();
    for (int sp = SPECIALPOINT::FIRST; sp <= SPECIALPOINT::LAST; ++sp) {
      switch (static_cast<Specialpoint>(sp)) {
      case SPECIALPOINT::NOSPECIALPOINT:
      case SPECIALPOINT::CAUGHT_FOX:
      case SPECIALPOINT::CAUGHT_FOX_LAST_TRICK:
      case SPECIALPOINT::FOX_LAST_TRICK:
      case SPECIALPOINT::CHARLIE:
      case SPECIALPOINT::CAUGHT_CHARLIE:
      case SPECIALPOINT::DOLLE_CAUGHT_DOLLE:
      case SPECIALPOINT::HEART_TRICK:
      case SPECIALPOINT::DOPPELKOPF:
        this->specialpoint.push_back(*this->players_db_list->prepend(this->specialpoints.children()));
        break;
      case SPECIALPOINT::WON:
      case SPECIALPOINT::NO90:
      case SPECIALPOINT::NO60:
      case SPECIALPOINT::NO30:
      case SPECIALPOINT::NO0:
      case SPECIALPOINT::CONTRA_WON:
      case SPECIALPOINT::SOLO:
      case SPECIALPOINT::BOCK:
        this->specialpoint.push_back(*this->players_db_list->append(this->specialpoints_winning.children()));
        break;
      case SPECIALPOINT::NO120_SAID:
      case SPECIALPOINT::NO90_SAID:
      case SPECIALPOINT::NO60_SAID:
      case SPECIALPOINT::NO30_SAID:
      case SPECIALPOINT::NO0_SAID:
      case SPECIALPOINT::NO90_SAID_120_GOT:
      case SPECIALPOINT::NO60_SAID_90_GOT:
      case SPECIALPOINT::NO30_SAID_60_GOT:
      case SPECIALPOINT::NO0_SAID_30_GOT:
      case SPECIALPOINT::NO120_REPLY:
      case SPECIALPOINT::NO90_REPLY:
      case SPECIALPOINT::NO60_REPLY:
      case SPECIALPOINT::NO30_REPLY:
      case SPECIALPOINT::NO0_REPLY:
        this->specialpoint.push_back(*this->players_db_list->append(this->specialpoints_announcement.children()));
        break;
      } // switch(sp)
    } // for (sp \in Specialpoint)
  } // special points

  { // heuristics
    this->heuristics = *this->players_db_list->append();
    this->heuristics_general
      = *this->players_db_list->append(this->heuristics.children());
    this->heuristics_poverty
      = *this->players_db_list->append(this->heuristics.children());
    this->heuristics_solo
      = *this->players_db_list->append(this->heuristics.children());
    this->heuristics_solo_color
      = *this->players_db_list->append(this->heuristics_solo.children());
    this->heuristics_solo_picture
      = *this->players_db_list->append(this->heuristics_solo.children());
    this->heuristics_solo_meatless
      = *this->players_db_list->append(this->heuristics_solo.children());

    this->heuristic.clear();
    for (int h = Aiconfig::HEURISTIC_FIRST;
         h <= Aiconfig::HEURISTIC_LAST;
         ++h) {
      switch (static_cast<Aiconfig::Heuristic>(h)) {
      case Aiconfig::NO_HEURISTIC:
      case Aiconfig::MANUAL:
      case Aiconfig::BUG_REPORT:
      case Aiconfig::NETWORK:
        this->heuristic.push_back(this->heuristics);
        break;
      case Aiconfig::ONLY_VALID_CARD:
        this->heuristic.push_back(*this->players_db_list->prepend(this->heuristics.children()));
        break;
      case Aiconfig::CHOOSE_BEST_CARD:
        this->heuristic.push_back(*this->players_db_list->prepend(this->heuristics.children()));
        std::swap(this->heuristic[Aiconfig::ONLY_VALID_CARD - Aiconfig::HEURISTIC_FIRST],
                  this->heuristic.back());
        break;
      case Aiconfig::CHOOSE_ACE:
      case Aiconfig::CHOOSE_TEN:
      case Aiconfig::CHOOSE_SINGLE_ACE:
      case Aiconfig::PLAY_TO_MARRY:
      case Aiconfig::PLAY_TO_GET_MARRIED:
      case Aiconfig::CHOOSE_FOR_COLOR_TRICK:
      case Aiconfig::JAB_COLOR_OVER_FOX:
      case Aiconfig::START_WITH_COLOR:
      case Aiconfig::START_WITH_LOW_COLOR:
      case Aiconfig::START_WITH_LOW_TRUMP:
      case Aiconfig::RETRY_COLOR:
      case Aiconfig::PLAY_COLOR_FOR_PARTNER:
      case Aiconfig::TRY_COLOR_FOR_PARTNER:
      case Aiconfig::PLAY_COLOR_FOR_PARTNER_ACE:
      case Aiconfig::PLAY_BAD_COLOR:
      case Aiconfig::SERVE_COLOR_TRICK:
      case Aiconfig::SERVE_TRUMP_TRICK:
      case Aiconfig::CHOOSE_PFUND:
      case Aiconfig::CHOOSE_PFUND_BEFORE_PARTNER:
      case Aiconfig::JAB_FOR_ACE:
      case Aiconfig::CREATE_FEHL:
      case Aiconfig::BEST_WINNING:
      case Aiconfig::LOW_HIGH:
      case Aiconfig::PLAY_TRUMP:
      case Aiconfig::PLAY_FOR_TEAM:
      case Aiconfig::JAB_FOX:
      case Aiconfig::TRY_FOR_DOPPELKOPF:
      case Aiconfig::PLAY_FOR_PARTNER_WORRIES:
      case Aiconfig::DRAW_TRUMP:
      case Aiconfig::PLAY_TO_JAB_LATER:
      case Aiconfig::PARTNER_BACKHAND_DRAW_TRUMP:
      case Aiconfig::PLAY_HIGHEST_COLOR_CARD_IN_GAME:
      case Aiconfig::GET_TRICK_FOR_ANNOUNCEMENT:
      case Aiconfig::GRAB_TRICK:
      case Aiconfig::LET_TRICK_PASS:
        this->heuristic.push_back(*this->players_db_list->append(this->heuristics_general.children()));
        break;
      case Aiconfig::COLOR_JAB_FOR_ACE:
      case Aiconfig::COLOR_LOW_HIGH:
      case Aiconfig::PLAY_COLOR_IN_SOLO:
        this->heuristic.push_back(*this->players_db_list->append(this->heuristics_solo_color.children()));
        break;
      case Aiconfig::PICTURE_SECONDBESTTRUMP:
        this->heuristic.push_back(*this->players_db_list->append(this->heuristics_solo_picture.children()));
        break;
      case Aiconfig::CHOOSE_PFUND_POVERTY:
      case Aiconfig::POVERTY_SPECIAL_PLAY_PFUND:
      case Aiconfig::POVERTY_SPECIAL_GIVE_NO_POINTS:
      case Aiconfig::POVERTY_SPECIAL_OFFER_PFUND:
      case Aiconfig::POVERTY_RE_TRUMP_COLOR_TRICK_HIGH:
      case Aiconfig::POVERTY_RE_PLAY_TRUMP:
      case Aiconfig::POVERTY_CONTRA_PLAY_COLOR:
      case Aiconfig::POVERTY_CONTRA_TRUMP_COLOR_TRICK_HIGH:
      case Aiconfig::POVERTY_LEAVE_TO_PARTNER:
      case Aiconfig::POVERTY_OVERJAB_RE:
      case Aiconfig::POVERTY_BEST_WINNING_CARD:
        this->heuristic.push_back(*this->players_db_list->append(this->heuristics_poverty.children()));
        break;
      case Aiconfig::MEATLESS_PLAYHIGHESTCOLOR:
        this->heuristic.push_back(*this->players_db_list->append(this->heuristics_solo_meatless.children()));
        break;
      } // switch(h)
    } // for (h \in Aiconfig::Heuristic)
  } // heuristics

  this->update_db();
  this->language_update();

  this->players_db_treeview->show_all();

  return ;
} // void PlayersDB::recreate_db()

/**
 ** updates the statistics
 **
 ** @param     -
 **
 ** @return    -
 **/
void
PlayersDB::update_db()
{
  if (!this->get_realized())
    return ;

  if (!this->players_db_list)
    return ;

  Party const& party = this->ui->party();

  for (unsigned p = 0; p < party.playerno(); ++p) {
    auto const& player = this->ui->party().player(p);
    auto const& db = player.db();

    this->ranking[this->players_db_model->statistic[p]]
      = DK::Utils::String::to_string(static_cast<int>(db.rank.value() * 10) / 10.0);

    this->average_game_points[this->players_db_model->statistic[p]]
      = DK::Utils::String::to_string(static_cast<int>(db.averageGamePoints() * 10) / 10.0);

    this->average_game_trick_points[this->players_db_model->statistic[p]]
      = DK::Utils::String::to_string(static_cast<int>(db.averageGameTrickPoints() * 10) / 10.0);

    { // game type
      this->games[this->players_db_model->statistic[p]]
        = this->statistic_data(db.games_all());
      this->games_marriage[this->players_db_model->statistic[p]]
        = this->statistic_data(db.games_group_marriage());
      this->games_poverty[this->players_db_model->statistic[p]]
        = this->statistic_data(db.games_group_poverty());
      this->games_solo[this->players_db_model->statistic[p]]
        = this->statistic_data(db.games_group_solo());
      this->games_solo_color[this->players_db_model->statistic[p]]
        = this->statistic_data(db.games_group_solo_color());
      this->games_solo_picture[this->players_db_model->statistic[p]]
        = this->statistic_data(db.games_group_solo_picture());
      this->games_solo_picture_single[this->players_db_model->statistic[p]]
        = this->statistic_data(db.games_group_solo_picture_single());
      this->games_solo_picture_double[this->players_db_model->statistic[p]]
        = this->statistic_data(db.games_group_solo_picture_double());
      for (int gt = GAMETYPE::FIRST; gt <= GAMETYPE::LAST; ++gt) {
        this->game[gt][this->players_db_model->statistic[p]]
          = this->statistic_data(db.games(static_cast<GameType const>(gt)));
      } // for (gt \in GAMETYPE)
    } // game type

    { // special points
      this->specialpoints[this->players_db_model->statistic[p]]
        = this->statistic_data(db.specialpoints_all());
      this->specialpoints_winning[this->players_db_model->statistic[p]]
        = this->statistic_data(db.specialpoints_group_winning());
      this->specialpoints_announcement[this->players_db_model->statistic[p]]
        = this->statistic_data(db.specialpoints_group_announcement());
      for (int sp = SPECIALPOINT::FIRST; sp <= SPECIALPOINT::LAST; ++sp) {
        this->specialpoint[sp][this->players_db_model->statistic[p]]
          = this->statistic_data(db.specialpoints(static_cast<Specialpoint const>(sp)));
      } // for (sp \in Specialpoint)
    } // special points
    { // heuristics
      if (dynamic_cast<AiDb const*>(&db) == nullptr)
        continue;
      auto const& aidb = dynamic_cast<AiDb const&>(db);
      auto const total = aidb.heuristic(Aiconfig::NO_HEURISTIC);

      this->heuristics[this->players_db_model->statistic[p]]
        = this->heuristic_statistic_data(total, total);
      this->heuristics_general[this->players_db_model->statistic[p]]
        = this->heuristic_statistic_data(total, aidb.heuristics_group_general());
      this->heuristics_poverty[this->players_db_model->statistic[p]]
        = this->heuristic_statistic_data(total, aidb.heuristics_group_poverty());
      this->heuristics_solo[this->players_db_model->statistic[p]]
        = this->heuristic_statistic_data(total, aidb.heuristics_group_solo());
      this->heuristics_solo_color[this->players_db_model->statistic[p]]
        = this->heuristic_statistic_data(total, aidb.heuristics_group_solo_color());
      this->heuristics_solo_picture[this->players_db_model->statistic[p]]
        = this->heuristic_statistic_data(total, aidb.heuristics_group_solo_picture());
      this->heuristics_solo_meatless[this->players_db_model->statistic[p]]
        = this->heuristic_statistic_data(total, aidb.heuristics_group_solo_meatless());
      for (int h = Aiconfig::HEURISTIC_FIRST;
           h <= Aiconfig::HEURISTIC_LAST;
           ++h) {
        if (this->heuristic[h] == this->heuristics)
          continue;
        this->heuristic[h][this->players_db_model->statistic[p]]
          = this->heuristic_statistic_data(total, aidb.heuristic(static_cast<Aiconfig::Heuristic const>(h)));
      } // for (h \in Heuristic)
    } // heuristics
  } // for (p < party.playerno())

  return ;
} // void PlayersDB::update_db()

/**
 ** updates all language elements
 **
 ** @param     -
 **
 ** @return    -
 **/
void
PlayersDB::language_update()
{
  if (!this->get_realized())
    return ;

  this->players_db_treeview->get_column(0)->set_title(::translation("players database data type").translation());

  { // ranking
    this->ranking[this->players_db_model->type]
      = ::translation("ranking").translation();
  } // ranking

  { // average game points
    this->average_game_points[this->players_db_model->type]
      = ::translation("average game points").translation();
  } // average game points

  { // average game trick points
    this->average_game_trick_points[this->players_db_model->type]
      = ::translation("average game trick points").translation();
  } // average game trick points

  { // game type
    this->games[this->players_db_model->type]
      = ::translation("games").translation();
    this->games_marriage[this->players_db_model->type]
      = ::translation("GameType::Group::marriage").translation();
    this->games_poverty[this->players_db_model->type]
      = ::translation("GameType::Group::poverty").translation();
    this->games_solo[this->players_db_model->type]
      = ::translation("GameType::Group::solo").translation();
    this->games_solo_color[this->players_db_model->type]
      = ::translation("GameType::Group::Solo::color").translation();
    this->games_solo_picture[this->players_db_model->type]
      = ::translation("GameType::Group::Solo::picture").translation();
    this->games_solo_picture_single[this->players_db_model->type]
      = ::translation("GameType::Group::Solo::Picture::single").translation();
    this->games_solo_picture_double[this->players_db_model->type]
      = ::translation("GameType::Group::Solo::Picture::double").translation();
    for (int gt = GAMETYPE::FIRST; gt <= GAMETYPE::LAST; ++gt) {
      this->game[gt][this->players_db_model->type]
        = ::translation(static_cast<GameType const>(gt)).translation();
    } // for (gt \in GAMETYPE)
  } // game type
  { // special points
    this->specialpoints[this->players_db_model->type]
      = ::translation("SpecialPoint::specialpoints").translation();
    this->specialpoints_winning[this->players_db_model->type]
      = ::translation("SpecialPoint::Group::winning").translation();
    this->specialpoints_announcement[this->players_db_model->type]
      = ::translation("SpecialPoint::Group::announcement").translation();
    for (int sp = SPECIALPOINT::FIRST; sp <= SPECIALPOINT::LAST; ++sp) {
      this->specialpoint[sp][this->players_db_model->type]
        = ::translation(static_cast<Specialpoint const>(sp)).translation();
    } // for (sp \in Specialpoint)
  } // special points
  { // heuristics
    this->heuristics[this->players_db_model->type]
      = ::translation("AiConfig::Group::heuristics").translation();
    this->heuristics_general[this->players_db_model->type]
      = ::translation("AiConfig::Heuristic::Group::normal game").translation();
    this->heuristics_poverty[this->players_db_model->type]
      = ::translation("AiConfig::Heuristic::Group::poverty").translation();
    this->heuristics_solo[this->players_db_model->type]
      = ::translation("AiConfig::Heuristic::Group::solo").translation();
    this->heuristics_solo_color[this->players_db_model->type]
      = ::translation("AiConfig::Heuristic::Group::Solo::color").translation();
    this->heuristics_solo_picture[this->players_db_model->type]
      = ::translation("AiConfig::Heuristic::Group::Solo::picture").translation();
    this->heuristics_solo_meatless[this->players_db_model->type]
      = ::translation("AiConfig::Heuristic::Group::Solo::meatless").translation();
    for (int h = Aiconfig::HEURISTIC_FIRST;
         h <= Aiconfig::HEURISTIC_LAST;
         ++h) {
      if (this->heuristic[h] != this->heuristics)
        this->heuristic[h][this->players_db_model->type]
          = ::translation(static_cast<Aiconfig::Heuristic const>(h)).translation();
    } // for (h \in Heuristic)
  } // heuristics

  return ;
} // void PlayersDB::language_update()

/**
 ** sets the statistic
 **
 ** @param     statistic   new statistic
 **
 ** @return    -
 **/
void
PlayersDB::set_statistic(Statistic const statistic)
{
  this->statistic = statistic;
  this->update_db();

  return ;
} // void PlayersDB::set_statistic(Statistic const statistic)

/**
 ** clears the database
 **
 ** @param     -
 **
 ** @return    -
 **
 ** @todo    remove 'const cast'
 **/
void
PlayersDB::clear_db()
{
  for (auto p : ::party.players())
    p->db().clear();

  this->update_db();

  return ;
} // void PlayersDB::clear_db()

/**
 ** -> result
 **
 ** @param     tlwcount   counter
 **
 ** @return    string of the data analysed by 'this->statistic'
 **/
Glib::ustring
PlayersDB::statistic_data(TLWcount const& tlwcount) const
{
  switch(this->statistic) {
  case TOTAL:
    return DK::Utils::String::to_string(tlwcount.total());
  case WON:
    if (tlwcount.total() > 0)
      return DK::Utils::String::to_string(tlwcount.won());
    else
      return "-";
  case LOST:
    if (tlwcount.total() > 0)
      return DK::Utils::String::to_string(tlwcount.lost());
    else
      return "-";
  case PERCENT_WON:
    if (tlwcount.total() > 0)
      return (DK::Utils::String::to_string(tlwcount.won() * 100
                                           / tlwcount.total())
              + "%");
    else
      return "-";
  } // switch(this->statistic)

  return "-";
} // void PlayersDB::statistic_data(TLWcout tlwcount) const

/**
 ** -> result
 **
 ** @param     total   total number of heuristics
 ** @param     num     number of the active heuristics
 **
 ** @return    string of the data analysed by 'this->statistic'
 **/
Glib::ustring
PlayersDB::heuristic_statistic_data(unsigned const total,
                                    unsigned const num) const
{
  if (num == 0)
    return "-";

  switch(this->statistic) {
  case TOTAL:
  case WON:
  case LOST:
    return DK::Utils::String::to_string(num);
  case PERCENT_WON:
    return (DK::Utils::String::to_string((num * 100 / total))
            + "."
            + DK::Utils::String::to_string((num * 1000 / total) % 10)
            + "%");
  } // switch(this->statistic)

  return "-";
} // void PlayersDB::heuristic_statistic_data(unsigned total, unsigned num) const

/**
 ** a key has been pressed
 ** C-o: output of the statistics on 'stdout'
 **
 ** @param     key   the key
 **
 ** @return    whether the key was managed
 **/
bool
PlayersDB::on_key_press_event(GdkEventKey* const key)
{
  bool managed = false;

  if ((key->state & ~GDK_SHIFT_MASK) == GDK_CONTROL_MASK) {
    switch (key->keyval) {
    case GDK_KEY_o: // ouput of the preferences
      for (unsigned p = 0; p < this->ui->party().playerno(); ++p)
        cout << this->ui->party().player(p).name() << '\n'
          << "{\n"
          << this->ui->party().player(p).db()
          << "}\n";
      managed = true;
      break;
    } // switch (key->keyval)
  } // if (key->state == GDK_CONTROL_MASK)

  return (managed
          || this->StickyDialog::on_key_press_event(key)
          || this->ui->key_press(key));
} // bool PlayersDB::on_key_press_event(GdkEventKey* key)

} // namespace UI_GTKMM_NS

#endif // #ifdef USE_UI_GTKMM
