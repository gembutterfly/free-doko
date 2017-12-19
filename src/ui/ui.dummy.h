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

#ifndef CLASS_UI_DUMMY_HEADER
#define CLASS_UI_DUMMY_HEADER

#include "../basistypes.h"

#include "ui.h"

/**
 **
 ** @brief	a dummy ui class (no user interaction)
 **
 ** @author	Diether Knof
 **
 **/
class UI_Dummy : public UI {
  public:
    // constructor
    UI_Dummy();

    // destructor
    ~UI_Dummy();

    // the party is closed
    void party_close();

    // return a reservation (should not be called)
    Reservation reservation_get(Player const& player);
    // return a card to play (should not be called)
    HandCard card_get(Player const& player);

    // return cards to shift (should not be called)
    HandCards poverty_shift(Player& player);
    // return whether to accept the poverty (should not be called)
    bool poverty_take_accept(Player& player,
			     unsigned const cardno);
    // return cards to return (should not be called)
    HandCards poverty_cards_change(Player& player,
				   HandCards const& cards);

    // return the partner for the genscher (should not be called)
    Player const* genscher_partner();

    // information for the user
    void information(Translator::Translation const& message,
		     INFORMATION::Type const type,
                     bool const force_show = false);
    // an error has occured
    void error(string const& message);

  private: // unused
    UI_Dummy(UI_Dummy const&);
    UI_Dummy& operator=(UI_Dummy const&);

}; // class UI_Dummy : public UI

#endif // #ifndef CLASS_UI_DUMMY_HEADER
