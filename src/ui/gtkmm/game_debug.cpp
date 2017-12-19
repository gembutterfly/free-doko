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

#include "game_debug.h"

#include "ui.h"
#include "translations.h"
#include "cards.h"

#include "widgets/stock_button.h"

#include "../../card/trick.h"
#include "../../player/ai/ai.h"
#include "../ui.wrap.h"
#include "../../utils/string.h"

#include <gtkmm/stock.h>
#include <gtkmm/label.h>
#include <gtkmm/treeview.h>
#include <gtkmm/liststore.h>
#include <gtkmm/treestore.h>
#include <gtkmm/alignment.h>
#include <gtkmm/frame.h>
#include <gtkmm/box.h>
#include <gtkmm/scrolledwindow.h>

namespace UI_GTKMM_NS {

/**
 ** constructor
 **
 ** @param    -
 **
 ** @return   -
 **/
GameDebug::PlayerInfoModel::PlayerInfoModel()
{
  this->add(this->name);
  this->add(this->team);
  this->add(this->swines);
  this->add(this->announcement);
  this->add(this->points);
  this->add(this->tricks);
} // GameDebug::PlayerInfoModel::PlayerInfoModel()

/**
 ** constructor
 **
 ** @param    -
 **
 ** @return   -
 **/
GameDebug::TeamInfoModel::TeamInfoModel()
{
  this->add(this->name);
  this->known_team.resize(::party.rule()(Rule::NUMBER_OF_PLAYERS_IN_GAME));
  for (auto& t : this->known_team)
    this->add(t);
} // GameDebug::TeamInfoModel::TeamInfoModel()

/**
 ** constructor
 **
 ** @param    -
 **
 ** @return   -
 **/
GameDebug::TricksModel::TricksModel()
{
  this->add(this->trickno);
  this->add(this->playerno);
  this->add(this->card);
  this->add(this->heuristic);
  this->add(this->isvirtual);
} // GameDebug::TricksModel::TricksModel()

/**
 ** constructor
 **
 ** @param     parent   the parent widget
 **
 ** @return    -
 **/
GameDebug::GameDebug(Base* const parent) :
  Base(parent),
  StickyDialog("Game info", false),
  OS()
{
  this->ui->add_window(*this);

#ifndef RELEASE
  dynamic_cast<UI_Wrap*>(::ui)->append(this);

  this->init();

  this->set_default_size(static_cast<int>(8 * this->ui->cards->height()),
                         static_cast<int>(5 * this->ui->cards->height()));
#endif

#ifndef GLIBMM_DEFAULT_SIGNAL_HANDLERS_ENABLED
  this->signal_key_press_event().connect(sigc::mem_fun(*this, &GameDebug::on_key_press_event));
#endif

  return ;
} // GameDebug::GameDebug(Base* parent)

/**
 ** destructor
 **
 ** @param     -
 **
 ** @return    -
 **/
GameDebug::~GameDebug()
{
  dynamic_cast<UI_Wrap*>(::ui)->remove(this);

  return ;
} // GameDebug::~GameDebug()

/**
 ** the game is opened
 **
 ** @param     game   game that is opened
 **
 ** @return    -
 **/
void
GameDebug::game_open(Game const& game)
{
  this->OS::game_open(game);

  this->tricks_rows.clear();
  this->tricks_store->clear();

  while (!this->ai_test_rows.empty())
    this->ai_test_rows.pop();

  for (auto p = game.players_begin(); p != game.players_end(); ++p)
    this->name_changed(**p);

  return ;
} // void GameDebug::game_open(Game game)

/**
 ** the game is started
 **
 ** @param     -
 **
 ** @return    -
 **/
void
GameDebug::game_start()
{
  this->update_info();

  return ;
} // void GameDebug::game_start()

/**
 ** a new trick has opened
 **
 ** @param     trick   new trick
 **
 ** @return    -
 **/
void
GameDebug::trick_open(::Trick const& trick)
{ 
  this->OS::trick_open(trick);

  if (!this->tricks_rows.empty())
    this->tricks_treeview->collapse_row(this->tricks_store->get_path(this->tricks_rows.back()));
  this->tricks_rows.push_back(*this->tricks_store->append());
  this->tricks_rows.back()[this->tricks_model.trickno]
    = DK::Utils::String::to_string(trick.no());

  return ;
} // void GameDebug::trick_open(Trick trick)

/**
 ** 'player' has played 'card'
 **
 ** @param     card   the played card
 **
 ** @return    -
 **/
void
GameDebug::card_played(HandCard const& card)
{ 
  Player const& player = card.player();

  Gtk::TreeModel::Row row;
  if (this->ai_test_rows.empty())
    row = *this->tricks_store->append(this->tricks_rows.back().children());
  else {
    row = this->ai_test_rows.top();
    this->tricks_treeview->collapse_row(this->tricks_store->get_path(row));
    this->ai_test_rows.pop();
    while (!this->ai_test_rows.empty())
      this->ai_test_rows.pop();
  }
  this->tricks_treeview->expand_row(this->tricks_store->get_path(row.parent()), false);
  row[this->tricks_model.playerno]
    = DK::Utils::String::to_string(player.game().no(player));
  // ToDo: translation in game
  row[this->tricks_model.card]
    = ::translation(card).translation();
  // ToDo: translation in game
  if (dynamic_cast<Ai const*>(&player)) {
    if (dynamic_cast<Ai const&>(player).lastHeuristic_
        == Aiconfig::CHOOSE_BEST_CARD)
      row[this->tricks_model.heuristic]
        = ::translation(dynamic_cast<Ai const&>(player).aitype()).translation();
    else
      row[this->tricks_model.heuristic]
        = ::translation(dynamic_cast<Ai const&>(player).lastHeuristic_).translation();
  }

  this->update_info();

  return ;
} // void GameDebug::card_played(HandCard card)

/**
 ** the ai tests the card 'card'
 **
 ** @param     card   	the card that is tested
 ** @param     playerno   number of the player who plays the card
 **
 ** @return    -
 **
 ** @todo	 automatic card translation
 **/
void
GameDebug::ai_test_card(Card const& card, unsigned const playerno)
{
  if (this->ai_test_rows.empty()) {
    this->ai_test_rows.push(*this->tricks_store->append(this->tricks_rows.back().children()));
    this->ai_test_rows.top()[this->tricks_model.heuristic]
      = ::translation(dynamic_cast<Ai const&>(this->ui->game().player_current()).aitype()).translation();
  }
  this->ai_test_rows.push(*this->tricks_store->append(this->ai_test_rows.top().children()));
  if (this->ai_test_rows.size() == 2)
    this->tricks_treeview->expand_to_path(this->tricks_store->get_path(this->ai_test_rows.top().parent()));
  this->ai_test_rows.top()[this->tricks_model.card]
    = ::translation(card).translation();
  this->ai_test_rows.top()[this->tricks_model.playerno]
    = DK::Utils::String::to_string(playerno);
  return ;
} // void GameDebug::ai_test_card(Card card, unsigned playerno)

/**
 ** the ai has calculated a weighting for the tested card
 **
 ** @param     weighting   the weighting for the tested card
 **
 ** @return    -
 **/
void
GameDebug::ai_card_weighting(int const weighting)
{
  if (this->ai_test_rows.empty())
    DEBUG_ASSERTION(false,
                    "GameDebug::ai_card_weighting():\n"
                    "  ai_test_rows is empty");
  while (this->ai_test_rows.top()[this->tricks_model.isvirtual])
    this->ai_test_rows.pop();

  if ( (weighting == INT_MIN)
      || (weighting == INT_MAX) )
    this->ai_test_rows.top()[this->tricks_model.heuristic]
      = "-";
  else
    this->ai_test_rows.top()[this->tricks_model.heuristic]
      = DK::Utils::String::to_string(weighting);

  this->ai_test_rows.top()[this->tricks_model.isvirtual]
    = false;

  this->tricks_treeview->collapse_row(this->tricks_store->get_path(this->ai_test_rows.top()));
  this->ai_test_rows.pop();

  return ;
} // void GameDebug::ai_card_weighting(int weighting)

/**
 ** virtual game: card is played
 **
 ** @param     card   the card that is played
 **
 ** @return    -
 **
 ** @todo	translate card automatic
 **/
void
GameDebug::virtual_card_played(HandCard const& card)
{
  ::Player const& player = card.player();

  this->ai_test_rows.push(*this->tricks_store->append(this->ai_test_rows.top().children()));
  auto row = this->ai_test_rows.top();

  row[this->tricks_model.card]
    = ::translation(card).translation();
  row[this->tricks_model.playerno]
    = DK::Utils::String::to_string(player.game().no(player));
  if (dynamic_cast<Ai const*>(&player)) {
    if (dynamic_cast<Ai const&>(player).lastHeuristic_
        == Aiconfig::CHOOSE_BEST_CARD)
      row[this->tricks_model.heuristic]
        = ::translation(dynamic_cast<Ai const&>(player).aitype()).translation();
    else
      row[this->tricks_model.heuristic]
        = ::translation(dynamic_cast<Ai const&>(player).lastHeuristic_).translation();
  }
  row[this->tricks_model.isvirtual]
    = true;

  return ;
} // void GameDebug::virtual_card_played(HandCard card)

/**
 ** the announcement 'announcement' has been made by player 'player'
 **
 ** @param     announcement   the announcement
 ** @param     player   	the player, who has made the announcement
 **
 ** @return    -
 **/
void
GameDebug::announcement_made(Announcement const& announcement,
                             Player const& player)
{
  this->update_info();

  return ;
} // void GameDebug::announcement_made(Announcement announcement, Player player)

/**
 ** the player has swines
 **
 ** @param     player   	the player with the swines
 **
 ** @return    -
 **/
void
GameDebug::swines_announced(Player const& player)
{
  this->update_info();

  return ;
} // void GameDebug::swines_announced(Player player)

/**
 ** the player has hyperswines
 **
 ** @param     player   	the player with the swines
 **
 ** @return    -
 **/
void
GameDebug::hyperswines_announced(Player const& player)
{
  this->update_info();

  return ;
} // void GameDebug::hyperswines_announced(Player player)

/**
 ** marriage: Information of the new team
 **
 ** @param     bridegroom   the player with the marriage
 ** @param     bride   	the bride
 **
 ** @return    -
 **/
void
GameDebug::marriage(Player const& bridegroom, Player const& bride)
{
  this->update_info();

  return ;
} // void GameDebug::marriage(Player bridegroom, Player bride)

/**
 ** genscher: Information of the new team
 **
 ** @param     genscher   genscher player
 ** @param     partner    partner of the genscher player
 **
 ** @return    -
 **/
void
GameDebug::genscher(Player const& genscher, Player const& partner)
{
  this->update_info();

  return ;
} // void GameDebug::genscher(Player genscher, Player partner)

/**
 ** create all subelements
 **
 ** @param     -
 **
 ** @return    -
 **/
void
GameDebug::init()
{
  this->ui->translations->add(*this, 
                              ::translation("~game information"));

  this->set_icon(this->ui->icon);

  { // action area
    auto close_button
      = Gtk::manage(new Gtk::StockButton(Gtk::Stock::CLOSE, "close"));
    this->ui->translations->add(*close_button,
                                ::translation("close"));
    this->add_action_widget(*close_button, Gtk::RESPONSE_CLOSE);

    close_button->grab_default();

    close_button->signal_clicked().connect(sigc::mem_fun(*this, &Gtk::Widget::hide));
  } // action area

  { // content
    auto content_box = Gtk::manage(new Gtk::HBox(false, 2 EM));

    auto left_box = Gtk::manage(new Gtk::VBox(false, 1 EX));
    content_box->pack_start(*left_box, false, false);
    { // game info
      auto box = Gtk::manage(new Gtk::VBox(true, 1 EX));
      box->set_border_width(1 EM);

      { // seed
        auto box2 = Gtk::manage(new Gtk::HBox(false, 0));
        auto label = Gtk::manage(new Gtk::Label("seed: "));
        this->ui->translations->add(*label,
                                    ::translation("seed") + ": ");
        box2->pack_start(*label, false, false);
        this->seed = Gtk::manage(new Gtk::Label(""));
        box2->pack_start(*this->seed, false, false);

        box->add(*box2);
      } // seed
      { // game type
        auto box2 = Gtk::manage(new Gtk::HBox(false, 0));
        auto label = Gtk::manage(new Gtk::Label("gametype: "));
        this->ui->translations->add(*label,
                                    ::translation("gametype") + ": ");
        box2->pack_start(*label, false, false);
        this->gametype = Gtk::manage(new Gtk::Label(""));
        box2->pack_start(*this->gametype, false, false);

        box->add(*box2);
      } // game type
      { // soloplayer
        auto box2 = Gtk::manage(new Gtk::HBox(false, 0));
        auto label = Gtk::manage(new Gtk::Label("soloplayer: "));
        this->ui->translations->add(*label,
                                    ::translation("soloplayer") + ": ");
        box2->pack_start(*label, false, false);
        this->soloplayer = Gtk::manage(new Gtk::Label(""));
        box2->pack_start(*this->soloplayer, false, false);

        box->add(*box2);
      } // seed

      auto frame = Gtk::manage(new Gtk::Frame("game info"));
      this->ui->translations->add(*dynamic_cast<Gtk::Label*>(frame->get_label_widget()),
                                  ::translation("game info"));
      frame->add(*box);
      frame->set_border_width(1 EM);
      auto alignment
        = Gtk::manage(new Gtk::Alignment(Gtk::ALIGN_CENTER,
                                         Gtk::ALIGN_CENTER,
                                         0, 0));
      alignment->add(*frame);
      left_box->pack_start(*alignment, false, true);
    } // game info

    { // player_info
      this->player_info_list
        = Gtk::ListStore::create(this->player_info_model);
      this->player_info_treeview
        = Gtk::manage(new Gtk::TreeView(this->player_info_list));
      this->player_info_treeview->get_selection()->set_mode(Gtk::SELECTION_NONE);

      unsigned column = 0;
      this->player_info_treeview->append_column("~name",
                                                this->player_info_model.name);
      column += 1;
      this->player_info_treeview->append_column("~team",
                                                this->player_info_model.team);
      this->player_info_treeview->get_column_cell_renderer(column)->set_property("xalign", 0.5);
      column += 1;
      this->player_info_treeview->append_column("~swines",
                                                this->player_info_model.swines);
      this->player_info_treeview->get_column_cell_renderer(column)->set_property("xalign", 0.5);
      column += 1;
      this->player_info_treeview->append_column("~announcement",
                                                this->player_info_model.announcement);
      this->player_info_treeview->get_column_cell_renderer(column)->set_property("xalign", 0.5);
      column += 1;
      this->player_info_treeview->append_column("~points",
                                                this->player_info_model.points);
      this->player_info_treeview->get_column_cell_renderer(column)->set_property("xalign", 0.5);
      column += 1;
      this->player_info_treeview->append_column("~tricks",
                                                this->player_info_model.tricks);
      this->player_info_treeview->get_column_cell_renderer(column)->set_property("xalign", 0.5);
      column += 1;

      for (unsigned p = 0;
           p < ::party.rule()(Rule::NUMBER_OF_PLAYERS_IN_GAME);
           ++p) {
        this->player_info_rows.push_back(*this->player_info_list->append());
        this->player_info_rows.back()[this->player_info_model.name]
          = ::party.player(p).name();
      }

      auto a = Gtk::manage(new Gtk::Alignment);
      a->set_border_width(1 EM);
      a->add(*this->player_info_treeview);
      auto frame = Gtk::manage(new Gtk::Frame("player info"));
      this->ui->translations->add(*dynamic_cast<Gtk::Label*>(frame->get_label_widget()),
                                  ::translation("player info"));
      frame->add(*a);
      frame->set_border_width(1 EM);
      auto alignment
        = Gtk::manage(new Gtk::Alignment(Gtk::ALIGN_CENTER,
                                         Gtk::ALIGN_CENTER,
                                         0, 0));
      alignment->add(*frame);
      left_box->pack_start(*alignment, false, true);
    } // player_info

    { // team_info
      this->team_info_list
        = Gtk::ListStore::create(this->team_info_model);
      this->team_info_treeview
        = Gtk::manage(new Gtk::TreeView(this->team_info_list));
      this->team_info_treeview->get_selection()->set_mode(Gtk::SELECTION_NONE);

      unsigned column = 0;
      this->team_info_treeview->append_column("~name",
                                              this->team_info_model.name);
      column += 1;
      for (unsigned p = 0;
           p < this->team_info_model.known_team.size();
           ++p) {
        this->team_info_treeview->append_column(::party.player(p).name(),
                                                this->team_info_model.known_team[p]);
        this->team_info_treeview->get_column_cell_renderer(column)->set_property("xalign", 0.5);
        column += 1;
      }

      // real team
      this->team_info_rows.push_back(*this->team_info_list->append());
      // teaminfo of the game
      this->team_info_rows.push_back(*this->team_info_list->append());
      for (unsigned p = 0;
           p < this->team_info_model.known_team.size();
           ++p) {
        this->team_info_rows.push_back(*this->team_info_list->append());
        this->team_info_rows.back()[this->team_info_model.name]
          = ::party.player(p).name();
      } // for (p)

      auto a = Gtk::manage(new Gtk::Alignment);
      a->set_border_width(1 EM);
      a->add(*this->team_info_treeview);
      Gtk::Frame* frame = Gtk::manage(new Gtk::Frame("team info"));
      this->ui->translations->add(*dynamic_cast<Gtk::Label*>(frame->get_label_widget()),
                                  ::translation("team info"));
      frame->add(*a);
      frame->set_border_width(1 EM);
      auto alignment
        = Gtk::manage(new Gtk::Alignment(Gtk::ALIGN_CENTER,
                                         Gtk::ALIGN_CENTER,
                                         0, 0));
      alignment->add(*frame);
      left_box->pack_start(*alignment, false, true);
    } // team_info

    { // tricks
      this->tricks_store = Gtk::TreeStore::create(this->tricks_model);
      this->tricks_treeview
        = Gtk::manage(new Gtk::TreeView(this->tricks_store));
      this->tricks_treeview->get_selection()->set_mode(Gtk::SELECTION_MULTIPLE);
      this->tricks_treeview->set_rules_hint(true);

      unsigned column = 0;
      this->tricks_treeview->append_column("Card",
                                           this->tricks_model.card);
      column += 1;
      this->tricks_treeview->append_column("Player",
                                           this->tricks_model.playerno);
      this->tricks_treeview->get_column_cell_renderer(column)->set_property("xalign", 0.5);
      column += 1;
      this->tricks_treeview->append_column("Trick",
                                           this->tricks_model.trickno);
      this->tricks_treeview->get_column_cell_renderer(column)->set_property("xalign", 0.5);
      column += 1;
      this->tricks_treeview->append_column("heuristic",
                                           this->tricks_model.heuristic);
      column += 1;

      auto scrolled_window = Gtk::manage(new Gtk::ScrolledWindow);
      scrolled_window->set_policy(Gtk::POLICY_AUTOMATIC,
                                  Gtk::POLICY_AUTOMATIC);
      scrolled_window->add(*this->tricks_treeview);
      scrolled_window->set_hadjustment(this->tricks_treeview->get_hadjustment());
      scrolled_window->set_vadjustment(this->tricks_treeview->get_vadjustment());

      auto a = Gtk::manage(new Gtk::Alignment);
      a->set_border_width(1 EM);
      a->add(*scrolled_window);
      auto frame = Gtk::manage(new Gtk::Frame("tricks"));
      this->ui->translations->add(*dynamic_cast<Gtk::Label*>(frame->get_label_widget()),
                                  ::translation("tricks"));
      frame->add(*a);
      frame->set_border_width(1 EM);

      content_box->pack_end(*frame, true, true);
    } // tricks
    this->get_vbox()->pack_start(*content_box);
  } // content

  this->show_all_children();

  this->realize();

  this->language_update();

  return ;
} // void GameDebug::init()

/**
 ** updates the information for the players
 **
 ** @param     -
 **
 ** @return    -
 **/
void
GameDebug::update_info()
{
  if (!(::game_status & GAMESTATUS::GAME))
    return ;

  Game const& game = this->ui->game();

  this->seed->set_label(DK::Utils::String::to_string(game.seed()));
  this->gametype->set_label(::translation(game.type()).translation());
  if (game.exists_soloplayer())
    this->soloplayer->set_label(game.soloplayer().name());
  else
    this->soloplayer->set_label("-");

  for (unsigned p = 0; p < game.number_of_players(); ++p) {
    ::Player const& player = game.player(p);

    { // player info
      auto& row = this->player_info_rows[p];

      row[this->player_info_model.team]
        = ::translation(player.team()).translation();
      row[this->player_info_model.swines]
        = (player.has_swines() ? "S" : "");
      if (player.has_hyperswines())
        row[this->player_info_model.swines]
          = row[this->player_info_model.swines] + "H";

      row[this->player_info_model.announcement]
        = ( (player.announcement() == ANNOUNCEMENT::NOANNOUNCEMENT)
           ? ""
           : ::translation(player.announcement()).translation()
          );
      row[this->player_info_model.points]
        = player.points_in_trickpile();
      row[this->player_info_model.tricks]
        = player.number_of_tricks();

    } // player info
    { // team info
      this->team_info_rows[0][this->team_info_model.known_team[p]]
        = ::translation("Team::" + ::short_name(player.team())).translation();
      this->team_info_rows[1][this->team_info_model.known_team[p]]
        = ::translation("Team::" + ::short_name(player.game().teaminfo(player))).translation();

      for (unsigned q = 0; q < game.number_of_players(); ++q) {
        Player const& player2 = player.game().player(q);
        if (dynamic_cast<Ai const*>(&player2)) {
          Ai const& ai2 = static_cast<Ai const&>(player2);
          this->team_info_rows[2 + q][this->team_info_model.known_team[p]]
            = (::translation("Team::" + ::short_name(ai2.teaminfo(player))).translation()
               + (TEAM::is_real(ai2.teaminfo(player))
                  ? string("")
                  : " " + DK::Utils::String::to_string(ai2.team_information().team_value(player))));
        } else {
          this->team_info_rows[2 + q][this->team_info_model.known_team[p]]
            = "";
        }
      } // for (q)
    } // team info
  } // for (p)

  return ;
} // void GameDebug::update_info()

/**
 ** the name of 'player' has changed
 **
 ** @param     player   player whose name has changed
 **
 ** @return    -
 **/
void
GameDebug::name_changed(Player const& player)
{
  if (!this->get_realized())
    return;

  if (::game_status < GAMESTATUS::GAME_INIT)
    return ;
#ifdef TODO
  if (!&this->ui->game())
    return ;
#endif
  if (!this->ui->game().contains(player))
    return ;

  this->team_info_treeview->get_column(1 + player.game().no(player))->set_title(player.name()); 

  this->player_info_rows[player.game().no(player)][this->player_info_model.name]
    = player.name();

  this->team_info_rows[2 + player.game().no(player)][this->team_info_model.name]
    = player.name();

  return ;
} // void GameDebug::name_changed(Player player)

/**
 ** updates the translations
 **
 ** @param     -
 **
 ** @return    -
 **/
void
GameDebug::language_update()
{
  if (!this->get_realized())
    return;

  { // player info
    unsigned column = 0;
    this->player_info_treeview->get_column(column)->set_title(::translation("name").translation());
    column += 1;
    this->player_info_treeview->get_column(column)->set_title(::translation("team").translation());
    column += 1;
    this->player_info_treeview->get_column(column)->set_title(::translation("swines").translation());
    column += 1;
    this->player_info_treeview->get_column(column)->set_title(::translation("announcement").translation());
    column += 1;
    this->player_info_treeview->get_column(column)->set_title(::translation("points").translation());
    column += 1;
    this->player_info_treeview->get_column(column)->set_title(::translation("tricks").translation());
    column += 1;
  } // player info

  { // team info
    this->team_info_treeview->get_column(0)->set_title(::translation("~name").translation()); 

    this->team_info_rows[0][this->team_info_model.name]
      = ::translation("team").translation();
    this->team_info_rows[1][this->team_info_model.name]
      = ::translation("game").translation();
  } // team info

  { // tricks
    unsigned column = 0;
    this->tricks_treeview->get_column(column)->set_title(::translation("card").translation());
    column += 1;
    this->tricks_treeview->get_column(column)->set_title(::translation("player").translation());
    column += 1;
    this->tricks_treeview->get_column(column)->set_title(::translation("trick").translation());
    column += 1;
    this->tricks_treeview->get_column(column)->set_title(::translation("heuristic").translation());
    column += 1;
  } // tricks

  this->update_info();

  return ;
} // void GameDebug::language_update()

/**
 ** a key has been pressed
 **
 ** @param     key   the key
 **
 ** @return    whether the key was managed
 **/
bool
GameDebug::on_key_press_event(GdkEventKey* const key)
{
  return this->ui->key_press(key);
} // bool GameDebug::on_key_press_event(GdkEventKey* key)

} // namespace UI_GTKMM_NS

#endif // #ifdef USE_UI_GTKMM
