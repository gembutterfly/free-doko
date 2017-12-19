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

#ifndef GTKMM_ICONGROUP_HEADER
#define GTKMM_ICONGROUP_HEADER

#include "htin.h"

namespace UI_GTKMM_NS {

/**
 **
 ** @brief	the graphic representation of a hand
 **
 ** @todo	test overlapping of icons
 ** @todo	use of 'Gdk::Point' and 'Gdk::Rectangle'
 **
 **/
class Icongroup : public HTIN {
  public:
    Icongroup(Table* table, Position position);
    ~Icongroup() = default;

    using HTIN::draw;
    void draw(Cairo::RefPtr<::Cairo::Context> cr, bool update = false);
    void draw_team();
    void draw_team(Cairo::RefPtr<::Cairo::Context> cr, bool update = false);
    void draw_announcement();
    void draw_announcement(Cairo::RefPtr<::Cairo::Context> cr, bool update = false);
    void draw_swines();
    void draw_swines(Cairo::RefPtr<::Cairo::Context> cr, bool update = false);

    int width(Position position) const;
    int height(Position position) const;
    int margin_x(Position position) const;
    int margin_y(Position position) const;

    int width() const;
    int height() const;
    int margin_x() const;
    int margin_y() const;

    int team_start_x(int width) const;
    int team_start_y(int height) const;
    int announcement_start_x(int width) const;
    int announcement_start_y(int height) const;
    int swines_start_x(int width) const;
    int swines_start_y(int height) const;

    bool mouse_over_next_announcement() const;

  private: // unused
    Icongroup() = delete;
    Icongroup(Icongroup const&) = delete;
    Icongroup& operator=(Icongroup const&) = delete;
}; // class Icongroup : public HTIN

} // namespace UI_GTKMM_NS

#endif // #ifdef GTKMM_ICONGROUP_HEADER

#endif // #ifdef USE_UI_GTKMM
