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

#ifndef GTKMM_MAIN_WINDOW_HEADER
#define GTKMM_MAIN_WINDOW_HEADER

#include "base.h"
#include <gtkmm/window.h>
namespace Gtk {
class Box;
} // class Gtk

namespace UI_GTKMM_NS {
class Menu;

/**
 ** @brief	the main window
 **/
class MainWindow : public Base, public Gtk::Window {
  public:
    // constructor
    MainWindow(Base* parent);
    // destructor
    ~MainWindow();

    // a new party shall be started
    void start_new_party_event();
    // the party shall be ended
    void end_party_event();
    // the program shall be quit
    void quit_program_event();
    // en error shall be generated
    void generate_error_event();

    // set the window to its minimal size
    void set_minimal_size();

    // a key press
    bool on_key_press_event(GdkEventKey* key);


  private:
    // the window is hidden
    void on_hide();
    // the window is shown
    void on_show();

    // this is called p.e. by iconfy/deiconify
    bool on_window_state_event(GdkEventWindowState* state);

  public:
    std::unique_ptr<Menu> menu;
    Gtk::Box* container = nullptr;

  private: // unused
    MainWindow() = delete;
    MainWindow(MainWindow const&) = delete;
    MainWindow& operator=(MainWindow const&) = delete;
}; // class MainWindow : public Base, public Gtk::Window

} // namespace UI_GTKMM_NS

#endif // #ifdef GTKMM_MAIN_WINDOW_HEADER

#endif // #ifdef USE_UI_GTKMM
