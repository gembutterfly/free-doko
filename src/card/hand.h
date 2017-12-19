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
#include "hand_cards.h"
class Trick;
class Player;

/**
 **
 ** Hand (of a player) with all cards (played and not played)
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 **
 **/
class Hand {
  public:

    Hand();
    Hand(Player const& player);
    Hand(Player const& player, Hand const& h);
    Hand(Hand const& h);
    Hand(vector<Card> const& cards);
    Hand(Player const& player, vector<Card> const& cards);
    Hand(Player const& player, vector<Card> const& unplayed_cards,
	 vector<Card> const& played_cards);
    Hand(istream& istr);

    ~Hand();

    Hand& operator=(Hand const& h);

    operator HandCards() const;
    operator vector<Card>() const;

    // checks the data
    bool self_check() const;

    // the player this hand belongs to 
    Player const& player() const;
    // set the player of this hand
    void set_player(Player const& player);

    // the corresponding game
    Game const& game() const;

    // whether the hand is empty (cardsnumber = 0)
    bool empty() const;
    // the number of cards (which are not played)
    unsigned cardsnumber() const;
    // the number of cards (all)
    unsigned cardsnumber_all() const;

    // the points on the hand
    unsigned points() const;

    // set an unknown card to a known one
    void set_unknown(Card const& card, unsigned n = 1);

    // the i'th card (of the unplayed)
    HandCard const& card(unsigned i) const;
    // the i'th card (of all)
    HandCard const& card_all(unsigned i) const;
    // whether the i'th card is played
    bool played(unsigned i) const;
    // in wich trick the i'th card was played
    unsigned played_trick(unsigned i) const;

    // all cards without double ones
    HandCards cards_single() const;
    // the cards and its number
    map<Card, unsigned> counted_cards() const;

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
    // the highest card of the given tcolor
    HandCard const& highest_card(Card::TColor const& tcolor) const;
    // the lowest trump 
    HandCard const& lowest_trump() const;
    // the lowest card of the given tcolor
    HandCard const& lowest_card(Card::TColor const& tcolor) const;
    // whether there exists a higher card
    bool higher_card_exists(Card const& card) const;
    // whether there exists a lower card
    bool lower_card_exists(Card const& card) const;
    // the number of higher cards
    unsigned higher_cards_no(Card const& card) const;
    // the next jabbing card of 'card'
    HandCard const& next_jabbing_card(Card const& card) const;
    // the next higher card then 'card'
    HandCard const& next_higher_card(Card const& card) const;
    // the card 'card' or next higher card then 'card'
    HandCard const& same_or_higher_card(Card const& card) const;
    // the next lower card then 'card' (in the same color)
    HandCard const& next_lower_card(Card const& card) const;
    // the card 'card' or next lower card then 'card'
    HandCard const& same_or_lower_card(Card const& card) const;
    // whether the trick can be jabbed
    bool can_jab(Trick const& trick) const;

    // get the position of the card
    unsigned getpos(Card const& card) const;
    // get the position of the card (in all cards)
    unsigned getpos_all(Card const&) const;

    // transform the position in the unplayed cards to the position in all cards
    unsigned pos_to_pos_all(unsigned pos) const;
    // transform the position in all cards to the position in the unplayed cards
    unsigned pos_all_to_pos(unsigned pos) const;



    // changing

    // play the card 'card'
    void playcard(HandCard const& card);
    // play the card at the position 'pos'
    void playcard(unsigned pos);
    // unplaye the card 'card'
    void unplaycard(HandCard const& card);

    // add the card to the hand
    void add(Card const& card, unsigned n = 1);
    // add the cards to the hand
    void add(vector<Card> const& cards);
    // add the hand to the hand
    void add(Hand const& hand);

    // add the played card to the hand
    void add_played(Card const& card, unsigned n = 1);
    // add the played cards to the hand
    void add_played(vector<Card> const& cards);

    // remove the card from the hand
    unsigned remove(HandCard const& card);
    // remove the card from the hand
    unsigned remove(Card const& card);
    // remove the cards from the hand
    void remove(HandCards const& cards);



    // card request

