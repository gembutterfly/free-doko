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

#ifndef CLASS_TEAM_INFORMATION_HEURISTICS_HEADER
#define CLASS_TEAM_INFORMATION_HEURISTICS_HEADER

#include "../../basistypes.h"

class Player;
class HandCard;
class Trick;
class Ai;

/**
 ** heuristics for the team information
 ** CardsInformations cannot be used, since the teaminfo is updated before the cards information and hence CardsInformation::self_check() will find errors (one card lacks to be played)
 **
 ** @author	Diether Knof
 **
 ** @todo       make use of CardsInformation possible
 **/
namespace TeamInformationHeuristic {
  int card_played(HandCard const& played_card,
                  Trick const& trick,
                  Ai const& ai);

} // namespace TeamInformationHeuristic 

#endif // #ifndef CLASS_TEAM_INFORMATION_HEURISTICS_HEADER
