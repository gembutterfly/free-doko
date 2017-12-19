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

#ifndef GTKMM_TRICKPILE_HEADER
#define GTKMM_TRICKPILE_HEADER

#include "htin.h"
class Trick;

#include <pangomm/layout.h>

namespace UI_GTKMM_NS {

/**
 ** @brief	the graphic representation of a trickpile with the points
 **
 ** @todo	use of 'Gdk::Point' and 'Gdk::Rectangle'
 **/
class TrickPile : public HTIN {
  public:
    TrickPile(Table* table, Position position);
    ~TrickPile() = default;

    using HTIN::draw;
    void draw(Cairo::RefPtr<::Cairo::Context> cr, bool update = false);
    void draw_tricks(Cairo::RefPtr<::Cairo::Context> cr);
    void draw_points(Cairo::RefPtr<::Cairo::Context> cr, bool update = false);

    Rotation pile_rotation() const;
    int pos_x() const;
    int pos_y() const;
    int width() const;
    int height() const;
    int start_x() const;
    int start_y() const;
    int tricks_step_x() const;
    int tricks_step_y() const;
    int cards_step_x() const;
    int cards_step_y() const;

    ::Trick const* over_trick(int x, int y) const;

    void update_layout();

  private:
    Glib::RefPtr<Pango::Layout> points_layout;

  private: // unused
    TrickPile() = delete;
    TrickPile(TrickPile const&) = delete;
    TrickPile& operator=(TrickPile const&) = delete;
}; // class TrickPile : public HTIN

} // namespace UI_GTKMM_NS

#endif // #ifdef GTKMM_TRICKPILE_HEADER

#endif // #ifdef USE_UI_GTKMM
