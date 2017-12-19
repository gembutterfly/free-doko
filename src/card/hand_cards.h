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

#include "../basistypes.h"
#include "hand_card.h"
class Trick;
class Player;

/**
 ** Cards of a hand
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 **/
class HandCards : public vector<HandCard> {
  public:
    HandCards(vector<HandCard> const& cards);
    HandCards(vector<Card> const& cards);
    HandCards(Hand const& hand, vector<Card> const& cards);
    HandCards(unsigned n);
    HandCards(istream& istr);
    HandCards();
    HandCards(HandCards const&);
    HandCards& operator=(HandCards const&);
    ~HandCards();
    
    operator vector<Card>() const;

    // the number of cards
    unsigned cardsnumber() const;

    // the hand card of number 'i'
    HandCard const& card(unsigned i) const;

    // the player the hand cards belong to 
    Player const& player() const;
    // the corresponding game
    Game const& game() const;

    // add a hand card
    void add(HandCard const& card);
    // add hand cards
    void add(HandCards const& cards);

    // remove a hand card
    void remove(Card const& card);
    // remove 'cards'
    void remove(HandCards const& cards);

    // the highest value
    Card::Value highest_value() const;
    // the highest value of the given tcolor
    Card::Value highest_value(Card::TColor const& tcolor) const;
    // the lowest value
    Card::Value lowest_value() const;
    // the lowest value of the given tcolor
    Card::Value lowest_value(Card::TColor const& tcolor) const;

    // the highest card 
    HandCard const& highest_card() const;
    // the highest trump 
    HandCard const& highest_trump() const;
    // the highest trump till the given card
    HandCard const& highest_trump_till(Card const& card_limit) const;
    // the highest card  of the given tcolor
    HandCard const& highest_card(Card::TColor const& tcolor) const;
    // the highest card of the given tcolor till the given card
    HandCard const& highest_card_till(Card::TColor const& tcolor,
                                      Card const& card_limit) const;
    // the lowest card  of the given tcolor
    HandCard const& lowest_card(Card::TColor const& tcolor) const;
    // the lowest trump 
    HandCard const& lowest_trump() const;
    // whether there exists a higher card
    bool higher_card_exists(Card const& card) const;
    // whether there exists a lower card of the same color
    bool lower_card_exists(Card const& card) const;
    // the number of higher cards
    unsigned higher_cards_no(Card const& card) const;
    // the next jabbing card then 'card'
    HandCard const& next_jabbing_card(Card const& card) const;
    // the next higher card then 'card'
    HandCard const& next_higher_card(Card const& card) const;
    // the card 'card' or the next higher card then 'card'
    HandCard const& same_or_higher_card(Card const& card) const;
    // the next lower card then 'card' (in the same color)
    HandCard const& next_lower_card(Card const& card) const;
    // the card 'card' or the next lower card then 'card'
    HandCard const& same_or_lower_card(Card const& card) const;



    // whether there is a trump card in the hand
    bool hastrump() const;
    // whether there is a color card in the hand
    bool hascolor() const;

    // the number of cards with the value
    unsigned numberof(Card::Value const& value) const;
    // the number of cards with 10 or 11 points
    unsigned numberofrichcards() const;
    // the number of cards with the trump color
    unsigned numberof(Card::TColor const& tcolor) const;
    // the number of cards 'card'
    unsigned numberof(Card const& card) const;
    // the number of cards 'Card(tcolor, value)'
    unsigned numberof(Card::TColor const& tcolor,
                      Card::Value const& value) const;
    // whether 'card' is contained
    bool contains(Card const& card) const;
    // whether 'Card(tcolor, value)' is contained
    bool contains(Card::TColor const& tcolor,
                  Card::Value const& value) const;
    // whether the tcolor is contianed
    bool contains(Card::TColor const& tcolor) const;

    // the number of cards 'Card(color)' without counting trump
    unsigned numberof(Card::Color const& color, Game const& game) const;
    unsigned numberof(Card::Color const& color,
                      GameType gametype,
                      bool dollen) const;

    // the number of trumps
    unsigned numberoftrumps() const;
    // the number of trumps
    unsigned numberoftrumps(GameType game_type) const;
    // the number of trumps aces
    unsigned numberoftrumpaces() const;
    // the number of trumps aces
    unsigned numberoftrumpaces(GameType game_type) const;
    // the number of trumps nines
    unsigned numberoftrumpnines() const;
    // the number of trump kings
    unsigned numberoftrumpkings() const;
    // the number of nines
    unsigned numberofnines() const;
    // the number of kings
    unsigned numberofkings() const;
    // the number of club queens
    unsigned numberofclubqueens() const;

    // whether 'cards' is equal to 'this' (without order)
    bool equal(HandCards const& cards) const;

  private: // unused
}; // class HandCards : public vector<HandCard>

// write the cards in the output
ostream& operator<<(ostream& ostr, HandCards const& cards);
