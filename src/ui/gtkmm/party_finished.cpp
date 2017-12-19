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

#include "party_finished.h"

#include "ui.h"
#include "translations.h"
#include "table.h"
#include "main_window.h"
#include "party_points.h"
#include "cards.h"

#include "widgets/stock_button.h"

#include "../../party/party.h"
#include "../../party/rule.h"
#include "../../misc/setting.h"

#include "../../utils/string.h"

#include <gtkmm/main.h>
#include <gtkmm/drawingarea.h>
#include <gtkmm/alignment.h>
#include <gtkmm/stock.h>

namespace UI_GTKMM_NS {

/**
 ** constructor
 **
 ** @param     parent   the parent widget
 **
 ** @return    -
 **/
PartyFinished::PartyFinished(Base* const parent) :
  Base(parent),
  StickyDialog("party finished", *parent->ui->main_window, false)
{
  this->ui->add_window(*this);

  { // set size
    Gdk::Geometry geometry;
    geometry.min_width = 6 * this->ui->cards->width();
    geometry.min_height = 3 * this->ui->cards->height();

    this->set_geometry_hints(*this, geometry, Gdk::HINT_MIN_SIZE);
  }

  this->signal_realize().connect(sigc::mem_fun(*this, &PartyFinished::init));
#ifndef GLIBMM_DEFAULT_SIGNAL_HANDLERS_ENABLED
  this->signal_show().connect(sigc::mem_fun(*this, &PartyFinished::on_show));
  this->signal_hide().connect(sigc::mem_fun(*this, &PartyFinished::on_hide));
#endif

  return ;
} // PartyFinished::PartyFinished(Base* parent)

/**
 ** destruktor
 **
 ** @param     -
 **
 ** @return    -
 **/
PartyFinished::~PartyFinished()
{ }

/**
 ** create all subelements
 **
 ** @param     -
 **
 ** @return    -
 **/
void
PartyFinished::init()
{
  this->ui->translations->add(*this, ::translation("~party finished"));

  this->set_icon(this->ui->icon);

  this->set_position(Gtk::WIN_POS_CENTER_ON_PARENT);

  this->get_vbox()->set_spacing(1 EX);

  this->points_graph = Gtk::manage(new Gtk::DrawingArea());
  this->get_vbox()->pack_start(*this->points_graph, true, true);
  {
    auto alignment
      = Gtk::manage(new Gtk::Alignment(Gtk::ALIGN_CENTER,
                                       Gtk::ALIGN_CENTER,
                                       0, 0));
    this->show_party_points
      = Gtk::manage(new Gtk::StockButton("show party points"));
    this->ui->translations->add(*this->show_party_points->label,
                                ::translation("show party points"));
    alignment->add(*this->show_party_points);
    this->get_vbox()->pack_start(*alignment, false, true);
    this->show_party_points->signal_clicked().connect(sigc::mem_fun0(*this->ui->table->party_points_, &Gtk::Window::present));
  }

  { // action area
    this->close_party_button
      = Gtk::manage(new Gtk::StockButton(Gtk::Stock::OK, "finish party"));
    this->ui->translations->add(*this->close_party_button->label,
                                ::translation("finish party"));
    this->add_action_widget(*this->close_party_button, Gtk::RESPONSE_CLOSE);

    this->close_party_button->grab_default();
    this->close_party_button->grab_focus();
  } // action area

  this->show_all_children();

  // signals
  this->close_party_button->signal_clicked().connect(sigc::mem_fun(*this, &PartyFinished::hide));
  this->points_graph->signal_draw().connect(sigc::mem_fun(*this, &PartyFinished::on_draw_graph));

  return ;
} // void PartyFinished::init()

/**
 ** the party has finished:
 ** show the summary and the winner
 **
 ** @param     -
 **
 ** @return    -
 **/
void
PartyFinished::party_finished()
{
  this->present();

  while (!this->ui->thrower
         && this->is_visible())
    ::ui->wait();
  if (this->ui->thrower)
    return ;

  this->hide();

  return ;
} // void PartyFinished::party_finished()

/**
 ** redraw the whole points graph
 **
 ** @param    -
 **
 ** @return    -
 **
 ** @todo      mark the winner(s)
 **/
void
PartyFinished::redraw_points_graph()
{
  /*
   *     Mara    Gerd    Erika   Sven
   *                           _________
   *   _________               |       |
   *   |       |________       |       |
   *   |       |       |       |       |
   *   |  75   |  50   |  -50  |  100  |
   *                   |       |       |
   *                   |_______|
   */

  if (!this->get_realized())
    return ;

  auto cr = this->points_graph->get_window()->create_cairo_context();

  auto const width = this->points_graph->get_width();
  auto const height = this->points_graph->get_height();

  cr->set_source_rgb(1, 1, 1);
  cr->rectangle(0, 0, width, height);
  cr->fill();

  int min_points = INT_MAX; // minimal points made by the players
  int max_points = INT_MIN; // maximal points made by the players

  Party const& party = this->ui->party();

  for (auto p : party.players()) {
    int const points = party.pointsum(*p);
    if (points < min_points)
      min_points = points;
    if (points > max_points)
      max_points = points;
  } // for (p : party.players())
  if (max_points == min_points)
    max_points += 1;

  int max_layout_height = 0;
  {
  auto name_layout = this->create_pango_layout("");
    name_layout->set_font_description(Pango::FontDescription("Sans Serif Bold 10"));
  for (unsigned p = 0; p < party.playerno(); ++p) {
    name_layout->set_text(party.player(p).name());
    int layout_width, layout_height;
    name_layout->get_pixel_size(layout_width, layout_height);
    if (layout_height > max_layout_height)
      max_layout_height = layout_height;
  } // for (p < party.playerno())
  }

  int const points_difference = (max(max_points, 0)
                                 - min(min_points, 0));
  int const border_left = this->ui->cards->width() / 5;
  int const border_right = border_left;
  int const border_top = (2 * max_layout_height
                          + ((max_points >= 0)
                             ? max_layout_height
                             : 0));
  int const border_bottom = (max_layout_height / 2
                             + ((min_points < 0)
                                ? max_layout_height
                                : 0));
  int const total_width = width - border_left - border_right;
  int const total_height = height - border_top - border_bottom;

  int const baseline = (border_top
                        + (max(max_points, 0)
                           * total_height
                           / points_difference));
  int const single_width = total_width / party.playerno();
  int const distance = 4; // for the distance between the columns
  { // draw all
    Gdk::Color color_name(::setting(Setting::NAME_FONT_COLOR));
    Gdk::Color color_winner_name(::setting(Setting::NAME_ACTIVE_FONT_COLOR));
    for (unsigned p = 0; p < party.playerno(); ++p) {
      Player const& player = party.player(p);
      bool winner = (party.rang(player) == 0);
      int const points = party.pointsum(player);
      int const pos_x = border_left + p * single_width;
      int height = (points * total_height / points_difference);
      (void) height;
      (void) distance;
      (void) baseline;

      if (winner)
        cr->set_source_rgb(color_winner_name.get_red_p(), color_winner_name.get_green_p(), color_winner_name.get_blue_p());
      else
        cr->set_source_rgb(color_name.get_red_p(), color_name.get_green_p(), color_name.get_blue_p());
      { // draw name
        auto name_layout = this->create_pango_layout("");
    name_layout->set_font_description(Pango::FontDescription("Sans Serif Bold 10"));
        name_layout->set_text(player.name());
        int layout_width, layout_height;
        name_layout->get_pixel_size(layout_width, layout_height);
        // bug: there seems to be a problem with italic characters
        layout_width += 2 * layout_height / 5;
        cr->move_to(pos_x + (single_width - layout_width) / 2,
                    max_layout_height / 2);
        name_layout->show_in_cairo_context(cr);
      } // draw name
      { // draw points
        auto points_layout = this->create_pango_layout("");
    points_layout->set_font_description(Pango::FontDescription("Sans Serif 10"));
        points_layout->set_text(std::to_string(points));
        int layout_width, layout_height;
        points_layout->get_pixel_size(layout_width, layout_height);
        // bug: there seems to be a problem with italic characters
        layout_width += 2 * layout_height / 5;

        cr->move_to(pos_x + (single_width - layout_width) / 2,
                    baseline - height
                    - ((height >= 0)
                       ? layout_height
                       : 0));
        points_layout->show_in_cairo_context(cr);
      } // draw points
      { // draw point column
        cr->rectangle(pos_x + single_width / distance,
                      baseline,
                      single_width - 2 * single_width / distance,
                      1);
        cr->fill();
        if (height >= 0)
          cr->rectangle(pos_x + single_width / distance,
                        baseline - height,
                        single_width - 2 * single_width / distance,
                        height);
        else
          cr->rectangle(pos_x + single_width / distance,
                        baseline,
                        single_width - 2 * single_width / distance,
                        -height);
        cr->fill();
      } // draw point column
    } // for (p)

    if (party.rule()(Rule::COUNTING) == COUNTING_NS::PLUSMINUS) {
      // draw baseline
      cr->set_source_rgb(color_name.get_red_p(), color_name.get_green_p(), color_name.get_blue_p());
      cr->rectangle(border_left + single_width / distance,
                    baseline,
                    total_width - 2 * (single_width / distance),
                    1);
      cr->fill();
    } // if (draw baseline)
  } // draw all

  return ;
} // void PartyFinished::redraw_points_graph()

/**
 ** the name of 'player' has changed
 **
 ** @param     player   the player with the changed name
 **
 ** @return    -
 **/
void
PartyFinished::name_changed(::Player const& player)
{
  if (!this->get_realized())
    return ;

  this->redraw_points_graph();

  return ;
} // void PartyFinished::name_changed(Player player)

/**
 ** event: hide the window
 **
 ** @param     -
 **
 ** @return    -
 **/
void
PartyFinished::on_hide()
{
  this->ui->main_quit();
#ifdef GLIBMM_DEFAULT_SIGNAL_HANDLERS_ENABLED
  this->Gtk::Dialog::on_hide();
#endif

  return ;
} // void PartyFinished::on_hide()

/**
 ** event: draw the graph
 **
 ** @param     cr   the graphic context
 **
 ** @return    true
 **/
bool
PartyFinished::on_draw_graph(::Cairo::RefPtr<::Cairo::Context> const& cr)
{
  this->redraw_points_graph();

  return true;
} // bool PartyFinished::on_draw_graph(::Cairo::RefPtr<::Cairo::Context> c)

} // namespace UI_GTKMM_NS

#endif // #ifdef USE_UI_GTKMM
