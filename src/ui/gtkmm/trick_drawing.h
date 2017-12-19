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

#ifndef GTKMM_TRICK_DRAWING_HEADER
#define GTKMM_TRICK_DRAWING_HEADER

#include "base.h"

#include <gdkmm/window.h>

class Trick;

namespace UI_GTKMM_NS {

/**
 ** @brief	drawing a trick on a Gdk::Drawable
 **
 ** @todo	use 'Gdk::Point' and 'Gdk::Rectangle'
 **/
class TrickDrawing : public Base {
  public:
    // constructor
    TrickDrawing(Base* base);
    // destructor
    ~TrickDrawing();

    // set the center for the draing
    void set_center(Gdk::Point const& center);
    // set the trick
    void set_trick(::Trick const& trick);
    // remove the trick
    void remove_trick();

    // draw the cards
    void draw(Cairo::RefPtr<Cairo::Context> const& cr);
    // whether the mouse is over the trick
    bool mouse_over_cards(int mouse_x, int mouse_y) const;

    // the starting point
    Gdk::Point start() const;
    // the outline of the trick
    Gdk::Rectangle outline() const;
    // the width of the trick
    int width() const;
    // the height of the trick
    int height() const;
    // the overlapping of the cards, x direction
    int overlap_x() const;
    // the overlapping of the cards, y direction
    int overlap_y() const;

  private:
    Gdk::Point center;
    bool center_set = false;
  protected:
    Trick const* trick = nullptr;

  private: // unused
    TrickDrawing() = delete;
    TrickDrawing(TrickDrawing const&) = delete;
    TrickDrawing& operator=(TrickDrawing const&) = delete;
}; // class TrickDrawing : public Base 

} // namespace UI_GTKMM_NS

#endif // #ifdef GTKMM_TRICK_DRAWING_HEADER

#endif // #ifdef USE_UI_GTKMM
