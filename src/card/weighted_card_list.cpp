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

#include "constants.h"

#include "weighted_card_list.h"

/**
 ** standard constructor
 **
 ** @param     game   corresponding game
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.6
 **/
WeightedCardList::WeightedCardList(Game const& game) :
  List(),
  game_(&game),
  tcolor_ptr(Card::NUMBER_OF_TCOLORS, WeightedCardList::end()),
  tcolor_pos(Card::NUMBER_OF_TCOLORS, -1)
{ }

/**
 ** copy constructor
 **
 ** @param     card_list   card list to copy
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.6
 **/
WeightedCardList::WeightedCardList(WeightedCardList const& card_list) :
  List(card_list),
  game_(card_list.game_),
  tcolor_ptr(Card::NUMBER_OF_TCOLORS, WeightedCardList::end()),
  tcolor_pos(Card::NUMBER_OF_TCOLORS, -1)
{ this->recreate_tcolor_ptrs(); }

/**
 ** copy operator
 **
 ** @param     card_list   card list to copy
 **
 ** @return    the card list
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.6
 **/
WeightedCardList&
WeightedCardList::operator=(WeightedCardList const& card_list)
{
  static_cast<List&>(*this) = card_list;
  this->recreate_tcolor_ptrs();

  return *this;
} // WeightedCardList& WeightedCardList::operator=(WeightedCardList card_list)

/**
 ** destructor
 **
 ** @param     -
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.6
 **/
WeightedCardList::~WeightedCardList()
{ }

/**
 ** clear the list
 **
 ** @param     -
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.6
 **/
void
WeightedCardList::clear()
{
  this->List::clear();
  this->tcolor_ptr
    = vector<WeightedCardList::iterator>(Card::NUMBER_OF_TCOLORS,
                                         this->end());
  this->tcolor_pos = vector<int>(Card::NUMBER_OF_TCOLORS, -1);
  return ;
} // void WeightedCardList::clear()

/**
 ** write the list in the stream
 **
 ** @param     ostr   output stream
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.6
 **/
void
WeightedCardList::write(ostream& ostr) const
{
  for (WeightedCardList::const_iterator wc = this->begin();
       wc != this->end();
       ++wc) {
    ostr << std::setw(5) << wc->first << ' ';
    char c = ' ';
    for (unsigned tcolor = 0; tcolor < Card::NUMBER_OF_TCOLORS; ++tcolor) {
      if (wc == this->tcolor_ptr[tcolor]) {
        c = ::name(static_cast<Card::TColor>(tcolor))[0];
        break;
      }
    }
    ostr << c << ' ' << wc->second << '\n';
  }
  ostr << '\n';
  for (unsigned tcolor = 0; tcolor < Card::NUMBER_OF_TCOLORS; ++tcolor) {
    ostr << setw(7) << static_cast<Card::TColor>(tcolor) << "  "
      << setw(2) << this->tcolor_pos[tcolor] << ' ';

    Card const& highest = this->highest(static_cast<Card::TColor>(tcolor));
    if (highest.is_empty())
      ostr << "--\n";
    else
      ostr << highest << '\n';
  }

  return ;
} // void WeightedCardList::write(ostream& ostr) const

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    the number of cards
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.6
 **/
unsigned
WeightedCardList::cards_no() const
{
  return this->size();
} // unsigned WeightedCardList::cards_no() const

/**
 ** add the card
 **
 ** @param     card        card to add
 ** @param     weighting   the weighting of the card
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.6
 **/
