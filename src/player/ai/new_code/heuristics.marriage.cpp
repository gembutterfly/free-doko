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
 ********************************************************************/

#include "constants.h"

#include "heuristics.h"
#include "heuristics_utils.h"

#include "HeuristicInterface.h"
#include "cards_information.h"
#include "cards_information.of_player.h"
#include "team_information.h"
#include "../../game/game.h"
#include "../../party/party.h"
#include "../../party/rule.h"
#include "../../card/trick.h"


using namespace HEURISTICS_UTILS;


/**
 ** returns a card to play for the bride in an open marriage
 **
 ** @param     trick   the current trick
 ** @param     hi      heuristic interface
 **
 ** @return    card to play (or Card())
 **
 ** @author    Borg Enders
 **
 ** @version   0.7.0
 **/
Card
Heuristics::play_to_marry( Trick const& trick,
                          HeuristicInterface const& hi )
{
  // @heuristic::name   play to marry
  // @heuristic::idea   play card to marry
  if( !(   hi.game().type() == GAMETYPE::MARRIAGE
        && hi.game().soloplayer().no() != hi.no()
       )
    )
    return Card::EMPTY;

  Card::Color color = Card::NOCARDCOLOR;

  if (trick.isempty()) {
    switch (hi.game().marriage_selector()) {
    case MARRIAGE_SELECTOR::SILENT:
    case MARRIAGE_SELECTOR::TEAM_SET:
      return Card::EMPTY;

    case MARRIAGE_SELECTOR::FIRST_FOREIGN:
    case MARRIAGE_SELECTOR::FIRST_TRUMP: {
      // @heuristic::condition   trick empty and first foreign or first trump
      // @heuristic::action      play the highest trump, if only the bride can jab
      Card const trump = lowest_highest_trump(trick, hi);
      if (trump.is_empty())
        return Card::EMPTY;

      if (!trump.less(highest_opposite_card_behind(trick, hi)))
        return trump;

      return Card::EMPTY;
    }
    case MARRIAGE_SELECTOR::FIRST_CLUB:
      color = Card::CLUB;
    case MARRIAGE_SELECTOR::FIRST_SPADE:
      if( color == Card::NOCARDCOLOR )
        color = Card::SPADE;
    case MARRIAGE_SELECTOR::FIRST_HEART:
      if( color == Card::NOCARDCOLOR )
        color = Card::HEART;

      // @heuristic::condition   trick empty and first specific color
      // @heuristic::action      play the ace of this color, if it can get through
      if (   (hi.hand().numberof(Card(color, Card::ACE)) > 0)
          && (hi.cards_information().remaining_others(Card(color, Card::ACE))
              >= hi.game().playerno() - 1) )
        return Card(color, Card::ACE);

      return Card::EMPTY;
    case MARRIAGE_SELECTOR::FIRST_COLOR:
      // this is caught by 'play ace'
      return Card::EMPTY;

    } // switch (hi.game().marriage_selector())
  } else { // if !(trick.isempty())

      // @heuristic::condition   must be a selector trick
    if (!is_selector(trick.startcard().tcolor(),
                     hi.game().marriage_selector()) )
      return Card::EMPTY;

      // @heuristic::condition   the player has the highest trump and can play it
      // @heuristic::action      play the highest trump, if only the bride can jab
      Card const card = choose_for_color_trick(trick, hi);
      if (!card.is_empty())
        return card;

      Card const trump = lowest_highest_trump(trick, hi);
      if (trump.is_empty())
        return Card::EMPTY;
      if (   trick.isvalid(trump, hi.hand())
          && trick.winnercard().less(trump)
          && !trump.less(highest_opposite_card_behind(trick, hi)))
        return trump;
  }  // if !(trick.isempty())


  return Card::EMPTY;
} // static Card Heuristics::play_to_marry(Trick trick, HeuristicInterface hi)

/**
 ** returns a card to play for the bride in an open marriage
 **
 ** @param     trick   the current trick
 ** @param     hi      heuristic interface
 **
 ** @return    card to play (or Card())
 **
 ** @author    Borg Enders
 **
 ** @version   0.7.6
 **/
