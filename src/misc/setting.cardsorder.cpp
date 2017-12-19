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
 ********************************************************************/

#include "constants.h"

#include "setting.cardsorder.h"

#include "../utils/string.h"

#include "translations.h"
#include "../party/party.h"
#include "../game/game.h"

#include "../ui/ui.h"


/**
 ** default constructor
 **
 ** @param     -
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
Setting::CardsOrder::CardsOrder() :
  sorted_p(false),
  pos_p(Card::NUMBER_OF_TCOLORS, Card::NOCARDCOLOR),
  direction_p(Card::NUMBER_OF_TCOLORS, DOWN)
{
  this->pos_p[Card::TRUMP]   = 0;
  this->pos_p[Card::CLUB]    = 1;
  this->pos_p[Card::HEART]   = 2;
  this->pos_p[Card::SPADE]   = 3;
  this->pos_p[Card::DIAMOND] = 4;

  return ;
} // Setting::CardsOrder::CardsOrder()

/**
 ** constructor with the order
 **
 ** @param     c0   first color
 ** @param     d0   direction of the first
 ** @param     c1   second color
 ** @param     d1   direction of the second
 ** @param     c2   third color
 ** @param     d2   direction of the third
 ** @param     c3   fourth color
 ** @param     d3   direction of the fourth
 ** @param     c4   fifth color
 ** @param     d4   direction of the fifth
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
Setting::CardsOrder::CardsOrder(Card::TColor c0, Direction d0,
                                Card::TColor c1, Direction d1,
                                Card::TColor c2, Direction d2,
                                Card::TColor c3, Direction d3,
                                Card::TColor c4, Direction d4) :
  sorted_p(true),
  pos_p(Card::NUMBER_OF_TCOLORS, Card::NOCARDCOLOR),
  direction_p(Card::NUMBER_OF_TCOLORS, DOWN)
{
  // the colors should be different
  this->pos_p[c0] = 0;
  this->pos_p[c1] = 1;
  this->pos_p[c2] = 2;
  this->pos_p[c3] = 3;
  this->pos_p[c4] = 4;

  this->direction_p[c0] = d0;
  this->direction_p[c1] = d1;
  this->direction_p[c2] = d2;
  this->direction_p[c3] = d3;
  this->direction_p[c4] = d4;

  this->ensure_validity();

  return ;
} // Setting::CardsOrder::CardsOrder(TColor, Direction, ...)

/**
 ** constructor from a string
 **
 ** @param     value   string representation of the order
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
Setting::CardsOrder::CardsOrder(string const& value) :
  sorted_p(true),
  pos_p(Card::NUMBER_OF_TCOLORS, Card::NOCARDCOLOR),
  direction_p(Card::NUMBER_OF_TCOLORS, DOWN)
{
  // *** should work better, when you can get words of the string
  // *** (expand the string-class)
  string remain_string = value;
  string word; // the word of the string
  Card::TColor tcolor = Card::NOCARDCOLOR;

  DK::Utils::String::remove_blanks(remain_string);
  for (unsigned i = 0; i < Card::NUMBER_OF_TCOLORS; i++) {
    if (remain_string.size() == 0)
      break;
    // get the color
    word = string(remain_string, 0, remain_string.find(' '));
    if (remain_string.find(' ') == string::npos)
      word = remain_string;
    if ((word == name(Card::TRUMP))
        || (word == ::translation(Card::TRUMP)))
      tcolor = Card::TRUMP;
    else if ((word == name(Card::CLUB))
             || (word == ::translation(Card::CLUB)))
      tcolor = Card::CLUB;
    else if ((word == name(Card::SPADE))
             || (word == ::translation(Card::SPADE)))
      tcolor = Card::SPADE;
    else if ((word == name(Card::HEART))
             || (word == ::translation(Card::HEART)))
      tcolor = Card::HEART;
    else if ((word == name(Card::DIAMOND))
             || (word == ::translation(Card::DIAMOND)))
      tcolor = Card::DIAMOND;
    else if ((i == 0)
             && ((word == "unsorted")
                 || (word == ::translation("Setting::CardsOrder::unsorted"))
                 || (word == "random")
                 || (word == ::translation("Setting::CardsOrder::random"))
                 || (word == "mixed")
                 || (word == ::translation("Setting::CardsOrder::mixed"))
                )
            ) {
      // random order
      this->sorted_p = false;
      break;
    } else {
      cerr << "Unknown TColor \"" << word << "\"" << endl;
      break ;
    }

    if (!this->sorted())
      break;

    Direction direction;

    // remove the used word from the string
    if (remain_string.length() == 0)
      break;
    remain_string = string(remain_string, word.length(), std::string::npos);
    DK::Utils::String::remove_blanks(remain_string);

    // get the direction
    word = string(remain_string, 0, remain_string.find(' '));
    if (remain_string.find(' ') == string::npos)
      word = remain_string;
    if ((word == ::name(CardsOrder::UP))
        || (word == ::translation(CardsOrder::UP))
        || (word == ::name(CardsOrder::UP) + ",")
        || (word == ::translation(CardsOrder::UP) + ","))
      direction = UP;
    else if ((word == name(CardsOrder::DOWN))
             || (word == ::translation(CardsOrder::DOWN))
             || (word == name(CardsOrder::DOWN) + ",")
             || (word == ::translation(CardsOrder::DOWN) + ","))
      direction = DOWN;
    else {
      cerr << "Unknown direction \"" << word << "\"" << endl;
      break ;
    }

    // removed the used word from the string
    remain_string = string(remain_string, word.length(), std::string::npos);
    DK::Utils::String::remove_blanks(remain_string);

    // set the position of the tcolor and the direction
    this->pos_p[tcolor] = i;
    this->direction_p[tcolor] = direction;
  } // for (i < Card::NUMBER_OF_TCOLORS)

  this->ensure_validity();

  return ;
} // Setting::CardsOrder::CardsOrder(string value)

/**
 ** copy constructor
 **
 ** @param     order   order to copy
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
Setting::CardsOrder::CardsOrder(CardsOrder const& order) :
  sorted_p(order.sorted_p),
  pos_p(order.pos_p),
  direction_p(order.direction_p)
{ }

/**
 ** copy operator
 **
 ** @param     order   order to copy
 **
 ** @return    this
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
Setting::CardsOrder&
Setting::CardsOrder::operator=(CardsOrder const& order)
{
  this->pos_p = order.pos_p;
  this->direction_p = order.direction_p;
  this->sorted_p = order.sorted_p;

  return *this;
} // Setting::CardsOrder& Setting::CardsOrder::operator=(CardsOrder order)

/**
 ** destructor
 **
 ** @param     -
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
Setting::CardsOrder::~CardsOrder()
{ }

/**
 ** ensure the validity of the card order, t.i. each color has a different position
 **
 ** @param     -
 **
 ** @return    whether the sorting was valid and each direction is set
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.8
 **/
