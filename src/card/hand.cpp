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
#include "hand.h"

#include "trick.h"
#include "../player/player.h"
#include "../game/game.h"
#include "../party/rule.h"
#include "../misc/setting.h"
#include "../utils/string.h"

/**
 ** constructor
 **
 ** @param	-
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **/
Hand::Hand() :
  cards_(),
  cards_all_(),
  played_(),
  requested_position_(UINT_MAX),
  player_(NULL)
{
  return ;
} // Hand::Hand()

/**
 ** constructor
 **
 ** @param	player	player this hand belongs to
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.8
 **/
Hand::Hand(Player const& player):
  cards_(),
  cards_all_(),
  played_(),
  requested_position_(UINT_MAX),
  player_(&player)
{
  return ;
} // Hand::Hand(Player const& player)

/**
 ** constructor
 **
 ** @param	player	player this hand belongs to
 ** @param	hand	hand to copy
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **/
Hand::Hand(Player const& player, Hand const& hand):
  cards_(),
  cards_all_(),
  played_(),
  requested_position_(hand.requested_position()),
  player_(&player)
{
  this->add(hand);
  return ;
} // Hand::Hand(Player const& player, Hand const& hand)

/**
 ** constructor
 **
 ** @param	hand	hand to copy
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **/
Hand::Hand(Hand const& hand):
  cards_(),
  cards_all_(),
  played_(),
  requested_position_(hand.requested_position()),
  player_(hand.player_)
{
  this->add(hand);
  return ;
} // Hand::Hand(Hand)

/**
 ** constructor
 **
 ** @param	cards	cards of the hand
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **/
Hand::Hand(vector<Card> const& cards) :
  cards_(),
  cards_all_(),
  played_(),
  requested_position_(UINT_MAX),
  player_(NULL)
{
  this->add(cards);
  return ;
} // Hand::Hand(vector<Card> const& cards) :

/**
 ** constructor
 **
 ** @param	player	the player who owns the hand
 ** @param	cards	cards of the hand
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **/
Hand::Hand(Player const& player, vector<Card> const& cards) :
  cards_(),
  cards_all_(),
  played_(),
  requested_position_(UINT_MAX),
  player_(&player)
{
  this->add(cards);
  return ;
} // Hand::Hand(Player const& player, vector<Card> cards) :

/**
 ** constructor
 **
 ** @param	player	        the player who owns the hand
 ** @param	unplayed_cards	cards of the hand (not played)
 ** @param	played_cards	cards of the hand (played)
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.0
 **
 ** @todo	make faster
 **/
Hand::Hand(Player const& player,
           vector<Card> const& unplayed_cards,
           vector<Card> const& played_cards) :
  cards_(),
  cards_all_(),
  played_(),
  requested_position_(UINT_MAX),
  player_(&player)
{
  this->add_played(played_cards);
  this->add(unplayed_cards);

  return ;
} // Hand::Hand(Player player, vector<Card> unplayed_cards, vector<Card> played_cards) :

/**
 ** copy operator
 **
 ** @param	hand	hand to copy
 **
 ** @return	copied hand
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **/
Hand&
Hand::operator=(Hand const& hand)
{
  if (this == &hand)
    return *this;

  this->cards_.clear();
  this->cards_all_.clear();
  this->played_.clear();
  this->add(hand);
  this->requested_position_ = hand.requested_position();
  this->player_ = hand.player_;

  return *this;
} // Hand& Hand::operator=(Hand const& hand)

/**
 ** constructor:
 ** read the hand from the stream
 **
 ** @param	istr	Stream with the info
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.5.4
 **/
Hand::Hand(istream& istr) :
  cards_(),
  cards_all_(),
  played_(),
  requested_position_(UINT_MAX),
  player_()
{
  string line;
  while (istr.good()) {
    // read the lines until there is an empty line or eof
    getline(istr, line);
    if (!line.empty() && (*line.rbegin() == '\r'))
      line.erase(line.end() - 1);
    if (line.empty())
      break;

    // the line is as followed:
    // 0  diamond king		(played 2)
    // (number of the card, card)
    DK::Utils::String::word_first_remove(line);

    string card_name;
    card_name = DK::Utils::String::word_first(line);
    DK::Utils::String::word_first_remove(line);
    card_name += " ";
    card_name += DK::Utils::String::word_first(line);
    DK::Utils::String::word_first_remove(line);

    HandCard card(*this, card_name);
    if (card.is_empty())
      break;
    this->cards_.push_back(card);
    this->cards_all_.push_back(card);

    DK::Utils::String::remove_blanks(line);
    if (line.empty()) {
      // the card has not been played, yet
      this->played_.push_back(UINT_MAX);
    } else { // if (line.empty())
      // the card has been played
      // the line is now like: (played 1)
      if (DK::Utils::String::word_first(line) != "(played")
        break;
      DK::Utils::String::word_first_remove(line);
      DK::Utils::String::remove_blanks(line);

      this->played_.push_back(static_cast<unsigned>(atoi(line.c_str())));
    } // if !(line.empty())

  } // while (istr.good())

  return ;
} // Hand::Hand(istream& istr)

/**
 ** destructor
 **
 ** @param	-
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **/
Hand::~Hand()
{
  return ;
} // Hand::~Hand()

/** converts into HandCards
 **
 ** @result    HandCards
 **/
Hand::operator HandCards() const
{
  return this->cards();
} // Hand::operator HandCards() const

/** converts into a vector of Cards
 **
 ** @result    HandCards
 **/
Hand::operator vector<Card>() const
{
  return this->cards();
} // Hand::operator vector<Card>() const

/**
 ** self check
 ** when an error is found, an ASSERTION is created
 **
 ** @param	-
 **
 ** @return	wether the self-check was successful (no error)
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.8
 **/
bool
Hand::self_check() const
{
  { // check the cards with cards_all
    unsigned c = 0;
    for (unsigned ca = 0; ca < this->cardsnumber_all(); ++ca) {
      if (!this->played(ca)) {
        DEBUG_ASSERTION((this->card(c) == this->card_all(ca)),
                        "Hand::self_check():\n"
                        "  card '" << this->card(c) << "' on position " << c
                        << " is not the same as card all '"
                        << this->card_all(ca) << "' at position " << ca << "\n"
                        << *this);
        c += 1;
      } // if (!this->played(ca))
    }
  } // check the cards with cards_all

  { // check the hand of the cards
    for (HandCards::const_iterator c = this->cards_.begin();
         c != this->cards_.end();
         ++c)
      DEBUG_ASSERTION((&c->hand() == this),
                      "Hand::self_check():\n"
                      "  card '" << *c << "' does not belong to this hand.\n"
                      << *this);
    for (HandCards::const_iterator c = this->cards_all_.begin();
         c != this->cards_all_.end();
         ++c)
      DEBUG_ASSERTION((&c->hand() == this),
                      "Hand::self_check():\n"
                      "  card_all '" << *c
                      << "' does not belong to this hand.\n"
                      << *this);
  } // check the hand of the cards

  return true;
} // bool Hand::self_check() const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	whether the hand is empty (cardsnumber == 0)
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.13
 **/
