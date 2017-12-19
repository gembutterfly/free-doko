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

#ifndef CLASS_HUMAN_HEADER
#define CLASS_HUMAN_HEADER

#include "../ai/ai.h"

class Human : public Ai {
  public:
    // constructor
    Human();
    // constructor
    Human(istream& istr);
    // copy constructor
    Human(Player const& player);
    // copy constructor
    Human(Player const& player, Aiconfig const& aiconfig);
    // copy constructor
    Human(Ai const& ai);
    // copy constructor
    Human(Human const& human);
    // clone the player
    virtual Human* clone() const;

    // desctructor
    ~Human();

    // get the reservation
    virtual Reservation const& reservation_get(bool const is_duty = false);
    // the game is started
    virtual void game_start();
    // make a save announcement
    virtual Announcement announcement_request() const;
    // get the card to play
    virtual HandCard card_get(); 

    // update the team information
    virtual void teaminfo_update();

    // 'player' shifts cards
    virtual HandCards poverty_shift();
    // returns whether 'player' accepts the poverty
    virtual bool poverty_take_accept(unsigned const cardno);
    // the player changes the cards of the poverty
    virtual HandCards poverty_cards_change(HandCards const& cards);
    // the poverty player gets 'cards'
    virtual void poverty_cards_get_back(HandCards const& cards);

    // the player has swines
    virtual void swines_announced(Player const& player);

    // return a partner for a genscher
    virtual Player const* genscher_partner();

  private: // unused
    Human& operator=(const Human& player);
}; // class Human: public Ai

#endif // #ifndef CLASS_HUMAN_HEADER