void
WeightedCardList::add(Card const& card, int const weighting)
{
  // pointer to the position to insert the card
  WeightedCardList::iterator wc = this->begin();
  // position of the card
  int pos = 0;
  // search the position to insert the card
  for (; wc != this->end(); ++wc, ++pos)
    if (weighting >= wc->first)
      break;
  wc = this->insert(wc, pair<int, Card>(weighting, card));

  { // update the pos of the highest tcolors
    for (unsigned tcolor = 0; tcolor < Card::NUMBER_OF_TCOLORS; ++tcolor)
      if (this->tcolor_pos[tcolor] >= pos)
        this->tcolor_pos[tcolor]  += 1;
  } // update the pos of the highest tcolors

  { // update the highest pointer of the tcolor of the card
    Card::TColor const tcolor = card.tcolor(this->game());
    if (   (this->tcolor_pos[tcolor] == -1)
        || (this->tcolor_pos[tcolor] >= pos) ) {
      this->tcolor_pos[tcolor] = pos;
      this->tcolor_ptr[tcolor] = wc;
    } // if (this->tcolor_pos[tcolor] >= pos)
  } // update the highest pointer of the tcolor of the card

  return ;
} // void WeightedCardList::add(Card const& card, int const weighting)

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    the card with the highest weighting
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.6
 **/
Card const&
WeightedCardList::highest() const
{
  if (this->empty())
    return Card::EMPTY;

  return this->front().second;
} // Card WeightedCardList::highest() const

/**
 ** -> result
 **
 ** @param     tcolor   tcolor to check
 **
 ** @return    the card with the highest weighting of 'tcolor'
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.6
 **/
Card const&
WeightedCardList::highest(Card::TColor const tcolor) const
{
  if (this->tcolor_ptr[tcolor] == this->end())
    return Card::EMPTY;

  return this->tcolor_ptr[tcolor]->second;
} // Card WeightedCardList::highest(Card::TColor const tcolor) const

/**
 ** -> result
 **
 ** @param     tcolor   tcolor not to check
 **
 ** @return    the card with the highest weighting not of 'tcolor'
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.6
 **/
Card const&
WeightedCardList::highest_not_of(Card::TColor const tcolor) const
{
  WeightedCardList::iterator const& ptr = this->highest_ptr_not_of(tcolor);
  if (ptr == this->end())
    return Card::EMPTY;

  return ptr->second;
} // Card WeightedCardList::highest_not_of(Card::TColor const tcolor) const

/**
 ** -> result
 **
 ** @param     tcolor_a   first tcolor to check
 ** @param     tcolor_b   second tcolor to check
 **
 ** @return    the card with the highest weighting of 'tcolor_a' or 'tcolor_b'
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.6
 **/
Card const&
WeightedCardList::highest(Card::TColor const tcolor_a,
                          Card::TColor const tcolor_b) const
{
  WeightedCardList::iterator const& ptr
    = this->highest_ptr(tcolor_a, tcolor_b);
  if (ptr == this->end())
    return Card::EMPTY;

  return ptr->second;
} // Card WeightedCardList::highest(Card::TColor tcolor_a, Card::TColor tcolor_b) const

/**
 ** -> result
 **
 ** @param     tcolor_a   first tcolor to check
 ** @param     tcolor_b   second tcolor to check
 ** @param     tcolor_c   third tcolor to check
 **
 ** @return    the card with the highest weighting of 'tcolor_'.
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.6
 **/
Card const&
WeightedCardList::highest(Card::TColor const tcolor_a,
                          Card::TColor const tcolor_b,
                          Card::TColor const tcolor_c) const
{
  WeightedCardList::iterator const& ptr
    = this->highest_ptr(tcolor_a, tcolor_b, tcolor_c);
  if (ptr == this->end())
    return Card::EMPTY;

  return ptr->second;
} // Card WeightedCardList::highest(Card::TColor tcolor_a, Card::TColor tcolor_b, Card::TColor tcolor_c) const

/**
 ** -> result
 **
 ** @param     tcolor_a   first tcolor to check
 ** @param     tcolor_b   second tcolor to check
 ** @param     tcolor_c   third tcolor to check
 ** @param     tcolor_d   fourth tcolor to check
 **
 ** @return    the card with the highest weighting of 'tcolor_'.
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.6
 **/
