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

/** counts the number of the single cards
 ** gives additionally a vector with the cards
 **
 ** @todo   replace map with vector
 **/
class CardCounter : private map<Card, unsigned> {
  friend bool operator==(CardCounter const&, CardCounter const&);
  friend bool operator!=(CardCounter const&, CardCounter const&);

  public:
  // standard constructor
  CardCounter();
  // copy constructor
  CardCounter(CardCounter const& card_counter);
  // copy operator
  CardCounter& operator=(CardCounter const& card_counter);
  // destructor
  ~CardCounter();

  // clears all data
  void clear();

  // writes the data in 'ostr'
  void write(ostream& ostr) const;

  using map<Card, unsigned>::const_iterator;
  map<Card, unsigned>::const_iterator begin() const
  { return map<Card, unsigned>::begin(); }
  map<Card, unsigned>::const_iterator end() const
  { return map<Card, unsigned>::end(); }
  using map<Card, unsigned>::size;
  using map<Card, unsigned>::empty;


  // returns the total number of cards
  unsigned cards_no() const;
  // returns the number for 'card'
  unsigned operator[](Card const& card) const;
  // returns the number of cards of 'tcolor'
  unsigned operator()(Card::TColor const& tcolor, Game const& game) const;

  // set the counter of 'card' to exactly 'n'
  bool set(Card const& card, unsigned const n);
  // set the counter of 'card' to minimal 'n'
  bool min_set(Card const& card, unsigned const n);
  // set the counter of 'card' to maximal 'n'
  bool max_set(Card const& card, unsigned const n);

  // set the counter of 'card' to '0'
  bool erase(Card const& card);
  // increment the counter of 'card'
  void inc(Card const& card);
  // add 'n' to the counter of 'card'
  void add(Card const& card, unsigned const n);
  // decrement the counter of 'card'
  void dec(Card const& card);

  // whether 'cards' are contained
  bool contains(vector<Card> const& cards) const;
  // whether 'cards' are contained
  bool contains(map<Card, unsigned> const& cards) const;
  // whether 'cards' contain the cards
  bool is_contained(vector<Card> const& cards) const;
  // whether 'cards' contain the cards
  bool is_contained(map<Card, unsigned> const& cards) const;

  private:
  // the number of counted cards
  PRIV_VAR(unsigned, cards_no);
  // the cards
  PRIV_VAR_R(vector<Card>, cards);

  // the number of cards
  PRIV_VAR(vector<unsigned>, no);
}; // class CardCounter : private map<Card, unsigned>

// write 'card_counter' in 'ostr'
ostream& operator<<(ostream& ostr, CardCounter const& card_counter);

// compare the two card counters
bool operator==(CardCounter const& card_counter_a,
		CardCounter const& card_counter_b);
// compare the two card counters
bool operator!=(CardCounter const& card_counter_a,
		CardCounter const& card_counter_b);
