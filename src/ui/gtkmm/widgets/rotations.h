/**********************************************************************
 *
 *   FreeDoko a Doppelkopf-Game
 *    
 *   Copyright (C) 2001-2017 by Diether Knof and Borg Enders
 *
 *   This program is free software; you can redistribute it and/or 
 *   modify it under the terms of the GNU General Public Settings as 
 *   published by the Free Software Foundation; either version 2 of 
 *   the Settings, or (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public Settings for more details. 
 *   You can find this license in the file 'gpl.txt'.
 *
 *   You should have received a copy of the GNU General Public Settings
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, 
 *   MA  02111-1307  USA
 *
 *  Contact:
 *    Diether Knof dknof@gmx.de
 *    Borg Enders  borg@borgsoft.de
 *
 *********************************************************************/

#ifdef USE_UI_GTKMM

#ifndef GTKMM_WIDGET_ROTATIONS_HEADER
#define GTKMM_WIDGET_ROTATIONS_HEADER

#include <gdkmm/pixbuf.h>

namespace Gdk {
  
  // turn the pixbuf upside down
  Glib::RefPtr<Pixbuf> rotate_180(Glib::RefPtr<Pixbuf> const& pixbuf);
  // rotate the pixbuf anti clockwise
  Glib::RefPtr<Pixbuf> rotate_left(Glib::RefPtr<Pixbuf> const& pixbuf);
  // rotate the pixbuf clockwise
  Glib::RefPtr<Pixbuf> rotate_right(Glib::RefPtr<Pixbuf> const& pixbuf);

} // namespace Gdk

#endif // #ifndef GTKMM_WIDGET_ROTATIONS_HEADER

#endif // #ifdef USE_UI_GTKMM