bool
Hand::empty() const
{
  return (this->cardsnumber() == 0);
} // bool Hand::empty() const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	number of cards still in the hand
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **/
unsigned
Hand::cardsnumber() const
{
  return this->cards_.size();
} // unsigned Hand::cardsnumber() const

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    points on the hand
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.5
 **/
unsigned
Hand::points() const
{
  unsigned points = 0;

  for (HandCards::const_iterator c = this->cards().begin();
       c != this->cards().end();
       ++c)
    points += c->points();

  return points;
} // unsigned Hand::points() const


/**
 ** set an unknown card to a known one
 **
 ** @param     card   card to set
 ** @param     n      how many cards to set (default: 1)
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.13
 **/
void
Hand::set_unknown(Card const& card, unsigned const n)
{
  if (n == 0)
    return;

  DEBUG_ASSERTION(this->numberof(Card::UNKNOWN) >= n,
                  "Hand::set_unknown(" << card << ", " << n << ")\n"
                  "  not enough unknown cards:\n"
                  << *this);

  unsigned const pos = this->getpos(Card::UNKNOWN);
  this->cards_[pos] = card;
  this->cards_all_[this->pos_to_pos_all(pos)] = card;

  if (n > 1)
    this->set_unknown(card, n - 1);

  return ;
} // void Hand::set_unknown(Card card, unsigned n = 1)


/**
 ** -> result
 **
 ** @param	i	number of card
 **
 ** @return	card i of the hand (with remaining cards)
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 **
 ** @version	0.5.4
 **/
HandCard const&
Hand::card(unsigned const i) const
{
#ifdef WORKAROUND
  if ( (this->cardsnumber() < 3)
      && (i <= 3) ){
    DEBUG_ASSERTION((i < this->cardsnumber()),
                    "Hand::card(i):\n"
                    "  invalid value 'i' = " << i
                    << " (max = " << this->cardsnumber() << ")\n"
                    "Hand:\n"
                    << *this
                    << "\n"
                    << "You seem to encounter this bug in a poverty while changing the cards.\n"
                    << "We have received this error a few times but we were not able to reproduce it, so far.\n"
                    << "Please write us how the cards are lying (or send us a screenshot from the table) and tell us what your last action was (where you have clicked with which button with your mouse)."
                    << "Thank you\n"
                    << "--\n"
                    << "Sie scheinen diesen Fehler waehrend einer Armut bei dem Auswechseln der Karten zu erhalten.\n"
                    << "Wir haben schon haeufiger Meldungen darueber erhalten, konnten den aber bisher nicht nachvollziehen.\n"
                    << "Bitte schreiben Sie uns zusaetzlich zum Fehlerbericht, wie die Karten liegen (oder machen Sie einen Bildschirmabzug (screenshot) ) und schreiben Sie uns, welche Aktionen Sie zuletzt durchgefuehrt haben (wohin Sie mit welcher Maustaste geklickt haben).\n"
                    << "Vielen Dank");
  } // if (in poverty)
#endif

  DEBUG_ASSERTION((i < this->cardsnumber()),
                  "Hand::card(i):\n"
                  "  invalid value 'i' = " << i
                  << " (max = " << this->cardsnumber() << ")\n"
                  "Hand:\n"
                  << *this);

  DEBUG_ASSERTION(!this->cards_[i].is_empty(),
                  "Hand::card(i):\n"
                  "  empty card " << i << ":\n"
                  << *this);

  return this->cards_[i];
} // HandCard const& Hand::card(unsigned const i) const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	all (not played) cards, but only one of multiple same
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.8
 **/
HandCards
Hand::cards_single() const
{
  HandCards cards_single;

  for (HandCards::const_iterator c = this->cards().begin();
       c != this->cards().end();
       ++c) {
    // search the card in the til now selected cards
    HandCards::const_iterator c2 = cards_single.begin();
    for ( ; c2 != cards_single.end(); ++c2)
      if (*c2 == *c)
        break;

    if (c2 == cards_single.end())
      cards_single.push_back(*c);
  } // for (c \in this->cards_)

  return cards_single;
} // HandCards Hand::cards_single() const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	the cards and its number
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.4
 **/
map<Card, unsigned>
Hand::counted_cards() const
{
  map<Card, unsigned> counter;

  for (HandCards::const_iterator c = this->cards().begin();
       c != this->cards().end();
       ++c)
    counter[*c] += 1;

  return counter;
} // map<Card, unsigned> Hand::counted_cards() const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	number of all cards in the hand
 **
 ** @author	Diether Knof
 **
 ** @version	0.5.4
 **/
unsigned
Hand::cardsnumber_all() const
{
  return this->cards_all_.size();
} // unsigned Hand::cardsnumber_all() const

/**
 ** -> result
 **
 ** @param	i	cardnumber
 **
 ** @return	the card number 'i'
 **
 ** @author	Diether Knof
 **
 ** @version	0.5.4
 **/
HandCard const&
Hand::card_all(unsigned const i) const
{
  DEBUG_ASSERTION((i < this->cardsnumber_all()),
                  "Error: file " << __FILE__ << ", line " << __LINE__ << ", "
                  << "function Hand::card_all()\n"
                  << "  illegal value " << i
                  << " (maximal value is: " << this->cardsnumber_all() - 1 << ")");


  DEBUG_ASSERTION(!this->cards_all_[i].is_empty(),
                  "Hand::card_all(i):\n"
                  "  empty card " << i << ":\n"
                  << *this);

  return this->cards_all_[i];
} // HandCard const& Hand::card(unsigned const i) const

/**
 ** -> result
 **
 ** @param	i	cardnumber
 **
 ** @return	whether the card 'i' has been played
 **
 ** @author	Diether Knof
 **
 ** @version	0.5.4
 **/
bool
Hand::played(unsigned const i) const
{
  return (this->played_trick(i) != UINT_MAX);
} // bool Hand::played(unsigned const i) const

/**
 ** -> result
 **
 ** @param	i	number of card
 **
 ** @return	in which trick the card has been played
 **		(UINT_MAX, if not played, yet)
 **
 ** @author	Diether Knof
 **
 ** @version	0.5.4
 **/
unsigned
Hand::played_trick(unsigned const i) const
{
  DEBUG_ASSERTION((i < this->cardsnumber_all()),
                  "Hand::played()\n"
                  "  illegal value i = " << i
                  << " (>= " << this->cardsnumber_all() << ")");

  return this->played_[i];
} // unsigned Hand::played_trick(i)

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	the highest value of the hand
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
Card::Value
Hand::highest_value() const
{
  return this->cards().highest_value();
} // Card::Value Hand::highest_value() const

