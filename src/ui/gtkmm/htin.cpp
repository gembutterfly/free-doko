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

#include "htin.h"
#include "table.h"
#include "ui.h"

#include "../../game/game.h"
#include "../../player/player.h"

#include <gdkmm/general.h>

namespace UI_GTKMM_NS {

/**
 ** constructor
 **
 ** @param     table      the table
 ** @param     position   the position of the element
 **
 ** @return    -
 **/
HTIN::HTIN(Table* const table, Position const position) :
  Base{table},
  position_{position}
{ }

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    the table
 **/
Table&
HTIN::table()
{
  return *dynamic_cast<Table*>(this->parent);
} // Table& HTIN::table()

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    the table
 **/
Table const&
HTIN::table() const
{
  return *dynamic_cast<Table*>(this->parent);
} // Table const& HTIN::table() const

/**
 ** draw the element and update the area
 **
 ** @param    -
 **
 ** @return   -
 **/
void
HTIN::draw()
{
  this->draw(this->table().create_cairo_context(), true);
} // void HTIN::draw()

/**
 ** draw a pixbuf
 **
 ** @param    cr       drawing context
 ** @param    pixbuf   pixbuf to draw
 ** @param    x        x position
 ** @param    y        y position
 **
 ** @return   -
 **/
void
HTIN::draw(Cairo::RefPtr<::Cairo::Context> cr,
           Glib::RefPtr<Gdk::Pixbuf> pixbuf,
           int const x, int const y)
{
  if (!pixbuf)
    return ;
  Gdk::Cairo::set_source_pixbuf(cr, pixbuf, x, y);
  cr->paint();
} // void HTIN::draw(Cairo::RefPtr<::Cairo::Context> cr,Glib::RefPtr<Gdk::Pixbuf> pixbuf, int x, int y)


/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    the rotation of the object
 **/
Rotation
HTIN::rotation() const
{
  return this->table().rotation(this->player());
} // Rotation HTIN::rotation() const

/**
 ** -> result
 **
 ** @param     position   the position
 **
 ** @return    the rotation of the object at 'position'
 **/
Rotation
HTIN::rotation(Position const position) const
{
  return this->table().rotation(position);
} // Rotation HTIN::rotation(Position position) const

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    the corresponding hand
 **/
Hand&
HTIN::hand()
{
  return this->table().hand(this->player());
} // Hand& HTIN::hand()

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    the corresponding hand
 **/
Hand const&
HTIN::hand() const
{
  return this->table().hand(this->position());
} // Hand const& HTIN::hand() const

/**
 ** -> result
 **
 ** @param     position   the position
 **
 ** @return    the hand at 'position'
 **/
Hand&
HTIN::hand(Position const position)
{
  return this->table().hand(position);
} // Hand& HTIN::hand(Position position)

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    the corresponding trickpile
 **/
TrickPile&
HTIN::trickpile()
{
  return this->table().trickpile(this->position());
} // TrickPile& HTIN::trickpile()

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    the corresponding trickpile
 **/
TrickPile const&
HTIN::trickpile() const
{
  return this->table().trickpile(this->position());
} // TrickPile const& HTIN::trickpile() const

/**
 ** -> result
 **
 ** @param     position   the position
 **
 ** @return    the trickpile at 'position'
 **/
TrickPile&
HTIN::trickpile(Position const position)
{
  return this->table().trickpile(position);
} // TrickPile& HTIN::trickpile(Position position)

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    the corresponding icongroup
 **/
Icongroup&
HTIN::icongroup()
{
  return this->table().icongroup(this->position());
} // Icongroup& HTIN::icongroup()

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    the corresponding icongroup
 **/
Icongroup const&
HTIN::icongroup() const
{
  return this->table().icongroup(this->position());
} // Icongroup const& HTIN::icongroup() const

/**
 ** -> result
 **
 ** @param     position   the position
 **
 ** @return    the icongroup at 'position'
 **/
Icongroup&
HTIN::icongroup(Position const position)
{
  return this->table().icongroup(position);
} // Icongroup& HTIN::icongroup(Position position)

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    the corresponding name
 **/
Name&
HTIN::name()
{
  return this->table().name(this->position());
} // Name& HTIN::name()

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    the corresponding name
 **/
Name const&
HTIN::name() const
{
  return this->table().name(this->position());
} // Name const& HTIN::name() const

/**
 ** -> result
 **
 ** @param     position   the position
 **
 ** @return    the name at 'position'
 **/
Name&
HTIN::name(Position const position)
{
  return this->table().name(position);
} // Name& HTIN::name(Position position)

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    the corresponding player
 **/
Player const&
HTIN::player() const
{
  return this->table().player(this->position());
} // Player const& HTIN::player() const

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    the corresponding game
 **/
Game const&
HTIN::game() const
{
  return this->ui->game();
} // Game const& HTIN::game() const

} // namespace UI_GTKMM_NS

#endif // #ifdef USE_UI_GTKMM
