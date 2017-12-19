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

#include "trick_drawing.h"
#include "ui.h"
#include "table.h"
#include "cards.h"

#include "../../card/trick.h"
#include "../../misc/setting.h"

#include <gdkmm/general.h>

namespace UI_GTKMM_NS {

/**
 ** constructor
 **
 ** @param     base   base element
 **
 ** @return    -
 **/
TrickDrawing::TrickDrawing(Base* const base) :
  Base{base}
{ }

/**
 ** destruktor
 **
 ** @param     -
 **
 ** @return    -
 **
 **/
TrickDrawing::~TrickDrawing()
{ }

/**
 ** sets the center of the trick in the drawable
 **
 ** @param     center   position of the center
 **
 ** @return    -
 **/
void
TrickDrawing::set_center(Gdk::Point const& center)
{
  this->center.set_x(center.get_x());
  this->center.set_y(center.get_y());
   this->center_set = true;

  return ;
} // void TrickDrawing::set_center(Gdk::Point center)

/**
 ** sets the trick
 **
 ** @param     trick   trick to draw
 **
 ** @return    -
 **/
void
TrickDrawing::set_trick(::Trick const& trick)
{
  if (&trick == &::Trick::EMPTY)
    this->remove_trick();
  else
    this->trick = &trick;

  return ;
} // void TrickDrawing::set_trick(::Trick trick)

/**
 ** removes the trick
 **
 ** @param     -
 **
 ** @return    -
 **/
void
TrickDrawing::remove_trick()
{
  this->trick = nullptr;

  return ;
} // void TrickDrawing::remove_trick()

/**
 ** draws the trick
 **
 ** @param     cr     context to draw into
 **
 ** @return    -
 **/
void
TrickDrawing::draw(Cairo::RefPtr<Cairo::Context> const& cr)
{
  DEBUG_ASSERTION(this->trick,
                  "TrickDrawing::draw(drawable):\n"
                  "  this->trick == nullptr");

  for (size_t c = 0; c < this->trick->actcardno(); c++) {
    auto pos_x = (this->center_set ? this->center.get_x() : 0);
    auto pos_y = (this->center_set ? this->center.get_y() : 0);
    auto const& player = this->trick->player_of_card(c);

    switch (this->ui->table->position(player)) {
    case NORTH:
      pos_x -= this->ui->cards->width() / 2;
      pos_y -= this->ui->cards->height();

      pos_x -= this->overlap_x();
      pos_y += this->overlap_y();
      break;
    case SOUTH:
      pos_x -= this->ui->cards->width() / 2;

      pos_x += this->overlap_x();
      pos_y -= this->overlap_y();
      break;
    case WEST:
      if (::setting(Setting::ROTATE_TRICK_CARDS)) {
        pos_x -= this->ui->cards->width(this->ui->table->rotation(player));
        pos_y -= (this->ui->cards->height(this->ui->table->rotation(player))
                  / 2);
      } else { // if (::setting(Setting::ROTATE_TRICK_CARDS))
        pos_x -= this->ui->cards->width();
        pos_y -= this->ui->cards->height() / 2;
      } // if (::setting(Setting::ROTATE_TRICK_CARDS))
      pos_x += this->overlap_x();
      pos_y += this->overlap_y();
      break;
    case EAST:
      if (::setting(Setting::ROTATE_TRICK_CARDS)) {
        pos_y -= (this->ui->cards->height(this->ui->table->rotation(player))
                  / 2);
      } else { // if (::setting(Setting::ROTATE_TRICK_CARDS))
        pos_y -= this->ui->cards->height() / 2;
      } // if (::setting(Setting::ROTATE_TRICK_CARDS))
      pos_x -= this->overlap_x();
      pos_y -= this->overlap_y();
      break;
    case CENTER:
      DEBUG_ASSERTION(false,
                      "TrickDrawgin::draw(drawable)\n"
                      "  wrong position 'CENTER' "
                      << "(" << this->ui->table->position(player) << ")");
      break;
    } // switch (this->ui->table->position(player))
    auto trick_pixbuf
      = (::setting(Setting::ROTATE_TRICK_CARDS)
         ? this->ui->cards->card(this->trick->card(c),
                                 this->ui->table->rotation(player))
         : this->ui->cards->card(this->trick->card(c))
        ).pixbuf();
    // check whether the cards shall be drawn greyed
    // Used in network games: if the player has taken the trick
    // but not all other players.
    if (   (::game_status == GAMESTATUS::GAME_TRICK_TAKEN)
        && !this->trick->intrickpile()) {
      auto copy = trick_pixbuf->copy();
      trick_pixbuf->saturate_and_pixelate(copy, 0, true);
      trick_pixbuf = copy;
    } // if (draw cards grayed)
    Gdk::Cairo::set_source_pixbuf(cr, trick_pixbuf, pos_x, pos_y);
    cr->paint();
  } // for (c < this->trick->actcardno())
} // void TrickDrawing::draw(Cairo::RefPtr<Cairo::Context> cr)

/**
 ** ->result
 **
 ** @param     mouse_x   x position
 ** @param     mouse_y   y position
 **
 ** @return    whether the mouse is over the trick
 **/
bool
TrickDrawing::mouse_over_cards(int const mouse_x, int const mouse_y) const
{
  DEBUG_ASSERTION(this->center_set,
                  "TrickDrawing::mouse_over_cards(x, y)\n"
                  "  center_set is false");

  for (unsigned c = 0; c < this->trick->actcardno(); c++) {
    int pos_x = this->center.get_x();
    int pos_y = this->center.get_y();
    Player const& player = this->trick->player_of_card(c);

    switch (this->ui->table->position(player)) {
    case NORTH:
      pos_x -= this->ui->cards->width() / 2;
      pos_y -= this->ui->cards->height();

      pos_x -= this->overlap_x();
      pos_y += this->overlap_y();
      break;
    case SOUTH:
      pos_x -= this->ui->cards->width() / 2;

      pos_x += this->overlap_x();
      pos_y -= this->overlap_y();
      break;
    case WEST:
      if (::setting(Setting::ROTATE_TRICK_CARDS)) {
        pos_x -= this->ui->cards->width(this->ui->table->rotation(player));
        pos_y -= (this->ui->cards->height(this->ui->table->rotation(player))
                  / 2);
      } else { // if (::setting(Setting::ROTATE_TRICK_CARDS))
        pos_x -= this->ui->cards->width();
        pos_y -= this->ui->cards->height() / 2;
      } // if (::setting(Setting::ROTATE_TRICK_CARDS))
      pos_x += this->overlap_x();
      pos_y += this->overlap_y();
      break;
    case EAST:
      if (::setting(Setting::ROTATE_TRICK_CARDS)) {
        pos_y -= (this->ui->cards->height(this->ui->table->rotation(player))
                  / 2);
      } else { // if (::setting(Setting::ROTATE_TRICK_CARDS))
        pos_y -= this->ui->cards->height() / 2;
      } // if (::setting(Setting::ROTATE_TRICK_CARDS))
      pos_x -= this->overlap_x();
      pos_y -= this->overlap_y();
      break;
    case CENTER:
      DEBUG_ASSERTION(false,
                      "TrickDrawing::mouse_over_cards()\n"
                      "  wrong position 'CENTER' "
                      << "(" << this->ui->table->position(player) << ")");
      break;
    } // switch (this->ui->table->position(player))
    auto trick_pixbuf
      = (::setting(Setting::ROTATE_TRICK_CARDS)
         ? this->ui->cards->card(this->trick->card(c),
                                 this->ui->table->rotation(player))
         : this->ui->cards->card(this->trick->card(c))
        );

    if (   (mouse_x >= pos_x)
        && (mouse_y >= pos_y)
        && (mouse_x <  pos_x + trick_pixbuf.get_width())
        && (mouse_y <  pos_y + trick_pixbuf.get_height())
       ) {
      // ToDo: check for transparency
      return true;
    } // if (mouse over card)
  } // for (c < this->trick->actcardno())

  return false;
} // bool TrickDrawing::mouse_over_cards(int mouse_x, int mouse_y) const

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    starting position of the trick
 **
 **/
Gdk::Point
TrickDrawing::start() const
{
  return Gdk::Point{this->center.get_x() - (this->width() / 2),
    this->center.get_y() - (this->height() / 2)};
} // Gdk::Point TrickDrawing::start()

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    starting y position of the trick
 **/
Gdk::Rectangle
TrickDrawing::outline() const
{
  return Gdk::Rectangle{this->start().get_x(),
    this->start().get_y(),
    this->width(),
    this->height()};
} // Gdk::Rectangle TrickDrawing::outline()

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    width of the trick
 **/
int
TrickDrawing::width() const
{
  return (2 * ((::setting(Setting::ROTATE_TRICK_CARDS)
                ? this->ui->cards->height()
                : this->ui->cards->width()
               ) - this->overlap_x()));
} // int TrickDrawing::width()

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    height of the trick
 **/
int
TrickDrawing::height() const
{
  return (2 * (this->ui->cards->height() - this->overlap_y()));
} // int TrickDrawing::height()

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    the x overlapping over the center of the cards
 **/
int
TrickDrawing::overlap_x() const
{
  return ((::setting(Setting::ROTATE_TRICK_CARDS)
           ? this->ui->cards->height()
           : this->ui->cards->width()
          ) / 10);
} // int TrickDrawing::overlap_x()

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    the y overlapping over the center of the cards
 **/
int
TrickDrawing::overlap_y() const
{
  return (this->ui->cards->height() / 8);
} // int TrickDrawing::overlap_y()

} // namespace UI_GTKMM_NS

#endif // #ifdef USE_UI_GTKMM
