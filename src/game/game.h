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

#ifndef CLASS_GAME_HEADER
#define CLASS_GAME_HEADER

#include "reservation.h"
#include "../card/card.h"
#include "../card/hand_cards.h"
class Player;
class Human;
class Trick;
class Hand;

class Gameplay;

class Game {
  friend class Party;
  friend class GameSummary;
  friend class Ai;
  friend class WVirtualGames;
  friend class WMonteCarlo;
  friend class Gametree;
  public:
  class AnnouncementWithTrickno {
    public:
      AnnouncementWithTrickno() :
	announcement(ANNOUNCEMENT::NOANNOUNCEMENT),
      trickno(UINT_MAX)
      { }
      AnnouncementWithTrickno(Announcement const announcement,
			      unsigned const trickno) :
	announcement(announcement),
      trickno(trickno)
      { }
      operator Announcement const&() const
      { return this->announcement; }
      Announcement announcement;
      unsigned trickno;
  }; // class AnnouncementWithTrickno

  public:
  // constructor
  Game(class Party& party, Player& startplayer);
  // constructor (for virtual games)
  Game(Game const& game, vector<Player*> const& players);
  Game(Game const& game, vector<unique_ptr<Player>> const& players);
  // destructor
  ~Game();

  // checks the data
  bool self_check() const;
  // resets the game to the beginning of the first trick
  // (that is after the cards distribution and reservation)
  void reset_to_first_trick();

  // output of the game
  ostream& write(ostream& ostr) const;
  // output of the tricks
  ostream& write_tricks(ostream& ostr = cout) const;
  // output of the hands
  ostream& write_hands(ostream& ostr = cout) const;

  // whether this is a virtual game
  bool isvirtual() const;
  // whether this game is finished
  bool is_finished() const;

  // the rule of the game/party
  class Rule const& rule() const;
  // whether the second dolle goes over the first in the current trick
  bool second_dolle_over_first() const;

  // the bock multiplier
  int bock_multiplier() const;


  // the gametype
  GameType type() const;
  // set the gametype
  void set_type(GameType const type);
  // whether this is a solo
  bool is_solo() const;
  // whether this is a real solo
  bool is_real_solo() const;
  // set whether this is a duty solo
  bool set_is_duty_solo();
  // whether it is a marriage and still undetermined
  bool is_undetermined_marriage() const;

  // set the startplayer
  void set_startplayer(unsigned const startplayer);
  // whether the startplayer is not incremented
  bool startplayer_stays() const;

  // update the cards (according to the game type)
  void update_cards();


  // players

  // set the players
  void set_players(vector<Player*> const& players);
  // all players
  vector<Player*> const& players_const() const;
  private:
  // all players
  vector<Player*>& players();
  public:
  // number of players
  unsigned number_of_players() const;
  unsigned playerno() const {return this->number_of_players(); }
  // number of humen players
  unsigned number_of_humans() const;
  // the human player (if there is exactly one)
  Human* human_player();
  Human const* human_player() const;
  // the player
  Player const& player(unsigned const& p) const;
  // the player
  Player& player(unsigned const& p);
  // the number (= position) of the player
  unsigned no(Player const& player) const;
  // whether 'player' plays in the game
  bool contains(Player const& player) const;
  // the following player
  Player const& player_following(Player const& player) const;
  // the following player
  Player& player_following(Player& player);
  // the previous player
  Player const& player_previous(Player const& player) const;
  // the previous player
  Player const& player_previous(unsigned const& p) const;
  // for iterating over the players
  vector<Player*>::iterator players_begin();
  // for iterating over the players
  vector<Player*>::iterator players_end();
  // for iterating over the players
  vector<Player*>::const_iterator players_begin() const;
  // for iterating over the players
  vector<Player*>::const_iterator players_end() const;
  // sort the hand of the players
  void players_hands_sort();
  // mix the hand of the players
  void players_hands_mix();
  // the startplayer
  Player const& startplayer() const;
  // the player whose turn it is
  Player const& player_current() const;
  // the player whose turn it is
  Player& player_current();


  // whether it exists a soloplayer
  bool exists_soloplayer() const;
  // the soloplayer
  Player const& soloplayer() const;
  // the soloplayer
  Player& soloplayer();
  /// the player who takes the poverty
  Player const& poverty_partner() const;
  // whether it is the soloplayer
  bool is_soloplayer(Player const& player) const;



  // whether the player has made a reservation
  bool has_made_reservation(Player const& player) const;
  // the reservation (not for the players, because of the swines)
  Reservation const& reservation(Player const& player) const;



  // poverty


  // number of shifted cards
  unsigned poverty_cardno() const;
  // number of reshifted trumps
  unsigned poverty_trumpno_returned() const;
  // shifted cards
  HandCards const& poverty_cards() const;
  // returned cards
  HandCards const& poverty_cards_returned() const;


  // infos of the gameplay

