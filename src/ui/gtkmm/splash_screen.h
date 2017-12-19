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

#ifndef GTKMM_SPLASH_SCREEN_HEADER
#define GTKMM_SPLASH_SCREEN_HEADER

#include "base.h"

#include "../../misc/translation.h"

#include <gtkmm/window.h>
namespace Gtk {
class Image;
class ProgressBar;
} // namespace Gtk

namespace UI_GTKMM_NS {

/**
 ** @brief	the splash screen
 **/
class SplashScreen : public Base, public Gtk::Window {
  friend class UI_GTKMM;
  public:
  // constructor
  SplashScreen(Base* parent);
  // destructor
  ~SplashScreen();

  // the status message has changed
  void status_message_changed(Translator::Translation const& status_message);
  // the progress has changed
  void progress_changed(double progress_max);

  private:
  // initialize all elements
  void init();

  private:
  Gtk::Image* image = nullptr;
  Gtk::ProgressBar* progress_bar = nullptr;

  private: // unused
  SplashScreen() = delete;
  SplashScreen(SplashScreen const&) = delete;
  SplashScreen& operator=(SplashScreen const&) = delete;
}; // class SplashScreen : public Base, public Gtk::Window

} // namespace UI_GTKMM_NS

#endif // #ifdef GTKMM_SPLASH_SCREEN_HEADER

#endif // #ifdef USE_UI_GTKMM
