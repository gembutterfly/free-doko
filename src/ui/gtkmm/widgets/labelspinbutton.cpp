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

#include "labelspinbutton.h"

namespace Gtk {

  /**
   ** constructor
   **
   ** @param	-
   **
   ** @return	-
   **/
  LabelSpinButton::LabelSpinButton() :
    HBox(false),
    label_(),
    spin_button_()
  { 
    this->get_spin_button().set_numeric(true);
    this->set_spacing(10);
    this->get_spin_button().set_increments(1, 10);

    Gtk::Alignment* alignment
      = Gtk::manage(new Gtk::Alignment(Gtk::ALIGN_CENTER, Gtk::ALIGN_CENTER,
                                       0, 0));
    alignment->add(this->get_spin_button());
    this->pack_start(*alignment, true, true);
  } // LabelSpinButton::LabelSpinButton()

  /**
   ** constructor
   **
   ** @param	label    name for the label
   **
   ** @return	-
   **/
  LabelSpinButton::LabelSpinButton(Glib::ustring const& label) :
    HBox(false),
    label_(label),
    spin_button_()
  { 
    this->get_spin_button().set_numeric(true);
    this->set_spacing(10);
    this->get_spin_button().set_increments(1, 10);

    Gtk::Alignment* alignment
      = Gtk::manage(new Gtk::Alignment(Gtk::ALIGN_START, Gtk::ALIGN_CENTER,
                                       0, 0));
    alignment->add(this->get_label());
    this->pack_start(*alignment, true, true);
    this->pack_end(this->get_spin_button(), false, true);
  } // LabelSpinButton::LabelSpinButton(Glib::ustring label);

  /**
   ** destructor
   **
   ** @param	-
   **
   ** @return	-
   **/
  LabelSpinButton::~LabelSpinButton()
  { }

  /**
   ** -> result
   **
   ** @param	-
   **
   ** @return	the label
   **/
  Label&
    LabelSpinButton::get_label()
    {
      return this->label_;
    }

  /**
   ** set the label
   **
   ** @param	str   the string for the label
   **
   ** @return	-
   **/
  void
    LabelSpinButton::set_label(Glib::ustring const& str)
    {
      this->get_label().set_text(str);
      return ;
    } // void LabelSpinButton::set_label(Glib::ustring str)

  /**
   ** -> result
   **
   ** @param	-
   **
   ** @return	the label
   **/
  SpinButton&
    LabelSpinButton::get_spin_button()
    {
      return this->spin_button_;
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
    LabelSpinButton::set_range(double const min, double const max)
    { 
      this->get_spin_button().set_range(min, max);
      return ;
    } // void LabelSpinButton::set_range(double min, double max)

  /**
   ** -> result
   **
   ** @param	-
   **
   ** @return	the value
   **/
  double
    LabelSpinButton::get_value()
    {
      return this->get_spin_button().get_value();
    }

  /**
   ** -> result
   **
   ** @param	-
   **
   ** @return	the value
   **/
  int
    LabelSpinButton::get_value_as_int()
    {
      return this->get_spin_button().get_value_as_int();
    }

  /**
   ** set the value
   **
   ** @param	value    new value
   **
   ** @return	-
   **/
  void
    LabelSpinButton::set_value(double const value)
    {
      return this->get_spin_button().set_value(value);
    }

  /**
   ** -> result
   **
   ** @param	-
   **
   ** @return	the signal for value changes
   **/
  Glib::SignalProxy0<void>
    LabelSpinButton::signal_value_changed()
    {
      return this->get_spin_button().signal_value_changed();
    }

} // namespace Gtk

#endif // #ifdef USE_UI_GTKMM