  // number of cards in the game
  unsigned cards_no() const;
  // number of different cards in the game
  unsigned different_cards_no() const;
  // number of cards of the color
  unsigned cards_no(Card::TColor const& tcolor) const;
  // number of cards of the color
  unsigned numberof(Card::TColor const& tcolor) const;
  // number of different cards of the color
  unsigned different_cards_no(Card::TColor const& tcolor) const;
  // the number of trumps
  unsigned trumps_no() const;
  // the number of played trumps
  unsigned trumps_played_no() const;
  // the number of the :played cards
  unsigned played_cards_no() const;
  // the number of the cards remaining to play
  unsigned remaining_cards_no() const;
  // the number of the card played
  unsigned played_no(Card const& card) const;
  // the trumpcolor
  Card::Color trumpcolor() const;
  // the cards of a color
  vector<Card> const& cards(Card::TColor const& tcolor) const;
  // the trumps
  vector<Card> const& trumps() const;

  // whether 'card_a' is less than 'card_b'
  bool less(Card const& card_a, Card const& card_b) const;
  // the next lower card
  Card next_lower_card(Card const& card) const;
  // the next higher card
  Card next_higher_card(Card const& card) const;
  // the next higher card in n steps
  Card next_higher_card(Card const& card, unsigned const n) const;
  // the cards lower than 'card'
  vector<Card> lower_cards(Card const& card) const;
  // the cards higher than 'card'
  vector<Card> higher_cards(Card const& card) const;
  // the cards between 'card_a' and 'card_b'
  vector<Card> cards_between(Card const& card_a, Card const& card_b) const;
  // the trump cards lower than 'card'
  vector<Card> lower_trumps(Card const& card) const;

  // whether the team has got a trick
  bool hastrick(TEAM::Team const& team) const;
  // whether the player has got a trick
  bool hastrick(Player const& player) const;
  // the number of tricks the player has got
  unsigned numberoftricks_of_player(Player const& player) const;
  // the number of tricks the player til the trick 'trickno' has got
  unsigned numberoftricks_of_player(Player const& player,
                                    unsigned trickno) const;
  // the number of tricks the team has got
  unsigned numberoftricks_of_team(Team const& team) const;
  // the number of points the player has got
  unsigned points_of_player(Player const& player) const;
  // the number of points the player has got
  unsigned points_of_player(unsigned const playerno) const;
  // the number of points the player till the trick 'trickno' has got
  unsigned points_of_player(Player const& player,
                            unsigned trickno) const;
  // the number of points the player till the trick 'trickno' has got
  unsigned points_of_player(unsigned const playerno,
                            unsigned trickno) const;
  // the points, the team has got
  unsigned points_of_team(Team const& team) const;
  // the played points
  unsigned played_points() const;
  // the points still in the game
  unsigned remaining_points() const;
  // the points needed to win the game
  unsigned needed_points_to_win(Team const& team) const;
  // the winnerteam
  Team winnerteam() const;


  // announcements

  // the announcement of the team
  AnnouncementWithTrickno announcement_of_team(Team const& team) const;
  // the announcement of the player
  AnnouncementWithTrickno const& announcement_of_player(unsigned const playerno) const;
  // the announcement of the player
  AnnouncementWithTrickno const& announcement_of_player(Player const& player) const;
  // all announcements of the player
  vector<AnnouncementWithTrickno> const& announcements_of_player(Player const& player) const;
  // set the announcement of the player
  void make_announcement(Announcement announcement,
                         Player const& player);
#ifdef USE_NETWORK
  // set the announcement of the player (from the network)
  void make_announcement_from_network(Announcement announcement,
                                      Player const& player);
#endif
  // test, whether the announcement is valid
  Announcement announcement_valid(Announcement announcement,
                                  Player const& player) const;
  // whether this is the last possibility to announce the announcement
  bool last_chance_to_announce(Announcement const& announcement,
                               Player const& player) const;


  // tricks

  private:
  // all tricks
  vector<Trick*>& tricks();
  public:
  // the number of tricks in the game
  unsigned trickno() const;
  // the trick nmber 'i'
  Trick const& trick(unsigned const& i) const;
  // the current trick
  Trick const& trick_current() const;
  // the current trick
  Trick& trick_current();
  // the number of tricks in all trickpiles
  unsigned tricks_in_trickpiles() const;
  // the number of the current trick
  unsigned trick_current_no() const;
  // the number of the current trick
  unsigned real_trick_current_no() const;
  // the number of remaining tricks
  unsigned tricks_remaining_no() const;
  // the number of remaining tricks
  unsigned real_tricks_remaining_no() const;
  // whether this is the last trick
  bool is_last_trick() const;
  // the number of the trump tricks
  unsigned trumptricks_no() const;


  // teaminfo (known to all)

  // the global known team of the player
  Team teaminfo(Player const& player) const;
  // the global known team of the player
  Team teaminfo(unsigned const playerno) const;
  // the known team for all humans
  Team teaminfo_for_humans(Player const& player) const;
  // set the teaminfo of 'player' to 'team'
  void set_teaminfo(Player const& player, Team const& team);

  // reset the teaminfo
  void teaminfo_reset();
  // set the teaminfo for the gamestart
  void teaminfo_set_at_gamestart();
  // update the team info
  void teaminfo_update();
  // update the team info for the humans
  void human_teaminfo_update();


