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
 **********************************************************************/


#ifndef BASISTYPES_HEADER
#define BASISTYPES_HEADER

#include "exception.h"

// some enumerate:
// - GameStatus
// - Team
// - GameType
// - Announcement
// - MarriageSelector
// - Rotation
// - Position
// - Information

#include "constants.h"


////////////////
// GAMESTATUS //
////////////////

namespace GAMESTATUS {
int const PARTY = 0x10;
int const GAME = 0x100;
enum GameStatus {
  PROGRAMSTART = 0,
  QUIT = 1,
  PARTY_NEW = PARTY,
  PARTY_INITIAL_LOADED,
  PARTY_LOADED,
  PARTY_PLAY,
  PARTY_FINISHED,
  GAME_NEW = GAME,
  GAME_INIT,
  GAME_RESERVATION,
  GAME_REDISTRIBUTE,
  GAME_POVERTY_SHIFT,
  GAME_START,
  GAME_PLAY,
  GAME_FULL_TRICK,
  GAME_TRICK_TAKEN,
  GAME_FINISHED,
  GAME_CLOSED
}; // enum GameStatus
} // namespace GAMESTATUS
using GAMESTATUS::GameStatus;

bool in_running_game();

extern GameStatus game_status;

string name(GameStatus const& game_status);
WRITE_NAME(GameStatus);


//////////
// Team //
//////////

namespace TEAM {
enum Team {
  NOTEAM,
  RE,
  CONTRA, 
  UNKNOWN,
  MAYBE_RE,   // Team for AI-Heuristics
  MAYBE_CONTRA // Team for AI-Heuristics
}; // enum Team

Team from_name(string const& name) throw (ReadException);

/// converts team MAYBE_X to X
Team maybe_to_team(Team const& t);
/// converts team X to MAYBE_X
Team team_to_maybe(Team const& t);
Team opposite(Team const& t);
// whether the team is 'RE' or 'CONTRA'
bool is_real(Team const& t);
} // namespace TEAM
using TEAM::Team;

using TEAM::maybe_to_team;
using TEAM::team_to_maybe;
using TEAM::opposite;
using TEAM::is_real;

string name(Team const& team);
string short_name(Team const& team);
WRITE_NAME(Team);



//////////////
// GAMETYPE //
//////////////

namespace GAMETYPE {
enum GameType {
  FIRST,
  NORMAL = FIRST, 
  THROWN_NINES,
  THROWN_KINGS,
  THROWN_NINES_AND_KINGS,
  THROWN_RICHNESS,
  FOX_HIGHEST_TRUMP,
  POVERTY,
  GENSCHER,
  MARRIAGE,
  MARRIAGE_SOLO,
  MARRIAGE_SILENT,
  SOLO_MEATLESS,
  SOLO_JACK,
  SOLO_QUEEN,
  SOLO_KING,
  SOLO_QUEEN_JACK,
  SOLO_KING_JACK,
  SOLO_KING_QUEEN,
  SOLO_KOEHLER,
  SOLO_CLUB,
  SOLO_HEART,
  SOLO_SPADE,
  SOLO_DIAMOND,
  LAST = SOLO_DIAMOND
}; // enum GameType
unsigned const NUMBER = LAST - FIRST + 1;
bool is_with_unknown_teams(GameType const game_type);
bool is_normal(GameType const game_type);
bool is_poverty(GameType const game_type);
bool is_marriage(GameType const game_type);
bool is_solo(GameType const game_type);
bool is_real_solo(GameType const game_type);
bool is_color_solo(GameType const game_type);
bool is_picture_solo(GameType const game_type);
bool is_single_picture_solo(GameType const game_type);
bool is_double_picture_solo(GameType const game_type);

GameType from_name(string const& name) throw (ReadException);
} // namespace GAMETYPE
using GAMETYPE::GameType;

string name(GameType const& game_type);
WRITE_NAME(GameType);



//////////////////
// ANNOUNCEMENT //
//////////////////

