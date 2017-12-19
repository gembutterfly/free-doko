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

#pragma once

#include "../basistypes.h"

#include "hand_cards.h"
#include "../game/specialpoint.h"
class Game;
class Player;
class Hand;

/**
 ** @brief represents a trick (the cards and the startplayer)
 **
 ** This class is part of game
 **
 ** @see Game
 ** @see Card
 **
 ** @author Borg Enders
 ** @author Diether Knof
 **
 ** @version 0.4.4
 **
 **/
class Trick {
  friend class Game;
  public:
  // empty trick
  static Trick const EMPTY;
  // constructor
  Trick(Player const& startplayer);
  // constructor
  Trick(unsigned const startplayer, vector<Card> const& cards,
	unsigned const winnerplayer = UINT_MAX);
  // constructor
  Trick(istream& istr);
  // constructor
  Trick();
  // destructor
  ~Trick();

  // copy the trick
  Trick(Trick const& trick);
  // copy the trick
  Trick& operator=(Trick const& trick);


  // set the game
  void set_game(Game const& game);

  // checks the data
  bool self_check() const;

  // output of the trick
  ostream& out(ostream& ostr) const;

  // add a card to the trick
  Trick& operator+=(HandCard const& c);

  // the trick till the given card
  Trick till_card(unsigned const c) const;
  // the trick till the given player
  Trick till_player(Player const& player) const;
  // the trick till before the given player
  Trick before_player(Player const& player) const;
  // the trick without the last played card
  Trick before_last_played_card() const;

  // the requested card
  HandCard const& card(unsigned const i) const;
  // the card played by the player
  HandCard const& card_of_player(Player const& player) const;


  // the player of the card no i
  Player const& player_of_card(unsigned const c) const;
  // the number of the player of the card no i
  unsigned playerno_of_card(unsigned const c) const;
  // at which position the player 'player' plays his card
  unsigned cardno_of_player(Player const& player) const;
  // returns whether the player has already played a card
  bool has_played(Player const& player) const;
  // returns whether the card is in the trick
  bool contains(Card const& card) const;
  // returns whether the trick contains a fox
  bool contains_fox() const;
  // returns whether the trick contains a possible extrapoint
  bool contains_possible_extrapoint(Game const& game) const;

  // the startcard
  HandCard const& startcard() const;
  // whether this is the startcard
  bool isstartcard() const;
  // whether the trick is empty
  bool isempty() const;
  // whether the trick is full
  bool isfull() const;
  // whether there only lacks one card
  bool islastcard() const;

  // moves the trick in the trickpile
  void move_in_trickpile();


  // returns the startplayer
  Player const& startplayer() const;
  // returns the last player
  Player const& lastplayer() const;
  // returns the second last player
  Player const& secondlastplayer() const;
  // returns the number of the startplayer
  unsigned startplayerno() const;
  // set the startplayer
  void set_startplayer(Player const& player);
  // set the startplayer
  void set_startplayer(unsigned const startplayerno);
  // returns how many cards are in the trick
  unsigned actcardno() const;
  // returns how many cards still have to be played
  unsigned remainingcardno() const;
  // returns which player has to play a card
  Player const& actplayer() const;
  // returns which player has to play a card
  unsigned actplayerno() const;
  // returns which player gets the trick (so far)
  Player const& winnerplayer() const;
  // returns the number of the winnerplayer
  unsigned winnerplayerno() const;

  // returns the points of the trick
  unsigned points() const;

  // returns whether the card is valid for the hand it belongs to
  bool isvalid(HandCard const& card) const;
  // returns whether the card is valid for the given hand
  bool isvalid(Card const& card, Hand const& hand) const;

  // returns the winnerteam
  Team winnerteam() const;
  // returns the winnercard
  HandCard const& winnercard() const;
  // returns the number of the winnercard
  unsigned winnercardno() const;
  // returns the winnercard before the last played card
  HandCard const& winnercard_before() const;

  // whether the card 'cardno' jabs the cards before
  bool jabs_cards_before(unsigned const cardno) const;
  // whether the player jabs the cards before
  bool jabs_cards_before(Player const& player) const;

  // returns whether this is the last trick
  bool islast() const;



  // whether the trick will be jabbed by this card
  bool isjabbed(Card const& card) const;
  // whether the trick will be jabbed by this card
  bool isjabbed(Card::Color const& color, Card::Value const& value) const;
  // whether the trick will be jabbed by this card, take possible special cards (p.e. swines) into account
  bool isjabbed(HandCard const& card) const;
  // whether the trick will be jabbed by this card, take possible special cards (p.e. swines) into account
  bool isjabbed(Card const& card, Hand const& hand) const;
  // whether card 'cardno_a' is less than card 'cardno_b' in the trick
  bool less(unsigned const cardno_a, unsigned const cardno_b) const;


  // returns the specialpoints of the trick
  // with all information taken from 'game'
  Specialpointsvector specialpoints() const;
  // returns the specialpoints of the trick
  Specialpointsvector specialpoints(vector<Team> const& real_team) const;
  public:
  // returns a vector with the cards that get a specialpoint
  HandCards specialpoints_cards() const;
  // whether this is a specialpoints card
  bool is_specialpoints_card(unsigned const c) const;

  private:
  // cards in this trick (sorted in the order of playing)
  PRIV_VAR_R(HandCards, cards);

  protected:
  // the game, the trick is part of
  PRIV_VAR_CP_R(Game, game);

  // the number of the trick
  PRIV_VAR_R(unsigned, no);

  // player, who played the first card
  unsigned startplayer_;
  // winnerplayer
  unsigned winnerplayer_;

  private:
  // whether the trick is in the trickpile
  PRIV_VAR_R(bool, intrickpile);

  private: // not to be used
}; // class Trick

// output of the trick
ostream& operator<<(ostream& ostr, Trick const& trick);

// comparison of the tricks
bool operator==(Trick const& trick1, Trick const& trick2);
bool operator!=(Trick const& trick1, Trick const& trick2);
