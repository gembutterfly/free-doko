/**********************************************************************
 *
 *   FreeDoko a Doppelkopf-Game
 *    
 *   Copyright (C) 2001-2017 by Diether Knof and Borg Enders
 *
 *   This program is free software; you can redistribute it and/or 
 *   modify it under the terms of the GNU General Public License as 
 *   published by the Free Software Foundation; either version 2 of 
 *   the License, or (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details. 
 *   You can find this license in the file 'gpl.txt'.
 *
 *   You should have received a copy of the GNU General Public License
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

#include "utils.h"

#include <gtkmm/togglebutton.h>
#include <gtkmm/alignment.h>
namespace UI_GTKMM_NS {

  /**
   ** -> result
   **
   ** @param     color   the color
   **
   ** @return    the name of the color
   **/
  string
    colorname(Gdk::Color const color)
  {
      ostringstream color_str;
      color_str << '#' << std::setfill('0')
	<< std::hex << setw(4) << color.get_red()
	<< std::hex << setw(4) << color.get_green()
	<< std::hex << setw(4) << color.get_blue()
	<< std::ends;

      return color_str.str();
  } // string colorname(Gdk::Colo color)

  /**
   ** -> result
   **
   ** @param     pixbuf   	the pixbuf that is to be rotated
   **
   ** @return    the rotated pixbuf
   **/
  Glib::RefPtr<Gdk::Bitmap>
    bitmap(Glib::RefPtr<Gdk::Pixbuf> const pixbuf)
    {
      Glib::RefPtr<Gdk::Pixmap> pixmap;
      Glib::RefPtr<Gdk::Bitmap> bitmap;

#ifdef OUTDATED
      // todo: need replacement
      pixbuf->render_pixmap_and_mask(pixmap, bitmap,
				     ALPHA_THRESHOLD);
#endif

      return bitmap;
    } // Glib::RefPtr<Gdk::Bitmap> bitmap(Glib::RefPtr<Gdk::Pixbuf> pixbuf)

  /**
   ** toggles whether 'widget' is to be shown, depending on the state of
   ** 'toggle_button'
   **
   ** @param     widget   	widget to toggle the view of
   ** @param     toggle_button   button with the state
   **
   ** @return    -
   **/
  void
    toggle_view(Gtk::Widget* const widget,
		Gtk::ToggleButton* const toggle_button)
    {
      if (toggle_button->get_active()
	  && toggle_button->is_visible())
	widget->show();
      else
	widget->hide();

      return ;
    } // void toggle_view(Gtk::Widget* const widget, Gtk::ToggleButton* const toggle_button)

  /**
   ** sets the signal so that 'widget' is to be shown, depending on the state of
   ** 'toggle_button'
   **
   ** @param     widget   	widget to toggle the view of
   ** @param     toggle_button   button with the state
   **
   ** @return    -
   **/
  void
    set_signal_toggle_view(Gtk::Widget* const widget,
			   Gtk::ToggleButton* const toggle_button)
    {
      toggle_button->signal_toggled().connect(sigc::bind<Gtk::Widget* const, Gtk::ToggleButton* const>(sigc::ptr_fun(&toggle_view), widget, toggle_button));

      toggle_button->signal_realize().connect(sigc::bind<Gtk::Widget* const, Gtk::ToggleButton* const>(sigc::ptr_fun(&toggle_view), widget, toggle_button));

      toggle_button->signal_show().connect(sigc::bind<Gtk::Widget* const, Gtk::ToggleButton* const>(sigc::ptr_fun(&toggle_view), widget, toggle_button));
      toggle_button->signal_hide().connect(sigc::bind<Gtk::Widget* const, Gtk::ToggleButton* const>(sigc::ptr_fun(&toggle_view), widget, toggle_button));

      toggle_button->signal_unmap().connect(sigc::bind<Gtk::Widget* const, Gtk::ToggleButton* const>(sigc::ptr_fun(&toggle_view), widget, toggle_button));

      return ;
    } // void set_signal_toggle_view(Gtk::Widget* const widget, Gtk::ToggleButton* const toggle_button)


  /**
   ** -> result
   **
   ** @param     widget   widget to indent
   ** @param     level    how much to indent (default: 1)
   **
   ** @return    alignment which conains the widget indented
   **/
  Gtk::Alignment*
    indent(Gtk::Widget& widget, unsigned const level)
  {
    auto alignment
      = Gtk::manage(new Gtk::Alignment(Gtk::ALIGN_START,
				       Gtk::ALIGN_CENTER,
				       0, 0));
    alignment->set_property("left_padding", level EM);
    alignment->add(widget);
    return alignment;
  } // Gtk::Alignment* indent(Gtk::Widget& widget, unsigned level = 1)

} // namespace UI_GTKMM_NS

#endif // #ifdef USE_UI_GTKMM
