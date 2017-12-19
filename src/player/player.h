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

#ifndef CLASS_PLAYER_HEADER
#define CLASS_PLAYER_HEADER

#include "../basistypes.h"

#include "../card/hand.h"
#include "../card/sorted_hand.h"
#include "../game/reservation.h"
#include "../misc/translation.h"


class PlayersDb;
class GameSummary;
class Game;

typedef vector<Trick const*> TrickPile;

/**
 **
 ** the base class for a player (human or ai)
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 **
 **/
class Player {
  friend class Party;
  friend class Game;
  public:

  enum Type {
    UNSET = 0,
    HUMAN = 0x10,
    AI = 0x100,
    AI_DUMMY = 0x101,
    AI_RANDOM = 0x102,
    NETWORK = 0x1000
  }; // enum Type

  public:
  // the default name for the player
  static string default_name(Type const& type, unsigned const playerno);
  // the default voice for the player
  static string default_voice(Type const& type, unsigned const playerno,
                              string const& name);

  // whether to write the database
  static bool write_database;

  public:    
  // create a player of the given type
  static Player* create(Type const& type);
  // create a player of the given type
  static Player* create(Type const& type, string const& name);
  // create a player of the given type with data from 'player'
  static Player* create(Type const& type, Player const& player);
  // create a player with the infos from 'filename'
  static Player* create(string const& filename);
  // create a player with the infos from 'istr'
  static Player* create(istream& istr);

  public:
  // constructor
  Player(Type const& type);
  // copy the player
  Player(Player const&);
  // copy the player
  Player& operator=(const Player&);
  // clone the player
  virtual Player* clone() const;

  // destructor
  virtual ~Player();

  // checks the data
  virtual bool self_check() const;


  // comparing

  bool is_equal(Player const& player) const;

  // save the player in the given file
  bool save(string const& filename) const;
  // write the player in 'ostr'
  virtual ostream& write(ostream& ostr) const;
  // write the player in 'ostr' (name and type)
  void write_short(ostream& ostr) const;

  // read the config 'config' from 'istr'
  virtual bool read(Config const& config, istream& istr);

  // the game of the player
  Game const& game() const;
  // the game of the player
  Game& game();
  // set the game
  virtual void set_game(Game& game);
  // the number of the player in the game
  unsigned no() const;



  // name

  // the name
  string const name() const;
  // set the name to the default one
  void set_default_name();
  // set the name
  void set_name(string const& name);
  // the value of the name (empty, if the default name is taken)
  string const& name_value() const;


  // voice

  // the voice
  string const voice() const;
  // set the voice to the default one
  void set_default_voice();
  // set the voice
  void set_voice(string const& voice);
  // the value of the voice (empty, if the default voice is taken)
  string const& voice_value() const;

  // Database
  PlayersDb const& db() const;
  PlayersDb& db();


  // party information

  bool is_remaining_duty_solo(GameType const game_type) const;
  unsigned remaining_duty_soli() const;
  unsigned remaining_duty_color_soli() const;
  unsigned remaining_duty_picture_soli() const;
  unsigned remaining_duty_free_soli() const;


  // hand

  // the hand
  Hand const& hand() const;
  // the hand
  Hand& hand();
  // the hand
  SortedHand const& sorted_hand() const;
  // the hand
  SortedHand& sorted_hand();
  // whether the player has swines
  bool has_swines() const;
  // whether the player has hyperswines
  bool has_hyperswines() const;



  // reservation

  // the reservation
  Reservation const& reservation() const;
  // the reservation
  Reservation& reservation();

  // trickpile

  // the trickpile of the player
  TrickPile const& trickpile() const;



  // team and teaminfo

  // the team
  //Team team() const;
  // set the team
  //void set_team(Team const team);

  // update the teaminfo
  virtual void teaminfo_update();


  // announcement (self and other)

  // the announcement the player has made (take from 'game')
  Announcement const& announcement() const;
  // the announcement the player can make next
  Announcement next_announcement() const;


  // the number of tricks of the player (take from 'game')
  unsigned number_of_tricks() const;
  // the number of trickpoints
  unsigned points_in_trickpile() const;

  // how many cards the player has still to play in the game
  unsigned cards_to_play() const;