Card
Heuristics::play_to_get_married( Trick const& trick,
                                HeuristicInterface const& hi )
{
  // @heuristic::name   play to get married
  // @heuristic::idea   play card to get married
  if( !(   hi.game().type() == GAMETYPE::MARRIAGE
        && hi.game().soloplayer().no() == hi.no()
       )
    )
    return Card::EMPTY;

  Card::Color color = Card::NOCARDCOLOR;

  switch( hi.game().marriage_selector() ) {
  case MARRIAGE_SELECTOR::SILENT:
  case MARRIAGE_SELECTOR::TEAM_SET:
    return Card::EMPTY;

  case MARRIAGE_SELECTOR::FIRST_FOREIGN:
    if( trick.isempty() ) {
      vector<Card> cards;
      for (vector<Card::Color>::const_iterator
           c = hi.game().rule().card_colors().begin();
           c != hi.game().rule().card_colors().end();
           ++c) {
        Card::Color const& color = *c;
        if (   Card(color, Card::ACE).istrump(hi.game())
            && !HandCard( hi.hand(), Card(color, Card::ACE)).possible_swine() )
          cards.push_back(Card(color, Card::ACE));
      } // foreach ( color in hi.game().rule().card_colors() )

      for (vector<Card::Color>::const_iterator
           c = hi.game().rule().card_colors().begin();
           c != hi.game().rule().card_colors().end();
           ++c) {
        Card::Color const& color = *c;
        if ( !HandCard( hi.hand(), Card(color, Card::TEN)).isdolle() ) {
          cards.push_back(Card(color, Card::TEN));
        }
      } // foreach ( color in hi.game().rule().card_colors() )

      for (vector<Card::Color>::const_iterator
           c = hi.game().rule().card_colors().begin();
           c != hi.game().rule().card_colors().end();
           ++c) {
        Card::Color const& color = *c;
        cards.push_back(Card(color, Card::KING));
      } // foreach ( color in hi.game().rule().card_colors() )

      for (vector<Card::Color>::const_iterator
           c = hi.game().rule().card_colors().begin();
           c != hi.game().rule().card_colors().end();
           ++c) {
        Card::Color const& color = *c;
        if ( !HandCard( hi.hand(), Card(color, Card::NINE)).possible_hyperswine() )
          cards.push_back(Card(color, Card::NINE));
      } // foreach ( color in hi.game().rule().card_colors() )

      for( vector<Card>::const_iterator card = cards.begin();
          card != cards.end();
          ++card ) {
        if( ( hi.hand().numberof( *card ) > 0 )  )
          return *card;
      } // foreach ( card in cards )

      return Card::EMPTY;
    }
    return choose_pfund_card( trick, hi );

  case MARRIAGE_SELECTOR::FIRST_TRUMP:
    if(   !trick.isempty()
       && trick.startcard().istrump() )
      return choose_pfund_card( trick, hi );

    if( trick.isempty() )
    {
      // take a 'good' card
      if(   hi.hand().numberoftrumpaces() > 0
         && !hi.hand().has_swines()
        )
        return Card(hi.game().trumpcolor(), Card::ACE);

      if(  hi.hand().numberof( Card( hi.game().trumpcolor(), Card::TEN ) ) > 0 )
        return Card(hi.game().trumpcolor(), Card::TEN);

      return lowest_best_trump_card(trick, hi.hand(), hi);
    } // if (trick.startplayer().no() == hi.no())
    break;

  case MARRIAGE_SELECTOR::FIRST_CLUB:
    color = Card::CLUB;
  case MARRIAGE_SELECTOR::FIRST_SPADE:
    if( color == Card::NOCARDCOLOR )
      color = Card::SPADE;
  case MARRIAGE_SELECTOR::FIRST_HEART:
    if( color == Card::NOCARDCOLOR )
      color = Card::HEART;

    if(   !trick.isempty()
       && trick.startcard().tcolor() == color
      )
      return choose_pfund_card( trick, hi );

    if( trick.isempty() )
    {
      vector<Card> cards;
      if (!hi.game().rule()(Rule::DOLLEN))
        cards.push_back(Card(color, Card::TEN));
      cards.push_back(Card(color, Card::KING));
      cards.push_back(Card(color, Card::NINE));
      //      cards.push_back(Card(color, Card::ACE)); // With Ace I will probably win the trick bad decision
      for( vector<Card>::const_iterator c = cards.begin();
          c != cards.end();
          ++c )
        if( hi.hand().numberof( *c ) > 0 )
          return *c;
    } // if( trick.isempty() )
    break;

  case MARRIAGE_SELECTOR::FIRST_COLOR:
    if( trick.isempty() )
    {
      vector<Card> cards;
      for (vector<Card::Color>::const_iterator
           c = hi.game().rule().card_colors().begin();
           c != hi.game().rule().card_colors().end();
           ++c)
      {
        Card::Color const& color = *c;
        if ( !HandCard( hi.hand(), Card(color, Card::TEN)).isdolle() )
          cards.push_back(Card(color, Card::TEN));
      } // foreach ( color in hi.game().rule().card_colors() )
      for (vector<Card::Color>::const_iterator
           c = hi.game().rule().card_colors().begin();
           c != hi.game().rule().card_colors().end();
           ++c)
      {
        Card::Color const& color = *c;
        cards.push_back(Card(color, Card::KING));

      } // foreach ( color in hi.game().rule().card_colors() )
      for (vector<Card::Color>::const_iterator
           c = hi.game().rule().card_colors().begin();
           c != hi.game().rule().card_colors().end();
           ++c)
      {
        Card::Color const& color = *c;
        cards.push_back(Card(color, Card::NINE));
      } // foreach ( color in hi.game().rule().card_colors() )

      for( vector<Card>::const_iterator card = cards.begin();
          card != cards.end();
          ++card )
      {
        if( ( hi.hand().numberof( *card ) > 0 ) && !card->istrump( hi.game() )  )
          return *card;
      } // foreach ( card in cards )

      return Card::EMPTY;
    } // if( trick.isempty() )

    if(    trick.startplayer().no() != hi.no()
       && !trick.startcard().istrump()
      )
      return choose_pfund_card( trick, hi );

    if( trick.startplayer().no() == hi.no() )
      return lowest_card(trick, hi.hand());

    break;

  } // switch(marriage_selector)

  return Card::EMPTY;
} // static Card Heuristics::play_to_get_married(Trick trick, HeuristicInterface hi)



/**
 ** returns a card to play for the groom in an open marriage using common strategies
 ** for games which have rules close to the official rules
 **
 ** @param     trick   the current trick
 ** @param     hi      heuristic interface
 **
 ** @return    card to play (or Card())
 **
 ** @author    Tassilo Kirsch
 **
 ** @version   0.7.6
 **/
