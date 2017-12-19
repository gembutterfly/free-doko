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

#include "scaled_pixbuf.h"
#ifdef WORKAROUND
#include "../../../utils/file.h"
#endif

namespace Gdk {

/**
 ** Constructor
 **
 ** @param	filename   the file to load the pixbuf from
 **
 ** @return	-
 **/
ScaledPixbuf::ScaledPixbuf(string const filename)
{
#ifdef GLIBMM_EXCEPTIONS_ENABLED
  try {
#ifdef WORKAROUND
    if (!DK::Utils::File::isfile(filename))
      throw Glib::FileError(Glib::FileError::NO_SUCH_ENTITY,
                            ("file " + filename + "not found").c_str());
#endif
    this->orig_pixbuf_ = Pixbuf::create_from_file(filename);
  } // try
  catch (Glib::FileError file_error) { }
  catch (Gdk::PixbufError pixbuf_error) { }
#else
  std::auto_ptr<Glib::Error> error;
  this->orig_pixbuf_ = Pixbuf::create_from_file(filename, error);
#endif

  return ;
} // ScaledPixbuf::ScaledPixbuf(string filename)

/**
 ** Constructor
 **
 ** @param	pixbuf	  the pixbuf for the 'up' rotation
 ** @param	scaling   the scaling, default: 1
 **
 ** @return	-
 **/
ScaledPixbuf::ScaledPixbuf(Glib::RefPtr<Pixbuf> const pixbuf,
                           double const scaling) :
  scaling_(scaling),
  orig_pixbuf_(pixbuf ? pixbuf->copy() : pixbuf)
{ }

/**
 ** Constructor
 **
 ** @param	scaled_pixbuf	scaled_pixbuf to copy
 **
 ** @return	-
 **/
ScaledPixbuf::ScaledPixbuf(ScaledPixbuf const& scaled_pixbuf) :
  ScaledPixbuf(scaled_pixbuf.orig_pixbuf_, scaled_pixbuf.scaling_)
{ }

/**
 **
 ** copy operator
 **
 ** @param	scaled_pixbuf	scaled_pixbuf to copy
 **
 ** @return	copied element
 **/
ScaledPixbuf&
ScaledPixbuf::operator=(ScaledPixbuf const& scaled_pixbuf)
{
  this->scaling_ = scaled_pixbuf.scaling_;
  if (scaled_pixbuf.orig_pixbuf_)
    this->orig_pixbuf_ = scaled_pixbuf.orig_pixbuf_->copy();
  else
    this->orig_pixbuf_.reset();
  this->scaled_pixbuf_.reset();

  return *this;
} // ScaledPixbuf& ScaledPixbuf::operator=(ScaledPixbuf scaled_pixbuf) :

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	the scaled pixbuf
 **/
ScaledPixbuf::operator Glib::RefPtr<Pixbuf> const&()
{
  return this->pixbuf();
} // ScaledPixbuf::operator Glib::RefPtr<Pixbuf>()

/**
 ** -> result
 ** the scaling is automatically created if it does not yet exists
 **
 ** @param	-
 **
 ** @return	the scaled pixbuf
 **/
Glib::RefPtr<Pixbuf> const&
ScaledPixbuf::pixbuf()
{
  if (!this->scaled_pixbuf_)
    this->scale();

  return this->scaled_pixbuf_;
} // Glib::RefPtr<Pixbuf> const& ScaledPixbuf::pixbuf()

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	the original pixbuf
 **/
Glib::RefPtr<Pixbuf> const&
ScaledPixbuf::orig_pixbuf() const
{
  return this->orig_pixbuf_;
} // Glib::RefPtr<Pixbuf> const& ScaledPixbuf::orig_pixbuf() const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	whether there is a pixbuf
 **/
ScaledPixbuf::operator bool() const
{
  return static_cast<bool>(this->orig_pixbuf_);
} // ScaledPixbuf::operator bool() const

/**
 ** -> result
 **
 ** @param	rotation   rotation
 **
 ** @return	a rotated pixbuf
 **/
ScaledPixbuf
ScaledPixbuf::rotate(Gdk::PixbufRotation const rotation) const
{
  if (!this->orig_pixbuf())
    return *this;
  return ScaledPixbuf(this->orig_pixbuf()->rotate_simple(rotation),
                      this->get_scaling());
} // ScaledPixbuf rotate(Gdk::PixbufRotation rotation) const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	the scaling
 **/
double
ScaledPixbuf::get_scaling() const
{
  return this->scaling_;
} // double ScaledPixbuf::get_scaling() const

/**
 ** set the scaling
 **
 ** @param	scaling		the new scaling of the pixbuf
 **
 ** @return	-
 **/
void
ScaledPixbuf::set_scaling(double const scaling) 
{
  this->scaling_ = scaling;
  this->scaled_pixbuf_.clear();

  return ;
} // void ScaledPixbuf::set_scaling(double scaling)

/**
 ** set the scaling such that at max width x height place is needed
 **
 ** @param	width    maximum width
 ** @param	height   maximum height
 **
 ** @return	-
 **/
void
ScaledPixbuf::set_to_max_size(int const width, int const height)
{
  if (   (this->get_orig_width() == 0)
      || (this->get_orig_height() == 0))
    return;
  this->set_scaling(min(1.0,
                        min(width / static_cast<double>(this->get_orig_width()),
                            height / static_cast<double>(this->get_orig_height())
                           )));
  return;
} // void ScaledPixbuf::set_to_max_size(int width, int height)

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	the width of the scaled pixbuf
 **/
int
ScaledPixbuf::get_width() const
{
  return (!this->scaled_pixbuf_
          ? static_cast<int>(this->get_orig_width() * this->get_scaling())
          : this->scaled_pixbuf_->get_width());
} // int ScaledPixbuf::get_width() const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	the width of the scaled pixbuf
 **/
int
ScaledPixbuf::get_height() const
{
  return (!this->scaled_pixbuf_
          ? static_cast<int>(this->get_orig_height() * this->scaling_)
          : this->scaled_pixbuf_->get_height());
} // int ScaledPixbuf::get_height() const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	the width of the original pixbuf
 **/
int
ScaledPixbuf::get_orig_width() const
{
  return (this->orig_pixbuf_ ? this->orig_pixbuf_->get_width() : 0);
} // int ScaledPixbuf::get_orig_width() const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	the height of the original pixbuf
 **/
int
ScaledPixbuf::get_orig_height() const
{
  return (this->orig_pixbuf_ ? this->orig_pixbuf_->get_height() : 0);
} // int ScaledPixbuf::get_orig_height() const

/**
 ** make the scaling
 **
 ** @param	-
 **
 ** @return	-
 **/
void
ScaledPixbuf::scale() 
{
  if (!this->orig_pixbuf_)
    return;

  if (this->scaling_ == 1)
    this->scaled_pixbuf_ = this->orig_pixbuf_->copy();
  else
    this->scaled_pixbuf_
      = this->orig_pixbuf_->scale_simple(static_cast<int>(this->get_orig_width()
                                                          * this->scaling_),
                                         static_cast<int>(this->get_orig_height()
                                                          * this->scaling_),
                                         Gdk::INTERP_TILES);
  // If you do not like the scaled images, try the following:
  //				     Gdk::INTERP_NEAREST);
  //				     Gdk::INTERP_BILINEAR);
  //				     Gdk::INTERP_HYPER);

  return ;
} // void ScaledPixbuf::scale()

} // namespace Gdk

#endif // #ifdef USE_UI_GTKMM
