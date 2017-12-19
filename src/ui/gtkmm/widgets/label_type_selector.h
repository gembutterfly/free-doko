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

#ifndef GTKMM_WIDGET_LABEL_TYPE_SELECTOR_HEADER
#define GTKMM_WIDGET_LABEL_TYPE_SELECTOR_HEADER

#include "type_selector.h"

#include <gtkmm/box.h>
#include <gtkmm/label.h>
#include <gtkmm/alignment.h>

namespace Gtk {
  /**
   ** @brief    a selector for types with a label
   **
   ** @author   Diether Knof
   **/
  template<typename Type>
    class LabelTypeSelector : public Gtk::HBox {
      public:
        // constructor
        LabelTypeSelector() :
          label_(),
          selector_()
      { this->pack_end(this->get_selector()); }
        // constructor
        LabelTypeSelector(Glib::ustring const& label) :
          label_(label),
          selector_()
      { this->init(); }
        // destructor
        ~LabelTypeSelector()
        { }

        // the label
        Label& get_label()
        { return this->label_; }
        // the type selector
        TypeSelector<Type>& get_selector()
        { return this->selector_; }

        using Gtk::Container::add;
        // adds the type to the selector
        void add(Type const& type)
        { this->get_selector().add(type); }

        // returns the selected type
        Type type()
        { return this->get_selector().type(); }
        // set the selected type
        void set_type(Type const& type)
        { this->get_selector().set_type(type); }

        // updates the text
        void language_update()
        { this->get_selector().language_update(); }

        Glib::SignalProxy0<void> signal_value_changed()
        { return this->get_selector().signal_changed(); }

      private:
        // inits the label type selector
        void init()
        {
          this->set_spacing(1 EX);

          auto alignment
            = Gtk::manage(new Gtk::Alignment(Gtk::ALIGN_START, Gtk::ALIGN_CENTER,
                                             0, 0));
          alignment->add(this->get_label());
          this->pack_start(*alignment, true, true);
          this->pack_end(this->get_selector(), false, true);
        }
      private:
        // the label with the description
        Label label_;
        // the menu with the types
        TypeSelector<Type> selector_;

      private: // unused
        LabelTypeSelector(LabelTypeSelector const&);
        LabelTypeSelector& operator=(LabelTypeSelector const&);
    }; // class LabelTypeSelector : public HBox

} // namespace Gtk

#endif // #ifdef GTKMM_WIDGET_LABEL_TYPE_SELECTOR_HEADER

#endif // #ifdef USE_UI_GTKMM
