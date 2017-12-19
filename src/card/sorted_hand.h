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

class Hand;
class HandCard;
class HandCards;
class Card;

/**
 **
 ** A sorted hand.
 ** This is a wrap around ::Hand
 **
 ** @author	Diether Knof
 **
 **/
class SortedHand {
  public:
    SortedHand(Hand& hand);
    SortedHand(SortedHand const& sorted_hand, Hand& hand);
    SortedHand(SortedHand const&);

    ~SortedHand();

    SortedHand& operator=(SortedHand const&);


    unsigned cardsnumber() const;
    unsigned cardsnumber_all() const;

    HandCard const& card(unsigned const i) const;
    HandCard const& card_all(unsigned const i) const;
    bool played(unsigned const i) const;
    unsigned played_trick(unsigned const i) const;

    unsigned from_sorted_pos(unsigned const i) const;
    unsigned from_sorted_pos_all(unsigned const i) const;
    unsigned to_sorted_pos(unsigned const i) const;
    unsigned to_sorted_pos_all(unsigned const i) const;
    
    unsigned pos_to_pos_all(unsigned pos) const;
    unsigned pos_all_to_pos(unsigned pos) const;

    HandCard requested_card() const;
    unsigned requested_position() const;
    unsigned requested_position_all() const;
    //void request_position(unsigned const position);
    void request_position_all(unsigned const position_all);
    //void request_card(Card const card);
    //void forget_request();

    void playcard(HandCard const& c);
    void playcard(unsigned const pos);
    void unplaycard(HandCard const& c);

    //void add(Card const& card);
    void add(vector<Card> const& cards);
    void add(HandCards const& cards);
    unsigned remove(HandCard const& card);
    void remove(HandCards const& cards);


    void set_sorting();

    bool sort();
    void unsort();
    void mix();

  private:
    void set_positions();

  private:
    PRIV_VAR_P_RW(Hand, hand);
    vector<unsigned> to_sorted_pos_; // position in the sorted hand
    vector<unsigned> from_sorted_pos_; // original position (SortedHand -> Hand)

    bool mixed;

  private: // unused
    SortedHand();
}; // class SortedHand

ostream& operator<<(ostream& ostr, SortedHand const& hand);
