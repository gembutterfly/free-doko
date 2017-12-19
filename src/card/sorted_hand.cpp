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
 *********************************************************************/

#include "constants.h"

#include "sorted_hand.h"

#include "hand.h"

#include "../misc/setting.h"

/**
 **
 ** constructor
 **
 ** @param	hand	the corresponding hand
 **
 ** @return	-
 **
 ** @version	0.6.4
 **
 ** @author	Diether Knof
 **
 **/
SortedHand::SortedHand(Hand& hand) :
  hand_(&hand),
  to_sorted_pos_(hand.cardsnumber_all()),
  from_sorted_pos_(hand.cardsnumber_all()),
  mixed(::setting(Setting::CARDS_ORDER).mixed())
{
  for (unsigned i = 0; i < this->cardsnumber_all(); ++i)
    this->to_sorted_pos_.push_back(i);

  this->set_sorting();

  return ;
} // SortedHand::SortedHand(Hand& hand)

/**
 **
 ** constructor
 **
 ** @param	sorted_hand	the sorted hand to copy
 ** @param	hand		the corresponding hand
 **
 ** @return	-
 **
 ** @version	0.6.4
 **
 ** @author	Diether Knof
 **
 **/
SortedHand::SortedHand(SortedHand const& sorted_hand, Hand& hand) :
  hand_(&hand),
  to_sorted_pos_(sorted_hand.to_sorted_pos_),
  from_sorted_pos_(sorted_hand.from_sorted_pos_),
  mixed(sorted_hand.mixed)
{
  return ;
} // SortedHand::SortedHand(SortedHand const& sorted_hand, Hand& hand) :

/**
 **
 ** constructor
 **
 ** @param	sorted_hand	the sorted hand to copy
 **
 ** @return	-
 **
 ** @version	0.6.7
 **
 ** @author	Diether Knof
 **
 **/
SortedHand::SortedHand(SortedHand const& sorted_hand) :
  hand_(sorted_hand.hand_),
  to_sorted_pos_(sorted_hand.to_sorted_pos_),
  from_sorted_pos_(sorted_hand.from_sorted_pos_),
  mixed(sorted_hand.mixed)
{
  return ;
} // SortedHand::SortedHand(SortedHand const& sorted_hand, Hand& hand) :

/**
 **
 ** setting equal
 **
 ** @param	sorted_hand	the sorted hand to copy
 **
 ** @return	-
 **
 ** @version	0.6.4
 **
 ** @author	Diether Knof
 **
 **/
SortedHand&
SortedHand::operator=(SortedHand const& sorted_hand)
{
  this->hand_ = sorted_hand.hand_;
  this->to_sorted_pos_ = sorted_hand.to_sorted_pos_;
  this->from_sorted_pos_ = sorted_hand.from_sorted_pos_;
  this->mixed = sorted_hand.mixed;

  return *this;
} // SortedHand& SortedHand::operator=(SortedHand const& sorted_hand) :

/**
 **
 ** destructor
 **
 ** @param	-
 **
 ** @return	-
 **
 ** @version	0.6.2
 **
 ** @author	Diether Knof
 **
 **/
SortedHand::~SortedHand()
{
  return ;
} // SortedHand::~SortedHand()

/**
 **
 ** -> result
 **
 ** @param	-
 **
 ** @return	the number of (unplayed) cards
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.4
 **
 **/
unsigned
SortedHand::cardsnumber() const
{
  return this->hand().cardsnumber();
} // unsigned SortedHand::cardsnumber() const

/**
 **
 ** -> result
 **
 ** @param	-
 **
 ** @return	the number of (all) cards
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.4
 **
 **/
unsigned
SortedHand::cardsnumber_all() const
{
  return this->hand().cardsnumber_all();
} // unsigned SortedHand::cardsnumber_all() const

/**
 **
 ** -> result
 **
 ** @param	i	number of card
 **
 ** @return	card i of the hand (with remaining cards)
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.2
 **
 **/
HandCard const&
SortedHand::card(unsigned const i) const
{
  return this->hand().card(this->from_sorted_pos(i));
} // HandCard const& SortedHand::card(unsigned const i) const

/**
 **
 ** -> result
 **
 ** @param	i	cardnumber
 **
 ** @return	the card number 'i'
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.2
 **
 **/
HandCard const&
SortedHand::card_all(unsigned const i) const
{
  DEBUG_ASSERTION((i < this->cardsnumber_all()),
		  "SortedHand::card_all(i):\n"
		  "  illegal value " << i
		  << " (maximal value is: "
		  << this->cardsnumber_all() - 1 << ")");

  return this->hand().card_all(this->from_sorted_pos_all(i));
} // HandCard const& SortedHand::card(unsigned const i) const