  // sort the hand
  Hand const& hand_sort();
  // mix the hand
  Hand const& hand_mix();
  // set the hand
  virtual Hand const& set_hand(Hand const& hand);

  // whether the player is re
  bool isre() const;


  // game information

  // a rule has changed
  virtual void rule_changed(int const type, void const* const old_value);

  // a new game is opened
  virtual void game_open(Game& game);
  // the game is closed
  virtual void game_close(Game const& game);

  // the game request an announcement
  virtual Announcement announcement_request() const;
  // a player has made an announcement
  virtual void announcement_made(Announcement const& announcement,
				 Player const& player);

  // 'player' shifts 'cardno' cards
  virtual void poverty_shift(Player const& player, unsigned const cardno);
  // the player 'player' has denied the poverty trumps
  virtual void poverty_take_denied(Player const& player);
  // all players have denied to take the cards
  virtual void poverty_take_denied_by_all();
  // the player 'player' has accepted the poverty trumps
  // and has returned 'cardno' cards with 'trumpno' trumps
  virtual void poverty_take_accepted(Player const& player,
				     unsigned const cardno,
				     unsigned const trumpno);
  // 'player' shifts cards
  virtual HandCards poverty_shift();
  // returns whether 'player' accepts the poverty
  virtual bool poverty_take_accept(unsigned const cardno);
  // the player changes the cards of the poverty
  virtual HandCards poverty_cards_change(HandCards const& cards);
  // the poverty player gets 'cards'
  virtual void poverty_cards_get_back(HandCards const& cards);

  // the marriage partner has found a bride
  virtual void marriage(Player const& bridegroom, Player const& bride);
  // return a partner for a genscher
  virtual Player const* genscher_partner();
  // a player has announced a genscher
  virtual void genscher(Player const& player, Player const& partner);


  // a new game is started
  virtual void new_game(Game& game);
  // returns the reservation of the plyaer
  virtual Reservation const& reservation_get( bool const is_duty = false );
  // returns the default reservation of the plyaer
  virtual Reservation reservation_get_default() const;
  // the game is started
  virtual void game_start();
  // the trick is opened
  virtual void trick_open(Trick const& trick);
  // get a card (but do not remove it from the hand, that makes 'game')
  virtual HandCard card_get();
  // the card is played by the player
  virtual void card_played(HandCard const& card);
  // the trick is full
  virtual void trick_full(Trick const& trick);
  // the trick is taken
  virtual void trick_taken();
  // move the trick in the pile
  virtual void move_in_trickpile(Trick const& trick);

  // check a swines announcement at the game start
  virtual void check_swines_announcement_at_game_start();
  // the player has swines
  virtual void swines_announced(Player const& player);
  // the player has hyperswines
  virtual void hyperswines_announced(Player const& player);

  // evaluate a finised game
  virtual void evaluate(GameSummary const& game_summary);

  private:
  // the game (NULL, if not in a game)
  Game* game_;

  // the type
  PRIV_VAR_RW(Type, type);
  // the name
  PRIV_VAR(string, name);
  // the voice
  PRIV_VAR(string, voice);
  // the color
  PRIV_VAR_RW(string, color);

  // the team of the player
  PRIV_VAR_RW(Team, team);

  // the hand
  PRIV_VAR(Hand, hand);
  // the sorted hand
  PRIV_VAR(SortedHand, sorted_hand);

  // the reservation
  PRIV_VAR(Reservation, reservation);

  // the trickpile of the player
  PRIV_VAR(TrickPile, trickpile);

  protected:  

  // statistics about the player
  PlayersDb* db_;


  private: // unused
  Player();
}; // class Player

// output of the player
ostream& operator<<(ostream& ostr, Player const& player);

// the name of the player type
string name(Player::Type const& type);
// output of the player type in the stream
ostream& operator<<(ostream& ostr, Player::Type const& type);

bool operator==(Player const& player1, Player const& player2);
bool operator!=(Player const& player1, Player const& player2);

inline
Translator::Translation translation(Player::Type const& type)
{ return Translator::Translation(::translator,
				 "Player::Type::"
				 + ::name(type)); }


#endif // #ifndef CLASS_PLAYER_HEADER
