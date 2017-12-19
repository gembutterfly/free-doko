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

#include "sticky_dialog.h"
#include "../main_window.h"
#include "../ui.h"

#include <gdk/gdkkeysyms.h>
#include "stock_button.h"

namespace Gtk {

/**
 ** constructor
 **
 ** @param	title	the title of the window
 ** @param	modal	whether the window is modal
 **			(default: false)
 **
 ** @return	-
 **/
StickyDialog::StickyDialog(Glib::ustring const& title,
                           bool const modal) :
  Dialog(title, modal)
{
#ifndef GLIBMM_DEFAULT_SIGNAL_HANDLERS_ENABLED
  this->signal_show().connect(sigc::mem_fun(*this, &StickyDialog::on_show));
  this->signal_hide().connect(sigc::mem_fun(*this, &StickyDialog::on_hide));
  this->signal_key_press_event().connect(sigc::mem_fun(*this, &StickyDialog::on_key_press_event));
#endif
  return ;
} // StickyDialog::StickyDialog(Glib::ustring title, bool modal) :

/**
 ** constructor
 **
 ** @param	title		the title of the window
 ** @param	main_window	the main window (transient)
 ** @param	modal		whether the window is modal
 **				(default: false)
 **
 ** @return	-
 **/
StickyDialog::StickyDialog(Glib::ustring const& title,
                           UI_GTKMM_NS::MainWindow& main_window,
                           bool const modal) :
  Dialog(title, main_window, modal),
  main_window(&main_window)
{
  this->set_transient_for(main_window);
  return ;
} // StickyDialog::StickyDialog(Glib::ustring title, UI_GTKMM_NS::MainWindow& main_window, bool modal) :

/**
 ** event: the window is hidden
 ** remember the position of the window
 **
 ** @param	-
 **
 ** @return	-
 **/
void
StickyDialog::on_hide()
{
  this->get_position(this->x, this->y);
#ifdef GLIBMM_DEFAULT_SIGNAL_HANDLERS_ENABLED
  this->Dialog::on_hide();
#endif
  return ;
} // void on_hide()

/**
 ** event: the window is shown
 ** move the window to the remembered position
 **
 ** @param	-
 **
 ** @return	-
 **/
void
StickyDialog::on_show()
{
  if (this->x != INT_MAX)
    this->move(this->x, this->y);
#ifdef GLIBMM_DEFAULT_SIGNAL_HANDLERS_ENABLED
  this->Dialog::on_show();
#endif
  return ;
} // void on_show()

/**
 ** event: a key has been pressed
 **
 ** @param	key	pressed key
 **
 ** @return	whether the key was accepted
 **/
bool
StickyDialog::on_key_press_event(GdkEventKey* key)
{
  // key commands
  //   Escape: close the window

  if (key->state == 0) {
    switch (key->keyval) {
    case GDK_KEY_Escape:
      this->hide();
      return true;
    } // switch (key->keyval)
  } // if (key->state == 0)

#ifdef GLIBMM_DEFAULT_SIGNAL_HANDLERS_ENABLED
  return (   this->Gtk::Dialog::on_key_press_event(key)
          || (this->main_window
              ? this->main_window->on_key_press_event(key)
              : false) );
#else
  return (this->main_window
          ? this->main_window->on_key_press_event(key)
          : false);
#endif
} // bool StickyDialog::on_key_press_event(GdkEventKey* key)

} // namespace Gtk

#endif // #ifdef USE_UI_GTKMM
