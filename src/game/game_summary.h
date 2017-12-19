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

#ifndef CLASS_GAME_SUMMARY_HEADER
#define CLASS_GAME_SUMMARY_HEADER

#include "../basistypes.h"
#include "specialpoint.h"
#include "exception.h"
class Game;
class Rule;
class Player;

/**
 ** The summary of a game:
 ** - seed
 ** - startplayer
 ** - gametype
 ** - gamepoints
 **
 ** @author	Diether Knof
 **
 **/
class GameSummary {
  public:

    GameSummary(Game const& game);
    GameSummary(Rule const& rule, istream& istr);
    GameSummary(GameSummary const& game_summary);
    ~GameSummary();

    void write(ostream& ostr) const;
    void read(istream& istr);

    // the number of the (game) player in the party
    unsigned party_player(unsigned const p) const;
    // the number of the (party) player in the game summary
    unsigned game_player(unsigned const p) const;
    // whether the player has played
    bool party_player_has_played(unsigned const player_no) const;

    // whether the game is a solo
    bool is_solo() const;
    // whether the game is a duty solo
    bool is_duty_solo() const;
    // whether the game is a lust solo
    bool is_lust_solo() const;
    // whether the startplayer is not incremented
    bool startplayer_stays() const;
    // the number of the next startplayer
    unsigned next_startplayer_no() const;

    // the points the player has made with tricks
    unsigned trick_points(unsigned const playerno) const;
    unsigned trick_points(Player const& player) const;
    unsigned trick_points(Team const team) const;

    // the team the points are for
    Team team_of_points() const;
    // whether the team gets points
    bool team_get_points(Team const team) const;
    // the points the team gets
    int points(Team const team) const;

    // whether the player gets points
    bool player_get_points(unsigned const playerno) const;
    bool player_get_points(Player const& player) const;
    // the points the player gets
    int points(unsigned const playerno) const;
    // the points the player gets
    int points(Player const& player) const;

    // the team of the player
    Team team(unsigned const playerno) const;
    Team team(Player const& player) const;

    // the i'th special point
    Specialpoints const& specialpoint(unsigned const i) const;

    // the bock multiplier
    int bock_multiplier() const;

  private:
    // evaluate the game to get the summary
    void evaluate(Game const& game);

  private:
    // the corresponding rules
    PRIV_VAR_CP_R(Rule, rule);
    // the seed
    PRIV_VAR_R(unsigned, seed);
    // the number of players
    PRIV_VAR_R(unsigned, players_number);
    // the numbers of the players (from the party)
    PRIV_VAR_R(vector<unsigned>, party_players);
    // the number of the startplayer
    PRIV_VAR_R(unsigned, startplayer_no);
    // the number of the soloplayer (UINT_MAX if none)
    PRIV_VAR_R(unsigned, soloplayer_no);

    // the type of the game
    PRIV_VAR_R(GameType, game_type);
    // the type of the game
    PRIV_VAR_R(bool, duty_solo);
    // the player who had swines (UINT_MAX if none)
    PRIV_VAR_R(unsigned, swines_player_no);
    // the player who had hyperswines (UINT_MAX if none)
    PRIV_VAR_R(unsigned, hyperswines_player_no);
    // the teams of the players
    PRIV_VAR_R(vector<Team>, teams);

    // the highest announcement made by the re team
    PRIV_VAR_R(Announcement, highest_announcement_re);
    // the highest announcement made by the contr team
    PRIV_VAR_R(Announcement, highest_announcement_contra);

    // the points made by the players (with tricks)
    PRIV_VAR_R(vector<unsigned>, trick_points);
    // the winnerteam
    PRIV_VAR_R(Team, winnerteam);
    // the points of the game
    PRIV_VAR_R(int, points);

    // the special points
    PRIV_VAR_R(Specialpointsvector, specialpoints);
    // the bock triggers
    PRIV_VAR_R(vector<BockTrigger>, bock_triggers);

  private: // unused
    GameSummary();
    GameSummary& operator=(GameSummary const& game_summary);
}; // class GameSummary

// write the game summary into the output stream
ostream& operator<<(ostream& ostr, GameSummary const& game_summary);

#endif // #ifndef CLASS_GAMEPOINTSTABLE_HEADER
