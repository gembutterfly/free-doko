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

#ifndef GTKMM_WIDGET_LABEL_ENTRY_HEADER
#define GTKMM_WIDGET_LABEL_ENTRY_HEADER

#include <gtkmm/box.h>
#include <gtkmm/label.h>
#include <gtkmm/entry.h>
#include <gtkmm/alignment.h>

namespace Gtk {
  /**
   ** @brief	a spin button with a label
   **
   ** @author	Diether Knof
   **/
  class LabelEntry : public HBox {
    public:
      // constructor
      LabelEntry();
      // constructor
      LabelEntry(Glib::ustring const& text);

      virtual ~LabelEntry();


      // Label functions
      Label& get_label();
      void set_label(Glib::ustring const& text);


      // Entry functions
      Entry& get_entry();
      Glib::ustring get_text();
      virtual void set_text(Glib::ustring const& text);

    private:
      Label label_;
      Entry entry_;

    private: // unused
      LabelEntry(LabelEntry const&);
      LabelEntry& operator=(LabelEntry const&);
  }; // class LabelEntry : public HBox

} // namespace Gtk

#endif // #ifdef GTKMM_WIDGET_LABEL_ENTRY_HEADER

#endif // #ifdef USE_UI_GTKMM
