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

#include "card_counter.h"
#include "../game/game.h"

/**
 ** standard constructor
 **
 ** @param	-
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.9
 **/
CardCounter::CardCounter() :
  map<Card, unsigned>(),
  cards_no_(0),
  cards_(),
  no_(Card::MAX_INT + 1, 0)
{ }

/**
 ** copy constructor
 **
 ** @param	card_counter	object to copy from
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.9
 **/
CardCounter::CardCounter(CardCounter const& card_counter) :
  map<Card, unsigned>(card_counter),
  cards_no_(card_counter.cards_no_),
  cards_(card_counter.cards_),
  no_(card_counter.no_)
{ }

/**
 ** copy operator
 **
 ** @param	card_counter	object to copy from
 **
 ** @return	object
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.9
 **/
CardCounter&
CardCounter::operator=(CardCounter const& card_counter)
{
  static_cast<map<Card, unsigned>& >(*this) = card_counter;
  this->cards_no_ = card_counter.cards_no_;
  this->cards_ = card_counter.cards_;
  this->no_ = card_counter.no_;

  return *this;
} // CardCounter& CardCounter::operator=(CardCounter card_counter)

/**
 ** destructor
 **
 ** @param	-
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.9
 **/
CardCounter::~CardCounter()
{ }

/**
 ** clears all data
 **
 ** @param	-
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.9
 **/
void
CardCounter::clear()
{
  this->cards_.clear();
  this->cards_no_ = 0;
  this->map<Card, unsigned>::clear();

  this->no_ = vector<unsigned>(Card::MAX_INT + 1, 0);

  return ;
} // void CardCounter::clear()

/**
 ** writes the data ('card = number') in 'ostr'
 **
 ** @param	ostr	output stream to write the data to
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.9
 **/
void
CardCounter::write(ostream& ostr) const
{
  ostr << "cardno = " << this->cards_no() << '\n';
  for (auto const& x : *this) {
    if (x.second)
      ostr << setw(14) << x.first << " = " << x.second << '\n';
  }

  return ;
} // void CardCounter::write(ostream& ostr) const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	the total number of cards
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.9
 **/
unsigned
CardCounter::cards_no() const
{
#ifdef DEBUG
  DEBUG_ASSERTION((this->cards_no_ == this->cards_.size()),
                  "CardCounter::cards_no():\n"
                  "  'this->cards_no_ = " << this->cards_no_
                  << " != " << this->cards_.size()
                  << " = this->cards_.size()");

  unsigned n = 0;
  for (auto const& x : *this)
    n += x.second;

  DEBUG_ASSERTION((this->cards_no_ == n),
                  "CardCounter::cards_no():\n"
                  "  'this->cards_no_ = " << this->cards_no_
                  << " != " << n
                  << " = Sum");
#endif

  return this->cards_no_;
} // unsigned CardCounter::cards_no() const

/**
 ** -> result
 **
 ** @param	card	card to return the number of
 **
 ** @return	the number for 'card'
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.9
 **/
unsigned
CardCounter::operator[](Card const& card) const
{
  return this->no_[card.to_int()];
} // unsigned CardCounter::operator[](Card card) const

/**
 ** -> result
 **
 ** @param	tcolor   tcolor to return the number of
 ** @param	game     corresponding game
 **
 ** @return	the number of cards of 'tcolor'
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
unsigned
CardCounter::operator()(Card::TColor const& tcolor, Game const& game) const
{
  unsigned no = 0;

  for (auto const& x : *this)
    if (x.first.tcolor(game) == tcolor)
      no += x.second;

  return no;
} // unsigned CardCounter::operator()(Card::TColor tcolor, Game game) const

/**
 ** set the counter of 'card' to exactly 'n'
 **
 ** @param	card	card to set the number of
 ** @param	n	new number for 'card'
 **
 ** @return	whether something has changed
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.9
 **/
bool
CardCounter::set(Card const& card, unsigned const n)
{
  unsigned const m = (*this)[card];
  if (n > m)
    return this->min_set(card, n);
  else if (m < n)
    return this->max_set(card, n);

  return false;
} // bool CardCounter::set(Card const& card, unsigned const n)

/**
 ** set the counter of 'card' to minmal 'n'
 **
 ** @param	card	card to set the number of
 ** @param	n	minimal number for 'card'
 **
 ** @return	whether something has changed
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.9
 **/
bool
CardCounter::min_set(Card const& card, unsigned const n)
{
  if (n == 0)
    return false;

  unsigned const t = (*this)[card];
  if (t >= n)
    return false;

  // add the card to the vector
  this->cards_.insert(this->cards_.end(), (n - t), card);

  // set the number
  this->map<Card, unsigned>::operator[](card) = n;
  this->no_[card.to_int()] = n;

  // adjust the total cards number
  this->cards_no_ += (n - t);

  return true;
} // bool CardCounter::min_set(Card const& card, unsigned const n)

/**
 ** set the counter of 'card' to maximal 'n'
 **
 ** @param	card	card to set the number of
 ** @param	n	maximal number for 'card'
 **
 ** @return	whether something has changed
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.9
 **/
bool
CardCounter::max_set(Card const& card, unsigned const n)
{
  unsigned const t = (*this)[card];
  if (t <= n)
    return false;

  { // remove the cards from the list
    auto i = std::find(this->cards_.begin(), this->cards_.end(), card);
    for (unsigned a = 0; a < t - n; ++a) {
      i = this->cards_.erase(i);
      i = std::find(i, this->cards_.end(), card);
    } // for (a < t - n)
  } // remove the cards from the list

  // set the number
  if (n == 0) {
    this->map<Card, unsigned>::erase(card);
  } else { // if !(n == 0)
    this->map<Card, unsigned>::operator[](card) = n;
  } // if !(n == 0)
  this->no_[card.to_int()] = n;

  // adjust the total cards number
  this->cards_no_ -= t - n;

  return true;
} // bool CardCounter::max_set(Card const& card, unsigned const n)

