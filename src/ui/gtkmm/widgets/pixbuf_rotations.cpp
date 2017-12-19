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

#include "pixbuf_rotations.h"

namespace Gdk {
  
  /**
   ** constructor
   **
   ** @param	filename   the file to load the pixbuf from
   **
   ** @return	-
   **/
  PixbufRotations::PixbufRotations(string const& filename) :
    Glib::RefPtr<Pixbuf>(),
  pixbuf(4)
  {
#ifdef GLIBMM_EXCEPTIONS_ENABLED
    try {
      static_cast<Glib::RefPtr<Pixbuf>&>(*this)
	= Pixbuf::create_from_file(filename);
    } // try
    catch (Glib::FileError file_error) { }
    catch (Gdk::PixbufError pixbuf_error) { }
#else
    std::auto_ptr<Glib::Error> error;
    static_cast<Glib::RefPtr<Pixbuf>&>(*this) = Pixbuf::create_from_file(filename, error);
#endif

    this->pixbuf[0] = *this;

    return ;
  } // PixbufRotations::PixbufRotations(string filename)

  /**
   ** constructor
   **
   ** @param	pixbuf   the pixbuf for the 'up' rotation
   **
   ** @return	-
   **/
  PixbufRotations::PixbufRotations(Glib::RefPtr<Pixbuf> const pixbuf) :
    Glib::RefPtr<Pixbuf>(pixbuf),
  pixbuf(4)
  {
    this->pixbuf[0] = *this;

    return ;
  } // PixbufRotations::PixbufRotations(Glib::RefPtr<Pixbuf> pixbuf)

#if 0
  /**
   ** constructor
   **
   ** @param	-
   **
   ** @return	-
   **/
  PixbufRotations::PixbufRotations() :
    Glib::RefPtr<Pixbuf>(),
  pixbuf(4)
  {
    this->pixbuf[0] = *this;

    return ;
  } // PixbufRotations::PixbufRotations()

  /**
   ** constructor
   **
   ** @param	pixbuf_rotations	pixbuf_rotations to copy
   **
   ** @return	-
   **/
  PixbufRotations::PixbufRotations(PixbufRotations const& pixbuf_rotations) :
    Glib::RefPtr<Pixbuf>(),
  pixbuf(4)
  {
    if (pixbuf_rotations)
      static_cast<Glib::RefPtr<Gdk::Pixbuf>& >(*this)
	= pixbuf_rotations->copy();

    this->pixbuf[0] = *this;
    for (unsigned i = 0; i < 4; i++)
      if (pixbuf_rotations.pixbuf[i])
	this->pixbuf[i] = pixbuf_rotations.pixbuf[i]->copy();

    return ;
  } // PixbufRotations::PixbufRotations(PixbufRotations const& pixbuf_rotations)

  /**
   **
   ** copy operator
   **
   ** @param	pixbuf_rotations	pixbuf_rotations to copy
   **
   ** @return	copied element
   ** 
   ** @author	Diether Knof
   **
   ** @version	0.6.1
   **
   **/
  PixbufRotations&
    PixbufRotations::operator=(PixbufRotations const& pixbuf_rotations)
    {
      if (pixbuf_rotations)
	static_cast<Glib::RefPtr<Gdk::Pixbuf>& >(*this)
	  = pixbuf_rotations->copy();
      else
	static_cast<Glib::RefPtr<Gdk::Pixbuf>& >(*this).clear();

      this->pixbuf[0] = *this;
      for (unsigned i = 1; i < 4; i++)
	if (pixbuf_rotations.pixbuf[i])
	  this->pixbuf[i] = pixbuf_rotations.pixbuf[i]->copy();
	else
	  this->pixbuf[i].clear();

      return *this;
    } // PixbufRotations& PixbufRotations::operator=(PixbufRotations const& pixbuf_rotations) :
#endif

  /**
   ** destruktor
   **
   ** @param	-
   **
   ** @return	-
   **/
  PixbufRotations::~PixbufRotations()
  { }

  /**
   ** -> result
   ** the rotation is automatically created if it does not yet exists
   **
   ** @param	rotation	rotation
   **
   ** @return	the pixbuf with 'rotation'
   **/
  Glib::RefPtr<Pixbuf>
    PixbufRotations::operator[](Rotation const rotation)
    {
      if (!*this)
	return *this;
      return (!this->pixbuf[static_cast<int>(rotation)]
	      ? this->create_rotation(rotation)
	      : this->pixbuf[static_cast<int>(rotation)]);
    } // Glib::RefPtr<Pixbuf> PixbufRotations::operator[](Rotation rotation)

  /**
   ** -> result
   **
   ** @param	rotation	rotation (default: 'UP')
   **
   ** @return	the width of the pixbuf with 'rotation'
   **/
  int
    PixbufRotations::width(Rotation const rotation) const
    {
      switch(rotation) {
      case Rotation::UP:
      case Rotation::DOWN:
	return (*this)->get_width();
      case Rotation::LEFT:
      case Rotation::RIGHT:
	return (*this)->get_height();
      } // switch(rotation)

      return 0;
    } // int PixbufRotations::width(Rotation rotation) const

  /**
   ** -> result
   **
   ** @param	rotation	rotation (default: 'UP')
   **
   ** @return	the height of the pixbuf with 'rotation'
   **/
  int
    PixbufRotations::height(Rotation const rotation) const
    {
      switch(rotation) {
      case Rotation::UP:
      case Rotation::DOWN:
	return (*this)->get_height();
      case Rotation::LEFT:
      case Rotation::RIGHT:
	return (*this)->get_width();
      } // switch(rotation)

      return 0;
    } // int PixbufRotations::height(Rotation rotation) const

  /**
   ** creates the rotation 'rotation' of '*this'
   **
   ** @param	rotation	the rotation
   **
   ** @return	rotated pixbuf
   **/
  Glib::RefPtr<Pixbuf>
    PixbufRotations::create_rotation(Rotation const rotation)
    {
      switch(rotation) {
      case Rotation::UP:
	// cannot happen
	return *this;
      case Rotation::DOWN:
	return (this->pixbuf[static_cast<int>(rotation)] = this->pixbuf[0]->rotate_simple(Gdk::PIXBUF_ROTATE_UPSIDEDOWN));
      case Rotation::LEFT:
	return (this->pixbuf[static_cast<int>(rotation)] = this->pixbuf[0]->rotate_simple(Gdk::PIXBUF_ROTATE_COUNTERCLOCKWISE));
      case Rotation::RIGHT:
	return (this->pixbuf[static_cast<int>(rotation)] = this->pixbuf[0]->rotate_simple(Gdk::PIXBUF_ROTATE_CLOCKWISE));
      } // switch(rotation)

      return *this;
    } // Glib::RefPtr<Pixbuf> PixbufRotations::create_rotation(Rotation rotation)

} // namespace Gdk

#endif // #ifdef USE_UI_GTKMM