    // the requested card
    HandCard requested_card() const;
    // the requested position
    unsigned requested_position_all() const;
    // request the card at the position (in the unplayed cards)
    void request_position(unsigned position);
    // request the card at the position (in all cards)
    void request_position_all(unsigned position_all);
    // request the card
    void request_card(Card card);
    // forget the request
    void forget_request();



    // counting

    // the valid cards for the trick (do not use in for-loop!)
    HandCards validcards(Trick const& trick) const;
    // the cards of a given tcolor
    HandCards cards(Card::TColor tcolor) const;
    // the trumps
    HandCards trumps() const;

    // whether there is a trump card in the hand
    bool hastrump() const;
    // whether there is a color card in the hand
    bool hascolor() const;

    // whether the hand does contain possible genscher cards
    bool has_possible_genscher() const;
    // whether the hand does contain swines (or can announce them)
    bool has_swines() const;
    // whether the hand does contain hyperswines (or can announce them)
    bool has_hyperswines() const;

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

    // the number of cards 'Card(color)' without counting trump
    unsigned numberof(Card::Color const& color, Game const& game) const;
    unsigned numberof(Card::Color const& color,
		      GameType gametype) const;
    unsigned numberof(Card::Color const& color,
		      GameType gametype,
		      bool dollen) const;

    // the number of played cards 'card'
    unsigned played(Card const& card) const;

    // the number of trumps
    unsigned numberoftrumps() const;
    // the number of trumps
    unsigned numberoftrumps(GameType game_type) const;
    // the number of fehl
    unsigned numberoffehl(GameType game_type) const;
    // the number of single tens
    unsigned numberofsingletens(GameType game_type) const;
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
    // the number of dolle
    unsigned numberofdolle() const;
    // the number of swines
    unsigned numberofswines() const;
    // the number of hyperswines
    unsigned numberofhyperswines() const;
    // the number of fehl colors
    unsigned numberoffehlcolors(GameType game_type) const;

    // the number of cards to shift in the poverty
    unsigned numberofpovertycards() const;
    // whether the hand has a poverty
    bool has_poverty() const;

    // the number of cards 'Card(tcolor, value)' in all cards
    unsigned numberofall(Card::TColor const& tcolor,
			 Card::Value const& value) const;
    // the number of cards 'card' in all cards
    unsigned numberofall(Card const& card) const;
    // the number trump aces in all cards
    unsigned numberofalltrumpaces() const;


    // the number of cards which are greater or equal in the game
    unsigned numberof_ge(Card const& card) const;
    // the number of cards which are less or equal in the game
    unsigned numberof_le(Card const& card) const;
    // the number of trump cards which are greater or equal in the game
    unsigned numberof_ge_trump(Card const& card) const;
    // the number of trump cards which are less or equal in the game
    unsigned numberof_le_trump(Card const& card) const;
    // the relative position of the card in the trumps
    float rel_pos_trump(Card const& card) const;

    // checking

    // whether the 'hand' has the same cards as 'this'
    bool has_same_cards(Hand const& hand) const;

    // whether 'tcolor' is contained in this hand
    bool contains(Card::TColor const& tcolor) const;
    // whether 'card' is contained in this hand
    bool contains(Card const& card) const;
    // whether 'card' is contained in this hand
    bool contains(Card::Color const& color, Card::Value const& value) const;
    // whether 'hand' is contained in this hand
    bool contains(Hand const& hand) const;
    // the cards which are in 'hand' but not in this hand
    HandCards missing_cards(Hand const& hand) const;

  private:
    // the cards on the hand
    PRIV_VAR_R(HandCards, cards);
    // all cards on the hand
    PRIV_VAR_R(HandCards, cards_all);
    // whether a card is played and in which trick
    vector<unsigned> played_;

    // which card is requested
    PRIV_VAR_R(unsigned, requested_position);

    // the player who owns the hand
    PRIV_VAR_CP(Player, player);
}; // class Hand

// write the hand in the output
ostream& operator<<(ostream& ostr, Hand const& hand);

// compare two hands
bool operator==(Hand const& hand1, Hand const& hand2);
// compare two hands
bool operator!=(Hand const& hand1, Hand const& hand2);
