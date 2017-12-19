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

unsigned const Card::NUMBER_OF_VALUES = 6;


/**
 ** -> result
 **
 ** @param     i   int value to be transformed into a CardValue
 **
 ** @return    ACE		: for input 0  or 1
 **		TEN		: for input 2  or 3
 **		KING		: for input 4  or 5
 **		QUEEN		: for input 6  or 7
 **		JACK		: for input 8  or 9
 **		NINE		: for input 10 or 11
 **		NOCARDVALUE	: otherwise
 **
 ** @author    Borg Enders
 **
 ** @version   0.5.4
 **
 ** @todo      remove
 **/
Card::Value
Card::InttoValue(int const i)
{
  switch(i) {
  case 0:
  case 1:
    return ACE;
  case 2:
  case 3:
    return TEN;
  case 4:
  case 5:
    return KING;
  case 6: 
  case 7:
    return QUEEN;
  case 8: 
  case 9:
    return JACK;
  case 10:
  case 11:
    return NINE;
  default: 
    DEBUG_ASSERTION(false, "Card::InttoValue(): illegal Value" << i);
    break;
  } // switch (i)

  return NOCARDVALUE;
} // Card::Value Card::InttoValue(int i)

/**
 ** -> result
 **
 ** @param     value_name   name of the value
 **
 ** @return    value 'name'
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.6
 **/
Card::Value
Card::value(string const& value_name)
{
  if (value_name == ::name(Card::NINE))
    return Card::NINE;
  else if (value_name == ::name(Card::JACK))
    return Card::JACK;
  else if (value_name == ::name(Card::QUEEN))
    return Card::QUEEN;
  else if (value_name == ::name(Card::KING))
    return Card::KING;
  else if (value_name == ::name(Card::TEN))
    return Card::TEN;
  else if (value_name == ::name(Card::ACE))
    return Card::ACE;
  else
    DEBUG_ASSERTION(false,
		    "Card::value(value_name):\n"
		    "  value '" << value_name << "' unknown");

  return NOCARDVALUE;
} // static Card::Value Card::value(string value_name)

/**
 ** -> result
 **
 ** @param     value   card value
 **
 ** @return    name of the value
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.6
 **/
string
name(Card::Value const value)
{
  switch(value) {
  case Card::NINE:
    return "nine";
  case Card::JACK:
    return "jack";
  case Card::QUEEN:
    return "queen";
  case Card::KING:
    return "king";
  case Card::TEN:
    return "ten";
  case Card::ACE:
    return "ace";
  case Card::NOCARDVALUE:
    return "no card value";
  case Card::UNKNOWNCARDVALUE:
    return "unknown card value";
  } // switch(value)

  return "";
} // string name(Card::Value value)

/**
 ** -> result
 **
 ** @param     value   card value
 **
 ** @return    name of the value
 **/
string
to_string(Card::Value const value)
{
  return name(value);
} // string to_string(Card::Value value)

/**
 ** writes the card value in the output stream
 **
 ** @param     ostr    output stream
 ** @param     value   card value
 **
 ** @return    output stream
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.6
 **/
ostream&
operator<<(ostream& ostr, Card::Value const value)
{
  return (ostr << name(value));
} // ostream& operator<<(ostream& ostr, Card::Value value);

/**
 ** read the card value out of the stream
 **
 ** @param     istr    input stream
 ** @param     value   card value
 **
 ** @return    output stream
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.6
 **/
istream&
operator>>(istream& istr, Card::Value& value)
{
  string value_name;
  istr >> value_name;

  value = Card::value(value_name);
  
  return istr;
} // istream& operator>>(istream& istr, Card::Value& value);
