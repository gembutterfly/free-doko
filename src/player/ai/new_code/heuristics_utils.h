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

#ifndef HEURISTICS_UTILS_HEADER
#define HEURISTICS_UTILS_HEADER

#include "../../card/card.h"
#include "../../card/hand_cards.h"
class Trick;
class Hand;
class HeuristicInterface;
class Game;

namespace HEURISTICS_UTILS {

/////////////////////////
//  GENERAL UTILITIES  //
/////////////////////////

  // get the number of "no-trump" cards per color
  unsigned number_of_no_trumps_per_color();

  // whether the card 'test_card' is to preferred to 'old_card' that is has more points
  bool better_points_optimize(HandCard const& old_card,
                              HandCard const& test_card,
                              HeuristicInterface const& hi);

  // returns the number of the other players' cards that are higher than the given card
  unsigned numberof_higher_cards_others(HeuristicInterface const& hi,
                                        Card const& card,
                                        bool ignore_identical_card = false);

  // higher_card is a better card than lower_card if there is at least one card left
  // on the other players' hands that is lower than higher_card but higher than lower_card
  bool is_better_card(HeuristicInterface const& hi, Card const& higher_card, Card const& lower_card);


/////////////////////
//  TEAM UTILITIES //
/////////////////////

  // checks if all remaining player in the trick are of my team
  bool checkAllMyTeam(Trick const& t, HeuristicInterface const& hi);

  // checks if a solo player sits behind the player
  bool soloPlayerBehind(Trick const& t, HeuristicInterface const& hi);

  // calculates all points of own team in Game g
  int calcPointsOfOwnTeam(HeuristicInterface const& hi, Game const& g);

  // calculates all points of opposite team in Game g
  int calcPointsOfOppositeTeam(HeuristicInterface const& hi, Game const& g);

  // estimates the points for the teams based on the player's knowledge
  // if the teams aren't known to the player, a worst case scenario is used
  void estimate_team_points(HeuristicInterface const& hi, unsigned& own, unsigned& opposite);

  // checks if player p is a teammate
  bool is_teammate(HeuristicInterface const& hi, Player const& p, bool strict = false);
  // checks if player number p is a teammate
  bool is_teammate(HeuristicInterface const& hi, unsigned p, bool strict = false);
  // checks if player number p in trick t is a teammate
  bool is_teammate(HeuristicInterface const& hi, Trick const& t, unsigned p, bool strict = false);

  // checks if player p is a possible teammate
  bool is_possible_teammate(HeuristicInterface const& hi, Player const& player);
  // checks if player number p is a possible teammate
  bool is_possible_teammate(HeuristicInterface const& hi, unsigned p);
  // checks if player number p in trick t is a possible teammate
  bool is_possible_teammate(HeuristicInterface const& hi, Trick const& t, unsigned p);

  // checks if our partner knows or can guess our team
  bool partner_knows_me(HeuristicInterface const& hi);

  // counts the number of tricks the player's partner will win by using his
  // highest trumps, based on the knowledge found in handofplayer
  unsigned trump_tricks_guaranteed_by_partner(HeuristicInterface const& hi);


///////////////////////////////////////////////
//  UTILITIES FOR ANALYSING A PLAYER'S HAND  //
///////////////////////////////////////////////

  // finds lowest card of value cv
  Card lowest_card(Hand const& h, Card::Value const cv);

  // finds highest card of value cv
  Card highest_card(Hand const& h, Card::Value const cv);

  // finds lowest queen
  Card lowest_queen(Hand const& h);

  // finds highest queen
  Card highest_queen(Hand const& h);

  // finds lowest jack
  Card lowest_jack(HeuristicInterface const& hi, bool spare_charlie = true);

  // finds highest jack greater or equal to min_jack and lower or equal to max_queen
  Card highest_jack(Hand const& h, Card::Color min_jack, Card::Color max_queen, bool spare_charlie = true);

  // The predictive quality of handofplayer() varies from card to card because all cards
  // have to be distributed and all hands have to be complete (i.e. have at least as many
  // cards as there are tricks remaining). estimate_numberof therefore checks the quality
  // of the estimate using information from card counting and card weighting to calculate
  // the most likely *minimum* number of cards 'card' a player has.
  unsigned estimate_numberof(HeuristicInterface const& hi, Player const& player, Card const& card);