Card
Heuristics::play_to_marry_conventional(Trick const& trick,
                                       HeuristicInterface const& hi )
{
  // @heuristic::name   play to marry conventional
  // @heuristic::idea   play card to marry (for details, see game plan comments)

  // leave it to other heuristics if...
  if (   // we're not in an undetermined marriage
         !hi.game().undetermined_marriage()
         // common (conventional) strategies don't apply
      || !hi.game().official_rules_strategies_apply()
         // we're not a groom
      || (hi.game().soloplayer().no() == hi.no())
     )
    return Card::EMPTY;

  // we can apply strategies for undetermined marriages developed for games
  // following the official rules, so let's do it:

  Game const& game = hi.game();
  Hand const& hand = hi.hand();
  Player const& player = game.player(hi.no());

  // @ToDo these could be Card constants like Card::FOX
  Card const& DIAMOND_TEN = Card(Card::DIAMOND, Card::TEN);
  Card const& SPADE_QUEEN = Card(Card::SPADE, Card::QUEEN);

  unsigned dollen = hand.numberof(Card::DOLLE);

  // number of tricks before the marriage has to be determined
  // 0 if this is the final selector trick
  unsigned tricks_before_deadline = game.rule()(Rule::MARRIAGE_DETERMINATION) - game.trick_current_no() - 1;

  Card card_to_play = Card::EMPTY;

  if (trick.isempty()) {

    // we're leading the first trick, this is our game plan:

    // if we can win the trick,
    // * if we have one dolle, play it
    // * if we have both dollen and a fox, play the fox
    // * if we have both dollen and a trump ten, play the ten
    // * if we have both dollen and a spade queen, play the queen
    // * if we have both dollen, play a dolle
    // * play the ace with the best chance to win the trick

    // we can't win the trick,
    // * play a singleton nine or (if there is no nine) king in club or spade
    // * play a nine or (if there is no nine) a king of heart
    // * play a nine or (if there is no nine) a king of club or spade
    // * play a nine or (if there is no nine) a king of trump
    // * play lowest trump above fox that isn't a single charlie


    // we're going to marry - play smartest card
    if (dollen == 2) {
      if (!tricks_before_deadline) {
        if (hand.contains(SPADE_QUEEN))
          return SPADE_QUEEN;
        else
          return Card::DOLLE;
      }
      if (hand.contains(Card::FOX))
        return Card::FOX;
      if (hand.contains(DIAMOND_TEN))
        return DIAMOND_TEN;
      if (hand.contains(SPADE_QUEEN))
        return SPADE_QUEEN;
    }
    if (dollen)
      return Card::DOLLE;

    // see if we can marry by playing an ace

    // we can't use choose_ace because this time we don't care about double aces
    // @ToDo: since we're re-using a lot of code from choose_ace here, how 'bout a parameter?

    // the maximum number of remaining cards found so far
    unsigned max_remaining = 0;
    for (vector<Card::Color>::const_iterator
         c = game.rule().card_colors().begin();
         c != game.rule().card_colors().end();
         ++c) {
      Card::Color const& color = *c;
      Card const ace(color, Card::ACE);

      // no trump and we have an ace
      if ( (color == game.trumpcolor()) || !hand.contains(ace))
        continue;

      // the number of remaining cards for the other players
      unsigned const remaining = hi.cards_information().remaining_others(color);

      // not enough cards for all players - the ace will be ruffed
      if (remaining < game.playerno() - 1)
        continue;

      // best ace found so far?
      if ( card_to_play.is_empty()
           || (remaining > max_remaining)
           || ((remaining == max_remaining)
              && (hi.hand().numberof(ace) < hi.hand().numberof(card_to_play)))
         ) {
        max_remaining = remaining;
        card_to_play = ace;
      }
    } // for (vector<Card::Color>::const_iterator

    if (!card_to_play.is_empty())
      // play our ace and hope for the best
      return card_to_play;

    // we can't win, choose a "good" loser (see above)

    // if we have a singleton in club or spade
    bool singleton = false;

    // *** depends on sort order club-spade-heart-diamonds in card_colors ***
    for (vector<Card::Color>::const_iterator
         c = game.rule().card_colors().begin();
         c != game.rule().card_colors().end();
         ++c) {
      Card::Color const& color = *c;
      Card const& card = hand.lowest_card(color);
      if (card.is_empty() || game.less(Card(color, Card::KING), card))
        continue;

      // we have a nine or king

      if (color == Card::HEART) {
        // any heart is better than club/spade, except for a black singleton
        if (!singleton)
          card_to_play = card;
        // plain suit is better than trump, we're done
        break;
      }

      if (color == game.trumpcolor()) {
        // any plain suit is better than trump
        if (card_to_play.is_empty())
          card_to_play = card;
        // we're done
        break;
      }

      // it's club or spade, check for singletons
      bool const single = hand.numberof(color) == 1;
      if ( card_to_play.is_empty()
              // a nine is better than a king if both are singletons or non-singletons
           || ((singleton == single) && (card.value() == Card::NINE))
              // any singleton is better than a non-singleton
           || (single > singleton)
         ) {
        card_to_play = card;
        singleton = single;
      }
    } // for (vector<Card::Color>::const_iterator

    if (!card_to_play.is_empty())
      return card_to_play;

    // situation: all we have in plain suits are tens, which gives us a minimum of 8 trumps,
    // 4 of them above trump ace. Since we always risk a doppelkopf when playing a ten,
    // we'll play trump - and given the trump length, we'll keep a single charlie for later

    HandCard const& trumpace = HandCard(hand, Card::FOX);
    for( unsigned i = 0; i < hand.cardsnumber(); i++ ) {
      HandCard const& card = hand.card(i);
      if (!trumpace.less(card))
        continue;
      if (card_to_play.is_empty() || card.less(card_to_play)) {
        if ((card != Card::CHARLIE) || (hand.numberof(Card::CHARLIE) == 2))
          card_to_play = card;
      }
    }

    return card_to_play;

  } // if (trick.isempty())
  else {

    // we're not leading the trick, this is our game plan:

    // in all cases not listed below (and especially if it's the last possible trick
    // to determine the marriage), we'll try to win the trick, ignoring all aiconfig limits.
    // if we can't win the trick, we'll play the lowest card if we have to follow suit and
    // leave it to the other heuristics to find a suitable card in all other cases

    // if the bride sits behind us
    // * it's a trump trick and we have both dollen,
    //   play double dollen signal fox - trump ten - spade queen (if it wins the trick) or dolle
    // * it's a trump trick, a double dollen signal has been played and we have no dollen
    //   make shure that a) the bride has to use a high trump and b) our team doesn't waste a high trump
    // * it's a plain suit trick, we don't have to follow suit and the trick hasn't been ruffed yet,
    //   ruff with most precious trump

    // if we're sitting behind the bride who doesn't lead the trick
    // * it's a trump trick or we don't have to follow suit
    //   and the bride will win the trick
    //   and we have both dollen or a single dolle right next to the bride,
    //   play a signal for the bride to marry in trump: fox - trump ten - plain suit pfund

    // if the bride leads with a first run plain suit ace which hasn't been ruffed and we don't have to follow suit
    // * we can guarantee to become the groom if the bride marries in trump,
    //   play a signal for the bride to marry in trump: plain suit pfund
    // * we can't guarantee to become the groom if the bride marries in trump,
    //   ruff with most precious trump

    // if the bride leads with the queen only a dolle could beat
    // * we can guarantee to become the groom if the bride marries in trump,
    //   play a trump pfund

    unsigned bride_dollen = 0;

    bool right_behind_bride = game.player_following(game.soloplayer()) == player;

    // @todo  a (fast! inline?) Trick::startcolor() would be nice
    Card::Color startcolor = trick.startcard().color();

    HandCard const& winnercard = trick.winnercard();
    bool istrump = trick.startcard().istrump();
    bool followsuit = !istrump && hand.existstcolor(startcolor);
    bool ruffed = !istrump && winnercard.istrump();


    if (tricks_before_deadline) {

      if (trick.startplayer() != game.soloplayer()) {
        // a hopeful groom leads the trick
        if (trick.has_played(game.soloplayer())) {
          // we're sitting behind the bride
          if (istrump || !followsuit) {
            if (trick.winnerplayer() == game.soloplayer()) {
              // bride wins so far, can we guarantee next trump trick?
              if ( (dollen == 2)
                   || (dollen && ((winnercard == Card::DOLLE) || right_behind_bride))
                 ) {
                // make shure that the player behind us won't get the trick
                if ( trick.islastcard()
                     || (!istrump && !ruffed && (winnercard.value() == Card::ACE))
                     || !winnercard.less(hi.handofplayer(trick.lastplayer()).highest_trump())
                   ) {
                  // try to play a pfund that clearly asks the bride to marry by trump
                  if (winnercard.istrump()) {
                    if (!trick.is_jabbed(Card::FOX) && hand.contains(Card::FOX))
                      return Card::FOX;
                    if (!trick.is_jabbed(DIAMOND_TEN) && hand.contains(DIAMOND_TEN))
                      return DIAMOND_TEN;
                  }
                  if (!istrump) {
                    card_to_play = smartest_plain_suit_pfund(hi);
                    if (!card_to_play.is_empty())
                      return card_to_play;
                  }
                }
              }
            }
          }
        } // if (trick.has_played(game.soloplayer()))
        else {
          // the bride sits behind us
          if (istrump) {
            if (dollen == 2) {
              if (hand.contains(Card::FOX))
                return Card::FOX;
              if (hand.contains(DIAMOND_TEN))
                return DIAMOND_TEN;
              if (hand.contains(SPADE_QUEEN) && trick.is_jabbed(SPADE_QUEEN))
                return SPADE_QUEEN;
              return Card::DOLLE;
            }
            if (!dollen) {
              HandCard const& highest = hand.highest_trump();
              if (winnercard.less(highest)) {
                // check if a fox or a trump ten has been played for the bride
                for (unsigned c = 0; c < trick.actcardno(); ++c) {
                  HandCard const& card = trick.card(c);
                  if ((card.color() == game.trumpcolor()) && (card.value() >= Card::TEN)) {
                    // we will lose this trick because the bride is going to take it, but
                    // we want to make it as hard as possible for her to win the trick
                    // if the bride isn't in backhand, she has to play her highest trump
                    // anyway, and we don't need to do something
                    if (trick.lastplayer() == game.soloplayer()) {
                      // the bride is in backhand, check if we can do something to draw a high trump
                      // find our lowest highest trump which takes the trick so far
                      Card best = Card::EMPTY;
                      vector<Card>::const_reverse_iterator c;
                      // first search the highest trump
                      for (c = hi.game().trumps().rbegin();
                           *c != highest;
                           ++c) {}
                      // now search for a lesser trump
                      for ( ; c != hi.game().trumps().rend(); ++c) {
                        if (!winnercard.less(*c) || !game.less(Card::FOX, *c))
                          break;
                        if (hand.contains(*c))
                          best = *c;
                        if (hi.cards_information().remaining_others(*c))
                          break;
                      }
                      if (!best.is_empty()) {
                        if (trick.actcardno() == 1) {
                          // we're at position 2 and our partner-to-be is next:
                          // if we have a trump which forces the bride to play a club queen,
                          // play it to save out partner's high trumps
                          if (SPADE_QUEEN == highest) {
                            return best;
                          }
                        } else {
                          // we're at position 3:
                          // if the winning card so far is a fox or lower, play our best trump
                          if (!game.less(Card::FOX, *c))
                            return best;
                          // if we have a trump which forces the bride to play a club queen, play it
                          vector<Card>::const_iterator c;
                          // first search the winnercard
                          for (c = hi.game().trumps().begin();
                               *c != winnercard;
                               ++c) {}
                          // now search for the next higher trump the bride may have
                          for ( ; c != hi.game().trumps().end(); ++c) {
                            if (hi.cards_information().remaining_others(*c))
                              // now we know the trump the bride has to play to take the trick
                              // check if we can and have to improve it
                              if (game.less(*c, best) && game.less(*c, Card(Card::HEART, Card::QUEEN)))
                                return best;
                          }
                        }
                      } // if (!best.is_empty()) {
                    } // if (trick.lastplayer() == game.soloplayer())

                    // standard heuristics may waste a high trump here, therefore
                    return lowest_trump_card(hand);

                  } // if ((card.color() == game.trumpcolor()) && (card.value() >= Card::TEN))
                } // for (unsigned c = 0; c < trick.actcardno(); ++c)
              } // if (winnercard.less(highest))
            } // if (!dollen)
          } else if (!followsuit && !ruffed) {
            return most_precious_trump_winner(hi, trick);
          }
        }


      } // if (trick.startplayer() != game.soloplayer()) {
      else { // if (trick.startplayer() == game.soloplayer()) {

        // the bride leads the trick

        // what has happened before?
        // while being in an undetermined marriage, *all* cards are considered important (i.e., we don't forget tricks)

        // Player* groom_to_be = NULL; // see below

        for (unsigned i = 0; i <= game.trick_current_no(); ++i) {
          Trick const& t = game.trick(i);
          // check for dollen signal played by the bride
          if (    !bride_dollen
               && (t.startplayer() == game.soloplayer())
               && Card(Card::HEART, Card::QUEEN).less(t.startcard())
             )
            bride_dollen = (t.startcard() == Card::CLUB_QUEEN) ? 2 : 1;
#if 0
// just as an example how to evaluate signals and determine the groom-to-be
// unfortunately, we cannot use these results because they're not reliable
          else {
            // check if a fox or a trump ten has been played for the bride
            for (unsigned c = 0; c < t.actcardno(); ++c) {
              if (c == t.cardno_of_player(player))
                continue;
              HandCard const& card = t.card(c);
              if ( (card.player() != game.soloplayer())
                   && (!c || t.startcard().istrump() || (c > t.cardno_of_player(game.soloplayer())))
                   && ((card.color() == game.trumpcolor()) && (card.value() >= Card::TEN))
                 ) {
                groom_to_be = &card.player();
                break;
              }
              // check if a plain suit pfund has been played for the bride
              if ( !groom_to_be
                   && (c > t.cardno_of_player(game.soloplayer()))
                   && !(t.startcard().istrump() || card.istrump())
                   && (card.color() != t.startcard().color())
                   && (dollen < hi.cards_information().remaining(Card::DOLLE))
                 ) {
                groom_to_be = &card.player();
                break;
              }
            }
          }
#endif

        } // for (unsigned i = 0; i < game.trick_current_no(); ++i)

        // find lowest card to guarantee the trick
        Card nuttrump = ((dollen == 2) || (dollen && (right_behind_bride || (bride_dollen == 1))))
                        ? Card::DOLLE
                        : Card::EMPTY;

        if (dollen + bride_dollen >= hi.cards_information().remaining(Card::DOLLE)) {
          for (vector<Card::Color>::const_iterator
               c = game.rule().card_colors().begin();
               c != game.rule().card_colors().end();
               ++c) {
            Card::Color const& color = *c;
            if (color == Card::CLUB)
              continue;
            Card const& queen = Card(color, Card::QUEEN);
            unsigned remaining = hi.cards_information().remaining_others(queen);
            if (remaining == 0) {
              if (hand.contains(queen))
                nuttrump = queen;
            } else {
              if ((remaining == 1) && right_behind_bride && hand.contains(queen))
                nuttrump = queen;
              break;
            }
          }
        }

        if (!istrump) {
          if (!followsuit && (trick.startcard().value() == Card::ACE) && !hi.color_runs(startcolor)) {
            if (!ruffed) {
              if (!nuttrump.is_empty()) {
                card_to_play = smartest_plain_suit_pfund(hi);
                if (!card_to_play.is_empty())
                  return card_to_play;
              }
              return most_precious_trump_winner(hi, trick);
            }
          }
        } else {
          Card highest_queen;
          for (vector<Card::Color>::const_iterator
               c = game.rule().card_colors().begin();
               c != game.rule().card_colors().end();
               ++c) {
            Card::Color const& color = *c;
            if (color == Card::CLUB)
              continue;
            highest_queen = Card(color, Card::QUEEN);
            if (hi.cards_information().remaining(highest_queen))
              break;
          }
          if (!trick.startcard().less(highest_queen)) {
            // the bride wants to win this trick
            if (!nuttrump.is_empty())
              // I'm going to be the groom, play a pfund
              return most_precious_trump_loser(hi, trick);
          }
        }

      } // if (trick.startplayer() == game.soloplayer())
    } // if (tricks_before_deadline)

    // now, try to win the trick

    if (followsuit) {
      card_to_play = hand.highest_card(startcolor);
      if ( ruffed
           || !winnercard.less(card_to_play)
           || (trick.remainingcardno() - 1 > hi.cards_information().remaining_others(startcolor))
           || (!trick.islastcard() && game.less(card_to_play, hi.cards_information().highest_remaining_card_of_others(startcolor)))
         )
        return hand.lowest_card(startcolor); // cannot win
      else
        return card_to_play;
    }

    if (trick.islastcard())
      return most_precious_trump_winner(hi, trick);

    // find lowest card to guarantee the trick
    if (dollen + bride_dollen >= hi.cards_information().remaining(Card::DOLLE)) {
      for (vector<Card::Color>::const_iterator
           c = hi.game().rule().card_colors().begin();
           c != hi.game().rule().card_colors().end();
           ++c) {
        Card::Color const& color = *c;
        if (color == Card::CLUB)
          continue;
        Card const& queen = Card(color, Card::QUEEN);
        if (!trick.is_jabbed(queen))
          break;
        if (hand.contains(queen))
          card_to_play = queen;
        if (hi.cards_information().remaining_others(queen))
          break;
      }
    }
    if (!card_to_play.is_empty())
      return card_to_play;
    else if (dollen && trick.is_jabbed(Card::DOLLE))
      return Card::DOLLE;

    // we cannot guarantee the trick, but we are doomed to try winning it,
    // because we cannot rely on any signals (see groom_to_be above)

    HandCard highest = hand.highest_trump();
    if (winnercard.less(highest) && Card::FOX.less(highest)) {
      vector<Card>::const_reverse_iterator c;
      // first search the highest trump
      for (c = hi.game().trumps().rbegin();
           *c != highest;
           ++c) {}
      // now search for a lesser trump
      for ( ; c != hi.game().trumps().rend(); ++c) {
        if (!winnercard.less(*c) || !game.less(Card::FOX, *c))
          break;
        if (hand.contains(*c))
          highest = *c;
        if (hi.cards_information().remaining_others(*c))
          break;
      }
      return highest;
    }

    // we can neither guarantee nor win the trick, leave it to the standard heuristics
    return Card::EMPTY;

  } // if (!trick.isempty())

  return Card::EMPTY;
} // static Card Heuristics::play_to_marry_conventional(Trick trick, HeuristicInterface hi)


