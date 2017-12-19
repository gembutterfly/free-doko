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

#include "gameplay_action.h"
#include "gameplay_actions.h"

#include "../card/trick.h"
#include "../card/hand.h"
#include "../player/player.h"

#include "../utils/string.h"


/**
 ** returns an action type determined by the line
 **
 ** @param	line	line with the action data
 **
 ** @return	action corresponding to the line
 **		(to be freed)
 **
 ** @author	Diether Knof
 ** 
 ** @version	0.7.6
 **/
GameplayAction*
GameplayAction::new_(string const& line)
{
  if (line.empty())
    return NULL;

  string const keyword
    = DK::Utils::String::remove_blanks(  (std::find(line.begin(), line.end(), '=')
					  != line.end())
				       ? string(line.begin(),
						std::find(line.begin(), line.end(), '=') - 1)
				       : line
				      );

  if (keyword == ::name(RESERVATION))
    return new Reservation(line);
  else if (keyword == ::name(MARRIAGE))
    return new Marriage(line);
  else if (keyword == ::name(GENSCHER))
    return new Genscher(line);
  else if (keyword == ::name(POVERTY_SHIFT))
    return new PovertyShift(line);
  else if (keyword == ::name(POVERTY_ACCEPTED))
    return new PovertyAccepted(line);
  else if (keyword == ::name(POVERTY_RETURNED))
    return new PovertyReturned(line);
  else if (keyword == ::name(POVERTY_DENIED))
    return new PovertyDenied(line);
  else if (keyword == ::name(POVERTY_DENIED_BY_ALL))
    return new PovertyDeniedByAll(line);
  else if (keyword == ::name(ANNOUNCEMENT))
    return new Announcement(line);
  else if (keyword == ::name(SWINES))
    return new Swines(line);
  else if (keyword == ::name(HYPERSWINES))
    return new Hyperswines(line);
  else if (keyword == ::name(CARD_PLAYED))
    return new CardPlayed(line);
  else if (keyword == ::name(TRICK_OPEN))
    return new TrickOpen(line);
  else if (keyword == ::name(TRICK_FULL))
    return new TrickFull(line);
  else if (keyword == ::name(TRICK_TAKEN))
    return new TrickTaken(line);
  else if (   (keyword == ::name(CHECK))
           || (keyword == "CHECK"))
    return new Check(line);
  else if (   (keyword == ::name(PRINT))
           || (keyword == "PRINT"))
    return new Print(line);

  return NULL;
} // static GameplayAction* GameplayAction::new_(string line)

/**
 ** returns an action type determined by the line
 **
 ** @param	line	line with the action data
 ** @param	istr	input stream (behind the line)
 **
 ** @return	action corresponding to the line
 **		(to be freed)
 **
 ** @author	Diether Knof
 **  
 ** @version	0.6.8
 **  
 ** @todo	remove (take the function without istr)
 **/
GameplayAction*
GameplayAction::new_(string const& line, istream& istr)
{
  if (line.empty())
    return NULL;

#ifndef OUTDATED
  // old bug report format (< 0.7.3)
  string const keyword
    = DK::Utils::String::remove_blanks
    ( (std::find(line.begin(), line.end(), ':')
       < std::find(line.begin(), line.end(), '='))
     ? string(line.begin(),
	      std::find(line.begin(), line.end(), ':'))
     : (  (std::find(line.begin(), line.end(), '=')
	   != line.end())
	? string(line.begin(),
		 std::find(line.begin(), line.end(), '='))
	: line
       )
    );
#else
  string const keyword
    = DK::Utils::String::remove_blanks
    (  (std::find(line.begin(), line.end(), '=')
	!= line.end())
     ? string(line.begin(),
	      std::find(line.begin(), line.end(), '=') - 1)
     : line
    );
#endif

#ifndef OUTDATED
  // old bug report format (< 0.7.3)
  if (keyword == "Marriage")
    return new Marriage(line);
  else if (keyword == "Genscher")
    return new Genscher(line);
  else if (keyword == "Poverty Shift")
    return new PovertyShift(line);
  else if (keyword == "Poverty Returned")
    return new PovertyReturned(line);
  else if (keyword == "Poverty Denied By All")
    return new PovertyDeniedByAll(line);
  else if (keyword == "Announcement")
    return new Announcement(line);
  else if (keyword == "Swines")
    return new Swines(line);
  else if (keyword == "Hyperswines")
    return new Hyperswines(line);
  else if (keyword == "Played")
    return new CardPlayed(line);
  else if (keyword == "Trick full")
    return new TrickFull(line, istr);
#endif

  if (keyword == ::name(RESERVATION))
    return new Reservation(line);
  else if (keyword == ::name(MARRIAGE))
    return new Marriage(line);
  else if (keyword == ::name(GENSCHER))
    return new Genscher(line);
  else if (keyword == ::name(POVERTY_SHIFT))
    return new PovertyShift(line);
  else if (keyword == ::name(POVERTY_ACCEPTED))
    return new PovertyAccepted(line);
  else if (keyword == ::name(POVERTY_RETURNED))
    return new PovertyReturned(line);
  else if (keyword == ::name(POVERTY_DENIED))
    return new PovertyDenied(line);
  else if (keyword == ::name(POVERTY_DENIED_BY_ALL))
    return new PovertyDeniedByAll(line);
  else if (keyword == ::name(ANNOUNCEMENT))
    return new Announcement(line);
  else if (keyword == ::name(SWINES))
    return new Swines(line);
  else if (keyword == ::name(HYPERSWINES))
    return new Hyperswines(line);
  else if (keyword == ::name(CARD_PLAYED))
    return new CardPlayed(line);
  else if (keyword == ::name(TRICK_OPEN))
    return new TrickOpen(line);
  else if (keyword == ::name(TRICK_FULL))
    return new TrickFull(line, istr);
  else if (keyword == ::name(TRICK_TAKEN))
    return new TrickTaken(line);
  else if (keyword == ::name(CHECK))
    return new Check(line);
  else if (keyword == ::name(PRINT))
    return new Print(line);

  return NULL;
} // static GameplayAction* GameplayAction::new_(string line, istream& istr)

