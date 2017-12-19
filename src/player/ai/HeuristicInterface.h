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

#ifndef CLASS_HEURISTICINTERFACE_HEADER
#define CLASS_HEURISTICINTERFACE_HEADER

#include "../aiconfig.h"
#include "../../basistypes.h"

#include "../player.h"
#include "../../game/game.h"
#include "../../card/trick.h"

class Hand;
class CardsInformation;
class TeamInformation;

/// reine Virtuelle Klasse die f�r Heuristics implementiert werden muss
class HeuristicInterface {
  public:	
    HeuristicInterface() { }
    virtual ~HeuristicInterface() { }

    virtual CardsInformation const& cards_information() const = 0;
    virtual TeamInformation const& team_information() const = 0;

    virtual Player const& player() const = 0;
    bool same_player(Player const& player) const
    { return (this->player() == player); }
    bool is_soloplayer() const
    { return this->game().is_soloplayer(this->player()); }
    bool is_winnerplayer(Trick const& trick) const
    { return (!trick.isempty() && this->same_player(trick.winnerplayer())); }
    virtual Hand const& handofplayer( Player const& player )    const = 0;
    Hand const& handofplayer( unsigned const playerno ) const 
    { return this->handofplayer( this->game().player(playerno) ); }
    virtual Team teamofplayer( Player const& player )    const = 0;
    Team teamofplayer( unsigned const playerno ) const 
    { return this->teamofplayer( this->game().player(playerno) ); }
    Team guessedteamofplayer( Player const& player )    const
    { return TEAM::maybe_to_team(this->teamofplayer(player)); }
    Team guessedteamofplayer( unsigned const playerno )    const
    { return TEAM::maybe_to_team(this->teamofplayer(playerno)); }
    bool same_team(Player const& player) const
    { return (   !this->game().marriage_selector()
              && (this->teamofplayer(player) == this->team())); }
    bool same_team(unsigned const playerno) const
    { return (this->same_team(this->game().player(playerno))); }
    bool opposite_team(Player const& player) const
    { return (   !this->game().marriage_selector()
              && (this->teamofplayer(player) == opposite(this->team()))); }
    bool opposite_team(unsigned const playerno) const
    { return (this->opposite_team(this->game().player(playerno))); }
    bool guessed_same_team(Player const& player) const
    { return (   !this->game().marriage_selector()
              && (this->guessedteamofplayer(player) == this->team())); }
    bool guessed_same_team(unsigned const playerno) const
    { return (this->guessed_same_team(this->game().player(playerno))); }
    bool guessed_opposite_team(Player const& player) const
    { return (   !this->game().marriage_selector()
              && (this->guessedteamofplayer(player) == opposite(this->team()))); }
    bool guessed_opposite_team(unsigned const playerno) const
    { return (this->guessed_opposite_team(this->game().player(playerno))); }



    virtual Hand const& hand() const = 0;
    virtual Team team() const = 0;
    virtual Game const& game() const = 0;

    virtual bool value(const Aiconfig::TypeBool& type) const = 0;
    virtual int value(const Aiconfig::TypeInt& type) const = 0;
    virtual Card value(const Aiconfig::TypeCard& type) const = 0;

    virtual bool colorjabbed(const Card::TColor c) const = 0;

    virtual unsigned color_runs(const Card::TColor c) const = 0;

    virtual bool jabbedbyownteam(const Card::TColor c) const = 0;
    virtual bool jabbedbyotherteam(const Card::TColor c) const = 0;

    virtual bool next_low() const = 0;

    virtual Card trump_card_limit() const = 0;
    virtual Card lowest_trump_card_limit() const = 0;
}; // class HeuristicInterface

#endif // #ifndef CLASS_HEURISTICINTERFACE_HEADER