Card const&
WeightedCardList::highest(Card::TColor const tcolor_a,
                          Card::TColor const tcolor_b,
                          Card::TColor const tcolor_c,
                          Card::TColor const tcolor_d) const
{
  WeightedCardList::iterator const& ptr
    = this->highest_ptr(tcolor_a, tcolor_b, tcolor_c, tcolor_d);
  if (ptr == this->end())
    return Card::EMPTY;

  return ptr->second;
} // Card WeightedCardList::highest(Card::TColor tcolor_a, Card::TColor tcolor_b, Card::TColor tcolor_c, Card::TColor tcolor_d) const

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    the highest weighting
 **            INT_MIN, if none exists
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.6
 **/
int
WeightedCardList::highest_weighting() const
{
  if (this->empty())
    return INT_MIN;

  return this->front().first;
} // int WeightedCardList::highest_weighting() const

/**
 ** -> result
 **
 ** @param     tcolor   tcolor
 **
 ** @return    the highest weighting of tcolor
 **            INT_MIN, if none exists
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.6
 **/
int
WeightedCardList::highest_weighting(Card::TColor const tcolor) const
{
  if (this->tcolor_ptr[tcolor] == this->end())
    return INT_MIN;

  return this->tcolor_ptr[tcolor]->first;
} // int WeightedCardList::highest_weighting(Card::TColor tcolor) const

/**
 ** -> result
 **
 ** @param     tcolor   tcolor
 **
 ** @return    the highest weighting not of tcolor
 **            INT_MIN, if none exists
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.6
 **/
int
WeightedCardList::highest_weighting_not_of(Card::TColor const tcolor) const
{
  WeightedCardList::iterator const& ptr = this->highest_ptr_not_of(tcolor);
  if (ptr == this->end())
    return INT_MIN;

  return ptr->first;
} // int WeightedCardList::highest_weighting_not_of(Card::TColor tcolor) const

/**
 ** -> result
 **
 ** @param     tcolor_a   first tcolor to check
 ** @param     tcolor_b   second tcolor to check
 **
 ** @return    the highest weighting of 'tcolor_a' or 'tcolor_b'
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.6
 **/
int
WeightedCardList::highest_weighting(Card::TColor const tcolor_a,
                                    Card::TColor const tcolor_b) const
{
  WeightedCardList::iterator const& ptr
    = this->highest_ptr(tcolor_a, tcolor_b);
  if (ptr == this->end())
    return INT_MIN;

  return ptr->first;
} // int WeightedCardList::highest_weighting(Card::TColor tcolor_a, Card::TColor tcolor_b) const

/**
 ** -> result
 **
 ** @param     tcolor_a   first tcolor to check
 ** @param     tcolor_b   second tcolor to check
 ** @param     tcolor_c   third tcolor to check
 **
 ** @return    the highest weighting of 'tcolor_'.
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.6
 **/
int
WeightedCardList::highest_weighting(Card::TColor const tcolor_a,
                                    Card::TColor const tcolor_b,
                                    Card::TColor const tcolor_c) const
{
  WeightedCardList::iterator const& ptr
    = this->highest_ptr(tcolor_a, tcolor_b, tcolor_c);
  if (ptr == this->end())
    return INT_MIN;

  return ptr->first;
} // int WeightedCardList::highest_weighting(Card::TColor tcolor_a, Card::TColor tcolor_b, Card::TColor tcolor_c) const

/**
 ** -> result
 **
 ** @param     tcolor_a   first tcolor to check
 ** @param     tcolor_b   second tcolor to check
 ** @param     tcolor_c   third tcolor to check
 ** @param     tcolor_d   fourth tcolor to check
 **
 ** @return    the card with the highest weighting of 'tcolor_'.
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.6
 **/
