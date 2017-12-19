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

#ifndef CLASS_SETTING_CARDSORDER_HEADER
#define CLASS_SETTING_CARDSORDER_HEADER

#include "../basistypes.h"

#include "../card/card.h"
#include "translation.h"

#include "setting.h"

class Setting::CardsOrder {
  public:
    enum Direction {
      NONE, UP, DOWN
    };

  public:

    CardsOrder();
    CardsOrder(Card::TColor, Direction,
	       Card::TColor, Direction,
	       Card::TColor, Direction,
	       Card::TColor, Direction,
	       Card::TColor, Direction);
    CardsOrder(string const& s);
    CardsOrder(CardsOrder const&);
    CardsOrder& operator=(CardsOrder const&);

    ~CardsOrder();

    bool ensure_validity();

    operator bool() const;
    operator string() const;

    bool operator==(CardsOrder const&) const;

    bool sorted() const;
    bool mixed() const;

    int relative_position(HandCard const& a, HandCard const& b) const;

    Direction direction(const Card::TColor) const;
    Direction direction(unsigned const pos) const;
    unsigned pos(const Card::TColor) const;
    Card::TColor tcolor(unsigned const pos) const;

    // toggle / set the direction
    void direction_toggle(const Card::TColor);
    void direction_toggle(unsigned const pos);
    void direction_set(const Card::TColor, Direction const direction);
    void direction_set(unsigned const pos, Direction const direction);

    void pos_to_left(unsigned const pos);
    void pos_to_right(unsigned const pos);

    void sorted_set(bool const sorted);
    void mix();

    // write of the cards order
    void write(ostream& ostr) const;

  private:
    bool sorted_p;

    vector<unsigned> pos_p;
    vector<Direction> direction_p;
}; // class Setting::CardsOrder
string name(Setting::CardsOrder::Direction const& direction);
// write of the CardsOrder
ostream& operator<<(ostream& ostr, Setting::CardsOrder const& cards_order);

inline
Translator::Translation translation(Setting::CardsOrder::Direction
                                    const& direction)
{ return Translator::Translation(::translator,
				 "Direction::"
				 + ::name(direction)); }

#endif // #ifndef CLASS_SETTING_CARDSORDER_HEADER
