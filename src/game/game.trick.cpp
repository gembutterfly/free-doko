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

#include "game.h"

#include "../party/rule.h"
#include "../card/trick.h"

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	the tricks
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.8
 **/
vector<Trick*>&
Game::tricks()
{
  return this->tricks_;
} // vector<Trick*>& Game::tricks()

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	number of tricks in game
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 **
 ** @version	0.4.4
 **/
unsigned
Game::trickno() const
{
  return this->rule()(Rule::NUMBER_OF_TRICKS_IN_GAME);
} // unsigned Game::trickno() const

/**
 ** -> result
 **
 ** @param	t	number of trick, which should be result of this method
 **
 ** @return	returns trick i, if i is legal
 **		otherwise initialized trick
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 **
 ** @version	0.4.4
 **/
Trick const&
Game::trick(unsigned const& t) const
{
  DEBUG_ASSERTION((t < this->tricks_.size()),
		  "Game::trick(t):\n"
		  "  t = " << t << " is too great"
		  " (>= " << this->tricks_.size() << ")");

  return *this->tricks_[t];
} // Trick const& Game::trick(unsigned const& t) const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	the current trick
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 **
 ** @version	0.4.5
 **/
Trick const&
Game::trick_current() const
{
  DEBUG_ASSERTION(!this->tricks_.empty(),
		  "Game::trick_current():\n"
		  "  Game::tricks_ is empty");

  return *this->tricks_.back();
} // Trick const& Game::trick_current() const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	the current trick
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 **
 ** @version	0.4.4
 **/
Trick&
Game::trick_current()
{
  DEBUG_ASSERTION(!this->tricks_.empty(),
		  "Game::trick_current():\n"
		  "  Game::tricks_ is empty");

  return *this->tricks_.back();
} // Trick& Game::trick_current()

/**
 **
 ** -> result
 ** a full trick is one trick further
 **
 ** @param	-
 **
 ** @return	the number of the current trick
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **
 ** @todo	replace with 'real_trick_current_no'
 **/
unsigned
Game::trick_current_no() const
{
  if (this->tricks_.empty())
    if (::game_status < GAMESTATUS::GAME_PLAY)
      return UINT_MAX;
    else
      return 0;

  if ((::game_status == GAMESTATUS::GAME_FINISHED)
      || (this->trick_current().isfull()))
    return this->tricks_.size();

  return (this->tricks_.size() - 1);
} // unsigned Game::trick_current_no() const

/**
 ** -> result
 ** a full trick in trickpile is one trick further
 **
 ** @param	-
 **
 ** @return	the number of tricks in the trickpile
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.9
 **/
unsigned
Game::tricks_in_trickpiles() const
{
  if (this->tricks_.empty())
    if (::game_status < GAMESTATUS::GAME_PLAY)
      return UINT_MAX;
    else
      return 0;

  if ((::game_status == GAMESTATUS::GAME_FINISHED)
      || (::game_status == GAMESTATUS::GAME_TRICK_TAKEN))
    return this->tricks_.size();

  return (this->tricks_.size() - 1);
} // unsigned Game::tricks_in_trickpiles() const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	the number of the current trick
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **/
unsigned
Game::real_trick_current_no() const
{
  if (this->tricks_.empty())
    if (::game_status < GAMESTATUS::GAME_PLAY)
      return UINT_MAX;
    else
      return 0;

  if (::game_status == GAMESTATUS::GAME_FINISHED)
    return this->tricks_.size();

  return (this->tricks_.size() - 1);
} // unsigned Game::real_trick_current_no() const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	the number of remaining tricks
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 **
 ** @version	0.4.4
 **/
unsigned
Game::tricks_remaining_no() const
{
  if (   (::game_status == GAMESTATUS::GAME_RESERVATION)
      || (::game_status == GAMESTATUS::GAME_POVERTY_SHIFT) )
    return this->trickno();

  DEBUG_ASSERTION((this->trick_current_no() != UINT_MAX),
		  "Game::tricks_remaining_no():\n"
		  "  this->trick_current_no() == -1");

  return (this->trickno() - this->trick_current_no());
} // unsigned Game::tricks_remaining_no() const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	the number of remaining tricks
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 **
 ** @version	0.7.3
 **/
unsigned
Game::real_tricks_remaining_no() const
{
  DEBUG_ASSERTION((this->real_trick_current_no() != UINT_MAX),
		  "Game::real_tricks_remaining_no():\n"
		  "  this->real_trick_current_no() == -1");

  return (this->trickno() - this->real_trick_current_no());
} // unsigned Game::real_tricks_remaining_no() const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	whether this is the last trick
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.6
 **/
bool
Game::is_last_trick() const
{
  if ( (::game_status == GAMESTATUS::GAME_RESERVATION)
       || (::game_status == GAMESTATUS::GAME_POVERTY_SHIFT) )
    return false;
  return (this->tricks_remaining_no() == 1);
} // bool Game::is_last_trick() const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	number of trump tricks (with the current trick)
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.12
 **/
unsigned
Game::trumptricks_no() const
{
  unsigned n = 0;
  for (vector<Trick*>::const_iterator t = this->tricks_.begin();
       t != this->tricks_.end();
       ++t)
    if (   !(*t)->isempty()
        && (*t)->startcard().istrump())
      n += 1;
  return n;
} // unsigned Game::trumptrick_no() const
