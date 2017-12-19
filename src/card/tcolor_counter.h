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

/** counts the number of the tcolors
 **/
class TColorCounter : private vector<unsigned> {
  friend bool operator==(TColorCounter const&, TColorCounter const&);
  friend bool operator!=(TColorCounter const&, TColorCounter const&);

  public:
  // standard constructor
  TColorCounter();
  // copy constructor
  TColorCounter(TColorCounter const& tcolor_counter);
  // copy operator
  TColorCounter& operator=(TColorCounter const& tcolor_counter);
  // destructor
  ~TColorCounter();

  // clears all data
  void clear();

  // writes the data in 'ostr'
  void write(ostream& ostr) const;

  using vector<unsigned>::const_iterator;
  using vector<unsigned>::begin;
  using vector<unsigned>::end;


  // returns the total number of cards
  unsigned cards_no() const;
  // returns the number for 'tcolor'
  unsigned const& operator[](Card::TColor const& tcolor) const;
  private:
  // returns the number for 'tcolor'
  unsigned& operator[](Card::TColor const& tcolor);
  public:

  // set the counter of 'tcolor' to exactly 'n'
  bool set(Card::TColor const& tcolor, unsigned const n);
  // set the counter of 'tcolor' to minimal 'n'
  bool min_set(Card::TColor const& tcolor, unsigned const n);
  // set the counter of 'tcolor' to maximal 'n'
  bool max_set(Card::TColor const& tcolor, unsigned const n);

  // increment the counter of 'tcolor'
  void inc(Card::TColor const& tcolor);
  // decrement the counter of 'tcolor'
  void dec(Card::TColor const& tcolor);

  private:
}; // class TColorCounter : private vector<unsigned>

// write 'card_counter' in 'ostr'
ostream& operator<<(ostream& ostr, TColorCounter const& tcolor_counter);

// compare the two tcolor counters
bool operator==(TColorCounter const& tcolor_counter_a,
                TColorCounter const& tcolor_counter_b);
// compare the two tcolor counters
bool operator!=(TColorCounter const& tcolor_counter_a,
                TColorCounter const& tcolor_counter_b);
