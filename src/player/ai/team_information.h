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

#ifndef CLASS_TEAM_INFORMATION_HEADER
#define CLASS_TEAM_INFORMATION_HEADER

#include "../../basistypes.h"
#include "../../card/card.h"

class HandCard;
class Player;
class Ai;
class Game;
class Trick;

#ifndef RELEASE
// card of interest, for debugging
#define COI Card(Card::CLUB, Card::ACE)
#endif

/**
 ** Contains and analyses the information, which team a player has
 ** Remark: cannot be an 'OS' because then the functions are not called in
 **         virtual games
 **
 ** @author    Diether Knof
 **/
class TeamInformation {
  friend bool operator==(TeamInformation const& team_information_a,
			  TeamInformation const& team_information_b);

  public:
  TeamInformation() = delete;
  TeamInformation(Ai const& player);
  TeamInformation(TeamInformation const& team_information) = default;
  TeamInformation& operator=(TeamInformation const& team_information) = default;
  ~TeamInformation();

  // writes the information in 'ostr'
  void write(ostream& ostr) const;

  // the corresponding player
  Ai const& player() const;

  // values for the team: positive -> re, negative -> contra
  vector<int> const& team_values() const;
  // the team (including maybe)
  vector<Team> const& teams() const;
  // the number of known teams (including the own)
  unsigned known_teams_no() const;

  // set the player
  void set_player(Ai const& player);

  // the corresponding game
  Game const& game() const;


  // the team value of the player
  int team_value(Player const& player) const;
  // the team value of the player
  int team_value(unsigned const playerno) const;
  // the team of the player
  Team const& team(Player const& player) const;
  // the team of the player
  Team const& team(unsigned const playerno) const;
  // the team of the player
  Team& team(Player const& player);
  // the team of the player
  Team& team(unsigned const playerno);
  // the guessed team of the player
  Team guessed_team(Player const& player) const;
  // the guessed team of the player
  Team guessed_team(unsigned const playerno) const;
  // the guessed partner
  Player const* guessed_partner() const;

  // all teams
  vector<Team> guessed_teams() const;
  // set the teams
  void set_teams(vector<Team> const& teams);

  // whether all teams are known
  bool all_known() const;
  // the certainty of the team decision
  int certainty() const;

  // the information from the game

  // the game starts
  void game_start();
  // the card is played
  void card_played(HandCard const& card);
  // an announcement is made
  void announcement_made(Announcement const& announcement,
			 Player const& player);
  // the trick is full
  void trick_full(Trick const& trick);
  // the marriage is determined
  void marriage(Player const& bridegroom, Player const& bride);
  // a player has announced a genscher
  void genscher(Player const& genscher, Player const& partner);

  // reset all values
  void reset();
  public:
  // updates the teams of the players according to the team values
  void update_teams();
  // recalcs the values from the beginning of the game
  void recalc_team_values();

  private:
  // the corresponding player
  Ai const* player_ = nullptr;

  // values for the team: positive -> re, negative -> contra
  vector<int> team_values_;
  // the team (including maybe)
  vector<Team> teams_;
  // the number of known teams (including the own)
  unsigned known_teams_no_ = 0;

  bool in_recalcing_ = false;
}; // class TeamInformation

ostream& operator<<(ostream& ostr, TeamInformation const& team_information);

bool operator==(TeamInformation const& team_information_a,
		TeamInformation const& team_information_b);
bool operator!=(TeamInformation const& team_information_a,
		TeamInformation const& team_information_b);

#endif // #ifndef CLASS_TEAM_INFORMATION_HEADER