/**
 ** -> result
 **
 ** @param	tcolor   tcolor to check
 **
 ** @return	the highest value of the given tcolor
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
Card::Value
Hand::highest_value(Card::TColor const& tcolor) const
{
  DEBUG_ASSERTION(this->player_,
                  "Hand:highest_value(tcolor):\n"
                  "  hand does not belongs to a player");

  return this->cards().highest_value(tcolor);
} // Card::Value Hand::highest_value(Card::TColor tcolor) const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	the lowest value of the hand
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
Card::Value
Hand::lowest_value() const
{
  return this->cards().lowest_value();
} // Card::Value Hand::lowest_value() const

/**
 ** -> result
 **
 ** @param	tcolor   tcolor to check
 **
 ** @return	the lowest value of the given tcolor
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
Card::Value
Hand::lowest_value(Card::TColor const& tcolor) const
{
  DEBUG_ASSERTION(this->player_,
                  "Hand:lowest_value(tcolor):\n"
                  "  hand does not belongs to a player");

  return this->cards().lowest_value(tcolor);
} // Card::Value Hand::lowest_value(Card::TColor tcolor) const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	the highest card of the hand
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.2
 **/
HandCard const&
Hand::highest_card() const
{
  DEBUG_ASSERTION(this->player_,
                  "Hand::highest_card():\n"
                  "  hand does not belongs to a player");

  return this->cards().highest_card();
} // HandCard const& Hand::highest_card() const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	the highest trump of the hand
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.2
 **/
HandCard const&
Hand::highest_trump() const
{
  return this->cards().highest_trump();
} // HandCard Hand::highest_trump() const

/**
 ** -> result
 **
 ** @param	card_limit   card limit
 **
 ** @return	the highest trump till the card limit
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.8
 **/
HandCard const&
Hand::highest_trump_till(Card const& card_limit) const
{
  return this->cards().highest_trump_till(card_limit);
} // HandCard Hand::highest_trump_till(Card card_limit) const

/**
 ** -> result
 **
 ** @param	tcolor   tcolor to check the cards of
 **
 ** @return	the highest card of the given tcolor
 **		(Card() if no card of the color is present)
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.2
 **/
HandCard const&
Hand::highest_card(Card::TColor const& tcolor) const
{
  DEBUG_ASSERTION(this->player_,
                  "Hand::highest_card():\n"
                  "  hand does not belongs to a player");

  return this->cards().highest_card(tcolor);
} // HandCard Hand::highest_card(Card::TColor tcolor) const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	the lowest trump of the hand
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.4
 **/
HandCard const&
Hand::lowest_trump() const
{
  return this->cards().lowest_trump();
} // HandCard Hand::lowest_trump() const

/**
 ** -> result
 **
 ** @param	tcolor   tcolor to check the cards of
 **
 ** @return	the lowest card of the given tcolor
 **		(Card() if no card of the tcolor is present)
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.4
 **/
HandCard const&
Hand::lowest_card(Card::TColor const& tcolor) const
{
  DEBUG_ASSERTION(this->player_,
                  "Hand::lowest_card():\n"
                  "  hand does not belongs to a player");

  return this->cards().lowest_card(tcolor);
} // HandCard Hand::lowest_card(Card::TColor tcolor) const

/**
 ** -> result
 **
 ** @param	card   card to compare with
 **
 ** @return	whether there exists a higher card than 'card' on the hand
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.3
 **/
bool
Hand::higher_card_exists(Card const& card) const
{
  return this->cards().higher_card_exists(card);
} // bool Hand::higher_card_exists(Card card) const

/**
 ** -> result
 **
 ** @param	card   card to compare with
 **
 ** @return	whether there exists a lower card than 'card' on the hand
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.3
 **/
bool
Hand::lower_card_exists(Card const& card) const
{
  return this->cards().lower_card_exists(card);
} // bool Hand::lower_card_exists(Card card) const

/**
 ** -> result
 **
 ** @param	card   card to compare with
 **
 ** @return	number of higher cards then 'card'
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.8
 **/
unsigned
Hand::higher_cards_no(Card const& card) const
{
  return this->cards().higher_cards_no(card);
} // unsigned Hand::higher_cards_no(Card card) const

/**
 ** -> result
 **
 ** @param	card   card to compare with
 **
 ** @return	the next jabbing card of 'card', EMPTY if no such exists
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.11
 **/
HandCard const&
Hand::next_jabbing_card(Card const& card) const
{
  return this->cards().next_jabbing_card(card);
} // HandCard Hand::next_jabbing_card(Card card) const

/**
 ** -> result
 **
 ** @param	card   card to compare with
 **
 ** @return	the next higher card then 'card', EMPTY if no such exists
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.9
 **/
HandCard const&
Hand::next_higher_card(Card const& card) const
{
  return this->cards().next_higher_card(card);
} // HandCard Hand::next_higher_card(Card card) const

/**
 ** -> result
 **
 ** @param	card   card to compare with
 **
 ** @return	'card' or the next higher card then 'card', EMPTY if no such exists
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.9
 **/
HandCard const&
Hand::same_or_higher_card(Card const& card) const
{
  return this->cards().same_or_higher_card(card);
} // HandCard Hand::same_or_higher_card(Card card) const

/**
 ** -> result
 **
 ** @param	card   card to compare with
 **
 ** @return	the next lower card then 'card', EMPTY if no such exists
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.11
 **/
HandCard const&
Hand::next_lower_card(Card const& card) const
{
  return this->cards().next_lower_card(card);
} // HandCard Hand::next_lower_card(Card card) const

/**
 ** -> result
 **
 ** @param	card   card to compare with
 **
 ** @return	'card' or the next lower card then 'card', EMPTY if no such exists
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.11
 **/
HandCard const&
Hand::same_or_lower_card(Card const& card) const
{
  return this->cards().same_or_lower_card(card);
} // HandCard Hand::same_or_lower_card(Card card) const

/**
 ** -> result
 **
 ** @param	trick   trick to check
 **
 ** @return	whether the trick can be jabbed
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.10
 **
 ** @todo       improve performance: do not use validcards
 **/
bool
Hand::can_jab(Trick const& trick) const
{
  return this->validcards(trick).higher_card_exists(trick.winnercard());
} // bool Hand::can_jab(Trick trick) const

/**
 ** -> result
 ** The pos of the argument is the pos in the remaining cards.
 ** It is searched, what position in all cards 'pos' stands for.
 **
 ** @param	pos	position in the unplayed card
 **
 ** @return	position the played cards
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.4
 **/
unsigned
Hand::pos_to_pos_all(unsigned pos) const
{
  if (pos == UINT_MAX)
    return UINT_MAX;

  if ( (pos >= this->cardsnumber())
      || (this->played_.empty()) ) {
    DEBUG_ASSERTION(false,
                    "Error: file " << __FILE__ << ", line " << __LINE__ << ", "
                    << "function Hand::pos_to_pos_all(" << pos << ")\n"
                    << "  illegal value " << pos
                    << " (max value is: " << cardsnumber() - 1 << ")");
    return UINT_MAX;
  }

  unsigned n;
  // translate the position
  for (n = 0, pos += 1; pos > 0; n++)
    if (!this->played(n))
      pos--;

  return (n - 1);
} // unsigned Hand::pos_to_pos_all(pos)


/**
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
 **/
