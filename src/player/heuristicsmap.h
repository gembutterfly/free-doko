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

#ifndef CLASS_HEURISTICS_MAP_HEADER
#define CLASS_HEURISTICS_MAP_HEADER

#include "../misc/translation.h"

class Game;
class Player;

namespace HeuristicsMap {
  enum GametypeGroup {
    GAMETYPE_GROUP_FIRST,

    DEFAULT = GAMETYPE_GROUP_FIRST,
    NORMAL,
    POVERTY,
    MARRIAGE_UNDETERMINED,
    MARRIAGE_SILENT,
    SOLO_MEATLESS,
    SOLI_COLOR,
    SOLI_SINGLE_PICTURE,
    SOLI_DOUBLE_PICTURE,
    SOLO_KOEHLER,

    GAMETYPE_GROUP_LAST = SOLO_KOEHLER
  }; // enum GametypeGroup
  GametypeGroup group(Game const& game);
  GametypeGroup GametypeGroup_from_name(string const& name);

  enum PlayerTypeGroup {
    PLAYERTYPE_GROUP_FIRST,

    SPECIAL = PLAYERTYPE_GROUP_FIRST,
    RE,
    CONTRA,

    PLAYERTYPE_GROUP_LAST = CONTRA
  }; // enum PlayerTypeGroup
  PlayerTypeGroup group(Player const& player);
  PlayerTypeGroup PlayerTypeGroup_from_name(string const& name);

  class Key {
    public:
      static Key const DEFAULT;

    public:
      Key(Player const& player);
      Key(GametypeGroup const gametype_group,
	  PlayerTypeGroup const playertype_group);

      GametypeGroup gametype_group;
      PlayerTypeGroup playertype_group;
  }; // class Key

  bool operator==(Key const& key_a, Key const& key_b);
  bool operator!=(Key const& key_a, Key const& key_b);
  bool operator<(Key const& key_a, Key const& key_b);

} // namespace HeuristicsMap

string name(HeuristicsMap::GametypeGroup const group);
string name(HeuristicsMap::PlayerTypeGroup const group);
WRITE_NAME(HeuristicsMap::GametypeGroup)
WRITE_NAME(HeuristicsMap::PlayerTypeGroup)

inline
Translator::Translation translation(HeuristicsMap::GametypeGroup const&
				    gametype_group)
{ return Translator::Translation(::translator,
				 "AiConfig::HeuristicsMap::GametypeGroup::"
				 + ::name(gametype_group)); }
inline
Translator::Translation translation(HeuristicsMap::PlayerTypeGroup const&
				    playertype_group)
{ return Translator::Translation(::translator,
				 "AiConfig::HeuristicsMap::PlayerTypeGroup::"
				 + ::name(playertype_group)); }

#endif // #ifndef CLASS_HEURISTICS_MAP_HEADER
