/**********************************************************************
 *
 *   FreeDoko a Doppelkopf-Game
 *    
 *   Copyright (C) 2001-2017 by Diether Knof and Borg Enders
 *
 *   This program is free software; you can redistribute it and/or 
 *   modify it under the terms of the GNU General Public ScaledPixbuf as 
 *   published by the Free Software Foundation; either version 2 of 
 *   the ScaledPixbuf, or (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public ScaledPixbuf for more details. 
 *   You can find this license in the file 'gpl.txt'.
 *
 *   You should have received a copy of the GNU General Public ScaledPixbuf
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

#ifndef GTKMM_WIDGET_SCALED_PIXBUF_HEADER
#define GTKMM_WIDGET_SCALED_PIXBUF_HEADER

#include "../../../basistypes.h"
#include <gdkmm/pixbuf.h>

namespace Gdk {
/**
 ** @brief	a scaled pixmap
 **		scaled is only when needed
 **
 ** @author	Diether Knof
 **/
class ScaledPixbuf {
  public:
    ScaledPixbuf(string filename);
    ScaledPixbuf(Glib::RefPtr<Pixbuf> pixbuf, double scaling = 1);
    ScaledPixbuf() = default;
    ScaledPixbuf(ScaledPixbuf const& pixbuf_rotations);
    ScaledPixbuf& operator=(ScaledPixbuf const&);

    ~ScaledPixbuf() = default;

    operator Glib::RefPtr<Pixbuf> const&();
    Glib::RefPtr<Pixbuf> const& pixbuf();
    Glib::RefPtr<Pixbuf> const& orig_pixbuf() const;

    operator bool() const;

    ScaledPixbuf rotate(Gdk::PixbufRotation rotation) const;

    double get_scaling() const;
    void set_scaling(double scaling);
    void set_to_max_size(int width, int height);

    int get_width() const;
    int get_height() const;
    int get_orig_width() const;
    int get_orig_height() const;

  private:
    void scale();

  private:
    double scaling_ = 1;
    Glib::RefPtr<Pixbuf> orig_pixbuf_;
    Glib::RefPtr<Pixbuf> scaled_pixbuf_;

}; // class ScaledPixbuf

} // namespace Gdk

#endif // #ifdef GTKMM_WIDGET_SCALED_PIXBUF_HEADER

#endif // #ifdef USE_UI_GTKMM
