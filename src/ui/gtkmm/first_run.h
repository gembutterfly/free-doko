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

#ifndef GTKMM_FIRST_RUN_HEADER
#define GTKMM_FIRST_RUN_HEADER

#include "base.h"

#include "../../misc/translation.h"
#include <gtkmm/dialog.h>
#include <gtkmm/textbuffer.h>

namespace UI_GTKMM_NS {

/**
 ** @brief	a dialog only shown on the first run of the program
 **
 ** @author	Diether Knof
 **
 ** @todo	when the program first starts let it stay on top (works),
 **		but when the user selects another window do not raise it again
 **/
class FirstRun : public Base, public Gtk::Dialog {
  public:
    FirstRun(Base* parent, Translator::Translation const& message);
    ~FirstRun() = default;

    void language_update();

  private:
    Translator::Translation const message;

    Glib::RefPtr<Gtk::TextBuffer> text_buffer;

  private: // unused
    FirstRun() = delete;
    FirstRun(FirstRun const&) = delete;
    FirstRun& operator=(FirstRun const&) = delete;
}; // class FirstRun : public Base, public Gtk::Dialog

} // namespace UI_GTKMM_NS

#endif // #ifdef GTKMM_FIRST_RUN_HEADER

#endif // #ifdef USE_UI_GTKMM
