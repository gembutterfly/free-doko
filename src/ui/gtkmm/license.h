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

#ifndef GTKMM_LICENSE_HEADER
#define GTKMM_LICENSE_HEADER

#include "base.h"
#include "widgets/sticky_dialog.h"
namespace Gtk {
class TextView;
class StockButton;
} // namespace Gtk

namespace UI_GTKMM_NS {

/**
 ** @brief	the license dialog
 **
 ** @author	Diether Knof
 **/
class License : public Base, public Gtk::StickyDialog {
  public:
    License(Base* const parent);
    ~License();

  private:
    void init();

  public:
    void license_update(); 

  private:
    Gtk::TextView* program_text = nullptr;
    Gtk::TextView* cardset_text = nullptr;
    Gtk::TextView* iconset_text = nullptr;
    Gtk::StockButton* close_button = nullptr;

  private: // unused
    License() = delete;
    License(License const&) = delete;
    License& operator=(License const&) = delete;
}; // class License : public Base, public Gtk::StickyDialog

} // namespace UI_GTKMM_NS

#endif // #ifdef GTKMM_LICENSE_HEADER
#endif // #ifdef USE_UI_GTKMM