unsigned
Hand::pos_all_to_pos(unsigned pos) const
{
  if (pos == UINT_MAX)
    return UINT_MAX;

  if ( (pos >= cardsnumber_all())
      || (played_.empty()) ) {
    DEBUG_ASSERTION(false,
                    "Error: file " << __FILE__ << ", line " << __LINE__ << ", "
                    << "function Hand::pos_all_to_pos(" << pos << ")\n"
                    << "  illegal value " << pos
                    << " (max value is: " << cardsnumber_all() - 1 << ")");
  }

  unsigned n;
  // translate the position
  for (n = 0, pos += 1; pos > 0; pos--)
    if (!played(pos - 1))
      n++;

  return (n - 1);
} // unsigned Hand::pos_all_to_pos(pos)

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	the player this hand belongs to
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **/
Player const&
Hand::player() const
{
  DEBUG_ASSERTION(this->player_,
                  "Hand::player():\n"
                  "  this->player_ == NULL");

  return *this->player_;
} // Player const& Hand::player() const

/**
 ** sets the player
 **
 ** @param	player	player of this hand
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.8
 **/
void
Hand::set_player(Player const& player)
{
  this->player_ = &player;

  return ;
} // void Hand::set_player(Player const& player)

/**
 **
 ** @param	-
 **
 ** @return	the corresponding game
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.8
 **/
Game const&
Hand::game() const
{
  return this->player().game();
} // Game Hand::game() const

/**
 ** -> result
 **
 ** @param	c	card to search
 **
 ** @return	position in the unplayed cards of 'c'
 **		if 'c' is the requested card, its position is returned
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.4
 **/
unsigned
Hand::getpos(Card const& c) const
{
  // if the card is in the requested position, take that
  if (   (this->requested_position() < this->cardsnumber())
      && (this->card(requested_position()) == c)
     )
    return this->requested_position();

  // search the card
  for (unsigned i = 0; i < this->cardsnumber(); i++)
    if (c == this->card(i))
      // the card has been found 
      return i;

  if (   (c != Card::UNKNOWN)
      && this->contains(Card::UNKNOWN))
    return this->getpos(Card::UNKNOWN);

  DEBUG_ASSERTION(false,
                  "Hand::getpos(" << c << ")\n"
                  "  card is not in the hand: " << c << '\n'
                  << *this);

  // the card is not in the hand
  return UINT_MAX;
} // unsigned Hand::getpos(Card)

/**
 ** -> result
 **
 ** @param	c	card to search
 **
 ** @return	position in all cards of 'c'
 **		if 'c' is the requested card, its position is returned
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.4
 **/
unsigned
Hand::getpos_all(Card const& c) const
{
  // if the card is in the requested position, take that
  if (   (requested_position() < cardsnumber())
      && (card_all(requested_position()) == c)
     )
    return requested_position();

  // search the card (in played form)
  for (unsigned i = 0; i < cardsnumber_all(); i++)
    if (this->played(i))
      if (c == card_all(i))
        // the card has been found
        return i;

  // search the card
  for (unsigned i = 0; i < cardsnumber_all(); i++)
    if (c == card_all(i))
      // the card has been found
      return i;

  // the card is not in the hand
  return UINT_MAX;
} // unsigned Hand::getpos_all(Card)

/**
 ** marks card 'c' as played
 **
 ** @param	c	card to play
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.4
 **/
void
Hand::playcard(HandCard const& c)
{
  DEBUG_ASSERTION((this == &c.hand()),
                  "Hand::playcard(" + to_string(c) + ")\n"
                  "  The card does not belong to the hand:\n"
                  << *this);

  unsigned const pos = this->getpos(c);
  if (this->card(pos) == Card::UNKNOWN) {
    this->cards_[pos] = c;
    this->cards_all_[this->pos_to_pos_all(pos)] = c;
  }
  this->playcard(pos);

  return ;
} // void Hand::playcard(HandCard c)

/**
 ** marks card at 'pos' as played
 **
 ** @param	pos	position of the card to play
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.4
 **/
void
Hand::playcard(unsigned pos)
{
  DEBUG_ASSERTION((pos != UINT_MAX),
                  "function Hand::playcard(pos)\n"
                  "  illegal position 'UINT_MAX'"
                  << *this);


  DEBUG_ASSERTION((pos < this->cardsnumber()),
                  "function Hand::playcard(" << pos << ")\n"
                  << "  illegal position"
                  << " (maximal value is " << cardsnumber() << ")");

  DEBUG_ASSERTION(!this->card(pos).is_empty(),
                  "Hand::playcard(" << pos << ")\n"
                  << "  empty card: " << card(pos));

  // mark the card as being played
  if (::game_status == GAMESTATUS::GAME_POVERTY_SHIFT) {
    this->played_[this->pos_to_pos_all(pos)] = pos;
  } else { //
    this->played_[this->pos_to_pos_all(pos)]
      = this->cardsnumber_all() - this->cardsnumber();
#ifdef OUTDATED
    = this->player().game().trick_current_no();
#endif
  }

  this->cards_.erase(this->cards_.begin() + pos);
  this->requested_position_ = UINT_MAX;

  return ;
} // void Hand::playcard(pos)

/**
 ** marks a played 'card' as not played
 **
 ** @param	c card to unmark
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.5
 **/
void
Hand::unplaycard(HandCard const& c)
{
  DEBUG_ASSERTION((this == &c.hand()),
                  "Hand::playcard(c):\n"
                  "  the card does not belong to the hand");

  for (unsigned i = 0; i < this->cardsnumber_all(); i++) {
    if ((this->card_all(i) == c)
        && (this->played(i))) {
      // card found:
      // unmark the card and add it to the hand
      this->played_[i] = UINT_MAX;
      unsigned j = 0; // position in 'cards_'
      for (; i > 0; i--)
        if (!this->played(i - 1))
          j++;

      this->cards_.insert(this->cards_.begin() + j, HandCard(*this, c));

      return ;
    } // card found
  } // for (i)

  // the card has not been found
  DEBUG_ASSERTION(false,
                  "Hand::unplaycard(" << c << "):\n"
                  "  card not found:\n"
                  << *this);

  return ;
} // void Hand::unplaycard(HandCard)

/**
 ** adds 'card' to the hand
 **
 ** @param     card   card to add
 ** @param     n      how many times to add (default: 1)
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
void
Hand::add(Card const& card, unsigned const n)
{
  this->cards_.insert(this->cards_.end(), n, HandCard(*this, card));
  this->cards_all_.insert(this->cards_all_.end(), n,HandCard(*this, card));
  this->played_.insert(this->played_.end(), n, UINT_MAX);

  return ;
} // void Hand::add(Card card, unsigned n = 1)

/**
 ** adds the cards 'cards' to the hand
 **
 ** @param	cards	cards to be added
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.4.5
 **/
void
Hand::add(vector<Card> const& cards)
{
  for (vector<Card>::const_iterator c = cards.begin();
       c != cards.end();
       c++)
    this->add(*c);

  return ;
} // void Hand::add(vector<Card> const& cards)

/**
 ** adds the cards from 'hand' to the hand
 **
 ** @param	hand	hand to be added
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.8
 **/