bool
Setting::CardsOrder::ensure_validity()
{
  bool result = true;
  { // check for double position
    for (unsigned c = 0; c < Card::NUMBER_OF_TCOLORS; c++) {
      if (   (this->tcolor(c) == Card::NOCARDCOLOR)
          || (this->pos(this->tcolor(c)) != c) ) {
        result = false;
        break;
      }
    }
    if (result == false) {
      this->pos_p[Card::TRUMP]   = 0;
      this->pos_p[Card::CLUB]    = 1;
      this->pos_p[Card::HEART]   = 2;
      this->pos_p[Card::SPADE]   = 3;
      this->pos_p[Card::DIAMOND] = 4;
      this->direction_p[Card::TRUMP]   = UP;
      this->direction_p[Card::CLUB]   = DOWN;
      this->direction_p[Card::SPADE]   = DOWN;
      this->direction_p[Card::HEART]   = DOWN;
      this->direction_p[Card::DIAMOND]   = DOWN;
    }
  } // check for double position
  { // check directions
    if (this->direction(Card::TRUMP) == NONE) {
      this->direction_set(Card::TRUMP, UP);
      result = false;
    }
    if (this->direction(Card::CLUB) == NONE) {
      this->direction_set(Card::CLUB, DOWN);
      result = false;
    }
    if (this->direction(Card::SPADE) == NONE) {
      this->direction_set(Card::SPADE, DOWN);
      result = false;
    }
    if (this->direction(Card::HEART) == NONE) {
      this->direction_set(Card::HEART, DOWN);
      result = false;
    }
    if (this->direction(Card::DIAMOND) == NONE) {
      this->direction_set(Card::DIAMOND, DOWN);
      result = false;
    }
  } // check directions
  return result;
} // bool Setting::CardsOrder::ensure_validity()

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    whether the cards are sorted
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
Setting::CardsOrder::operator bool() const
{
  return this->sorted();
} // Setting::CardsOrder::operator bool()

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    the order as a string
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
Setting::CardsOrder::operator string() const
{
  string value;

  if (this->sorted()) {
    for (unsigned c = 0; c < Card::NUMBER_OF_TCOLORS; c++) {
      value += name(this->tcolor(c)) + " " + name(this->direction(c));
      if (c + 1 < Card::NUMBER_OF_TCOLORS)
        value += ", ";
    } // for (c < Card::NUMBER_OF_TCOLORS))
  } else { // if !(this->sorted())
    value = "mixed";
  } // if !(this->sorted())

  return value;
} // Setting::CardsOrder::operator string()