/**
 **
 ** -> result
 **
 ** @param	i	cardnumber
 **
 ** @return	whether the card 'i' has been played
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.2
 **
 **/
bool
SortedHand::played(unsigned const i) const
{
  DEBUG_ASSERTION((i < this->cardsnumber_all()),
		  "SortedHand::played(i):\n"
		  "  illegal value " << i
		  << " (maximal value is: "
		  << this->cardsnumber_all() - 1 << ")");

  return this->hand().played(this->from_sorted_pos_all(i));
} // bool SortedHand::played(unsigned const i) const

/**
 **
 ** -> result
 **
 ** @param	i	number of card
 **
 ** @return	in which trick the card has been played
 **		(UINT_MAX, if not played, yet)
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.2
 **
 **/
unsigned
SortedHand::played_trick(unsigned const i) const
{
  DEBUG_ASSERTION((i < this->cardsnumber_all()),
		  "SortedHand::played_trick(i):\n"
		  "  illegal value " << i
		  << " (maximal value is: "
		  << this->cardsnumber_all() - 1 << ")");

  return this->hand().played_trick(this->from_sorted_pos_all(i));
} // unsigned SortedHand::played_trick(i)

/**
 **
 ** -> result
 **
 ** @param	i	number of card
 **
 ** @return	real position of the card (in the played cards)
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.4
 **
 **/
unsigned
SortedHand::from_sorted_pos(unsigned const i) const
{
  DEBUG_ASSERTION((i < this->cardsnumber()),
		  "SortedHand::from_sorted_pos(i):\n"
		  "  illegal value " << i
		  << " (maximal value is: "
		  << this->cardsnumber() - 1 << ")");

  return this->hand().pos_all_to_pos(this->from_sorted_pos_all(this->pos_to_pos_all(i)));
} // unsigned SortedHand::from_sorted_pos(unsigned const i) const

/**
 **
 ** -> result
 **
 ** @param	i	number of card
 **
 ** @return	real position of the card (in all cards)
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.2
 **
 **/
unsigned
SortedHand::from_sorted_pos_all(unsigned const i) const
{
  DEBUG_ASSERTION((i < this->cardsnumber_all()),
		  "SortedHand::from_sorted_pos_all(i):\n"
		  "  illegal value " << i
		  << " (maximal value is: "
		  << this->cardsnumber_all() - 1 << ")");

  return this->from_sorted_pos_[i];
} // unsigned SortedHand::from_sorted_pos_all(unsigned const i) const

/**
 **
 ** -> result
 **
 ** @param	i	number of card
 **
 ** @return	translated position of the card (real pos is 'i')
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.4
 **
 **/
unsigned
SortedHand::to_sorted_pos(unsigned const i) const
{
  if (i == UINT_MAX)
    return UINT_MAX;

  DEBUG_ASSERTION((i < this->cardsnumber()),
		  "SortedHand::to_sorted_pos(i):\n"
		  "  illegal value " << i
		  << " (maximal value is: "
		  << this->cardsnumber() - 1 << ")");

  return this->hand().pos_all_to_pos(this->to_sorted_pos_all(this->pos_to_pos_all(i)));
} // unsigned SortedHand::to_sorted_pos(unsigned const i) const

/**
 **
 ** -> result
 **
 ** @param	i	number of card
 **
 ** @return	translated position of the card (real pos is 'i')
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.4
 **
 **/
unsigned
SortedHand::to_sorted_pos_all(unsigned const i) const
{
  if (i == UINT_MAX)
    return UINT_MAX;

  DEBUG_ASSERTION((i < this->cardsnumber_all()),
		  "SortedHand::to_sorted_pos_all(i):\n"
		  "  illegal value " << i
		  << " (maximal value is: "
		  << this->cardsnumber_all() - 1 << ")");

  return this->to_sorted_pos_[i];
} // unsigned SortedHand::to_sorted_pos_all(unsigned const i) const

/**
 **
 ** -> result
 ** The pos of the argument is the pos in the unplayed cards.
 ** It is searched, what position in all cards 'pos' stands for.
 **
 ** @param	pos	position in the unplayed card
 **
 ** @return	position the played cards
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.4
 **
 **/
unsigned
SortedHand::pos_to_pos_all(unsigned pos) const
{
  if (pos == UINT_MAX)
    return UINT_MAX;

  unsigned n;
  // translate the position
  for (n = 0, pos += 1; pos > 0; ++n)
    if (!this->hand().played(this->from_sorted_pos_all(n)))
      pos -= 1;

  return (n - 1);
} // unsigned SortedHand::pos_to_pos_all(unsigned pos)

/**
 **
 ** -> result
 ** The pos of the argument is the pos in all cards.
 ** It is searched, what position in the remaining cards 'pos' stands for.
 **
 ** @param	pos	position in all card
 **
 ** @return	position in the unplayed cards
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.4
 **
 **/
