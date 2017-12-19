/**********************************************************************
 *
 *   FreeDoko a Doppelkopf-Game
 *    
 *   Copyright (C) 2001-2017 by Diether Knof and Borg Enders
 *
 *   This program is free software; you can redistribute it and/or 
 *   modify it under the terms of the GNU General Public ScaledPixbufRotations as 
 *   published by the Free Software Foundation; either version 2 of 
 *   the ScaledPixbufRotations, or (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public ScaledPixbufRotations for more details. 
 *   You can find this license in the file 'gpl.txt'.
 *
 *   You should have received a copy of the GNU General Public ScaledPixbufRotations
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

#ifndef GTKMM_WIDGET_SCALED_PIXBUF_ROTATIONS_HEADER
#define GTKMM_WIDGET_SCALED_PIXBUF_ROTATIONS_HEADER

#include "../../../basistypes.h"
#include "scaled_pixbuf.h"

namespace Gdk {
/**
 ** @brief	a scaled pixmap with rotations (UP, DOWN, LEFT, RIGHT)
 **		the rotations are created when needed
 **
 ** @author	Diether Knof
 **/
class ScaledPixbufRotations {
  public:
    ScaledPixbufRotations(string const filename);
    ScaledPixbufRotations(Glib::RefPtr<Pixbuf> const pixbuf);
    ScaledPixbufRotations();
    ScaledPixbufRotations(ScaledPixbufRotations const& scaled_pixbuf_rotations);
    ScaledPixbufRotations& operator=(ScaledPixbufRotations const&);

    ~ScaledPixbufRotations() = default;

    ScaledPixbuf& operator[](Rotation const rotation);
    operator bool() const;

    void set_scaling(double const scaling);

    int get_width(Rotation const rotation) const;
    int get_width() const { return this->get_width(Rotation::UP); }
    int get_height(Rotation const rotation) const;
    int get_height() const { return this->get_height(Rotation::UP); }
    int get_orig_width(Rotation const rotation) const;
    int get_orig_width() const { return this->get_orig_width(Rotation::UP); }
    int get_orig_height(Rotation const rotation) const;
    int get_orig_height() const { return this->get_orig_height(Rotation::UP); }

  private:
    ScaledPixbuf& create_rotation(Rotation const rotation);
  private:
    // the [0] (UP) is always a pointer to '*this',
    // the other three (DOWN, LEFT, RIGHT) are created when requested
    vector<unique_ptr<ScaledPixbuf>> scaled_pixbuf = vector<unique_ptr<ScaledPixbuf>>(4);

}; // class ScaledPixbufRotations

} // namespace Gdk

#endif // #ifdef GTKMM_WIDGET_SCALED_PIXBUF_ROTATIONS_HEADER

#endif // #ifdef USE_UI_GTKMM
