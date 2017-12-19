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
#include "card.h"

#include "../misc/setting.h"
unsigned const Card::NUMBER_OF_COLORS = 4;
unsigned const Card::NUMBER_OF_TCOLORS = Card::NUMBER_OF_COLORS + 1;


/**
 ** -> result
 **
 ** @param     i   int value to be transformed into a CardColor
 **
 ** @return    DIAMOND     : for input 0
 **	       HEART       : for input 1
 **	       SPADE       : for input 2
 **	       CLUB        : for input 3
 **	       NOCARDCOLOR : otherwise
 **
 ** @author    Borg Enders
 **
 ** @version   0.7.6
 **
 ** @todo      remove
 **/
Card::Color
Card::InttoColor(int const i)
{
  switch(i) {
  case 0:
    return DIAMOND;
  case 1:
    return HEART;
  case 2:
    return SPADE;
  case 3:
    return CLUB;
  default:
    DEBUG_ASSERTION(false,
		    "Card::InttoColor(): illegal value");

    break;
  } // switch (i)

  return NOCARDCOLOR;
} // Card::Color Card::InttoColor(int i)

/**
 ** -> result
 **
 ** @param     color_a   first color
 ** @param     color_b   second color
 **
 ** @return    whether 'color_a' is greater than 'color_b'
 **	       (in the order: 'club', 'spade', 'heart', 'diamond'
 **
 ** @author    Diether Knof
 ** @author    Borg Enders
 **
 ** @version	0.7.6
 **/
bool
operator>(Card::Color const color_a, Card::Color const color_b)
{
  switch (color_a) {
  case Card::CLUB:
    return (   (color_b == Card::SPADE)
            || (color_b == Card::HEART)
            || (color_b == Card::DIAMOND) );
  case Card::SPADE:
    return (   (color_b == Card::HEART)
            || (color_b == Card::DIAMOND) );
  case Card::HEART:
    return (color_b == Card::DIAMOND);
  case Card::DIAMOND:
    return false;
  default:
    DEBUG_ASSERTION(false,
                    "operator>(color_a, color_b): illegal value");

    break;
  } // switch (color_a)

  return false;
} // bool operator>(Card::Color color_a, Card::Color color_b)

/**
 ** -> result
 **
 ** @param     tcolor   card tcolor
 **
 ** @return    name of the tcolor
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.6
 **/
string
name(Card::TColor const tcolor)
{
  switch(tcolor) {
  case Card::CLUB:
    return "club";
  case Card::SPADE:
    return "spade";
  case Card::HEART:
    return "heart";
  case Card::DIAMOND:
    return "diamond";
  case Card::TRUMP:
    return "trump";
  case Card::NOCARDCOLOR:
    return "no card color";
  case Card::UNKNOWNCARDCOLOR:
    return "unknown card color";
  } // switch(tcolor)

  return "";
} // string name(Card::TColor const& tcolor)

/**
 ** -> result
 **
 ** @param     tcolor   card tcolor
 **
 ** @return    name of the tcolor
 **/
string
to_string(Card::TColor const tcolor)
{
  return name(tcolor);
} // string to_string(Card::TColor tcolor)


/**
 ** -> result
 **
 ** @param     color_name   name of the color
 **
 ** @return    color 'name'
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.6
 **/
Card::Color
Card::color(string const& color_name)
{
  if (color_name == ::name(Card::CLUB))
    return Card::CLUB;
  else if (color_name == ::name(Card::SPADE))
    return Card::SPADE;
  else if (color_name == ::name(Card::HEART))
    return Card::HEART;
  else if (color_name == ::name(Card::DIAMOND))
    return Card::DIAMOND;
  else
    DEBUG_ASSERTION(false,
                    "Card::color(color_name):\n"
                    "  color '" << color_name << "' unknown");

  return Card::NOCARDCOLOR;
} // static Card::Color Card::color(string color_name)

/**
 ** writes the card color in the output stream
 **
 ** @param     ostr     output stream
 ** @param     tcolor   card color
 **
 ** @return    output stream
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.6
 **/
ostream&
operator<<(ostream& ostr, Card::TColor const tcolor)
{
  return (ostr << name(tcolor));
} // ostream& operator<<(ostream& ostr, Card::TColor tcolor);

/**
 ** read the card color out of the stream
 **
 ** @param     istr    input stream
 ** @param     color   card color
 **
 ** @return    output stream
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.6
 **/
istream&
operator>>(istream& istr, Card::Color& color)
{
  string color_name;
  istr >> color_name;
  color = Card::color(color_name);

  return istr;
} // istream& operator>>(istream& istr, Card::Color& color);
