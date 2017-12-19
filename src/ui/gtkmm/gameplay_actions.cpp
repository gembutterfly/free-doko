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

#include "gameplay_actions.h"

#include "ui.h"
#include "translations.h"

#include "../../party/party.h"

#include "../../utils/string.h"

#include <gtkmm/treeview.h>

namespace UI_GTKMM_NS {

/**
 ** constructor
 **
 ** @param     parent   parent object
 **
 ** @return    -
 **/
GameplayActions::GameplayActions(Base* const parent) :
  Base(parent),
  Gtk::TreeView()
{
  this->init();
  this->signal_realize().connect(sigc::mem_fun(*this, &GameplayActions::language_update));

  return ;
} // GameplayActions::GameplayActions(Base* parent)

/**
 ** destruktor
 **
 ** @param     -
 **
 ** @return    -
 **/
GameplayActions::~GameplayActions()
{ }

/**
 ** create all subelements
 **
 ** @param     -
 **
 ** @return    -
 **/
void
GameplayActions::init()
{
  this->list = Gtk::ListStore::create(this->model);
  this->set_model(this->list);

  this->get_selection()->set_mode(Gtk::SELECTION_NONE);

  this->append_column("type", this->model.type);
  this->append_column("player", this->model.player);
  this->get_column_cell_renderer(1)->set_property("xalign", 0.5);
  this->append_column("data", this->model.data);

  for (auto c : this->get_columns()) {
    c->set_cell_data_func(*c->get_first_cell(),
                          sigc::mem_fun(*this, &GameplayActions::set_cell_color));
  }
  this->show_all_children();

  this->update();
  this->language_update();

  return ;
} // void GameplayActions::init()

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
GameplayActions::update()
{
  if (!this->get_realized())
    return ;

  this->list->clear();

  if (this->actions) {
    unsigned trickno = 0;
    for (unsigned a = 0; a < this->actions->size(); ++a) {
      auto const& action = *((*this->actions)[a]);
      auto row = *this->list->append();
      row[this->model.type] = ::translation(action.type).translation();
      if (action.player == UINT_MAX)
        row[this->model.player] = "";
      else
#ifdef WORKAROUND
        // in 'party_open()' 'BugReportReplay' first gets the party and then ui
        row[this->model.player]
          = ::party.player(action.player).name();
#else
      row[this->model.player]
        = this->ui->party().player(action.player).name();
#endif
      if (action.type == GameplayAction::TRICK_FULL) {
        trickno += 1;
        row[this->model.data]
          = (DK::Utils::String::to_string(trickno) + ": " 
             + action.data_translation().translation());
      } else {
        row[this->model.data] = action.data_translation().translation();
      }
      row[this->model.action] = &action;
      row[this->model.no] = a;

    } // for (a < this->actsion->size())
  } // if (this->actions)

  return ;
} // void GameplayActions::update()

/**
 ** sets the actions
 **
 ** @param     actions   actions to display
 **
 ** @return    -
 **/
void
GameplayActions::set_actions(vector< ::GameplayAction const*> const& actions)
{
  this->actions = &actions;
  this->update();
  return ;
} // void GameplayActions::set_actions(vector< ::GameplayAction const*> actions)

/**
 ** sets the discrepancy
 **
 ** @param     discrepancies   discrepancies of the actions to the gameplay
 **
 ** @return    -
 **/
void
GameplayActions::set_discrepancies(vector< ::GameplayAction::Discrepancy>
                                   const& discrepancies)
{
  this->discrepancies = &discrepancies;
  this->update();
  return ;
} // void GameplayActions::set_discrepancies(vector<GameplayAction::Discrepancy> discrepancies)

/**
 ** removes the actions
 **
 ** @param     -
 **
 ** @return    -
 **/
void
GameplayActions::remove_actions()
{
  this->actions = nullptr;
  this->discrepancies = nullptr;
  this->update();
  return ;
} // void GameplayActions::remove_actions()

/**
 ** sets the number of the current action
 **
 ** @param     current_action_no   new number
 **
 ** @return    -
 **/
void
GameplayActions::set_current_action_no(unsigned const current_action_no)
{
  this->current_action_no = current_action_no;

  if (!this->get_realized())
    return ;

  // the redrawing is needed, so that the colors are updated
  this->queue_draw();

  // just scroll to the current row (I do not know a simplier way)
  if (this->current_action_no < this->get_model()->children().size())
    this->scroll_to_row(this->get_model()->get_path(*(this->get_model()->get_iter(DK::Utils::String::to_string(current_action_no)))));

  return ;
} // void GameplayActions::set_current_action_no(unsigned current_action_no)

/**
 ** the name of 'player' has changed
 **
 ** @param     player   the player with the changed name
 **
 ** @return    -
 **/
void
GameplayActions::name_changed(Player const& player)
{
  if (!this->get_realized())
    return ;

  this->update();

  return ;
} // void GameplayActions::name_changed(Player player)

/**
 ** updates all language elements
 **
 ** @param     -
 **
 ** @return    -
 **/
void
GameplayActions::language_update()
{
  if (!this->get_realized())
    return ;

  this->get_column(0)->set_title(::translation("type").translation());
  this->get_column(1)->set_title(::translation("player").translation());
  this->get_column(2)->set_title(::translation("data").translation());

  this->update();

  return ;
} // void GameplayActions::language_update()

/**
 ** changes the color of the cell at 'iterator'
 **
 ** @param     cell_renderer   cell renderer to change
 ** @param     iterator   row
 **
 ** @return    -
 **/
void
GameplayActions::set_cell_color(Gtk::CellRenderer* const cell_renderer,
                                Gtk::TreeModel::iterator const& iterator)
{
  auto const row = *iterator;
  GameplayAction const* const action = row[this->model.action];
  auto const action_no = row[this->model.no];

  pair<string, string> colors;
  if (this->discrepancies) {
    if (row[this->model.no] < this->discrepancies->size())
      colors = GameplayActions::colors(action->type,
                                       (*this->discrepancies)[action_no]);
    else
      colors = GameplayActions::colors(action->type,
                                       ::GameplayAction::FUTURE);

  } else { // if !(this->discrepancies)
    colors = GameplayActions::colors(action->type,
                                     ::GameplayAction::NONE);
  } // if !(this->discrepancies)

  cell_renderer->set_property("foreground", colors.first);
  cell_renderer->set_property("background", colors.second);
  cell_renderer->set_property("cell_background", colors.second);

  return ;
} // void GameplayActions::set_cell_color(Gtk::CellRenderer* cell_renderer, Gtk::TreeModel::iterator iterator)

/**
 ** -> result
 **
 ** @param     action_type   the action type
 ** @param     discrepancy   the discrepancy to the action
 **
 ** @return    pair of colors (foreground, background)
 **/
pair<string, string>
GameplayActions::colors(::GameplayAction::Type const action_type,
                        ::GameplayAction::Discrepancy const discrepancy)
{
  switch (action_type) {
  case ::GameplayAction::POVERTY_SHIFT:
  case ::GameplayAction::POVERTY_ACCEPTED:
  case ::GameplayAction::POVERTY_RETURNED:
  case ::GameplayAction::POVERTY_DENIED:
  case ::GameplayAction::POVERTY_DENIED_BY_ALL:
  case ::GameplayAction::ANNOUNCEMENT:
  case ::GameplayAction::SWINES:
  case ::GameplayAction::HYPERSWINES:
  case ::GameplayAction::MARRIAGE:
  case ::GameplayAction::GENSCHER:
    switch (discrepancy) {
    case ::GameplayAction::FUTURE:
      return std::make_pair("black", "white");
    case ::GameplayAction::NONE:
      return std::make_pair("black", "lightgreen");
    case ::GameplayAction::SKIPPED:
    case ::GameplayAction::PLAYER:
    case ::GameplayAction::CARD:
    case ::GameplayAction::OTHER:
      return std::make_pair("black", "red");
    } // switch (discrepancy)
    break;

  case ::GameplayAction::RESERVATION:
  case ::GameplayAction::TRICK_OPEN:
  case ::GameplayAction::TRICK_FULL:
  case ::GameplayAction::TRICK_TAKEN:
    switch (discrepancy) {
    case ::GameplayAction::FUTURE:
      return std::make_pair("black", "lightgray");
    case ::GameplayAction::NONE:
      return std::make_pair("white", "darkgreen");
    case ::GameplayAction::SKIPPED:
    case ::GameplayAction::PLAYER:
    case ::GameplayAction::CARD:
    case ::GameplayAction::OTHER:
      return std::make_pair("white", "darkred");
    } // switch (discrepancy)
    break;

  case ::GameplayAction::CARD_PLAYED:
    switch (discrepancy) {
    case ::GameplayAction::FUTURE:
      return std::make_pair("black", "white");
    case ::GameplayAction::NONE:
      return std::make_pair("black", "green");
    case ::GameplayAction::SKIPPED:
    case ::GameplayAction::PLAYER:
    case ::GameplayAction::CARD:
    case ::GameplayAction::OTHER:
      return std::make_pair("black", "red");
    } // switch (discrepancy)
    break;

  case ::GameplayAction::CHECK:
    return std::make_pair("red", "blue");
  case ::GameplayAction::PRINT:
    return std::make_pair("black", "blue");
  }; // switch (action_type)

  return std::make_pair("black", "white");
} // static pair<string, string> GameplayActions::colors(GameplayAction::Type action_type, GameplayAction::Discrepancy discrepancy)

} // namespace UI_GTKMM_NS

#endif // #ifdef USE_UI_GTKMM