int
WeightedCardList::highest_weighting(Card::TColor const tcolor_a,
                                    Card::TColor const tcolor_b,
                                    Card::TColor const tcolor_c,
                                    Card::TColor const tcolor_d) const
{
  WeightedCardList::iterator const& ptr
    = this->highest_ptr(tcolor_a, tcolor_b, tcolor_c, tcolor_d);
  if (ptr == this->end()) {
    return INT_MIN;
  }

  return ptr->first;
} // int WeightedCardList::highest_weighting(Card::TColor tcolor_a, Card::TColor tcolor_b, Card::TColor tcolor_c, Card::TColor tcolor_d) const

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    the pointer to the highest weighting
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.6
 **/
WeightedCardList::const_iterator
WeightedCardList::highest_ptr() const
{
  return this->begin();
} // WeightedCardList::const_iterator WeightedCardList::highest_ptr() const

/**
 ** -> result
 **
 ** @param     tcolor   tcolor to check
 **
 ** @return    the pointer to the highest weighting of 'tcolor'
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.6
 **/
WeightedCardList::iterator const&
WeightedCardList::highest_ptr(Card::TColor const tcolor) const
{
  return this->tcolor_ptr[tcolor];
} // WeightedCardList::iterator WeightedCardList::highest_ptr(Card::TColor const tcolor) const

/**
 ** -> result
 **
 ** @param     tcolor_a   first tcolor to check
 ** @param     tcolor_b   second tcolor to check
 **
 ** @return    the ptr with the highest weighting of 'tcolor_a' or 'tcolor_b'
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.6
 **/
WeightedCardList::iterator const&
WeightedCardList::highest_ptr(Card::TColor const tcolor_a,
                              Card::TColor const tcolor_b) const
{
  if (this->tcolor_ptr[tcolor_a] == this->end())
    return this->tcolor_ptr[tcolor_b];

  if (this->tcolor_ptr[tcolor_b] == this->end())
    return this->tcolor_ptr[tcolor_a];

  if (this->tcolor_pos[tcolor_a] < this->tcolor_pos[tcolor_b])
    return this->tcolor_ptr[tcolor_a];
  else
    return this->tcolor_ptr[tcolor_b];
} // WeightedCardList::iterator WeightedCardList::highest_ptr(Card::TColor tcolor_a, Card::TColor tcolor_b) const

/**
 ** -> result
 **
 ** @param     tcolor_a   first tcolor to check
 ** @param     tcolor_b   second tcolor to check
 ** @param     tcolor_c   third tcolor to check
 **
 ** @return    the ptr with the highest weighting of 'tcolor_'.
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.6
 **/
WeightedCardList::iterator const&
WeightedCardList::highest_ptr(Card::TColor const tcolor_a,
                              Card::TColor const tcolor_b,
                              Card::TColor const tcolor_c) const
{
  if (this->tcolor_ptr[tcolor_a] == this->end())
    return this->highest_ptr(tcolor_b, tcolor_c);

  if (this->tcolor_ptr[tcolor_b] == this->end())
    return this->highest_ptr(tcolor_a, tcolor_c);

  if (this->tcolor_pos[tcolor_a] < this->tcolor_pos[tcolor_b])
    return this->highest_ptr(tcolor_a, tcolor_c);
  else
    return this->highest_ptr(tcolor_b, tcolor_c);
} // WeightedCardList::iterator WeightedCardList::highest_ptr(Card::TColor tcolor_a, Card::TColor tcolor_b, Card::TColor tcolor_c) const

/**
 ** -> result
 **
 ** @param     tcolor_a   first tcolor to check
 ** @param     tcolor_b   second tcolor to check
 ** @param     tcolor_c   third tcolor to check
 ** @param     tcolor_d   fourth tcolor to check
 **
 ** @return    the ptr with the highest weighting of 'tcolor_'.
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.6
 **/
