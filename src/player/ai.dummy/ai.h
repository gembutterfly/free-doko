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
 *   GNU General Public License for more detai_dummyls. 
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

#ifndef CLASS_AI_DUMMY_HEADER
#define CLASS_AI_DUMMY_HEADER

#include "../player.h"
#include "../aiconfig.h"

/**
 ** dummy ai
 ** If the ai can win the trick, take the highest card,
 ** else take the lowest.
 **/
class AiDummy : public Player, public Aiconfig {
  public:
    // constructor
    AiDummy();
    // constructor
    AiDummy(istream& istr);
    // copy constructor
    AiDummy(Player const& player);
    // copy constructor
    AiDummy(Player const& player, Aiconfig const& aiconfig);
    // copy constructor
    AiDummy(AiDummy const& ai_dummy);
    // copy operator
    AiDummy& operator=(AiDummy const& ai_dummy);
    // clone the player
    AiDummy* clone() const;

    // destructor
    ~AiDummy();

    // the game request a reservation
    Reservation const& reservation_get(bool const duty_solo = false);
    // 'player' shifts cards
    HandCards poverty_shift();
    // returns whether 'player' accepts the poverty
    bool poverty_take_accept(unsigned const cardno);
    // the player changes the cards of the poverty
    HandCards poverty_cards_change(HandCards const& cards);
    // the poverty player gets 'cards'
    void poverty_cards_get_back(HandCards const& cards);

    // the game request a card to play
    HandCard card_get();
    // the game request an announcement
    Announcement announcement_request() const;
    // the parther for a genscher
    Player const* genscher_partner();
}; // class AiDummy

#endif // #ifndef CLASS_AI_DUMMY_HEADER