/**
 ** erases the counter of 'card'
 **
 ** @param	card	card to erase
 **
 ** @return	whether something has changed
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.9
 **/
bool
CardCounter::erase(Card const& card)
{
  return this->max_set(card, 0);
} // bool CardCounter::erase(Card const& card)

/**
 ** increments the counter of 'card'
 **
 ** @param	card	card to increment
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.9
 **/
void
CardCounter::inc(Card const& card)
{
  this->map<Card, unsigned>::operator[](card) += 1;
  this->cards_.push_back(card);
  this->cards_no_ += 1;

  this->no_[card.to_int()] += 1;

  return ;
} // void CardCounter::inc(Card const& card)

/**
 ** adds 'n' to the counter of 'card'
 **
 ** @param	card	card to add
 ** @param	n	number to add
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.12
 **/
void
CardCounter::add(Card const& card, unsigned const n)
{
  this->map<Card, unsigned>::operator[](card) += n;
  for (unsigned i = 0; i < n; ++i)
  this->cards_.push_back(card);
  this->cards_no_ += n;

  this->no_[card.to_int()] += n;

  return ;
} // void CardCounter::add(Card card, unsigned n)

/**
 ** decrements the counter of 'card'
 **
 ** @param	card	card to decrements
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.9
 **/
void
CardCounter::dec(Card const& card)
{
  map<Card, unsigned>::iterator x = this->find(card);
  DEBUG_ASSERTION((x != this->end())
                  && x->second,
                  "CardCounter::dec(" << card << "):\n"
                  "  the counter is '0'");
  x->second -= 1;
  if (x->second == 0)
    this->map<Card, unsigned>::erase(x);

  this->no_[card.to_int()] -= 1;

  this->cards_.erase(std::find(this->cards_.begin(), this->cards_.end(), card));
  this->cards_no_ -= 1;

  return ;
} // void CardCounter::dec(Card const& card)

/**
 ** -> return
 **
 ** @param	cards   cards to check
 **
 ** @return	whether 'cards' are contained
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
bool
CardCounter::contains(vector<Card> const& cards) const
{
  map<Card, unsigned> counter;
  for (auto const& c : cards)
    counter[c] += 1;

  return this->contains(counter);
} // bool CardCounter::contains(vector<Card> cards) const

/**
 ** -> return
 **
 ** @param	cards   cards to check
 **
 ** @return	whether 'cards' are contained
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
bool
CardCounter::contains(map<Card, unsigned> const& cards) const
{
  for (auto const& c : cards) {
    if (c.first == Card::UNKNOWN)
      continue;
    if ((*this)[c.first] < c.second) {
      cerr << "CardCounter::contains() "
        << c.first << ": " << (*this)[c.first] << " < " << c.second << endl;
      return false;
    }
  }

  return true;
} // bool CardCounter::contains(map<Card, unsigned> cards) const

/**
 ** -> return
 **
 ** @param	cards   cards to check
 **
 ** @return	whether 'cards' contains the cards
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
bool
CardCounter::is_contained(vector<Card> const& cards) const
{
  map<Card, unsigned> counter;
  for (auto const& c : cards)
    counter[c] += 1;

  return this->is_contained(counter);
} // bool CardCounter::is_contained(vector<Card> cards) const

/**
 ** -> return
 **
 ** @param	cards   cards to check
 **
 ** @return	whether 'cards' contains the cards
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
bool
CardCounter::is_contained(map<Card, unsigned> const& cards) const
{
  for (auto const& c : cards)
    if ((*this)[c.first] > c.second) {
      cerr << "CardCounter::is_contained() "
        << c.first << ": " << (*this)[c.first] << " > " << c.second << endl;
      return false;
    }

  return true;
} // bool CardCounter::is_contained(map<Card, unsigned> cards) const

/**
 ** writes the card counter in 'ostr'
 **
 ** @param	ostr		output stream to write into
 ** @param	card_counter	object to write
 **
 ** @return	output stream
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.9
 **/
ostream&
operator<<(ostream& ostr, CardCounter const& card_counter)
{
  card_counter.write(ostr);
  return ostr;
} // ostream& operator<<(ostream& ostr, CardCounter const& card_counter)

/**
 ** -> result
 **
 ** @param	card_counter_a   first object
 ** @param	card_counter_b   second object
 **
 ** @return	whether the two objects are equal
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.9
 **/
bool
operator==(CardCounter const& card_counter_a,
           CardCounter const& card_counter_b)
{
  return (static_cast<map<Card, unsigned> const&>(card_counter_a)
          == static_cast<map<Card, unsigned> const&>(card_counter_b));
} // bool operator==(CardCounter card_counter_a, CardCounter card_counter_b)

/**
 ** -> result
 **
 ** @param	card_counter_a	first object
 ** @param	card_counter_b	second object
 **
 ** @return	whether the two objects differ
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.9
 **/
bool
operator!=(CardCounter const& card_counter_a,
           CardCounter const& card_counter_b)
{
  return (static_cast<map<Card, unsigned> const&>(card_counter_a)
          != static_cast<map<Card, unsigned> const&>(card_counter_b));
} // bool operator!=(CardCounter card_counter_a, CardCounter card_counter_b)
