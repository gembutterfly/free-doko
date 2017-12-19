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

#ifndef GTKMM_ABOUT_HEADER
#define GTKMM_ABOUT_HEADER

#include "base.h"
#include "widgets/sticky_dialog.h"
namespace Gtk {
class TextView;
} // namespace Gtk

namespace UI_GTKMM_NS {

/**
 ** @brief	the about dialog
 **
 ** @author	Diether Knof
 **/
class About : public Base, public Gtk::StickyDialog {
  public:
    About(Base* parent);
    ~About() = default;

  private:
    void init();

  private:
    Gtk::TextView* text = nullptr;

  private: // unused
    About() = delete;
    About(About const&) = delete;
    About& operator=(About const&) = delete;
}; // class About : public Base, public Gtk::StickyDialog

} // namespace UI_GTKMM_NS

#endif // #ifdef GTKMM_ABOUT_HEADER

#endif // #ifdef USE_UI_GTKMM
