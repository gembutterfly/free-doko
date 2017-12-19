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

#ifdef USE_UI_GTKMM

#ifndef GTKMM_HTIN_HEADER
#define GTKMM_HTIN_HEADER

#include "base.h"
#include "../../basistypes.h"

class Player;
class Game;

#include <cairomm/context.h>
#include <gdkmm/pixbuf.h>

namespace UI_GTKMM_NS {
class Table;

class Hand;
class TrickPile;
class Icongroup;
class Name;

/**
 ** @brief	base class for 'Hand', 'TrickPile', 'Icongroup' and 'Name'
 **/
class HTIN : public Base {
  public:
    // constructor
    HTIN(Table* table, Position position);
    virtual ~HTIN() = default;

    // the corresponding table
    Table& table();
    Table const& table() const;
    // the rotation of this location
    Rotation rotation() const;

    // the corresponding player
    Player const& player() const;
    // the corresponding game
    Game const& game() const;

    // the corresponding hand
    Hand& hand();
    Hand const& hand() const;
    Hand& hand(Position position);
    // the corresponding trickpile
    TrickPile& trickpile();
    TrickPile const& trickpile() const;
    TrickPile& trickpile(Position position);
    // the corresponding icongroup
    Icongroup& icongroup();
    Icongroup const& icongroup() const;
    Icongroup& icongroup(Position position);
    // the corresponding name
    Name& name();
    Name const& name() const;
    Name& name(Position position);

    // draw the element
    void draw();
    virtual void draw(Cairo::RefPtr<::Cairo::Context> cr,
                      bool update = false) = 0;
    // draw a pixbuf
    void draw(Cairo::RefPtr<::Cairo::Context> cr,
              Glib::RefPtr<Gdk::Pixbuf> pixbuf, int x, int y);

    // the rotation at the given position
    Rotation rotation(Position position) const;

  private:
    PRIV_VAR_R(Position, position);

  private: // unused
    HTIN() = delete;
    HTIN(HTIN const&) = delete;
    HTIN& operator=(HTIN const&) = delete;
}; // class HTIN : public Base

} // namespace UI_GTKMM_NS

#endif // #ifdef GTKMM_HTIN_HEADER

#endif // #ifdef USE_UI_GTKMM