void
Hand::add(Hand const& hand)
{
  for (unsigned c = 0; c < hand.cardsnumber_all(); c++) {
    if (!hand.played(c))
      this->cards_.push_back(HandCard(*this, hand.card_all(c)));
    this->cards_all_.push_back(HandCard(*this, hand.card_all(c)));
    this->played_.push_back(hand.played_trick(c));
  } // for (c < hand.cardsnumber_all())

  return ;
} // void Hand::add(Hand const& hand)

/**
 ** adds 'card' to the hand
 **
 ** @param     card   card to add
 ** @param     n      how many times to add (default: 1)
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
void
Hand::add_played(Card const& card, unsigned const n)
{
  for (unsigned i = 0; i < n; ++i) {
    this->played_.push_back(this->cards_all_.size() - this->cards_.size());
    this->cards_all_.push_back(HandCard(*this, card));
  } // for (unsigned i = 0; i < n; ++i)

  return ;
} // void Hand::add_played(Card card, unsigned n = 1)

/**
 ** adds the played cards 'cards' to the hand
 **
 ** @param     cards   played cards to be added
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
void
Hand::add_played(vector<Card> const& cards)
{
  for (vector<Card>::const_iterator c = cards.begin();
       c != cards.end();
       c++)
    this->add_played(*c);

  return ;
} // void Hand::add_played(vector<Card> cards)

/**
 ** removes 'card' from the hand (one card)
 **
 ** @param	card	card to remove
 **
 ** @return	position of the removed card
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.4
 **/
unsigned
Hand::remove(HandCard const& card)
{
  DEBUG_ASSERTION((this->player_ == card.hand().player_),
                  "Hand::remove(card):\n"
                  "  the card does not belong to the same player: " << card << '\n'
                  << *this);

  unsigned const pos_all = this->getpos_all(card);
  if ((pos_all == UINT_MAX)
      && card.is_empty())
    return UINT_MAX;

  DEBUG_ASSERTION((pos_all != UINT_MAX),
                  "Hand::remove(card):"
                  "  card '" << card << "' not in the hand: \n"
                  << *this);

  if (!this->played(pos_all))
    this->cards_.erase(this->cards_.begin() + this->pos_all_to_pos(pos_all));
  this->cards_all_.erase(this->cards_all_.begin() + pos_all);
  this->played_.erase(this->played_.begin() + pos_all);

  return pos_all;
} // unsigned Hand::remove(HandCard const& card)

/**
 ** removes the card 'card' from the hand
 **
 ** @param	card	card to remove
 **
 ** @return	position of the removed card
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.8
 **/
unsigned
Hand::remove(Card const& card)
{
  return this->remove(HandCard(*this, card));
} // unsigned Hand::remove(Card card)

/**
 ** removes the cards 'cards' from the hand
 **
 ** @param	cards	cards to remove
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.4.5
 **/
void
Hand::remove(HandCards const& cards)
{
  for (HandCards::const_iterator c = cards.begin();
       c != cards.end();
       c++)
    this->remove(*c);

  return ;
} // void Hand::remove(HandCards const& cards)

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	the requested card (to be played next)
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.1
 **/
HandCard
Hand::requested_card() const
{
  if (this->requested_position() == UINT_MAX)
    return HandCard(*this, Card());

  return this->card(this->requested_position());
} // HandCard Hand::requested_card() const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	the requested position
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.3
 **/
unsigned
Hand::requested_position_all() const
{
  return this->pos_to_pos_all(this->requested_position());
} // unsigned Hand::requested_position_all() const

/**
 ** request the card in 'position' to be played next
 **
 ** @param	position	position of the requested card
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.1
 **/
void
Hand::request_position(unsigned const position)
{
  this->requested_position_ = position;

  return ;
} // void Hand::request_position(unsigned const position)

/**
 ** request the card in 'position_all' of all cards to be played next
 **
 ** @param	position_all	position (of all cards) of the requested card
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.1
 **/
void
Hand::request_position_all(unsigned const position_all)
{
  if (!this->played(position_all))
    this->request_position(this->pos_all_to_pos(position_all));

  return ;
} // void Hand::request_position_all(unsigned const position_all)

/**
 ** request the card 'card' to be played next
 **
 ** @param	card	card to be requested
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.1
 **/
void
Hand::request_card(Card const card)
{
  DEBUG_ASSERTION((this->numberof(card) > 0),
                  "Hand::request_card(card):\n"
                  "  'card' " << card << " is not in the hand:\n"
                  << *this);

  this->request_position(this->getpos(card));

  return ;
} // void Hand::request_card(Card const card)

/**
 ** forget the request
 **
 ** @param	-
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.4
 **/
void
Hand::forget_request()
{
  this->requested_position_ = UINT_MAX;

  return ;
} // void Hand::forget_request()

/**
 ** -> result:
 ** 
 ** @param	trick	trick
 ** 
 ** @return	the cards from the hand, that are valid to play in the trick
 ** 		(of two same cards only one is taken)
 ** 
 ** @author	Diether Knof 
 ** 
 ** @version	0.6.8
 **/
HandCards
Hand::validcards(Trick const& trick) const
{
  HandCards cards;

  for (HandCards::const_iterator c = this->cards().begin();
       c != this->cards().end();
       ++c)
    if (c->isvalid(trick))
      if (std::find(cards.begin(),
                    cards.end(),
                    *c)
          == cards.end())
        cards.push_back(*c);

  return cards;
} // HandCards Hand::validcards(trick)

/**
 ** -> result:
 ** 
 ** @param	tcolor   tcolor
 ** 
 ** @return	the cards from the hand of 'tcolor'
 ** 		(of two same cards only one is taken)
 ** 
 ** @author	Diether Knof 
 ** 
 ** @version	0.7.11
 **/
HandCards
Hand::cards(Card::TColor const tcolor) const
{
  DEBUG_ASSERTION((this->cardsnumber() > 0),
                  "Hand::cards(tcolor):\n"
                  "  The hand has no cards");

  HandCards cards;

  for (HandCards::const_iterator c = this->cards().begin();
       c != this->cards().end();
       ++c)
    if (c->tcolor() == tcolor)
      if (std::find(cards.begin(),
                    cards.end(),
                    *c)
          == cards.end())
        cards.push_back(*c);

  return cards;
} // HandCards Hand::cards(Card::TColor tcolor)

/**
 ** -> result:
 ** 
 ** @param	-
 ** 
 ** @return	the trumps of the hand
 ** 		(of two same cards only one is taken)
 ** 
 ** @author	Diether Knof 
 ** 
 ** @version	0.7.11
 **/
HandCards
Hand::trumps() const
{
  return this->cards(Card::TRUMP);
} // HandCards Hand::trumps() const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	whether there is a trump still on the hand
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.8
 **/
bool
Hand::hastrump() const
{
  return this->cards().hastrump();
} // bool Hand::hastrump()

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	whether there is a color card still on the hand
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.5
 **/
bool
Hand::hascolor() const
{
  return this->cards().hascolor();
} // bool Hand::hascolor()

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	whether there is a genscher on the hand
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.0
 **/
