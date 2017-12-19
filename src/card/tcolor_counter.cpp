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

#include "tcolor_counter.h"
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
 ** @version	0.7.6
 **/
TColorCounter::TColorCounter() :
  vector<unsigned>(Card::MAXCARDCOLOR+1, 0)
{ }

/**
 ** copy constructor
 **
 ** @param	tcolor_counter	object to copy from
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
TColorCounter::TColorCounter(TColorCounter const& tcolor_counter) :
  vector<unsigned>(tcolor_counter)
{ }

/**
 ** copy operator
 **
 ** @param	tcolor_counter	object to copy from
 **
 ** @return	object
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
TColorCounter&
TColorCounter::operator=(TColorCounter const& tcolor_counter)
{
  static_cast<vector<unsigned>& >(*this) = tcolor_counter;

  return *this;
} // TColorCounter& TColorCounter::operator=(TColorCounter tcolor_counter)

/**
 ** destructor
 **
 ** @param	-
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
TColorCounter::~TColorCounter()
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
 ** @version	0.7.6
 **/
void
TColorCounter::clear()
{
  static_cast<vector<unsigned>&>(*this) = TColorCounter();

  return ;
} // void TColorCounter::clear()

/**
 ** writes the data ('card = number') in 'ostr'
 **
 ** @param	ostr	output stream to write the data to
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
void
TColorCounter::write(ostream& ostr) const
{
  ostr << "cardno = " << this->cards_no() << '\n';
  for (unsigned c = 0; c < Card::NUMBER_OF_TCOLORS; ++c)
    ostr << setw(14) << static_cast<Card::TColor>(c)
      << " = " << (*this)[static_cast<Card::TColor>(c)] << '\n';

  return ;
} // void TColorCounter::write(ostream& ostr) const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	the total number of cards
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
unsigned
TColorCounter::cards_no() const
{
  unsigned n = 0;

  for (unsigned c = 0; c < Card::NUMBER_OF_TCOLORS; ++c)
    n += (*this)[static_cast<Card::TColor>(c)];
  n += (*this)[Card::UNKNOWNCARDCOLOR];

  return n;
} // unsigned TColorCounter::cards_no() const

/**
 ** -> result
 **
 ** @param	tcolor   tcolor to return the number of
 **
 ** @return	the number for 'tcolor'
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
unsigned const&
TColorCounter::operator[](Card::TColor const& tcolor) const
{
  return static_cast<vector<unsigned> const&>(*this)[tcolor];
} // unsigned TColorCounter::operator[](Card::TColor tcolor) const

/**
 ** -> result
 **
 ** @param	tcolor   tcolor to return the number of
 **
 ** @return	the number for 'tcolor'
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
unsigned&
TColorCounter::operator[](Card::TColor const& tcolor)
{
  return static_cast<vector<unsigned>&>(*this)[tcolor];
} // unsigned& TColorCounter::operator[](Card::TColor tcolor)

/**
 ** set the counter of 'tcolor' to exactly 'n'
 **
 ** @param	tcolor   tcolor to set the number of
 ** @param	n        new number for 'tcolor'
 **
 ** @return	whether something has changed
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
bool
TColorCounter::set(Card::TColor const& tcolor, unsigned const n)
{
  unsigned const m = (*this)[tcolor];
  if (n > m)
    return this->min_set(tcolor, n);
  else if (m < n)
    return this->max_set(tcolor, n);

  return false;
} // bool TColorCounter::set(Card::TColor tcolor, unsigned n)

/**
 ** set the counter of 'tcolor' to minmal 'n'
 **
 ** @param	tcolor   tcolor to set the number of
 ** @param	n        minimal number for 'tcolor'
 **
 ** @return	whether something has changed
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
bool
TColorCounter::min_set(Card::TColor const& tcolor, unsigned const n)
{
  unsigned const t = (*this)[tcolor];
  if (t >= n)
    return false;

  (*this)[tcolor] = n;

  return true;
} // bool TColorCounter::min_set(Card::TColor tcolor, unsigned n)

/**
 ** set the counter of 'tcolor' to maximal 'n'
 **
 ** @param	tcolor	tcolor to set the number of
 ** @param	n	maximal number for 'tcolor'
 **
 ** @return	whether something has changed
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.9
 **/
bool
TColorCounter::max_set(Card::TColor const& tcolor, unsigned const n)
{
  unsigned const t = (*this)[tcolor];
  if (t <= n)
    return false;

  (*this)[tcolor] = n;

  return true;
} // bool TColorCounter::max_set(Card::TColor tcolor, unsigned n)

/**
 ** increments the counter of 'tcolor'
 **
 ** @param	tcolor   tcolor to increment
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
void
TColorCounter::inc(Card::TColor const& tcolor)
{
  (*this)[tcolor] += 1;

  return ;
} // void TColorCounter::inc(Card::TColor tcolor)

/**
 ** decrements the counter of 'tcolor'
 **
 ** @param	tcolor   tcolor to decrement
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
void
TColorCounter::dec(Card::TColor const& tcolor)
{
  (*this)[tcolor] -= 1;

  return ;
} // void TColorCounter::dec(Card::TColor tcolor)

/**
 ** writes the card counter in 'ostr'
 **
 ** @param	ostr		output stream to write into
 ** @param	tcolor_counter	object to write
 **
 ** @return	output stream
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
ostream&
operator<<(ostream& ostr, TColorCounter const& tcolor_counter)
{
  tcolor_counter.write(ostr);
  return ostr;
} // ostream& operator<<(ostream& ostr, TColorCounter tcolor_counter)

/**
 ** -> result
 **
 ** @param	tcolor_counter_a   first object
 ** @param	tcolor_counter_b   second object
 **
 ** @return	whether the two objects are equal
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
bool
operator==(TColorCounter const& tcolor_counter_a,
           TColorCounter const& tcolor_counter_b)
{
  return (static_cast<vector<unsigned> const&>(tcolor_counter_a)
          == static_cast<vector<unsigned> const&>(tcolor_counter_b));
} // bool operator==(TColorCounter tcolor_counter_a, TColorCounter tcolor_counter_b)

/**
 ** -> result
 **
 ** @param	tcolor_counter_a	first object
 ** @param	tcolor_counter_b	second object
 **
 ** @return	whether the two objects differ
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
bool
operator!=(TColorCounter const& tcolor_counter_a,
           TColorCounter const& tcolor_counter_b)
{
  return (static_cast<vector<unsigned> const&>(tcolor_counter_a)
          != static_cast<vector<unsigned> const&>(tcolor_counter_b));
} // bool operator!=(TColorCounter tcolor_counter_a, TColorCounter tcolor_counter_b)