WeightedCardList::iterator const&
WeightedCardList::highest_ptr(Card::TColor const tcolor_a,
                              Card::TColor const tcolor_b,
                              Card::TColor const tcolor_c,
                              Card::TColor const tcolor_d) const
{
  if (this->tcolor_ptr[tcolor_a] == this->end())
    return this->highest_ptr(tcolor_b, tcolor_c, tcolor_d);

  if (this->tcolor_ptr[tcolor_b] == this->end())
    return this->highest_ptr(tcolor_a, tcolor_c, tcolor_d);

  if (this->tcolor_pos[tcolor_a] < this->tcolor_pos[tcolor_b])
    return this->highest_ptr(tcolor_a, tcolor_c, tcolor_d);
  else
    return this->highest_ptr(tcolor_b, tcolor_c, tcolor_d);
} // WeightedCardList::iterator WeightedCardList::highest_ptr(Card::TColor tcolor_a, Card::TColor tcolor_b, Card::TColor tcolor_c, Card::TColor tcolor_d) const

/**
 ** -> result
 **
 ** @param     tcolor   tcolor not to check
 **
 ** @return    the pointer to the highest weighting not of 'tcolor'
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.6
 **/
WeightedCardList::iterator const&
WeightedCardList::highest_ptr_not_of(Card::TColor const tcolor) const
{
  Card::TColor const tc = this->highest().tcolor(this->game());
  if (tc != tcolor)
    return this->highest_ptr(tc);

  return this->highest_ptr(( (tcolor == Card::DIAMOND)
                            ? Card::HEART
                            : Card::DIAMOND),
                           (  (tcolor <= Card::HEART)
                            ? Card::SPADE
                            : Card::HEART),
                           (  (tcolor <= Card::SPADE)
                            ? Card::CLUB
                            : Card::SPADE),
                           (  (tcolor <= Card::CLUB)
                            ? Card::TRUMP
                            : Card::CLUB)
                          );
} // WeightedCardList::iterator WeightedCardList::highest_ptr_not_of(Card::TColor const tcolor) const

/**
 ** -> result
 **
 ** @param     card   card to return the (heighest) weighting of
 **
 ** @return    the (highest) weighting weighting of 'card'
 **            INT_MIN, if none exists
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.6
 **/
int
WeightedCardList::weighting(Card const& card) const
{
  // search the card in the pointers
  for (WeightedCardList::const_iterator ptr
       = this->tcolor_ptr[card.tcolor(this->game())];
       ptr != this->end();
       ++ptr)
    if (ptr->second == card)
      return ptr->first;

  return INT_MIN;
} // int WeightedCardList::weighting(Card card) const

/**
 ** remove the highest card
 **
 ** @param     -
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.6
 **/
void
WeightedCardList::pop_highest()
{
  if (this->empty())
    return ;

  // the tcolor of the card to remove
  Card::TColor const tcolor = this->front().second.tcolor(this->game());

  // remove the card
  this->pop_front();

  // adjust the positions
  for (unsigned tc = 0; tc < Card::NUMBER_OF_TCOLORS; ++tc)
    if (this->tcolor_pos[tc] >= 0)
      this->tcolor_pos[tc] -= 1;

  // search the first of 'tcolor' in the list
  this->recreate_tcolor_ptr(tcolor);

  return ;
} // void WeightedCardList::pop_highest()

/**
 ** remove the highest card of the tcolor
 **
 ** @param     tcolor   tcolor to remove the highest card from
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.6
 **/
void
WeightedCardList::pop_highest(Card::TColor const tcolor)
{
  // search the card in the pointers
  WeightedCardList::iterator& ptr = this->tcolor_ptr[tcolor];
  if (ptr == this->end())
    return ;

  int& pos = this->tcolor_pos[tcolor];
  // update the pos of the further tcolors
  for (unsigned tc = 0; tc < Card::NUMBER_OF_TCOLORS; ++tc)
    if (this->tcolor_pos[tc] > pos)
      this->tcolor_pos[tc]  += 1;
  // remove the pointer
  ptr = this->erase(ptr);
  for (; ptr != this->end(); ++ptr, ++pos)
    if (ptr->second.tcolor(this->game()) == tcolor)
      break;
  if (ptr == this->end())
    pos = -1;

  return ;
} // void WeightedCardList::pop_highest(Card::TColor tcolor)

