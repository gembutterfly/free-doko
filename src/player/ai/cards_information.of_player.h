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

#ifndef CLASS_CARDS_INFORMATION_OF_PLAYER_HEADER
#define CLASS_CARDS_INFORMATION_OF_PLAYER_HEADER

#include "cards_information.h"

/**
 ** Information of the cards a specific player can have
 **
 ** @author   Diether Knof
 **
 ** @todo     join the vector<Card> and vector<Card> functions (by begin, end iterators?)
 ** @todo     open shifted fox
 **/
class CardsInformation::OfPlayer {
  friend bool operator==(OfPlayer const&, OfPlayer const&);
  friend class CardsInformation;

  public:
  OfPlayer(CardsInformation& cards_information,
	   unsigned const playerno);
  // copy constructor
  OfPlayer(OfPlayer const& of_player);
  // copy operator
  OfPlayer& operator=(OfPlayer const& of_player);
  // destructor
  ~OfPlayer();


  // reset all informations
  void reset();


  // writes the information in 'ostr'
  void write(ostream& ostr) const;


  // the corresponding game
  Game const& game() const;
  // the corresponding player
  Player const& player() const;

  // changes the corresponding cards information
  void set_cards_information(CardsInformation& cards_information);

  // whether all cards are known
  bool all_known() const;

  // how many cards 'card' the player has played
  unsigned played(Card const& card) const;
  // how many cards of the tcolor 'tcolor' the player has played
  unsigned played(Card::TColor const& tcolor) const;
  // how many cards the player must still have on the hand
  unsigned must_have(Card const& card) const;
  // how many cards the player must still have on the hand
  unsigned must_have(Card::TColor const& tcolor) const;
  // how many cards the player can still have on the hand
  unsigned can_have(Card const& card) const;
  // how many cards the player can still have on the hand
  unsigned can_have(Card::TColor const& tcolor) const;
  // whether the player cannot have the card
  bool cannot_have(Card const& card) const;
  // whether the player has not the color
  bool does_not_have(Card::TColor const& tcolor) const;
  // how the unkown number of 'card' (can have - must have)
  unsigned unknown(Card const& card) const;
  // how the unkown number of 'tcolor' (can have - must have)
  unsigned unknown(Card::TColor const& tcolor) const;


  // the weighting for the card
  int weighting(Card const& card, bool const modify = true) const;


  // the cards the player has played
  Hand played_cards() const;
  // the cards the player must have
  Hand must_have_cards() const;
  // the cards the player cannot have
  Hand cannot_have_cards() const;
  // hand with all possible cards of the player
  Hand possible_hand() const;
  // the estimated hand for the player
  Hand estimated_hand() const;

  // checks, whether the hand is valid
  bool hand_valid(Hand const& hand) const;


  // the game starts
  void game_start();
  // the player has played 'card'
  void card_played(HandCard const& card, Trick const& trick);

  // change the weightings according to the played card
  void weight_played_card(HandCard const& card, Trick const& trick);

  // adds the information that the player must have the card 'no' times
  void add_must_have(Card const& card, unsigned const no = 1);
  // adds the information that the player must have the cards
  void add_must_have(vector<Card> const& cards);
  // adds the information that the player must have the tcolor cards 'no' times
  void add_must_have(Card::TColor const& tcolor, unsigned const no);

  // adds the information that the player can have the card at max 'no' times
  void add_can_have(Card const& card, unsigned const no);
  // adds the information that the player can have the cards at max 'no' times
  void add_can_have(vector<Card> const& cards, unsigned const no);
  // adds the information that the player can only have the cards
  void add_can_only_have(vector<Card> const& cards);
  // adds the information that the player can have the tcolor cards at max 'no' times
  void add_can_have(Card::TColor const& tcolor, unsigned const no);
  // adds the information that the player must have the tcolor cards exactly 'no' times
  void add_must_exactly_have(Card::TColor const& tcolor, unsigned const no);

  // adds the information that the player cannot have the card
  void add_cannot_have(Card const& card);
  // adds the information that the player cannot have the cards
  void add_cannot_have(vector<Card> const& cards);
  // whether the player has not the color
  void add_cannot_have(Card::TColor const tcolor);

  // updates the information of the card
  void update_information(Card const& card);
  // updates the information of the tcolor
  void update_information(Card::TColor const& tcolor);
  // updates the information of the tcolors
  void update_tcolor_information();
  // updates 'can have' according to 'remaining cards'
  void update_remaining_cards();

  private:
  // check whether the cards the player can have are the ones
  // he has to have
  void check_can_is_must();
  // check whether the cards the player can have are the ones
  // he has to have
  void check_can_is_must(Card::TColor const& tcolor);
  // check whether the cards the player has to have are the only ones
  // he can have
  void check_must_is_can();
  // checks the data for error
  bool self_check() const;

  private:
  // the cards information
  PRIV_VAR_P_RW(CardsInformation, cards_information);
  // the playerno
  PRIV_VAR_R(unsigned, playerno);

  // number of played cards of the player
  CardCounter played_;
  // number of cards, the player must have
  CardCounter must_have_;
  // number of cards, the player can have
  CardCounter can_have_;

  // number of played tcolors of the player
  TColorCounter tcolor_played_;
  // number of cards, the tcolors must have
  TColorCounter tcolor_must_have_;
  // number of cards, the tcolors can have
  TColorCounter tcolor_can_have_;

  // weighting for the cards for estimations
  // -100 
  mutable map<Card, int> cards_weighting_;
}; // class CardsInformation::OfPlayer

// writes the cards information about a player into the output stream
ostream& operator<<(ostream& ostr,
		    CardsInformation::OfPlayer const& of_player);
// compares two cards informations about a player
bool operator==(CardsInformation::OfPlayer const& of_player_a,
		CardsInformation::OfPlayer const& of_player_b);
// compares two cards informations about a player
bool operator!=(CardsInformation::OfPlayer const& of_player_a,
		CardsInformation::OfPlayer const& of_player_b);

#endif // #ifndef CLASS_CARDS_INFORMATION_OF_PLAYER_HEADER
