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

#include "main_window.h"
#include "menu.h"

#include "ui.h"
#include "cards.h"

#include "first_run.h"
#include "program_updated.h"
#include "bug_report.h"

#ifdef USE_NETWORK
#ifdef USE_NETWORK_DOKOLOUNGE
#include "dokolounge/lounge.h"
#include "dokolounge/chat.h"
#endif
#endif

#include "../../misc/translations.h"

#include <gtkmm/box.h>
#include <gtkmm/entry.h>
namespace UI_GTKMM_NS {

  /**
   ** constructor
   **
   ** @param     parent   parent object (the ui)
   **
   ** @return    -
   **/
  MainWindow::MainWindow(Base* const parent) :
    Base(parent),
    Window(),
    menu(std::make_unique<Menu>(this)),
    container(Gtk::manage(new Gtk::HBox(false, 0)))
  {
    this->set_icon(this->ui->icon);

    this->set_minimal_size();

    this->set_title(PROGRAM_NAME + string("   ")
                    + static_cast<string>(*::version));

    auto box = Gtk::manage(new Gtk::VBox(false, 0));
    this->add(*box);
    box->pack_start(*this->menu, false, true);

    box->add(*this->container);
    this->container->show();
    box->show();

#ifndef GLIBMM_DEFAULT_SIGNAL_HANDLERS_ENABLED
    this->signal_window_state_event().connect(sigc::mem_fun(*this, &MainWindow::on_window_state_event));
    this->signal_key_press_event().connect(sigc::mem_fun(*this, &MainWindow::on_key_press_event));
    this->signal_show().connect(sigc::mem_fun(*this, &MainWindow::on_show));
    this->signal_hide().connect(sigc::mem_fun(*this, &MainWindow::on_hide));
#endif
    this->signal_key_press_event().connect(sigc::mem_fun(*this, &MainWindow::on_key_press_event));

    this->ui->bug_report->set_dnd_destination(*this);

    return ;
  } // MainWindow::MainWindow(Base* const parent)

  /**
   ** destruktor
   **
   ** @param     -
   **
   ** @return    -
   **/
  MainWindow::~MainWindow()
  { }

  /**
   ** start a new party
   **
   ** @param     -
   **
   ** @return    -
   **/
  void
    MainWindow::start_new_party_event()
    {
      this->ui->start_new_party();

      return ;
    } // void MainWindow::start_new_party_event()

  /**
   ** end a new party
   **
   ** @param     -
   **
   ** @return    -
   **/
  void
    MainWindow::end_party_event()
    {
      this->ui->end_party();

      return ;
    } // void MainWindow::end_party_event()

  /**
   ** quits the program.
   ** 'UI_GTKMM' cannot be used by the signals sinc it is no Gtk object. So the MainWindow is used instead
   **
   ** @param     -
   **
   ** @return    -
   **/
  void
    MainWindow::quit_program_event()
    {
      this->ui->quit_program();

      return ;
    } // void MainWindow::quit_program_event()

  /**
   ** generates an error
   **
   ** @param     -
   **
   ** @return    -
   **/
  void
    MainWindow::generate_error_event()
    {
      this->ui->generate_error(::translation("BugReport::generated error").translation());

      return ;
    } // void MainWindow::generate_error_event()

  /**
   ** a key has been pressed
   **
   ** @param     key   the key
   **
   ** @return    from 'ui->key_press(key)'
   **/
  bool
    MainWindow::on_key_press_event(GdkEventKey* const key)
    {
#ifdef GLIBMM_DEFAULT_SIGNAL_HANDLERS_ENABLED
      if (this->Gtk::Window::on_key_press_event(key))
        return true;
#endif
      return this->ui->key_press(key);
    } // bool MainWindow::on_key_press_event(GdkEventKey* key)

  /**
   ** the window is shown -- make sure that the start windows stay on top
   **
   ** @param     -
   **
   ** @return    -
   **/
  void
    MainWindow::on_show()
    {
#ifdef GLIBMM_DEFAULT_SIGNAL_HANDLERS_ENABLED
      this->Gtk::Window::on_show();
#endif

      if (this->ui->first_run_window)
        this->ui->first_run_window->raise();
      if (this->ui->program_updated_window)
        this->ui->program_updated_window->raise();

      return ;
    } // void MainWindow::on_show()

  /**
   ** the window is hidden -- quit the program
   **
   ** @param     -
   **
   ** @return    -
   **/
  void
    MainWindow::on_hide()
    {
      this->ui->quit_program();

      return ;
    } // void MainWindow::on_hide()

  /**
   ** sets the minimal size of the window (depends on the size of the cards)
   **
   ** @param     -
   **
   ** @return    -
   **/
  void
    MainWindow::set_minimal_size()
    {
      Gdk::Geometry geometry;
      geometry.min_width = 5 * this->ui->cards->height();
      geometry.min_height = 5 * this->ui->cards->height();

      this->set_geometry_hints(*this, geometry, Gdk::HINT_MIN_SIZE);

      return ;
    } // void MainWindow::set_minimal_size()


  /**
   ** a change in the window state (p.e. iconify)
   ** track this event in order to iconify/deiconify all windows with the main
   ** window
   **
   ** @param     state    the (new) state
   **
   ** @return    false (let the Gtk::Window class handle the iconification)
   **/
  bool
    MainWindow::on_window_state_event(GdkEventWindowState* const state)
    {
      if (state->new_window_state & GDK_WINDOW_STATE_ICONIFIED)
        this->ui->iconify_all_windows();
      else
        this->ui->deiconify_all_windows();

      return false;
    } // bool MainWindow::on_window_state_event(GdkEventWindowState* state)

} // namespace UI_GTKMM_NS

#endif // #ifdef USE_UI_GTKMM
