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

#ifndef GTKMM_ERROR_HEADER
#define GTKMM_ERROR_HEADER

#include "base.h"

#include <gtkmm/messagedialog.h>

namespace UI_GTKMM_NS {

/**
 ** @brief	the error dialog
 **
 ** @author	Diether Knof
 **/
class Error : public Base, public Gtk::MessageDialog {
  public:
    Error(string const& message, Base* const parent);
    ~Error() = default;

    void create_bug_report();

  private:
    string const message;

  private: // unused
    Error() = delete;
    Error(Error const&) = delete;
    Error& operator=(Error const&) = delete;
}; // class Error : public Base, public Gtk::MessageDialog

} // namespace UI_GTKMM_NS

#endif // #ifdef GTKMM_ERROR_HEADER

#endif // #ifdef USE_UI_GTKMM
