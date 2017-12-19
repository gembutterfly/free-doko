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
#include <list>

/** a list of hand cards with weighting
 ** the cards are sorted descending according to the weighting
 **
 ** first element of the pair is the weighting, the second the card
 **/
class WeightedCardList : private std::list<pair<int, Card> > {
  typedef std::list<pair<int, Card> > List;
  public:
  // standard constructor
  WeightedCardList(Game const& game);
  // copy constructor
  WeightedCardList(WeightedCardList const& card_list);
  // copy operator
  WeightedCardList& operator=(WeightedCardList const& card_list);
  // destructor
  ~WeightedCardList();

  // clear the list
  void clear();

  // write the list in 'ostr'
  void write(ostream& ostr) const;

  // return the total number of cards
  unsigned cards_no() const;
  using List::empty;

  // add the card
  void add(Card const& card, int const weighting);

  // return the card with the highest weighting
  Card const& highest() const;
  // return the card with the highest weighting of tcolor
  Card const& highest(Card::TColor const tcolor) const;
  // return the card with the highest weighting not of tcolor
  Card const& highest_not_of(Card::TColor const tcolor) const;
  // return the card with the highest weighting of one of tcolor
  Card const& highest(Card::TColor const tcolor_a,
                      Card::TColor const tcolor_b) const;
  // return the card with the highest weighting of one of tcolor
  Card const& highest(Card::TColor const tcolor_a,
                      Card::TColor const tcolor_b,
                      Card::TColor const tcolor_c) const;
  // return the card with the highest weighting of one of tcolor
  Card const& highest(Card::TColor const tcolor_a,
                      Card::TColor const tcolor_b,
                      Card::TColor const tcolor_c,
                      Card::TColor const tcolor_d) const;
  // return the highest weighting
  int highest_weighting() const;
  // return the highest weighting of the tcolor
  int highest_weighting(Card::TColor const tcolor) const;
  // return the highest weighting not of the tcolor
  int highest_weighting_not_of(Card::TColor const tcolor) const;
  // return the card with the highest weighting of one of tcolor
  int highest_weighting(Card::TColor const tcolor_a,
                        Card::TColor const tcolor_b) const;
  // return the card with the highest weighting of one of tcolor
  int highest_weighting(Card::TColor const tcolor_a,
                        Card::TColor const tcolor_b,
                        Card::TColor const tcolor_c) const;
  // return the card with the highest weighting of one of tcolor
  int highest_weighting(Card::TColor const tcolor_a,
                        Card::TColor const tcolor_b,
                        Card::TColor const tcolor_c,
                        Card::TColor const tcolor_d) const;


  private:
  // return pointer with the highest weighting
  WeightedCardList::const_iterator highest_ptr() const;
  // return the pointer with the highest weighting of the tcolor
  WeightedCardList::iterator const& highest_ptr(Card::TColor const tcolor
                                               ) const;
  // return the pointer with the highest weighting not of the tcolor
  WeightedCardList::iterator const& highest_ptr_not_of(Card::TColor const
                                                       tcolor) const;
  // return the ptr with the highest weighting of one of tcolor
  WeightedCardList::iterator const& highest_ptr(Card::TColor const tcolor_a,
                                                Card::TColor const tcolor_b
                                               ) const;
  // return the ptr with the highest weighting of one of tcolor
  WeightedCardList::iterator const& highest_ptr(Card::TColor const tcolor_a,
                                                Card::TColor const tcolor_b,
                                                Card::TColor const tcolor_c
                                               ) const;
  // return the ptr with the highest weighting of one of tcolor
  WeightedCardList::iterator const& highest_ptr(Card::TColor const tcolor_a,
                                                Card::TColor const tcolor_b,
                                                Card::TColor const tcolor_c,
                                                Card::TColor const tcolor_d
                                               ) const;

  public:

  // return the (highest) weighting of the card
  int weighting(Card const& card) const;

  // remove the highest card
  void pop_highest();
  // remove the card with the highest weighting of tcolor
  void pop_highest(Card::TColor const tcolor);
  // remove the card with the highest weighting not of tcolor
  void pop_highest_not_of(Card::TColor const tcolor);
  // remove the given card
  void pop(Card const& card);

  private:
  // recreate the pointers to the highest card of each tcolor
  void recreate_tcolor_ptrs();
  // recreate the pointer to the highest card of the tcolor
  void recreate_tcolor_ptr(Card::TColor const tcolor);

  private:
  // the corresponding game
  PRIV_VAR_CP_R(Game, game);
  // an iterator to the highest card for each tcolor
  vector<WeightedCardList::iterator> tcolor_ptr;
  // the position of the highest card for each tcolor (-1 for none)
  vector<int> tcolor_pos;


  private: // unused
  WeightedCardList();
}; // class WeightedCardList : private vector<pair<int, Card> >

// write 'card_list' in 'ostr'
ostream& operator<<(ostream& ostr, WeightedCardList const& card_list);
