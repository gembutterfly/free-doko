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
 *********************************************************************/

#include "constants.h"

#ifdef USE_UI_GTKMM

#include <cstring>
#include "help.h"
#include "../help.h"

#include "ui.h"
#include "translations.h"

#include "widgets/stock_button.h"
#include <gtkmm/image.h>
#include <gtkmm/alignment.h>

namespace UI_GTKMM_NS {

  /**
   ** constructor
   **
   ** @param     parent   the parent object
   **
   ** @return    -
   **/
  Help::Help(Base* const parent) :
    Base(parent),
    Gtk::MessageDialog("help not found",
		       false,
		       Gtk::MESSAGE_WARNING, Gtk::BUTTONS_NONE,
		       false
		      )
  {
    this->ui->add_window(*this);
    this->init();

    return ;
  } // Help::Help(Base* const parent)

  /**
   ** destruktor
   **
   ** @param     -
   **
   ** @return    -
   **/
  Help::~Help()
  { }

  /**
   ** create all subelements
   **
   ** @param     -
   **
   ** @return    -
   **/
  void
    Help::init()
    {
      this->ui->translations->add(*this, ::translation("FreeDoko %ttitle%",
						       ::translation("~help")));
      this->set_icon(this->ui->icon);

      this->set_skip_taskbar_hint();

      { // buttons
	auto close_button
	  = Gtk::manage(new Gtk::StockButton(Gtk::Stock::CLOSE, "close"));
	this->ui->translations->add(*close_button,
				    ::translation("close"));
	this->add_action_widget(*close_button, Gtk::RESPONSE_CLOSE);
	close_button->show_all();

	auto online_button
	  = Gtk::manage(new Gtk::StockButton(Gtk::Stock::HELP, "online help"));
	this->ui->translations->add(*online_button,
				    ::translation("online help"));
	this->add_action_widget(*online_button, Gtk::RESPONSE_NONE);
	online_button->show_all();
	online_button->grab_default();

	{ // signals
	  close_button->signal_clicked().connect(sigc::mem_fun(*this, &Gtk::Widget::hide));
	  online_button->signal_clicked().connect(sigc::mem_fun(*this, &Help::show_online_help));
	} // signals
      } // buttons


      this->set_default_size(this->ui->logo->get_width(),
			     this->ui->logo->get_height() * 2);
#ifdef POSTPONED
      this->get_window()->set_decorations(Gdk::DECOR_BORDER
					  | Gdk::DECOR_RESIZEH
					  | Gdk::DECOR_TITLE
					  | Gdk::DECOR_MENU);
#endif

      { // the text
	this->language_update();
      } // the text

      this->show_all_children();

      return ;
    } // void Help::init()

  /**
   ** show the homepage
   **
   ** @param     -
   **
   ** @return    -
   **/
  void
    Help::show_homepage()
    {
      this->show_internet_page(::Help::homepage_location());
      return ;
    } // void Help::show_homepage()

  /**
   ** show the homepage
   **
   ** @param     -
   **
   ** @return    -
   **/
  void
    Help::show_cardsets_download()
    {
      this->show_internet_page(::Help::cardsets_download_location());
      return ;
    } // void Help::show_cardsets_download()

  /**
   ** show the manual
   **
   ** @param     -
   **
   ** @return    -
   **/
  void
    Help::show_manual()
    {
      this->show_help(::Help::manual_location());
      return ;
    } // void Help::show_manual()

  /**
   ** show the help at the location
   **
   ** @param     location   location of the help
   **
   ** @return    -
   **/
  void
    Help::show_help(string const& location)
    {
      if (   (location[0] == '/')
	  || (string(location, 0, strlen("./")) == "./")
	  || (string(location, 0, strlen("file://")) == "file://") ) {
	// local adress
	::Help::show(location);
	return ;
      }

      if (this->online_accepted) {
	::Help::show(location);
	return ;
      }

      this->set_location(location);

      this->present();

      return ;
    } // void Help::show_help(string location)

  /**
   ** show the internet page at the location
   **
   ** @param     location   internet address
   **
   ** @return    -
   **/
  void
    Help::show_internet_page(string const& location)
    {
      ::Help::show(location);
      return ;
    } // void Help::show_internet_page(string location)

  /**
   ** show the help at the online location
   **
   ** @param     -
   **
   ** @return    -
   **/
  void
    Help::show_online_help()
    {
      this->online_accepted = true;
      ::Help::show(this->location);
      this->hide();

      return ;
    } // void Help::show_online_help()

  /**
   ** sets the location
   **
   ** @param     location   online location of the help
   **
   ** @return    -
   **/
  void
    Help::set_location(string const& location)
    {
      this->location = location;
      this->language_update();

      return ;
    } // void Help::set_location(string location)

  /**
   ** update the language
   **
   ** @param     -
   **
   ** @return    -
   **/
  void
    Help::language_update()
    {
#if GTKMM_VERSION_GE(2,8)
      this->set_message(::translation("Help::not found").translation());
      this->set_secondary_text(::translation("Help::not found: %slocation%",
                                             location).translation());
#else
      this->set_message(::translation("Help::not found: %slocation%",
                                      location).translation(),
                        false);
#endif

      return ;
    } // void Help::language_update()

} // namespace UI_GTKMM_NS

#endif // #ifdef USE_UI_GTKMM