/**
 ** remove the highest card not of the tcolor
 **
 ** @param     tcolor   tcolor to remove the highest card from
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.6
 **
 ** @todo      own code (for better performance)
 **/
void
WeightedCardList::pop_highest_not_of(Card::TColor const tcolor)
{
  this->pop(this->highest_not_of(tcolor));

  return ;
} // void WeightedCardList::pop_highest_not_of(Card::TColor tcolor)

/**
 ** remove the given card
 **
 ** @param     card   card to remove
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.6
 **/
void
WeightedCardList::pop(Card const& card)
{
  // search the card in the pointers
  Card::TColor const tcolor = card.tcolor(this->game());
  WeightedCardList::iterator& ptr = this->tcolor_ptr[tcolor];
  if (  (ptr != this->end())
      && (ptr->second == card) ) {
    int& pos = this->tcolor_pos[tcolor];
    // update the pos of the further tcolors
    for (unsigned tc = 0; tc < Card::NUMBER_OF_TCOLORS; ++tc)
      if (this->tcolor_pos[tc] > pos)
        this->tcolor_pos[tc]  += 1;
    // remove the pointer
    ptr = this->erase(ptr);
    for (; ptr != this->end(); ++ptr, ++pos)
      if (ptr->second.tcolor(this->game()) == tcolor)
        break;
    if (ptr == this->end())
      pos = -1;
  } else { // if !(this->tcolor_ptr[tcolor].second == card)
    int pos = this->tcolor_pos[tcolor];
    for (; ptr != this->end(); ++ptr, ++pos)
      if (ptr->second == card)
        break;
    DEBUG_ASSERTION(ptr != this->end(),
                    "WeightedCardList::pop(card)\n"
                    "  card '" << card << "' not found in the list:\n"
                    << *this);
    ptr = this->erase(ptr);
    // update the pos of the further tcolors
    for (unsigned tc = 0; tc < Card::NUMBER_OF_TCOLORS; ++tc)
      if (this->tcolor_pos[tc] > pos)
        this->tcolor_pos[tc] -= 1;
  } // if !(this->tcolor_ptr[tcolor].second == card)

  return ;
} // void WeightedCardList::pop(Card card)

/**
 ** recreate the pointers to the highest card of each tcolor
 **
 ** @param     -
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.6
 **/
void
WeightedCardList::recreate_tcolor_ptrs()
{
  // search the first of 'tcolor' in the list
  for (unsigned tcolor = 0; tcolor < Card::NUMBER_OF_TCOLORS; ++tcolor)
    this->recreate_tcolor_ptr(static_cast<Card::TColor>(tcolor));
  return ;
} // void WeightedCardList::recreate_tcolor_ptrs()

/**
 ** recreate the pointer to the highest card of the tcolor
 **
 ** @param     tcolor   tcolor to recreate the pointer of
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.6
 **/
void
WeightedCardList::recreate_tcolor_ptr(Card::TColor const tcolor)
{
  // search the first of 'tcolor' in the list
  int& pos = this->tcolor_pos[tcolor];
  WeightedCardList::iterator& ptr = this->tcolor_ptr[tcolor];
  for (pos = 0, ptr = this->begin(); ptr != this->end(); ++ptr, ++pos)
    if (ptr->second.tcolor(this->game()) == tcolor)
      break;
  if (ptr == this->end())
    pos = -1;
  return ;
} // void WeightedCardList::recreate_tcolor_ptr(Card::TColor tcolor)

/**
 ** write 'card_list' in 'ostr'
 **
 ** @param     ostr        output stream
 ** @param     card_list   card list to write
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.6
 **/
ostream&
operator<<(ostream& ostr, WeightedCardList const& card_list)
{
  card_list.write(ostr);
  return ostr;
} // ostream& operator<<(ostream& ostr, WeightedCardList card_list)
