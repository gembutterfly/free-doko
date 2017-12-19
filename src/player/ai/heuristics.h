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

#ifndef CLASS_HEURISTICS_HEADER
#define CLASS_HEURISTICS_HEADER

#include "../../card/card.h"
#include "../../card/hand_cards.h"
#include "../aiconfig.h"
class Trick;
class Hand;
class HeuristicInterface;
class Game;

class Heuristics {
  public:

    static Card play_to_marry(Trick const& trick,
                              HeuristicInterface const& hi);
    static Card play_to_get_married(Trick const& trick,
                                    HeuristicInterface const& hi);
    static Card play_color_in_solo(Trick const& trick,
                                   HeuristicInterface const& hi);			

    static Card choose_ace(Trick const& trick,
                           HeuristicInterface const& hi);
    static Card choose_ten(Trick const& trick,
                           HeuristicInterface const& hi);
    static Card choose_single_ace(Trick const& trick,
                                  HeuristicInterface const& hi);
    static Card choose_for_color_trick(Trick const& trick,
                                       HeuristicInterface const& hi);
    static Card jab_color_over_fox(Trick const& trick,
                                   HeuristicInterface const& hi);
    static Card retry_color(Trick const& trick,
                            HeuristicInterface const& hi);
    static Card serve_color_trick(Trick const& trick,
                                  HeuristicInterface const& hi);
    static Card serve_trump_trick(Trick const& trick,
                                  HeuristicInterface const& hi);
    static Card choose_pfund(Trick const& trick,
                             HeuristicInterface const& hi);
    static Card choose_pfund_before_partner(Trick const& trick,
                                            HeuristicInterface const& hi);
    static Card best_winning_card_old(Trick const& trick,
                                  HeuristicInterface const& hi, 
                                  unsigned pointmodi = 0 );
    static Card best_winning_card(Trick const& trick,
                                      HeuristicInterface const& hi);
    static Card jab_for_ace(Trick const& trick,
                            HeuristicInterface const& hi);
    static Card create_fehl(Trick const& trick,
                            HeuristicInterface const& hi);
    static Card play_color_for_partner(Trick const& trick,
                                       HeuristicInterface const& hi);
    static Card try_color_for_partner(Trick const& trick,
                                      HeuristicInterface const& hi);
    static Card play_color_for_partner_ace(Trick const& trick,
                                           HeuristicInterface const& hi);
    static Card play_bad_color(Trick const& trick,
                               HeuristicInterface const& hi);
    static Card start_with_color(Trick const& trick,
                                 HeuristicInterface const& hi);
    static Card play_low_high(Trick const& trick,
                              HeuristicInterface const& hi);
    static Card play_for_team(Trick const& trick,
                              HeuristicInterface const& hi);
    static Card jab_fox(Trick const& trick,
                        HeuristicInterface const& hi);
    static Card try_for_doppelkopf(Trick const& trick,
                                   HeuristicInterface const& hi);
    static Card SecondBestTrump(HeuristicInterface const& hi);  
    static Card partner_backhand_draw_trump(Trick const& trick,
                                            HeuristicInterface const& hi);
    static Card draw_trump(Trick const& t, HeuristicInterface const& hi );
    static Card play_highest_color(Trick const& trick,
                                   HeuristicInterface const& hi,
                                   Card::TColor const& co);
    static Card grab_trick(Trick const& trick,
                           HeuristicInterface const& hi);

    static Card start_with_low_color(Trick const& trick,
                                     HeuristicInterface const& hi);
    static Card start_with_low_trump(Trick const& trick,
                                     HeuristicInterface const& hi);

    static Card play_for_partner_worries(Trick const& trick,
                                         HeuristicInterface const& hi);

    // poverty