  // team (real team)

  // the team of the player
  Team team(Player const& player) const;



  // swines and hyperswines

  // whether the card is a special card
  bool is_special(HandCard const& card) const;
  // the fox
  Card fox() const;
  // the dolle
  Card dolle() const;
  // the swine
  Card swine() const;
  // the swine
  Card hyperswine() const;



  // the player who has swines
  Player const* swines_owner() const;
  // whether the player can announce swines
  bool swines_announcement_valid(Player const& player) const;
  // announce swines
  bool swines_announce(Player& player);
  // the player who has hyperswines
  Player const* hyperswines_owner() const;
  // whether the player can announce swines
  bool hyperswines_announcement_valid(Player const& player) const;
  // announce hyperswines
  bool hyperswines_announce(Player& player);
  private:
  // test swines from the reservations
  void test_swines_from_reservations();
  public:


  // gameplay

  // play the game
  void play();
  // play the game according to the gameplay
  void play(Gameplay const& gameplay);
  private:
  // the party, the game is part of
  class Party& party();

  // request announcements from the players
  void announcements_request();
  // return a valid announcement for 'player'
  Announcement valid_announcement(Player const& player) const;
  // check, whether a genscher is played
  void check_for_genscher(HandCard const& played_card);
  // check for a silent marriage
  void check_for_silent_marriage();
  // check, whether the silent marriage is known to all
  void determine_silent_marriage();
  // check, whether the partner is now found
  void determine_marriage();

  // init the game
  void init();
  // make the shifting of the poverty cards
  void poverty_shift();
  // distribute the cards
  void distributecards();
  public:
  // distribute the cards (for network games)
  void distributecards(vector<Hand*> const& hands);
  protected:
  // the next player has to play a card
  void nextplayer();
  public:
  // the player plays a card
  void play_card(Player& player, Card card);
  void play_card(Card card);
  // evaluate the full trick
  void evaluatetrick();
  private:
  // finish the game
  void finish();

  // print statistics of the seeds
  void print_seed_statistics() const;

  private:
  // party, the game belongs to
  PRIV_VAR_P_R(class Party, party);

  // the gameplay saved as actions
  PRIV_VAR_P_R(Gameplay, gameplay);

  // players in this game 
  PRIV_VAR_R(vector<Player*>, players);
  // the startplayer of the game
  PRIV_VAR_CP(Player, startplayer);
  // the player whose turn it is
  PRIV_VAR_P(Player, player_current);

  // Seed for the distribution of the cards
  PRIV_VAR_RW(unsigned, seed);
  // type of the game
  PRIV_VAR(GameType, type);
  // the cards in the colors (in ascending order)
  PRIV_VAR_R(vector<vector<Card> >, cards);
  // player, who is playing a solo (NULL, if no one)
  PRIV_VAR_P(Player, soloplayer);
  // whether this is a duty solo
  PRIV_VAR_R(bool, is_duty_solo);
  // which trick selects the team
  PRIV_VAR_R(MarriageSelector, marriage_selector);
  // in which trick the marriage has been determined
  PRIV_VAR_R(unsigned, marriage_determination_trickno);

  // the shifted cards in the poverty;
  PRIV_VAR(HandCards, poverty_cards);
  // the returned cards in the poverty;
  PRIV_VAR(HandCards, poverty_cards_returned);
  // whether the poverty cards were shifted
  PRIV_VAR_R(bool, poverty_shifted);



  // tricks in this game
  PRIV_VAR_R(vector<Trick*>, tricks);

  // Team of the players (information, known to all)
  PRIV_VAR(vector<Team>, teaminfo);
  // Team of the players (information, known to all humans)
  PRIV_VAR(vector<Team>, human_teaminfo);
  // Announcement of the players (information, known to all)
  PRIV_VAR(vector<vector<AnnouncementWithTrickno> >, announcement);
#ifdef USE_NETWORK
  // announcements made over the network
  PRIV_VAR(vector<Announcement>, announcement_over_network);
#endif

  //card::is... 

  // the player who has two fox and has made the reservation (for game)
  PRIV_VAR_P(Player, swines_owner);
  // the player, who has caught the first fox
  PRIV_VAR_CP_PR(Player, first_fox_catcher);
  // whether the swines has been announced
  PRIV_VAR_R(bool, swines_announced);

  // the player who has two trump nines and has made the reservation (for game)
  PRIV_VAR_P(Player, hyperswines_owner);
  // whether the swines has been announced
  PRIV_VAR_R(bool, hyperswines_announced);

  // whether the game is finished
  PRIV_VAR_R(bool, finished);

  private: // not to be used  
  Game();
  Game(Game const& game);
  Game& operator=(Game const& g);
}; // class Game

// output of the game
ostream& operator<<(ostream& ostr, Game const& game);

// comparison
bool operator==(Game const& game1, Game const& game2);
bool operator!=(Game const& game1, Game const& game2);

#endif // #ifndef CLASS_GAME_HEADER
