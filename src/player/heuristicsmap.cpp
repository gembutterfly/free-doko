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

#include "heuristicsmap.h"
#include "player.h"
#include "../game/game.h"
#include "../party/rule.h"
#include "../basistypes.h"

#include "../misc/bug_report.h"

HeuristicsMap::Key const HeuristicsMap::Key::DEFAULT(HeuristicsMap::DEFAULT,
						     HeuristicsMap::RE);

/**
 ** -> result
 **
 ** @param	game	game
 **
 ** @return	group of the gametype
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 **   
 ** @version	0.7.3
 **/
HeuristicsMap::GametypeGroup
HeuristicsMap::group(Game const& game)
{
  GametypeGroup group = DEFAULT;

  switch(game.type()) {
  case GAMETYPE::NORMAL:
    return NORMAL;
  case GAMETYPE::THROWN_NINES:
  case GAMETYPE::THROWN_KINGS:
  case GAMETYPE::THROWN_NINES_AND_KINGS:
  case GAMETYPE::THROWN_RICHNESS:
  case GAMETYPE::FOX_HIGHEST_TRUMP:
    DEBUG_ASSERT(false);
    break;
  case GAMETYPE::POVERTY:
    return POVERTY;
  case GAMETYPE::GENSCHER:
    return NORMAL;
  case GAMETYPE::MARRIAGE:
    if (game.marriage_selector() == MARRIAGE_SELECTOR::TEAM_SET)
      return NORMAL;
    else
      return MARRIAGE_UNDETERMINED;
  case GAMETYPE::MARRIAGE_SOLO:
    return SOLI_COLOR;
  case GAMETYPE::MARRIAGE_SILENT:
    return MARRIAGE_SILENT;
  case GAMETYPE::SOLO_JACK:
  case GAMETYPE::SOLO_QUEEN:
  case GAMETYPE::SOLO_KING:
    return SOLI_SINGLE_PICTURE;
  case GAMETYPE::SOLO_QUEEN_JACK:
  case GAMETYPE::SOLO_KING_JACK:
  case GAMETYPE::SOLO_KING_QUEEN:
    return SOLI_DOUBLE_PICTURE;
  case GAMETYPE::SOLO_KOEHLER:
    return SOLO_KOEHLER;
  case GAMETYPE::SOLO_CLUB:
  case GAMETYPE::SOLO_SPADE:
  case GAMETYPE::SOLO_HEART:
  case GAMETYPE::SOLO_DIAMOND:
    return SOLI_COLOR;
  case GAMETYPE::SOLO_MEATLESS:
    return SOLO_MEATLESS;
  } // switch(game.type())

  DEBUG_ASSERT(group != DEFAULT);

  return DEFAULT;
} // HeuristicsMap::GametypeGroup HeuristicsMap::group(Game const& game)

/**
 **
 ** -> result
 **
 ** @param	player	player
 **
 ** @return	group of the player
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 **   
 ** @version	0.6.6
 **
 **/
HeuristicsMap::PlayerTypeGroup
HeuristicsMap::group(Player const& player)
{
  PlayerTypeGroup group;

  switch(player.team()) {
  case TEAM::RE:
    group = RE;
    break;
  case TEAM::CONTRA:
  case TEAM::MAYBE_CONTRA:
    group = CONTRA;
    break;
  default:
#ifndef RELEASE
    cerr << "HeuristicsMap::group(player):\n"
      <<"  wrong team " << player.team()
      << endl;
#if 0
    ::bug_report.report("HeuristicsMap::group(player):\n"
			"  wrong team " + ::name(player.team()));
#endif
    group = CONTRA;
    break;
#endif
#ifdef POSTPONED
    DEBUG_ASSERTION(false,
		    "HeuristicsMap::group(player):\n"
		    "  wrong team " << player.team());
#else
    group = CONTRA;
#endif
    break;
  } // switch(gametype)

  // special types

  // poverty
  if (    (player.game().type() == GAMETYPE::POVERTY)
       && (&player == &player.game().soloplayer()) )
    group = SPECIAL;

  // genscher
  if ( (player.game().type() == GAMETYPE::NORMAL)
       && player.game().rule()(Rule::GENSCHER)
       && (player.hand().numberoftrumpkings()
	   == player.game().rule()(Rule::NUMBER_OF_SAME_CARDS)) )
    group = SPECIAL;

  return group;
} // HeuristicsMap::PlayerTypeGroup HeuristicsMap::group(Player const& player)

/**
 ** constructor
 **
 ** @param	player	player
 **
 ** @return	key according to the player
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 **   
 ** @version	0.7.3
 **/
HeuristicsMap::Key::Key(Player const& player) :
  gametype_group(HeuristicsMap::group(player.game())),
  playertype_group(HeuristicsMap::group(player))
{ }

/**
 ** constructor
 **
 ** @param	gametype_group     game group
 ** @param	playertype_group   player group
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **   
 ** @version	0.7.3
 **/
HeuristicsMap::Key::Key(GametypeGroup const gametype_group,
			PlayerTypeGroup const playertype_group) :
  gametype_group(gametype_group),
  playertype_group(playertype_group)
{ }

/**
 ** -> result
 **
 ** @param	key_a   first key
 ** @param	key_b   second key
 **
 ** @return	whether 'key_a' is equal to 'key_b'
 **
 ** @author	Diether Knof
 **   
 ** @version	0.7.3
 **/
