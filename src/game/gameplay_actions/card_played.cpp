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

#include "card_played.h"

#include "../../misc/translations.h"

#include "../../utils/string.h"

/**
 ** constructor
 **
 ** @param	playerno	player who has played the card
 ** @param	card		played card
 **
 ** @return	-
 **
 ** @author	Diether Knof
 ** 
 ** @version	0.7.1
 **/
GameplayAction::CardPlayed::CardPlayed(unsigned const playerno,
				       Card const& card) :
  Player1(CARD_PLAYED, playerno),
  card(card),
  heuristic(Aiconfig::NO_HEURISTIC)
{ }

/**
 ** constructor
 **
 ** @param     playerno    player who has played the card
 ** @param     card        played card
 ** @param     heuristic   heuristic of the card
 **
 ** @return    -
 **
 ** @author    Diether Knof
 ** 
 ** @version   0.7.3
 **/
GameplayAction::CardPlayed::CardPlayed(unsigned const playerno,
				       Card const& card,
				       Aiconfig::Heuristic const heuristic) :
  Player1(CARD_PLAYED, playerno),
  card(card),
  heuristic(heuristic)
{ }

/**
 ** constructor
 **
 ** @param     line   line with the data
 **
 ** @return    action corresponding to the line
 **            (to be freed)
 **
 ** @author    Diether Knof
 ** 
 ** @version   0.7.3
 ** 
 ** @todo      read heuristic
 **/
GameplayAction::CardPlayed::CardPlayed(string line) :
  Player1(CARD_PLAYED, UINT_MAX),
  card(),
  heuristic(Aiconfig::NO_HEURISTIC)
{
#ifndef OUTDATED
  // old format in the bug report:
  //   Played: Player 2: club ace
  if (string(line, 0, strlen("Played: ")) == "Played: ") {
    // the first word is 'Played'
    DK::Utils::String::word_first_remove_with_blanks(line);
    // the next word is 'player', remove it also
    DK::Utils::String::word_first_remove_with_blanks(line);
    // next comes the player number, followed by a ':'
    this->player = static_cast<unsigned>(atoi(line.c_str()));
    // remove the player number
    DK::Utils::String::word_first_remove_with_blanks(line);

    // the last part is the card
    this->card = Card(line);

    return ;
  }
#endif

  // ex: card played = 0, club ace
  string const pre = ::name(this->type) + " =";
  if (string(line, 0, pre.length()) != pre)
    return ;

  istringstream istr(string(line, pre.length()));
  istr >> this->player;
  if (istr.get() != ',')
    return ;
  istr >> this->card;


  // ToDo: read heuristic
  while (   istr.good()
	 && ::isspace(istr.peek()))
    istr.get();

  if (!istr.good())
    return ;

  if (istr.peek() != '(')
    return ;
  istr.get();

  string heuristic_name;
  while (   istr.good()
	 && (istr.peek() != ')'))
    heuristic_name.push_back(istr.get());

  if (istr.peek() != ')')
    return ;

  this->heuristic = Aiconfig::Heuristic_from_name(heuristic_name);

  return ;
} // GameplayAction::CardPlayed(string line)

/**
 ** copy constructor
 **
 ** @param	card_played	action to copy
 **
 ** @return	-
 **
 ** @author	Diether Knof
 ** 
 ** @version	0.7.3
 **/
GameplayAction::CardPlayed::CardPlayed(CardPlayed const& card_played) :
  Player1(card_played),
  card(card_played.card),
  heuristic(card_played.heuristic)
{ }

/**
 ** copy constructor
 **
 ** @param	card_played	action to copy
 **
 ** @return	this action
 **
 ** @author	Diether Knof
 ** 
 ** @version	0.7.3
 **/
GameplayAction::CardPlayed&
GameplayAction::CardPlayed::operator=(CardPlayed const& card_played)
{
  this->Player1::operator=(card_played);
  this->card = card_played.card;
  this->heuristic = card_played.heuristic;
  return *this;
} // CardPlayed& CardPlayed::operator=(CardPlayed card_played)

/**
 ** destructor
 **
 ** @param	-
 **
 ** @return	-
 **
 ** @author	Diether Knof
 ** 
 ** @version	0.7.1
 **/
GameplayAction::CardPlayed::~CardPlayed()
{ }

/**
 ** -> result
 **
 ** @param	action	action to compare with
 **
 ** @return	whether the action is equal to this
 **
 ** @author	Diether Knof
 ** 
 ** @version	0.7.3
 **/
bool
GameplayAction::CardPlayed::equal(GameplayAction const& action) const
{
  return (   (this->GameplayAction::equal(action))
	  && (this->card == static_cast<GameplayAction::CardPlayed const&>(action).card));
} // bool GameplayAction::CardPlayed::equal(GameplayAction action) const

/**
 **
 ** writes the action in the output stream
 **
 ** @param	ostr	output stream
 **
 ** @return	-
 **
 ** @author	Diether Knof
 ** 
 ** @version	0.7.1
 **/
void
GameplayAction::CardPlayed::write(ostream& ostr) const
{
  ostr << this->type;
  ostr << " = " << this->player;
  ostr << ", " << this->card;

  if (this->heuristic != Aiconfig::NO_HEURISTIC)
    ostr << " (" << this->heuristic << ")";

  return ;
} // void write(ostream& ostr) const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	a translation representing the data
 ** 
 ** @author	Diether Knof
 **
 ** @version	0.7.1
 **/
Translator::Translation
GameplayAction::CardPlayed::data_translation() const
{
  if (this->heuristic == Aiconfig::NO_HEURISTIC)
    return ::translation(this->card);
  else
    return (::translation(this->card)
	    + " (" + ::translation(this->heuristic) + ")");
} // virtual Translator::Translation GameplayAction::CardPlayed::data_translation() const