/**
 ** returns a card to play for the bride in an open marriage using common strategies
 ** for games which have rules close to the official rules
 **
 ** @param     trick   the current trick
 ** @param     hi      heuristic interface
 **
 ** @return    card to play (or Card())
 **
 ** @author    Tassilo Kirsch
 **
 ** @version   0.7.6
 **/
Card
Heuristics::play_to_get_married_conventional(Trick const& trick,
                                             HeuristicInterface const& hi)
{
  // @heuristic::name   play to get married conventional
  // @heuristic::idea   play card to get married (for details, see game plan comments)

  // leave it to other heuristics if...
  if (   // we're not in an undetermined marriage
         !hi.game().undetermined_marriage()
         // common (conventional) strategies don't apply
      || !hi.game().official_rules_strategies_apply()
         // we're not the bride
      || (hi.game().soloplayer().no() != hi.no())
     )
    return Card::EMPTY;

  Game const& game = hi.game();
  Hand const& hand = hi.hand();
  Player const& player = game.player(hi.no());
  unsigned dollen = hand.numberof(Card::DOLLE);
  unsigned tricks_before_deadline = game.rule()(Rule::MARRIAGE_DETERMINATION) - game.trick_current_no() - 1;

  // @ToDo these could be Card constants like Card::FOX
  Card const& DIAMOND_TEN = Card(Card::DIAMOND, Card::TEN);
  Card const& SPADE_QUEEN = Card(Card::SPADE, Card::QUEEN);

  if( trick.isempty() ) {


    // bride leads the trick, here's the game plan:

    // always:
    // if the groom played a signal for us and we have a trump loser, continue with trump
    // if we've played a dollen signal and have a trump loser, continue with trump

    // deadline (last chance to determine the marriage):
    // if we have to continue trump, play the smartest trump loser
    // if we have a trouble suit with a loser, play it
    // if we have no trump loser, play the smartest plain suit loser
    // play the most precious trump loser

    // one trick before deadline:
    // if we have to continue trump, play smartest trump winner
    // if we have a trouble suit with an ace, play the ace
    // if we have no trouble suits and a trump loser, play dollen signal and continue trump
    // if we have a strong ace, play it
    // continue as if it was deadline

    // two tricks before deadline:
    // if we have to continue trump, play smartest trump winner
    // if we have a strong ace, play it
    // if we have no trouble suits and a trump loser, play dollen signal and continue trump
    // continue as if it was one trick before deadline

    // three or more tricks before deadline:
    // if we have a strong ace, play it
    // continue as if it was one trick less before deadline

    // remarks:
    // * trouble suits: 4 cards in club/spade, 5 cards in club/spade, 3 cards in heart
    //   this means trouble if we marry someone who has 2 or more cards of the suit
    // * dollen signal: club queen means both dollen, spade queen or dolle means one dolle
    // * strong ace: single ace as single- or doubleton in club or spade
    // * smartest trump winner: lowest queen below club queen which will take the trick
    //   (we can ignore dollen) or a dollen signal if we haven't played one yet.

    bool trump_loser = hand.lowest_trump().less(hi.cards_information().highest_remaining_trump_of_others());

    bool continue_with_trump = false;
    unsigned bride_showed_dollen = 0;
    unsigned partner_has_signalled = 0;
    unsigned initial_dollen = hand.numberofall(Card::DOLLE);

    int initial_spadequeens = hand.numberofall(SPADE_QUEEN);
    int spadequeens_played = 0;
    unsigned dollen_played = 0;

    for (unsigned i = 0; i < game.trick_current_no(); ++i) {
      Trick const& t = game.trick(i);
      unsigned brideno = t.cardno_of_player(player);
      for (unsigned c = 0; c < t.actcardno(); ++c) {
        HandCard const& card = t.card(c);

        if (c == brideno) {
          // check if we played a dollen signal
          if (!c && Card(Card::HEART, Card::QUEEN).less(card)) {
            if (!bride_showed_dollen)
              bride_showed_dollen = (card == Card::CLUB_QUEEN) ? 2 : 1;
          }
          if (card == Card::DOLLE)
            ++dollen_played;
        } else if (trump_loser && !partner_has_signalled) {
          if (   (t.startcard().istrump() || (c > brideno))
                     // check if a fox or a trump ten has been played for us
              && (   ((card.color() == game.trumpcolor()) && (card.value() >= Card::TEN))
                  || // check if a plain suit pfund has been played for us
                     (!(t.startcard().istrump() || card.istrump()) && (card.color() != t.startcard().color()))
                 )
             )
          {
            // check if the signal is plausible
            unsigned crucial_trumps_on_hand = initial_dollen - max(bride_showed_dollen, dollen_played);
            if (initial_dollen && !crucial_trumps_on_hand)
              crucial_trumps_on_hand = max(0, initial_spadequeens - spadequeens_played);

            if (   !crucial_trumps_on_hand
                || ((crucial_trumps_on_hand == 1) && (c == brideno + 1))
               ) {
              // a plausible signal has been played
              partner_has_signalled = (c == brideno + 1) ? 1 : 2;
            }
          }
        }
        if (card == SPADE_QUEEN)
          ++spadequeens_played;
      } // if (c == brideno)
    } // for (unsigned i = 0; i < game.trick_current_no(); ++i)

    if (!game.isvirtual())
      cout << "*marriage signals: bride=" << bride_showed_dollen << " groom=" << partner_has_signalled << endl;

    continue_with_trump = trump_loser && (bride_showed_dollen || partner_has_signalled);
    unsigned trumps = hand.numberoftrumps();
    // find card to prolong the announcement period
    // make shure that it can't be mistaken for a dollen signal
    Card trump_to_prolong = Card::EMPTY;
    if (    tricks_before_deadline
        && (trumps >= hand.cardsnumber() / (3 - trump_loser))
        && (   !trump_loser
            || dollen
            || bride_showed_dollen
           )
       )
    {
      if (   dollen
          && (   (trump_loser && bride_showed_dollen)
              || (!trump_loser && !dollen_played)
             )
         )
        trump_to_prolong = Card::DOLLE;
      for (vector<Card::Color>::const_iterator
           c = game.rule().card_colors().begin();
           c != game.rule().card_colors().end();
           ++c) {
        Card::Color const& color = *c;
        Card const& queen = Card(color, Card::QUEEN);
        if (hand.contains(queen)) {
          if (   bride_showed_dollen
              || (color < Card::SPADE)
              || ((color == Card::SPADE) && !trump_loser)
             )
            trump_to_prolong = queen;
        }
        if (color == Card::CLUB)
          // it's a marriage, stupid ;-)
          continue;
        if (!trump_loser && !hi.cards_information().remaining_others(queen))
          // we don't have a trump loser, find the actually lowest suitable queen
          continue;
        if (hi.cards_information().remaining(queen) > 1)
          // we will marry in trump, find the lowest queen our partner-to-be can identify
          break;
      }
    } // if (prolonging is allowed)


    // check if we want to signal our trump strength
    Card dollen_signal = Card::EMPTY;
    if (    tricks_before_deadline
         && dollen // d'oh
         && !bride_showed_dollen // signal already given
         && trump_loser // signal only if we're going to marry by trump
         && (trumps > 4) // *value*
       )
    {
      if (initial_dollen == 2)
        dollen_signal = Card::CLUB_QUEEN;
      else if (hand.contains(SPADE_QUEEN))
        dollen_signal = SPADE_QUEEN;
      else
        dollen_signal = Card::DOLLE;
    }

    if (!continue_with_trump) {

      // check if we have strong aces
      Card ace_to_play = Card::EMPTY;
      unsigned aces_to_play = 0;
      unsigned shortest = 3;
      for (vector<Card::Color>::const_iterator
           c = game.rule().card_colors().begin();
           *c != Card::HEART; // we only need to check club and spade
           ++c) {
        Card::Color const& color = *c;
        // first run only
        if (hi.color_runs(color))
          continue;
        Card const ace(color, Card::ACE);
        unsigned len = hand.numberof(color);
        if ((hand.numberof(ace) == 1) && (len < 3)) {
          aces_to_play++;
          if (len < shortest) {
            ace_to_play = ace;
            shortest = len;
          }
        }
      }

      // check if there's a suit with trouble
      Card::Color trouble_suit = Card::NOCARDCOLOR;
      bool trouble_suit_has_ace = false;
      bool trouble_suit_has_loser = false;
      for (vector<Card::Color>::const_iterator
           c = game.rule().card_colors().begin();
           *c != Card::DIAMOND;
           ++c) {
        Card::Color const& color = *c;
        // first run only
        if (hi.color_runs(color))
          continue;
        unsigned len = hand.numberof(color);
        if ((len == 5) || (len == 4) || ((len == 3) && (color == Card::HEART)) ) {
          bool has_ace = hand.contains(Card(color, Card::ACE));
          // make shure we won't try to find our partner in heart if we can try a black color instead
          // only necessary if we're going to marry before the last possible trick
          if ( (color == Card::HEART)
               && (trouble_suit != Card::NOCARDCOLOR)
               && tricks_before_deadline
               && !has_ace
             )
            break;
          trouble_suit_has_loser = hand.lowest_card(color).less(hi.cards_information().highest_remaining_card_of_others(color));
          trouble_suit_has_ace = has_ace;
          trouble_suit = color;
          if ((tricks_before_deadline && trouble_suit_has_ace)
              || (!tricks_before_deadline && trouble_suit_has_loser))
            break;
        }
      }

      // don't play a dollen signal if there's a trouble suit or a singleton ace
      // @todo fine-tune when to play an ace and when to play a dollen signal instead, based on trump length, suit control and handvalue (Essener)
      if (   (trouble_suit != Card::NOCARDCOLOR)
          || (   !ace_to_play.is_empty()
              && (   (shortest == 1)
                  || (dollen_signal == Card::DOLLE)
                 )
             )
         )
        dollen_signal = Card::EMPTY;

      // now it's time to play ;-)

      // without a trump loser, we may want to prolong the announcement period
      if (!trump_loser && !trump_to_prolong.is_empty()) {
        if (tricks_before_deadline > trouble_suit_has_ace + aces_to_play)
          return trump_to_prolong;
      }

      // see if there are enough tricks left to play a strong ace
      if (!ace_to_play.is_empty()) {
        if (tricks_before_deadline > (!dollen_signal.is_empty()) + trouble_suit_has_ace)
           return ace_to_play;
      }

      // see if it's time to signal our dollen
      if (!dollen_signal.is_empty())
        return dollen_signal;

      // try to clear a trouble suit by playing its ace
      if (trouble_suit_has_ace) {
        if (tricks_before_deadline)
          return Card(trouble_suit, Card::ACE);
      }

      // do we have to determine the marriage in plain suit?
      if (trouble_suit_has_loser)
        return highest_loser(hi, trouble_suit);
      if (!trump_loser)
        return smartest_plain_suit_pfund(hi, true);

    } // if (!continue_with_trump)

    // see if it's time to signal our dollen
    if (!dollen_signal.is_empty())
      return dollen_signal;

    // try to prolong the announcement period
    if (!trump_to_prolong.is_empty())
      return trump_to_prolong;

    // now let's find our partner in trump
    return most_precious_trump_loser(hi, trick);

  } // if (trick.isempty())

  // trick is led by someone else

  // game plan:

  // all cases
  // * if it's the last chance to determine the marriage, play a pfund
  // * if we can't take the trick, play a pfund

  // trump trick
  // * if a fox or a trump ten has been played, play lowest guaranteed winner (excluding dollen)
  // * if a spade queen has been played and we have no dollen, play a pfund
  // * if we have guaranteed winners (including dollen), play the lowest one
  // * play a pfund
  //   rationale: now it's most likely that the players behind us have the dollen and since they
  //   can't risk not taking the trick, let's not waste a high trump

  // plain suit trick
  // * if we have to follow suit, play highest card
  // * if we're the first to ruff, play most precious winner
  // * if we're the last to ruff, take the trick
  // * if we're the second to ruff and the startcard wasn't an ace and we have a guaranteed winner
  //   (including dollen), play the lowest one
  // * overruff using the lowest winner, including a club queen

  HandCards const ha = hand.validcards(trick);
  bool cannot_win = !trick.winnercard().less(ha.highest_card());


  // find lowest guaranteed winner (excluding dollen)
  Card lowest_guaranteed_winner = Card::EMPTY;
  for (vector<Card::Color>::const_iterator
       c = game.rule().card_colors().begin();
       c != game.rule().card_colors().end();
       ++c) {
    Card::Color const& color = *c;
    Card const& queen = Card(color, Card::QUEEN);
    if (trick.is_jabbed(queen)) {
      if (hand.contains(queen))
        lowest_guaranteed_winner = queen;
      if (!hi.cards_information().remaining_others(queen))
        continue;
    }
    break;
  }


  if (trick.startcard().istrump()) {
    if (!tricks_before_deadline || cannot_win)
      return most_precious_trump_loser(hi, trick);

    // check if a double dollen signal (fox, trump ten or spade queen) has been played
    if (!dollen) { // only if the signal is plausible
      for (unsigned i = 0; i < trick.actcardno(); i++ ) {
        if ((trick.card(i).color() == game.trumpcolor()) && (trick.card(i).value() >= Card::TEN))
          return lowest_guaranteed_winner;
      }
      if (trick.winnercard() == SPADE_QUEEN)
        return most_precious_trump_loser(hi, trick);
    }

    // play a guaranteed winner
    if (trick.islastcard())
      return most_precious_trump_winner(hi, trick);
    else if (dollen) {
      if (dollen == 2)
        return lowest_guaranteed_winner;
      else
        return Card::DOLLE;
    }

    // we have both dollen sitting behind us
    if (trick.actcardno() == 1) {
      // in second position, we play a pfund to avoid wasting high trumps
      return most_precious_trump_loser(hi, trick);
    } else {
      // in third position, we take the trick with the lowest possible trump,
      // thus allowing our partner-to-be to play a pfund
      return most_precious_trump_winner(hi, trick);
    }

  } // if (trick.startcard().istrump())
  else {
    if (ha.existstcolor(trick.startcard().color())) {
      // we have to follow suit
      if (!tricks_before_deadline)
        return highest_loser(hi, trick.startcard().color());
      else
        return ha.highest_card();
    }

    // we don't have to follow suit
    if (!tricks_before_deadline) {
      Card const& loser = smartest_plain_suit_pfund(hi);
      if (loser.is_empty())
        return most_precious_trump_loser(hi, trick, (PT_SPARE_CHARLIE | PT_FORCE_LOSING));
      else
        return loser;
    }

    // we can't win the trick
    if (cannot_win) {
      if (ha.contains(Card::FOX))
        return Card::FOX;
      Card loser = smartest_plain_suit_pfund(hi, false, false, true);
      if (loser.is_empty()) {
        if (ha.contains(DIAMOND_TEN))
          return DIAMOND_TEN;
        else
          loser = smartest_plain_suit_pfund(hi);
      }
      if (loser.is_empty())
        return most_precious_trump_loser(hi, trick);
      else
        return loser;
    }

    if (trick.winnercard().istrump() && !trick.islastcard() && (trick.startcard().value() != Card::ACE)) {
      // we're in position 3, try to secure the trick
      if (dollen) {
        if (dollen == 2)
          return lowest_guaranteed_winner;
        else
          return Card::DOLLE;
      }
    }

    // take the trick so far
    return most_precious_trump_winner(hi, trick);
  }
  return Card::EMPTY;
} // static Card Heuristics::play_to_get_married_conventional(Trick trick, HeuristicInterface hi)