unsigned
SortedHand::pos_all_to_pos(unsigned pos) const
{
  if (pos == UINT_MAX)
    return UINT_MAX;

  unsigned n;
  // translate the position
  for (n = 0, pos += 1; pos > 0; pos--)
    if (!this->hand().played(this->from_sorted_pos_all(pos - 1)))
      n++;

  return (n - 1);
} // unsigned SortedHand::pos_all_to_pos(unsigned pos)

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	the requested card (to be played next)
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.8
 **/
HandCard
SortedHand::requested_card() const
{
  return this->hand().requested_card();
} // HandCard Hand::requested_card() const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	the position of the requested card (to be played next)
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.3
 **/
unsigned
SortedHand::requested_position() const
{
  return this->to_sorted_pos(this->hand().requested_position());
} // unsigned Hand::requested_position() const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	the position of the requested card (to be played next)
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.3
 **/
unsigned
SortedHand::requested_position_all() const
{
  return this->to_sorted_pos_all(this->hand().requested_position_all());
} // unsigned Hand::requested_position_all() const

/**
 **
 ** -> result
 ** The pos of the argument is the pos in all cards.
 ** It is searched, what position in the remaining cards 'pos' stands for.
 **
 ** @param	pos_all   position of the card to request (in all cards)
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.4
 **
 **/
void
SortedHand::request_position_all(unsigned const pos_all)
{
  this->hand().request_position_all(this->from_sorted_pos_all(pos_all));

  return ;
} // void SortedHand::request_position_all(unsigned const pos_all)

/**
 **
 ** marks card 'c' as played
 **
 ** @param	c	card to play
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.4
 **
 **/
void
SortedHand::playcard(HandCard const& c)
{
  this->hand().playcard(c);

  return ;
} // void SortedHand::playcard(HandCard const& c)

/**
 **
 ** marks card at 'pos' as played
 **
 ** @param	pos	position of the card to play
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.4
 **
 **/
void
SortedHand::playcard(unsigned const pos)
{
  this->hand().playcard(this->from_sorted_pos(pos));

  return ;
} // void SortedHand::playcard(unsigned const pos)

/**
 **
 ** marks a played 'card' as not played
 **
 ** @param	c card to unmark
 **
 ** @return	-
 **
 ** @version	0.5
 **
 ** @author	Diether Knof
 **
 **/
void
SortedHand::unplaycard(HandCard const& c)
{
  this->hand().unplaycard(c);

  return ;
} // void SortedHand::unplaycard(HandCard const& c)

/**
 **
 ** adds the cards to the hand
 **
 ** @param	cards	cards to be added
 **
 ** @return	-
 **
 ** @version	0.6.8
 **
 ** @author	Diether Knof
 **
 **/
void
SortedHand::add(vector<Card> const& cards)
{
  // here we know, that the cards are added at the end of the hand
  for (unsigned c = 0; c < cards.size(); c++) {
    this->to_sorted_pos_.push_back(c + this->cardsnumber());
    this->from_sorted_pos_.push_back(c + this->cardsnumber());
  } // for (c < hand.cardsnumber_all())
  this->hand().add(cards);

  if (!this->mixed)
    this->sort();

  return ;
} // void SortedHand::add(vector<Card> const& cards)

/**
 **
 ** adds the cards to the hand
 **
 ** @param	cards	cards to be added
 **
 ** @return	-
 **
 ** @version	0.6.8
 **
 ** @author	Diether Knof
 **
 **/
void
SortedHand::add(HandCards const& cards)
{
  this->add(static_cast<vector<Card> >(cards));

  return ;
} // void SortedHand::add(HandCards const& cards)

/**
 **
 ** removes 'card' from the hand
 **
 ** @param	card	card to remove
 **
 ** @return	position of the removed card
 **
 ** @version	0.6.4
 **
 ** @author	Diether Knof
 **
 **/
unsigned
SortedHand::remove(HandCard const& card)
{
  // the position in the hand of the removed card
  unsigned const pos_all = this->hand().getpos_all(card);
  // the position in the sorted hand of the removed card
  unsigned const sorted_pos_all = this->to_sorted_pos_all(pos_all);

  this->hand().remove(card);

  // update the sorting vectors
  //vector<unsigned> to_sorted_pos_;
  //vector<unsigned> from_sorted_pos_;

  this->from_sorted_pos_.erase(this->from_sorted_pos_.begin()
			       + sorted_pos_all);
  for (vector<unsigned>::iterator p = this->from_sorted_pos_.begin();
       p != this->from_sorted_pos_.end();
       ++p)
    if (*p > pos_all)
      *p -= 1;

  this->to_sorted_pos_.erase(this->to_sorted_pos_.begin() + pos_all);
  for (vector<unsigned>::iterator p = this->to_sorted_pos_.begin();
       p != this->to_sorted_pos_.end();
       ++p)
    if (*p > sorted_pos_all)
      *p -= 1;

  return sorted_pos_all;
} // unsigned SortedHand::remove(HandCard const& card)

