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

#include "scaled_pixbuf_rotations.h"

namespace Gdk {

/**
 ** Constructor
 **
 ** @param	filename	the file to load the pixbuf from
 **
 ** @return	-
 **/
ScaledPixbufRotations::ScaledPixbufRotations(string const filename)
{
  this->scaled_pixbuf[0] = make_unique<ScaledPixbuf>(filename);
} // ScaledPixbufRotations::ScaledPixbufRotations(string filename)

/**
 ** Constructor
 **
 ** @param	pixbuf	the pixbuf for the 'up' rotation
 **
 ** @return	-
 **/
ScaledPixbufRotations::ScaledPixbufRotations(Glib::RefPtr<Pixbuf> const pixbuf)
{
  this->scaled_pixbuf[0] = make_unique<ScaledPixbuf>(pixbuf);
} // ScaledPixbufRotations::ScaledPixbufRotations(Glib::RefPtr<Pixbuf> const pixbuf)

/**
 ** Constructor
 **
 ** @param	-
 **
 ** @return	-
 **/
ScaledPixbufRotations::ScaledPixbufRotations()
{
  this->scaled_pixbuf[0] = make_unique<ScaledPixbuf>();
} // ScaledPixbufRotations::ScaledPixbufRotations()

/**
 ** Constructor
 **
 ** @param	scaled_pixbuf_rotations	scaled_pixbuf_rotations to copy
 **
 ** @return	-
 **/
ScaledPixbufRotations::ScaledPixbufRotations(ScaledPixbufRotations const& scaled_pixbuf_rotations)
{
  this->scaled_pixbuf[0]
    = make_unique<ScaledPixbuf>(*(scaled_pixbuf_rotations.scaled_pixbuf[0]));
} // ScaledPixbufRotations::ScaledPixbufRotations(ScaledPixbufRotations const& scaled_pixbuf_rotations)

/**
 ** copy operator
 **
 ** @param	scaled_pixbuf_rotations	scaled_pixbuf_rotations to copy
 **
 ** @return	copied element
 **/
ScaledPixbufRotations&
ScaledPixbufRotations::operator=(ScaledPixbufRotations const& scaled_pixbuf_rotations)
{
  for (size_t i = 0; i < 4; ++i)
    this->scaled_pixbuf[i].release();

  this->scaled_pixbuf[0]
    = make_unique<ScaledPixbuf>(*(scaled_pixbuf_rotations.scaled_pixbuf[0]));

  return *this;
} // ScaledPixbufRotations& ScaledPixbufRotations::operator=(ScaledPixbufRotations const& scaled_pixbuf_rotations) :

/**
 ** -> result
 ** the rotation is automatically created if it does not yet exists
 **
 ** @param	rotation	rotation
 **
 ** @return	the pixbuf with 'rotation'
 **/
ScaledPixbuf&
ScaledPixbufRotations::operator[](Rotation const rotation)
{
  DEBUG_ASSERTION(this->scaled_pixbuf[0],
                  "ScaledPixufRotations::operator[](rotation)\n"
                  "  no pixbuf set.\n");
  auto const i = static_cast<size_t>(rotation);
  return (  this->scaled_pixbuf[i]
          ? *this->scaled_pixbuf[i]
          : this->create_rotation(rotation));
} // ScaledPixbuf& ScaledPixbufRotations::operator[](Rotation rotation)

/**
 ** -> result
 **
 ** @param    -
 **
 ** @result   whether the pixbuf is set
 **/
ScaledPixbufRotations::operator bool() const
{
  if (!this->scaled_pixbuf[0])
    return false;
  return *this->scaled_pixbuf[0];
} // ScaledPixbuf::operator bool() const

/**
 ** set the scaling
 **
 ** @param	scaling		the new scaling of the pixbuf
 **
 ** @return	-
 **/
void
ScaledPixbufRotations::set_scaling(double const scaling) 
{
  this->scaled_pixbuf[0]->set_scaling(scaling);
  for (size_t i = 1; i < 4; ++i)
    this->scaled_pixbuf[i].release();

  return ;
} // void ScaledPixbufRotations::set_scaling(double scaling)

/**
 ** -> result
 **
 ** @param     rotation   rotation (default: 'UP')
 **
 ** @return    the width of the pixbuf with 'rotation'
 **/
int
ScaledPixbufRotations::get_width(Rotation const rotation) const
{
  switch(rotation) {
  case Rotation::UP:
  case Rotation::DOWN:
    return this->scaled_pixbuf[0]->get_width();
  case Rotation::LEFT:
  case Rotation::RIGHT:
    return this->scaled_pixbuf[0]->get_height();
  } // switch(rotation)

  return 0;
} // int ScaledPixbufRotations::get_width(Rotation rotation) const

/**
 ** -> result
 **
 ** @param     rotation   rotation (default: 'UP')
 **
 ** @return    the height of the pixbuf with 'rotation'
 **/
int
ScaledPixbufRotations::get_height(Rotation const rotation) const
{
  switch(rotation) {
  case Rotation::UP:
  case Rotation::DOWN:
    return this->scaled_pixbuf[0]->get_height();
  case Rotation::LEFT:
  case Rotation::RIGHT:
    return this->scaled_pixbuf[0]->get_width();
  } // switch(rotation)

  return 0;
} // int ScaledPixbufRotations::get_height(Rotation rotation) const

/**
 ** -> result
 **
 ** @param	rotation	rotation (default: 'UP')
 **
 ** @return	the original width of the pixbuf with 'rotation'
 **/
int
ScaledPixbufRotations::get_orig_width(Rotation const rotation) const
{
  switch(rotation) {
  case Rotation::UP:
  case Rotation::DOWN:
    return this->scaled_pixbuf[0]->get_orig_width();
  case Rotation::LEFT:
  case Rotation::RIGHT:
    return this->scaled_pixbuf[0]->get_orig_height();
  } // switch(rotation)

  return 0;
} // int ScaledPixbufRotations::get_orig_width(Rotation const rotation) const

/**
 ** -> result
 **
 ** @param	rotation	rotation (default: 'UP')
 **
 ** @return	the original height of the pixbuf with 'rotation'
 **/
int
ScaledPixbufRotations::get_orig_height(Rotation const rotation) const
{
  switch(rotation) {
  case Rotation::UP:
  case Rotation::DOWN:
    return this->scaled_pixbuf[0]->get_orig_height();
  case Rotation::LEFT:
  case Rotation::RIGHT:
    return this->scaled_pixbuf[0]->get_orig_width();
  } // switch(rotation)

  return 0;
} // int ScaledPixbufRotations::get_orig_height(Rotation const rotation) const

/**
 ** creates the rotation 'rotation' of '*this'
 **
 ** @param	rotation	the rotation
 **
 ** @return	rotated pixbuf
 **/
ScaledPixbuf&
ScaledPixbufRotations::create_rotation(Rotation const rotation)
{
  switch(rotation) {
  case Rotation::UP:
    // cannot happen
    return *this->scaled_pixbuf[0];
  case Rotation::DOWN:
    return *(this->scaled_pixbuf[1]
            = make_unique<ScaledPixbuf>(this->scaled_pixbuf[0]->rotate(Gdk::PIXBUF_ROTATE_UPSIDEDOWN )));
  case Rotation::LEFT:
    return *(this->scaled_pixbuf[2]
            = make_unique<ScaledPixbuf>(this->scaled_pixbuf[0]->rotate(Gdk::PIXBUF_ROTATE_COUNTERCLOCKWISE)));
  case Rotation::RIGHT:
    return *(this->scaled_pixbuf[3]
            = make_unique<ScaledPixbuf>(this->scaled_pixbuf[0]->rotate(Gdk::PIXBUF_ROTATE_CLOCKWISE)));
  } // switch(rotation)
  return *this->scaled_pixbuf[0];
} // ScaledPixbuf& ScaledPixbufRotations::create_rotation(Rotation const rotation)

} // namespace Gdk

#endif // #ifdef USE_UI_GTKMM