namespace ANNOUNCEMENT {
enum Announcement{
  FIRST = 0,
  NOANNOUNCEMENT = FIRST,
  NO120, // means RE or CONTRA
  NO90,
  NO60,
  NO30,
  NO0,
  // a reply or a RE/CONTRA as reply to another announcement (no further announcement can be made)
  // this is only announced, when the rule 'ANNOUNCEMENT_STRICT_LIMIT' is inactive
  REPLY,
  // these are only announced, when the rule 'ANNOUNCEMENT_STRICT_LIMIT' is active
  NO120_REPLY,
  NO90_REPLY,
  NO60_REPLY,
  NO30_REPLY,
  NO0_REPLY,
  LAST = NO0_REPLY
}; // enum Announcement
unsigned const NUMBER = NO0_REPLY - NOANNOUNCEMENT + 1;

// the announcement with name 'name'
Announcement from_name(string const& name) throw (ReadException);;

// whether the announcement is a normal one
bool is_real(Announcement const& announcement);
// whether the announcement is a reply
bool is_reply(Announcement const& announcement);
// convert the announcement to a reply announcement
Announcement to_reply(Announcement const& announcement);
// convert the announcement to a normal announcement
Announcement from_reply(Announcement const& announcement);
// the next lower announcement
Announcement previous(Announcement const& announcement);
// the next higher announcement
Announcement next(Announcement const& announcement);

// the points needed for the announcement
unsigned needed_points(Announcement const& announcement);
// the points needed for the opposite team to win
unsigned point_limit_for_opposite_team(Announcement const& announcement);
} // namespace ANNOUNCEMENT
using ANNOUNCEMENT::Announcement;

// the name of the announcement
string name(Announcement const& announcement);
WRITE_NAME(Announcement);



///////////////////////
// MARRIAGE_SELECTOR //
///////////////////////

namespace MARRIAGE_SELECTOR {
enum MarriageSelector {
  FIRST,
  TEAM_SET = FIRST,
  FIRST_FOREIGN,
  FIRST_TRUMP,
  FIRST_COLOR,
  FIRST_CLUB,
  FIRST_SPADE,
  FIRST_HEART,
  SILENT,
  LAST = SILENT
}; // enum MarriageSelector
unsigned const MARRIAGE_SELECTOR_NUMBER = 1 + 1 + 1 + 3 + 1;
MarriageSelector from_name(string const& name);
} // namespace MARRIAGE_SELECTOR
using MARRIAGE_SELECTOR::MarriageSelector;

string name(MarriageSelector const& marriage_selector);
WRITE_NAME(MarriageSelector);


namespace BOCK_TRIGGER {
enum BockTrigger {
  FIRST,
  // bock after equal points between re and contra
  EQUAL_POINTS = FIRST,
  // bock after a lost solo game
  SOLO_LOST,
  // bock after a lost re game (only re is announced)
  RE_LOST,
  // bock after a lost contra game (only contra is announced)
  CONTRA_LOST,
  // bock after a real heart trick
  HEART_TRICK,
  // bock after a black game (a team got no trick)
  BLACK,
  LAST = BLACK
}; // enum BockTrigger
BockTrigger from_name(string const& name);
} // namespace BOCK_TRIGGER
using BOCK_TRIGGER::BockTrigger;

string name(BockTrigger const& bock_trigger);
WRITE_NAME(BockTrigger);

enum class Rotation {
  UP,
  DOWN,
  LEFT,
  RIGHT
}; // enum ROTATION
unsigned const ROTATION_NUMBER = 4;
Rotation clockwise(Rotation rotation);
Rotation counterclockwise(Rotation rotation);

enum Position {
  NORTH,
  SOUTH,
  EAST,
  WEST,
  CENTER
}; // enum class Position

namespace INFORMATION {
enum Type {
  NORMAL,
  WARNING,
  PROBLEM
};
} // namespace INFORMATION 

#endif // #ifndef BASISTYPES_HEADER
