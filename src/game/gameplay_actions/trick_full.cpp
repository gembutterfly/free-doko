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

#include "trick_full.h"

#include "../../misc/translations.h"
#include "../../utils/string.h"

/**
 ** constructor
 **
 ** @param	trick	full trick
 **
 ** @return	-
 **
 ** @author	Diether Knof
 ** 
 ** @version	0.7.1
 **/
GameplayAction::TrickFull::TrickFull(Trick const& trick) :
  GameplayAction(TRICK_FULL, UINT_MAX),
  trick(trick)
{ }

/**
 ** constructor
 **
 ** @param	line	line with the data
 **
 ** @return	action corresponding to the line
 **		(to be freed)
 **
 ** @author	Diether Knof
 ** 
 ** @version	0.7.3
 **/
GameplayAction::TrickFull::TrickFull(string const& line) :
  GameplayAction(TRICK_FULL, UINT_MAX),
  trick()
{
  // ex: trick full = 0, diamond queen, diamond jack, spade queen, diamond nine, 2

  string const pre = ::name(this->type) + " =";
  if (string(line, 0, pre.length()) != pre)
    return ;

  // first split the line in the single parts
  vector<string> const parts(DK::Utils::String::split(string(line, pre.length()),
						    ','));
  vector<string>::const_iterator p = parts.begin();
  if (p == parts.end()) {
      this->player = UINT_MAX;
      return ;
  }

  unsigned startplayer = UINT_MAX;
  unsigned winnerplayer = UINT_MAX;
  vector<Card> cards;
  { // read the player
    if (*p == "-")
      return ;

    istringstream istr(*p);
    istr >> startplayer;
    ++p;
  } // read the player

  { // read the cards
    for (; p != parts.end(); ++p) {
      if (isdigit((*p)[0]))
	  break;
      cards.push_back(Card(*p));
    }
  } // read the cards
  { // read the winnerplayer
    if (p != parts.end()) {
      istringstream istr(*p);
      istr >> winnerplayer;
      ++p;
    }
  } // read the winnerplayer
  if (p != parts.end())
    return ;

  this->trick = Trick(startplayer, cards, winnerplayer);

  return ;
} // GameplayAction::TrickFull(string line)

/**
 ** constructor
 **
 ** @param	line	line with the data
 ** @param	istr	input stream (behind the line) (to be removed)
 **
 ** @return	action corresponding to the line
 **		(to be freed)
 **
 ** @author	Diether Knof
 ** 
 ** @version	0.7.3
 **/
GameplayAction::TrickFull::TrickFull(string const& line,
				     istream& istr) :
  GameplayAction(TRICK_FULL, UINT_MAX),
  trick()
{
#ifndef OUTDATED
  // old format in the bug report:
  //   Played: Player 2: club ace
  if (string(line, 0, strlen("Trick full: ")) == "Trick full: ") {
    this->trick = Trick(istr);

    return ;
  }
#endif

#ifdef WORKAROUND
  // copied from 'TrickFull::TrickFull(string line)'

  // ex: trick full = 0, diamond queen, diamond jack, spade queen, diamond nine, 2

  { // new format: one line
    string const pre = ::name(this->type) + " =";
    if (string(line, 0, pre.length()) != pre)
      return ;

    // first split the line in the single parts
    vector<string> const parts(DK::Utils::String::split(string(line, pre.length()),
						      ','));
    vector<string>::const_iterator p = parts.begin();
    if (p == parts.end()) {
      this->player = UINT_MAX;
      return ;
    }

    unsigned startplayer = UINT_MAX;
    unsigned winnerplayer = UINT_MAX;
    vector<Card> cards;
    { // read the player
      if (*p == "-")
	return ;

      istringstream istr(*p);
      istr >> startplayer;
      ++p;
    } // read the player

    { // read the cards
      for (; p != parts.end(); ++p) {
	if (isdigit((*p)[0]))
	  break;
	cards.push_back(Card(*p));
      }
    } // read the cards
    { // read the winnerplayer
      if (p != parts.end()) {
	istringstream istr(*p);
	istr >> winnerplayer;
	++p;
      }
    } // read the winnerplayer
    if (p != parts.end())
      return ;

    this->trick = Trick(startplayer, cards, winnerplayer);
  } // new format: one line
#endif // #ifdef WORKAROUND

  return ;
} // GameplayAction::TrickFull(string line, istream& istr)

/**
 ** copy constructor
 **
 ** @param	trick_full	action to copy
 **
 ** @return	-
 **
 ** @author	Diether Knof
 ** 
 ** @version	0.7.1
 **/
GameplayAction::TrickFull::TrickFull(TrickFull const& trick_full) :
  GameplayAction(trick_full),
  trick(trick_full.trick)
{ }

/**
 ** copy operator
 **
 ** @param	trick_full	action to copy
 **
 ** @return	this action
 **
 ** @author	Diether Knof
 ** 
 ** @version	0.7.1
 **/
GameplayAction::TrickFull&
GameplayAction::TrickFull::operator=(TrickFull const& trick_full)
{
  this->GameplayAction::operator=(trick_full);
  this->trick = trick_full.trick;
  return *this;
} // TrickFull& TrickFull::operator=(TrickFull trick_full)

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
GameplayAction::TrickFull::~TrickFull()
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
GameplayAction::TrickFull::equal(GameplayAction const& action) const
{
  return (   (this->GameplayAction::equal(action))
	  && (this->trick == static_cast<GameplayAction::TrickFull const&>(action).trick));
} // bool GameplayAction::TrickFull::equal(GameplayAction action) const

/**
 ** writes the action in the output stream
 **
 ** @param	ostr	output stream
 **
 ** @return	-
 **
 ** @author	Diether Knof
 ** 
 ** @version	0.7.3
 **/
void
GameplayAction::TrickFull::write(ostream& ostr) const
{
  ostr << this->type << " = ";
  if (this->trick.startplayerno() == UINT_MAX) {
    ostr << "-";
    return ;
  }

  ostr << this->trick.startplayerno();
  for (unsigned c = 0; c < this->trick.actcardno(); ++c)
    ostr << ", " << this->trick.card(c);

  // cannot use 'Trick::isfull()' because that needs a corresponding game/party
  if (this->trick.winnerplayerno() != UINT_MAX)
    ostr << ", " << this->trick.winnerplayerno();

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
 ** @version	0.7.3
 **/
Translator::Translation
GameplayAction::TrickFull::data_translation() const
{
  Translator::Translation translation(::translator);

  if (this->trick.startplayerno() == UINT_MAX)
    return translation += string("-");

  translation
    += DK::Utils::String::to_string(this->trick.startplayerno());

  for (unsigned c = 0; c < this->trick.actcardno(); ++c)
    translation += ", " + ::translation(this->trick.card(c));

  if (this->trick.winnerplayerno() != UINT_MAX)
    translation
      += ", " + DK::Utils::String::to_string(this->trick.winnerplayerno());

  return translation;
} // virtual Translator::Translation GameplayAction::TrickFull::data_translation() const
