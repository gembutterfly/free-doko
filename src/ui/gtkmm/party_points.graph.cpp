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
 **********************************************************************/

#include "constants.h"

#ifdef USE_UI_GTKMM

#include "party_points.graph.h"

#include "ui.h"
#include "translations.h"

#include "../../party/party.h"
#include "../../party/rule.h"
#include "../../game/game.h"

namespace UI_GTKMM_NS {

/**
 ** constructor
 **
 ** @param     party_points   the parent object
 **
 ** @return    -
 **/
PartyPoints::Graph::Graph(PartyPoints* const party_points) :
  Base(party_points),
  Gtk::DrawingArea()
{ }

/**
 ** draw the whole graph
 **
 ** @param     -
 **
 ** @return    -
 **/
void 
PartyPoints::Graph::draw_all()
{
  this->queue_draw();
} // void PartyPoints::Graph::draw_all()

/**
 ** draw the whole graph
 **
 ** @param     -
 **
 ** @return    true
 **
 ** @todo     add game numbers
 **/
bool 
PartyPoints::Graph::on_draw(::Cairo::RefPtr<::Cairo::Context> const& cr)
{
  this->DrawingArea::on_draw(cr);
  auto const width = this->get_width();
  auto const height = this->get_height();

  cr->push_group();

  { // clear the whole area
    cr->save();
    cr->set_source_rgb(1, 1, 1);
    cr->scale(width, height);
    cr->rectangle(0, 0, width, height);
    cr->fill();
    cr->restore();
  } // clear the whole area

  //Party const& party = this->ui->party();
  auto const& party = ::party;

  Counting const counting = party.rule()(Rule::COUNTING);

  // last game to draw + 1
  unsigned const played_gamesno
    = (party.gameno()
       + ( (::game_status < GAMESTATUS::GAME_FINISHED)
          ? 0
          : 1) );

  // calculate the minimum and maximum points made by the players
  int min_points = 0; // minimal points made by the players
  int max_points = 0; // maximal points made by the players

  if (played_gamesno == 0) {
    if (party.rule()(Rule::POINTS_LIMITED))
      max_points = party.rule()(Rule::POINTS) / 2;
    if (max_points == 0)
      max_points = 10;

    switch (party.rule()(Rule::COUNTING)) {
    case COUNTING_NS::PLUS:
      break;
    case COUNTING_NS::MINUS:
      min_points = -max_points;
      max_points = 0;
      break;
    case COUNTING_NS::PLUSMINUS:
      min_points = -max_points;
      break;
    } // switch (party.rule()(Rule::COUNTING))
  } // if (played_gamesno == 0)

  for (unsigned g = 0; g < played_gamesno; ++g) {
    for (unsigned p = 0; p < party.playerno(); ++p) {
      int const points = party.pointsum_till_game(g, p);
      if (points < min_points)
        min_points = points;
      if (points > max_points)
        max_points = points;
    } // for (p < party.playerno())
  } // for (g < played_gamesno)
  if (max_points == min_points)
    max_points += 1;

  // how many games to display
  unsigned gamesno = UINT_MAX;
  if (party.is_last_game()) {
    gamesno = played_gamesno;
    if (::game_status < GAMESTATUS::GAME_FINISHED)
      gamesno += 1;
  } else {
    if (party.rule()(Rule::NUMBER_OF_ROUNDS_LIMITED))
      gamesno = min(gamesno,
                    party.round_startgame(party.rule()(Rule::NUMBER_OF_ROUNDS)));
    if (   party.rule()(Rule::POINTS_LIMITED)
        && (party.points() > 0) )
      gamesno = min(gamesno,
                    max( (  (party.rule()(Rule::POINTS)
                             * (played_gamesno + 1))
                          / static_cast<int>(party.points())),
                        played_gamesno + 1));
    if (gamesno < UINT_MAX) {
      gamesno += party.remaining_duty_soli();
      if (   (::game_status == GAMESTATUS::GAME_FINISHED)
          && party.game().is_duty_solo())
        gamesno += 1;
    }
  } // if (party.is_last_game())

  if (gamesno == UINT_MAX)
    gamesno = played_gamesno;
  if (gamesno < party.round_startgame(1))
    gamesno = party.round_startgame(1);
  // how many rounds to display
  int const roundsno = party.round_of_game(gamesno);



  // layout:
  // +-----------------------------+ border_y
  // | 10+       /                 |
  // |   |     _/                  |
  // |   |   _/  _                 |
  // |  5+  /   /                  | height_graph
  // |   | / __/                   |
  // |   |/ /                      |
  // |  0+----+----+----+----+----+| border_roundno
  // |   0    5   10   15   20   25| height_roundno, border_names
  // |    player1 player2 p3  p4   | height_names
  // +-----------------------------+ border_x
  // border_x width_points border_points width_graph border_x

  // the layout values
  int border_y = 0;
  int height_graph = 0;
  int border_roundno = 0;
  int height_roundno = 0;
  int border_names = 0;
  int height_names = 0;

  int border_x = 0;
  int width_points = 0;
  int border_points = 0;
  int width_graph = 0;

  int zero_x = 0;           // the x position of the zero point of the graph
  int zero_y = 0;           // the y position of the zero point of the graph
  double scale_games = 0;   // the distance beween two games
  double scale_points = 0;  // the distance between two points
  // note: the 'scale_points' distance is negative, so that the points go up
  int points_step = 0; // the steps beween two points markers

  // whether the player names are shown on the bottom (else on the top)
  bool const names_on_bottom = (counting != COUNTING_NS::MINUS);

  // the layout for the name and the description
  auto text_layout = this->create_pango_layout("");
  text_layout->set_font_description(Pango::FontDescription("Sans Serif 10"));

  // size of the markers (in each direction)
  int marker_pixel_size = 10;

  { // marker size
    int layout_width, layout_height;
    text_layout->set_text("0");
    text_layout->get_pixel_size(layout_width, layout_height);
    marker_pixel_size = layout_height / 2;
  } // marker size

  { // set the layout values
    { // y
      border_y = max((height / 50), 2);
      border_names = border_y / 2;
      { // height name
        height_names = 0;
        int layout_width, layout_height;
        for (unsigned p = 0; p < party.playerno(); ++p) {
          text_layout->set_text(party.player(p).name());
          text_layout->get_pixel_size(layout_width, layout_height);
          if (layout_height > height_names)
            height_names = layout_height;
        }
      } // height name
      border_roundno = border_y / 2;
      if (counting == COUNTING_NS::PLUSMINUS) {
        height_roundno = 0;
#ifdef WORKAROUND
        scale_points = 0;
#endif
        // ensure that there is enough place for the roundno below the graph
        if (min_points * scale_points
            < height_names + border_names + marker_pixel_size)
          height_roundno = ((height_names + border_names
                             + marker_pixel_size)
                            - static_cast<int>(min_points * scale_points));
      } else { // if !(counting = PLUSMINUS)
        height_roundno = height_names;
      } // if !(counting = PLUSMINUS)
      if (height < (2 * border_y
                    + height_names + border_names
                    + height_roundno + border_roundno)) {
        return true;
      }
      height_graph = (height - (2 * border_y
                                + height_names + border_names
                                + height_roundno + border_roundno));
      scale_points = - (height_graph
                        / static_cast<double>(max_points - min_points));
      zero_y = (border_y - static_cast<int>(max_points * scale_points));
      if (!names_on_bottom)
        zero_y += (height_names + border_names
                   + height_roundno + border_roundno);
    } // y

    { // x
      border_x = max((width / 50), 2);
      border_points = border_x / 2;
      { // width points
        vector<int> steps = {1, 2, 5};

        int layout_width, layout_height;
        text_layout->set_text("0");
        text_layout->get_pixel_size(layout_width, layout_height);

        auto const min_pixel_distance
          = (static_cast<int>(layout_height) * 3) / 2;
        points_step = 0;

        for (size_t i = 0; i < steps.size(); ++i) {
          auto const s = steps[i];
          if (height_graph / ((max_points - min_points) / s)
              >= min_pixel_distance) {
            points_step = s;
            break;
          }
          steps.push_back(10 * s);
        } // for (i < steps.size())
        if (points_step == 0) {
          return true;
        }

        width_points = 0;
        for (auto p = -((-min_points / points_step) * points_step);
             p <= max_points;
             p += points_step)  {
          text_layout->set_text(std::to_string(p));
          text_layout->get_pixel_size(layout_width, layout_height);
          if (width_points < layout_width)
            width_points = layout_width;
        }
      } // width points
      zero_x = border_x + width_points + border_points;
      if (width < zero_x + border_x) {
        return true;
      }
      width_graph = width - zero_x - border_x;

      scale_games = (width_graph / static_cast<double>(gamesno));
    } // x
  } // set the layout values

  { // draw the curves of the points of the players
    // reorder the players so that the human players are drawn last
    vector<unsigned> players;
    for (unsigned p = 0; p < party.playerno(); ++p)
      if (   (party.player(p).type() != Player::HUMAN)
#ifdef USE_NETWORK
          && (party.player(p).type() != Player::NETWORK)
#endif
         )
        players.push_back(p);
#ifdef USE_NETWORK
    for (unsigned p = 0; p < party.playerno(); ++p)
      if (party.player(p).type() == Player::NETWORK)
        players.push_back(p);
#endif
    for (unsigned p = 0; p < party.playerno(); ++p)
      if (party.player(p).type() == Player::HUMAN)
        players.push_back(p);

    text_layout->set_font_description(Pango::FontDescription("Sans Serif Bold 10"));
    for (auto const p : players) {
      // the y offset for the line of the player
      auto const player_offset_y = ( ((static_cast<int>(p) - 1) * 2 - 1)
                                    / (static_cast<int>(party.roundno())
                                       / 5 + 1) );
      string color = party.player(p).color();
#ifdef WORKAROUND
      // set a default color if the player has none
      if (color == "") {
        static vector<string> colors = {"blue", "red", "green", "gold"};
        if (p < colors.size())
          color = colors[p];
        else
          color = "black";
      } // if (color == "")
#endif
      cr->save();
      {
        auto const c = Gdk::Color(color);
        cr->set_source_rgb(c.get_red_p(), c.get_green_p(), c.get_blue_p());
      }

      // the name
      text_layout->set_text(party.player(p).name());
      int layout_width, layout_height;
      text_layout->get_pixel_size(layout_width, layout_height);
      cr->move_to(zero_x
                  + (width_graph * p / party.playerno())
                  + (width_graph / party.playerno()
                     - layout_width) / 2,
                  ( names_on_bottom
                   ? (height
                      - border_y
                      - height_names)
                   : border_y
                  ));
      text_layout->show_in_cairo_context(cr);

      // the points
      cr->set_line_width(2);
      cr->translate(zero_x, zero_y + player_offset_y);
      cr->move_to(0, 0);
      for (unsigned g = 0; g < played_gamesno; ++g) {
        cr->line_to(scale_games * (g + 1),
                    scale_points * party.pointsum_till_game(g, p));
      } // for (g <= party.gameno())
      cr->stroke();

      cr->restore();
    } // for (p < party.playerno())
  } // draw the curves of the points of the players

  cr->set_line_width(0.5);
  { // zero lines
    cr->save();
    cr->set_source_rgb(0, 0, 0);
    cr->move_to(zero_x + width_graph, zero_y);
    cr->line_to(zero_x, zero_y);
    cr->line_to(zero_x, zero_y + static_cast<int>(scale_points * min_points));
    cr->stroke();
    cr->move_to(zero_x + width_graph, zero_y);
    cr->line_to(zero_x, zero_y);
    cr->line_to(zero_x, zero_y + static_cast<int>(scale_points * max_points));
    cr->stroke();
    cr->restore();
  } // zero lines
  { // round markers
    int round_step = 0;

    auto steps = vector<int>{1, 2, 5};

    int layout_width, layout_height;
    text_layout->set_font_description(Pango::FontDescription("Sans Serif 10"));
    text_layout->set_text(std::to_string(roundsno));
    text_layout->get_pixel_size(layout_width, layout_height);

    // minimal pixel distance between two markers
    auto const min_pixel_distance = 2 * layout_width;

    if (roundsno == 0) {
      round_step = 2 * width_graph;
    } else { // if !(roundsno == 0)
      for (size_t i = 0; i < steps.size(); ++i) {
        auto const s = steps[i];
        if (width_graph / (roundsno / s) >= min_pixel_distance) {
          round_step = s;
          break;
        }
        steps.push_back(10 * s);
      } // for (i < steps.size())
    } // if !(roundsno == 0)
    if (round_step == 0) {
      return true;
    }

    // how many pixel the marker goes up/down
    int const marker_y_up = ( (counting == COUNTING_NS::MINUS)
                             ? 0
                             : marker_pixel_size);

    int const marker_y_down = ( (counting == COUNTING_NS::PLUS)
                               ? 0
                               : marker_pixel_size);

    for (int r = 0; r <= roundsno; r += round_step) {
      int const x = (zero_x
                     + static_cast<int>(scale_games
                                        * party.round_startgame(r)));
      cr->move_to(x, zero_y - marker_y_down);
      cr->rel_line_to(0, marker_y_up + marker_y_down);
      cr->stroke();
      int layout_width, layout_height;
      text_layout->set_text(std::to_string(r));
      text_layout->get_pixel_size(layout_width, layout_height);
      if (counting == COUNTING_NS::MINUS) {
        cr->move_to(zero_x
                    + static_cast<int>(scale_games
                                       * party.round_startgame(r))
                    - (layout_width / 2),
                    zero_y - marker_y_up - border_roundno
                    - layout_height);
      } else { // if (counting == COUNTING_NS::MINUS)
        cr->move_to(zero_x
                    + static_cast<int>(scale_games
                                       * party.round_startgame(r))
                    - (layout_width / 2),
                    zero_y + marker_y_down + border_roundno);
      } // if (counting == COUNTING_NS::MINUS)
      text_layout->show_in_cairo_context(cr);
    }
  } // round markers
  { // point markers
    int layout_width, layout_height;
    for (int p = -((-min_points / points_step) * points_step);
         p <= max_points;
         p += points_step)  {
      text_layout->set_text(std::to_string(p));
      text_layout->get_pixel_size(layout_width, layout_height);
      cr->move_to(border_x + width_points
                  - layout_width,
                  zero_y + static_cast<int>(scale_points * p)
                  - layout_height / 2);
      text_layout->show_in_cairo_context(cr);
      cr->move_to(zero_x,
                  zero_y + static_cast<int>(scale_points * p));
      cr->rel_line_to(marker_pixel_size, 0);
      cr->stroke();
    }
  } // point markers
  cr->pop_group_to_source();
  cr->paint();

  return true;
} // bool PartyPoints::Graph::on_draw(::Cairo::RefPtr<::Cairo::Context> cr)

} // namespace UI_GTKMM_NS

#endif // #ifdef USE_UI_GTKMM
