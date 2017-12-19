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

#include "labelentry.h"

namespace Gtk {

  /**
   ** constructor
   **
   ** @param	-
   **
   ** @return	-
   **/
  LabelEntry::LabelEntry() :
    HBox(false),
    label_(),
    entry_()
  { 
    this->set_spacing(1 EX);

    auto alignment
      = Gtk::manage(new Gtk::Alignment(Gtk::ALIGN_CENTER, Gtk::ALIGN_CENTER,
                                       0, 0));
    alignment->add(this->get_entry());
    this->pack_start(*alignment, true, true);
  } // LabelEntry::LabelEntry()

  /**
   ** constructor
   **
   ** @param	label   text for the label
   **
   ** @return	-
   **/
  LabelEntry::LabelEntry(Glib::ustring const& text) :
    HBox(false),
    label_(text),
    entry_()
  { 
    this->set_spacing(1 EX);

    auto alignment
      = Gtk::manage(new Gtk::Alignment(Gtk::ALIGN_CENTER, Gtk::ALIGN_CENTER,
                                       0, 0));
    alignment->add(this->get_label());
    this->pack_start(*alignment, true, true);
    this->pack_end(this->get_entry(), false, true);
  } // LabelEntry::LabelEntry(Glib::ustring text)

  /**
   ** destructor
   **
   ** @param	-
   **
   ** @return	-
   **/
  LabelEntry::~LabelEntry()
  { }

  /**
   ** -> result
   **
   ** @param	-
   **
   ** @return	the label
   **/
  Label&
    LabelEntry::get_label()
    {
      return this->label_;
    }

  /**
   ** set the text for the label
   **
   ** @param	text   text for the label
   **
   ** @return	the label
   **/
  void
    LabelEntry::set_label(Glib::ustring const& text)
    {
      this->get_label().set_text(text);
      return ;
    }

  /**
   ** -> result
   **
   ** @param	-
   **
   ** @return	the entry
   **/
  Entry&
    LabelEntry::get_entry()
    {
      return this->entry_;
    }

  /**
   ** -> result
   **
   ** @param	-
   **
   ** @return	the text of the entry
   **/
  Glib::ustring
    LabelEntry::get_text()
    {
      return this->get_entry().get_text();
    }

  /**
   ** set the text for the entry
   **
   ** @param	text   text for the entry
   **
   ** @return	-
   **/
  void
    LabelEntry::set_text(Glib::ustring const& text)
    {
      this->get_entry().set_text(text);
      return ;
    }

} // namespace Gtk

#endif // #ifdef USE_UI_GTKMM
