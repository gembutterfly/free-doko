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

#ifdef USE_UI_GTKMM

#ifndef GTKMM_HELP_HEADER
#define GTKMM_HELP_HEADER

#include "base.h"
#include <gtkmm/messagedialog.h>

namespace UI_GTKMM_NS {

/**
 ** @brief	the about dialog
 **
 ** @author	Diether Knof
 **/
class Help : public Base, public Gtk::MessageDialog {
  public:
    // constructor
    Help(Base* const parent);
    // destructor
    ~Help();

    // show the homepage
    void show_homepage();
    // show the homepage for loading cardsets
    void show_cardsets_download();
    // show the manual
    void show_manual();

    // show the help at the location
    void show_help(string const& location);
    // show the internet page at the location
    void show_internet_page(string const& location);


    // update the language
    void language_update();

  private:
    // initialize the window
    void init();
    // sets the location
    void set_location(string const& location);

    // show the help at the online location
    void show_online_help();

  private:
    // the location
    string location = "";
    // whether 'online' is accepted
    bool online_accepted = false;

  private: // unused
    Help() = delete;
    Help(Help const&) = delete;
    Help& operator=(Help const&) = delete;
}; // class Help : public Base, public Gtk::StickyDialog

} // namespace UI_GTKMM_NS

#endif // #ifdef GTKMM_HELP_HEADER

#endif // #ifdef USE_UI_GTKMM
