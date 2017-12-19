/**********************************************************************
 *
 *   freedoko a Doppelkopf-Game
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

#include "basistypes.h"


////////////////
// GAMESTATUS //
////////////////

// status of the current game
GameStatus game_status = GAMESTATUS::PROGRAMSTART;


/**
 ** -> result
 **
 ** @param	game_status	game status
 **
 ** @return	name of 'game_status'
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
string
name(GameStatus const& game_status)
{
  using namespace GAMESTATUS;

  switch(game_status) {
  case PROGRAMSTART:
    return "program start";
  case QUIT:
    return "quit";
  case PARTY_NEW:
    return "party new";
  case PARTY_INITIAL_LOADED:
    return "party initial loaded";
  case PARTY_LOADED:
    return "party loaded";
  case PARTY_PLAY:
    return "party play";
  case PARTY_FINISHED:
    return "party finished";
  case GAME_NEW:
    return "game new";
  case GAME_INIT:
    return "game init";
  case GAME_RESERVATION:
    return "game reservation";
  case GAME_REDISTRIBUTE:
    return "game redistribute";
  case GAME_POVERTY_SHIFT:
    return "game poverty shift";
  case GAME_START:
    return "game start";
  case GAME_PLAY:
    return "game play";
  case GAME_FULL_TRICK:
    return "game full trick";
  case GAME_TRICK_TAKEN:
    return "game trick taken";
  case GAME_FINISHED:
    return "game finished";
  case GAME_CLOSED:
    return "game closed";
  } // switch(game_status)

  DEBUG_ASSERTION(false,
		  "::name(GameStatus):\n"
		  "  game status not found: " << static_cast<int>(game_status));

  return "";
} // string name(GameStatus game_status)


//////////
// Team //
//////////

/**
 ** converts a maybe team to a real team
 **
 ** @param	team	team to convert
 **
 ** @result	RE for 'team == RE' or 'team == MAYBE_RE',
 **		CONTRA for 'team == CONTRA' or 'team == MAYBE_CONTRA'
 **		UNKNOWN otherwise
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
Team
TEAM::maybe_to_team(Team const& team)
{
  switch(team) {
  case RE:
  case MAYBE_RE:
    return RE;
  case CONTRA:
  case MAYBE_CONTRA:
    return CONTRA;
  default:
    return UNKNOWN;
  } // switch(team)
} // Team TEAM::maybe_to_team(Team team)

/**
 ** converts a real team to a maybe team
 **
 ** @param	team	team to convert
 **
 ** @result	MAYBE_RE for 'team == RE' or 'team == MAYBE_RE',
 **		MAYBE_CONTRA for 'team == CONTRA' or 'team == MAYBE_CONTRA'
 **		UNKNOWN otherwise
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
Team
TEAM::team_to_maybe(Team const& team)
{
  switch(team) {
  case RE:
  case MAYBE_RE:
    return MAYBE_RE;
  case CONTRA:
  case MAYBE_CONTRA:
    return MAYBE_CONTRA;
  default:
    return UNKNOWN;
  } // switch(team)
} // Team TEAM::team_to_maybe(Team team)

/**
 ** -> result
 **
 ** @param	team	team to flip
 **
 ** @result	CONTRA for 'team == RE',
 **		MAYBE_CONTRA for 'team == MAYBE_RE',
 **		RE for 'team == CONTRA'
 **		MAYBE_RE for 'team == MAYBE_CONTRA'
 **		UNKNOWN otherwise
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **
 **/
Team
TEAM::opposite(Team const& team)
{
  switch(team) {
  case RE:
    return CONTRA;
  case MAYBE_RE:
    return MAYBE_CONTRA;
  case CONTRA:
    return RE;
  case MAYBE_CONTRA:
    return MAYBE_RE;
  default:
    return UNKNOWN;
  } // switch(team)
} // Team TEAM::opposite(Team team)