/**
 ** constructor
 **
 ** @param	type		action type
 ** @param	playerno	player who has made the action
 **
 ** @return	-
 **
 ** @author	Diether Knof
 ** 
 ** @version	0.6.8
 **/
GameplayAction::GameplayAction(Type const type,
			       unsigned const playerno) :
  type(type),
  player(playerno)
{
} // GameplayAction::Action(Type type, unsigned playerno)

/**
 ** destructor
 **
 ** @param	-
 **
 ** @return	-
 ** 
 ** @author	Diether Knof
 **
 ** @version	0.6.8
 **/
GameplayAction::~GameplayAction()
{ }

/**
 ** copy constructor
 **
 ** @param	action	action to copy
 **
 ** @return	-
 **
 ** @author	Diether Knof
 ** 
 ** @version	0.6.8
 **/
GameplayAction::GameplayAction(GameplayAction const& action) :
  type(action.type),
  player(action.player)
{ }

/**
 ** copy operator
 **
 ** @param	action	action to copy
 **
 ** @return	-
 **
 ** @author	Diether Knof
 ** 
 ** @version	0.6.8
 **/
GameplayAction&
GameplayAction::operator=(GameplayAction const& action)
{
  this->type = action.type;
  this->player = action.player;

  return *this;
} // GameplayAction& operator=(GameplayAction action)

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    clone of this gameplay action
 **
 ** @author    Diether Knof
 ** 
 ** @version   0.7.6
 **/
GameplayAction*
GameplayAction::clone() const
{
  switch (this->type) {
  case RESERVATION:
    return new Reservation(static_cast<Reservation const&>(*this));
  case MARRIAGE:
    return new Marriage(static_cast<Marriage const&>(*this));
  case GENSCHER:
    return new Genscher(static_cast<Genscher const&>(*this));
  case POVERTY_SHIFT:
    return new PovertyShift(static_cast<PovertyShift const&>(*this));
  case POVERTY_ACCEPTED:
    return new PovertyAccepted(static_cast<PovertyAccepted const&>(*this));
  case POVERTY_RETURNED:
    return new PovertyReturned(static_cast<PovertyReturned const&>(*this));
  case POVERTY_DENIED:
    return new PovertyDenied(static_cast<PovertyDenied const&>(*this));
  case POVERTY_DENIED_BY_ALL:
    return new PovertyDeniedByAll(static_cast<PovertyDeniedByAll const&>(*this));
  case ANNOUNCEMENT:
    return new Announcement(static_cast<Announcement const&>(*this));
  case SWINES:
    return new Swines(static_cast<Swines const&>(*this));
  case HYPERSWINES:
    return new Hyperswines(static_cast<Hyperswines const&>(*this));
  case CARD_PLAYED:
    return new CardPlayed(static_cast<CardPlayed const&>(*this));
  case TRICK_OPEN:
    return new TrickOpen(static_cast<TrickOpen const&>(*this));
  case TRICK_FULL:
    return new TrickFull(static_cast<TrickFull const&>(*this));
  case TRICK_TAKEN:
    return new TrickTaken(static_cast<TrickTaken const&>(*this));
  case CHECK:
    return new Check(static_cast<Check const&>(*this));
  case PRINT:
    return new Print(static_cast<Print const&>(*this));
  } // switch (this->type)

  return NULL;
} // GameplayAction* GameplayAction::clone() const


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
GameplayAction::equal(GameplayAction const& action) const
{
  return (   (this->type == action.type)
	  && (this->player == action.player));
} // bool GameplayAction::equal(GameplayAction action) const