bool
Hand::has_possible_genscher() const
{
  return (this->player().game().rule()(Rule::GENSCHER)
          && (this->player().game().type() == GAMETYPE::NORMAL)
          && (this->numberoftrumpkings()
              == this->player().game().rule()(Rule::NUMBER_OF_SAME_CARDS)));
} // bool Hand::has_possible_genscher()

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	whether the hand does contain swines (or can announce them)
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
bool
Hand::has_swines() const
{
  if (this->game().swines_announced())
    return ((this->game().swines_owner() == &this->player())
            && (this->numberoftrumpaces() > 0));

  if (   (::game_status == GAMESTATUS::GAME_RESERVATION)
      && (   !::setting(Setting::SHOW_ALL_HANDS)
          || (this->player().type() == Player::HUMAN) )
      && !this->player().reservation().swines)
    return false;

  return this->player().game().swines_announcement_valid(this->player());
} // bool Hand::has_swines()

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	whether the hand does contain hyperswines (or can announce them)
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **
 ** @todo       when all cards are shown: show also hyperswines if another player has the swines
 **/
bool
Hand::has_hyperswines() const
{
  if (this->game().hyperswines_announced())
    return ((this->game().hyperswines_owner() == &this->player())
            && ((this->game().rule()(Rule::WITH_NINES)
                 ? this->numberoftrumpnines()
                 : this->numberoftrumpkings())
                > 0));

  if (   (::game_status == GAMESTATUS::GAME_RESERVATION)
      && (   !::setting(Setting::SHOW_ALL_HANDS)
          || (this->player().type() == Player::HUMAN) )
      && (this->player() == this->game().player_current())
      && (   !this->player().reservation().swines
          || !this->player().reservation().hyperswines) )
    return false;

  return this->player().game().hyperswines_announcement_valid(this->player());
} // bool Hand::has_hyperswines()

/**
 ** -> result
 **
 ** @param	value	value to count
 **
 ** @return	number of cards with the value 'value'
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.8
 **/
unsigned
Hand::numberof(Card::Value const& value) const
{
  return this->cards().numberof(value);
} // unsigned Hand::numberof(Card::Value value)


/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	number of cards with value 10 or mor
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.12
 **/
unsigned
Hand::numberofrichcards() const
{
  return this->cards().numberofrichcards();
} // unsigned Hand::numberofrichcards() const

/**
 ** -> result
 **
 ** @param	tcolor	trump-color to count
 **
 ** @return	number of cards with the trump-color 'tcolor'
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.8
 **/
unsigned
Hand::numberof(Card::TColor const& tcolor) const
{
  return this->cards().numberof(tcolor);
} // unsigned Hand::numberof(Card::TColor const& tcolor)

/**
 ** -> result
 **
 ** @param	card	card to count
 **
 ** @return	number of cards 'card'
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.8
 **/
unsigned
Hand::numberof(Card const& card) const
{
  return this->cards().numberof(card);
} // unsigned Hand::numberof(Card const& card) const

/**
 ** -> result
 ** Note: 'Hand::numberof(Card)' does not cover the case (TRUMP, ACE)
 **
 ** @param	tcolor	trump-color of the card
 ** @param	value	value of the card
 **
 ** @return	number of cards with trump-color 'tcolor' and value 'value'
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.8
 **/
unsigned
Hand::numberof(Card::TColor const& tcolor, Card::Value const& value) const
{
  return this->cards().numberof(tcolor, value);
} // unsigned Hand::numberof(Card::TColor, Card::Value)

/**
 ** -> result
 ** Note: 'Hand::numberof(Card)' does not cover the case (TRUMP, ACE)
 **
 ** @param	color	color to count
 ** @param	game	game
 **
 ** @return	number of cards with color 'color' given the gametype 'gt'
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.8
 **/
unsigned
Hand::numberof(Card::Color const& color, Game const& game) const
{
  return this->numberof(color, game.type(), game.rule()(Rule::DOLLEN));
} // unsigned Hand::numberof(Card::Color color, Game game) const

/**
 ** -> result
 ** Note: 'Hand::numberof(Card)' does not cover the case (TRUMP, ACE)
 **
 ** @param	color		color to count
 ** @param	gametype	the gametype
 **
 ** @return	number of cards with color 'color' given the gametype
 **/
unsigned
Hand::numberof(Card::Color const& color,
               GameType const gametype) const
{
  return this->cards().numberof(color, gametype, this->game().rule()(Rule::DOLLEN));
} // unsigned Hand::numberof(Card::Color color, GameType gametype) const

/**
 ** -> result
 ** Note: 'Hand::numberof(Card)' does not cover the case (TRUMP, ACE)
 **
 ** @param	color		color to count
 ** @param	gametype	the gametype
 ** @param	dollen		whether dollen are allowed
 **
 ** @return	number of cards with color 'color' given the gametype and the dollen rule
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.8
 **/
unsigned
Hand::numberof(Card::Color const& color,
               GameType const gametype, bool const dollen) const
{
  return this->cards().numberof(color, gametype, dollen);
} // unsigned Hand::numberof(Card::Color color, GameType gametype, bool dollen) const

/**
 ** -> result
 **
 ** @param	card   card to check
 **
 ** @return	how many times 'card' has been played
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.12
 **/
unsigned
Hand::played(Card const& card) const
{
  return (this->numberofall(card) - this->numberof(card));
} // unsigned Hand::played(Card const& card) const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	number of trumps on the hand
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.6
 **/
unsigned
Hand::numberoftrumps() const
{
  return this->cards().numberoftrumps();
} // unsigned Hand::numberoftrumps() const

/**
 ** -> result
 **
 ** @param	game_type   the game type
 **
 ** @return	number of trumps on the hand for the game type 'game_type'
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.6
 **/
unsigned
Hand::numberoftrumps(GameType const game_type) const
{
  return this->cards().numberoftrumps(game_type);
} // unsigned Hand::numberoftrumps(GameType game_type) const

/**
 ** -> result
 **
 ** @param	game_type   the game type
 **
 ** @return	number of fehl on the hand for the game type 'game_type'
 **/
unsigned
Hand::numberoffehl(GameType const game_type) const
{
  return (this->cardsnumber() - this->numberoftrumps(game_type));
} // unsigned Hand::numberoffehl(GameType game_type) const

/**
 ** -> result
 **
 ** @param    gametype    game type
 **
 ** @result   number of single tens
 **/
unsigned
Hand::numberofsingletens(GameType const gametype) const
{

  bool const dollen = this->game().rule()(Rule::DOLLEN);

  unsigned result = 0;
  for (auto const c : this->game().rule().card_colors()) {
  if (   this->numberof(c, gametype) <= 2
      && this->numberof(c, Card::ACE) == 0 
      && !Card(c, Card::ACE).istrump(gametype, dollen)
      && !Card(c, Card::TEN).istrump(gametype, dollen))
    result += this->numberof(c, Card::TEN);
  }

  return result;
} // unsigned Hand::numberofsingletens(GameType gametype) const

/**
 ** -> result
 **
 ** @param	tcolor	card tcolor
 ** @param	value	card value
 **
 ** @return	number of 'Card(tcolor, value)' on the hand with all cards
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.6
 **/