/**
 **
 ** removes the cards 'cards' from the hand
 **
 ** @param	cards	cards to be removeed
 **
 ** @return	-
 **
 ** @version	0.6.4
 **
 ** @author	Diether Knof
 **
 **/
void
SortedHand::remove(HandCards const& cards)
{
  for (HandCards::const_iterator c = cards.begin();
       c != cards.end();
       c++)
    this->remove(*c);

  return ;
} // void SortedHand::remove(HandCards const& cards)

/**
 **
 ** the hand has been changed - update the sorting
 **
 ** @param	-
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.4
 **
 **/
void
SortedHand::set_sorting()
{
  if (this->mixed)
    this->mix();
  else
    this->sort();

  return ;
} // void SortedHand::set_sorting()

/**
 **
 ** sorts the cards (as set in the settings)
 **
 ** @param	-
 **
 ** @return	whether the order has changed
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.4
 **
 ** @todo       overview, especially the return value
 **/
bool
SortedHand::sort()
{
  this->from_sorted_pos_.clear();
  for (unsigned i = 0; i < this->cardsnumber_all(); ++i)
    this->from_sorted_pos_.push_back(i);

  bool changed = false;
  if (::setting(Setting::CARDS_ORDER).sorted()) {
    // a bad bubble sort, but we have at max 12 cards

    for (unsigned c1 = 0; c1 < this->cardsnumber_all(); ++c1)
      for (unsigned c2 = c1 + 1; c2 < this->cardsnumber_all(); ++c2)
	if (HandCard::relative_position(this->hand().card_all(this->from_sorted_pos_[c1]),
					this->hand().card_all(this->from_sorted_pos_[c2]))
	    > 0) {
	  std::swap(this->from_sorted_pos_[c1], this->from_sorted_pos_[c2]);
	  changed = true;
	}

    this->set_positions();
  } // bubble sort over all cards

  this->mixed = false;

  return changed;
} // changed SortedHand::sort()

/**
 **
 ** unsort the cards (make them the original order)
 **
 ** @param	-
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.4
 **
 **/
void
SortedHand::unsort()
{
  this->from_sorted_pos_.clear();
  for (unsigned i = 0; i < this->cardsnumber_all(); ++i)
    this->from_sorted_pos_.push_back(i);

  this->set_positions();

  this->mixed = true;

  return ;
} // void SortedHand::unsort()

/**
 **
 ** mix the (order of the) cards
 **
 ** @param	-
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.4
 **
 **/
void
SortedHand::mix()
{
  if (!this->mixed) {
    this->unsort();
  } else { // if !(!this->mixed)
    this->from_sorted_pos_.clear();
    for (unsigned i = 0; i < this->cardsnumber_all(); ++i)
      this->from_sorted_pos_.push_back(i);
    if (::setting(Setting::CARDS_ORDER).mixed())
      for (unsigned c = 0; c < this->cardsnumber_all(); c++)
	std::swap(this->from_sorted_pos_[c], this->from_sorted_pos_[RAND(c + 1)]);

    this->set_positions();
  } // if !(!this->mixed)

  return ;
} // void SortedHand::mix()

/**
 **
 ** calculate the position of the cards from the hand
 **
 ** @param	-
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.4
 **
 **/
void
SortedHand::set_positions()
{
  this->to_sorted_pos_.resize(this->from_sorted_pos_.size());

  for (unsigned c = 0; c < this->cardsnumber_all(); c++)
    this->to_sorted_pos_[this->from_sorted_pos_[c]] = c;

  return ;
} // void SortedHand::set_positions()

/**
 **
 ** writes the (sorted) hand in 'ostr'
 **
 ** @param	ostr		stream to write to
 ** @param	sorted_hand	hand to write
 **
 ** @return	output stream
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.2
 **
 **/
ostream&
operator<<(ostream& ostr, SortedHand const& sorted_hand)
{
  for (unsigned i = 0; i < sorted_hand.cardsnumber_all(); i++) {
    ostr << std::hex << i << std::dec << "  " << sorted_hand.card_all(i);
    if (sorted_hand.played(i))
      ostr << "\t(played " << setw(2) << sorted_hand.played_trick(i) << ")";
    ostr << "\n";
  } // for (i < hand.cardsnumber_all())

  return ostr;
} // ostream& operator<<(ostream& ostr, SortedHand const& sorted_hand)
