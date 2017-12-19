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

#include "constants.h"

#ifdef USE_UI_GTKMM

#include "rotations.h"

namespace Gdk {
  
  /**
   **
   ** rotates 'pixbuf' 180 degree
   **
   ** @param	pixbuf	pixbuf to rotate
   **
   ** @return	the rotated pixbuf
   ** 
   ** @author	Diether Knof
   **
   ** @version	0.6.1
   **
   **/
  Glib::RefPtr<Pixbuf>
    rotate_180(Glib::RefPtr<Pixbuf> const& pixbuf)
    {
      int const width = pixbuf->get_width();
      int const height = pixbuf->get_height();

      Glib::RefPtr<Gdk::Pixbuf> pixbuf_tmp
	= Gdk::Pixbuf::create(pixbuf->get_colorspace(),
			      pixbuf->get_has_alpha(),
			      pixbuf->get_bits_per_sample(),
			      width, height);

      for (int x = 0; x < width; ++x)
	pixbuf->copy_area(x, 0, 1, height,
			  pixbuf_tmp, width - 1 - x, 0);

      Glib::RefPtr<Gdk::Pixbuf> pixbuf_new
	= Gdk::Pixbuf::create(pixbuf->get_colorspace(),
			      pixbuf->get_has_alpha(),
			      pixbuf->get_bits_per_sample(),
			      width, height);

      for (int y = 0; y < height; ++y)
	pixbuf_tmp->copy_area(0, y, width, 1,
			      pixbuf_new, 0, height - 1 - y);

      return pixbuf_new;
    } // Glib::RefPtr<Pixbuf> rotate_180(Glib::RefPtr<Pixbuf> const pixbuf)

  /**
   **
   ** rotates 'pixbuf' to the left
   **
   ** @param	pixbuf	pixbuf to rotate
   **
   ** @return	the rotated pixbuf
   ** 
   ** @author	Diether Knof
   **
   ** @version	0.6.1
   **
   **/
  Glib::RefPtr<Pixbuf>
    rotate_left(Glib::RefPtr<Pixbuf> const& pixbuf)
    {
      int const width = pixbuf->get_width();
      int const height = pixbuf->get_height();

      Glib::RefPtr<Gdk::Pixbuf> pixbuf_new
	= Gdk::Pixbuf::create(pixbuf->get_colorspace(),
			      pixbuf->get_has_alpha(),
			      pixbuf->get_bits_per_sample(),
			      height, width);

      for (int x = 0; x < width; ++x)
	for (int y = 0; y < height; ++y)
	  pixbuf->copy_area(x, y, 1, 1,
			    pixbuf_new, y, width - 1 - x);

      return pixbuf_new;
    } // Glib::RefPtr<Pixbuf> rotate_left(Glib::RefPtr<Pixbuf> const pixbuf)

  /**
   **
   ** rotates 'pixbuf' to the right
   **
   ** @param	pixbuf	pixbuf to rotate
   **
   ** @return	the rotated pixbuf
   ** 
   ** @author	Diether Knof
   **
   ** @version	0.6.1
   **
   **/
  Glib::RefPtr<Pixbuf>
    rotate_right(Glib::RefPtr<Pixbuf> const& pixbuf)
    {
      int const width = pixbuf->get_width();
      int const height = pixbuf->get_height();

      Glib::RefPtr<Gdk::Pixbuf> pixbuf_new
	= Gdk::Pixbuf::create(pixbuf->get_colorspace(),
			      pixbuf->get_has_alpha(),
			      pixbuf->get_bits_per_sample(),
			      height, width);

      for (int x = 0; x < width; ++x)
	for (int y = 0; y < height; ++y)
	  pixbuf->copy_area(x, y, 1, 1,
			    pixbuf_new, height - 1 - y, x);

      return pixbuf_new;
    } // Glib::RefPtr<Pixbuf> rotate_right(Glib::RefPtr<Pixbuf> const pixbuf)

} // namespace Gdk

#endif // #ifdef USE_UI_GTKMM
