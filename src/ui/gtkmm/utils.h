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

#ifndef HEADER_UTILS_CONSTANTS
#define HEADER_UTILS_CONSTANTS

#include "../../basistypes.h"

#include <gdkmm/pixbuf.h>
#include <gdkmm/color.h>
namespace Gtk {
  class Widget;
  class ToggleButton;
  class Alignment;
}
namespace Gdk {
using Bitmap = Pixbuf;
using Pixmap = Pixbuf;
}
namespace UI_GTKMM_NS {
  // returns the name of the color
  string colorname(Gdk::Color color);

#ifdef OUTDATED
  // loads a pixbuf from the file and rotates it
  vector<Glib::RefPtr<Gdk::Pixbuf> > load_pixbuf_with_rotations(string const& filename);
#endif // #ifdef OUTDATED

  // returns the mask of the bitmap
  Glib::RefPtr<Gdk::Bitmap> bitmap(Glib::RefPtr<Gdk::Pixbuf> pixbuf);

  // toggle the view of 'widget' depending on the state of 'toggle_button'
  void toggle_view(Gtk::Widget* widget,
		   Gtk::ToggleButton* toggle_button);
  // sets the signal for 'toggle view'
  void set_signal_toggle_view(Gtk::Widget* widget,
			      Gtk::ToggleButton* toggle_button);

  // return the widget packed in an alignment wich indents the widget
  Gtk::Alignment* indent(Gtk::Widget& widget, unsigned level = 1);
} // namespace UI_GTKMM_NS

#endif // #ifndef HEADER_UTILS_CONSTANTS

#endif // #ifdef USE_UI_GTKMM