/**
 ** -> result
 **
 ** @param     order   order to compare with
 **
 ** @return    whether 'this' is equal to 'order'
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
bool
Setting::CardsOrder::operator==(CardsOrder const& order) const
{
  if (this->sorted() != order.sorted())
    return false;

  for(unsigned i = 0; i < Card::NUMBER_OF_TCOLORS; i++)
    if ( (this->tcolor(i) != order.tcolor(i))
        || (this->direction(i) != order.direction(i)) )
      return false;

  return true;
} // bool Setting::CardsOrder::operator==(CardsOrder const& order) const

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    whether the cards are sorted
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
bool
Setting::CardsOrder::sorted() const
{
  return this->sorted_p;
} // bool Setting::CardsOrder::sorted() const

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    whether the cards are mixed
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
bool
Setting::CardsOrder::mixed() const
{
  return !this->sorted();
} // bool Setting::CardsOrder::mixed() const

/**
 ** -> result
 **
 ** @param     a   first card
 ** @param     b   second card
 **
 ** @return    the relative position of b to a,
 **            t.i. -1, if a < b, 0 if a == b, 1, if a > b
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
int
Setting::CardsOrder::relative_position(HandCard const& a, HandCard const& b) const
{
  if (this->pos(a.tcolor()) < this->pos(b.tcolor()))
    return -1;
  if (this->pos(a.tcolor()) > this->pos(b.tcolor()))
    return 1;

  // the two cards are the same color
  if (a == b)
    return 0;
  if (a.less(b))
    return ((this->direction(a.tcolor()) == UP) ? -1 :  1);
  else
    return ((this->direction(a.tcolor()) == UP) ?  1 : -1);
} // int Setting::CardsOrder::relative_position(HandCard a, HandCard b) const

/**
 ** -> result
 **
 ** @param     tcolor   trump color
 **
 ** @return    the direction of the tcolor
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
Setting::CardsOrder::Direction
Setting::CardsOrder::direction(Card::TColor const tcolor) const
{
  return this->direction_p[tcolor];
} // Setting::CardsOrder::Direction CardsOrder::direction(Card::TColor tcolor) const

/**
 ** -> result
 **
 ** @param     pos_a   position of the color
 **
 ** @return    the direction of the color at position 'pos_a'
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
Setting::CardsOrder::Direction
Setting::CardsOrder::direction(unsigned const pos_a) const
{
  for (unsigned tcolor = 0; tcolor < Card::NUMBER_OF_TCOLORS; tcolor++)
    if (this->pos_p[tcolor] == pos_a) {
      return this->direction_p[tcolor];
    }

  return NONE;
} // Setting::CardsOrder::Direction CardsOrder::direction(unsigned pos_a) const

/**
 ** -> result
 **
 ** @param     tcolor   trump color
 **
 ** @return    the position of the tcolor
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
unsigned
Setting::CardsOrder::pos(const Card::TColor tcolor) const
{
  if (tcolor == Card::UNKNOWNCARDCOLOR)
    return 101;
  return this->pos_p[tcolor];
} // unsigned Setting::CardsOrder::pos(Card::TColor tcolor) const

/**
 ** -> result
 **
 ** @param     pos_a   position
 **
 ** @return    the tcolor at the position 'pos_a'
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
Card::TColor
Setting::CardsOrder::tcolor(unsigned const pos_a) const
{
  for (unsigned tcolor = 0; tcolor < 5; tcolor++)
    if (this->pos_p[tcolor] == pos_a) {
      return Card::TColor(tcolor);
    }

  return Card::NOCARDCOLOR;
} // Card::TColor Setting::CardsOrder::tcolor(unsigned pos_a) const

/**
 ** toggle the direction of 'tcolor'
 **
 ** @param     tcolor   tcolor to toggle the direction of
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
void
Setting::CardsOrder::direction_toggle(const Card::TColor tcolor)
{
  CardsOrder const old_value = *this;

  this->direction_p[tcolor] = ( (direction(tcolor) == CardsOrder::UP)
                               ? CardsOrder::DOWN
                               : CardsOrder::UP);

  // resort the hands
  if (game_status & GAMESTATUS::GAME)
    ::party.game().players_hands_sort();

  if ((this == &(::setting(Setting::CARDS_ORDER)))
      && (ui != NULL))
    ui->setting_changed(Setting::CARDS_ORDER, &old_value);

  return ;
} // void Setting::CardsOrder::direction_toggle(Card::TColor tcolor)

/**
 ** toggle the direction of the color at 'pos_a'
 **
 ** @param     pos_a   position
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
void
Setting::CardsOrder::direction_toggle(unsigned const pos_a)
{
  CardsOrder const old_value = *this;

  for (unsigned tcolor = 0; tcolor < 5; tcolor++)
    if (this->pos_p[tcolor] == pos_a)
      this->direction_p[tcolor] = ( (direction(pos_a) == CardsOrder::UP)
                                   ? CardsOrder::DOWN
                                   : CardsOrder::UP);

  // resort the hands
  if (game_status & GAMESTATUS::GAME)
    ::party.game().players_hands_sort();

  if ((this == &(::setting(Setting::CARDS_ORDER)))
      && (ui != NULL))
    ui->setting_changed(Setting::CARDS_ORDER, &old_value);

  return ;
} // void Setting::CardsOrder::direction_toggle(unsigned const pos_a)

/**
 ** sets the direction of the given color
 **
 ** @param     tcolor      color
 ** @param     direction   new direction
 **
 ** @return    direction of the color on the given position
 **
 ** @author    Diether Knof
 **
 ** @version   0.5.1
 **/