/**
 ** -> result
 **
 ** @param	team	team to flip
 **
 ** @result	whether the team is real (that means 'RE' or 'CONTRA')
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
bool
TEAM::is_real(Team const& team)
{
  return (   (team == RE)
	  || (team == CONTRA));
} // bool TEAM::is_real(Team team)

/**
 ** -> result
 **
 ** @param	team	team
 **
 ** @return	name of the team
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
string
name(Team const& team)
{
  using namespace TEAM;

  switch(team) {
  case RE:
    return "re";
  case CONTRA:
    return "contra";
  case MAYBE_RE:
    return "maybe re";
  case MAYBE_CONTRA:
    return "maybe contra";
  case UNKNOWN:
    return "unknown team";
  case NOTEAM:
    return "no team";
  } // switch(team)

  DEBUG_ASSERTION(false,
		  "::name(Team):\n"
		  "  team not found: " << static_cast<int>(team));

  return "";
} // string name(Team team)

/**
 ** -> result
 **
 ** @param	team	team
 **
 ** @return	name of the team
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
string
short_name(Team const& team)
{
  using namespace TEAM;

  switch(team) {
  case RE:
    return "re";
  case CONTRA:
    return "contra";
  case MAYBE_RE:
    return "?re?";
  case MAYBE_CONTRA:
    return "?contra?";
  case UNKNOWN:
    return "?";
  case NOTEAM:
    return "-";
  } // switch(team)

  DEBUG_ASSERTION(false,
		  "::short_name(Team):\n"
		  "  team not found: " << static_cast<int>(team));

  return "";
} // string short_name(Team team)

/**
 ** -> result
 **
 ** @param	name	team name
 **
 ** @return	the team with name 'name'
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
Team
TEAM::from_name(string const& name)
  throw(ReadException)
{
  for (int t = NOTEAM; t <= MAYBE_CONTRA; ++t)
    if (name == ::name(static_cast<Team>(t)))
      return static_cast<Team>(t);

  throw ReadException("unknown team '" + name + "'");
  return NOTEAM;
} // Team TEAM::from_name(string name)


//////////////
// GAMETYPE //
//////////////

/**
 ** -> result
 **
 ** @param	game_type	game_type
 **
 ** @return	name of the game type
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
string
name(GameType const& game_type)
{
  using namespace GAMETYPE;

  switch(game_type) {
  case NORMAL:
    return "normal";
  case THROWN_NINES:
    return "thrown nines";
  case THROWN_KINGS:
    return "thrown kings";
  case THROWN_NINES_AND_KINGS:
    return "thrown nines and kings";
  case THROWN_RICHNESS:
    return "thrown richness";
  case FOX_HIGHEST_TRUMP:
    return "fox highest trump";
  case POVERTY:
    return "poverty";
  case GENSCHER:
    return "genscher";
  case MARRIAGE:
    return "marriage";
  case MARRIAGE_SOLO:
    return "marriage solo";
  case MARRIAGE_SILENT:
    return "marriage silent";
  case SOLO_JACK:
    return "solo jack";
  case SOLO_QUEEN:
    return "solo queen";
  case SOLO_KING:
    return "solo king";
  case SOLO_QUEEN_JACK:
    return "solo queen-jack";
  case SOLO_KING_JACK:
    return "solo king-jack";
  case SOLO_KING_QUEEN:
    return "solo king-queen";
  case SOLO_KOEHLER:
    return "solo koehler";
  case SOLO_CLUB:
    return "solo club";
  case SOLO_SPADE:
    return "solo spade";
  case SOLO_HEART:
    return "solo heart";
  case SOLO_DIAMOND:
    return "solo diamond";
  case SOLO_MEATLESS:
    return "solo meatless";
  } // switch(gametype)

  DEBUG_ASSERTION(false,
		  "::name(GameType):\n"
		  "  game type not found: " << static_cast<int>(game_type));

  return "";
} // string name(GameType game_type)

/**
 ** -> result
 **
 ** @param	name	gametype name
 **
 ** @return	the gametype with name 'name'
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
GameType
GAMETYPE::from_name(string const& name)
  throw(ReadException)
{
  for (int g = FIRST; g <= LAST; ++g)
    if (name == ::name(static_cast<GameType>(g)))
      return static_cast<GameType>(g);

  throw ReadException("unknown game type '" + name + "'");
  return FIRST;
} // GameType GAMETYPE::from_name(string name)

/**
 ** -> result
 **
 ** @param	-
 **
 ** @result	whether the status is in a running game
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
bool
in_running_game()
{
  switch (::game_status) {
  case GAMESTATUS::GAME_PLAY:
  case GAMESTATUS::GAME_FULL_TRICK:
  case GAMESTATUS::GAME_TRICK_TAKEN:
    return true;
  default:
    return false;
  } // switch(::game_status)
} // bool in_running_game()

/**
 ** -> return
 **
 ** @param	game_type	the game type
 **
 ** @return	whether 'game_type' is a game where the teams are not known in the beginning
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.12
 **/