    static Card poverty_special_offer_pfund(Trick const& trick,
                                            HeuristicInterface const& hi);
    static Card poverty_special_play_pfund(Trick const& trick,
                                           HeuristicInterface const& hi);
    static Card poverty_special_give_no_points(Trick const& trick,
                                               HeuristicInterface const& hi);
    static Card poverty_re_trump_color_trick_high(Trick const& trick,
                                                  HeuristicInterface const& hi);
    static Card poverty_re_play_trump(Trick const& trick,
                                      HeuristicInterface const& hi);
    static Card poverty_contra_play_color(Trick const& trick,
                                          HeuristicInterface const& hi);
    static Card poverty_contra_trump_color_trick_high(Trick const& trick,
                                                      HeuristicInterface const& hi);
    static Card poverty_leave_to_partner(Trick const& trick,
                                         HeuristicInterface const& hi);
    static Card poverty_overjab_re(Trick const& trick,
                                   HeuristicInterface const& hi);
    static Card poverty_best_winning_card(Trick const& trick,
                                          HeuristicInterface const& hi);
    static Card play_highest_color_card_in_game(Trick const& trick,
                                                HeuristicInterface const& hi);
    static Card play_to_jab_later(Trick const& trick,
                                  HeuristicInterface const& hi);				  

    // announcement

    static bool make_announcement(HeuristicInterface const& hi);
    static bool say_no90(HeuristicInterface const& hi);
    static bool say_no60(HeuristicInterface const& hi);
    static bool say_no30(HeuristicInterface const& hi);
    static bool say_no0(HeuristicInterface const& hi);
    static int CalcHandValue(HeuristicInterface const& hi);
    static int CalcHandValue(HeuristicInterface const& hi, Game const& game);

    static Announcement make_reply(HeuristicInterface const& hi);

    static Card get_trick_for_keeping_announcement(Trick const& t, HeuristicInterface const& hi);

    static Card let_trick_pass(Trick const& trick, HeuristicInterface const& hi);

    static Card choose_best_prefiltered_card(Trick const& trick, HeuristicInterface const& hi, Hand const& hand, HandCards const& cards);
    static Card choose_best_prefiltered_color_card(Trick const& trick, HeuristicInterface const& hi, Hand const& hand, HandCards const& cards);
    static Card choose_best_prefiltered_trump_card(Trick const& trick, HeuristicInterface const& hi, Hand const& hand, HandCards const& cards);
    static Card play_trump(Trick const& trick,
                           HeuristicInterface const& hi);
  private:
    static Card choose_pfund_card(Trick const& trick,
                                  HeuristicInterface const& hi);
    static Card lowest_best_trump_card(Trick const& trick,
                                       Hand const& hand,
                                       HeuristicInterface const& hi);
    static Card best_winning_card_single_picture(Trick const& t, 
                                                 Card::Value v, HeuristicInterface const& hi, unsigned pointmodi );
    static Card best_winning_card_triple_picture(Trick const& t, HeuristicInterface const& hi, unsigned pointmodi );
    static Card best_winning_card_double_picture(Trick const& t, Card::Value v1, Card::Value v2, HeuristicInterface const& hi, unsigned pointmodi );


  public:
    static Card lowest_card(Trick const& trick,
                            Hand const& hand);
    static Card lowest_trump_card(Hand const& hand);
    static Card lowest_color_card(Card::TColor const& tcolor,
                                  Hand const& hand);
    static Card lowest_free_card(Hand const& hand);

    // return a better card to play, t.i. take a lower one if they are equal
    static HandCard optimized_card_to_play(Card const& card,
                                           Trick const& trick,
                                           HeuristicInterface const& hi,
                                           bool const only_same_points = false);

    // lowest serving card
    static Card lowest_serving_card(Trick const& trick,
                                    HeuristicInterface const& hi);
    // best jabbing card
    static Card best_jabbing_card(Trick const& trick,
                                  HeuristicInterface const& hi,
                                  bool const jab_always = false);
    // a jabbing card (best winning (= best jabbing) or lowest jabbing card)
    static Card jabbing_card(Trick const& trick,
                             HeuristicInterface const& hi);
}; // class Heuristics


#endif // #ifndef CLASS_HEURISTICS_HEADER
