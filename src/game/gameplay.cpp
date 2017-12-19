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

#include "gameplay.h"
#include "gameplay_actions.h"

#include "game.h"
#include "../card/trick.h"
#include "../card/hand.h"
#include "../player/player.h"

/**
 ** constructor
 **
 ** @param	-
 **
 ** @return	-
 **
 ** @author	Diether Knof
 ** 
 ** @version	0.6.8
 **/
Gameplay::Gameplay() :
  OS(OS_TYPE::GAMEPLAY),
  seed_(UINT_MAX),
  gametype_(),
  soloplayer_(UINT_MAX),
  actions_()
{
  return ;
} // Gameplay::Gameplay()

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
Gameplay::~Gameplay()
{
  for (vector<GameplayAction*>::iterator a = this->actions_.begin();
       a != this->actions_.end();
       ++a)
    delete *a;
  return ;
} // Gameplay::~Gameplay()

/**
 ** writes the gameplay in the output stream
 **
 ** @param	ostr		output stream
 **
 ** @return	output stream
 **
 ** @author	Diether Knof
 ** 
 ** @version	0.6.8
 **/
ostream&
Gameplay::write(ostream& ostr) const
{
  ostr
    << "seed = " << this->seed() << '\n'
    << "gametype = " << this->gametype() << '\n'
    << "soloplayer = " << this->soloplayer() << '\n';

  ostr
    << "hands\n"
    << "{\n";
  for (vector<Hand>::const_iterator h = this->hands().begin();
       h != this->hands().end();
       ++h)
    ostr
      << "{\n"
      << *h
      << "}\n";
  ostr << "}\n";

  ostr
    << "actions\n"
    << "{\n";
  for (vector<GameplayAction*>::const_iterator a = this->actions().begin();
       a != this->actions().end();
       ++a)
    ostr << "  " << **a << '\n';
  ostr << "}\n";

  return ostr;
} // ostream& Gameplay::write(ostream& ostr) const

/**
 ** the game is opened
 **
 ** @param	game	game that is opened
 **
 ** @return	-
 **
 ** @author	Diether Knof
 ** 
 ** @version	0.6.8
 **/
void
Gameplay::game_open(Game const& game)
{
  this->OS::game_open(game);

  for (vector<GameplayAction*>::iterator a = this->actions_.begin();
       a != this->actions_.end();
       ++a)
    delete *a;
  this->actions_.clear();

  return ;
} // void Gameplay::game_open(Game const& game)

/**
 ** the cards are distributed
 **
 ** @param	-
 **
 ** @return	-
 **
 ** @author	Diether Knof
 ** 
 ** @version	0.6.8
 **/
void
Gameplay::game_cards_distributed()
{
  this->seed_ = this->game().seed();

  this->hands_.clear();
  for (unsigned p = 0; p < this->game().number_of_players(); ++p)
    this->hands_.push_back(this->game().player(p).hand());

  return ;
} // void Gameplay::game_cards_distributed()

/**
 ** the game is redistributed
 **
 ** @param	-
 **
 ** @return	-
 **
 ** @author	Diether Knof
 ** 
 ** @version	0.6.8
 **/
void
Gameplay::game_redistribute()
{
  for (vector<GameplayAction*>::iterator a = this->actions_.begin();
       a != this->actions_.end();
       ++a)
    delete *a;
  this->actions_.clear();

  return ;
} // void Gameplay::game_redistribute()

/**
 ** the game is started
 **
 ** @param	-
 **
 ** @return	-
 **
 ** @author	Diether Knof
 ** 
 ** @version	0.6.8
 **/
void
Gameplay::game_start()
{
  this->gametype_ = this->game().type();

  if (is_solo(this->gametype()))
    this->soloplayer_ = this->game().no(this->game().soloplayer());
  else
    this->soloplayer_ = UINT_MAX;

  return ;
} // void Gameplay::game_start()

/**
 ** the game is closed
 **
 ** @param	-
 **
 ** @return	-
 **
 ** @author	Diether Knof
 ** 
 ** @version	0.6.8
 **/
void
Gameplay::game_close()
{
  this->OS::game_close();

  return ;
} // void Gameplay::game_close()

/**
 ** a gameplay action
 **
 ** @param	action   the gameplay action
 **
 ** @return	-
 **
 ** @author	Diether Knof
 ** 
 ** @version	0.7.5
 **/
void
Gameplay::gameplay_action(GameplayAction const& action)
{
#ifdef WORKAROUND
  // In order to get the syncing of the network games work,
  // in a full trick the announcements have to be between
  // 'trick full' and 'trick taken'
  switch (action.type) {
  case GameplayAction::TRICK_FULL: {
    // insert this action directly after the last played card
    vector<GameplayAction*>::iterator a = this->actions_.end();
    --a;
    while ((*a)->type != GameplayAction::CARD_PLAYED)
      --a;

    ++a;
    this->actions_.insert(a, action.clone());
    break;
  } // case GameplayAction::TRICK_FULL:
  case GameplayAction::TRICK_OPEN: {
    // move the last 'trick taken' action just before this action
#ifdef OUTDATED
    // 2014-08-03 dknof
    // Should not change anything
    vector<GameplayAction*>::iterator a = this->actions_.end();
    for (--a; (  (a != this->actions_.begin())
               &&  ((*a)->type != GameplayAction::TRICK_TAKEN)); a--)
      ;
    if ((*a)->type == GameplayAction::TRICK_TAKEN) {
      this->actions_.splice(a, this->actions_, a);
    }
#endif

    this->actions_.push_back(action.clone());
    break;
  } // case GameplayAction::TRICK_FULL:
  default:
    this->actions_.push_back(action.clone());
    break;
  } // switch (action.type())
#else
  this->actions_.push_back(action.clone());
#endif

  return ;
} // void Gameplay::gameplay_action(GameplayAction action)

/**
 ** writes the gameplay in the output stream
 **
 ** @param	ostr		output stream
 ** @param	gameplay	gameplay
 **
 ** @return	output stream
 **
 ** @author	Diether Knof
 ** 
 ** @version	0.6.8
 **/
ostream&
operator<<(ostream& ostr, Gameplay const& gameplay)
{
  gameplay.write(ostr);

  return ostr;
} // ostream& operator<<(ostream& ostr, Gameplay gameplay)
