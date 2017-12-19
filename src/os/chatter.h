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

#ifndef CLASS_OS_CHATTER_HEADER
#define CLASS_OS_CHATTER_HEADER

#include "os.h"

namespace OS_NS {

  /**
   ** a chatter (to test the chat window)
   **
   ** @author	Diether Knof
   **
   ** @version	0.6.8
   **/
  class Chatter : public OS {
    public:
      Chatter();

      ~Chatter();

      void party_start();

      void game_finish();

      void trick_full();
      // the card is played
      void card_played(HandCard const& card);
      // an announcement is made
      void announcement_made(Announcement const& announcement,
			     Player const& player);
      // the player has swines
      void swines_announced(Player const& player);
      // the player has hyperswines
      void hyperswines_announced(Player const& player);

    private: // not implemented
      Chatter(Chatter const&);
      Chatter& operator=(Chatter const&);
  }; // class Chatter : public OS

} // namespace OS_NS

#endif // #ifndef CLASS_OS_CHATTER_HEADER
