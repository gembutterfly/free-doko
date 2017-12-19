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

#pragma once

#ifdef USE_UI_GTKMM

#include "base.h"
#include "widgets/sticky_dialog.h"
namespace Gtk {
class TextView;
class Entry;
} // namespace Gtk

namespace UI_GTKMM_NS {

/**
 ** @brief	the chat dialog
 ** Here a text buffer is used, since a treeview cannot wrap the lines :-(
 **
 ** @author	Diether Knof
 **
 ** @todo	save
 **/
class Chatter : public Base, public Gtk::StickyDialog {
  public:
    // how long an urgency hint is shown (in milisec)
    static unsigned const URGENCY_HINT_TIMEOUT;

  public:
    // constructor
    Chatter(Base* parent);
    // destructor
    ~Chatter();

    // add a chat message
    void chat(string const& player, string const& message);

  private:
    // init the chatter
    void init();
    // the tag of a player
    string tag(string const& player);
    // send the chat text over the network
    void chat_now();
    // stop the urgency hint
    bool stop_urgency_hint();

  private:
    Gtk::TextView* text = nullptr;
    Gtk::Entry* chat_line = nullptr;

  private: // unused
    Chatter() = delete;
    Chatter(Chatter const&) = delete;
    Chatter& operator=(Chatter const&) = delete;
}; // class Chatter : public Base, public Gtk::StickyDialog

} // namespace UI_GTKMM_NS

#endif // #ifdef USE_UI_GTKMM