unsigned
Hand::numberofall(Card::TColor const& tcolor, Card::Value const& value) const
{
  unsigned res=0;

  for (unsigned i = 0; i < this->cardsnumber_all(); i++)
    if ((this->card_all(i).tcolor() == tcolor)
        && (this->card_all(i).value() == value))
      ++res;

  return res;
} // unsigned Hand::numberofall(Card::TColor const& tcolor, Card::Value const& value) const

/**
 ** -> result
 **
 ** @param	card	card to search
 **
 ** @return	number of 'card' on the hand with all cards
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.1
 **/
unsigned
Hand::numberofall(Card const& card) const
{
  unsigned res=0;

  for (unsigned i = 0; i < this->cardsnumber_all(); i++)
    if (this->card_all(i) == card)
      ++res;

  return res;
} // unsigned Hand::numberofall(Card card) const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	number of trump aces on the hand with all cards
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.6
 **/
unsigned
Hand::numberofalltrumpaces() const
{
  return this->numberofall(Card::TRUMP, Card::ACE);
} // unsigned Hand::numberofalltrumpaces() const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	number of trump kings on the hand
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.8
 **/
unsigned
Hand::numberoftrumpkings() const
{
  return this->cards().numberoftrumpkings();
} // unsigned Hand::numberoftrumpkings() const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	number of nines on the hand
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.8
 **/
unsigned
Hand::numberofnines() const
{
  return this->cards().numberofnines();
} // unsigned Hand::numbernines()

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	number of kings on the hand
 **
 ** @author	Diether Knof
 **
 ** @version	0.5.4
 **/
unsigned
Hand::numberofkings() const
{
  return this->cards().numberofkings();
} // unsigned Hand::numberkings()

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	number of club queens on the hand
 **
 ** @author	Diether Knof
 **
 ** @version	0.5.4
 **/
unsigned
Hand::numberofclubqueens() const
{
  return this->cards().numberofclubqueens();
} // unsigned Hand::numberofclubqueens()

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	number of dolle on the hand
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.10
 **/
unsigned
Hand::numberofdolle() const
{
  if (!this->game().rule()(Rule::DOLLEN))
    return 0;
  return this->numberof(Card::DOLLE);
} // unsigned Hand::numberofdolle()

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	number of swines on the hand
 **
 ** @author	Diether Knof
 **
 ** @version	0.5.4
 **/
unsigned
Hand::numberofswines() const
{
  if (!this->has_swines())
    return 0;
  return this->numberoftrumpaces();
} // unsigned Hand::numberofswines()

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	number of hyperswines on the hand
 **
 ** @author	Diether Knof
 **
 ** @version	0.5.4
 **/
unsigned
Hand::numberofhyperswines() const
{
  if (!this->has_hyperswines())
    return 0;
  return (this->game().rule()(Rule::WITH_NINES)
          ? this->numberoftrumpaces()
          : this->numberoftrumpkings());
} // unsigned Hand::numberofhyperswines()

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	number of trump aces on the hand
 **
 ** @author	Diether Knof
 **
 ** @version	0.5.4
 **/
unsigned
Hand::numberoftrumpaces() const
{
  return this->cards().numberoftrumpaces();
} // unsigned Hand::numberoftrumpaces() const

/**
 ** -> result
 **
 ** @param	game_type   game type
 **
 ** @return	number of trump aces on the hand
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.14
 **/
unsigned
Hand::numberoftrumpaces(GameType const game_type) const
{
  return this->cards().numberoftrumpaces(game_type);
} // unsigned Hand::numberoftrumpaces(GameType game_type) const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	number of trump nines on the hand
 **
 ** @author	Diether Knof
 **
 ** @version	0.5.4
 **/
unsigned
Hand::numberoftrumpnines() const
{
  return this->cards().numberoftrumpnines();
} // unsigned Hand::numberoftrumpnines() const

/**
 ** -> result
 **
 ** @param	game_type   the game type
 **
 ** @return	number of fehl colors on the hand for the game type 'game_type'
 **/
unsigned
Hand::numberoffehlcolors(GameType const game_type) const
{
  auto const dollen = this->game().rule()(Rule::DOLLEN);

  auto const& colors = this->game().rule().card_colors();
  return std::count_if(colors.begin(), colors.end(), 
                       [this, game_type, dollen](auto const& c) {
                       return (this->numberof(c, game_type, dollen) == 0);
                       });
} // unsigned Hand::numberoffehlcolors(GameType game_type) const


/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	number of cards to shift in the poverty
 **             UINT_MAX when no poverty is valid
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.3
 **/
unsigned
Hand::numberofpovertycards() const
{
  if (!this->has_poverty())
    return UINT_MAX;

  if (this->game().rule()(Rule::POVERTY_SHIFT_ONLY_TRUMP))
    return this->numberoftrumps();
  else if (!this->game().rule()(Rule::POVERTY_FOX_DO_NOT_COUNT))
    return this->game().rule()(Rule::MAX_NUMBER_OF_POVERTY_TRUMPS);
  else if (!this->game().rule()(Rule::POVERTY_FOX_SHIFT_EXTRA))
    return max(this->game().rule()(Rule::MAX_NUMBER_OF_POVERTY_TRUMPS),
               this->numberoftrumps(GAMETYPE::POVERTY));
  else
    // Cannot use 'this->has_swines()' in the case that swines can be
    // announced during the game.
    return max(this->game().rule()(Rule::MAX_NUMBER_OF_POVERTY_TRUMPS)
               + (this->game().swines_announced()
                  ? 0
                  : this->numberoftrumpaces(GAMETYPE::POVERTY)
                 ),
               this->numberoftrumps(GAMETYPE::POVERTY));
} // unsigned Hand::numberofpovertycards() const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	whether the hand has a poverty
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.3
 **/
bool
Hand::has_poverty() const
{
  if (!this->game().rule()(Rule::POVERTY))
    return false;

  return (this->cards().numberoftrumps(GAMETYPE::POVERTY)
          - ( (   this->game().rule()(Rule::POVERTY_FOX_DO_NOT_COUNT)
               && !this->game().swines_announced())
             ? this->numberoftrumpaces(GAMETYPE::POVERTY)
             : 0)
          <= this->game().rule()(Rule::MAX_NUMBER_OF_POVERTY_TRUMPS));
} // bool Hand::has_poverty() const

/**
 ** -> result
 **
 ** @param	card	card to compare with
 **
 ** @return	number of cards greater or equal to 'card'
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.8
 **/
unsigned
Hand::numberof_ge(Card const& card) const
{
  unsigned res = 0;
  for (HandCards::const_iterator c = this->cards().begin();
       c != this->cards().end();
       ++c)
    if (!c->less(card))
      res += 1;

  return res;
} // unsigned Hand::numberof_ge(Card card)

/**
 ** -> result
 **
 ** @param	card	card to compare with
 **
 ** @return	number of cards less or equal to 'card'
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.9
 **/
unsigned
Hand::numberof_le(Card const& card) const
{
  unsigned res = 0;
  for (HandCards::const_iterator c = this->cards().begin();
       c != this->cards().end();
       ++c) {
    if (c->less(card) || (*c == card)) {
      res += 1;
    }
  }

  return res;
} // unsigned Hand::numberof_le(Card card)

