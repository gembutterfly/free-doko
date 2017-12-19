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

#include "card.h"

class Hand;
class Player;
class Game;
class Trick;

/**
 ** class for a card which belongs to a hand (and so to a player)
 **
 ** @author	Diether Knof
 **
 **/
class HandCard : public Card {
  public:
    // empty card
    static HandCard const EMPTY;
    // for sorting the hands
    static int relative_position(HandCard const& a, HandCard const& b);
  public:
    // constructor
    HandCard();
    // constructor
    HandCard(Hand const& hand);
    // constructor
    explicit HandCard(Card const& card);
    // constructor
    HandCard(Hand const& hand, Card const& card);
    // constructor
    HandCard(Hand const& hand,
	     Card::Color const color, Card::Value const value);
    // copy constructor
    HandCard(HandCard const&);
    // copy operator
    HandCard& operator=(HandCard const& card);
    // copy operator
    HandCard& operator=(Card const& card);
    // destructor
    ~HandCard();

    Player const& player() const;
    Game const& game() const;

    bool istrump() const;
    Card::TColor tcolor() const;

    bool istrumpace() const;
    bool istrumpnine() const;
    bool istrumpking() const;

    bool isfox() const;
    bool isdolle() const;
    bool isswine() const;
    bool ishyperswine() const;
    bool is_special() const;

    // whether the card is a genscher card
    bool possible_genscher() const;
    // whether the card is or can be a swine
    bool possible_swine() const;
    // whether the card is or can be a hyperswine
    bool possible_hyperswine() const;


    // returns whether the card is less than 'card'
    // This also test for 'possible swines'
    bool less(HandCard const& card) const;
    // returns whether the card is less than 'card'
    bool less(Card const& card) const;

    // returns whether the card can be played in the trick
    bool isvalid(Trick const& trick) const;

  private:
    // the corresponding hand
    PRIV_VAR_CP_R(Hand, hand);
}; // class HandCard : public Card