bool
HeuristicsMap::operator==(HeuristicsMap::Key const& key_a,
			 HeuristicsMap::Key const& key_b)
{
  return (   (key_a.gametype_group == key_b.gametype_group)
	  && (key_a.playertype_group == key_b.playertype_group));
} // bool HeuristicsMap::operator==(HeuristicsMap::Key key_a, HeuristicsMap::Key key_b)

/**
 ** -> result
 **
 ** @param	key_a   first key
 ** @param	key_b   second key
 **
 ** @return	whether 'key_a' is different to 'key_b'
 **
 ** @author	Diether Knof
 **   
 ** @version	0.7.3
 **/
bool
HeuristicsMap::operator!=(HeuristicsMap::Key const& key_a,
			 HeuristicsMap::Key const& key_b)
{
  return !(key_a == key_b);
} // bool HeuristicsMap::operator!=(HeuristicsMap::Key key_a, HeuristicsMap::Key key_b)

/**
 ** -> result
 **
 ** @param	key_a   first key
 ** @param	key_b   second key
 **
 ** @return	whether 'key_a' is less than 'key_b'
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 **   
 ** @version	0.7.3
 **/
bool
HeuristicsMap::operator<(HeuristicsMap::Key const& key_a,
			 HeuristicsMap::Key const& key_b)
{
  return (   (key_a.gametype_group < key_b.gametype_group)
	  || (   (key_a.gametype_group == key_b.gametype_group)
	      && (key_a.playertype_group < key_b.playertype_group)));
} // bool HeuristicsMap::operator<(HeuristicsMap::Key key_a, HeuristicsMap::Key key_b)

/**
 ** -> result
 **
 ** @param	group   group
 **
 ** @return	the name of the group
 **
 ** @author	Diether Knof
 **   
 ** @version	0.7.3
 **/
string
name(HeuristicsMap::GametypeGroup const group)
{
  switch (group) {
  case HeuristicsMap::DEFAULT:
    return "default";
  case HeuristicsMap::NORMAL:
    return "normal";
  case HeuristicsMap::POVERTY:
    return "poverty";
  case HeuristicsMap::MARRIAGE_UNDETERMINED:
    return "undetermined marriage";
  case HeuristicsMap::MARRIAGE_SILENT:
    return "silent marriage";
  case HeuristicsMap::SOLO_MEATLESS:
    return "solo meatless";
  case HeuristicsMap::SOLI_COLOR:
    return "soli color";
  case HeuristicsMap::SOLI_SINGLE_PICTURE:
    return "soli single picture";
  case HeuristicsMap::SOLI_DOUBLE_PICTURE:
    return "soli double picture";
  case HeuristicsMap::SOLO_KOEHLER:
    return "solo koehler";
  } // switch (group)

  return "";
} // string name(HeuristicsMap::GametypeGroup const group)

/**
 ** -> result
 **
 ** @param	name   name of the group
 **
 ** @return	the game type group with the name 'name'
 **
 ** @author	Diether Knof
 **   
 ** @version	0.7.3
 **/
HeuristicsMap::GametypeGroup
HeuristicsMap::GametypeGroup_from_name(string const& name)
{
  for (int g = GAMETYPE_GROUP_FIRST; g <= GAMETYPE_GROUP_LAST; ++g)
    if (name == ::name(static_cast<GametypeGroup>(g)))
	return static_cast<GametypeGroup>(g);

  DEBUG_ASSERTION(false,
		  "HeuristicsMap::GametypeGroup_from_name(" << name << ")\n"
		  "  did not find gametype group '" << name << "'");
  return GAMETYPE_GROUP_FIRST;
} // HeuristicsMap::GametypeGroup HeuristicsMap::GametypeGroup_from_name(string const& name)

/**
 ** -> result
 **
 ** @param	name   name of the group
 **
 ** @return	the player type group with the name 'name'
 **
 ** @author	Diether Knof
 **   
 ** @version	0.7.3
 **/
HeuristicsMap::PlayerTypeGroup
HeuristicsMap::PlayerTypeGroup_from_name(string const& name)
{
  for (int g = PLAYERTYPE_GROUP_FIRST; g <= PLAYERTYPE_GROUP_LAST; ++g)
    if (name == ::name(static_cast<PlayerTypeGroup>(g)))
	return static_cast<PlayerTypeGroup>(g);

  DEBUG_ASSERTION(false,
		  "HeuristicsMap::PlayerTypeGroup_from_name(\"" << name << "\")\n"
		  "  did not find playertype group '" << name << "'");
  return PLAYERTYPE_GROUP_FIRST;
} // HeuristicsMap::PlayerTypeGroup HeuristicsMap::PlayerTypeGroup_from_name(string const& name)

/**
 ** -> result
 **
 ** @param	group   group
 **
 ** @return	the name of the group
 **
 ** @author	Diether Knof
 **   
 ** @version	0.7.3
 **/
string
name(HeuristicsMap::PlayerTypeGroup group)
{
  switch (group) {
  case HeuristicsMap::CONTRA:
    return "contra";
  case HeuristicsMap::RE:
    return "re";
  case HeuristicsMap::SPECIAL:
    return "special";
  } // switch (group)

  return "";
} // string name(HeuristicsMap::PlayerTypeGroup group)
