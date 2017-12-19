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

#ifndef GTKMM_WIDGET_TYPE_SELECTOR_HEADER
#define GTKMM_WIDGET_TYPE_SELECTOR_HEADER

#include <gtkmm/comboboxtext.h>

namespace Gtk {
  /**
   ** @brief    a selector for types (p.e. 'Card')
   **
   ** @author   Diether Knof
   **/
  template<typename Type>
    class TypeSelector : public ComboBoxText {
      public:
        // constructor
        TypeSelector();
        // destructor
        ~TypeSelector();

        using Gtk::Container::add;
        // adds the type to the selector
        void add(Type const& type);

        // returns the selected type
        Type type();
        // set the selected type
        void set_type(Type const& type);

        // updates the text
        void language_update();

      private:
        // the types
        vector<Type> types;

      private: // unused
        TypeSelector(TypeSelector const&);
        TypeSelector& operator=(TypeSelector const&);
    }; // class TypeSelector : public ComboBoxText

} // namespace Gtk

#include "type_selector.hpp"

#endif // #ifdef GTKMM_WIDGET_TYPE_SELECTOR_HEADER

#endif // #ifdef USE_UI_GTKMM
