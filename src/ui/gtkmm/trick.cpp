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

#include "trick.h"
#include "table.h"
#include "icongroup.h"

#include "../../party/party.h"
#include "../../game/game.h"
#include "../../card/trick.h"
#include "../../player/player.h"

namespace UI_GTKMM_NS {

/**
 ** constructor
 **
 ** @param     table   corresponding table
 **
 ** @return    -
 **/
Trick::Trick(Table* const table) :
  TrickDrawing(table)
{ }

/**
 ** destructor
 **
 ** @param     -
 **
 ** @return    -
 **/
Trick::~Trick()
{ }

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    the table
 **/
Table&
Trick::table()
{
  return static_cast<Table&>(*this->parent);
} // Table& Trick::table()

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    the table
 **/
Table const&
Trick::table() const
{
  return static_cast<Table&>(*this->parent);
} // Table const& Trick::table() const

/**
 ** draw the element and update the area
 **
 ** @param    -
 **
 ** @return   -
 **/
void
Trick::draw()
{
  this->draw(this->table().create_cairo_context(), true);
} // void Trick::draw()

/**
 ** draws the trick
 **
 ** @param     cr       drawing context
 ** @param     update   whether to update the area
 **
 ** @return    -
 **/
void
Trick::draw(Cairo::RefPtr<::Cairo::Context> cr, bool const update)
{
  if (!(   (::game_status >= GAMESTATUS::GAME_PLAY)
        && (   (::game_status < GAMESTATUS::GAME_FINISHED)
            || (this->table().in_game_review()) ) ) )
    return ;

  cr->push_group();
  this->set_center(Gdk::Point(this->table().width() / 2,
                              this->table().height() / 2));

  if (this->table().in_game_review()) {
    if (this->table().game_review_trick_visible())
      this->set_trick(this->table().game_review_trick());
    else
      this->remove_trick();
  } else {
    this->set_trick(::party.game().trick_current());
  }

  if (update) {
    this->table().clear(cr, this->outline());
  }

  // only show the trick if it is in the middle
  // (and not in the trickpile)
  if (this->trick
      && (!this->trick->isfull()
          || this->trick->winnerplayer().trickpile().empty()
          || (this->trick->winnerplayer().trickpile().back()->no()
              < this->trick->no())
          || this->table().in_game_review()))
    this->TrickDrawing::draw(cr);
  cr->pop_group_to_source();
  cr->paint();

  return ;
} // void Trick::draw(Cairo::RefPtr<::Cairo::Context> cr, bool update = true)

/**
 ** ->result
 **
 ** @param     -
 **
 ** @return    whether the mouse is over the trick
 **/
bool
Trick::mouse_over_cards() const
{
  if (!this->trick)
    return false;

  int x, y;
  this->table().get_pointer(x, y);
  return this->TrickDrawing::mouse_over_cards(x, y);
} // bool Trick::mouse_over_cards() const

/**
 ** ->result
 **
 ** @param     -
 **
 ** @return    whether the mouse is over the trick (sloppy version)
 **/
bool
Trick::mouse_over_cards_sloppy() const
{
  if (!this->trick)
    return false;

  auto const outline = this->TrickDrawing::outline();

  auto rec
    = Gdk::Rectangle{outline.get_x() / 2
      + (this->table().icongroup(Position::WEST).announcement_start_x(0)
         +  this->table().icongroup(Position::WEST).width()
        ) / 2,
        outline.get_y() / 2
          + (this->table().icongroup(Position::NORTH).announcement_start_y(0)
             +  this->table().icongroup(Position::NORTH).height()
            ) / 2,
        0, 0};
  rec.set_width(outline.get_width()
                + 2 * (outline.get_x() - rec.get_x())
                + 1);
  rec.set_height(outline.get_height()
                 + 2 * (outline.get_y() - rec.get_y())
                 + 1);

  int x, y;
  this->table().get_pointer(x, y);

  return (   (x >= rec.get_x())
          && (x < rec.get_x() + rec.get_width())
          && (y >= rec.get_y())
          && (y < rec.get_y() + rec.get_height()) );
} // bool Trick::mouse_over_cards_sloppy() const

} // namespace UI_GTKMM_NS

#endif // #ifdef USE_UI_GTKMM
