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

#include "name.h"
#include "table.h"
#include "hand.h"
#include "icongroup.h"

#include "ui.h"
#include "cards.h"

#include "../../player/player.h"
#include "../../game/game.h"
#include "../../game/game.h"
#include "../../party/rule.h"
#include "../../card/trick.h"

#include "../../utils/string.h"

namespace UI_GTKMM_NS {

#define NAME_TRICKS_STEP_BIG_PART \
static_cast<int>(this->ui->cards->height() / 15.0)
#define NAME_TRICKS_STEP_SMALL_PART \
static_cast<int>(this->ui->cards->width() / 30.0)
#define NAME_CARDS_STEP_BIG_PART \
static_cast<int>(this->ui->cards->height() * 0)
#define NAME_CARDS_STEP_SMALL_PART \
static_cast<int>(this->ui->cards->width() / 20.0)

/**
 ** constructor
 **
 ** @param     table   the table
 ** @param     position   the position of the name
 **
 ** @return    -
 **/
Name::Name(Table* const table, Position const position) :
  HTIN(table, position)
{ }

/**
 ** draws the name
 **
 ** @param     cr       cairo context
 ** @param     update   whether to update the area (default: true)
 **
 ** @return    -
 **
 ** @bug	the rectangle does not take the 'italic' font into account
 **/
void
Name::draw(Cairo::RefPtr<::Cairo::Context> cr, bool const update)
{
  if (!(::game_status & GAMESTATUS::GAME))
    return ;
  if (!this->layout) {
    this->layout = Pango::Layout::create(cr);
    this->update_name();
    this->update_layout();
  }

#ifdef WINDOWS
#ifdef WORKAROUND
  // Der aktuelle Fensterinhalt wird nicht uebertragen
  if (update) {
    this->table().queue_resize();
    return ;
  }
#endif
#endif

  cr->save();
  cr->push_group();
  // test whether it is the active player
  bool is_active = false;
  switch (::game_status) {
  case GAMESTATUS::GAME_RESERVATION:
    // in the reservation show the current player
    if (this->game().player_current().type() == Player::HUMAN) {
      // for the human show the startplayer
      if (   GAMETYPE::is_solo(this->game().type())
          && (this->game().rule()(Rule::LUSTSOLO_PLAYER_LEADS)))
        is_active = (this->player()
                     == this->game().player_current());
      else
        is_active = (this->player()
                     == this->game().startplayer());
    } else {
      // for all other players show the player to wait for
      // (for the network game)
      is_active = (this->player()
                   == this->game().player_current());
    }
    break;
  case GAMESTATUS::GAME_START:
  case GAMESTATUS::GAME_INIT:
    is_active = (this->player()
                 == this->game().player_current());
    break;
  case GAMESTATUS::GAME_POVERTY_SHIFT:
    if (this->game().player_current().type() == Player::HUMAN)
      is_active = (this->player()
                   == this->game().startplayer());
    else
      is_active = (this->player()
                   == this->game().player_current());

    break;
  case GAMESTATUS::GAME_PLAY:
  case GAMESTATUS::GAME_FULL_TRICK:
  case GAMESTATUS::GAME_TRICK_TAKEN:
    is_active = (this->player() == this->game().player_current());
    break;
  case GAMESTATUS::GAME_FINISHED:
    if (this->table().in_game_review())
      is_active = (   this->table().game_review_trick_visible()
                   && (this->player()
                       == this->table().game_review_trick().winnerplayer()));
    else
      is_active = (   (this->player().team()
                       == this->game().winnerteam())
                   || (   (this->game().winnerteam()
                           == TEAM::NOTEAM)
                       && (this->player().team() == TEAM::RE)) );
#ifdef POSTPONED
    if (this->gui().playfield().table().last_trick().trickno()
        == UINT_MAX) {
      is_active = (this->player().team()
                   == this->player().game().winnerteam());
    } else { // if (show a specific trick)
      is_active = (this->player()
                   == this->player().game().trick(this->gui().playfield().table().last_trick().trickno()).winnerplayer());
    } // if (show a specific trick)
#endif
    break;

  default:
    break;
  } // switch (::game_status)

  auto color = Gdk::Color(::setting(is_active
                                    ? Setting::NAME_ACTIVE_FONT_COLOR
                                    : (   (::game_status == GAMESTATUS::GAME_RESERVATION)
                                       && this->game().has_made_reservation(this->player())
                                       && (this->game().reservation(this->player()).game_type
                                           != GAMETYPE::NORMAL) )
                                    ? Setting::NAME_RESERVATION_FONT_COLOR
                                    : Setting::NAME_FONT_COLOR));

  {
    // bug: there seems to be a problem with italic characters
    auto layout_width_border = this->height() / 5;
    if (update) {
      // redrawing only the name leads to graphical errors (transparency?)
      this->table().clear(cr, Gdk::Rectangle(this->pos_x() - layout_width_border,
                                             this->pos_y(),
                                             this->width()
                                             + 2 * layout_width_border,
                                             this->height()));
      // ToDo: remove
      switch(this->position()) {
      case Position::NORTH:
      case Position::SOUTH:
        this->icongroup().draw(cr);
        break;
      case Position::EAST:
        if (this->width() > this->hand().width())
          this->icongroup(Position::SOUTH).draw(cr);
        break;
      case Position::WEST:
        break;
      case Position::CENTER:
        DEBUG_ASSERTION(false,
                        "Name::draw(update)\n"
                        "  wrong position 'CENTER' "
                        << "(" << this->position() << ")");
        break;
      } // switch(this->position)
    } // if (update)

    cr->move_to(this->pos_x(), this->pos_y());
    cr->set_source_rgb(color.get_red_p(), color.get_green_p(), color.get_blue_p());
    this->layout->show_in_cairo_context(cr);
    if (   is_active
        && this->ui->in_progress()
        && this->ui->progress() != 1
       ) {
      {
#ifdef TODO
        // die Größen sind nicht richtig aktualisiert
        this->update_name();
#endif
        cr->save();
        cr->rectangle(this->pos_x(), this->pos_y(),
                      static_cast<int>(this->width() * this->ui->progress()),
                      this->height());
        cr->clip();
        auto color = Gdk::Color(::setting(Setting::NAME_RESERVATION_FONT_COLOR));
        cr->set_source_rgb(color.get_red_p(), color.get_green_p(), color.get_blue_p());
        cr->move_to(this->pos_x(), this->pos_y());
        this->layout->show_in_cairo_context(cr);
        cr->restore();
      }
    } // if !(progress and active player)
  }

  cr->pop_group_to_source();
  cr->paint();
  cr->restore();

  return ;
} // void Name::draw(Cairo::RefPtr<::Cairo::Context> cr, bool update = false)

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    starting x position of the name
 **/
int
Name::pos_x() const
{
  int value = this->hand().pos_x();

  if (this->position() == Position::EAST) {
    if (this->width() > this->hand().width())
      value -= (this->width() - this->hand().width());
  } // if (this->position() == Position::EAST)

  return value;
} // int Name::pos_x() const

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    starting y position of the name
 **/
int
Name::pos_y() const
{
  switch(this->position()) {
  case Position::NORTH:
    return (this->hand().pos_y()
            + this->hand().height()
            + this->hand().margin_y());
  case Position::WEST:
    return (this->hand().pos_y()
            - this->hand().margin_y()
            - this->height());
  case Position::SOUTH:
    return (this->hand().pos_y()
            - this->hand().margin_y()
            - this->height());
  case Position::EAST:
    return (this->hand().pos_y()
            + this->hand().height()
            + this->hand().margin_y());
  case Position::CENTER:
    DEBUG_ASSERTION(false,
                    "Name::pos_y()\n"
                    "  wrong position 'CENTER' "
                    << "(" << this->position() << ")");
    break;
  } // switch(position)

  return 0;
} // int Name::pos_y() const

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    width of the name
 **/
int
Name::width() const
{ return this->width_; }

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    height of the name
 **/
int
Name::height() const
{ return this->height_; }

/**
 ** the name has changed
 **
 ** @param     -
 **
 ** @return    -
 **/
void
Name::update_name()
{
  this->layout->set_text(this->player().name());
  this->layout->get_pixel_size(this->width_, this->height_);
} // void Name::update_name()

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    height of the name
 **/
void
Name::update_layout()
{ 
  this->layout->set_font_description(Pango::FontDescription(::setting(Setting::NAME_FONT)));
  this->layout->get_pixel_size(this->width_, this->height_);
} // void Name::update_layout()

} // namespace UI_GTKMM_NS

#endif // #ifdef USE_UI_GTKMM
