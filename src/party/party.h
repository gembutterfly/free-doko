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

#ifndef CLASS_PARTY_HEADER
#define CLASS_PARTY_HEADER

#include "../basistypes.h"
#include "../exception.h"

#include "../player/player.h"
class Rule;
class GamePointsTable;
class GameSummary;
class Human;

class Party;
extern Party party;

class Party {
  friend class Game;
  public:

  // constructor
  Party();
  // destructor
  ~Party();

  // number of players
  unsigned playerno() const;
  // all players
  vector<Player*>& players();
  // a player
  Player& player(unsigned const p);
  Player const& player(unsigned const p) const;
  // the position of a player
  unsigned no(Player const& player) const;
  // returns the player with the name
  Player* player_by_name(string const& name);
  Player const* player_by_name(string const& name) const;

  // number of human players
  unsigned humanno() const;
  // the human player (if there is exactly one)
  Human* human_player();
  Human const* human_player() const;

  // set a player
  void set_player(unsigned const n, Player* player);
  // add a player
  void add_player(Player* player);
  // remove all but the first four players
  void remove_spare_players();
  // replace a player
  Player* replace_player(unsigned const n, Player* player);
  // switch two players
  void switch_players(unsigned const n0, unsigned const n1);
  void set_playername(unsigned const n, string const& new_name);
  void set_playervoice(unsigned const n, string const& voice);
  void set_playertype(unsigned const n, Player::Type const new_type);

  unsigned seed() const;
  void set_seed(unsigned const seed);
  void set_random_seed();
  unsigned seed_next();

  unsigned startplayer() const;
  void set_startplayer(unsigned const startplayer);
  void set_random_startplayer();

  // whether the party can be configured (seed, startplayer)
  bool in_configuration() const;

  unsigned roundno() const;
  unsigned remaining_rounds() const;
  unsigned remaining_normal_games() const;
  unsigned remaining_games() const;
  unsigned gameno() const;
  unsigned finished_games() const;
  bool is_duty_soli_round() const;
  bool is_last_game() const;
  bool is_finished() const;

  // the remaining points for the party
  int remaining_points() const;

  bool starts_new_round(unsigned const gameno) const;
  unsigned round_of_game(unsigned const gameno) const;
  unsigned round_startgame(unsigned const roundno) const;

  void add_game_summary(GameSummary* const game_summary);
  GameSummary const& last_game_summary() const;
  GameSummary const& game_summary(unsigned const gameno) const;

  // the bock multiplier for the current game
  unsigned current_bock_multiplier() const;
  // the bock multiplier for the game gameno
  unsigned bock_multiplier(unsigned const gameno) const;


  int pointsum() const;
  int pointsum_till_game(unsigned const gameno) const;
  int pointsum_till_round(unsigned const roundno) const;
  int pointsum_in_round(unsigned const roundno) const;
  int pointsum(Player const& player) const;
  int pointsum(unsigned const playerno) const;
  int pointsum_till_game(unsigned const gameno, unsigned const playerno) const;
  int pointsum_till_round(unsigned const roundno, unsigned const playerno) const;
  int pointsum_in_round(unsigned const roundno, unsigned const playerno) const;

  unsigned rang(unsigned const playerno) const;
  unsigned rang(Player const& player) const;

  unsigned played_duty_soli(unsigned const playerno) const;
  unsigned played_duty_soli(Player const& player) const;
  unsigned played_duty_color_soli(unsigned const playerno) const;
  unsigned played_duty_color_soli(Player const& player) const;
  unsigned played_duty_picture_soli(unsigned const playerno) const;
  unsigned played_duty_picture_soli(Player const& player) const;
  unsigned played_duty_free_soli(unsigned const playerno) const;
  unsigned played_duty_free_soli(Player const& player) const;
  unsigned played_lust_soli(unsigned const playerno) const;
  unsigned played_lust_soli(Player const& player) const;

  unsigned remaining_duty_soli() const;
  unsigned remaining_duty_soli(unsigned const playerno) const;
  unsigned remaining_duty_soli(Player const& player) const;
  unsigned remaining_duty_color_soli(unsigned const playerno) const;
  unsigned remaining_duty_color_soli(Player const& player) const;
  unsigned remaining_duty_picture_soli(unsigned const playerno) const;
  unsigned remaining_duty_picture_soli(Player const& player) const;
  unsigned remaining_duty_free_soli(unsigned const playerno) const;
  unsigned remaining_duty_free_soli(Player const& player) const;

  void write(ostream& ostr) const;
  void write_pointstable(ostream& ostr) const;
  void write_players_short(ostream& ostr) const;
  bool read(istream& istr) throw (ReadException);

  // save the party
  bool save(string const& filename) const;
  // load the party
  bool load(string const& filename) throw (ReadException);
  // load the players
  bool load_players() throw (ReadException);


  // open the party
  void open();
  // initialize the party
  void init();
  // play the party (main program flow)
  void play();
  // close the party
  void close();

  // change of the rules
  void rule_changed(int const type, void const* const old_value);

  private:
  // save the party automatically
  bool auto_save() const;
  public:
  void remove_auto_save() const;
  // save the changes of the party (t.i. rules and players)
  void save_changes() const;

  private:
  // increment the round
  void increment_round();

  private:
  // rules for the party
  PRIV_VAR_P_RW(Rule, rule);
  // players
  PRIV_VAR_R(vector<Player*>, players);
  // starting seed (UINT_MAX for random)
  PRIV_VAR_R(unsigned, seed_first);
  // starting startplayer
  PRIV_VAR_R(unsigned, startplayer_first);
  // real starting seed (can not be random)
  PRIV_VAR_R(unsigned, real_seed_first);
  // real startplayer (can not be random)
  PRIV_VAR_R(unsigned, real_startplayer_first);
  // the game in which the round starts
  PRIV_VAR_R(vector<unsigned>, round_startgame);

  // the number of the duty soli round
  PRIV_VAR_R(unsigned, duty_soli_round);

  // the points for the party
  PRIV_VAR_R(int, points);
  // current game
  PRIV_VAR_P_RW(Game, game);
  // the current seed
  unsigned seed_;
  // the current startplayer
  unsigned startplayer_;

  // game summaries
  PRIV_VAR_R(vector<GameSummary*>, game_summaries);
  // the multipliers for bock games
  PRIV_VAR_R(vector<unsigned>, bock_multipliers);

  // set_ai_players
  // partie typ (tutorial...)
  // pflichtstsoli
  // punkte topf
  // tunier status

  private: // not to be used
  Party(Party const&);
  Party& operator=(Party const&);
}; // class Party

ostream& operator<<(ostream& ostr, Party const& p);

#endif // #ifdef CLASS_PARTY_HEADER
