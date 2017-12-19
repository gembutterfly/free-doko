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

#include "stock_button.h"

#include <gtkmm/image.h>
#include <gtkmm/label.h>
#include <gtkmm/box.h>
#include <gtkmm/alignment.h>

namespace Gtk {

  /**
   ** Constructor
   **
   ** @param	label	the label for the button
   **
   ** @return	-
   **/
  StockButton::StockButton(Glib::ustring const& label) :
    Button(),
    label(Gtk::manage(new Gtk::Label(label)))
  { 
    this->add(*this->label);

    this->show_all_children();
    this->set_can_default();

    return ;
  } // StockButton::StockButton(Glib::ustring const& label)

  /**
   ** Constructor
   **
   ** @param	stock_id	the stock id for the image
   ** @param	label		the label for the button
   ** @param	align_center	whether to align the image in the center
   **				(default: true)
   **
   ** @return	-
   **/
  StockButton::StockButton(Gtk::StockID const stock_id,
                           Glib::ustring const& label,
                           bool const align_center) :
    Button(),
    image(Gtk::manage(new Gtk::Image(stock_id,
                                     Gtk::IconSize(Gtk::ICON_SIZE_BUTTON)))),
    label(Gtk::manage(new Gtk::Label(label)))
  { 
    // the code is taken from 'gtk+-2.2.4', file 'gtkbutton.c',
    // function 'gtk_button_construct_child (GtkButton *button)'
    auto hbox = Gtk::manage(new Gtk::HBox(false, 0));
    // here I take a relative space (one ' '), not the absolute as in the source
    hbox->pack_start(*(Gtk::manage(new Gtk::Label(" "))), Gtk::PACK_SHRINK);
    hbox->pack_start(*this->image, Gtk::PACK_SHRINK);
    // here I take a relative space (one ' '), not the absolute as in the source
    hbox->pack_start(*(Gtk::manage(new Gtk::Label(" "))), Gtk::PACK_SHRINK);
    hbox->pack_end(*(Gtk::manage(new Gtk::Label(" "))), Gtk::PACK_SHRINK);
    if (align_center) {
      hbox->pack_end(*this->label);

      Gtk::Alignment* alignment
        = Gtk::manage(new Gtk::Alignment(Gtk::ALIGN_CENTER, Gtk::ALIGN_CENTER,
                                         0, 0));
      alignment->add(*hbox);

      this->add(*alignment);
    } else { // if ! (align_center)
      Gtk::Alignment* alignment
        = Gtk::manage(new Gtk::Alignment(Gtk::ALIGN_CENTER, Gtk::ALIGN_CENTER,
                                         1, 0));
      alignment->add(*(this->label));

      hbox->pack_end(*alignment);

      this->add(*hbox);
    } // if ! (align_center)

    this->show_all_children();
    this->set_can_default();

    return ;
  } // StockButton::StockButton(Gtk::StockID const& stock_id, Glib::ustring const& label, bool const align_center = true)

  /**
   ** Destructor
   **
   ** @param	-
   **
   ** @return	-
   **/
  StockButton::~StockButton()
  { }

  /**
   ** sets the label
   ** Note: this does _not_ overwrite Gtk::Button::set_label()
   **
   ** @param	label	new label
   **
   ** @return	-
   **/
  void
    StockButton::set_label(Glib::ustring const& label)
    {
      this->label->set_text(label);

      return ;
    } // void StockButton::set_label(Glib::ustring const& label)

} // namespace Gtk

#endif // #ifdef USE_UI_GTKMM
