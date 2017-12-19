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

#ifndef CLASS_SPECIALPOINTS_HEADER
#define CLASS_SPECIALPOINTS_HEADER

#include "../basistypes.h"
#include "../misc/translation.h"
class Game;

//////////////////
// SPECIALPOINT //
//////////////////

// a structure for the management of specialpoints in tricks, gamepoints and
// gamepointstable

namespace SPECIALPOINT {
  enum Specialpoint {
    FIRST,
    NOSPECIALPOINT = FIRST,
    CAUGHT_FOX,
    CAUGHT_FOX_LAST_TRICK,
    FOX_LAST_TRICK,
    //DOUBLE_FOX_LAST_TRICK,
    CHARLIE,
    CAUGHT_CHARLIE,
    //DOUBLE_CHARLIE,
    //CAUGHT_DOUBLE_CHARLIE,
    DOLLE_CAUGHT_DOLLE,
    HEART_TRICK,
    DOPPELKOPF,
    WON,
    NO90,
    NO60,
    NO30,
    NO0,
    NO120_SAID,
    NO90_SAID,
    NO60_SAID,
    NO30_SAID,
    NO0_SAID,
    NO90_SAID_120_GOT,
    NO60_SAID_90_GOT,
    NO30_SAID_60_GOT,
    NO0_SAID_30_GOT,
    NO120_REPLY,
    NO90_REPLY,
    NO60_REPLY,
    NO30_REPLY,
    NO0_REPLY,
    CONTRA_WON,
    SOLO,
    BOCK,
    LAST=BOCK
  }; // enum Specialpoint

  bool is_winning(Specialpoint const sp);
  bool is_announcement(Specialpoint const sp);
} // namespace SPECIALPOINT

using SPECIALPOINT::Specialpoint;

int Value_of_Specialpoint(Specialpoint const sp);
string name(Specialpoint const specialpoint);
Specialpoint specialpoint_from_name(string const& name) throw(ReadException);
ostream& operator<<(ostream& ostr, Specialpoint const& specialpoint);


class Specialpoints {
#ifdef POSTPONED
  enum Specialpoint {
    // take from above
  };
#endif

  public:
  Specialpoints(Specialpoint const& type,
		Team const& team);
  Specialpoints(Specialpoint const& type,
		Team const& team,
		Team const& counting_team);
  Specialpoints(Specialpoint const& type,
                int const value);
  Specialpoints(istream& istr);

  bool is_valid(vector<Team> const team) const;

  public:
  Specialpoint type;
  Team team; // team which has made the point
  Team counting_team; // team, which gets the point
  // the player who get the special point
  // (i.e. the player who has caught a fox)
  unsigned player_get_no;
  // the player who has 'lost' the special point
  // (i.e. the player whose fox has been caught)
  unsigned player_of_no;
  // the value of the specialpoint
  PRIV_VAR_R(int, value);
  

}; // class Specialpoints

typedef vector<Specialpoints> Specialpointsvector;

ostream& operator<<(ostream& ostr, Specialpoints const& specialpoint);
ostream& operator<<(ostream& ostr,
		    Specialpointsvector const& specialpointsvector);

#if 0
// sets the counting team
Specialpoints& counting_team_set(Specialpoints& sp, Team const winner);
#endif

int Sum_of_Specialpoints(Specialpointsvector const& spv,
			 Team const& winner,
			 Game const& g);

inline
Translator::Translation translation(Specialpoint const& special_point)
{ return Translator::Translation(::translator,
				 "SpecialPoint::"
				 + ::name(special_point)); }
Translator::Translation translation(Specialpoints const& special_points);

#endif// #ifndef CLASS_SPECIALPOINTS_HEADER