bool
GAMETYPE::is_with_unknown_teams(GameType const game_type)
{
  switch (game_type) {
  case GAMETYPE::NORMAL:
  case GAMETYPE::MARRIAGE_SILENT:
  case GAMETYPE::GENSCHER:
    return true;
  default:
    return false;
  } // switch(game_type)
} // bool GAMETYPE::is_normal(GameType game_type)

/**
 ** -> return
 **
 ** @param	game_type	the game type
 **
 ** @return	whether 'game_type' is a 'normal' game
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
bool
GAMETYPE::is_normal(GameType const game_type)
{
  switch (game_type) {
  case GAMETYPE::NORMAL:
  case GAMETYPE::POVERTY:
  case GAMETYPE::MARRIAGE:
  case GAMETYPE::GENSCHER:
    return true;
  default:
    return false;
  } // switch(game_type)
} // bool GAMETYPE::is_normal(GameType game_type)

/**
 ** -> return
 **
 ** @param     game_type   the game type
 **
 ** @return    whether 'game_type' is a marriage
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.6
 **/
bool
GAMETYPE::is_marriage(GameType const game_type)
{
  switch (game_type) {
  case GAMETYPE::MARRIAGE:
  case GAMETYPE::MARRIAGE_SOLO:
  case GAMETYPE::MARRIAGE_SILENT:
    return true;
  default:
    return false;
  } // switch(game_type)
} // bool GAMETYPE::is_marriage(GameType game_type)

