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
#ifdef USE_NETWORK
#ifdef USE_NETWORK_DOKOLOUNGE

#ifndef GTKMM_DOKOLOUNGE_CHAT_HEADER
#define GTKMM_DOKOLOUNGE_CHAT_HEADER

#include "lounge.h"
#include <gtkmm/frame.h>

#include <map>
using std::map;

namespace Gtk {
  class TextView;
  class Entry;
}; // namespace Gtk

namespace UI_GTKMM_NS {
  namespace DokoLounge {
    class Chat : public Base, public Gtk::Frame {
      public:
        // constructor
        Chat(Lounge* const parent);
        // destructor
        ~Chat();

        // the corresponding lounge
        Lounge& lounge();

        // a chat entry has been added
        void entry_added(::LoungeChatEntry const& entry);

        // update the chat colors
        void update_chat_colors();

      private:
        // initialize the Chat
        void init();
        // chat signal
        void chat_signal();

      public: // for MainWindow in order to pass the key signal through
        Gtk::TextView* chat_messages;
        Gtk::Entry* chat_line;

      private: // unused
        Chat();
        Chat(Chat const&);
        Chat& operator=(Chat const&);
    }; // class Chat : public Base, publick Gtk::Frame
  } // namespace DokoLounge
} // namespace UI_GTKMM_NS

#endif // #ifndef GTKMM_DOKOLOUNGE_CHAT_HEADER
#endif // #ifdef USE_NETWORK_DOKOLOUNGE
#endif // #ifdef USE_NETWORK
#endif // #ifdef USE_UI_GTKMM
