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

#ifndef GTKMM_TRICK_HEADER
#define GTKMM_TRICK_HEADER

#include "trick_drawing.h"

class Trick;

namespace UI_GTKMM_NS {
class Table;

/**
 ** @brief	the graphic representation of the current trick
 **/
class Trick : public TrickDrawing {
  public:
    // constructor
    Trick(Table* table);
    // destuctor
    ~Trick();

    // the table
    Table const& table() const;
    // the table
    Table& table();

    // draw the trick
    void draw();
    void draw(Cairo::RefPtr<::Cairo::Context> cr, bool update = true);
    // whether the mouse is over the trick
    bool mouse_over_cards() const;
    // whether the mouse is over the trick in a sloppy way
    bool mouse_over_cards_sloppy() const;

  private: // unused
    Trick() = delete;
    Trick(Trick const&) = delete;
    Trick& operator=(Trick const&) = delete;
}; // class Trick : public Base 

} // namespace UI_GTKMM_NS

#endif // #ifdef GTKMM_TRICK_HEADER

#endif // #ifdef USE_UI_GTKMM