void
Setting::CardsOrder::direction_set(Card::TColor const tcolor,
                                   Direction const direction)
{
  CardsOrder const old_value = *this;

  if (this->direction(tcolor) == direction)
    return ;

  this->direction_p[tcolor] = direction;

  // resort the hands
  if (game_status & GAMESTATUS::GAME)
    ::party.game().players_hands_sort();

  if ((this == &(::setting(Setting::CARDS_ORDER)))
      && (ui != NULL))
    ui->setting_changed(Setting::CARDS_ORDER, &old_value);

  return ;
} // void Setting::CardsOrder::direction_set(Card::TColor tcolor, Direction direction)

/**
 ** sets the direction at the given position
 **
 ** @param     pos         position
 ** @param     direction   new direction
 **
 ** @return    new direction
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
void
Setting::CardsOrder::direction_set(unsigned const pos,
                                   Direction const direction)
{
  this->direction_set(this->tcolor(pos), direction);
  return ;
} // Setting::CardsOrder::direction_set(unsigned pos, Direction direction)

/**
 ** pushes the color on 'p' one position to the left
 **
 ** @param     pos_a   position
 **
 ** @return    the moved color
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
void
Setting::CardsOrder::pos_to_left(unsigned const pos_a)
{
  CardsOrder const old_value = *this;

  unsigned c;
  for (c = 0; c < Card::NUMBER_OF_TCOLORS; c++) {
    if (this->pos(Card::TColor(c)) == pos_a)
      this->pos_p[c] = (pos_a - 1 + Card::NUMBER_OF_TCOLORS) % Card::NUMBER_OF_TCOLORS;
    else if ( ((pos(Card::TColor(c)) + Card::NUMBER_OF_TCOLORS)
               % Card::NUMBER_OF_TCOLORS)
             == (pos_a - 1 + Card::NUMBER_OF_TCOLORS) % Card::NUMBER_OF_TCOLORS)
      this->pos_p[c] = pos_a;
  }

  // resort the hands
  if (game_status & GAMESTATUS::GAME)
    ::party.game().players_hands_sort();

  if ((this == &(::setting(Setting::CARDS_ORDER)))
      && (ui != NULL))
    ui->setting_changed(Setting::CARDS_ORDER, &old_value);

  return ;
} // void Setting::CardsOrder::pos_to_left(unsigned pos_a)

/**
 ** pushes the color on 'p' one position to the right
 **
 ** @param     pos_a   position
 **
 ** @return    the moved color
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
void
Setting::CardsOrder::pos_to_right(unsigned const pos_a)
{
  this->pos_to_left((pos_a + 1) % Card::NUMBER_OF_TCOLORS);

  return ;
} // Setting::CardsOrder::pos_to_right(unsigned const pos_a)

/**
 ** set whether the cards are to be sorted
 **
 ** @param     sorted   whether the cards are to be sorted
 **
 ** @return    whether the cards are sorted
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
void
Setting::CardsOrder::sorted_set(bool const sorted)
{
  if (this->sorted() == sorted)
    return ;

  CardsOrder const old_value = *this;

  this->sorted_p = sorted;

  if (this == &(::setting(Setting::CARDS_ORDER))) {
    // resort the hands
    if (game_status & GAMESTATUS::GAME)
      if (this->sorted())
        ::party.game().players_hands_sort();
      else
        ::party.game().players_hands_mix();

    if (ui != NULL)
      ui->setting_changed(Setting::CARDS_ORDER, &old_value);
  } // if (this == &(::setting(Setting::CARDS_ORDER)))

  return ;
} // void Setting::CardsOrder::sorted_set(bool sorted)

/**
 ** mix the cards
 **
 ** @param     -
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
void
Setting::CardsOrder::mix()
{
  CardsOrder const old_value = *this;

  this->sorted_p = false;

  if (this == &(::setting(Setting::CARDS_ORDER))) {
    // resort the hands
    if (game_status & GAMESTATUS::GAME)
      if (!this->sorted())
        ::party.game().players_hands_mix();

    if (::ui != NULL)
      ::ui->setting_changed(Setting::CARDS_ORDER, &old_value);
  } // if (this == &(::setting(Setting::CARDS_ORDER)))

  return ;
} // void Setting::CardsOrder::mix()

/**
 ** -> result
 **
 ** @param     direction   the direction
 **
 ** @return    name of the direction
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
string
name(Setting::CardsOrder::Direction const& direction)
{
  switch(direction) {
  case Setting::CardsOrder::NONE:
    return "none";
  case Setting::CardsOrder::UP:
    return "up";
  case Setting::CardsOrder::DOWN:
    return "down";
  } // switch(direction)

  return "";
} // string name(Setting::CardsOrder::Direction direction)

/**
 ** print the cards order
 **
 ** @param     ostr   stream to write into
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
void
Setting::CardsOrder::write(ostream& ostr) const
{
  if (this->sorted()) {
    ostr << this->tcolor(0) << " " << name(this->direction(0));
    for (unsigned i = 1; i < Card::NUMBER_OF_TCOLORS; i++)
      ostr << ", " << this->tcolor(i) << " " << name(this->direction(i));
  } else { // if !(this->sorted())
    ostr << "mixed";
  } // if !(this->sorted())

  return ;
} // void Setting::CardsOrder::write(ostream& ostr) const

/**
 ** @param     ostr          stream to write into
 ** @param     cards_order   cards order to write
 **
 ** @return    output stream
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
ostream&
operator<<(ostream& ostr, Setting::CardsOrder const& cards_order)
{
  cards_order.write(ostr);

  return ostr;
} // ostream& operator<<(ostream& ostr, Setting::CardsOrder cards_order)
