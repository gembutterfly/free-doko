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

#ifdef USE_UI_GTKMM

#ifndef GTKMM_WIDGET_LABEL_SCALE_SPIN_BUTTON_HEADER
#define GTKMM_WIDGET_LABEL_SCALE_SPIN_BUTTON_HEADER

#include "labelspinbutton.h"
#include <gtkmm/scale.h>

namespace Gtk {
  /**
   ** @brief	a spin button with a label
   **
   ** @author	Diether Knof
   **/
  class LabelScaleSpinButton : public LabelSpinButton {
    public:
      // constructor
      LabelScaleSpinButton();
      // constructor
      LabelScaleSpinButton(Glib::ustring const& label);

      // destructor
      virtual ~LabelScaleSpinButton();


      // SpinButton functions
      HScale& get_scale();

      virtual void set_range(double min, double max);

    private:
      HScale scale_;

    private: // unused
      LabelScaleSpinButton(LabelScaleSpinButton const&);
      LabelScaleSpinButton& operator=(LabelScaleSpinButton const&);
  }; // class LabelScaleSpinButton : public LabelSpinButton

} // namespace Gtk

#endif // #ifdef GTKMM_WIDGET_LABEL_SCALE_SPIN_BUTTON_HEADER

#endif // #ifdef USE_UI_GTKMM
