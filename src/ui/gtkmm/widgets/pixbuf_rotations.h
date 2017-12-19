/**********************************************************************
 *
 *   FreeDoko a Doppelkopf-Game
 *    
 *   Copyright (C) 2001-2017 by Diether Knof and Borg Enders
 *
 *   This program is free software; you can redistribute it and/or 
 *   modify it under the terms of the GNU General Public PixbufRotations as 
 *   published by the Free Software Foundation; either version 2 of 
 *   the PixbufRotations, or (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public PixbufRotations for more details. 
 *   You can find this license in the file 'gpl.txt'.
 *
 *   You should have received a copy of the GNU General Public PixbufRotations
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

#ifndef GTKMM_WIDGET_PIXBUF_ROTATIONS_HEADER
#define GTKMM_WIDGET_PIXBUF_ROTATIONS_HEADER

#include "../../../basistypes.h"
#include <gdkmm/pixbuf.h>

namespace Gdk {
/**
 ** @brief	a pixmap with rotations (UP, DOWN, LEFT, RIGHT)
 **		the rotations are created when needed
 **
 ** @author	Diether Knof
 **/
class PixbufRotations : public Glib::RefPtr<Pixbuf> {
  public:
    // constructor (from file)
    PixbufRotations(string const& filename);
    // constructor (from a pixbuf)
    PixbufRotations(Glib::RefPtr<Pixbuf> pixbuf);
    // destructor
    virtual ~PixbufRotations();

    // pixbuf according to the rotation
    Glib::RefPtr<Pixbuf> operator[](Rotation rotation);

    // width of the pixbuf
    int width(Rotation rotation = Rotation::UP) const;
    // height of the pixbuf
    int height(Rotation rotation = Rotation::UP) const;

  private:
    Glib::RefPtr<Pixbuf> create_rotation(Rotation rotation);
  private:
    vector<Glib::RefPtr<Pixbuf> > pixbuf;

  private: // unused
    PixbufRotations() = delete;
    PixbufRotations(PixbufRotations const&) = delete;
    PixbufRotations& operator=(PixbufRotations const&) = delete;
}; // class PixbufRotations : public Glib::RefPtr<Pixbuf>

} // namespace Gdk

#endif // #ifdef GTKMM_WIDGET_PIXBUF_ROTATIONS_HEADER

#endif // #ifdef USE_UI_GTKMM