  // conservative estimate of the player's highest card (uses estimate_numberof())
  HandCard const& estimate_highest_card(HeuristicInterface const& hi, Player const& player);

  // conservative estimate of the player's highest card of suit tcolor (uses estimate_numberof())
  HandCard const& estimate_highest_card(HeuristicInterface const& hi, Player const& player, Card::TColor const tcolor);

  // checks if a player has a higher card than 'card' (uses estimate_numberof())
  bool player_has_higher_card(HeuristicInterface const& hi, Player const& player, Card const& card);

  // checks if the player has an endangered fox on hand, i.e. if the player can't
  // afford to wait for a better opportunity to play his fox(es).
  bool has_endangered_fox(Trick const& t, HeuristicInterface const& hi);

  // Returns the highest card of an opponent's hand. This is identical to
  // hand.highest_card() except for a re player in a normal game who doesn't
  // know his partner but should ignore club queens in opponents' hands.
  HandCard const& highest_opp_card(HeuristicInterface const& hi, Hand const& hand);


///////////////////////////////////////
//  UTILITIES FOR ANALYSING A TRICK  //
///////////////////////////////////////

  // check if own team can still win this trick.
  bool ownTeamCanWinTrick(Trick const &t, HeuristicInterface const& hi);

  // check if opposite team can still win this trick.
  bool oppositeTeamCanWinTrick(Trick const &t, HeuristicInterface const& hi);

  // check if a card to be played by the own team will win the trick
  bool cardWillWinTheTrick(Trick const &t, HeuristicInterface const& hi, HandCard const& card);

  // the lowest trump which guarantees the trick (if to_win is true) or the lowest of the player's highest trumps
  Card lowest_highest_trump(Trick const& trick,
                            HeuristicInterface const& hi,
                            bool to_win = true,
                            bool spare_charlie = true);

  // returns the lowest card within the given range that wins the given trick
  Card lowest_winner(Trick const& t,                       // the trick to win
                     HeuristicInterface const& hi,
                     bool spare_charlie = true,            // spare a single charlie
                     Card const& min_card = Card::EMPTY,   // not lower than this card (Card::EMPTY skips this test)
                     Card const& max_card = Card::EMPTY);  // not higher than this card (Card::EMPTY skips this test)

  // finds lowest trump card on Hand h, but gives only fuchs or ten back
  // if there are better cards to keep
  Card lowest_best_trump_card(Trick const& trick,
                              Hand const& hand,
                              HeuristicInterface const& hi);

  // checks if there are any trumps in trick t
  bool trumpInTrick(Trick const& trick);

  // checks if the player of hand has to follow plain suit in the given trick
  bool has_to_follow_suit(Trick const& trick, Hand const& hand);

  // the highest card of the players of the opposite team behind
  HandCard const& highest_opposite_card_behind(Trick const& trick, HeuristicInterface const& hi);

  // returns the highest opposite card behind which takes the trick and may lead to a doppelkopf
  Card doppelkopf_alert_card(Trick const& t, HeuristicInterface const& hi);

  // check if the card to be played is a plain suit trap: it's a trap if we know that the card
  // can be ruffed by the other team, but our partner doesn't
  bool is_plain_suit_trap(Trick const& t, HeuristicInterface const& hi, Card const& card_to_play);

  // checks if a suit will be ruffed by a player of the opposite team
  bool suit_will_be_ruffed_by_other_team(Trick const& t, HeuristicInterface const& hi, Card::Color color = Card::NOCARDCOLOR);


/////////////////////////////////
//  UTILITIES FOR PFUND CARDS  //
/////////////////////////////////

  // returns the best pfund on hand for the given trick
  Card choose_pfund_card(Trick const& trick, HeuristicInterface const& hi);

  // returns the highest card of the suit color in hand hi.hand() which will lose a trick
  // when played as first card (this can be an ace if the suit is going to be ruffed)
  Card highest_loser(HeuristicInterface const& hi, Card::Color const& color);

