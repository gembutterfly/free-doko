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

#include "trickpile.h"
#include "table.h"
#include "hand.h"

#include "ui.h"
#include "cards.h"

#include "../../player/player.h"
#include "../../game/game.h"
#include "../../game/game.h"
#include "../../party/rule.h"
#include "../../card/trick.h"
#include "../../misc/setting.h"

#include "../../utils/string.h"

namespace UI_GTKMM_NS {

#define TRICKPILE_TRICKS_STEP_BIG_PART \
static_cast<int>(this->ui->cards->height() / 15.0)
#define TRICKPILE_TRICKS_STEP_SMALL_PART \
static_cast<int>(this->ui->cards->width() / 30.0)
#define TRICKPILE_CARDS_STEP_BIG_PART \
static_cast<int>(this->ui->cards->height() * 0)
#define TRICKPILE_CARDS_STEP_SMALL_PART \
static_cast<int>(this->ui->cards->width() / 20.0)

/**
 ** Constructor
 **
 ** @param     table   the table
 ** @param     position   the position of the trickpile
 **
 ** @return    -
 **/
TrickPile::TrickPile(Table* const table, Position const position) :
  HTIN(table, position)
{ }

/**
 ** draws the trickpile
 **
 ** @param     cr       cairo context
 ** @param     update   whether to update the area
 **
 ** @return    -
 **/
void
TrickPile::draw(Cairo::RefPtr<::Cairo::Context> cr, bool const update)
{
  if (!(::game_status & GAMESTATUS::GAME))
    return ;

#ifdef WINDOWS
#ifdef WORKAROUND
  // Der aktuelle Fensterinhalt wird nicht uebertragen
  if (update) {
	  this->table().queue_resize();
    return ;
  }
#endif
#endif

  if (!this->points_layout)
    this->points_layout = Pango::Layout::create(cr);

  if (!(   (::game_status >= GAMESTATUS::GAME_INIT)
        && (::game_status <= GAMESTATUS::GAME_FINISHED)))
    return ;

  auto pos_x = this->pos_x();
  auto pos_y = this->pos_y();
  auto width = this->width();
  auto height = this->height();

  // for the specialpoints update the values
  // I hope, there cannot be more specialpoints than cards, since that
  // is not tested here.
  switch(this->position()) {
  case SOUTH:
    pos_y += this->ui->cards->back().get_width();
    pos_y -= this->ui->cards->back().get_height();
    height -= this->ui->cards->back().get_width();
    height += this->ui->cards->back().get_height();
    break;
  case WEST:
    width -= this->ui->cards->back().get_width();
    width += this->ui->cards->back().get_height();
    break;
  case NORTH:
    height -= this->ui->cards->back().get_width();
    height += this->ui->cards->back().get_height();
    break;
  case EAST:
    pos_x += this->ui->cards->back().get_width();
    pos_x -= this->ui->cards->back().get_height();
    width -= this->ui->cards->back().get_width();
    width += this->ui->cards->back().get_height();
    break;
  default:
    DEBUG_ASSERTION(false,
                    "TrickPile::draw_tricks():\n"
                    "  position not valid: "
                    << this->position());
    break;
  } // switch(this->player().no())

  cr->save();
  cr->push_group();
  if (update)
    this->table().clear(cr, Gdk::Rectangle(pos_x, pos_y, width, height));

  // draw the cards
  this->draw_tricks(cr);
  this->draw_points(cr);
  cr->pop_group_to_source();
  cr->paint();
  cr->restore();

return ;
} // void TrickPile::draw(Cairo::RefPtr<::Cairo::Context> cr, bool update = true)

/**
 ** draws the tricks in the trickpile
 **
 ** @param     cr    cairo context
 **
 ** @return    -
 **
 ** @todo	skip the cards, the human player(s) do know not to make a special points (because of further team informations)
 **/
void
TrickPile::draw_tricks(Cairo::RefPtr<::Cairo::Context> cr)
{
  if (!(::game_status & GAMESTATUS::GAME))
    return ;

  if (   (::game_status == GAMESTATUS::GAME_FINISHED)
      && !this->table().in_game_review())
    return ;

  cr->save();
  cr->push_group();
  unsigned trickno = this->player().number_of_tricks();

  if (this->table().in_game_review()) {
    trickno = 0;
    for (auto t : this->player().trickpile()) {
      if (t->no() + 1 < this->table().game_review_trickno())
        trickno += 1;
      else
        break;
    } // for (auto t : this->player.trickpile())
  } // if (this->table().in_game_review())

  for (unsigned t = 0; t < trickno; t++) {
    auto cards = this->player().trickpile()[t]->specialpoints_cards(); 
    auto const specialpoints_no = cards.size();
    // ToDo: skip the cards, the human player(s) do know not to make a special points (because of further team informations)

    if (this->game().rule()(Rule::SHOW_TRICKS_IN_TRICKPILES)) {
      while (cards.size() < this->game().number_of_players())
        cards.push_back(HandCard());
    } else {
      cards.push_back(HandCard());
    }
    for (unsigned c = 0; c < cards.size(); ++c) {
      auto const& card = cards[c];
      auto rotation = this->pile_rotation();
      int x = (this->start_x()
               + this->tricks_step_x() * t
               + this->cards_step_x() * c);
      int y = (this->start_y()
               + this->tricks_step_y() * t
               + this->cards_step_y() * c);
      if (!card.is_empty()) {
        switch (this->position()) {
        case Position::SOUTH:
          y += this->ui->cards->back(rotation).get_height();
          rotation = counterclockwise(rotation);
          y -= this->ui->cards->card(card, rotation).get_height();
          x -= this->cards_step_y() * c;
          break;
        case Position::WEST:
          rotation = clockwise(rotation);
          y += this->cards_step_x() * c;
          break;
        case Position::EAST:
          x += this->ui->cards->back(rotation).get_width();
          rotation = counterclockwise(rotation);
          x -= this->ui->cards->card(card, rotation).get_width();
          x -= this->cards_step_y() * c;
          break;
        case Position::NORTH:
          x += this->ui->cards->back(rotation).get_width();
          rotation = clockwise(rotation);
          x -= this->ui->cards->card(card, rotation).get_width();
          y -= this->cards_step_x() * c;
          break;
        case Position::CENTER:
          DEBUG_ASSERTION(false,
                          "TrickPile::draw_tricks()\n"
                          "  wrong position 'CENTER' "
                          << "(" << this->position() << ")");
          break;
        } // switch (this->position())
      } else { // if !(card)
        x -= (specialpoints_no * this->cards_step_x());
        y -= (specialpoints_no * this->cards_step_y());
      } // if !(card)
      this->draw(cr, this->ui->cards->card(card, rotation), x, y);
    } // for (c < cards.size())
  } // for (t < this->player().number_of_tricks())
  cr->pop_group_to_source();
  cr->paint();
  cr->restore();

  return ;
} // void TrickPile::draw_tricks(Cairo::RefPtr<::Cairo::Context> cr))

/**
 ** draws the points in the trickpile
 **
 ** @param     cr       cairo context
 ** @param     update   whether to update the area (default: false)
 **
 ** @return    -
 **/
void
TrickPile::draw_points(Cairo::RefPtr<::Cairo::Context> cr, bool const update)
{
  if (!(::game_status & GAMESTATUS::GAME))
    return ;
  if (!(   (this->game().rule()(Rule::SHOW_TRICKPILES_POINTS)
            && ::setting(Setting::SHOW_TRICKPILES_POINTS))
        || (::game_status == GAMESTATUS::GAME_FINISHED)))
    return ;

#ifdef WINDOWS
#ifdef WORKAROUND
  // Der aktuelle Fensterinhalt wird nicht uebertragen
  if (update) {
	  this->table().queue_resize();
    return ;
  }
#endif
#endif

  auto number_of_tricks = this->player().number_of_tricks();
  this->points_layout->set_text(DK::Utils::String::to_string(this->player().points_in_trickpile()));

  if (::game_status == GAMESTATUS::GAME_FINISHED) {
    auto points = this->player().points_in_trickpile();
    if (this->table().in_game_review()) {
      number_of_tricks = 0;
      points = 0;

      for (auto t : this->player().trickpile()) {
        if (t->no() + 1 < this->table().game_review_trickno()) {
          points += t->points();
          number_of_tricks += 1;
        } else {
          break;
        }
      } // for (auto t : this->player().trickpile())
    } // if (this->table().in_game_review())

    this->points_layout->set_text(DK::Utils::String::to_string(points));
  } // if (::game_status == GAMESTATUS::GAME_FINISHED)

  if (number_of_tricks == 0)
    return ;

  auto number_of_cards
    = (  this->game().rule()(Rule::SHOW_TRICKS_IN_TRICKPILES)
       ? (this->game().number_of_players()
          - this->player().trickpile().back()->specialpoints_cards().size())
       : 1);
  if (number_of_cards < 1)
    number_of_cards = 1;

  int layout_width, layout_height;
  this->points_layout->get_pixel_size(layout_width, layout_height);
  int x = (this->start_x()
           + (this->tricks_step_x()
              * (number_of_tricks - 1))
           + (this->cards_step_x()
              * (number_of_cards - 1))
           + this->ui->cards->width(this->pile_rotation()) / 2
           - (layout_width / 2)
          );
  int y = (this->start_y()
           + (this->tricks_step_y()
              * (number_of_tricks - 1))
           + (this->cards_step_y()
              * (number_of_cards - 1))
           + this->ui->cards->height(this->pile_rotation()) / 2
           - (layout_height / 2)
          );

  Gdk::Color color;
  if (   (::game_status == GAMESTATUS::GAME_FINISHED)
      && !this->table().in_game_review()
     ) {
    if (   (this->player().team()
            == this->game().winnerteam())
        || (   (this->game().winnerteam() == TEAM::NOTEAM)
            && (this->player().team() == TEAM::RE)) )
      color = Gdk::Color(::setting(Setting::NAME_FONT_COLOR));
    else
      color = Gdk::Color(::setting(Setting::NAME_ACTIVE_FONT_COLOR));
    switch (this->position()) {
    case Position::SOUTH:
      x = (this->hand().start_x()
           + this->hand().width()
           + this->hand().margin_x());
      y = (this->hand().start_y()
           + this->hand().height() / 2
           - layout_height / 2);
      break;
    case Position::NORTH:
      x = (this->hand().start_x()
           + this->hand().width()
           + this->hand().margin_x());
      y = (this->hand().start_y()
           + this->hand().height() / 2
           - layout_height / 2);
      break;
    case Position::WEST:
      x = (this->hand().start_x()
           + this->hand().width() / 2
           - layout_width / 2);
      y = (this->hand().start_y()
           + this->hand().height()
           + this->hand().margin_y());
      break;
    case Position::EAST:
      x = (this->hand().start_x()
           + this->hand().width() / 2
           - layout_width / 2);
      y = (this->hand().start_y()
           - this->hand().margin_y()
           - layout_height);
      break;
    case Position::CENTER:
      DEBUG_ASSERTION(false,
                      "Trick::draw_points()\n"
                      "  wrong position 'CENTER' "
                      << "(" << this->position() << ")");
      break;
    } // switch (this->position())
#if 0
    Gdk::Rectangle r(x - layout_width / 2,
                     y - layout_height / 2,
                     2 * layout_width, 2 * layout_height);
    gc->set_clip_rectangle(r);
#endif
  } else {
    color = Gdk::Color(::setting(Setting::TRICKPILE_POINTS_FONT_COLOR));
  } // if ()
  points_layout->set_font_description(Pango::FontDescription(::setting(Setting::TRICKPILE_POINTS_FONT)));

  cr->save();
  cr->push_group();
  cr->move_to(x, y);
  cr->set_source_rgb(color.get_red_p(), color.get_green_p(), color.get_blue_p());
  points_layout->show_in_cairo_context(cr);
  cr->pop_group_to_source();
  cr->paint();
  cr->restore();

  return ;
} // void TrickPile::draw_points(Cairo::RefPtr<::Cairo::Context> cr, bool update = false)

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    rotation of the cards in the pile
 **/
Rotation
TrickPile::pile_rotation() const
{
  switch(this->position()) {
  case Position::NORTH:
    return Rotation::LEFT;
  case Position::SOUTH:
    return Rotation::RIGHT;
  case Position::WEST:
    return Rotation::UP;
  case Position::EAST:
    return Rotation::DOWN;
  case Position::CENTER:
    DEBUG_ASSERTION(false,
                    "TrickPile::pile_rotation()\n"
                    "  wrong position 'CENTER' "
                    << "(" << this->position() << ")");
    break;
  } // switch(this->position())

  return Rotation::UP;
} // Rotation TrickPile::pile_rotation() const

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    starting x position of the trickpile
 **/
int
TrickPile::pos_x() const
{
  auto value = this->start_x();

  switch(this->position()) {
  case Position::NORTH:
  case Position::EAST:
    value -= (this->width()
              - this->ui->cards->width(this->pile_rotation()));
    break;
  case Position::SOUTH:
  case Position::WEST:
    break;
  case Position::CENTER:
    DEBUG_ASSERTION(false,
                    "TrickPile::pos_x()\n"
                    "  wrong position 'CENTER' "
                    << "(" << this->position() << ")");
    break;
  } // switch(position)

  return value;
} // int TrickPile::pos_x()

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    starting y position of the trickpile
 **/
int
TrickPile::pos_y() const
{
  auto value = this->start_y();

  switch(this->position()) {
  case Position::NORTH:
  case Position::WEST:
    break;
  case Position::SOUTH:
  case Position::EAST:
    value -= (this->height()
              - this->ui->cards->height(this->pile_rotation()));
    break;
  case Position::CENTER:
    DEBUG_ASSERTION(false,
                    "TrickPile::pos_y()\n"
                    "  wrong position 'CENTER' "
                    << "(" << this->position() << ")");
    break;
  } // switch(position)

  return value;
} // int TrickPile::pos_y()

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    width of the trickpile
 **/
int
TrickPile::width() const
{
  return (this->ui->cards->width(this->pile_rotation())
          + ((this->player().number_of_tricks() - 1)
             * std::abs(this->tricks_step_x()))
          + ((this->player().game().number_of_players() - 1)
             * std::abs(this->cards_step_x()))
         );
} // int TrickPile::width()

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    height of the trickpile
 **/
int
TrickPile::height() const
{
  return (this->ui->cards->height(this->pile_rotation())
          + ((this->player().number_of_tricks() - 1)
             * std::abs(this->tricks_step_y()))
          + ((this->player().game().number_of_players() - 1)
             * std::abs(this->cards_step_y()))
         );
} // int TrickPile::height()

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    starting x position of the trickpile
 **/
int
TrickPile::start_x() const
{
  switch(this->position()) {
  case Position::NORTH:
    return (this->hand().start_x()
            - this->ui->cards->width(this->pile_rotation())
            - this->hand().margin_x());
  case Position::SOUTH:
    return (this->hand().start_x()
            + this->hand().width()
            + this->hand().margin_x());
  case Position::WEST:
    return this->hand().start_x();
  case Position::EAST:
    return (this->hand().start_x()
            + this->hand().width()
            - this->ui->cards->width(this->pile_rotation()));
  case Position::CENTER:
    DEBUG_ASSERTION(false,
                    "TrickPile::start_x()\n"
                    "  wrong position 'CENTER' "
                    << "(" << this->position() << ")");
    break;
  } // switch(this->position())

  return 0;
} // int TrickPile::start_x()

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    starting y position of the trickpile
 **/
int
TrickPile::start_y() const
{
  switch(this->position()) {
  case Position::NORTH:
    return this->hand().start_y();
  case Position::SOUTH:
    return (this->hand().start_y()
            + this->hand().height()
            - this->ui->cards->height(this->pile_rotation()));
  case Position::WEST:
    return (this->hand().start_y()
            + this->hand().height()
            + this->hand().margin_y());
  case Position::EAST:
    return (this->hand().start_y()
            - this->hand().margin_y()
            - this->ui->cards->height(this->pile_rotation()));
  case Position::CENTER:
    DEBUG_ASSERTION(false,
                    "TrickPile::start_y()\n"
                    "  wrong position 'CENTER' "
                    << "(" << this->position() << ")");
    break;
  } // switch(this->position())

  return 0;
} // int TrickPile::start_y()

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    x step of the tricks
 **/
int
TrickPile::tricks_step_x() const
{
  if (!this->game().rule()(Rule::SHOW_TRICKS_IN_TRICKPILES))
    return 0;

  switch(this->position()) {
  case Position::NORTH:
    return -TRICKPILE_TRICKS_STEP_BIG_PART;
  case Position::SOUTH:
    return TRICKPILE_TRICKS_STEP_BIG_PART;
  case Position::WEST:
    return TRICKPILE_TRICKS_STEP_SMALL_PART;
  case Position::EAST:
    return -TRICKPILE_TRICKS_STEP_SMALL_PART;
  case Position::CENTER:
    DEBUG_ASSERTION(false,
                    "TrickPile::tricks_step_x()\n"
                    "  wrong position 'CENTER' "
                    << "(" << this->position() << ")");
    break;
  } // switch(this->position())

  return 0;
} // int TrickPile::tricks_step_x()

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    y step of the tricks
 **/
int
TrickPile::tricks_step_y() const
{
  if (!this->game().rule()(Rule::SHOW_TRICKS_IN_TRICKPILES))
    return 0;

  switch(this->position()) {
  case Position::NORTH:
    return TRICKPILE_TRICKS_STEP_SMALL_PART;
  case Position::SOUTH:
    return -TRICKPILE_TRICKS_STEP_SMALL_PART;
  case Position::WEST:
    return TRICKPILE_TRICKS_STEP_BIG_PART;
  case Position::EAST:
    return -TRICKPILE_TRICKS_STEP_BIG_PART;
  case Position::CENTER:
    DEBUG_ASSERTION(false,
                    "TrickPile::tricks_step_y()\n"
                    "  wrong position 'CENTER' "
                    << "(" << this->position() << ")");
    break;
  } // switch(this->position())

  return 0;
} // int TrickPile::tricks_step_y()

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    x step of the cards
 **/
int
TrickPile::cards_step_x() const
{
  switch(this->position()) {
  case Position::NORTH:
    return -TRICKPILE_CARDS_STEP_BIG_PART;
  case Position::SOUTH:
    return TRICKPILE_CARDS_STEP_BIG_PART;
  case Position::WEST:
    return TRICKPILE_CARDS_STEP_SMALL_PART;
  case Position::EAST:
    return -TRICKPILE_CARDS_STEP_SMALL_PART;
  case Position::CENTER:
    DEBUG_ASSERTION(false,
                    "TrickPile::cards_step_x()\n"
                    "  wrong position 'CENTER' "
                    << "(" << this->position() << ")");
    break;
  } // switch(this->position())

  return 0;
} // int TrickPile::cards_step_x()

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    y step of the cards
 **/
int
TrickPile::cards_step_y() const
{
  switch(this->position()) {
  case Position::NORTH:
    return TRICKPILE_CARDS_STEP_SMALL_PART;
  case Position::SOUTH:
    return -TRICKPILE_CARDS_STEP_SMALL_PART;
  case Position::WEST:
    return TRICKPILE_CARDS_STEP_BIG_PART;
  case Position::EAST:
    return -TRICKPILE_CARDS_STEP_BIG_PART;
  case Position::CENTER:
    DEBUG_ASSERTION(false,
                    "TrickPile::cards_step_y()\n"
                    "  wrong position 'CENTER' "
                    << "(" << this->position() << ")");
    break;
  } // switch(this->position())

  return 0;
} // int TrickPile::cards_step_y()

/**
 ** -> result
 **
 ** @param     x   x position
 ** @param     y   y position
 **
 ** @return    the pointer to the trick under the mouse curser
 **		nullptr if the pointer is over no trick
 **			or the trick cannot be shown (-> rules)
 **
 ** @todo	transparency
 **/
::Trick const*
TrickPile::over_trick(int const x, int const y) const
{
  if (::in_running_game()) {
    auto const& trickpile = this->player().trickpile();

    for (int t = trickpile.size() - 1; t >= 0; --t) {
      if (trickpile[t]->no()
          + this->game().rule()(Rule::SHOW_TRICKS_NUMBER)
          < this->game().real_trick_current_no())
        break;

      int const c = this->game().number_of_players() - 1;

      int const step_x = ((this->cards_step_x() > 0)
                          ? this->cards_step_x()
                          : -this->cards_step_x());
      int const step_y = ((this->cards_step_y() > 0)
                          ? this->cards_step_y()
                          : -this->cards_step_y());
      int const pos_x = (this->start_x()
                         + t * this->tricks_step_x()
                         + c * ((this->cards_step_x() > 0)
                                ? 0
                                : -step_x));

      int const pos_y = (this->start_y()
                         + t * this->tricks_step_y()
                         + c * ((this->cards_step_y() > 0)
                                ? 0
                                : -step_y));

      if ( (x >= pos_x)
          && (x < (pos_x
                   + step_x * c
                   + this->ui->cards->back(this->pile_rotation()
                                          ).get_width()))
          && (y >= pos_y)
          && (y < (pos_y
                   + step_y * c
                   + this->ui->cards->back(this->pile_rotation()
                                          ).get_height()))
         ) {
        return trickpile[t];
      }
    } // if (this->player has made the last trick)
  } // if (::in_running_game())

  return nullptr;
} // ::Trick const* TrickPile::over_trick(int x, int y)

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    height of the name
 **/
void
TrickPile::update_layout()
{ 
  this->points_layout->set_font_description(Pango::FontDescription(::setting(Setting::TRICKPILE_POINTS_FONT)));
} // void TrickPile::update_layout()

} // namespace UI_GTKMM_NS

#endif // #ifdef USE_UI_GTKMM