/**
 ** -> result
 **
 ** @param	card	card to compare with
 **
 ** @return	number of trump cards greater or equal to 'card'
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.10
 **/
unsigned
Hand::numberof_ge_trump(Card const& card) const
{
  unsigned res = 0;
  for (HandCards::const_iterator c = this->cards().begin();
       c != this->cards().end();
       ++c)
    if (   c->istrump()
        && !c->less(card))
      res += 1;

  return res;
} // unsigned Hand::numberof_ge_trump(Card card)

/**
 ** -> result
 **
 ** @param	card	card to compare with
 **
 ** @return	number of trump cards less or equal to 'card'
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.11
 **/
unsigned
Hand::numberof_le_trump(Card const& card) const
{
  unsigned res = 0;
  for (HandCards::const_iterator c = this->cards().begin();
       c != this->cards().end();
       ++c)
    if (   c->istrump()
        && (c->less(card) || (*c == card)) )
      res += 1;

  return res;
} // unsigned Hand::numberof_le_trump(Card card)

/**
 ** -> result
 **
 ** @param	card	trump card to compare with
 **
 ** @return	the relative position of the card (between 0 and 1)
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.11
 **/
float
Hand::rel_pos_trump(Card const& card) const
{
  if (this->numberoftrumps() == 0)
    return 0;
  if (!card.istrump(this->game()))
    return 1;

  unsigned const le = this->numberof_le_trump(card);
  unsigned const ge = this->numberof_ge_trump(card);

  return (static_cast<float>(le) / (le + ge));
} // float Hand::rel_pos_trump(Card card)

/**
 ** -> result:
 ** 
 ** @param	hand	hand to compare with
 ** 
 ** @return	whether the cards of 'hand' and 'this' are the same
 ** 
 ** @author	Diether Knof 
 ** 
 ** @version	0.7.3
 **/
bool
Hand::has_same_cards(Hand const& hand) const
{
  return hand.cards().equal(this->cards());
} // bool Hand::has_same_cards(Hand const& hand) const

/**
 ** -> result
 ** 
 ** @param	tcolor   tcolor to search
 ** 
 ** @return	whether 'tcolor' is contained in 'this'
 ** 
 ** @author	Diether Knof 
 ** 
 ** @version	0.7.5
 **/
bool
Hand::contains(Card::TColor const& tcolor) const
{
  return this->cards().contains(tcolor);
} // bool Hand::contains(Card::TColor tcolor) const

/**
 ** -> result
 ** 
 ** @param	card   card to search
 ** 
 ** @return	whether 'card' is contained in 'this'
 ** 
 ** @author	Diether Knof 
 ** 
 ** @version	0.7.3
 **/
bool
Hand::contains(Card const& card) const
{
  return this->cards().contains(card);
} // bool Hand::contains(Card card) const

/**
 ** -> result
 ** 
 ** @param	color   color of the card to search
 ** @param	value   value of the card to search
 ** 
 ** @return	whether Card(color, value) is contained in 'this'
 ** 
 ** @author	Diether Knof 
 ** 
 ** @version	0.7.9
 **/
bool
Hand::contains(Card::Color const& color, Card::Value const& value) const
{
  if (   (color == Card::NOCARDCOLOR)
      || (color == Card::TRUMP)
      || (value == Card::NOCARDVALUE))
    return false;
  return this->contains(Card(color, value));
} // bool Hand::contains(Card::Color color, Card::Value value) const

/**
 ** -> result
 ** 
 ** @param	hand	hand to compare with
 ** 
 ** @return	whether all cards of 'hand' are contained in 'this'
 ** 
 ** @author	Diether Knof 
 ** 
 ** @version	0.6.7
 **/
bool
Hand::contains(Hand const& hand) const
{
  for (HandCards::const_iterator c = hand.cards().begin();
       c != hand.cards().end();
       ++c)
    if (this->numberof(*c) < hand.numberof(*c))
      return false;

  return true;
} // bool Hand::contains(Hand hand) const

/**
 ** -> result:
 ** 
 ** @param	hand	hand to compare with
 ** 
 ** @return	the cards which are in 'hand' but not in 'this'
 ** 
 ** @author	Diether Knof 
 ** 
 ** @version	0.6.6
 **/
HandCards
Hand::missing_cards(Hand const& hand) const
{
  HandCards cards;
  for (unsigned c = 0; c < hand.cardsnumber(); ++c)
    if (this->numberof(hand.card(c)) < hand.numberof(hand.card(c)))
      cards.push_back(hand.card(c));

  return cards;
} // HandCards Hand::missing_cards(Hand hand) const

/**
 ** -> result:
 ** compares the hands.
 ** The order of the cards is ignored.
 ** 
 ** @param	hand1	first hand
 ** @param	hand2	second hand
 ** 
 ** @return	whether the two hands are equal
 ** 
 ** @author	Diether Knof 
 ** 
 ** @version	0.6.7
 ** 
 ** @todo	compare the players
 **/
bool
operator==(Hand const& hand1, Hand const& hand2)
{
  if ((hand1.cardsnumber_all() != hand2.cardsnumber_all())
      || (hand1.cardsnumber() != hand2.cardsnumber()))
    return false;

  vector<bool>found(hand1.cardsnumber_all(), false);

  for (unsigned c = 0; c < hand1.cardsnumber_all(); c++) {
    unsigned d;
    for (d = 0; d < hand2.cardsnumber_all(); d++) {
      if (!found[d]
          && (hand1.card_all(c) == hand2.card_all(d))
          && (hand1.played_trick(c) == hand2.played_trick(d)))
        break;
    } // for (d < hand2.cardsnumber_all())
    if (d < hand2.cardsnumber_all())
      found[d] = true;
    else
      return false;
  } // for (c < hand1.cardsnumber_all())

  return true;
} // bool operator==(Hand const& hand1; Hand const& hand2)

/**
 ** -> result:
 ** compares the hands.
 ** The order of the cards is ignored.
 ** 
 ** @param	hand1	first hand
 ** @param	hand2	second hand
 ** 
 ** @return	whether the two hands are different
 ** 
 ** @author	Diether Knof 
 ** 
 ** @version	0.5.4
 **/
bool
operator!=(Hand const& hand1, Hand const& hand2)
{
  return !(hand1 == hand2);
} // bool operator!=(Hand const& hand1; Hand const& hand2)

/**
 ** write the hand on 'ostr'
 **
 ** @param	ostr	stream to write the hand to
 ** @param	hand	hand to write
 **
 ** @return	output stream
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **/
ostream&
operator<<(ostream& ostr, Hand const& hand)
{
  for(unsigned i = 0; i < hand.cardsnumber_all(); i++) {
    ostr << std::hex << i << std::dec << "  " << hand.card_all(i);
    if (hand.played(i))
      ostr << "\t(played " << setw(2) << hand.played_trick(i) << ")";
    ostr << "\n";
  } // for (i < hand.cardsnumber_all())

  return ostr;
} // ostream operator<<(ostream, Hand)