  // calculates the plain suit pfund cards in hand devided into categories of usefulness
  void calc_plain_suit_pfunds(HeuristicInterface const& hi,
                              bool to_lead,
                              bool below_ten,
                              Card& first_run,
                              Card& second_run_no_void,
                              Card& highest_card_no_void,
                              Card& second_run,
                              Card& highest_card,
                              Card& single_ace);

  // returns the smartest plain suit pfund in hand hi.hand()
  Card smartest_plain_suit_pfund(HeuristicInterface const& hi,
                                 bool to_lead = false,
                                 bool below_ten = false,
                                 bool ruff_only = false);


  // constants to be used as flags for the following most_precious_trump... functions
  // meant to be combined using bitwise operators
  enum PreciousTrumpFlags {
    // no flags at all
    PT_NO_FLAGS = 0,
    // spare a fox (not a trump ace in a solo!)
    PT_SPARE_FOX = 1,
    // try to spare a single charlie if it can make an extrapoint
    PT_SPARE_CHARLIE = 2,
    // spare trump ace or ten unless they are (hyper-)swines or dollen
    PT_SPARE_TEN = 4,
    // the card returned has to win the trick so far
    PT_PLAY_TO_WIN = 8,
    // the card returned must not win if played as the last card of the trick
    // (if PL_PLAY_TO_WIN and PT_PLAY_TO_LOSE are both set, no win/lose tests will be performed)
    PT_PLAY_TO_LOSE = 16,
    // the card returned must not win the trick
    // (implies PT_PLAY_TO_LOSE and overrides PT_PLAY_TO_WIN)
    PT_FORCE_LOSING = 32
  };

  // returns the most precious trump of the cards in hand, that is, in descending order:
  // ace - ten - king - jack - 9 - queen - dolle (- swine - hyperswine)
  // also works for all kinds of solos (order adjusted accordingly), except for meatless
  Card most_precious_trump(HeuristicInterface const& hi, Trick const& t, unsigned flags);

  // returns the most precious trump which wins the trick so far
  Card most_precious_trump_winner(HeuristicInterface const& hi, Trick const& t, unsigned flags = PT_SPARE_CHARLIE);

  // returns the most precious trump loser of the cards in hand
  Card most_precious_trump_loser(HeuristicInterface const& hi, Trick const& t, unsigned flags = PT_SPARE_CHARLIE);


/////////////////////////////////
//  UTILITIES FOR THROW CARDS  //
/////////////////////////////////

  // returns lowest card to play from 'cards'
  Card choose_best_prefiltered_card(HeuristicInterface const& hi, Hand const& hand, HandCards const& cards);

  // returns lowest plain suit card to play from 'cards'
  Card choose_best_prefiltered_color_card(HeuristicInterface const& hi, Hand const& hand, HandCards const& cards);

  // returns lowest trumo card to play from 'cards'
  Card choose_best_prefiltered_trump_card(HeuristicInterface const& hi, Hand const& hand, HandCards const& cards);

  // returns lowest card to play in the trick
  Card lowest_card(Trick const& trick, Hand const& hand);

  // returns lowest card of suit 'tcolor' to play
  Card lowest_color_card(Card::TColor const& tcolor, Hand const& hand);

  // returns lowest trump card to play
  Card lowest_trump_card(Hand const& hand);

  // returns lowest plain suit card to play
  Card lowest_free_card(Hand const& hand);

  // finds lowest color card on Hand h, wihich is the best in a meatless solo
  Card lowest_color_card_meatless(Trick const& t, Hand const& h);

  // smartest plain suit card to throw
  Card choose_best_fehl(Trick const& trick,
                        HeuristicInterface const& hi,
                        bool const forceRemaining = true,
                        bool const forceSingleton = true,
                        bool const forceFirstRun  = false);


  // checks if a player has control in a suit in a meatless solo
  bool meatless_color_control(HeuristicInterface const& hi, Card::Color const color, bool lead);

  // estimates the points for a trick in a meatless solo using a worst case scenario
  unsigned meatless_estimated_trick_points(Trick const& t,
                                           HeuristicInterface const& hi,
                                           HandCard const& card_to_play,
                                           bool const play_to_win);



} // namespace HEURISTICS_UTILS

#endif // #ifndef HEURISTICS_UTILS_HEADER

