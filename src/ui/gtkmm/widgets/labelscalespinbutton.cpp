/**********************************************************************
 *
 *   FreeDoko a Doppelkopf-Game
 *    
 *   Copyright (C) 2001-2017 by Diether Knof and Borg Enders
 *
 *   This program is free software; you can redistribute it and/or 
 *   modify it under the terms of the GNU General Public Rules as 
 *   published by the Free Software Foundation; either version 2 of 
 *   the Rules, or (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public Rules for more details. 
 *   You can find this license in the file 'gpl.txt'.
 *
 *   You should have received a copy of the GNU General Public Rules
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, 
 *   MA  02111-1307  USA
 *
 *  Contact:
 *    Diether Knof dknof@gmx.de
 *    Borg Enders  borg@borgsoft.de
 *
 **********************************************************************/

#include "constants.h"

#ifdef USE_UI_GTKMM

#include "labelscalespinbutton.h"

namespace Gtk {

  /**
   ** constructor
   **
   ** @param	-
   **
   ** @return	-
   **/
  LabelScaleSpinButton::LabelScaleSpinButton() :
    LabelSpinButton(),
    scale_()
  { 
    this->get_scale().set_adjustment(this->get_spin_button().get_adjustment());
    this->get_scale().set_digits(0);
    this->get_scale().set_draw_value(false);
    this->remove(**(this->get_children().begin()));
    this->pack_end(this->get_scale(), true, true);
    this->pack_start(this->get_spin_button(), false, true);
  } // LabelScaleSpinButton::LabelScaleSpinButton()

  /**
   ** constructor
   **
   ** @param	label    name for the label
   **
   ** @return	-
   **/
  LabelScaleSpinButton::LabelScaleSpinButton(Glib::ustring const& label) :
    LabelSpinButton(label),
    scale_()
  { 
    this->get_scale().set_adjustment(this->get_spin_button().get_adjustment());
    this->get_scale().set_digits(0);
    this->get_scale().set_draw_value(false);
    this->remove(this->get_spin_button());
    this->pack_end(this->get_scale(), false, true);
    this->pack_end(this->get_spin_button(), false, true);
  } // LabelScaleSpinButton::LabelScaleSpinButton(Glib::ustring label)

  /**
   ** destructor
   **
   ** @param	-
   **
   ** @return	-
   **/
  LabelScaleSpinButton::~LabelScaleSpinButton()
  { }

  /**
   ** -> result
   **
   ** @param	-
   **
   ** @return	the scale
   **/
  HScale&
    LabelScaleSpinButton::get_scale()
    {
      return this->scale_;
    }

  /**
   ** set the range
   **
   ** @param	min    minimum value
   ** @param	max    maximum value
   **
   ** @return	-
   **/
  void
    LabelScaleSpinButton::set_range(double const min, double const max)
    { 
      this->scale_.set_range(min, max);
      this->scale_.set_size_request(static_cast<int>((max - min) * 10));
      this->LabelSpinButton::set_range(min, max);
      return ;
    } // void LabelScaleSpinButton::set_range(double min, double max)

} // namespace Gtk

#endif // #ifdef USE_UI_GTKMM