/**
 ** -> result
 **
 ** @param	type	action tye
 **
 ** @return	name of the action type
 ** 
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
string
name(GameplayAction::Type const type)
{
  switch(type) {
  case GameplayAction::RESERVATION:
    return "reservation";
  case GameplayAction::MARRIAGE:
    return "marriage";
  case GameplayAction::GENSCHER:
    return "genscher";
  case GameplayAction::POVERTY_SHIFT:
    return "poverty shift";
  case GameplayAction::POVERTY_ACCEPTED:
    return "poverty accepted";
  case GameplayAction::POVERTY_RETURNED:
    return "poverty returned";
  case GameplayAction::POVERTY_DENIED:
    return "poverty denied";
  case GameplayAction::POVERTY_DENIED_BY_ALL:
    return "poverty denied by all";
  case GameplayAction::ANNOUNCEMENT:
    return "announcement";
  case GameplayAction::SWINES:
    return "swines";
  case GameplayAction::HYPERSWINES:
    return "hyperswines";
  case GameplayAction::CARD_PLAYED:
    return "card played";
  case GameplayAction::TRICK_OPEN:
    return "trick open";
  case GameplayAction::TRICK_FULL:
    return "trick full";
  case GameplayAction::TRICK_TAKEN:
    return "trick taken";
  case GameplayAction::CHECK:
    return "check";
  case GameplayAction::PRINT:
    return "print";
  } // switch(type)

  DEBUG_ASSERTION(false,
		  "name(GameplayAction::Type type):\n"
		  "  unknown type '" << static_cast<int>(type) << "'");

  return "";
} // string name(GameplayAction::Type type)

/**
 ** writes the type in the output stream
 **
 ** @param	ostr	output stream
 ** @param	type	action type
 **
 ** @return	output stream
 ** 
 ** @author	Diether Knof
 **
 ** @version	0.6.8
 **/
ostream&
operator<<(ostream& ostr, GameplayAction::Type const type)
{
  ostr << ::name(type);
  return ostr;
} // ostream& operator<<(ostream& ostr, GameplayAction::Type type)

/**
 ** -> result
 **
 ** @param     discrepancy   discrepancy
 **
 ** @return    name of the discrepancy
 ** 
 ** @author    Diether Knof
 **
 ** @version   0.7.3
 **/
string
name(GameplayAction::Discrepancy const discrepancy)
{
  switch(discrepancy) {
  case GameplayAction::FUTURE:
    return "future";
  case GameplayAction::NONE:
    return "none";
  case GameplayAction::SKIPPED:
    return "skipped";
  case GameplayAction::PLAYER:
    return "player";
  case GameplayAction::CARD:
    return "card";
  case GameplayAction::OTHER:
    return "other";
  } // switch(discrepancy)

  DEBUG_ASSERTION(false,
		  "name(GameplayAction::Discrepancy discrepancy):\n"
		  "  unknown discrepancy "
		  << "'" << static_cast<int>(discrepancy) << "'");

  return "";
} // string name(GameplayAction::Discrepancy discrepancy)

/**
 ** writes the type in the output stream
 **
 ** @param     ostr          output stream
 ** @param     discrepancy   action discrepancy
 **
 ** @return    output stream
 ** 
 ** @author    Diether Knof
 **
 ** @version   0.7.3
 **/
ostream&
operator<<(ostream& ostr, GameplayAction::Discrepancy const discrepancy)
{
  ostr << ::name(discrepancy);
  return ostr;
} // ostream& operator<<(ostream& ostr, GameplayAction::Discrepancy discrepancy)

/**
 ** writes the action in the output stream
 **
 ** @param	ostr	output stream
 ** @param	action	action
 **
 ** @return	output stream
 ** 
 ** @author	Diether Knof
 **
 ** @version	0.6.8
 **/
ostream&
operator<<(ostream& ostr, GameplayAction const& action)
{
  action.write(ostr);

  return ostr;
} // ostream& operator<<(ostream& ostr, GameplayAction action)

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	a string representing the action
 ** 
 ** @author	Diether Knof
 **
 ** @version	0.7.3
 **/
string
GameplayAction::str() const
{
  ostringstream ostr;

  this->write(ostr);

  return ostr.str();
} // string GameplayAction::str() const

/**
 ** -> result
 **
 ** @param	action_a   first action
 ** @param	action_b   second action
 **
 ** @return	whether the actions are equal
 **
 ** @author	Diether Knof
 ** 
 ** @version	0.7.3
 **/
bool
operator==(GameplayAction const& action_a, GameplayAction const& action_b)
{
  return action_b.equal(action_a);
} // bool operator==(GameplayAction action_a, GameplayAction action_b)

/**
 ** -> result
 **
 ** @param	action_a   first action
 ** @param	action_b   second action
 **
 ** @return	whether the actions are different
 **
 ** @author	Diether Knof
 ** 
 ** @version	0.7.3
 **/
bool
operator!=(GameplayAction const& action_a, GameplayAction const& action_b)
{
  return !(action_a == action_b);
} // bool operator!=(GameplayAction action_a, GameplayAction action_b)
