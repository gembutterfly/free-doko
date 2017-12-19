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

#pragma once

#include "../../card/card.h"

#include "../../basistypes.h"
#include "../../card/hand.h"
#include "../../card/card_counter.h"
#include "../../card/tcolor_counter.h"

#include "../../game/exception.h"

class Player;
class Ai;
class Game;

#ifndef RELEASE
// card of interest, for debugging
#define COI Card(Card::CLUB, Card::ACE)
#endif

/**
 ** Contains and analyses the information, who has played which card
 ** and gives the information which cards a player can have.
 **
 ** @author	Diether Knof
 **/
class CardsInformation {
  friend bool operator==(CardsInformation const&, CardsInformation const&);
  friend class Ai;

  public:
  class OfPlayer;

  public:
  CardsInformation() = delete;
  CardsInformation(Ai const& player);
  CardsInformation(CardsInformation const& cards_information);
  CardsInformation& operator=(CardsInformation const& cards_information);
  ~CardsInformation();


  // writes the information in 'ostr'
  void write(ostream& ostr) const;
  // writes the summary information in 'ostr'
  void write_summary(ostream& ostr) const;

  // whether this cards information is from a virtual game or an internal used
  bool is_virtual() const;

  // reset all informations
  void reset();
  // recalcs all information
  void recalc();

  // the corresponding player
  Ai const& player() const;
  bool auto_update() const;
  void set_auto_update(bool a);

  // the information of single players
  OfPlayer const& of_player(Player const& player) const;
  OfPlayer const& of_player(unsigned const playerno) const;
  OfPlayer& of_player(Player const& player);
  OfPlayer& of_player(unsigned const playerno);

  // how many of the players still have unknown cards
  unsigned remaining_unknown_players() const;

  // the color runs (without the current trick)
  unsigned color_runs(Card::TColor const& tcolor) const;
  // the trump runs (without the current trick)
  unsigned trump_runs() const;

  // hand with all possible cards of the player
  Hand const& possible_hand(Player const& player) const;
  // hand with all possible cards of the player
  Hand const& possible_hand(unsigned const playerno) const;
  // the estimated hand for the player
  Hand const& estimated_hand(Player const& player) const;
  // the estimated hand for the player
  Hand const& estimated_hand(unsigned const playerno) const;


  // the highest remaining trump
  Card highest_remaining_trump() const;
  // the highest remaining trump of the other players
  Card highest_remaining_trump_of_others() const;
  // the highest remaining card of the other players
  Card highest_remaining_card_of_others(Card::TColor const& tcolor) const;
  // whether there exists a higher card on the other hands
  bool higher_card_exists(HandCard const& card) const;
  // the number of higher cards of the other players
  unsigned higher_cards_no_of_others(HandCard const& card) const;

  // the corresponding game
  Game const& game() const;

  // some settings

  // set the player
  void set_player(Ai const& player);
  // set the hand of the player
  void set_hand(Player const& player, Hand const& hand);


  // intern methods

  private:
  // updates the information of the cards
  void queue_update(vector<Card> const& cards);
  // updates the information of the card
  void queue_update(Card const& card);
  // updates the information of the tcolor
  void queue_update(Card::TColor const& tcolor);
  // updates the information of all cards
  void queue_update_all();

  // the following functions are const, but the variables to update are mutable
  // do the update
  void do_update() const;
  // updates 'can have' according to 'remaining cards'
  void update_remaining_cards();
  // check joined hands
  void check_joined_hands();
#ifdef DKNOF
  public:
#endif
  // updates the (possible and estimated) hands
  void recalc_hands() const;
  // updates the possible hands
  void recalc_possible_hands() const;
  // updates the estimated hands
  void recalc_estimated_hands() const;
  // recalc the weightings
  void recalc_weightings() const;

  public:

  // information

  // the number of forgottennknown tricks
  unsigned forgotten_tricks_no() const;
  // the number of forgottennknown cards
  unsigned forgotten_cards_no() const;
  // how many cards have been played in total in the game
  unsigned played_cards_no() const;
  // how many of 'card' have been played
  unsigned played(Card const& card) const;
  // how many of 'Card(color, value)' have been played
  unsigned played(Card::Color const& color, Card::Value const& value) const;
  // how many cards of 'tcolor' have been played
  unsigned played(Card::TColor const& tcolor) const;
  // how many cards the player still has to play
  unsigned cardno_to_play(Player const& player) const;
  // how many cards still are in the game
  unsigned remaining_cards_no() const;
  // how many of 'card' still are in the game
  unsigned remaining(Card const& card) const;
  // how many trumps still are in the game
  unsigned remaining_trumps() const;
  // how many cards of 'tcolor' still are in the game
  unsigned remaining(Card::TColor const& tcolor) const;
  // how many cards the other players do have
  unsigned remaining_others_cards_no() const;
  // how many of 'card' the other players do have
  unsigned remaining_others(Card const& card) const;
  // how many of 'card' the other players do have
  unsigned remaining_others(Card::Color const& color,
                            Card::Value const& value) const;
  // how many trumps other players do have
  unsigned remaining_trumps_others() const;
  // how many cards of 'tcolor' the other players do have
  unsigned remaining_others(Card::TColor const& tcolor) const;

  // how many of 'card' are known (played or knowned for sure)
  unsigned known(Card const& card) const;
  // how many of 'card' are remaining and unknown where
  unsigned remaining_unknown(Card const& card) const;

  // the joined "can have" information of both players
  CardCounter joined_can_have(OfPlayer const& p1, OfPlayer const& p2) const;

  // the information from the game

  // the game starts
  void game_start();
  // the trick is opened
  void trick_open(Trick const& trick);
  // the trick is full
  void trick_full(Trick const& trick);
  // the card is played
  void card_played(HandCard const& card);
  // the card is played
  void card_played(HandCard const& card, Trick const& trick);
  // an announcement is made
  void announcement_made(Announcement const& announcement,
			 Player const& player);
  // the player has swines
  void swines_announced(Player const& player);
  // the player has hyperswines
  void hyperswines_announced(Player const& player);
  // a player has announced a genscher
  void genscher(Player const& genscher, Player const& partner);

  private:
#ifdef DKNOF
  public:
#endif
  // checks the data for error
  bool self_check() const;

  private:
  // the corresponding player
  Ai const* player_ = nullptr;
  // the information for a specific player
  mutable vector<OfPlayer> of_player_;
  // the color runs (without the current trick)
  vector<unsigned> color_runs_;
  // how many cards have been played
  CardCounter played_;
  // how many tcolors have been played
  TColorCounter tcolor_played_;
  // whether we are in a recalcing
  bool in_recalcing = false;
  // whether we are in an update
  mutable bool in_update = false;
  // the possible hands
  mutable vector<Hand> possible_hands_;
  // the estimated hands
  mutable vector<Hand> estimated_hands_;

  // whether to update the dependend cards and recalc the hands automatically
  bool auto_update_ = true;

  // the cards that shall be updated
  mutable set<Card> cards_to_update;

  // whether the hands are outdated and must be updated
  mutable bool hands_outdated = false;

}; // class CardsInformation

// writes the cards information into the output stream
ostream& operator<<(ostream& ostr,
		    CardsInformation const& cards_information);
// compares two cards informations
bool operator==(CardsInformation const& cards_information_a,
		CardsInformation const& cards_information_b);
// compares two cards informations
bool operator!=(CardsInformation const& cards_information_a,
		CardsInformation const& cards_information_b);