/**
 ** -> return
 **
 ** @param	game_type	the game type
 **
 ** @return	whether 'game_type' is a poverty
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
bool
GAMETYPE::is_poverty(GameType const game_type)
{
  switch (game_type) {
  case THROWN_NINES:
  case THROWN_KINGS:
  case THROWN_NINES_AND_KINGS:
  case FOX_HIGHEST_TRUMP:
  case POVERTY:
    return true;
  default:
    return false;
  } // switch(game_type)
} // bool GAMETYPE::is_poverty(GameType game_type)

/**
 ** -> return
 **
 ** @param	game_type	the game type
 **
 ** @return	whether 'game_type' is a real solo (t.i. no marriage solo)
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
bool
GAMETYPE::is_real_solo(GameType const game_type)
{
  return (   GAMETYPE::is_color_solo(game_type)
          || GAMETYPE::is_picture_solo(game_type)
          || (game_type == GAMETYPE::SOLO_MEATLESS) );
} // bool GAMETYPE::is_real_solo(GameType game_type)

/**
 ** -> return
 **
 ** @param	game_type	the game type
 **
 ** @return	whether 'game_type' is a solo
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
bool
GAMETYPE::is_solo(GameType const game_type)
{
  return (   is_real_solo(game_type)
          || (game_type == GAMETYPE::MARRIAGE_SOLO)
          || (game_type == GAMETYPE::MARRIAGE_SILENT) );
} // bool GAMETYPE::is_solo(GameType game_type)

/**
 ** -> return
 **
 ** @param	game_type	the game type
 **
 ** @return	whether 'game_type' is a color solo
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
bool
GAMETYPE::is_color_solo(GameType const game_type)
{
  switch (game_type) {
  case GAMETYPE::SOLO_CLUB:
  case GAMETYPE::SOLO_SPADE:
  case GAMETYPE::SOLO_HEART:
  case GAMETYPE::SOLO_DIAMOND:
    return true;
  default:
    return false;
  } // switch(game_type)
} // bool GAMETYPE::is_color_solo(GameType game_type)

/**
 ** -> return
 **
 ** @param	game_type	the game type
 **
 ** @return	whether 'game_type' is a picture solo
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
bool
GAMETYPE::is_picture_solo(GameType const game_type)
{
  return (   GAMETYPE::is_single_picture_solo(game_type)
          || GAMETYPE::is_double_picture_solo(game_type)
          || (game_type == GAMETYPE::SOLO_KOEHLER));
} // bool GAMETYPE::is_picture_solo(GameType game_type)

/**
 ** -> return
 **
 ** @param	game_type	the game type
 **
 ** @return	whether 'game_type' is a single picture solo
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
bool
GAMETYPE::is_single_picture_solo(GameType const game_type)
{
  switch (game_type) {
  case GAMETYPE::SOLO_JACK:
  case GAMETYPE::SOLO_QUEEN:
  case GAMETYPE::SOLO_KING:
    return true;
  default:
    return false;
  } // switch(game_type)
} // bool GAMETYPE::is_single_picture_solo(GameType game_type)

/**
 ** -> return
 **
 ** @param	game_type	the game type
 **
 ** @return	whether 'game_type' is a double picture solo
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
bool
GAMETYPE::is_double_picture_solo(GameType const game_type)
{
  switch (game_type) {
  case GAMETYPE::SOLO_QUEEN_JACK:
  case GAMETYPE::SOLO_KING_JACK:
  case GAMETYPE::SOLO_KING_QUEEN:
    return true;
  default:
    return false;
  } // switch(game_type)
} // bool GAMETYPE::is_double_picture_solo(GameType game_type)



//////////////////
// ANNOUNCEMENT //
//////////////////

/**
 ** -> result
 **
 ** @param	announcement	announcement
 **
 ** @result	name of the announcement
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
string
name(Announcement const& announcement)
{
  using namespace ANNOUNCEMENT;

  switch (announcement) {
  case NOANNOUNCEMENT:
    return "no announcement";
  case NO120:
    return "no 120";
  case NO90:
    return "no 90";
  case NO60:
    return "no 60";
  case NO30:
    return "no 30";
  case NO0:
    return "no 0";
  case REPLY:
    return "reply";
  case NO120_REPLY:
    return "no 120 reply";
  case NO90_REPLY:
    return "no 90 reply";
  case NO60_REPLY:
    return "no 60 reply";
  case NO30_REPLY:
    return "no 30 reply";
  case NO0_REPLY:
    return "no 0 reply";
  } // switch(announcement)

  DEBUG_ASSERTION(false,
                  "::name(Announcement):\n"
                  "  announcement not found: " << static_cast<int>(announcement)
                 );

  return "";
} // string name(Announcement announcement)

/**
 ** -> result
 **
 ** @param	name	announcement name
 **
 ** @return	the announcement with name 'name'
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
Announcement
ANNOUNCEMENT::from_name(string const& name)
throw(ReadException)
{
  for (int g = FIRST; g <= LAST; ++g)
    if (name == ::name(static_cast<Announcement>(g)))
      return static_cast<Announcement>(g);

  throw ReadException("unknown announcement '" + name + "'");
  return FIRST;
} // Announcement ANNOUNCEMENT::from_name(string name)

/**
 ** -> result
 **
 ** @param	announcement	announcement
 **
 ** @return	whether the announcement is a real announcement (no reply)
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
bool
ANNOUNCEMENT::is_real(Announcement const& announcement)
{
  using namespace ANNOUNCEMENT;

  switch(announcement) {
  case NOANNOUNCEMENT:
    return false;
  case NO120:
  case NO90:
  case NO60:
  case NO30:
  case NO0:
    return true;
  case REPLY:
  case NO120_REPLY:
  case NO90_REPLY:
  case NO60_REPLY:
  case NO30_REPLY:
  case NO0_REPLY:
    return false;
  } // switch(announcement)

  DEBUG_ASSERTION(false,
                  "ANNOUNCEMENT::is_real(Announcement):\n"
                  "  announcement not found: " << announcement);
  return false;
} // bool ANNOUNCEMENT::is_real(Announcement announcement)

/**
 ** -> result
 **
 ** @param	announcement	announcement
 **
 ** @return	whether the announcement is a reply
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
bool
ANNOUNCEMENT::is_reply(Announcement const& announcement)
{
  using namespace ANNOUNCEMENT;

  switch(announcement) {
  case NOANNOUNCEMENT:
    return false;
  case NO120:
  case NO90:
  case NO60:
  case NO30:
  case NO0:
    return false;
  case REPLY:
  case NO120_REPLY:
  case NO90_REPLY:
  case NO60_REPLY:
  case NO30_REPLY:
  case NO0_REPLY:
    return true;
  } // switch(announcement)

  DEBUG_ASSERTION(false,
                  "ANNOUNCEMENT::is_reply(Announcement):\n"
                  "  announcement not found: " << announcement);
  return false;
} // bool ANNOUNCEMENT::is_reply(Announcement announcement)

/**
 ** -> result
 **
 ** @param	announcement	announcement
 **
 ** @return	reply version of 'announcement'
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
Announcement
ANNOUNCEMENT::to_reply(Announcement const& announcement)
{
  using namespace ANNOUNCEMENT;

  switch(announcement) {
  case NOANNOUNCEMENT:
    DEBUG_ASSERTION(false,
                    "ANNOUNCEMENT::to_reply(announcement):\n"
                    "  announcement '" << ::name(announcement) << "' not valid.");
    return NOANNOUNCEMENT;
  case NO120:
    return NO120_REPLY;
  case NO90:
    return NO90_REPLY;
  case NO60:
    return NO60_REPLY;
  case NO30:
    return NO30_REPLY;
  case NO0:
    return NO0_REPLY;
  case REPLY:
  case NO120_REPLY:
  case NO90_REPLY:
  case NO60_REPLY:
  case NO30_REPLY:
  case NO0_REPLY:
    DEBUG_ASSERTION(false,
                    "ANNOUNCEMENT::to_reply(announcement):\n"
                    "  announcement '" << ::name(announcement) << "' not valid.");
    return NOANNOUNCEMENT;;
  } // switch(announcement)

  DEBUG_ASSERTION(false,
                  "ANNOUNCEMENT::to_reply(Announcement):\n"
                  "  announcement not found: " << announcement);
  return NOANNOUNCEMENT;
} // Announcement ANNOUNCEMENT::to_reply(Announcement announcement)

/**
 ** -> result
 **
 ** @param	announcement	announcement
 **
 ** @return	on which announcement this announcement was replied
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
Announcement
ANNOUNCEMENT::from_reply(Announcement const& announcement)
{
  using namespace ANNOUNCEMENT;

  switch(announcement) {
  case NOANNOUNCEMENT:
  case NO120:
  case NO90:
  case NO60:
  case NO30:
  case NO0:
  case REPLY:
    DEBUG_ASSERTION(false,
                    "ANNOUNCEMENT::from_reply(announcement):\n"
                    "  announcement '" << ::name(announcement) << "' not valid.");
    return NOANNOUNCEMENT;;
  case NO120_REPLY:
    return NO120;
  case NO90_REPLY:
    return NO90;
  case NO60_REPLY:
    return NO60;
  case NO30_REPLY:
    return NO30;
  case NO0_REPLY:
    return NO0;
  } // switch(announcement)

  DEBUG_ASSERTION(false,
                  "ANNOUNCEMENT::is_reply(Announcement):\n"
                  "  announcement not found: " << announcement);
  return NOANNOUNCEMENT;
} // Announcement ANNOUNCEMENT::from_reply(Announcement announcement)

/**
 ** -> result
 **
 ** @param	announcement	announcement
 **
 ** @return	previous announcement
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
Announcement
ANNOUNCEMENT::previous(Announcement const& announcement)
{
  DEBUG_ASSERTION(   (announcement >= NO90)
                  && (announcement <= NO0),
                  "ANNOUNCEMENT::previous(announcement)\n"
                  "  there exists no previous announcement of "
                  << announcement);

  return static_cast<Announcement>(announcement - 1);
} // Announcement ANNOUNCEMENT::previous(Announcement announcement)

/**
 ** -> result
 **
 ** @param	announcement	announcement
 **
 ** @return	next announcement
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
Announcement
ANNOUNCEMENT::next(Announcement const& announcement)
{
  DEBUG_ASSERTION(   (announcement >= NOANNOUNCEMENT)
                  && (announcement <  NO0),
                  "ANNOUNCEMENT::next(announcement)\n"
                  "  there exists no next announcement of "
                  << announcement);

  return static_cast<Announcement>(announcement + 1);
} // Announcement ANNOUNCEMENT::next(Announcement announcement)

/**
 ** -> result
 **
 ** @param	announcement	announcement
 **
 ** @return	the points needed to redeem the announcement
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
unsigned
ANNOUNCEMENT::needed_points(Announcement const& announcement)
{
  switch (announcement) {
  case NOANNOUNCEMENT:
    return 120;
  case NO120:
    return 240 - 120 + 1;
  case NO90:
    return 240 -  90 + 1;
  case NO60:
    return 240 -  60 + 1;
  case NO30:
    return 240 -  30 + 1;
  case NO0:
    return 240 -   0 + 1;
  case REPLY:
    return 120;
  case NO120_REPLY:
    return 120;
  case NO90_REPLY:
    return 90;
  case NO60_REPLY:
    return 60;
  case NO30_REPLY:
    return 30;
  case NO0_REPLY:
    return 0;
  } // switch (announcement)

  return 0;
} // unsigned ANNOUNCEMENT::needed_points(Announcement announcement)

/**
 ** -> result
 **
 ** @param	announcement	announcement
 **
 ** @return	the points needed for the opposite team to win
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.9
 **/
