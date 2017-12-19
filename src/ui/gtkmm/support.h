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

#ifndef GTKMM_SUPPORT_HEADER
#define GTKMM_SUPPORT_HEADER

#include "base.h"
#include "widgets/sticky_dialog.h"
namespace Gtk {
class TextView;
} // namespace Gtk

namespace UI_GTKMM_NS {

/**
 ** @brief	the support dialog
 **
 ** @author	Diether Knof
 **/
class Support : public Base, public Gtk::StickyDialog {
  public:
    Support(Base* const parent);
    ~Support();

  private:
    void init();

  private:
    Gtk::TextView* text = nullptr;

  private: // unused
    Support() = delete;
    Support(Support const&) = delete;
    Support& operator=(Support const&) = delete;
}; // class Support : public Base, public Gtk::StickyDialog

} // namespace UI_GTKMM_NS

#endif // #ifdef GTKMM_SUPPORT_HEADER
#endif // #ifdef USE_UI_GTKMM