unsigned
ANNOUNCEMENT::point_limit_for_opposite_team(Announcement const& announcement)
{
  return 240 - needed_points(announcement) + 1;
} // unsigned ANNOUNCEMENT::point_limit_for_opposite_team(Announcement announcement)




///////////////////////
// MARRIAGE_SELECTOR //
///////////////////////


/**
 ** -> result
 **
 ** @param	marriage_selector	marriage selector
 **
 ** @return	name of 'marriage_selector'
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
string
name(MarriageSelector const& marriage_selector)
{
  using namespace MARRIAGE_SELECTOR;

  switch(marriage_selector) {
  case SILENT:
    return "silent";
  case TEAM_SET:
    return "team set";
  case FIRST_FOREIGN:
    return "first foreign";
  case FIRST_TRUMP:
    return "first trump";
  case FIRST_COLOR:
    return "first color";
  case FIRST_CLUB:
    return "first club";
  case FIRST_SPADE:
    return "first spade";
  case FIRST_HEART:
    return "first heart";
  } // switch(marriage_selector)

  DEBUG_ASSERTION(false,
                  "::name(MarriageSelector):\n"
                  "  no name found for " << static_cast<int>(marriage_selector)
                 );

  return "";
} // string name(MarriageSelector marriage_selector)

/**
 ** -> result
 **
 ** @param	name	marriage selector name
 **
 ** @return	the marriage selector with name 'name'
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
MarriageSelector
MARRIAGE_SELECTOR::from_name(string const& name)
{
  for (int g = FIRST; g <= LAST; ++g)
    if (name == ::name(static_cast<MarriageSelector>(g)))
      return static_cast<MarriageSelector>(g);

  throw ReadException("unknown marriage selector '" + name + "'");
  return FIRST;
} // MarriageSelector MARRIAGE_SELECTOR::from_name(string name)

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	the name of the bock trigger
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
string
name(BockTrigger const& bock_trigger)
{
  switch (bock_trigger) {
  case BOCK_TRIGGER::EQUAL_POINTS:
    return "equal points";
  case BOCK_TRIGGER::SOLO_LOST:
    return "solo lost";
  case BOCK_TRIGGER::RE_LOST:
    return "re lost";
  case BOCK_TRIGGER::CONTRA_LOST:
    return "contra lost";
  case BOCK_TRIGGER::HEART_TRICK:
    return "heart trick";
  case BOCK_TRIGGER::BLACK:
    return "black";
  } // switch (bock_trigger)

  DEBUG_ASSERTION(false,
                  "::name(BockTrigger):\n"
                  "  no name found for " << static_cast<int>(bock_trigger)
                 );

  return "";
} // string name(BockTrigger bock_trigger)

/**
 ** -> result
 **
 ** @param	name	bock trigger name
 **
 ** @return	the bock trigger with name 'name'
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
BockTrigger
BOCK_TRIGGER::from_name(string const& name)
{
  for (int g = FIRST; g <= LAST; ++g)
    if (name == ::name(static_cast<BockTrigger>(g)))
      return static_cast<BockTrigger>(g);

  throw ReadException("unknown bock trigger '" + name + "'");
  return FIRST;
} // BockTrigger BOCK_TRIGGER::from_name(string name)

/**
 ** -> result
 **
 ** @param	rotation	rotation
 **
 ** @return	'rotation' clockwise rotated
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
Rotation
clockwise(Rotation const rotation)
{
  switch (rotation) {
  case Rotation::UP:
    return Rotation::LEFT;
  case Rotation::DOWN:
    return Rotation::RIGHT;
  case Rotation::RIGHT:
    return Rotation::UP;
  case Rotation::LEFT:
    return Rotation::DOWN;
  } // switch(rotation)

  DEBUG_ASSERTION(false,
                  "ROTATION::conterclockwise(rotation):\n"
                  "  behind 'switch(rotation)'");

  return Rotation::UP;
} // Rotation clockwise(Rotation rotation)

/**
 ** -> result
 **
 ** @param	rotation	rotation
 **
 ** @return	'rotation' counterclockwise rotated
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
Rotation
counterclockwise(Rotation const rotation)
{
  switch (rotation) {
  case Rotation::UP:
    return Rotation::RIGHT;
  case Rotation::DOWN:
    return Rotation::LEFT;
  case Rotation::RIGHT:
    return Rotation::DOWN;
  case Rotation::LEFT:
    return Rotation::UP;
  } // switch(rotation)

  DEBUG_ASSERTION(false,
                  "ROTATION::conterclockwise(rotation):\n"
                  "  behind 'switch(rotation)'");

  return Rotation::UP;
} // Rotation counterclockwise(Rotation rotation)
