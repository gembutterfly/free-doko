/*********************************************************************
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

#include "HeuristicInterface.h"
#include "cards_information.h"
#include "cards_information.of_player.h"
#include "team_information.h"
#include "../../game/game.h"
#include "../../party/party.h"
#include "../../party/rule.h"
#include "../../card/trick.h"

#ifdef BENDERS
#ifndef RELEASE
// whether to print some weighting informations

#define ANNOUNCE_DEBUG
//#define ANNOUNCE_DEBUG_DETAIL  //CalcHandValue
#endif
#endif
/*
 * Steps to create a new heuristic
 *
 *  1) src/player/aiconfig.h
 *     - enum Heuristic: add heuristic
 *  2) src/player/aiconfig.cpp
 *     - function name(): add heuristic
 *     - Aiconfig::init_heuristic_states(): add heuristic to default ai
 *  3) src/player/ai/heuristic.h
 *     - add declaration of heuristic function
 *  4) src/player/ai/ai.cpp
 *     - Ai::nextcard(trick): add heuristic and function call in switch
 *  5) src/ui/gtkmm/players_db.cpp
 *     - PlayersDB::recreate_db(): add heuristic to switch (mind the group)
 *  6) src/player/ai/heuristic.cpp
 *     - add definition of heuristic (here is the work)
 *     - do not forget the documentation (see below)
 *  7) data/translations/de/aiconfig.heuristics.description
 *     - add german translation for heuristic name and description
 *  8) src/player/ai/aiconfigs.preset
 *     - create presets:
 *       a) save standard ai under 'aiconfig.defensive.saved'
 *       b) call 'make'
 *  9) test
 * 10) create references (remember: set the ai to standard types)
 *
 * Mostly the compiler will find an error.
 * To find the places in the files, just search for 'CREATE_FEHL' and 'create_fehl'.
 */

/* Documentation
 *
 * Each heuristic should contain the following documentation for automatic
 * generation of the description.
 * Note that the text shall not be split into multiple lines.
 * The name must be the first entry and the action the last.
 * For a real example look at 'Heuristics::serve_color_trick()'.
 */
// @heuristic::name        the name of the heuristic (as in '::name(Heuristic)')
// @heuristic::idea        the main idea for the heuristic
// @heuristic::comment     a simple comment
// @heuristic::condition   a condition for the heuristic
// @heuristic::condition   a second condition for the heuristic
// @heuristic::action      which card is taken



// missing

// @heuristic::name   choose pfund poverty
// @heuristic::idea   ???

// @heuristic::name   meatless: play highest color
// @heuristic::idea   ???

// @heuristic::name   picture: second best trump
// @heuristic::idea   ???

// @heuristic::name   color: choose ace
// @heuristic::idea   ???

// @heuristic::name   color: best winning
// @heuristic::idea   ???

// @heuristic::name   color: jab for ace
// @heuristic::idea   ???

// @heuristic::name   color: play low high
// @heuristic::idea   ???




// special heuristics

// @heuristic::name      no heuristic
// @heuristic::idea      internal use: no heuristic was used, yet

// @heuristic::name      manual
// @heuristic::idea      The card is played manual (from the human).
//
// @heuristic::name      bug report
// @heuristic::idea      Play the card from the bug report.
// @heuristic::comment   This heuristic is used so that in a bug report replay
// @heuristic::comment   the card suggestion is the card from the bug report.

// @heuristic::name      network
// @heuristic::idea      The card is played by the network player.

// @heuristic::name      only valid card
// @heuristic::idea      If there is only one valid card, play it.


/*
   Ideen

   normales Spiel
 * Partner sitzt hinten:
 Farbe anspielen, die er nicht hat und von der noch mindestens 1 draußen ist
 * Startspieler aber kein Farb-As:
Re: Karo König, Karo Neun, Karo Bube (damit Partner mit Kreuz-Dame rausrückt)
Kontra: Lange Farbe anspielen (>= 4)
 * Partner sitzt hinter einem, beide Gegner vorne (Trumpfstich):
 - Pfunden (wenn Fuchs / Karo Zehn da)
 - rübergehen, damit Partner Sorgen los wird
 * Wenn Partner direkt nach mir sitzt und noch nicht dran war und kein As gespielt hat:
 Spiele Farbe an, damit er mit potentiellen As übernehmen kann

 Armut (z.B. seed 27350 (3) )
 * Erster Kontra Spieler nach Re:
 Wenn Partner noch übertrumpfen kann (trump limit?): Punkte/drunter bleiben.
 (damit er hinten sitzt)
 wenn Partner nicht übertrumpfen kann (-> cards information): selber übertrumpfen oder kleinen rein.
 * Armutler nach Partner:
 Gegner haben Stich: wenig Punkte geben
 */

// ToDo: own trumplimit value for poverty

unsigned number_of_no_trumps_per_color();
Card lowest_card(Hand const& hand, Card::Value const cv,
                 HeuristicInterface const& hi);
Card lowest_jack(Hand const& hand, HeuristicInterface const& hi);
Card lowest_queen(Hand const& hand, HeuristicInterface const& hi);
Card highest_queen(Hand const& hand, HeuristicInterface const& hi);
Card highest_card(Hand const& hand, Card::Value const cv,
                  HeuristicInterface const& hi);
Card highest_trump_card( Trick const& t, Hand h );
bool better_points_optimize(HandCard const& old_card,
                            HandCard const& test_card,
                            HeuristicInterface const& hi);

// Checks if the card will win the trick
bool cardWillWinTheTrick(Trick const &t, HeuristicInterface const& hi,
                         HandCard const& card);
unsigned calcPointsOfOwnTeam( HeuristicInterface const& hi,
                             bool const with_current_trick = true );
unsigned calcPointsOfOppositeTeam( HeuristicInterface const& hi, 
                                  bool const with_current_trick = true );

// whether all players behind are of the own team
bool checkAllMyTeam(Trick const& t, HeuristicInterface const& hi);
// whether all players behind are of the opposite team
bool checkAllOppositeTeam(Trick const& t, HeuristicInterface const& hi);
// whether the opposite team behind still has a trump
bool opposite_trump_behind(Trick const& trick, HeuristicInterface const& hi);
// the highest card of the playes of the opposite team behind
HandCard const& highest_card_behind_of_opposite_team(Trick const& trick,
                                                     HeuristicInterface const& hi);
// the valid cards of the playes of the opposite team behind
HandCards valid_cards_behind_of_opposite_team(Trick const& trick,
                                              HeuristicInterface const& hi);
// the highest card of the playes of the same team behind
HandCard const& highest_card_behind_of_same_team(Trick const& trick,
                                                 HeuristicInterface const& hi);
// the lowest of the highest trumps
HandCard lowest_highest_trump(Trick const& trick, HeuristicInterface const& hi);
// the highest trump of the playes of the opposite team
HandCard const& highest_trump_of_opposite_team(HeuristicInterface const& hi);
// the number of tricks the team will get
unsigned secure_tricks_of_team(HeuristicInterface const& hi);

Card choose_best_fehl(Trick const& trick, HeuristicInterface const& hi,
                      bool const forceFehl = false);

/**
 ** check if own team can still win this trick.
 **
 ** @param      trick   the current trick
 ** @param      hi      heuristic interface
 **
 ** @return     returns true if the own team can win the trick
 **
 ** @author     Borg Enders
 ** @author     Diether Knof
 **
 ** @version    0.7.9
 **/
bool
ownTeamCanWinTrick( Trick const &t, HeuristicInterface const& hi )
{
  if (t.winnerplayer().team() == hi.team())
    return true;

  // take a look if all players coming in this trick of own team can still win the trick
  for( unsigned i = t.actcardno(); i < hi.game().playerno(); i++ ) {
    if (   !hi.same_player(t.player_of_card(i))
        && ( maybe_to_team(hi.teamofplayer(t.player_of_card( i ))) == hi.team() ) )
    {
      if (hi.handofplayer(t.player_of_card( i )
                         ).validcards(t).higher_card_exists( t.winnercard() ))
        return true;
    } // if( maybe_to_team(hi.teamofplayer(t.player_of_card(i))) != hi.team() )
  } // for (i)

  return false;
} // bool ownTeamCanWinTrick ( Trick t, HeuristicInterface hi )

/**
 ** check if opposite team can still win this trick.
 **
 ** @param      trick   the current trick
 ** @param      hi      heuristic interface
 **
 ** @return     returns true if the opposite team can win the trick
 **
 ** @author     Borg Enders
 **
 ** @version    ???
 **
 ** @todo      use highest_opposite_card_behind for oppositeTeamCanWinTrick
 **/
bool
oppositeTeamCanWinTrick( Trick const &t, HeuristicInterface const& hi )
{
  if (!hi.guessed_same_team(t.winnerplayer()))
    return true;

  // take a look if all players coming in this trick of other team can still win the trick
  for( unsigned i = t.actcardno(); i < hi.game().playerno(); i++ )
    if (   !hi.guessed_same_team(t.player_of_card(i))
        && hi.handofplayer(t.player_of_card(i)
                          ).higher_card_exists( t.winnercard() ))
      return true;

  return false;
} // bool oppositeTeamCanWinTrick ( Trick t, HeuristicInterface hi )

/**
 ** check if the card will win the trick
 **
 ** @param      trick   the current trick
 ** @param      hi      heuristic interface
 ** @param      card    the card to check
 **
 ** @return     returns true if the card will win the trick
 **
 ** @author     Florian Sundermann
 **
 ** @version    0.7.5
 **/
bool
cardWillWinTheTrick(Trick const &t, HeuristicInterface const& hi,
                    HandCard const& card)
{
  return card.less(highest_card_behind_of_opposite_team(t, hi));

#ifdef OUTDATED
  // dknof, 2013-09-11
  bool cannotwin = !t.winnercard().less(card);

  // take a look if all players coming in this trick of other team can still win the trick
  for( unsigned i = t.actcardno(); i < hi.game().playerno(); i++ )
    if( maybe_to_team(hi.teamofplayer(t.player_of_card( i ))) != hi.teamofplayer(card.player()) )
    {
      if ( t.cardno_of_player( card.player() ) < i ) // Test for dolle etc.
        cannotwin = cannotwin || card.less(hi.handofplayer(i).highest_card());
      else
        cannotwin = cannotwin || !hi.handofplayer(i).highest_card().less(card);
    } // if( maybe_to_team(hi.teamofplayer(t.player_of_card(i))) != hi.team() )
  return ! cannotwin;
#endif
} // bool oppositeTeamHasBetterCardForTrick ( Trick t, HeuristicInterface hi )


/**
 ** get the number of "no-trump" cards per color
 **
 ** @param     trick   the current trick
 ** @param     hi      heuristic interface
 **
 ** @return    number of cards per miscolor
 **
 ** @author    Borg Enders
 **
 ** @version   ???
 **/
unsigned
number_of_no_trumps_per_color()
{
  unsigned i;
  unsigned notrump_per_color=0;

  // calculate number of no trump cards per color
  // only one color can be trump
  // if spade is trump caculate with club otherwise with spade
  if( Card( Card::SPADE, Card::ACE ).istrump( ::party.game() ) )
  {
    for ( i = 0;  i < 2 * ::party.rule()(Rule::NUMBER_OF_CARD_VALUES); i += 2 )
      if( !Card( Card::InttoColor(Card::CLUB), Card::InttoValue(i) ).istrump(
                                                                             ::party.game() )
        )
        notrump_per_color+=2;
  } else { // if( Card( Card::SPADE, Card::ACE ).istrump( ::party.game() ) )
    for ( i = 0;  i < 2 * ::party.rule()(Rule::NUMBER_OF_CARD_VALUES); i += 2 )
      if( !Card( Card::InttoColor( Card::SPADE ),
                Card::InttoValue(i) ).istrump( ::party.game() )
        )
        notrump_per_color+=2;
  } // else

  return notrump_per_color;
} // unsigned number_of_no_trumps_per_color()


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
        && !hi.is_soloplayer()
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

      if (!trump.less(highest_card_behind_of_opposite_team(trick, hi))) {
        if (   (trump == Card::DOLLE)
            && !hi.cards_information().remaining_others(Card::DOLLE)
            && hi.hand().contains(Card::SPADE, Card::QUEEN))
          return Card(Card::SPADE, Card::QUEEN);
        else
          return trump;
      }

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
        && !trump.less(highest_card_behind_of_opposite_team(trick, hi)))
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

  // @heuristic::condition   marriage player
  if( !(   hi.game().type() == GAMETYPE::MARRIAGE
        && hi.is_soloplayer()
       )
    )
    return Card::EMPTY;

  // @heuristic::action   As first player, play card of the marriage color to get married,
  // @heuristic::action   else in a marriage trick, play pfund.
  Card::Color color = Card::NOCARDCOLOR;

  switch( hi.game().marriage_selector() ) {
  case MARRIAGE_SELECTOR::SILENT:
  case MARRIAGE_SELECTOR::TEAM_SET:
    return Card::EMPTY;

  case MARRIAGE_SELECTOR::FIRST_FOREIGN:
    if (trick.isempty()) {
      vector<Card> cards;
      for (vector<Card::Color>::const_iterator
           c = hi.game().rule().card_colors().begin();
           c != hi.game().rule().card_colors().end();
           ++c) {
        Card::Color const& color = *c;
        if (   (hi.hand().numberof( color ) < 3)
            && !HandCard( hi.hand(), Card(color, Card::ACE)).possible_swine()
            && hi.game().trick_current_no() < hi.game().rule()(Rule::MARRIAGE_DETERMINATION) - 1)
          cards.push_back(Card(color, Card::ACE));
      } // foreach ( color in hi.game().rule().card_colors() )
      if ( !HandCard( hi.hand(), Card(hi.game().trumpcolor(), Card::ACE)).possible_swine() )
        cards.push_back( Card(hi.game().trumpcolor(), Card::ACE ));
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
    return Heuristics::choose_pfund_card( trick, hi );

  case MARRIAGE_SELECTOR::FIRST_TRUMP:
    if(   !trick.isempty()
       && trick.startcard().istrump() )
      return Heuristics::choose_pfund_card( trick, hi );

    if( trick.isempty() )
    {
      // take a 'good' card
      if(   hi.hand().numberoftrumpaces() > 0
         && !hi.hand().has_swines()
        )
        return Card(hi.game().trumpcolor(), Card::ACE);

      if(  hi.hand().numberof( Card( hi.game().trumpcolor(), Card::TEN ) ) > 0 )
        return Card(hi.game().trumpcolor(), Card::TEN);

      return Heuristics::lowest_best_trump_card(trick, hi.hand(), hi);
    } // if (trick.isempty())
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
      return Heuristics::choose_pfund_card( trick, hi );

    if( trick.isempty() )
    {
      vector<Card> cards;
      if (!HandCard(hi.hand(), Card(color, Card::TEN)).is_special())
        cards.push_back(Card(color, Card::TEN));
      if (!trick.isjabbed(Card(color, Card::ACE)))
        cards.push_back(Card(color, Card::ACE));
      cards.push_back(Card(color, Card::KING));
      cards.push_back(Card(color, Card::NINE));
      cards.push_back(Card(color, Card::ACE)); // With Ace I will probably win the trick bad decision
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

    if(    hi.same_player(trick.startplayer())
       && !trick.startcard().istrump()
      )
      return Heuristics::choose_pfund_card( trick, hi );

    if( hi.same_player(trick.startplayer()) )
      return Heuristics::lowest_card(trick, hi.hand());

    break;

  } // switch(marriage_selector)

  return Card::EMPTY;
} // static Card Heuristics::play_to_get_married(Trick trick, HeuristicInterface hi)


/**
 ** For a first card in a trick play a color ace
 ** if there are still enough cards of this color in the game
 ** and the color has not been jabbed
 ** choose the ace with the most cards in that color still in the game
 ** If not the first player, take a color ace if the trick can get through.
 **
 ** @param     trick   current trick
 ** @param     hi      heuristic interface
 **
 ** @return    card to play (color ace or none)
 **
 ** @author    Diether Knof
 ** @author    Borg Enders
 **
 ** @version   0.7.4
 **/
Card
Heuristics::choose_ace(Trick const& trick, HeuristicInterface const& hi)
{
  // @heuristic::name   choose ace
  // @heuristic::idea   For the first card in a trick choose the best color Ace to play. For a following card in a color trick choose the ace, if the trick can get through.

  if (trick.isstartcard()) {
    // I am the start player

    // the number of remaining cards for the color
    map<Card::Color, unsigned> number_of_remaining_cards;

    // the card to play
    Card ace_to_play;
    // the maximal number of remaining cards found so far
    unsigned best_modified_remaining_cards = 0;
    for (vector<Card::Color>::const_iterator
         c = hi.game().rule().card_colors().begin();
         c != hi.game().rule().card_colors().end();
         ++c) {
      Card::Color const& color = *c;
      Card const ace(color, Card::ACE);

      // test that the color is no trumpcolor
      if ( color == hi.game().trumpcolor())
        continue;
      // test that the player has an ace of the color
      if (!hi.hand().contains(ace))
        continue;

      { // each player behind not of the same team has the color
        unsigned i;
        for (i = trick.actcardno() + 1; i < hi.game().playerno(); ++i)
          if (   (hi.teamofplayer(trick.player_of_card(i)) != hi.team())
              && !hi.handofplayer(trick.player_of_card(i)).contains(color)) {
            break;
          }
        if (i < hi.game().playerno())
          continue;
      } // each player behind has the color

      // the number of remaining cards for the other players
      unsigned const remaining_cards
        = (hi.game().numberof(color)
           - (hi.cards_information().played(*c) + hi.hand().numberof(*c)));
      // modified remaining cards
      // Modified by the number of aces of the color.
      // This is used to prefer single aces to double aces.
      unsigned const modified_remaining_cards
        = (remaining_cards
           +  (  (hi.hand().numberof(ace) + hi.cards_information().played(ace)
                  < hi.game().rule()(Rule::NUMBER_OF_SAME_CARDS))
               ? (hi.game().numberof(color) - 1) // *Value*
               : 0) );

      // there are enough cards for all players
      if (remaining_cards < hi.game().playerno() - 1) {
        // check whether the opposite team must have the color
        bool opposite_team_must_serve = true;
        for (unsigned i = trick.actcardno() + 1; i < hi.game().playerno(); ++i) {
          if (   (hi.teamofplayer(trick.player_of_card(i)) != hi.team())
              && !hi.cards_information().of_player(i).must_have(color)) {
            opposite_team_must_serve = false;
            break;
          }
        }
        if (!opposite_team_must_serve)
          continue;
      }

      // if the color has already been jabbed, it has to be from the own team
      if (   hi.colorjabbed(color)
          && (   !hi.jabbedbyownteam(color)
              || hi.jabbedbyotherteam(color)))
        continue;

      // no ace found so far
      if (   !ace_to_play.is_empty()
          // there must not be less remaining cards then the best
          && (   (modified_remaining_cards < best_modified_remaining_cards)
              // if both colors have the same cards out,
              // take the one the player has fewer aces
              || (  (modified_remaining_cards
                     == best_modified_remaining_cards)
                  ? (hi.hand().numberof(ace)
                     >= hi.hand().numberof(ace_to_play))
                  : false)
             ))
        continue;

      best_modified_remaining_cards = modified_remaining_cards;
      ace_to_play = ace;
    } // for (c)

    return ace_to_play;
  } // if (trick.isstartcard())

  if( !trick.isstartcard() ) {
    // I am not the start player

    // check for pure color trick
    if (trick.winnercard().istrump())
      return Card::EMPTY;

    // the winnercard is no ace
    if (trick.winnercard().value() == Card::ACE)
      return Card::EMPTY;

    // the player has the ace of the color
    Card::Color const color = trick.startcard().color();
    if (   !hi.hand().contains(Card(color, Card::ACE))
        || Card(color, Card::ACE).istrump(hi.game())
        || !trick.isvalid(Card(color, Card::ACE), hi.hand()))
      return Card::EMPTY;

    // there are enough remaining color cards, so the trick can go through
    if (hi.game().numberof(color)
        - hi.cards_information().played(color)
        - hi.hand().numberof(color)
        < trick.remainingcardno() - 1)
      return Card::EMPTY;

    // each player behind of the opposite team has the color
    for (unsigned c = trick.actcardno() + 1; c < hi.game().playerno(); ++c)
      if (   (hi.teamofplayer(trick.player_of_card(c)) != hi.team())
          && !hi.handofplayer(trick.player_of_card(c)).contains(color))
        return Card::EMPTY;

    // in solo games: check whether a ten is sufficient
    if (   hi.game().is_solo()
        && !hi.is_soloplayer()
        && (Heuristics::choose_ten(trick,hi) == Card(color, Card::TEN))
        && (hi.cards_information().of_player(hi.game().soloplayer()).must_have(color)
            + (trick.has_played(hi.game().soloplayer()) ? 1 : 0))
        > hi.hand().numberof(Card(color, Card::ACE)))
      return Card(color, Card::TEN);

    return Card(color, Card::ACE);
  } // if( !trick.isstartcard() )

  return Card::EMPTY;
} // Card Heuristics::choose_ace(Trick trick, HeuristicInterface hi)

/**
 ** For a first card in a trick play a color ten
 ** if there are still enough cards of this color in the game
 ** and the color has not been jabbed and the ten is the highest card
 ** choose the ten with the most cards in that color still in the game
 ** If not the first player, take a color ten if the trick can get through.
 **
 ** @param     trick   current trick
 ** @param     hi      heuristic interface
 **
 ** @return    card to play (color ten or none)
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.9
 **/
Card
Heuristics::choose_ten(Trick const& trick, HeuristicInterface const& hi)
{
  // @heuristic::name   choose ten
  // @heuristic::idea   For the first card in a trick choose the best color ten to play. For a following card in a color trick choose the ten, if the trick can get through.

  if (trick.isstartcard()) {

    // the number of remaining cards for the color
    map<Card::Color, unsigned> number_of_remaining_cards;

    // the card to play
    Card ten_to_play;
    // the maximal number of remaining cards found so far
    unsigned best_modified_remaining_cards = 0;
    for (vector<Card::Color>::const_iterator
         c = hi.game().rule().card_colors().begin();
         c != hi.game().rule().card_colors().end();
         ++c) {
      Card::Color const& color = *c;
      Card const ten(color, Card::TEN);

      // there are no aces remaining of the color
      if (hi.cards_information().remaining(Card(color, Card::ACE)))
        continue;

      // test that the color is no trumpcolor
      if (   (color != hi.game().trumpcolor())
          // test that the player has an ten of the color
          && hi.hand().contains(ten)) {

        { // each player behind has the color
          unsigned i;
          for (i = trick.actcardno() + 1; i < hi.game().playerno(); ++i)
            if (!hi.handofplayer(trick.player_of_card(i)).contains(color)) {
              break;
            }
          if (i < hi.game().playerno())
            continue;
        } // each player behind has the color

        // the number of remaining cards for the other players
        unsigned const remaining_cards
          = (hi.game().numberof(color)
             - (hi.cards_information().played(*c) + hi.hand().numberof(*c)));
        // modified remaining cards
        // Modified by the number of tens of the color.
        // This is used to prefer single tens to double tens.
        unsigned const modified_remaining_cards
          = (remaining_cards
             +  (  (hi.hand().numberof(ten) + hi.cards_information().played(ten)
                    < hi.game().rule()(Rule::NUMBER_OF_SAME_CARDS))
                 ? (hi.game().numberof(color) - 1) // *Value*
                 : 0) );

        // there are enough cards for all players
        if (   (remaining_cards >= hi.game().playerno() - 1)
            // if the color has already been jabbed, it has to be from the own team
            && (   !hi.colorjabbed(color)
                || (   hi.jabbedbyownteam(color))
                && !hi.jabbedbyotherteam(color))
            // no ten found so far
            && (   ten_to_play.is_empty()
                // there must not be less remaining cards then the best
                || (   (modified_remaining_cards >= best_modified_remaining_cards)
                    // if both colors have the same cards out,
                    // take the one the player has fewer tens
                    && (  (modified_remaining_cards
                           == best_modified_remaining_cards)
                        ? (hi.hand().numberof(ten)
                           < hi.hand().numberof(ten_to_play))
                        : true)
                   ) ) ) {
          best_modified_remaining_cards = modified_remaining_cards;
          ten_to_play = ten;
        } // if (take card)

      } // if (the color is no trumpcolor and the player has an ten of the color)
    } // for (i < ha.cardsnumber())

    return ten_to_play;
  } // if (trick.isstartcard())

  // I am not the start player
  if( !trick.isstartcard() ) {
    // check for pure color trick
    if (trick.winnercard().istrump())
      return Card::EMPTY;

    // the winnercard is no ten or ace
    if (trick.winnercard().value() >= Card::TEN)
      return Card::EMPTY;

    // the player has the ten of the color
    Card::Color const color = trick.startcard().color();
    if (   !hi.hand().contains(Card(color, Card::TEN))
        || Card(color, Card::TEN).istrump(hi.game())
        || !trick.isvalid(Card(color, Card::TEN), hi.hand()))
      return Card::EMPTY;

    // there are no aces remaining of the color
    if (hi.cards_information().remaining_others(Card(color, Card::ACE)))
      return Card::EMPTY;

    // there are enough remaining color cards, so the trick can go through
    if (hi.game().numberof(color)
        - hi.cards_information().played(color)
        - hi.hand().numberof(color)
        < trick.remainingcardno() - 1)
      return Card::EMPTY;

    // each player behind has the color
    for (unsigned c = trick.actcardno() + 1; c < hi.game().playerno(); ++c)
      if (   (hi.teamofplayer(trick.player_of_card(c)) != hi.team())
          && !hi.handofplayer(trick.player_of_card(c)).contains(color))
        return Card::EMPTY;

    return Card(color, Card::TEN);
  } // if( !trick.isstartcard() )

  return Card::EMPTY;
} // Card Heuristics::choose_ace(Trick trick, HeuristicInterface hi)


/**
 ** for a first card in a trick play a single color ace
 ** if there are still enough cards of this color in the game
 ** and the color has not been jabbed
 ** choose the single ace with the most cards in that color still in the game
 **
 ** @param     trick   current trick
 ** @param     hi      heuristic interface
 **
 ** @return    card to play (color ace or none)
 **
 ** @author    Diether Knof
 ** @author    Borg Enders
 **
 ** @version   0.7.6
 **/
Card
Heuristics::choose_single_ace(Trick const& trick, HeuristicInterface const& hi)
{
  // @heuristic::name   choose single ace
  // @heuristic::idea   for the first card in a trick choose the best single color Ace to play

  // I am not the start Player
  if( !trick.isstartcard() )
    return Card::EMPTY;

  // the number of remaining cards for the color
  map<Card::Color, unsigned> number_of_remaining_cards;


  // the card to play
  Card ace_to_play;
  // the maximal number of remaining cards found so far
  unsigned best_modified_remaining_cards = 0;
  for (vector<Card::Color>::const_iterator
       c = hi.game().rule().card_colors().begin();
       c != hi.game().rule().card_colors().end();
       ++c) {
    Card::Color const& color = *c;
    Card const ace(color, Card::ACE);

    // test that the color is no trumpcolor
    if (   (color != hi.game().trumpcolor())
        // test that the player has an ace of the color
        && hi.hand().contains( ace )
        && hi.hand().numberof( ace.color() ) == 1 ) {

      // the number of remaining cards for the other players
      unsigned const remaining_cards
        = (hi.game().numberof(color)
           - (hi.cards_information().played(*c) + hi.hand().numberof(*c)));
      // modified remaining cards
      // Modified by the number of aces of the color.
      // This is used to prefer single aces to double aces.
      unsigned const modified_remaining_cards
        = (remaining_cards
           +  (  (hi.hand().numberof(ace) + hi.cards_information().played(ace)
                  < hi.game().rule()(Rule::NUMBER_OF_SAME_CARDS))
               ? (hi.game().numberof(color) - 1) // *Value*
               : 0) );

      // there are enough cards for all players
      if (   (remaining_cards >= hi.game().playerno() - 1)
          // if the color has already been jabbed, it has to be from the own team
          && (   !hi.colorjabbed(color)
              || (   hi.jabbedbyownteam(color))
              && !hi.jabbedbyotherteam(color))
          // no ace found so far
          && (   ace_to_play.is_empty()
              // there must not be less remaining cards then the best
              || (   (modified_remaining_cards >= best_modified_remaining_cards)
                  // if both colors have the same cards out,
                  // take the one the player has fewer aces
                  && (  (modified_remaining_cards
                         == best_modified_remaining_cards)
                      ? (hi.hand().numberof(ace)
                         < hi.hand().numberof(ace_to_play))
                      : true)
                 ) ) ) {
        best_modified_remaining_cards = modified_remaining_cards;
        ace_to_play = ace;
      } // if (take card)

    } // if (the color is no trumpcolor and the player has an ace of the color)
  } // for (i < ha.cardsnumber())

  return ace_to_play;
} // Card Heuristics::choose_single_ace(Trick trick, HeuristicInterface hi)

/**
 ** finds lowest card with value cv
 **
 ** @param     h       the hand to search for a card
 ** @param     cv      the value to search for
 ** @param     hi      heuristic interface
 **
 ** @return    lowest card with the value cv (or 'Card()' if no card was found)
 **
 ** @author    Borg Enders
 **
 ** @version   Beta
 **/
Card
lowest_card( Hand const& h, Card::Value const cv, HeuristicInterface const& hi )
{
  Card c;

  unsigned i;

  for( i = 0; i < h.cardsnumber(); i++ )
  {
    // find any card
    if( h.card(i).value() == cv )
    {
      c = h.card(i);
      break;
    } // if ( h.card(i).value() == cv )
  } // for ( i < h.cardsnumber() )

  if (c.is_empty())
    return c;

  for( i = 0; i < h.cardsnumber(); i++ )
  {
    // find lowest card
    if(   h.card(i).value() == cv
       && h.card(i).less(c) )
      c = h.card(i);
  } // for ( i < h.cardsnumber() )

  return c;
} // Card lowest_card( Hand h, Card::Value cv, HeuristicInterface hi )


/**
 ** finds highest card with value cv
 **
 ** @param     h       the hand to search for a card
 ** @param     cv      the value to search for
 ** @param     hi      heuristic interface
 **
 ** @return    highest card with the value cv (or 'Card()' if no card was found)
 **
 ** @author    Borg Enders
 **
 ** @version   Beta
 **/
Card
highest_card( Hand const& h, Card::Value const cv, HeuristicInterface const& hi )
{
  Card c;

  unsigned i;

  for( i = 0; i < h.cardsnumber(); i++ )
  {
    // find any card
    if( h.card(i).value() == cv )
    {
      c = h.card(i);
      break;
    } // if ( h.card(i).value() == cv )
  } // for ( i < h.cardsnumber() )

  if (c.is_empty())
    return Card::EMPTY;

  for( i = 0; i < h.cardsnumber(); i++ )
  {
    // find highest card
    if(   h.card(i).value() == cv
       && !h.card(i).less(c) )
      c=h.card(i);
  } // for ( i < h.cardsnumber() )

  return c;
} // Card highest_card( Hand h, Card::Value cv, HeuristicInterface hi )


/**
 ** finds lowest jack
 **
 ** @param     h       the hand to search for a card
 ** @param     hi      heuristic interface
 **
 ** @return    lowest jack on hand (or 'Card()' if no card was found)
 **
 ** @author    Borg Enders
 **
 ** @version   Beta
 **/
Card
lowest_jack( Hand const& h, HeuristicInterface const& hi )
{
  return lowest_card( h, Card::JACK, hi );
} // Card lowest_jack ( Hand h, HeuristicInterface hi )


/**
 ** finds highest queen
 **
 ** @param     h       the hand to search for a card
 ** @param     hi      heuristic interface
 **
 ** @return    highest queen on hand (or 'Card()' if no card was found)
 **
 ** @author    Borg Enders
 **
 ** @version   Beta
 **/
Card
highest_queen( Hand const& h, HeuristicInterface const& hi )
{
  return highest_card( h, Card::QUEEN, hi );
} // Card highest_queen( Hand h, HeuristicInterface hi )


/**
 ** finds lowest queen
 **
 ** @param     h       the hand to search for a card
 ** @param     hi      heuristic interface
 **
 ** @return    lowest queen on hand (or 'Card()' if no card was found)
 **
 ** @author    Borg Enders
 **
 ** @version   Beta
 **/
Card
lowest_queen( Hand const& h, HeuristicInterface const& hi )
{
  return lowest_card( h, Card::QUEEN, hi );
} // Card lowest_queen ( Hand h, HeuristicInterface hi )


/**
 ** checks if there are any trumps in trick t
 **
 ** @param     trick   trick to check
 **
 ** @return    whether there is a trump card in the trick
 **
 ** @author    Borg Enders
 ** @author    Diether Knof
 **
 ** @version   0.7.5
 **/
bool
trumpInTrick( Trick const& trick )
{
  for (unsigned i = 0; i < trick.actcardno(); i++)
    if (trick.card(i).istrump())
      return true;

  return false;
} // bool trumpInTrick( Trick trick )


/**
 ** checks if all remaining player in the trick are of my team
 **
 ** @param     trick   trick to check
 ** @param     hi      Heuristic interface
 **
 ** @return    returns true if all remaining players are in my team
 **
 ** @author    Borg Enders
 ** @author    Diether Knof
 **
 ** @version   0.7.9
 **/
bool
checkAllMyTeam( Trick const& t, HeuristicInterface const& hi )
{
  // take a look if all players coming in this trick are of mine team
  for( unsigned i = t.actcardno() + 1; i < hi.game().playerno(); i++ )
    if (!hi.guessed_same_team(t.player_of_card(i)))
      return false;

  return true;
} // bool checkAllMyTeam( Trick t, HeuristicInterface hi )

/**
 ** checks if all remaining player in the trick are of the opposite team
 **
 ** @param     trick   trick to check
 ** @param     hi      Heuristic interface
 **
 ** @return    returns true if all remaining players are in the opposite team
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.9
 **/
bool
checkAllOppositeTeam(Trick const& t, HeuristicInterface const& hi)
{
  // take a look if all players coming in this trick are of the opposite team
  for (unsigned i = t.actcardno() + 1; i < hi.game().playerno(); i++)
    if (!hi.guessed_opposite_team(t.player_of_card(i)))
      return false;

  return true;
} // bool checkAllOppositeTeam(Trick t, HeuristicInterface hi)


/**
 ** -> result
 **
 ** @param     trick   trick to check
 ** @param     hi      heuristic interface
 **
 ** @return    whether the opposite team has a trump behind
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.6
 **
 ** @todo      check for 'must have' of the color of the trick
 **/
bool
opposite_trump_behind(Trick const& trick, HeuristicInterface const& hi)
{
  for (unsigned i = trick.actcardno() + 1; i < hi.game().playerno(); i++) {
    if (hi.team() == hi.teamofplayer(trick.player_of_card(i)))
      continue;

    // check for 'must have' of the color of the trick
    if (hi.cards_information().of_player(trick.player_of_card(i)).can_have(Card::TRUMP))
      return true;
  } // for (i < hi.game().playerno())

  return false;
} // bool opposite_trump_behind(Trick trick, HeuristicInterface hi)

/**
 ** -> result
 **
 ** @param     trick   trick to check
 ** @param     hi      heuristic interface
 **
 ** @return    the highest card of the playes of the opposite team behind
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.6
 **
 ** @todo      check for 'must have' of the color of the trick
 **/
HandCard const&
highest_card_behind_of_opposite_team(Trick const& trick,
                                     HeuristicInterface const& hi)
{
  HandCard const* highest_card = &HandCard::EMPTY;

  // take a look if all players coming in this trick are of mine team
  for (unsigned i = trick.actcardno() + 1; i < hi.game().playerno(); i++) {
    if (hi.game().is_undetermined_marriage()) {
      if (::is_selector((  trick.isempty()
                         ? Card::TRUMP
                         : trick.startcard().tcolor()),
                        hi.game().marriage_selector())) {
        if (   (hi.team() != TEAM::RE)
            && (hi.game().soloplayer() == trick.player_of_card(i)) ) {
          // in a selector trick, the re player is seen as of the own team
          continue;
        }
      }
    } else { // if (normal game)
      if (hi.team() == hi.teamofplayer(trick.player_of_card(i)))
        continue;
    }

    if (   (hi.game().type() == GAMETYPE::POVERTY)
        && (trick.player_of_card(i) == hi.game().soloplayer()) ) {
      if (   !trick.isempty()
          && !trick.startcard().istrump()) {
        HandCard const& c
          = hi.handofplayer(trick.player_of_card(i)).highest_card(trick.startcard().tcolor());
        if (highest_card->less(c))
          highest_card = &c;
        continue;
      }
    }
    // check for 'must have' of the color of the trick
    HandCard const& c = hi.handofplayer(trick.player_of_card(i)).highest_card();
    if (highest_card->less(c))
      highest_card = &c;
  } // for (i < hi.game().playerno())

  return *highest_card;
} // HandCard const& highest_card_behind_of_opposite_team(Trick trick, HeuristicInterface hi)

/**
 ** -> result
 **
 ** @param     trick   trick to check
 ** @param     hi      heuristic interface
 **
 ** @return    the cards of the playes of the opposite team behind
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.9
 **
 ** @todo      check for 'must have' of the color of the trick
 **/
HandCards
valid_cards_behind_of_opposite_team(Trick const& trick,
                                    HeuristicInterface const& hi)
{
  HandCards cards;

  // take a look if all players coming in this trick are of mine team
  for (unsigned i = trick.actcardno() + 1; i < hi.game().playerno(); i++) {
    Player const& p = trick.player_of_card(i);
    if (hi.game().is_undetermined_marriage()) {
      if (::is_selector((  trick.isempty()
                         ? Card::TRUMP
                         : trick.startcard().tcolor()),
                        hi.game().marriage_selector())) {
        if (   (hi.team() != TEAM::RE)
            && (hi.game().soloplayer() == p) ) {
          // in a selector trick, the re player is seen as of the own team
          continue;
        }
      }
    } else { // if (normal game)
      if (hi.team() == hi.teamofplayer(p))
        continue;
    }

    cards.add(hi.handofplayer(p).validcards(trick));
    // special case: second color run
    if (   !trick.isempty()
        && !trick.startcard().istrump()
        && (hi.cards_information().of_player(p).must_have(trick.startcard().tcolor()) == 0)
        && (hi.color_runs(trick.startcard().tcolor()) > 0))
      cards.add(hi.handofplayer(p).trumps());
  } // for (i < hi.game().playerno())

  return cards;
} // HandCards valid_cards_behind_of_opposite_team(Trick trick, HeuristicInterface hi)

/**
 ** -> result
 **
 ** @param     trick   trick to check
 ** @param     hi      heuristic interface
 **
 ** @return    the highest card of the playes of the same team behind
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.6
 **
 ** @todo      check for 'must have' of the color of the trick
 **/
HandCard const&
highest_card_behind_of_same_team(Trick const& trick,
                                 HeuristicInterface const& hi)
{
  HandCard const* highest_card = &HandCard::EMPTY;

  // take a look if all players coming in this trick are of mine team
  for (unsigned i = trick.actcardno() + 1; i < hi.game().playerno(); i++) {

    if (hi.game().is_undetermined_marriage()) {
      if (::is_selector((  trick.isempty()
                         ? Card::TRUMP
                         : trick.startcard().tcolor()),
                        hi.game().marriage_selector())) {
        if (   (hi.team() != TEAM::RE)
            && (hi.game().soloplayer() != trick.player_of_card(i)) ) {
          // in a selector trick, the contra players are seen as of the opposite team
          continue;
        }
      }
    } else { // if (normal game)
      if (hi.team() != hi.teamofplayer(trick.player_of_card(i)))
        continue;
    }

    if (   (hi.game().type() == GAMETYPE::POVERTY)
        && hi.game().is_soloplayer(trick.player_of_card(i) )) {
      if (   !trick.isempty()
          && !trick.startcard().istrump()) {
        HandCard const& c
          = hi.handofplayer(trick.player_of_card(i)).highest_card(trick.startcard().tcolor());
        if (highest_card->less(c))
          highest_card = &c;
        continue;
      }
    }
    // check for 'must have' of the color of the trick
    HandCard const& c = hi.handofplayer(trick.player_of_card(i)).highest_card();
    if (highest_card->less(c))
      highest_card = &c;
  } // for (i < hi.game().playerno())

  return *highest_card;
} // HandCard const& highest_card_behind_of_same_team(Trick trick, HeuristicInterface hi)

/**
 ** -> result
 **
 ** @param     hi      heuristic interface
 **
 ** @return    the lowest trump the player has which gets over the of the opposite team, or the highest trump; empty, if the trick can not be jabbed
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.9
 **/
HandCard
lowest_highest_trump(Trick const& trick, HeuristicInterface const& hi)
{
  HandCard lh_trump = hi.hand().highest_trump();
  if (   (lh_trump == Card::EMPTY)
      || !trick.winnercard().less(lh_trump))
    return HandCard::EMPTY;

  return Heuristics::optimized_card_to_play(lh_trump, trick, hi);
} // Card lowest_highest_trump(Trick trick, HeuristicInterface hi)

/**
 ** -> result
 **
 ** @param     hi      heuristic interface
 **
 ** @return    the highest trump of the playes of the opposite team
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.9
 **/
HandCard const&
highest_trump_of_opposite_team(HeuristicInterface const& hi)
{
  HandCard const* highest_trump = &HandCard::EMPTY;

  for (unsigned p = 0; p < hi.game().playerno(); ++p) {
    if (hi.team() == hi.teamofplayer(p))
      continue;

    HandCard const& c = hi.handofplayer(hi.game().player(p)).highest_trump();
    if (highest_trump->less(c))
      highest_trump = &c;
  } // for (p < hi.game().playerno())

  return *highest_trump;
} // HandCard const& highest_trump_of_opposite_team(HeuristicInterface hi)

/**
 ** -> result
 **
 ** @param     hi      heuristic interface
 **
 ** @return    the number of tricks the team will get
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.9
 **/
unsigned
secure_tricks_of_team(HeuristicInterface const& hi)
{
  HandCard const& highest_trump = highest_trump_of_opposite_team(hi);
  if (highest_trump.is_empty()) {
    // the opposite team has no more trump
    for (unsigned p = 0; p < hi.game().playerno(); ++p)
      if (   (hi.teamofplayer(p) == hi.team())
          && (   hi.handofplayer(p).numberoftrumps()
              == hi.handofplayer(p).cardsnumber()))
        return hi.game().tricks_remaining_no();
  } // if (!highest_trump)

  unsigned result = 0;
  for (unsigned p = 0; p < hi.game().playerno(); ++p)
    if (hi.teamofplayer(p) == hi.team())
      result = max(result,
                   hi.cards_information().of_player(p).must_have_cards().higher_cards_no(highest_trump));

  return result;
} // unsigned secure_tricks_of_team(HeuristicInterface const& hi)

/**
 ** finds best card on Hand of hi, which wins this trick,
 ** non trump ace or lowest trump
 ** greater than jack, only in lastcard lower trumps are allowed
 ** the card will be chosen taking the value of the trick in account
 **
 ** @param     trick     trick to check
 ** @param     v         value of the card
 ** @param     hi        heuristic interface
 ** @param     pointmodi point modifikator to add to points of t
 **
 ** @return    the card to play ( or 'Card()' if no card was found )
 **
 ** @author    Borg Enders
 **
 ** @version   Beta
 **/
Card
Heuristics::best_winning_card_single_picture(Trick const& t, Card::Value v,
                                             HeuristicInterface const& hi, unsigned pointmodi)
{
  // @heuristic::name   ?  best winning card single picture  ?
  // @heuristic::idea   Only valid for soli with only one picture as trump: Tries to find the best card to win a trick depending on different values like Aiconfig::LIMITQUEEN
  // @heuristic::action  jab the trick

  if ( t.isstartcard() )
    return Card::EMPTY;

  Card c;
  HandCards const ha = hi.hand().validcards( t );
  unsigned i;

  bool allmyteam = checkAllMyTeam( t, hi );

  // find any card that wins this trick greater or equal to lowest_trump_card_limit
  for( i = 0; i < ha.cardsnumber(); i++ )
  {
    if(   t.isvalid( ha.card(i), hi.hand() )
       && t.isjabbed(ha.card(i))
       && (   hi.lowest_trump_card_limit().less( ha.card( i ) )
           || allmyteam
          )
      )
    {
      c = ha.card(i);
      break;
    }
  } // for ( i < ha.cardsnumber() )

  // if can't win this trick and I'am not the last player: that's it
  if (c.is_empty() && !t.islastcard())
    return Card::EMPTY;

  bool trumpintrick = trumpInTrick( t );

  // or this is a color trick of a first color run with my card the first trump
  allmyteam =  (  allmyteam
                || (   t.actcardno() > 1
                    && hi.color_runs(t.startcard().color()) == 0
                    && !t.startcard().istrump()
                    && !trumpintrick
                   ) );

  // find a better card
  for( i = 0; i < ha.cardsnumber(); i++ )
  {

    if(   t.isvalid( ha.card( i ), hi.hand() )
       && t.isjabbed(ha.card(i))
      )
    {
      // non trump ace is always the best choice if color is not already jabbed
      if(   ha.card(i).value() == Card::ACE
         && !ha.card(i).istrump()
         && (   !hi.colorjabbed( ha.card(i).color() )
             || hi.jabbedbyownteam( ha.card(i).color() )
             || allmyteam
            )
        )
      {
        c=ha.card(i);
        break;
      }

      if ( ha.card(i).istrump() )
      {

        // find lowest trump
        if( (    hi.lowest_trump_card_limit().less( ha.card(i) )
             || t.islastcard()
             || allmyteam
            )
           && ha.card(i).less( c )
          )
        {

          c = ha.card(i);
          continue;
        }

      }// if card istrump
    } // if winneris
  } // for ( i < ha.cardsnumber() )


  // if last card or first_color_run of trick let's check for a better card
  // Ace, ten, king, nine

  if(   t.islastcard()
     || allmyteam
     ||(    !t.isstartcard()
        && !t.startcard().istrump()
        && hi.color_runs(t.startcard().color()) < 1
        && (   t.startcard().color()!= Card::HEART
            || !HandCard( hi.hand(), Card::HEART, Card::TEN ).isdolle()
           )
       )
    )
  {

    for( i = 0; i < ha.cardsnumber(); i++ )
    {
      if(   t.isvalid( ha.card( i ), hi.hand() )
         && t.isjabbed(ha.card(i))
        )
      {

        // ace is always the best card
        if(  !ha.card(i).isswine()
           && ha.card(i).value()==Card::ACE
           && (    allmyteam
               || !ha.card( i ).istrump()
               || t.islastcard()
              )
          )
        {
          c = ha.card(i);
          break;
        }

        if(   ha.card(i).value() == Card::TEN
           && !ha.card(i).isdolle()
           && (     t.islastcard()
               || allmyteam
               || (  hi.color_runs( t.startcard().color() ) == 0
                   && !t.startcard().istrump()
                  )
              )
          )
        {
          c = ha.card(i);
          if ( t.islastcard()
              || allmyteam )
            break;
          continue;
        }

        if(   ha.card( i ).value() != v
           && ha.card( i ).value() < Card::TEN )
        {
          c = ha.card(i);
          continue;
        }

      } // if( t.isvalid( ha.card( i ), hi.hand() ) && t.isjabbed(ha.card(i))
    } // for (i < ha.cardsnumber())
  } // if ( t.islastcard() || allmyteam || ... )


  return c;
} // Card Heuristics::best_winning_card_single_picture(Trick t, Card::Value v, HeuristicInterface hi, unsigned pointmodi)


/**
 ** finds best card on Hand of hi, which wins this trick,
 ** non trump ace or lowest trump
 ** greater than jack, only in lastcard lower trumps are allowed
 ** the card will be chosen taking the value of the trick in account
 **
 ** @param     trick     trick to check
 ** @param     v1        value of the first card
 ** @param     v2        value of the second card
 ** @param     hi        heuristic interface
 ** @param     pointmodi point modifikator to add to points of t
 **
 ** @return    the card to play ( or 'Card()' if no card was found )
 **
 ** @author    Borg Enders
 **
 ** @version   Beta
 **/
Card
Heuristics::best_winning_card_double_picture(
                                             Trick const& t, Card::Value v1, Card::Value v2,
                                             HeuristicInterface const& hi, unsigned pointmodi )
{
  // @heuristic::name   ?  best winning card double picture  ?
  // @heuristic::idea   Only valid for soli with only tow picutes as trump: Tries to find the best card to win a trick depending on different values like Aiconfig::LIMITQUEEN
  // @heuristic::action  jab the trick

  if ( t.isstartcard() )
    return Card::EMPTY;

  Trick tr;
  Card c;
  HandCards const ha = hi.hand().validcards(t);
  unsigned i;

  bool allmyteam = checkAllMyTeam( t, hi );;


  // find any card that wins this trick greater or equal to lowest_trump_card_limit
  for( i = 0; i < ha.cardsnumber(); i++ )
  {
    tr = t;
    tr += ha.card(i);
    if(   hi.is_winnerplayer(tr)
       && t.isvalid( ha.card(i),hi.hand() )
       && (   hi.lowest_trump_card_limit().less( ha.card(i) )
           || allmyteam
          )
      )
    {
      c = ha.card(i);
      break;
    }
  } // for ( i < ha.cardsnumber() )

  // if can't win this trick and I'am not the last player: that's it
  if (c.is_empty() && !t.islastcard() )
    return Card::EMPTY;

  bool trumpintrick = trumpInTrick( t );


  // or this is a color trick of a first color run with my card the first trump
  allmyteam =    allmyteam
    ||(   t.actcardno() > 1
       && hi.color_runs(t.startcard().color()) == 0
       && !t.startcard().istrump()
       && !trumpintrick
      );

  // find a better card
  for ( i = 0; i < ha.cardsnumber(); i++ )
  {
    tr = t;
    tr += ha.card( i );
    if(   hi.is_winnerplayer(tr)
       && t.isvalid( ha.card(i),hi.hand() )
      )
    {
      // non trump ace is always the best choice if color is not already jabbed
      if(   ha.card(i).value() == Card::ACE
         && !ha.card(i).istrump()
         && (   !hi.colorjabbed( ha.card(i).color() )
             || hi.jabbedbyownteam(ha.card(i).color())
             || allmyteam
            )
        )
      {
        c = ha.card( i );
        break;
      }

      if( ha.card(i).istrump() )
      {

        // find lowest trump
        if( (   hi.lowest_trump_card_limit().less( ha.card(i) )
             || t.islastcard()
             || allmyteam
            )
           && ha.card(i).less(c)
          )
        {
          c = ha.card(i);
          continue;
        }

      }// if card istrump

    } // if winneris
  } // for ( i < ha.cardsnumber() )

  // if last card or first_color_run of trick let's check for a better card
  // Ace, ten, king, nine
  if(   t.islastcard()
     || allmyteam
     ||(   !t.isstartcard()
        && !t.startcard().istrump()
        && hi.color_runs(t.startcard().color())<1
        && (   t.startcard().color()!= Card::HEART
            || !HandCard( hi.hand(), Card::HEART, Card::TEN ).isdolle()
           )
       )
    )
  {

    for( i = 0; i < ha.cardsnumber(); i++ )
    {
      tr = t;
      tr += ha.card(i);

      if(   hi.is_winnerplayer(tr)
         && t.isvalid(ha.card(i),hi.hand())
        )
      {
        // ace is always the best card
        if(   !ha.card(i).isswine()
           &&  ha.card(i).value()==Card::ACE
           && (   allmyteam
               || !ha.card(i).istrump()
               || t.islastcard()
              )
          )
        {
          c = ha.card(i);
          break;
        }

        if(   ha.card(i).value() == Card::TEN
           && !ha.card(i).isdolle()
           && (   t.islastcard()
               || allmyteam
               || (   hi.color_runs(t.startcard().color()) == 0
                   && !t.startcard().istrump()
                  )
              )
          )
        {
          c = ha.card(i);
          if ( t.islastcard()
              || allmyteam )
            break;

          continue;
        }

        if(    ha.card(i).value() != v1
           && ha.card(i).value() != v2
           && ha.card(i).value() < Card::TEN )
        {
          c = ha.card(i);
          continue;
        }

      } // if (hi.is_winnerplayer(tr))
    } // for ( i < ha.cardsnumber() )
  } // if ( t.islastcard() || allmyteam || (....) )

  return c;
} // Card Heuristics::best_winning_card_double_picture( ... )


/**
 ** finds best card on Hand of hi, which wins this trick,
 ** non trump ace or lowest trump
 ** greater than jack, only in lastcard lower trumps are allowed
 ** the card will be chosen taking the value of the trick in account
 **
 ** @param     trick     trick to check
 ** @param     hi        heuristic interface
 ** @param     pointmodi point modifikator to add to points of t
 **
 ** @return    the card to play ( or 'Card()' if no card was found )
 **
 ** @author    Borg Enders
 **
 ** @version   Beta
 **/
Card
Heuristics::best_winning_card_triple_picture( Trick const& t,
                                             HeuristicInterface const& hi, unsigned pointmodi )
{
  // @heuristic::name   ?  best winning card triple picture  ?
  // @heuristic::idea Only valid for soli with only three pictures as trump: Tries to find the best card to win a trick depending on different values like Aiconfig::LIMITQUEEN
  // @heuristic::action  jab the trick

  if ( t.isstartcard() )
    return Card::EMPTY;

  Trick tr;
  Card c;
  HandCards const ha = hi.hand().validcards( t );
  unsigned i;

  bool allmyteam = checkAllMyTeam( t, hi );;

  // find any card that wins this trick greater or equal to lowest_trump_card_limit
  for( i = 0; i < ha.cardsnumber(); i++ )
  {
    tr = t;
    tr += ha.card( i );

    if(   hi.is_winnerplayer(tr)
       && t.isvalid( ha.card(i), hi.hand() )
       && (  hi.lowest_trump_card_limit().less( ha.card(i) )
           || allmyteam
          )
      )
    {
      c = ha.card(i);
      break;
    }
  } // for ( i < ha.cardsnumber() )
  // if can't win this trick and I'am not the last player: that's it
  if( c.is_empty() && !t.islastcard() )
    return Card::EMPTY;

  bool trumpintrick = trumpInTrick( t );

  // or this is a color trick of a first color run with my card the first trump
  allmyteam =    allmyteam
    || (   t.actcardno() > 1
        && hi.color_runs(t.startcard().color()) == 0
        && !t.startcard().istrump()
        && !trumpintrick
       );

  // find a better card

  for( i = 0; i < ha.cardsnumber(); i++ )
  {
    tr = t;
    tr += ha.card(i);
    if(   hi.is_winnerplayer(tr)
       && t.isvalid( ha.card( i ), hi.hand() ) )
    {
      // non trump ace is always the best choice if color is not already jabbed
      if(   ha.card(i).value() == Card::ACE
         && !ha.card(i).istrump()
         && (   !hi.colorjabbed( ha.card( i ).color() )
             || hi.jabbedbyownteam( ha.card( i ).color() )
             || allmyteam
            )
        )
      {
        c = ha.card( i );
        break;
      }

      if( ha.card(i).istrump() )
      {

        // find lowest trump
        if( (   hi.lowest_trump_card_limit().less( ha.card(i) )
             || t.islastcard()
             || allmyteam )
           && ha.card(i).less(c) )
        {

          c = ha.card( i );
          continue;
        }

      }// if card istrump

    } // if winneris
  } // for ( i < ha.cardsnumber() )



  // if last card or first_color_run of trick let's check for a better card
  // Ace, ten, king, nine
  if(   t.islastcard()
     || allmyteam
     ||(   !t.isstartcard()
        && !t.startcard().istrump()
        && hi.color_runs( t.startcard().color() ) < 1
        && (   t.startcard().color()!= Card::HEART
            || !HandCard( hi.hand(), Card::HEART, Card::TEN ).isdolle()
           )
       )
    )
  {

    for( i = 0; i < ha.cardsnumber(); i++ )
    {
      tr = t;
      tr += ha.card( i );

      if(   hi.is_winnerplayer(tr)
         && t.isvalid( ha.card( i ), hi.hand() ) )
      {

        // ace is always the best card
        if(   !ha.card(i).isswine()
           && ha.card(i).value() == Card::ACE
           && (   allmyteam
               || !ha.card(i).istrump()
               || t.islastcard()
              )
          )
        {
          c = ha.card(i);
          break;
        }

        if(    ha.card(i).value() == Card::TEN
           && !ha.card(i).isdolle()
           && (   t.islastcard()
               || allmyteam
               || (   hi.color_runs( t.startcard().color()) == 0
                   && !t.startcard().istrump()
                  )
              )
          )
        {
          c = ha.card( i );
          if ( t.islastcard()
              || allmyteam )
            break;
          continue;
        }

        if( ha.card(i).value() == Card::NINE )
        {
          c = ha.card(i);
          continue;
        }
      } // if ( hi.is_winnerplayer(tr) && t.isvalid( ... ) )
    } // for ( i < ha.cardsnumber() )
  } // if ( t.islastcard() || allmyteam || ( ...... ) )

  return c;
} // Card Heuristics::best_winning_card_triple_picture( Trick t, HeuristicInterface hi, unsigned pointmodi )


/**
 ** finds best card on Hand of hi, which wins this trick,
 ** non trump ace or lowest trump
 ** greater than jack, only in lastcard lower trumps are allowed
 ** the card will be chosen taking the value of the trick in account
 **
 ** @param     trick     trick to check
 ** @param     hi        heuristic interface
 ** @param     pointmodi point modifikator to add to points of t
 **
 ** @return    the card to play ( or 'Card()' if no card was found )
 **
 ** @author    Borg Enders
 **
 ** @version   Beta
 **/
Card
Heuristics::best_winning_card_old(Trick const& t, HeuristicInterface const& hi,
                                  unsigned pointmodi )
{
  // @heuristic::name   best winning card
  // @heuristic::idea   Tries to find the best card to win a trick depending on different values like Aiconfig::LIMITQUEEN

  switch (hi.game().type()) {
  case GAMETYPE::SOLO_JACK:
    return best_winning_card_single_picture( t, Card::JACK, hi, pointmodi );
  case GAMETYPE::SOLO_QUEEN:
    return best_winning_card_single_picture( t, Card::QUEEN, hi, pointmodi);
  case GAMETYPE::SOLO_KING:
    return best_winning_card_single_picture( t, Card::KING, hi, pointmodi);
  case GAMETYPE::SOLO_QUEEN_JACK:
    return best_winning_card_double_picture( t, Card::JACK, Card::QUEEN, hi, pointmodi);
  case GAMETYPE::SOLO_KING_JACK:
    return best_winning_card_double_picture( t, Card::JACK, Card::KING, hi, pointmodi);
  case GAMETYPE::SOLO_KING_QUEEN:
    return best_winning_card_double_picture( t, Card::KING, Card::QUEEN, hi, pointmodi);
  case GAMETYPE::SOLO_KOEHLER:
    return best_winning_card_triple_picture( t, hi, pointmodi);
  default:
    break;
  } // switch (hi.game().type())


  if( t.isstartcard() )
    return Card::EMPTY;

  HandCard c( hi.hand() );
  HandCards const ha = hi.hand().validcards( t );
  unsigned i;
  Game const& game = t.game();

  bool allmyteam = checkAllMyTeam(t, hi);

  int const tpoints = t.points() + pointmodi + (t.remainingcardno()-1)*1;


  bool const trickLost = t.winnercard().less(highest_card_behind_of_opposite_team(t, hi));

  // check for enough points in the trick
  if (   t.islastcard()
      && (t.points() <= 6))
    return Card::EMPTY;

  if( !trickLost || allmyteam ) {
    // take lowest trump which takes this trick
    for( i = 0; i < ha.cardsnumber(); i++ ) {
      HandCard const& card = ha.card(i);

      if (!t.isjabbed(card))
        continue;
      if( (   c.is_empty()
           || card.value() < c.value()
           || ( card.value() == c.value()
               && card.less( c ) )
          )
        )
      {
        if (card.is_special()) {
          if ( tpoints >= hi.value( Aiconfig::LIMITDOLLE ) ) {
            c = card;
          }
        } else {
          c = card;
        }
      }

    } // for (i < ha.cardsnumber())

    if (!c.is_empty()) {
      // improve the card: take the low trumps in the order ace, ten, king, nine
      if (game.trumpcolor() != Card::NOCARDCOLOR) {
        if (c.istrump()) {
          vector<Card> cards;
          cards.push_back(Card(game.trumpcolor(), Card::ACE));
          cards.push_back(Card(game.trumpcolor(), Card::TEN));
          cards.push_back(Card(game.trumpcolor(), Card::KING));
          if (hi.game().rule()(Rule::WITH_NINES))
            cards.push_back(Card(game.trumpcolor(), Card::NINE));
          for (vector<Card>::const_iterator c = cards.begin();
               c != cards.end();
               ++c) {
            if (!HandCard(hi.hand(), *c).is_special())
              if (hi.hand().contains(*c)
                  && t.isjabbed(*c)) {
                return *c;
              }
          } // for (c)

        } // if c.istrump())
      } // if (game.trumpcolor() != Card::NOCARDCOLOR)

      { // take club queen over heart/spade queen to show myself
        if (   (game.type() == GAMETYPE::NORMAL)
            && (game.announcement_of_player(hi.player())
                == ANNOUNCEMENT::NOANNOUNCEMENT)
            && (hi.team() == TEAM::RE)
            && hi.hand().contains(Card::CLUB_QUEEN)
            && (   (c == Card(Card::HEART, Card::QUEEN))
                || (c == Card(Card::SPADE, Card::QUEEN)) )
           )
          c = Card::CLUB_QUEEN;
      } // take club queen over heart/spade queen

      return c;

    } // if (!c.is_empty())

  } // if( !trickLost || allmyteam )

  // find any card that wins this trick greater or equal to lowest_trump_card_limit
  for( i = 0; i < ha.cardsnumber(); i++ ) {
    HandCard const& card = ha.card(i);

    if (!t.isjabbed(card))
      continue;

    if (!(   hi.lowest_trump_card_limit().less(card)
          || allmyteam) )
      continue;

    if(   (card.value() == Card::QUEEN )
       && (tpoints >= hi.value(Aiconfig::LIMITQUEEN))
      )
    {
      c = card;
      if (!t.islastcard())
      {
        break;
      }
    }
    if( card.value() == Card::QUEEN )
      continue;

    if(  (   card.isdolle()
          || card.possible_swine()
          || card.possible_hyperswine()
         )
       && tpoints >= hi.value( Aiconfig::LIMITDOLLE )
      )
    {
      c = card;
      if (!t.islastcard())
      {
        break;
      }
    }

    if(   card.isdolle()
       || card.possible_swine()
       || card.possible_hyperswine() )
      continue;

    c = card;
    break;
  } // for(i)

  // if can't win this trick and I'am not the last player: that's it
  if(   c.is_empty()
     && !t.islastcard() )
    return Card::EMPTY;


  bool trumpintrick = trumpInTrick( t );

  // or this is a color trick of a first color run with my card the first trump
  allmyteam =   allmyteam
    ||(   t.actcardno() > 1
       && hi.color_runs(t.startcard().color()) == 0
       && !t.startcard().istrump()
       && !trumpintrick
       && (   t.startcard().color()!= Card::HEART
           || !HandCard(hi.hand(), Card::HEART, Card::TEN ).isdolle()
          )
      );

  // find a better card
  for( i = 0; i < ha.cardsnumber(); i++ ) {
    HandCard const& card = ha.card(i);

    if (!t.isjabbed(card))
      continue;

    // non trump ace is always the best choice if color is not already jabbed
    if(   card.value() == Card::ACE
       && !card.istrump()
       && (   !hi.colorjabbed( card.color() )
           || hi.jabbedbyownteam( card.color() )
           || allmyteam
          )
      ) {
      c = card;
      break;
    }

    if( card.istrump() ) {
      // find lowest trump
      if(     t.islastcard()
         && card.less(Card(Card::DIAMOND, Card::JACK))
         && card.less( c )
        )
      {
        c = card;
        continue;
      }

      if (!card.less(Card(Card::DIAMOND, Card::JACK))) {
        if(   !t.isstartcard()
           && card.less(Card(Card::DIAMOND, Card::QUEEN))
           && !t.startcard().istrump()
           && card.less( c )
           && (   !hi.colorjabbed( t.startcard().color() )
               || tpoints <= hi.value( Aiconfig::LIMITQUEEN )
               || ha.numberof( Card::QUEEN ) < 2
               || t.islastcard()
              )
          ) {
          c = card;
          continue;
        }
        if(    (card.value() == Card::QUEEN)
           && (tpoints >= hi.value( Aiconfig::LIMITQUEEN ) )
           && (  (    c.less(Card(Card::DIAMOND, Card::QUEEN))
                  && !t.islastcard()
                  && (    hi.color_runs( t.startcard().color() ) != 0
                      || (   hi.color_runs( t.startcard().color() ) == 0
                          && hi.game().type() == GAMETYPE::POVERTY
                         )
                      || t.startcard().istrump()
                      || tpoints > hi.value( Aiconfig::LIMITDOLLE )
                     )
                 )
               || (    card.less( c )
                   &&  !(    c == Card( Card::CLUB, Card::QUEEN )
                         && (hi.game().teaminfo( hi.player() ) == TEAM::UNKNOWN)
                         && (hi.game().trick_current_no() > 3)
                        )

                  )
               || ( (card == Card( Card::CLUB, Card::QUEEN ))
                   && (hi.game().teaminfo( hi.player() ) == TEAM::UNKNOWN)
                   && (hi.game().trick_current_no() > 3)
                  )
               )
               )
               {

                 if ( (hi.hand().numberofclubqueens() > 0) // club queen is as good as dolle
                     && !hi.game().second_dolle_over_first()
                     && hi.color_runs( t.startcard().color() ) > 0)
                 {

                   c = Card( Card::CLUB, Card::QUEEN );
                 } else {
                   c = card;
                 }
                 continue;
               }

        if(
           (   card.isdolle()
            || card.possible_swine()
            || card.possible_hyperswine()
           )
           && (   card.less(c)
               || (   tpoints >= hi.value( Aiconfig::LIMITDOLLE )
                   && c.less(Card(Card::DIAMOND, Card::QUEEN))
                   && (hi.color_runs( t.startcard().color() ) > 0)
                   && !t.islastcard()
                  )
              )
          )
        {

          if ( (hi.hand().numberofclubqueens() > 0) // club queen is as good as dolle
              && !hi.game().second_dolle_over_first()
              && card.isdolle() )
          {

            c = Card( Card::CLUB, Card::QUEEN );
          } else {
            c = card;
          }

          continue;
        }
      }//if card>jack
    }// if card istrump

  } // if winneris


  if (c.is_empty())
    return Card::EMPTY;



  Trick tempTrick = t;
  tempTrick += c;

  //if opposite team can win trick no further optimization

  if (    oppositeTeamCanWinTrick( tempTrick, hi )
      && !t.islastcard()
      && (   (hi.color_runs(t.startcard().color()) != 0 )
          || (hi.game().numberof(t.startcard().color())
              - hi.cards_information().played(t.startcard().color())
              <= t.remainingcardno()
             )
         ) )
  {
    // make it as hard as possible with selected trump type
    if( c.value() ==Card::JACK || c.value() == Card::QUEEN )
      c = highest_card( hi.hand(), c.value(), hi );

    c = optimized_card_to_play(c, t, hi);
    if (   hi.same_team(t.winnerplayer())
        && c.less(valid_cards_behind_of_opposite_team(t, hi).next_higher_card(t.winnercard())))
      return Card::EMPTY;

    return c;
  }



  bool onlyPovertyPlayer = true;
  for( unsigned i = t.actcardno() + 1; i < hi.game().playerno(); i++ )
    onlyPovertyPlayer = (   onlyPovertyPlayer
                         &&  (   hi.game().type() == GAMETYPE::POVERTY
                              && t.player_of_card( i ) == hi.game().soloplayer()
                             )); /// @todo poverty partner has no trumps


  // if last card or first_color_run of trick let's check for a better card
  // Ace, ten, king, nine
  if(   t.islastcard()
     || allmyteam
     || onlyPovertyPlayer
     ||(   !t.isstartcard()
        && !t.startcard().istrump()
        && hi.color_runs( t.startcard().color() ) < 1
        && (   t.startcard().tcolor() != Card::HEART
            || !hi.game().rule()(Rule::DOLLEN)
            || (hi.hand().numberoftrumpaces() - hi.hand().numberofswines()
                + hi.hand().numberof(hi.game().trumpcolor(), Card::TEN)
                >= 2 )
           )
       )
    )
  {

    for( i = 0; i < ha.cardsnumber(); i++ ) {
      HandCard const& card = ha.card(i);


      if (!t.isjabbed(card))
        continue;

      // Maybe it's time to get the heart ten back home
      if(   card.isdolle()
         &&(   (  hi.hand().numberoftrumps() < 5
                &&  hi.game().tricks_remaining_no() < 7
                &&  hi.hand().numberofdolle() == 1
                &&  hi.game().rule()(Rule::EXTRAPOINT_DOLLE_JABS_DOLLE)
                &&  !hi.cards_information().played(Card::DOLLE)
               )
            || (   hi.hand().numberoftrumps()  < 6
                &&  hi.game().tricks_remaining_no() < 8
                && hi.game().swines_owner() != NULL
                && hi.teamofplayer( *hi.game().swines_owner() ) != hi.team()
               )
            || (    hi.hand().numberoftrumps() < 6
                &&  hi.game().tricks_remaining_no()  < 8
                && hi.game().hyperswines_owner() != NULL
                && hi.teamofplayer( *hi.game().hyperswines_owner() ) != hi.team()
               )
           )
        )
      {
        c = card;
        break;
      }

      // ace is always the best card
      if(   (card.value() == Card::ACE)
         && !card.possible_swine()
         && (   t.islastcard()
             || allmyteam
             || (   hi.color_runs( t.startcard().color() ) == 0
                 && hi.game().type() != GAMETYPE::POVERTY
                 && !t.startcard().istrump()
                )
            )
        )
      {
        c = card;
        break;
      }

      if(    card.value() == Card::TEN
         && !card.isdolle()
         && (   t.islastcard()
             || allmyteam
             || (   hi.color_runs( t.startcard().color() ) == 0
                 && hi.game().type() != GAMETYPE::POVERTY
                 && !t.startcard().istrump()
                )
            )
        )
      {
        c = card;
        continue;
      }

      if(   card.value() == Card::KING
         && game.type() != GAMETYPE::SOLO_KOEHLER
         && !card.possible_hyperswine()
        )
      {
        if( c.value()!=Card::TEN || c.isdolle() )
        {
          c = card;
        }
        continue;
      }
      if(   card.value() == Card::NINE
         && !card.possible_hyperswine() )
      {
        if(  (   c.value() != Card::TEN
              && c.value() != Card::KING
              && c.value() != Card::ACE
              && better_points_optimize( c, card, hi )
             )
           || c.isdolle()
           || c.possible_hyperswine()
          )
        {
          c = card;
        }
        continue;
      }


      if(   c.istrump()
         && card.istrump()
         && card.less(c)

        )
      {
        if(  (   c.value() != Card::TEN
              && c.value() != Card::KING
              && c.value() != Card::ACE
              && better_points_optimize( c, card, hi )
              && !(   (c == Card( Card::CLUB, Card::QUEEN ))
                   && (hi.game().teaminfo(hi.player()) == TEAM::UNKNOWN)
                   && !(hi.game().type() == GAMETYPE::MARRIAGE_SILENT))
             )
           || c.isdolle()
           || c.possible_hyperswine()
          )
        {
          c = card;
        }
        continue;
      }
    } // for (i)
  } // if (last card or first color run)

  c = optimized_card_to_play(c, t, hi);
  if (   hi.same_team(t.winnerplayer())
      && c.less(valid_cards_behind_of_opposite_team(t, hi).next_higher_card(t.winnercard())))
    return Card::EMPTY;

  return c;
} // Card Heuristics::best_winning_card_old(Trick t, HeuristicInformation hi, unsigned pointmodi)


/**
 ** finds lowest color card on Hand h, which is the best in a meatless solo
 **
 ** @param     trick     trick to check
 ** @param     h         hand to choose the card from
 **
 ** @return    the card to play ( or 'Card()' if no card was found )
 **
 ** @author    Borg Enders
 **
 ** @version   Beta
 **/
Card
lowest_color_card_meatless( Trick const& t, Hand h )
{
  Card c;
  HandCards const ha = h.validcards( t );
  unsigned i;

  // find any card that's allowed for this trick
  for( i = 0; i < ha.cardsnumber(); i++ )
  {
    if( !ha.card( i ).istrump() )
    {
      c = ha.card( i );
      break;
    }
  } // for ( i < ha.cardsnumber() )

  if (c.is_empty())
    return Card::EMPTY;

  // find a better card
  for( i = 0; i < ha.cardsnumber(); i++ )
  {
    if(   !ha.card( i ).istrump()
       && c.value() <= ha.card(i).value()
       && (   ha.numberof( ha.card(i).color() ) >  ha.numberof( c.color() )
           || (   ha.numberof( ha.card(i).color(), Card::TEN ) == 0
               && ha.numberof( ha.card(i).color(), Card::ACE ) == 0
              )
          )
      )
      c = ha.card(i);
  } // for ( i < ha.cardsnumber() )

  return c;
} // Card lowest_color_card_meatless( Trick t, Hand h )


/**
 ** finds lowest trump card on Hand h
 **
 ** @param     trick     trick to check
 ** @param     h         hand to choose the card from
 **
 ** @return    the card to play ( or 'Card()' if no card was found )
 **
 ** @author    Borg Enders
 **
 ** @version   Beta
 **/
Card
lowest_trump_card( Trick const& t, Hand h )
{
  Card c;
  HandCards const ha = h.validcards( t );
  unsigned i;

  // find any card that's allowed for this trick
  for( i = 0; i < ha.cardsnumber(); i++ ) {
    if( ha.card( i ).istrump() )
    {
      c = ha.card( i );
      break;
    }
  } // for ( i < ha.cardsnumber() )

  if (c.is_empty())
    return Card::EMPTY;

  // find a better card
  for( i = 0; i < ha.cardsnumber(); i++ )
  {
    if(   ha.card(i).less( c )
       && ha.card(i).istrump()
      )
      c = ha.card( i );
  } // for ( i < ha.cardsnumber() )

  return c;
} // Card lowest_trump_card( Trick t, Hand h )


/**
 ** finds highest trump card on Hand h
 **
 ** @param     trick     trick to check
 ** @param     h         hand to choose the card from
 **
 ** @return    the card to play ( or 'Card()' if no card was found )
 **
 ** @author    Borg Enders
 **
 ** @version   Beta
 **/
Card
highest_trump_card( Trick const& t, Hand h )
{
  Card c;
  HandCards const ha = h.validcards( t );
  unsigned i;

  // find any card that's allowed for this trick
  for( i = 0; i < ha.cardsnumber(); i++ )
  {
    if( ha.card( i ).istrump() )
    {
      c = ha.card( i );
      break;
    }
  } // for ( i < ha.cardsnumber() )

  if (c.is_empty())
    return Card::EMPTY;

  // find a better card
  for( i = 0; i < ha.cardsnumber(); i++ )
  {
    if(   c.less( ha.card(i) )
       && ha.card(i).istrump()
      )
      c = ha.card( i );
  } // for ( i < ha.cardsnumber() )

  return c;
} // Card highest_trump_card( Trick t, Hand h )


/**
 ** finds lowest trump card on Hand h, but gives only fuchs or ten back
 ** if there are better cards to keep
 **
 ** @param     trick     trick to check
 ** @param     h         hand to choose the card from
 ** @param     hi        heuristic interface
 **
 ** @return    the card to play ( or 'Card()' if no card was found )
 **
 ** @author    Borg Enders
 **
 ** @version   Beta
 **/
Card
Heuristics::lowest_best_trump_card( Trick const& t, Hand const& h,
                                   HeuristicInterface const& hi )
{
  // @heuristic::name   ?  lowest best trump  ?
  // @heuristic::idea   find smallest best trump card (avoiding for example a fox in most situations)
  // @heuristic::action  play the smallest trump, but no fox/ten

  HandCard c( h );
  HandCards const ha = hi.hand().validcards( t );

  // find any card that's allowed for this trick
  for(unsigned i = 0; i < ha.cardsnumber(); i++ ) {
    if ( ha.card( i ).istrump() ) {
      c = ha.card( i );
      break;
    }
  } // for( i < ha.cardsnumber() )

  if (c.is_empty())
    return Card::EMPTY;

  // find a better card
  for(unsigned i = 0; i < ha.cardsnumber(); i++ ) {
    if(   ha.card( i ).istrump()
       && (   (c.value() >= Card::TEN)
           || ha.card( i ).less( c ) )
       && !ha.card( i ).isfox()
       && (   ha.card(i).value() < Card::TEN
           || c.isfox()
          )
      ) {
      c = ha.card( i );
    }
  } // for( i = 0; i < ha.cardsnumber(); i++ )

  if( !c.less( hi.trump_card_limit() ) ) {
    // maybe a diamond ten isn't all that bad
    for(unsigned i = 0; i < ha.cardsnumber(); i++ ) {
      if(   ha.card( i ).less( c )
         && ha.card( i ).istrump()
         && !ha.card( i ).isfox()
        ) {
        c = ha.card( i );
      }
    }
  } // if( !c.less( hi.trump_card_limit() ) )

  if( !c.less( hi.trump_card_limit() ) ) {
    // maybe a diamond Ace isn't all that bad
    for(unsigned i = 0; i < ha.cardsnumber(); i++ ) {
      if(   ha.card( i ).less( c )
         && ha.card(i).istrump()
        ) {
        c = ha.card( i );
      }
    }
  } // if( !c.less( hi.trump_card_limit() ) )

  return c;
} // Card Heuristics::lowest_best_trump_card( Trick t, Hand h, HeuristicInterface hi )


/**
 ** best pfund on hand: a trump Ass, color ten or Ace, trump ten, a king,
 ** a notrump queen or jack, nine
 **
 ** @param     trick     trick to check
 ** @param     hi        heuristic interface
 **
 ** @return    the card to play ( or 'Card()' if no card was found )
 **
 ** @author    Borg Enders
 **
 ** @version   Beta
 **/
Card
Heuristics::choose_pfund_card(Trick const& trick, HeuristicInterface const& hi)
{
  // @heuristic::name   ?  choose pfund card  ?
  // @heuristic::idea   find a pfund card
  // @heuristic::action   play a pfund

  Trick t = trick;

  DEBUG_ASSERTION( !t.isempty(),
                  "Heuristics::choose_pfund_card():\n"
                  "  Called with empty trick");


  bool allmyteam = (checkAllMyTeam( t, hi ) && ownTeamCanWinTrick(t, hi));
  allmyteam = (   allmyteam
               || (   hi.game().type() == GAMETYPE::POVERTY
                   && hi.is_soloplayer()
                   && trick.cardno_of_player(hi.game().poverty_partner()) == 3 ) );
  // marriage determination for the bride
  if (   (hi.game().type() == GAMETYPE::MARRIAGE)
      && is_selector(t.startcard().tcolor(),
                     hi.game().marriage_selector())
      && hi.is_soloplayer() )
    allmyteam = true;

  if (   hi.guessed_same_team(t.winnerplayer())
      && !oppositeTeamCanWinTrick(t, hi))
    allmyteam = true;

  // first color run
  if (   !t.startcard().istrump()
      && (hi.color_runs(t.startcard().color()) == 0)
      && (   (t.winnercard().value() == Card::ACE)
          || t.winnercard().istrump()))
    allmyteam |= hi.guessed_same_team(t.winnerplayer());

  Card c;

  HandCards const ha = hi.hand().validcards( t );

  // first color trick in a solo, soloplayer wins
  bool solo_check =
    (   GAMETYPE::is_solo( hi.game().type() )
     && hi.color_runs(t.startcard().color()) == 0
     && !t.startcard().istrump()
     && t.winnerplayer() == hi.game().soloplayer()
     && t.winnercard().less( hi.trump_card_limit() )
     && !t.islastcard() ) ;

  if (   !hi.game().is_solo()
      && !(   hi.same_team(t.winnerplayer())
           && t.islastcard()
           && t.cardno_of_player( t.winnerplayer() ) == 0
           && !t.winnercard().istrump() ) //only use trump ace when partner is not winning already in first position
      && !(   hi.game().rule()(Rule::GENSCHER)
           && (hi.game().type() == GAMETYPE::NORMAL)
           && !hi.cards_information().played(Card(Card::DIAMOND,
                                                  Card::KING))
           && (ha.numberoftrumps() >= 6) // *Value*
          )
     ) {
    // then find first trump ace
    for( unsigned i = 0; i < ha.cardsnumber(); i++ ) {
      if(   ha.card(i).value() == Card::ACE
         && ha.card(i).istrump()
         && (   t.islastcard()
             || allmyteam
             || ha.numberoftrumps() < 3
             || (   hi.color_runs(t.startcard().color()) == 0
                 && !t.startcard().istrump()
                )
             || (   ! hi.cards_information().higher_card_exists( t.winnercard())
                 //!t.winnercard().less( Card( Card::CLUB, Card::QUEEN ) )
                 && hi.same_team(t.winnerplayer())
                )
             || (   hi.game().is_undetermined_marriage()
                 && hi.same_player(hi.game().soloplayer())
                 && hi.game().marriage_selector() == MARRIAGE_SELECTOR::FIRST_FOREIGN
                )
            )
         && !ha.card(i).possible_swine()
        )
      {
        t += ha.card( i );
        if (   (hi.teamofplayer( t.winnerplayer() )== hi.team())
            || (   (   hi.value(Aiconfig::TRUSTING)
                    || (hi.team_information().certainty() >= 30) ) // **Value**
                && hi.guessed_same_team(t.winnerplayer()) )
            || (   allmyteam
                && !hi.trump_card_limit().less(t.winnercard()) ) ) {
          return ha.card(i);
        }
      }
    } // for (i < ha.cardsnumber)
  } // if partner is start player and with none trump card.

  { // choose a single ten of a color
    Card best_ten;
    for (vector<Card::Color>::const_iterator
         color = hi.game().rule().card_colors().begin();
         color != hi.game().rule().card_colors().end();
         ++color) {
      Card const local_ten(*color, Card::TEN);
      if (   !hi.hand().contains(local_ten)
          || local_ten.istrump(hi.game())
          || !t.isvalid(local_ten, hi.hand()))
        continue;

      if (ha.numberof(local_ten.color()) == 1) {
        if (   best_ten.is_empty()
            || (   hi.cards_information().remaining_others(*color)
                && (hi.cards_information().remaining_others(*color)
                    < hi.cards_information().remaining_others(best_ten.color())) )
           ) {
          best_ten = local_ten;
        }
      } // if (single ten)
    } // for (color \in card_colors)
    if(!best_ten.is_empty()) {
      return best_ten;
    }
  } // choose a single ten of a color

  if (   (   hi.teamofplayer(t.winnerplayer()) == hi.team()
          || solo_check
          || allmyteam )
      // do not pfund an ace in a meatless solo, if the soloplayer is behind
      && !(   (hi.game().type() == GAMETYPE::SOLO_MEATLESS)
           && (   !t.has_played(hi.game().soloplayer())
               || (t.cardno_of_player(hi.game().soloplayer())
                   < t.winnercardno()) ) ) )
  {
    { // choose a ten of color if the corresponding ace is on the hand
      Card best_ten;
      for (vector<Card::Color>::const_iterator
           color = hi.game().rule().card_colors().begin();
           color != hi.game().rule().card_colors().end();
           ++color) {
        Card const local_ten( *color, Card::TEN );
        if (   !hi.hand().contains(local_ten)
            || local_ten.istrump( hi.game() )
            || !t.isvalid(local_ten, hi.hand())
            || !hi.hand().contains(Card(*color, Card::ACE))
            || hi.cards_information().remaining_others(Card(*color, Card::ACE))
            || (   hi.hand().contains(Card(*color, Card::ACE))
                && !hi.cards_information().remaining_others(Card(*color, Card::ACE)))
           )
          continue;
        if (   best_ten.is_empty()
            || (   hi.cards_information().remaining_others(best_ten.color())
                && !(   (hi.hand().numberof(*color) < hi.hand().numberof(best_ten.color()))
                     || (hi.cards_information().remaining_others(*color) == 0))
               )
           ) {
          best_ten = local_ten;
        }
      } // for (color \in card_colors)
      if(!best_ten.is_empty()) {
        if (hi.cards_information().remaining_others(best_ten))
          return best_ten;
        else
          return Card(best_ten.color(), Card::ACE);
      }
    } // choose a ten of color if the corresponding ace is on the hand
    { // choose an ace of a color 
      Card best_ace;
      // choose an ace of a color 
      // * if the ace is single
      for (vector<Card::Color>::const_iterator
           color = hi.game().rule().card_colors().begin();
           color != hi.game().rule().card_colors().end();
           ++color) {
        Card const local_ace( *color, Card::ACE );
        if (   !hi.hand().contains(local_ace)
            || local_ace.istrump( hi.game() )
            || !t.isvalid(local_ace, hi.hand()))
          continue;

        // if the ace is single, take it
        if (hi.hand().numberof(*color) == 1) {
          if(   best_ace.is_empty()
             || (hi.cards_information().remaining_others(*color)
                 && (hi.cards_information().remaining_others(*color)
                     < hi.cards_information().remaining_others(best_ace.color())) )
            ) {
            best_ace = local_ace;
          }
        }
      } // for (color)
      if(!best_ace.is_empty()) {
        return best_ace;
      }

      // choose an ace of a color 
      // * if the player has two aces
      for (vector<Card::Color>::const_iterator
           color = hi.game().rule().card_colors().begin();
           color != hi.game().rule().card_colors().end();
           ++color) {
        Card const local_ace( *color, Card::ACE );
        if (   !hi.hand().contains(local_ace)
            || local_ace.istrump( hi.game() )
            || !t.isvalid(local_ace, hi.hand()))
          continue;

        // if the player has two aces, take it
        if (   hi.hand().numberof( local_ace )
            == hi.game().rule()(Rule::NUMBER_OF_SAME_CARDS)) {
          if(   best_ace.is_empty()
             || (hi.cards_information().remaining_others(*color)
                 < hi.cards_information().remaining_others(best_ace.color()))
            ) {
            best_ace = local_ace;
          }
        }
      } // for (color)
      if(!best_ace.is_empty()) {
        return best_ace;
      }

      // choose an ace of a color 
      // * if there are not enough remaining color cards and the player has not the ten or no other player has a ten
      for (vector<Card::Color>::const_iterator
           color = hi.game().rule().card_colors().begin();
           color != hi.game().rule().card_colors().end();
           ++color) {
        Card const local_ace( *color, Card::ACE );
        if (   !hi.hand().contains(local_ace)
            || local_ace.istrump( hi.game() )
            || !t.isvalid(local_ace, hi.hand())
            ||   (   !Card(*color, Card::TEN).istrump(hi.game())
                  && hi.cards_information().remaining_others(*color, Card::TEN))
           )
          continue;

        if (   (   (hi.game().type() == GAMETYPE::POVERTY)
                && (hi.is_soloplayer() ) )
            || (   (hi.hand().numberof(local_ace) == 1)
                && (hi.cards_information().remaining_others(*color)
                    < hi.game().playerno() - 1
                    + ( (   (t.startcard().tcolor() == *color)
                         && !t.winnercard().istrump())
                       ? (t.remainingcardno() - 1)
                       : 0))
                && (hi.game().type() != GAMETYPE::SOLO_MEATLESS)
               )
           ) {
          if (best_ace.is_empty()) {
            best_ace = local_ace;
          } else if (hi.cards_information().remaining_others(*color)
                     > hi.cards_information().remaining_others(best_ace.color())) {
            best_ace = local_ace;
          }
        }
      } // for (color)
      if(!best_ace.is_empty()) {
        return best_ace;
      }
    } // choose an ace of a color 

    { // choose a ten of color
      Card best_ten;
      for (vector<Card::Color>::const_iterator
           color = hi.game().rule().card_colors().begin();
           color != hi.game().rule().card_colors().end();
           ++color) {
        Card const local_ten( *color, Card::TEN );
        if (   !hi.hand().contains(local_ten)
            || local_ten.istrump( hi.game() )
            || !t.isvalid(local_ten, hi.hand()))
          continue;
        if (best_ten.is_empty())
          best_ten = local_ten;
        else if (hi.cards_information().remaining_others(*color) == 0) {
          if (hi.cards_information().remaining_others(best_ten.color()) == 0) {
            if (hi.hand().numberof(*color) < hi.hand().numberof(best_ten.color()))
              best_ten = local_ten;
          }
        } else if (hi.cards_information().remaining_others(best_ten.color()) == 0)
          ;
        else if (hi.hand().numberof(*color) < hi.hand().numberof(best_ten.color()))
          best_ten = local_ten;
      } // for (color \in card_colors)

      if(!best_ten.is_empty()) {
        return best_ten;
      }
    } // choose a ten of color

    { // choose a color ace from the longest color
      Card best_ace;
      // do not pfund an ace in a meatless solo, if the soloplayer is behind
      if (!(   (hi.game().type() == GAMETYPE::SOLO_MEATLESS)
            && (   !t.has_played(hi.game().soloplayer())
                || (t.cardno_of_player(hi.game().soloplayer())
                    < t.winnercardno()) ) ) ) {
        for( vector<Card::Color>::const_iterator
            color = hi.game().rule().card_colors().begin();
            color != hi.game().rule().card_colors().end();
            ++color ) {
          Card const local_ace( *color, Card::ACE );
          if (   !hi.hand().contains(local_ace)
              || local_ace.istrump( hi.game() )
              || !t.isvalid(local_ace, hi.hand()))
            continue;
          if (  (hi.hand().numberof(local_ace) == 1)
              && (  (   (   (hi.cards_information().remaining_others(*color)
                             < hi.game().playerno() - 1
                             + ( (   (t.startcard().tcolor() == *color)
                                  && !t.winnercard().istrump())
                                ? (t.remainingcardno() - 1)
                                : 0))
                         || !(   hi.hand().contains(*color, Card::TEN)
                              && !HandCard(hi.hand(), *color, Card::TEN).is_special())
                        )
                     && (hi.game().type() != GAMETYPE::SOLO_MEATLESS))
                  || (hi.hand().numberof(*color) == 1)
                  || (trick.points() + 11 >= 40)
                 )
              || !hi.hand().contains(*color, Card::TEN)
             )
          {
            if(   best_ace.is_empty()
               || (hi.cards_information().remaining_others(*color)
                   > hi.cards_information().remaining_others(best_ace.color()))
              ) {
              best_ace = local_ace;
            }
          }
        } // for (color \in card_colors)

        if(!best_ace.is_empty()) {
          return best_ace;
        }
      } // if (no meatless solo with solo player behind)
    } // choose a color ace
  } // if (trick goes to own team)

  bool const to_fat_solo
    = (   GAMETYPE::is_solo( hi.game().type() )
       && (static_cast<int>(t.points()) > hi.value( Aiconfig::LIMITQUEEN ))
       && (t.player_of_card( 3 ) == hi.game().soloplayer()));
  // or an ace of color
  for( unsigned i = 0; i < ha.cardsnumber(); i++ ) {
    int cardsInGame
      = hi.cards_information().remaining_others(ha.card(i).color());

    if(   (ha.card(i) == Card(t.startcard().color(), Card::ACE))
       && (hi.color_runs(ha.card(i).color()) == 0)
       && (hi.hand().numberof(t.startcard().color()) >= 3)
       && !ha.card(i).istrump()
       /// @todo   see 'choose pfund', check that enough cards still remain
       && (   cardsInGame >= 2
           || ( hi.game().type() == GAMETYPE::POVERTY
               && hi.is_soloplayer() )
          )
       && (   hi.teamofplayer(t.winnerplayer()) == hi.team()
           || solo_check
           || allmyteam
          )
       && !(hi.game().type() == GAMETYPE::SOLO_MEATLESS)
      ) {
      return ha.card(i);
    }

    if(   ha.card(i).value()==Card::ACE
       && (  hi.color_runs(ha.card(i).color()) > 0
           || (   t.winnercard().istrump()
               && allmyteam )
          )
       && cardsInGame >= 2
       && !ha.card(i).istrump()
       && t.isvalid( ha.card(i),hi.hand() )
       && !to_fat_solo
       && (   (   hi.game().type()!=GAMETYPE::SOLO_JACK // with not much trumps
               && hi.game().type()!=GAMETYPE::SOLO_QUEEN // it is good to
               && hi.game().type()!=GAMETYPE::SOLO_KING  // keep a second ace
              )
           || hi.color_runs(t.startcard().color()) > 0
          )
      )
    {
      // prefer a ten to an ace
      if (   hi.hand().contains(ha.card(i).color(), Card::TEN)
          && !HandCard(hi.hand(), ha.card(i).color(), Card::TEN).istrump()
          && hi.cards_information().remaining_others(Card(ha.card(i).color(), Card::TEN)))
        return Card(ha.card(i).color(), Card::TEN);
      return ha.card(i);
    }
  } // for ( i < ha.cardsnumber() )

  // or an ace of trump
  for( unsigned i = 0; i < ha.cardsnumber(); i++ )
    if(   ha.card(i).value() == Card::ACE
       && ha.card(i).istrump()
       && !ha.card(i).possible_swine()
      )
    {
      return ha.card(i);
    }

  // or a ten of trump
  for( unsigned i = 0; i < ha.cardsnumber(); i++ )
    if(   ha.card(i).value() == Card::TEN
       && ha.card(i).istrump()
       && !ha.card(i).isdolle()
      )
    {
      return ha.card(i);
    }


  // or a King of color
  for ( unsigned i = 0; i < ha.cardsnumber(); i++ )
    if(   ha.card(i).value() == Card::KING
       && !ha.card(i).istrump()
      )
    {
      return ha.card(i);
    }


  // or a king of trump
  for( unsigned i = 0; i < ha.cardsnumber(); i++ )
    if(   (ha.card(i).value() == Card::KING)
       && ha.card(i).istrump()
       && !ha.card(i).possible_hyperswine()
       && !is_picture_solo(hi.game().type())
      )
    {
      return ha.card(i);
    }


  // or Queen if no trump
  for ( unsigned i = 0; i < ha.cardsnumber(); i++ )
    if(    ha.card(i).value()==Card::QUEEN
       && !ha.card(i).istrump()
       && hi.game().type() != GAMETYPE::SOLO_QUEEN
       && hi.game().type() != GAMETYPE::SOLO_KING_QUEEN
       && hi.game().type() != GAMETYPE::SOLO_QUEEN_JACK
       && hi.game().type() != GAMETYPE::SOLO_KOEHLER
      )
    {
      return ha.card(i);
    }


  // or Jack if no trump
  for( unsigned i = 0; i < ha.cardsnumber(); i++ )
    if(   ha.card(i).value() == Card::JACK
       && !ha.card(i).istrump()
       && hi.game().type() != GAMETYPE::SOLO_JACK
       && hi.game().type() != GAMETYPE::SOLO_QUEEN_JACK
       && hi.game().type() != GAMETYPE::SOLO_KING_JACK
       && hi.game().type() != GAMETYPE::SOLO_KOEHLER
      )
    {
      return ha.card(i);
    }


  // or a nine of color
  for( unsigned i = 0; i < ha.cardsnumber(); i++ )
    if(   ha.card(i).value() == Card::NINE
       && !ha.card(i).istrump()
      )
    {
      return ha.card(i);
    }


  { // or a small jack of trump
    Card best_jack;
    for ( unsigned i = 0; i < ha.cardsnumber(); i++ ) {
      if(   ha.card(i).value() == Card::JACK
         && (ha.card(i).color() <= Card::HEART)
         && ha.card(i).istrump()
        ) {
        if (best_jack.is_empty()
            || ha.card(i).less(best_jack))
          best_jack = ha.card(i);
      }
    } // for (i)
    if (!best_jack.is_empty())
      return best_jack;
  } // or a small jack of trump

  return c;
} // Card Heuristics::choose_pfund_card( Trick trick, HeuristicInterface hi )


/**
 ** checks if a solo player sits behind the player
 **
 ** @param     t    trick
 ** @param     hi   heuristics interface
 **
 ** @return    whether the solo player sits behind
 **
 ** @author    Borg Enders
 **
 ** @version   0.7.5
 **/
bool
soloPlayerBehind( Trick const& t, HeuristicInterface const& hi )
{
  return (   hi.game().is_solo()
          && !t.has_played(hi.game().soloplayer()));
} // bool soloPlayerBehind( Trick const& t, HeuristicInterface const& hi )


/**
 ** play a good card for the last card in one Trick with color, which
 ** hi doesn't have anymore,
 **
 ** if this trick is won by own team
 **   play a pfund
 ** if there are only color cards in this trick
 **   choose best_winning_card or if there is no lowest_color_card
 ** if there is already a trump try jab this trump with lowest possible
 **   trump, if there are more then a limit of points already in this trick
 **
 ** @param     trick     trick to check
 ** @param     hi        heuristic interface
 **
 ** @return    the card to play ( or 'Card()' if no card was found )
 **
 ** @author    Borg Enders
 **
 ** @version   Beta
 **/
Card
Heuristics::choose_for_color_trick( Trick const& t,
                                   HeuristicInterface const& hi )
{
  // @heuristic::name   choose for color trick
  // @heuristic::idea   choose a card for a color trick either a pfund for the own team or a small card for the opponents
  // @heuristic::condition color trick
  // @heuristic::action play a pfund for the own team or a small card for the opponetnts or jab the trick

  if (   GAMETYPE::is_real_solo(hi.game().type())
      && soloPlayerBehind(t, hi))
    return Card::EMPTY;

  if (t.isempty())
    return Card::EMPTY;

  if (t.startcard().istrump())
    return Card::EMPTY;

  Game const& game = hi.game();

  HandCard c( hi.hand() );

  HandCards ha;

  bool allmyteam = (   checkAllMyTeam( t, hi )
                    && hi.jabbedbyownteam( t.startcard().color() )
                    && !t.islastcard());
  // Check whether the soloplayer in a poverty can jab the trick
  if (   (game.type() == GAMETYPE::POVERTY)
      && hi.same_player(game.poverty_partner())
      && !hi.handofplayer(game.soloplayer()).can_jab(t))
    allmyteam = false;

  allmyteam = (   allmyteam
               && (   hi.teamofplayer(t.winnerplayer())==hi.team()
                   || (   t.winnercard().less( hi.trump_card_limit() )
                       && t.winnercard().istrump() )
                  ) );

  // in a color trick assume, the partners can get over a nine or a king
  if (   !t.winnercard().istrump()
      && checkAllMyTeam(t, hi)
      && !t.islastcard()
      && (   (t.winnercard().value() == Card::NINE)
          || (t.winnercard().value() == Card::KING)) )
    // ToDo: check whether a player behind can jab the trick
    allmyteam = true;

  if (!allmyteam) {
    // ToDo: if the partner has won the trick, check whether the opposite team can still get the trick
    for (unsigned i = t.actcardno() + 2; i < game.playerno(); ++i) {
      Player const& player = t.player_of_card(i);
      if (   (hi.teamofplayer(player) != hi.team())
          && (   (hi.handofplayer(player).contains(t.startcard().tcolor())
                  && t.winnercard().less(hi.handofplayer(player).highest_card(t.startcard().tcolor())) )
              || (!hi.handofplayer(player).contains(t.startcard().tcolor())
                  && hi.handofplayer(player).hastrump() ) ) ) {
        return Card::EMPTY;
      }
    } // for (i)
    // special case: poverty
    if (   (game.type() == GAMETYPE::POVERTY)
        && (hi.team() == TEAM::CONTRA)
        && !t.has_played(game.poverty_partner()) ) {
      return Card::EMPTY;
    }
  } // if (!allmyteam)

  // lets see if this is the last card for this trick
  // or the first run of this  color
  // and first card is no trump
  if ( (   t.islastcard()
        || allmyteam
        || (   !t.isstartcard()
            && hi.color_runs(t.startcard().color()) == 0
           )
       )
      && !t.startcard().istrump()
     )
  {
    ha = hi.hand().validcards( t );

    // first check if this trick goes to my team
    bool pred
      = (  (static_cast<int>(t.points()) >= hi.value(Aiconfig::LIMITDOLLE))
         ? (hi.teamofplayer(t.winnerplayer()) == hi.team())
         : (hi.guessedteamofplayer(t.winnerplayer()) == hi.team() ));

    if( game.type()==GAMETYPE::MARRIAGE ) {
      if (::is_selector(t.startcard().tcolor(),
                        game.marriage_selector()) ) {
        pred = hi.is_soloplayer();
      } else { // if !(trick determines marriage)
        pred = hi.teamofplayer(t.winnerplayer()) == maybe_to_team( hi.team() );
      } // if !(trick determines marriage)
    }  // if (game.type() == GAMETYPE::MARRIAGE)

    pred = (   pred
            && (    t.islastcard()
                || (   t.winnercard().value() == Card::ACE
                    &&  (hi.color_runs(t.startcard().color()) == 0)
                    &&  (hi.cards_information().remaining_others(t.startcard().color()) >= t.remainingcardno() - 1) )
                ||(   t.winnercard().istrump()
                   && !t.winnercard().less( hi.trump_card_limit() )
                  )
               ));

    pred = pred || allmyteam;

    if( game.type() == GAMETYPE::POVERTY ) {
      pred &=    (hi.same_player(game.poverty_partner()))
        && (  (   !game.rule()(Rule::DOLLEN)
               || t.startcard().color() != Card::HEART)
            || allmyteam);
    }

    if (   pred
        || (   GAMETYPE::is_solo( game.type() )
            && t.winnerplayer() != game.soloplayer()
            &&  hi.color_runs(t.startcard().color()) == 0
            &&  t.winnercard().less( hi.trump_card_limit() )
            &&  !t.islastcard()
           )
       ) {
      Card const c = choose_best_fehl( t, hi, false );
      if(   !c.is_empty()
         && hi.hand().numberof( c.color() ) == 1
         && hi.color_runs( c.color() ) == 0 )
        return c;
      return choose_pfund_card( t, hi );
    } // if own Team gets this trick

    // if there are only color cards in this trick
    bool trumpintrick = trumpInTrick( t );

    if( !trumpintrick ) {
      c = best_winning_card( t, hi );

      if( !ha.contains(t.startcard().color() ) ) {
        if(!c.is_empty())
        {
          if (static_cast<int>(t.points()) > hi.value(Aiconfig::LIMITDOLLE)) {
            return c;
          }
          if(   (static_cast<int>(t.points()) > hi.value( Aiconfig::LIMITQUEEN))
             && (  !Card( Card::CLUB, Card::QUEEN ).less( c )
                 || t.islastcard() )
            ) {
            return c;
          }

          if( !Card( Card::CLUB, Card::JACK ).less( c )
             || t.islastcard() ) {
            return c;
          }

        } // if(!c.is_empty())

        return Card::EMPTY;

      } else if (!c.is_empty()) { // if( !ha.contains(t.startcard().color() ) )

        return c;
      } else { // if (c.is_empty())
        GameType gt = game.type();

        if( gt != GAMETYPE::SOLO_MEATLESS ) {
          if(   hi.color_runs(t.startcard().color()) == 0
             && t.winnercard().value() != Card::ACE
             && ha.numberof( t.startcard().color(), Card::ACE )  > 0 )
            return Card( t.startcard().color(), Card::ACE );

          if( hi.color_runs(t.startcard().color()) > 0) {
            return Heuristics::lowest_color_card(t.startcard().color(),
                                                 hi.hand());
          } else {
            return lowest_best_trump_card(t, hi.hand(), hi );
          }
        } else { // if( gt != GAMETYPE::SOLO_MEATLESS )
          return lowest_color_card_meatless( t, hi.hand() );
        } // if( gt == GAMETYPE::SOLO_MEATLESS )
      } // if (c.is_empty())


      return Card::EMPTY;
    }// there was no other trumps in trick
    else // there is a trump in this trick
    {
      // if there is already a trump
      // and my partner doesn't already win this trick
      // there are more then 10 points already in this trick
      // try jab this trump with lowest possible trump

      if( maybe_to_team( hi.teamofplayer( t.winnerplayer() ) ) != hi.team() ) {
        c = best_winning_card( t, hi );

        if( !c.is_empty() ) {
          if( static_cast<int>(t.points()) > hi.value(Aiconfig::LIMITDOLLE) )
            return c;
          if(   (static_cast<int>(t.points()) > hi.value(Aiconfig::LIMITQUEEN))
             && !Card( Card::CLUB, Card::QUEEN ).less( c )
            )
            return c;
          if (static_cast<int>(t.points())
              < hi.value(Aiconfig::LIMIT_THROW_FEHL)) {
            Card const card = Heuristics::lowest_card(t, hi.hand());
            if (   (card.value() == Card::NINE)
                || (card.value() == Card::KING))
              if (better_points_optimize(c, HandCard(hi.hand(), card), hi))
                return card;
          } // if (t.points() < 5)
          if( !Card( Card::CLUB, Card::JACK ).less( c ) ) {
            return c;
          }

        } // if (!c.is_empty())
      } // if( maybe_to_team( hi.teamofplayer( t.winnerplayer() ) ) != hi.team() )

      return Card::EMPTY;
    } // else

  } // check if trick is valid for this heuristic

  return c;
} // Card Heuristics::choose_for_color_trick( Trick t, HeuristicInterface hi )

/**
 ** Jab a color trick with the lowest card over the fox.
 ** There is intentionally no check, whether the trick goes to the own team.
 **
 ** @param     trick     trick to check
 ** @param     hi        heuristic interface
 **
 ** @return    the card to play ( or 'Card()' if no card was found )
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.9
 **/
Card
Heuristics::jab_color_over_fox(Trick const& trick,
                               HeuristicInterface const& hi )
{
  // @heuristic::name   jab color over fox
  // @heuristic::idea   Jab a color trick with the lowest card over the fox

  // @heuristic::condition   color trick
  if (trick.isempty())
    return Card::EMPTY;
  if (trick.startcard().istrump())
    return Card::EMPTY;

  // the trick must not be served
  if (hi.hand().contains(trick.winnercard().color()))
    return Card::EMPTY;

  // @heuristic::condition   trick not already jabbed
  if (trick.winnercard().istrump())
    return Card::EMPTY;

  // @heuristic::condition   not the last card in the trick
  if (trick.islastcard())
    return Card::EMPTY;

  // @heuristic::condition   no meatless and no picture solo
  if (hi.game().trumpcolor() == Card::NOCARDCOLOR)
    return Card::EMPTY;

  // @heuristic::condition   some points in the trick
  if (   (trick.points() == 0)
      && (    trick.islastcard()
          || (   (trick.remainingcardno() == 2)
              && hi.guessed_same_team(trick.lastplayer()))))
    return Card::EMPTY;

  // @heuristic::action   play the next card above the fox
  Card const card = hi.hand().next_higher_card(Card(hi.game().trumpcolor(), Card::ACE));
  // check to play a club queen
  if (   (hi.game().type() == GAMETYPE::NORMAL)
      // ToDo: check the own gameplay for team informations
      && (hi.game().teaminfo(hi.player()) == TEAM::UNKNOWN)
      && (hi.team() == TEAM::RE)
      && hi.hand().contains(Card::CLUB_QUEEN)
      && (card.value() == Card::QUEEN)
      && (   (card.color() == Card::SPADE)
          || (  (trick.actcardno() <= 1)
              ? !hi.guessed_opposite_team(trick.player_of_card(2)) : false)
          || (  (trick.actcardno() <= 2)
              ? !hi.guessed_opposite_team(trick.player_of_card(3)) : false)
         )
     )
    return Card::CLUB_QUEEN;

  return card;
} // Card Heuristics::jab_color_over_fox(Trick trick, HeuristicInterface hi)


/**
 ** Play a color again when not jabbed or the own team has jabbed it
 **
 ** @param        trick        current trick
 ** @param        hi        heuristic interface
 **
 ** @return       card to play, 'Card()' if the heuristic does not match
 **
 ** @author       Borg Enders
 ** @author       Diether Knof
 **
 ** @version      0.7.9
 **/
Card
Heuristics::retry_color(Trick const& trick, HeuristicInterface const& hi)
{
  // @heuristic::name   retry color
  // @heuristic::idea   try a color again, which was not jabbed or jabbed by the own team
  if (!trick.isstartcard())
    return Card::EMPTY;

  for (vector<Card::Color>::const_iterator
       c = hi.game().rule().card_colors().begin();
       c != hi.game().rule().card_colors().end();
       ++c) {
    if (   (hi.color_runs(*c) == 0)
        || (hi.hand().numberof(*c) == 0))
      continue;

    if (   hi.colorjabbed(*c)
        && !hi.jabbedbyownteam(*c))
      continue;

    // check that the trick could get through
    if (hi.cards_information().played(*c)
        + (hi.game().playerno() - 1)
        + hi.hand().numberof(*c)
        > hi.game().numberof(*c))
      continue;

    if (   hi.jabbedbyownteam(*c)
        && hi.guessed_same_team(trick.lastplayer()))
      return hi.hand().highest_card(*c);

    if (   (hi.hand().numberof(Card(*c, Card::TEN)) > 0)
        && !Card(*c, Card::TEN).istrump(hi.game())
        && hi.jabbedbyownteam(*c)
        && (hi.teamofplayer(trick.lastplayer()) == hi.team()) )
      return Card(*c, Card::TEN);
    if (   (hi.hand().numberof(Card(*c, Card::NINE)) > 0)
        && !Card(*c, Card::NINE).istrump(hi.game()))
      return Card(*c, Card::NINE);
    if (   (hi.hand().numberof(Card(*c, Card::KING)) > 0)
        && !Card(*c, Card::KING).istrump(hi.game()))
      return Card(*c, Card::KING);

  } // for (c \in valid card colors)

  return Card::EMPTY;
} // Card Heuristics::retry_color(Trick trick, HeuristicInterface hi)


/**
 ** Play a color the partner does not have
 ** prerequisites:
 **   1. it must be the first card in the trick
 **   2. the partner does not have the color
 **   3. the players behind the partner can have the color
 ** Then take the card with the greatest value in the shortest color
 **
 **
 ** @param        trick        current trick
 ** @param        hi        heuristic interface
 **
 ** @return        card to play, 'Card()' if the heuristic does not match
 **
 ** @author        Diether Knof
 **
 ** @version        0.7.4
 **/
Card
Heuristics::play_color_for_partner(Trick const& trick,
                                   HeuristicInterface const& hi)
{
  // @heuristic::name   play color for partner
  // @heuristic::idea   play a color the partner does not have

  // @heuristic::condition   startplayer in the trick
  if (!trick.isstartcard())
    return Card::EMPTY;

  // one partner known
  if (   (hi.game().type() == GAMETYPE::NORMAL)
      && !hi.team_information().guessed_partner())
    return Card::EMPTY;

  // best card so far to play
  Card best_card;
  // maximal remaining cards of the selected color
  unsigned max_remaining_cards_no = 0;

  for (vector<Card::Color>::const_iterator
       c = hi.game().rule().card_colors().begin();
       c != hi.game().rule().card_colors().end();
       ++c) {
    Card::TColor const& color = *c;
    // test that the color is no trumpcolor
    if (color == hi.game().trumpcolor())
      continue;

    unsigned const remaining_cards_no
      = hi.cards_information().remaining_others(*c);
    // @heuristic::condition   there are at least as many remaining cards as the best so far
    if (remaining_cards_no < max_remaining_cards_no)
      continue;

    // @heuristic::condition   the partner does not have the color
    // @heuristic::condition   the partner has the highest trump
    // @heuristic::condition   or behind the partner all players have the color
    //                         (players - me - partner)
    Card highest_trump_of_opponents = Card::EMPTY;
    Card highest_trump_of_partners = Card::EMPTY;
    for (unsigned p = hi.game().playerno() - 1; p > trick.actcardno(); --p) {
      Player const& player = trick.player_of_card(p);
      // update the highest card of the opponents
      if (!hi.handofplayer(player).contains(color)) {
        if (hi.guessed_same_team(player)) {
          if (highest_trump_of_partners.less(hi.handofplayer(player).highest_trump()))
            highest_trump_of_partners = hi.handofplayer(player).highest_trump();
        } else { // if !(same team)
          if (highest_trump_of_opponents.less(hi.handofplayer(player).highest_trump()))
            highest_trump_of_opponents = hi.handofplayer(player).highest_trump();
        } // if !(same team)

      } // if (!hi.handofplayer(player).contains(color))
    } // for (p)
    if (highest_trump_of_partners.is_empty())
      continue;
    if (hi.game().less(highest_trump_of_partners, highest_trump_of_opponents)) {
      highest_trump_of_opponents = Card::EMPTY;
      bool bad_color = false; // whether this is a bad color to play
      for (unsigned p = hi.game().playerno() - 1; p > trick.actcardno(); --p) {
        Player const& player = trick.player_of_card(p);
        // update the highest card of the opponents
        if (   !hi.guessed_same_team(player)
            && !hi.handofplayer(player).contains(color))
          if (highest_trump_of_opponents.less(hi.handofplayer(player).highest_trump()))
            highest_trump_of_opponents = hi.handofplayer(player).highest_trump();
        // the last partner does not have the color and has a high trump
        if (hi.guessed_same_team(player)) {
          if (hi.handofplayer(player).contains(color))
            bad_color = true;
          if (   hi.handofplayer(player).highest_trump().less(hi.trump_card_limit())
              && hi.handofplayer(player).highest_trump().less(highest_trump_of_opponents))
            bad_color = true;
          break;
        }
      } // for (p \in playerno)
      if (bad_color)
        continue;
    } // if (highest_trump_of_partners.less(highest_trump_of_opponents, hi.game()))

    // search the card with the highest value
    HandCard const& card = hi.hand().highest_card(color);

    if (   best_card.is_empty()
        || (remaining_cards_no > max_remaining_cards_no)
        || (card.value() > best_card.value()) ) {
      best_card = card;
      max_remaining_cards_no = remaining_cards_no;
    } // if (found better card)
  } // for (c \in possible colors)

  // @heuristic::action   play the card with the highest value in the smallest color
  return best_card;
} // Card Heuristics::play_color_for_partner(Trick trick, HeuristicInterface hi)


/**
 ** Play a color the partner hopefully does not have
 ** prerequisites:
 **   1. it must be the first card in the trick
 **   2. the partner is the last player
 **   3. there is only one remaining card of the color
 **   4. another player could have the card
 ** Then take the card with the lowest value in the shortest color
 **
 **
 ** @param        trick        current trick
 ** @param        hi        heuristic interface
 **
 ** @return       card to play, 'Card()' if the heuristic does not match
 **
 ** @author       Diether Knof
 **
 ** @version      0.7.4
 **
 ** @todo         take card value into account
 **/
Card
Heuristics::try_color_for_partner(Trick const& trick,
                                  HeuristicInterface const& hi)
{
  // @heuristic::name   try color for partner
  // @heuristic::idea   try a color the partner hopefully does not have

  // @heuristic::condition   startplayer in the trick
  if (!trick.isstartcard())
    return Card::EMPTY;

  // @heuristic::condition   last player of the trick is partner
  if (!(::maybe_to_team(hi.teamofplayer(trick.lastplayer())) == hi.team()))
    return Card::EMPTY;

  // best card so far to play
  Card best_card;
  // maximal number of other players who can have the card
  // there must be at least one
  unsigned max_other_players = 1;

  for (vector<Card::Color>::const_iterator
       c = hi.game().rule().card_colors().begin();
       c != hi.game().rule().card_colors().end();
       ++c) {
    Card::Color const& color = *c;
    // test that the color is no trumpcolor
    if (color == hi.game().trumpcolor())
      continue;

    // the player has the color
    if (hi.hand().numberof(color) == 0)
      continue;

    // @heuristic::condition   the last player can have the last remaining card of the color
    if (!(  (hi.handofplayer(trick.lastplayer()).contains(color))
          && (hi.cards_information().remaining_others(color) == 1)) )
      continue;

    // number of other players who can have the last card
    unsigned other_players = 0;

    for (unsigned p = hi.game().playerno() - 2; p > trick.actcardno(); --p) {
      Player const& player = trick.player_of_card(p);
      // ToDo: check that the card value for 'p' is greater than
      //       the card value for the partner
      if (hi.handofplayer(player).contains(color))
        other_players += 1;
    } // for (p \in trick.actcardno())

    // search the card with the lowest value
    HandCard const& card = hi.hand().lowest_card(color);

    if (   (other_players > max_other_players)
        || (   (other_players == max_other_players)
            && (   best_card.is_empty()
                || (card.value() < best_card.value())) ) ) {
      best_card = card;
      max_other_players = other_players;
    } // if (found better card)
  } // for (c \in possible colors)

  // @heuristic::action   play the card with the lowest value
  return best_card;
} // Card Heuristics::try_color_for_partner(Trick trick, HeuristicInterface hi)


/**
 ** Play shortest color in game for solo contra in hope solo player and partner
 ** can jab
 **
 **
 ** @param        trick        current trick
 ** @param        hi        heuristic interface
 **
 ** @return       card to play, 'Card()' if the heuristic does not match
 **
 ** @author       Diether Knof
 **
 ** @version      0.7.4
 **
 ** @todo         take card value into account
 **/
Card
Heuristics::play_color_in_solo(Trick const& trick,
                               HeuristicInterface const& hi)
{
  // @heuristic::name        play color in solo
  // @heuristic::idea        Play shortest color in game for solo contra in hope solo player and partner can jab.

  // @heuristic::condition   start card in the trick
  if (!trick.isstartcard())
    return Card::EMPTY;

  // @heuristic::condition   contra player in a picture or color solo
  if (hi.team() != TEAM::CONTRA)
    return Card::EMPTY;

  if (!(   GAMETYPE::is_picture_solo(hi.game().type())
        || GAMETYPE::is_color_solo(hi.game().type()) )
     )
    return Card::EMPTY;

  unsigned best_remaining = UINT_MAX;
  Card best_card;

  // @heuristic::action      play color card with the least cards remaining
  // 2do color with the minimum remaining points??? (if the re player is last player or all players behind the re players have to serve the color)
  for( vector<Card::Color>::const_iterator
      color = hi.game().rule().card_colors().begin();
      color != hi.game().rule().card_colors().end();
      ++color ) {
    if(   (hi.cards_information().remaining_others(*color) < best_remaining)
       && hi.hand().contains(*color)
       && (*color != hi.game().trumpcolor()) ) {
      best_remaining = hi.cards_information().remaining_others(*color);
      best_card = hi.hand().lowest_card(*color);
    }
  } // for (color \in card_colors)


  return best_card;
} // Card Heuristics::play_color_in_solo(Trick trick, HeuristicInterface hi)

/**
 ** Start a new color hoping the partner has an ace
 ** prerequisites:
 **   1. it must be the first card in the trick
 **   2. first run of the color
 **   3. the partner is known and can have the ace
 ** Then take the card with the greatest value in the shortest color
 **
 **
 ** @param        trick        current trick
 ** @param        hi        heuristic interface
 **
 ** @return       card to play, 'Card()' if the heuristic does not match
 **
 ** @author       Diether Knof
 **
 ** @version      0.7.4
 **/
Card
Heuristics::play_color_for_partner_ace(Trick const& trick,
                                       HeuristicInterface const& hi)
{
  // @heuristic::name   play color for partner ace
  // @heuristic::idea   play a color hoping the partner has the ace

  // @heuristic::condition   startplayer in the trick
  if (!trick.isstartcard())
    return Card::EMPTY;

  // best card so far to play
  Card best_card;
  // maximal remaining cards of the selected color
  // @heuristic::condition   or there are enough remaining cards for all players
  //                         (players - me)
  unsigned max_remaining_cards_no = hi.game().playerno() - 1;

  for (vector<Card::Color>::const_iterator
       c = hi.game().rule().card_colors().begin();
       c != hi.game().rule().card_colors().end();
       ++c) {
    Card::Color const& color = *c;
    Card const ace(color, Card::ACE);
    // test that the color is no trumpcolor
    // @heuristic::condition   first run of the color
    if (   (color == hi.game().trumpcolor())
        || (hi.cards_information().played(color)
            > hi.cards_information().of_player(hi.player()).played(color)) )
      continue;

    // the player has the color
    if (hi.hand().contains(color) == 0)
      continue;

    // @heuristic::condition   the next in line who can have the ace is of the same team
    Player const* partner = NULL;
    for (unsigned p = 1; p < hi.game().playerno(); ++p) {
      if (hi.guessed_same_team(trick.player_of_card(p))) {
        if (hi.handofplayer(trick.player_of_card(p)).contains(ace)) {
          partner = &trick.player_of_card(p);
          break;
        }
      } else { // if !(same team)
        if (hi.handofplayer(trick.player_of_card(p)).contains(ace))
          break;
        if (!hi.handofplayer(trick.player_of_card(p)).contains(color))
          break;
      } // if !(same team)
    } // for (c < hi.game().playerno())

    // a partner is found
    if (!partner)
      continue;

    // @heuristic::condition   the partner has not started a trick with a non-color-ace
    for (unsigned t = 0; t < hi.game().trick_current_no(); ++t) {
      Trick const& trick = hi.game().trick(t);
      if (   (trick.startplayer() == *partner)
          && !trick.startcard().istrump()
          && (trick.startcard().value() == Card::ACE) ) {
        partner = NULL;
        break;
      }
    } // for (t < hi.game().trick_current_no())
    if (!partner)
      continue;

    unsigned const remaining_cards_no
      = hi.cards_information().remaining_others(*c);
    // there are at least as many remaining cards as the best so far
    if (remaining_cards_no < max_remaining_cards_no)
      continue;


    // search the card with the highest value
    HandCard const& card = hi.hand().highest_card(color);

    if (   (remaining_cards_no > max_remaining_cards_no)
        || (card.value() > best_card.value()) ) {
      best_card = card;
      max_remaining_cards_no = remaining_cards_no;
    } // if (found better card)
  } // for (c \in possible colors)

  // @heuristic::action   play the card with the highest value in the smallest color
  return best_card;
} // Card Heuristics::play_color_for_partner_ace(Trick trick, HeuristicInterface hi)

/**
 ** Play a bad color, t.i. one the partner has also
 **
 ** @param     trick   current trick
 ** @param     hi      heuristic interface
 **
 ** @return    card to play, 'Card()' if the heuristic does not match
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.9
 **
 ** @todo      perhaps add condition "the opposite team does not have the color"
 **/
Card
Heuristics::play_bad_color(Trick const& trick,
                           HeuristicInterface const& hi)
{
  // @heuristic::name   play bad color
  // @heuristic::idea   play a color the partner has

  // @heuristic::condition   startplayer in the trick
  if (!trick.isstartcard())
    return Card::EMPTY;

  // @heuristic::condition   the partner is in the back
  if (hi.guessedteamofplayer(trick.lastplayer()) != hi.team())
    return Card::EMPTY;
  Player const& partner = trick.lastplayer();

  Card::Color selected_color = Card::NOCARDCOLOR;
  for (vector<Card::Color>::const_iterator
       c = hi.game().rule().card_colors().begin();
       c != hi.game().rule().card_colors().end();
       ++c) {
    Card::Color const& color = *c;
    if (color == hi.game().trumpcolor())
      continue;

    if (!hi.hand().contains(color))
      continue;

    // @heuristic::condition   the partner has the color
    if (!hi.handofplayer(partner).contains(color))
      continue;

    if (   (selected_color == Card::NOCARDCOLOR)
        || (hi.handofplayer(partner).numberof(color)
            < hi.handofplayer(partner).numberof(selected_color)))
      selected_color = color;
  } // for (c \in colors)

  if (selected_color == Card::NOCARDCOLOR)
    return Card::EMPTY;

  // @heuristic::action    play lowest card of selected color
  return hi.hand().lowest_card(selected_color);
} // Card Heuristics::play_bad_color(Trick trick, HeuristicInterface hi)

/**
 ** When a player cannot get a color trick take the card with the smallest value
 ** prerequisites:
 **   1. it must not be the first card in the trick
 **   2. the trick must be a color trick;
 **   3. one must have to serve the color
 **   4. the winnercard must be an ace or a trump or one does not have the ace;
 **   5. the winnerplayer must not be of the own team;
 ** The main case is that one is the last to play and has only got to
 ** take ones lowest card.
 **
 ** @param        trick        current trick
 ** @param        hi        heuristic interface
 **
 ** @return       card to play, 'Card()' if the heuristic does not match
 **
 ** @author       Diether Knof
 **
 ** @version      0.6.6
 **/
Card
Heuristics::serve_color_trick(Trick const& trick, HeuristicInterface const& hi)
{
  // @heuristic::name   serve color trick
  // @heuristic::idea   When a player cannot get a color trick take the card with the smallest value. The main case is that one is the last to play and has only got to take ones lowest card.

  // ToDo: Check, whether the trick can go to the own team

  // @heuristic::condition   it must not be the first card in the trick
  if (trick.isstartcard()) {
    return Card::EMPTY;
  }

  // @heuristic::condition   the trick must be a color trick
  if (trick.startcard().istrump()) {
    return Card::EMPTY;
  }

  Card::Color const color = trick.startcard().color();

  // @heuristic::condition   one cannot get the trick
  if (   hi.hand().contains(color)
      && hi.hand().contains(Card(color, Card::ACE))
      && trick.winnercard().less(Card(color, Card::ACE))
      && !HandCard(hi.hand(), color, Card::ACE).is_special())
    return Card::EMPTY;
  if (   hi.hand().contains(color)
      && (hi.cards_information().remaining(Card(color, Card::ACE)) == 0)
      && hi.hand().contains(Card(color, Card::TEN))
      && trick.winnercard().less(Card(color, Card::TEN))
      && !HandCard(hi.hand(), color, Card::TEN).is_special())
    return Card::EMPTY;
  if (   !hi.hand().contains(color)
      && hi.hand().higher_card_exists(trick.winnercard()))
    return Card::EMPTY;

  // as last player: the trick should not go to the own team
  if (   (hi.guessedteamofplayer(trick.winnerplayer()) == hi.team())
      && !oppositeTeamCanWinTrick(trick, hi))
    return Card::EMPTY;

  HandCards const color_cards = hi.hand().validcards( trick );

  // @heuristic::condition   the winnercard must be an ace or a trump or one does not have the ace in the non-trump case or there are not enough remaining cards to get through
  if (!(   (trick.winnercard().value() == Card::ACE)
        || trick.winnercard().istrump() ) ) {
    if (   (color_cards.numberof(Card(color, Card::ACE)) > 0)
        && (hi.cards_information().remaining_others(color)
            >= trick.remainingcardno() - 1) )
      return Card::EMPTY;
    if ( !trick.winnercard().less(highest_card_behind_of_opposite_team(trick,
                                                                       hi))
        && (   color_cards.higher_card_exists(trick.winnercard())
            || trick.winnercard().less(highest_card_behind_of_same_team(trick,
                                                                        hi)) ) )
      return Card::EMPTY;
  } // if (trick.winnercard.value() != Card::ACE) && (...)

  // @heuristic::condition   the winnerplayer must not be of the own team, or the trick does not get through
  if (   (hi.teamofplayer( trick.winnerplayer() ) == hi.team())
      && (   (hi.color_runs(color) != 0)
          || trick.winnercard().istrump()
          || (trick.winnercard() == Card(color, Card::ACE))
         )
      && (hi.cards_information().remaining_others(color)
          >= trick.remainingcardno() - 1)
     ) {
    return Card::EMPTY;
  }

  { // @heuristic::condition   no partner can jab

    Player const* p;

    for (p = &hi.game().player_following(trick.actplayer());
         p != &trick.startplayer();
         p = &hi.game().player_following(*p)) {
      Hand const& hand = hi.handofplayer(*p);
      if (   (hi.guessedteamofplayer(*p) == hi.team())
          && hand.higher_card_exists(trick.winnercard()) ) {
        switch (trick.winnercard().value()) {
        case Card::ACE:
          if (!Card(color, Card::ACE).istrump(hi.game())
              && hand.contains(Card(color, Card::ACE)))
            break;
        case Card::TEN:
          if (!Card(color, Card::TEN).istrump(hi.game())
              && hand.contains(Card(color, Card::TEN)))
            break;
        case Card::KING:
          if (!Card(color, Card::KING).istrump(hi.game())
              && hand.contains(Card(color, Card::KING)))
            break;
        case Card::QUEEN:
          if (!Card(color, Card::QUEEN).istrump(hi.game())
              && hand.contains(Card(color, Card::QUEEN)))
            break;
        case Card::JACK:
          if (!Card(color, Card::JACK).istrump(hi.game())
              && hand.contains(Card(color, Card::JACK)))
            break;
        case Card::NINE:
          if (!Card(color, Card::NINE).istrump(hi.game())
              && hand.contains(Card(color, Card::NINE)))
            break;

        case Card::NOCARDVALUE:
        case Card::UNKNOWNCARDVALUE:
          return Card::EMPTY;
        } // switch (trick.winnercard().value())
      } // if (own team and player has higher card)
    } // for (p)

#ifdef OUTDATED
    // DK 0.7.8 -- no check, that the partner has to play the color
    // first make sure there are not enough cards for everyone
    if (!(   (hi.color_runs(color) == 0)
          && (color_cards.cardsnumber() + (hi.game().playerno() - 1)
              <= hi.game().numberof(color)) )) {
      for (p = &hi.game().player_following(trick.actplayer());
           p != &trick.startplayer();
           p = &hi.game().player_following(*p)) {
        if (   (hi.guessedteamofplayer(*p) == hi.team())
            && hi.handofplayer(*p).higher_card_exists(trick.winnercard())) {
          break;
        }
      } // for (p)
      if (p != &trick.startplayer()) {
        return Card::EMPTY;
      }
    } // if (enough cards remain)
#endif
  } // no partner can jab

  // @heuristic::condition   it must not be an undetermined marriage
  if (    hi.game().is_undetermined_marriage()
      && !(   is_selector(color, hi.game().marriage_selector())
           && !hi.is_soloplayer())
     )
  {
    return Card::EMPTY;
  }

  // @heuristic::action   take last trump card
  if (   hi.hand().hastrump()
      && !hi.hand().contains(trick.startcard().color())
      && (hi.hand().numberoftrumps() <= 2)
      && (hi.hand().numberoftrumps() < hi.hand().cardsnumber() / 2) ) {
    HandCards const trumps = hi.hand().trumps();
    if (   (trumps[0].value() <= 3)
        && trumps[0].less(hi.lowest_trump_card_limit())) {
      if (trumps.size() == 1)
        return trumps[0];
      if (   (trumps[1].value() <= 3)
          && trumps[1].less(hi.lowest_trump_card_limit())) {
        return trumps.lowest_trump();
      }
    } // if (first trump small)
  } // if (hi.hand().numberoftrumps() <= 2)

  // @heuristic::action   take lowest color card
  Card const card = Heuristics::lowest_card(trick, hi.hand());
  if (   hi.lowest_trump_card_limit().less(HandCard(hi.hand(), card))
      && hi.hand().lowest_trump().less(card))
    return Card::EMPTY;
  // do not play a color ace blank in a solo
  if (   hi.game().is_solo()
      && (hi.hand().numberof(card.color()) == 2)
      && hi.hand().contains(card.color(), Card::ACE)
      && hi.cards_information().remaining_others(card.color(), Card::ACE)
     )
    return Card::EMPTY;
  return card;
} // Card Heuristics::serve_color_trick(Trick trick, HeuristicInterface hi)


/**
 ** If a player cannot get a trump trick take the card with the smallest value
 ** prerequisites:
 **   1. it must not be the first card in the trick
 **   2. the trick must be a trump trick;
 **   3. one cannot get the trick
 **   4. if the value of the cards are not the same:
 **      a) the winnerplayer must not be of the own team;
 **      b) the remaining players must not be of the own team
 **   5. There is a smallest card both in value and in jab order
 **      or diamond king against spade jack or higher
 ** The main case is p.e. in a jack solo to play the smallest jack when the
 ** club jack is played
 **
 ** @param        trick     current trick
 ** @param        hi        heuristic interface
 **
 ** @return       card to play, 'Card()' if the heuristic does not match
 **
 ** @author       Diether Knof
 ** @author       Borg Enders
 **
 ** @version      0.7.8
 **
 ** @todo         eventuell erweitern, dass der Spieler auch nur Farb-Zehnen auf der Hand hat
 **/
Card
Heuristics::serve_trump_trick(Trick const& trick,
                              HeuristicInterface const& hi)
{
  // @heuristic::name   serve trump trick
  // @heuristic::idea   When a player cannot get a trump trick take the card with the smallest value. The main case is that one is the last to play and has only got to take ones lowest card. This heuristik should be sorted behind the pfund heuristics.


  // @heuristic::condition   it must not be the first card in the trick
  if (trick.isstartcard())
    return Card::EMPTY;

  Game const& game = hi.game();
  Player const& player = hi.player();
  Hand const& hand = hi.hand();

  // @heuristic::condition   the trick must be a trump trick or the player has only color aces and trumps
  if (!trick.startcard().istrump()) {
    if (hand.numberof(trick.startcard().tcolor()))
      return Card::EMPTY;
    // special case: only ace of color on the hand and some trump
    if (hand.numberoftrumps() <= hand.cardsnumber() / 2)
      return Card::EMPTY;
    for (vector<Card::Color>::const_iterator
         c = game.rule().card_colors().begin();
         c != game.rule().card_colors().end();
         ++c) {
      if (hand.numberof(*c) > (Card(*c, Card::ACE).istrump(game)
                               ? 0
                               : hand.numberof(Card(*c, Card::ACE))))
        return Card::EMPTY;
    } // for (c)
  } // if (!trick.startcard().istrump())

  // @heuristic::condition   not: game is solo and solo player is behind
  if(     GAMETYPE::is_solo( game.type() )
     &&  (trick.cardno_of_player( player ) < trick.cardno_of_player( game.soloplayer()) ) )
    return Card::EMPTY;

  // @heuristic::condition   one cannot get the trick or has only 1/2 cards over the winnercard and over trump limit
  if (hand.higher_card_exists(trick.winnercard())) {
    // @heuristic::condition   there is no extrapoint in the trick
    if (trick.contains_possible_extrapoint(game))
      return Card::EMPTY;
    if (game.is_solo())
      return Card::EMPTY;
    // not many more trumps
    if (hand.numberoftrumps() <= 4)
      return Card::EMPTY;
    if (trick.winnercard().is_special())
      return Card::EMPTY;
    // not many cards over the winnercard
    if (   (hand.higher_cards_no(trick.winnercard())
            > (trick.game().trick_current_no() >= (trick.game().trickno() / 2)
               ? 1
               : 2))
        || (hand.higher_cards_no(trick.winnercard())
            > hand.higher_cards_no(hi.trump_card_limit()))
       )
      return Card::EMPTY;
    // not many points in the trick
    if (static_cast<int>(trick.points())
        >= (  (   trick.winnercard().less(hi.trump_card_limit())
               || (hand.numberoftrumps() >= hand.cardsnumber() / 2)
               || (
                   hand.numberoftrumps() > 3) )
            ? hi.value(Aiconfig::LIMITQUEEN)
            : hi.value(Aiconfig::LIMITDOLLE)))
      return Card::EMPTY;
  } // if (hand.higher_card_exists(trick.winnercard())

#ifdef OUTDATED
  // 0.7.9 2010-09-15 DK
  // @heuristic::condition   the player has no trump or at max 4 points in the low trumps
  if (   !hand.hastrump()
      || (  hand.contains(Card(game.trumpcolor(), Card::ACE))
          && !HandCard(hi.hand(), game.trumpcolor(), Card::ACE).is_special())
      || (  hand.contains(Card(game.trumpcolor(), Card::TEN))
          && !HandCard(hi.hand(), game.trumpcolor(), Card::TEN).is_special())
     ) {
    // heuristic::condition   1) the winnerplayer must not be of the own team
    Player const& winnerplayer = trick.winnerplayer();
    if (hi.teamofplayer(winnerplayer) == hi.team()) {
      return Card::EMPTY;
    }

    // heuristic::condition   2) the remaining players must not be of the own team
    for (Player const*
         player = &(game.player_following(trick.actplayer()));
         player != &trick.startplayer();
         player = &(game.player_following(*player)))
      if (   (hi.teamofplayer(*player) == hi.team())
          && hi.handofplayer(*player).higher_card_exists(trick.winnercard())) {
        return Card::EMPTY;
      }
  } // if (hand.highest_value(Card::TRUMP) > Card::KING)
#endif

  // heuristic::condition   the player has no trump or the lowest card has at max 4 points or is a jack (diamond/heart/spade)
  // @heuristic::action   play the lowest card
  Card const c = Heuristics::lowest_card(trick, hand);
  if (HandCard(hand, c).possible_genscher())
    return Card::EMPTY;
  if (   (c != hand.lowest_trump())
      && hi.lowest_trump_card_limit().less(HandCard(hand, c)))
    return Card::EMPTY;
  if (!hand.hastrump()
      || (   c.istrump(trick.game())
          && (c.value() <= Card::KING)))
    return c;
  else if (hand.contains(Card::DIAMOND_JACK)
           && Card::DIAMOND_JACK.istrump(game))
    return Card(Card::DIAMOND_JACK);
  else if (hand.contains(Card::HEART_JACK)
           && Card::HEART_JACK.istrump(game))
    return Card::HEART_JACK;
  else if (hand.contains(Card::SPADE_JACK)
           && Card::SPADE_JACK.istrump(game))
    return Card::SPADE_JACK;

  return Card::EMPTY;
} // Card Heuristics::serve_trump_trick(Trick const& trick, HeuristicInterface const& hi)


/**
 ** -> result
 **
 ** @param     trick   current trick
 ** @param     hand    hand of the player
 **
 ** @return    lowest card to play in the trick,
 **            Card::EMPTY, if none is found
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.5
 **/
Card
Heuristics::lowest_card(Trick const& trick,
                        Hand const& hand)
{
  if (trick.isstartcard())
    return Heuristics::lowest_free_card(hand);

  if (!hand.contains(trick.startcard().tcolor()))
    return Heuristics::lowest_free_card(hand);

  if (trick.startcard().istrump())
    return Heuristics::lowest_trump_card(hand);
  else
    return Heuristics::lowest_color_card(trick.startcard().tcolor(), hand);
} // static Card Heuristics::lowest_card(Trick trick, Hand hand)


/**
 ** -> result
 ** decision order:
 ** 1) normal trump with less than 10 points
 ** 2) normal trump
 ** 3) special trump
 **
 ** @param     hand    hand of the player
 **
 ** @return    lowest trump to play
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.5
 **/
Card
Heuristics::lowest_trump_card(Hand const& hand)
{
  DEBUG_ASSERTION(hand.contains(Card::TRUMP),
                  "Heuristics::lowest_trump_card(hand)\n"
                  "  hand does not contain any trump");

  HandCard const& lowest_trump = hand.lowest_trump();
  Game const& game = hand.game();

  if (   !game.is_special(lowest_trump)
      && (lowest_trump.value() < Card::TEN) )
    return lowest_trump;

  DEBUG_ASSERTION(!GAMETYPE::is_picture_solo(game.type()),
                  "Heuristics::lowest_trump_card(hand)\n"
                  "  picture solo: " << game.type());


  vector<Card> const& trumps = game.trumps();
  // search lowest trump with less then 10 points
  for (vector<Card>::const_iterator t = trumps.begin();
       t != trumps.end();
       ++t)
    if (hand.contains(*t))
      if (   !game.is_special(HandCard(hand, *t))
          && (t->value() < Card::TEN))
        return *t;

  // search lowest trump but no special card
  for (vector<Card>::const_iterator t = trumps.begin();
       t != trumps.end();
       ++t)
    if (hand.contains(*t))
      if (!game.is_special(HandCard(hand, *t)))
        return *t;
  // search lowest trump
  Card const* c = &Card::EMPTY;
  for (vector<Card>::const_iterator t = trumps.begin();
       t != trumps.end();
       ++t)
    if (hand.contains(*t)
        && (t->less(HandCard(hand, *c))))
      c = &*t;

  return *c;
} // static Card Heuristics::lowest_trump_card(Hand hand)


/**
 ** -> result
 **
 ** @param     tcolor   tcolor to serve
 ** @param     hand     hand of the player
 **
 ** @return    lowest card to play the tcolor
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.5
 **/
Card
Heuristics::lowest_color_card(Card::TColor const& tcolor,
                              Hand const& hand)
{
  DEBUG_ASSERTION((tcolor != Card::TRUMP),
                  "Heuristics::lowest_color_card(tcolor, hand)\n"
                  "  tcolor ist trump");
  DEBUG_ASSERTION(hand.contains(tcolor),
                  "Heuristics::lowest_color_card(tcolor, hand)\n"
                  "  hand does not contain the color " << tcolor);

  return hand.lowest_card(tcolor);
} // static Card Heuristics::lowest_color_card(Card::TColor tcolor, Hand hand)


/**
 ** -> result
 **
 ** @param     hand     hand of the player
 **
 ** @return    lowest card to play (free color selection),
 **            Card::EMPTY, if none is found
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.5
 **
 ** @todo      replace with Heuristics::choose_best_prefiltered_card()
 **/
Card
Heuristics::lowest_free_card(Hand const& hand)
{
  if (hand.numberoftrumps() == hand.cardsnumber())
    return Heuristics::lowest_trump_card(hand);

  // search the lowest value
  Card::Value lowest_value = Card::ACE;
  for (HandCards::const_iterator c = hand.cards().begin();
       c != hand.cards().end();
       ++c)
    if (c->value() < lowest_value)
      lowest_value = c->value();

  // the card colors with the lowest value and not trump
  vector<Card::Color> card_colors;
  for (vector<Card::Color>::const_iterator
       c = hand.game().rule().card_colors().begin();
       c != hand.game().rule().card_colors().end();
       ++c)
    if (!Card(*c, lowest_value).istrump(hand.game()))
      card_colors.push_back(*c);

  if (card_colors.empty())
    return Card::EMPTY;

  if (lowest_value < Card::ACE) {
    // search single card with lowest value
    for (vector<Card::Color>::const_iterator
         c = card_colors.begin();
         c != card_colors.end();
         ++c) {
      if (   (hand.numberof(*c) == 1)
          && (hand.contains(Card(*c, lowest_value)))) {
        return Card(*c, lowest_value);
      }
    }
  } // if (lowest_value < Card::ACE)

  if (lowest_value < Card::TEN) {
    // search lowest card of the smallest color without ACE and TEN
    Card::Color best_color = Card::NOCARDCOLOR;
    for (vector<Card::Color>::const_iterator
         c = card_colors.begin();
         c != card_colors.end();
         ++c)
      if (   (hand.numberof(Card(*c, Card::ACE)) == 0)
          && (hand.numberof(Card(*c, Card::TEN)) == 0))
        if (   (hand.numberof(*c) > 0)
            && (   (best_color == Card::NOCARDCOLOR)
                || (hand.numberof(*c) < hand.numberof(best_color)) ) )
          best_color = *c;
    if (best_color != Card::NOCARDCOLOR) {
      return hand.lowest_card(best_color);
    }
  } // if (lowest_value < Card::TEN)

  if (lowest_value == Card::ACE) {
    // search double ace
    for (vector<Card::Color>::const_iterator
         c = card_colors.begin();
         c != card_colors.end();
         ++c)
      if (hand.numberof(*c) == 2) {
        return Card(*c, lowest_value);
      }
  } // if (lowest_value == Card::ACE)

  if (lowest_value == Card::TEN) {
    // search only tens
    for (vector<Card::Color>::const_iterator
         c = card_colors.begin();
         c != card_colors.end();
         ++c)
      if (   hand.contains(Card(*c, lowest_value))
          && (hand.numberof(*c)
              == hand.numberof(Card(*c, lowest_value))) ) {
        return Card(*c, lowest_value);
      }

    // search biggest color
    Card::Color best_color = Card::NOCARDCOLOR;
    for (vector<Card::Color>::const_iterator
         c = card_colors.begin();
         c != card_colors.end();
         ++c) {
      if (hand.contains(Card(*c, lowest_value)))
        if (   (best_color == Card::NOCARDCOLOR)
            || (hand.numberof(*c) > hand.numberof(best_color)))
          best_color = *c;
    } // for (c \in card_colors())
    if (best_color != Card::NOCARDCOLOR) {
      return hand.lowest_card(best_color);
    }
  } // if (lowest_value == Card::TEN)

  { // search lowest card of the biggest color
    Card::Color best_color = Card::NOCARDCOLOR;
    unsigned best_cards_no = 2;
    for (vector<Card::Color>::const_iterator
         c = card_colors.begin();
         c != card_colors.end();
         ++c)
      if (hand.numberof(*c) > best_cards_no) {
        best_color = *c;
        best_cards_no = hand.numberof(*c);
      }
    if (best_color != Card::NOCARDCOLOR) {
      return hand.lowest_card(best_color);
    }
  } // search lowest card of the biggest color

  { // search lowest card
    Card::Color best_color = Card::NOCARDCOLOR;
    for (vector<Card::Color>::const_iterator
         c = card_colors.begin();
         c != card_colors.end();
         ++c)
      if (   hand.contains(Card(*c, lowest_value))
          && (   (best_color == Card::NOCARDCOLOR)
              || (hand.numberof(*c) > hand.numberof(best_color))))
        best_color = *c;
    if (best_color != Card::NOCARDCOLOR) {
      return hand.lowest_card(best_color);
    }
  } // search lowest card

  return Card::EMPTY;
} // Card Heuristics::lowest_free_card(Hand hand)

/**
 ** -> result
 **
 ** @param     hi       heuristic interface
 ** @param     hand     hand of the player
 ** @param     cards    cards to choose from
 **
 ** @return    lowest card to play from 'cards'
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.5
 **/
Card
Heuristics::choose_best_prefiltered_card(Trick const& trick,
                                         HeuristicInterface const& hi,
                                         Hand const& hand,
                                         HandCards const& cards)
{
  if (cards.cardsnumber() == 0)
    return Card::EMPTY;
  if (cards.cardsnumber() == 1)
    return cards[0];

  if (cards.numberoftrumps() < cards.cardsnumber())
    return Heuristics::choose_best_prefiltered_color_card(trick, hi,
                                                          hand, cards);
  else
    return Heuristics::choose_best_prefiltered_trump_card(trick, hi,
                                                          hand, cards);
} // Card Heuristics::choose_best_prefiltered_card(Trick trick, HeuristicsInterface hi, Hand hand, HandCards cards)

/**
 ** -> result
 **
 ** @param     hi       heuristic interface
 ** @param     hand     hand of the player
 ** @param     cards    cards to choose from
 **
 ** @return    lowest card to play from 'cards',
 **            Card::EMPTY, if none is found
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.5
 **/
Card
Heuristics::choose_best_prefiltered_color_card(Trick const& trick,
                                               HeuristicInterface const& hi,
                                               Hand const& hand,
                                               HandCards const& cards)
{
  DEBUG_ASSERTION(cards.numberoftrumps() < cards.cardsnumber(),
                  "Heuristics::choose_best_prefilterd_color_card(hand, cards)\n"
                  "  'cards' only contains trumps");
  if (cards.cardsnumber() == 0)
    return Card::EMPTY;
  if (cards.cardsnumber() == 1)
    return cards[0];

  Game const& game = hand.game();
  Rule const& rule = game.rule();

  // whether the trick goes to the own team
  bool const own_team = (   !trick.isempty() 
                         && (trick.winnerteam() == hi.team()));

  if (own_team) {
    // search a single ace
    for (vector<Card::Color>::const_iterator
         c = rule.card_colors().begin();
         c != rule.card_colors().end();
         ++c) {
      if (   !Card(*c, Card::ACE).istrump(game)
          && (hand.numberof(*c) == 1)
          && (cards.contains(*c, Card::ACE))) {
        // ToDo: consider two colors with the same number of cards
        return Card(*c, Card::ACE);
      }
    } // for (c \in rule.card_colors())
    // search single ten
    for (vector<Card::Color>::const_iterator
         c = rule.card_colors().begin();
         c != rule.card_colors().end();
         ++c) {
      if (   !Card(*c, Card::TEN).istrump(game)
          && (hand.numberof(*c) == 1)
          && (cards.contains(*c, Card::TEN))) {
        // ToDo: consider two colors with the same number of cards
        return Card(*c, Card::TEN);
      }
    } // for (c \in rule.card_colors())
  } // if (own team)

  // search the lowest / highest value
  Card::Value best_value = (own_team ? Card::NINE : Card::ACE);
  for (vector<HandCard>::const_iterator c = cards.begin();
       c != cards.end();
       ++c) {
    if (   !c->istrump()
        && (own_team
            ? (c->value() > best_value)
            : (c->value() < best_value) )
       ) {
      best_value = c->value();
    }
  } // for (c)

  // the card colors with the best value and not trump
  vector<Card::Color> card_colors;
  for (vector<Card::Color>::const_iterator
       c = rule.card_colors().begin();
       c != rule.card_colors().end();
       ++c)
    if (   !Card(*c, best_value).istrump(hand.game())
        && cards.contains(*c, best_value) )
      card_colors.push_back(*c);

  if (card_colors.empty())
    return Card::EMPTY;


  if (best_value < Card::ACE) {
    // search single card with best value
    for (vector<Card::Color>::const_iterator
         c = card_colors.begin();
         c != card_colors.end();
         ++c)
      if (   (hand.numberof(*c) == 1)
          && (cards.contains(*c, best_value))) {
        // ToDo: consider two colors with the same number of cards
        return Card(*c, best_value);
      }
  } // if (best_value < Card::ACE)

  if (best_value < Card::TEN) {
    // search best card of the smallest color without ACE and TEN
    Card::Color best_color = Card::NOCARDCOLOR;
    for (vector<Card::Color>::const_iterator
         c = card_colors.begin();
         c != card_colors.end();
         ++c)
      if (   cards.contains(*c)
          && (hand.numberof(Card(*c, Card::ACE)) == 0)
          && (hand.numberof(Card(*c, Card::TEN)) == 0))
        if (   (best_color == Card::NOCARDCOLOR)
            || (cards.numberof(*c) < hand.numberof(best_color)))
          // ToDo: improve selection -- card value comparison, compare remaining cards of the color, compare jabbing, ...
          best_color = *c;
    if (best_color != Card::NOCARDCOLOR) {
      return (own_team
              ? cards.highest_card(best_color)
              : cards.lowest_card(best_color));
    }
  } // if (best_value < Card::TEN)

  if (best_value == Card::ACE) {
    // search double ace
    for (vector<Card::Color>::const_iterator
         c = card_colors.begin();
         c != card_colors.end();
         ++c) {
      if (   cards.contains(*c, best_value)
          && (hand.numberof(*c, best_value) == 2) ) {
        return Card(*c, best_value);
      }
    } // for (c \in card_colors)
  } // if (best_value == Card::ACE)

  if (best_value == Card::TEN) {
    // search only tens
    for (vector<Card::Color>::const_iterator
         c = card_colors.begin();
         c != card_colors.end();
         ++c)
      if (   cards.contains(*c, best_value)
          && (hand.numberof(*c)
              == hand.numberof(*c, best_value)) ) {
        return Card(*c, best_value);
      }
  } // if (best_value == Card::TEN)

  { // search best card of the biggest color with min 3 cards
    Card::Color best_color = Card::NOCARDCOLOR;
    unsigned best_cards_no = 2;
    for (vector<Card::Color>::const_iterator
         c = card_colors.begin();
         c != card_colors.end();
         ++c)
      if (   (hand.numberof(*c) > best_cards_no)
          && (cards.contains(*c)) ) {
        best_color = *c;
        best_cards_no = hand.numberof(*c);
      }
    if (   (best_color != Card::NOCARDCOLOR)
        && cards.contains(hand.lowest_card(best_color)) ) {
      // ToDo: take smallest card from cards
      return (own_team
              ? cards.highest_card(best_color)
              : cards.lowest_card(best_color));
    }
  } // search best card of the biggest color

  { // search best card of the largest color
    Card::Color best_color = Card::NOCARDCOLOR;
    for (vector<Card::Color>::const_iterator
         c = card_colors.begin();
         c != card_colors.end();
         ++c)
      if (   cards.contains(Card(*c, best_value))
          && (   (best_color == Card::NOCARDCOLOR)
              || (hand.numberof(*c) > hand.numberof(best_color))))
        best_color = *c;
    if (best_color != Card::NOCARDCOLOR) {
      return (own_team
              ? cards.highest_card(best_color)
              : cards.lowest_card(best_color));
    }
  } // search best card

  return Card::EMPTY;
} // static Card Heuristics::choose_best_prefiltered_color_card(Trick trick, HeuristicsInterface hi, Hand hand, HandCards cards)


/**
 ** -> result
 **
 ** @param     hi       heuristic interface
 ** @param     hand     hand of the player
 ** @param     cards    cards to choose from
 **
 ** @return    lowest card to play from 'cards'
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.5
 **/
Card
Heuristics::choose_best_prefiltered_trump_card(Trick const& trick,
                                               HeuristicInterface const& hi,
                                               Hand const& hand,
                                               HandCards const& cards)
{
  DEBUG_ASSERTION(cards.numberoftrumps() == cards.cardsnumber(),
                  "Heuristics::choose_best_prefilterd_trump_card(hand, cards)\n"
                  "  'cards' does contain color cards");
  if (cards.cardsnumber() == 0)
    return Card::EMPTY;
  if (cards.cardsnumber() == 1)
    return cards[0];

  // whether the trick goes to the own team
  bool const own_team = (trick.winnerteam() == hi.team());

  Game const& game = hand.game();
  if (own_team
      && (game.trumpcolor() != Card::NOCARDCOLOR)) {
    if (cards.contains(Card(game.trumpcolor(), Card::ACE)))
      return Card(game.trumpcolor(), Card::ACE);
    if (cards.contains(Card(game.trumpcolor(), Card::TEN)))
      return Card(game.trumpcolor(), Card::TEN);
    if (cards.contains(Card(game.trumpcolor(), Card::KING)))
      return Card(game.trumpcolor(), Card::KING);
  } // if (own_team) {

  HandCard card(hand);

  // find any card that's allowed for this trick
  for(unsigned i = 0; i < cards.cardsnumber(); i++ ) {
    if ( cards.card( i ).istrump() ) {
      card = cards.card( i );
      break;
    }
  } // for( i < ha.cardsnumber() )

  if( card.is_empty())
    return Card::EMPTY;

  // find a better card
  for(unsigned i = 0; i < cards.cardsnumber(); i++ ) {
    if(   cards.card( i ).less(card)
       && cards.card( i ).istrump()
       && !cards.card( i ).isfox()
       && (   cards.card(i).value() != Card::TEN
           || card.isfox()
          )
      )
      card = cards.card( i );
  } // for( i = 0; i < cards.cardsnumber(); i++ )

  if( !card.less( hi.trump_card_limit() ) ) {
    // maybe a diamond ten isn't all that bad
    for(unsigned i = 0; i < cards.cardsnumber(); i++ ) {
      if(   cards.card( i ).less( card )
         && cards.card( i ).istrump()
         && !cards.card( i ).isfox()
        )
        card = cards.card( i );
    }
  } // if( !card.less( cards.trump_card_limit() ) )

  if( !card.less( hi.trump_card_limit() ) ) {
    // maybe a diamond Ace isn't all that bad
    for(unsigned i = 0; i < cards.cardsnumber(); i++ ) {
      if(   cards.card( i ).less( card )
         && cards.card(i).istrump()
        )
        card = cards.card( i );
    }
  } // if( !c.less( hi.trump_card_limit() ) )

  return card;
} // Card Heuristics::choose_best_prefiltered_trump_card(Trick trick, HeuristicsInterface hi, Hand hand, HandCards cards)

/**
 ** for the last card in a trick, which wins your own team
 ** or where your teammate has already a played a high enough trump
 ** play a pfund
 **
 ** @param        t         current trick
 ** @param        hi        heuristic interface
 **
 ** @return       card to play, 'Card()' if the heuristic does not match
 **
 ** @author    Borg Enders
 **
 ** @version      Beta
 **/
Card
Heuristics::choose_pfund( Trick const& t, HeuristicInterface const& hi )
{
  // @heuristic::name   choose pfund
  // @heuristic::idea   choose for trump trick won by the own team a pfund

  // I'm the start Player
  if( t.isstartcard() )
    return Card::EMPTY;

  HandCard const pfund_card = HandCard(hi.hand(), choose_pfund_card( t, hi ));
  if (pfund_card == Card::EMPTY)
    return Card::EMPTY;
  if (   (pfund_card.value() <= 2)
      && !hi.guessed_same_team(t.winnerplayer())
      && hi.hand().can_jab(t))
    return Card::EMPTY;

  // undetermined marriage => no pfund
  if (   hi.game().is_undetermined_marriage()
      && (  (  !is_selector(t.startcard().color(), hi.game().marriage_selector())
             && hi.is_soloplayer())
          || !hi.is_soloplayer()
         )
     )
    return Card::EMPTY;

  // picture solo, trump trick, has to serve => no pfund
  if (   GAMETYPE::is_picture_solo(hi.game().type())
      && t.startcard().istrump()
      && hi.hand().hastrump())
    return Card::EMPTY;

  // last player
  if  (t.islastcard()) {
    if (hi.same_team(t.winnerplayer()))
      return pfund_card;
    if (!hi.guessed_same_team(t.winnerplayer()))
      return Card::EMPTY;
    // guessed same team but not sure
    if (hi.value(Aiconfig::TRUSTING))
      return pfund_card;

    // has to get two foxes home
    if (   pfund_card.isfox()
        && !hi.game().rule()(Rule::SWINES)
        && (hi.hand().numberof(pfund_card) > 1))
      return pfund_card;
    // have many pfund cards
    if (   (hi.hand().numberofrichcards() >= 3)
        || (hi.hand().numberofrichcards() * 2 >= hi.hand().cardsnumber()))
      return pfund_card;

    return Card::EMPTY;
  } // if  (t.islastcard())

  // special case: first color run
  Card::Color color = t.startcard().tcolor();
  if (   (color != Card::TRUMP)
      && (hi.color_runs(color) == 0)
      && (hi.teamofplayer(t.winnerplayer()) == hi.team())
      && (t.winnercard().istrump()
          || (t.winnercard() == Card(color, Card::ACE)))
      && (hi.cards_information().remaining_others(color)
          >= t.remainingcardno() - 1)
     ) {
    return pfund_card;
  }

  // special case: second color run
  if (   (color != Card::TRUMP)
      && (hi.color_runs(color) == 1)
      && (hi.teamofplayer(t.winnerplayer()) == hi.team())
      && (t.winnercard().istrump()
          || (t.winnercard() == Card(color, Card::ACE)))
      && (hi.cards_information().remaining_others(color)
          >= t.remainingcardno() - 1 + 1)
     ) {
    for (Player const*
         player = &(t.game().player_following(t.actplayer()));
         player != &t.startplayer();
         player = &(t.game().player_following(*player))) {
      if (   (hi.teamofplayer(*player) != hi.team())
          && (hi.handofplayer(*player).numberof(color) < 2)) {
        return Card::EMPTY;
      }
    } // for (player)
    return pfund_card;
  }

  // no opponent behind me can jab the trick
  if(   !t.islastcard()
     && !checkAllMyTeam( t, hi )
     && oppositeTeamCanWinTrick( t, hi ) ) {
    return Card::EMPTY;
  }

  bool const soloPlayer = soloPlayerBehind( t, hi );

  // if winner is own team
  if (   hi.teamofplayer(t.winnerplayer()) == hi.team()
      &&  !oppositeTeamCanWinTrick( t, hi ) ) {
    HandCards const cards = hi.hand().validcards(t);
    if (cards.lowest_value() == cards.highest_value())
      return Card::EMPTY;
    // small pfund an the last player in the next trick is of the opposite team
    if (   (pfund_card.value() <= 4)
        && cards.higher_card_exists(pfund_card)
        && t.winnercard().less(hi.trump_card_limit())
        && !hi.same_team(hi.game().player_previous(t.winnerplayer()))
       ) {
      if (!hi.same_team(hi.game().player_following(t.actplayer())))
        return Card::EMPTY;
      if (best_winning_card(t, hi).value() == pfund_card.value())
        return Card::EMPTY;
    }
    return pfund_card;
  }

  if (   hi.guessed_same_team(t.winnerplayer())
      && (  !hi.cards_information().higher_card_exists( t.winnercard() )
          ||(  !soloPlayer
             && (    t.islastcard()
                 || !t.winnercard().less( hi.trump_card_limit() )
                )
            )
         )
     )
  {
    return pfund_card;
  }

  if (   hi.game().is_solo()
      && t.winnerplayer() == hi.game().soloplayer()
      && t.winnercard().less( hi.trump_card_limit() )
      && !t.islastcard()
      && choose_pfund_card(t,hi).points() > 0
      && t.startcard().istrump()
     )
  {
    return pfund_card;
  }

  if (   checkAllMyTeam( t, hi )
      && ownTeamCanWinTrick( t, hi )
      && (    hi.teamofplayer(t.winnerplayer()) == hi.team()
          || (   t.winnercard().less( hi.trump_card_limit() )
              && (t.winnercard() != Card(color, Card::ACE))
              && !(!t.startcard().istrump() && t.winnercard().istrump())
              && !t.islastcard()
             )
         )
     )
  {
    return pfund_card;
  }

  return Card::EMPTY;
} // Card Heuristics::choose_pfund( Trick t, HeuristicInterface hi )


/**
 ** if the partner has the highest card in the game, play a pfund (>= 10 points)
 **
 ** @param     trick   current trick
 ** @param     hi      heuristics interface
 **
 ** @return    card to play
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.9
 **/
Card
Heuristics::choose_pfund_before_partner(Trick const& trick,
                                        HeuristicInterface const& hi)
{
  // @heuristic::name   choose pfund before partner
  // @heuristic::idea   if the partner has the highest card in the game play a pfund

  // @heuristic::condition   the partner has the highest trump still in the game

  // whether the partner has the highest card
  bool partner_has_highest_card = false;

  // the highest trump the other players can have
  Card const highest_trump
    = hi.cards_information().highest_remaining_trump_of_others();
  // the trick can be jabbed with the highest trump
  if (   !trick.isempty()
      && !trick.isjabbed(highest_trump))
    return Card::EMPTY;

  // the partner
  Player const* partner = hi.team_information().guessed_partner();

  // special case: first color run in a solo game, soloplayer behind
  if (   hi.game().is_solo()
      && (hi.team() == TEAM::CONTRA)
      && !trick.isempty()
      && !trick.startcard().istrump()
      && !trick.has_played(hi.game().soloplayer())
      && (   (trick.winnercard().value() == Card::ACE)
          || hi.same_team(trick.player_of_card(trick.actcardno() + 1)) )
      && (hi.color_runs(trick.startcard().tcolor()) == 0)
      && hi.handofplayer(hi.game().soloplayer()).contains(trick.startcard().tcolor())
     ) {
    return Heuristics::choose_pfund_card(trick, hi);
  }

  // color trick and the partner has to serve the color
  if (   !trick.isempty()
      && !trick.startcard().istrump()
      && hi.cards_information().remaining_others(trick.startcard().tcolor())
      && (   !partner
          || hi.handofplayer(*partner).contains(trick.startcard().tcolor())))
    return Card::EMPTY;

  if (GAMETYPE::is_solo(hi.game().type())) {
    if (hi.team() == TEAM::CONTRA) {
      if (   trick.has_played(hi.game().soloplayer())
          && trick.winnercard().less(highest_card_behind_of_same_team(trick, hi))
         ) {
        partner_has_highest_card = true;
      }
    } // if (hi.team() == TEAM::CONTRA)
  } else if (partner) {
    // no partner behind
    if (trick.cardno_of_player(*partner) < trick.actcardno() )
      return Card::EMPTY;

    // normal case
    if ( // the partner has the highest card
        hi.cards_information().of_player(*partner).must_have(highest_trump)
        // no other player then the partner has the highest card
        && (hi.cards_information().of_player(*partner).must_have(highest_trump)
            + hi.cards_information().played(highest_trump)
            + hi.hand().numberof(highest_trump)
            == hi.game().rule()(Rule::NUMBER_OF_SAME_CARDS) )
       )
      partner_has_highest_card = true;

    // special case: only the partner behind
    if (trick.actcardno() == hi.game().playerno() - 2) {
      // check, wether he has a card higher then the played one
      if (   (hi.cards_information().of_player(*partner).must_have_cards().higher_card_exists(trick.winnercard()))
          || (   trick.winnercard().less(hi.trump_card_limit())
              && hi.handofplayer(*partner).higher_card_exists(trick.winnercard())))
        partner_has_highest_card = true;
      // the player has more then one pfund
      unsigned const pfund_no
        = ((hi.hand().has_swines() ? 0 : hi.hand().numberoftrumpaces())
           + hi.hand().numberof(hi.game().trumpcolor(), Card::TEN));

      if (   !hi.game().is_solo()
          && (pfund_no >= 2)
          && hi.handofplayer(*partner).higher_card_exists(trick.winnercard())) {
        if (   is_solo(trick.game().type())
            || trick.winnercard().less(hi.value(Aiconfig::LIMITHIGH))
            || (hi.hand().cardsnumber() < pfund_no * 3)
            || (   (hi.game().swines_owner() == partner)
                && hi.handofplayer(*partner).contains(hi.game().swine()))
            || (   (hi.game().hyperswines_owner() == partner)
                && hi.handofplayer(*partner).contains(hi.game().hyperswine()))
            || trick.points() >= 10)
          partner_has_highest_card = true;
      }

    } // if (only partner behind)
  } else { // if !(partner)
    // special case: partner re but I do not know who he is
    if (   (hi.game().type() == GAMETYPE::NORMAL)
        // re team
        && (hi.team() == TEAM::RE)
        // startcard or the player before seems to be contra
        && (   trick.isstartcard()
            || (   (trick.actcardno() == 1)
                && (hi.guessedteamofplayer(trick.startplayer()) == TEAM::CONTRA))
           )
       ) {
      // the club queen is the highest card
      if (highest_trump == Card::CLUB_QUEEN)
        partner_has_highest_card = true;

    }
  } // if !(partner)

  // special case: partner unknown, but club queen not the highest card
  if (   !partner_has_highest_card
      && (hi.game().type() == GAMETYPE::NORMAL)
      && (hi.team() == TEAM::RE)
      && (hi.cards_information().played(Card::CLUB_QUEEN)
          == hi.cards_information().of_player(hi.player()).played(Card::CLUB_QUEEN) )
      && hi.hand().contains(hi.game().trumpcolor(), Card::KING)
      && trick.isvalid(Card(hi.game().trumpcolor(), Card::KING), hi.hand())
      && (   trick.isstartcard()
          || (   (trick.actcardno() == 1)
              && (hi.guessedteamofplayer(trick.startplayer()) == TEAM::CONTRA)) )
      && !HandCard(hi.hand(),
                   hi.game().trumpcolor(), Card::KING).is_special()
      && !(   hi.hand().contains(Card(hi.game().trumpcolor(), Card::TEN))
           && !HandCard(hi.hand(),
                        hi.game().trumpcolor(), Card::TEN).is_special())
      && !(   hi.hand().contains(Card(hi.game().trumpcolor(), Card::ACE))
           && !HandCard(hi.hand(),
                        hi.game().trumpcolor(), Card::ACE).is_special())
     ) {
    return Card(hi.game().trumpcolor(), Card::KING);
  }

  if (!partner_has_highest_card)
    return Card::EMPTY;

  // @heuristic::action   play a pfund of at least 10 points
  if (trick.isstartcard()) {
    // play a fox
    if (   (hi.hand().numberoftrumpaces() > 0)
        && !hi.hand().has_swines())
      return Card(hi.game().trumpcolor(), Card::ACE);

    // play a diamond ten
    Card const card(hi.game().trumpcolor(), Card::TEN);
    if (   hi.hand().contains(card)
        && !HandCard(hi.hand(), card).is_special())
      return card;

    // ToDo: play a color card of a color the partner does not have
    // ToDo: play a many points for the partner

  } else { // if !(trick.isstartcard())
    // the partner can play the card
    if (   trick.isempty()
        || trick.startcard().istrump()
        || (   partner
            && (trick.isvalid(hi.handofplayer(*partner).highest_trump(),
                              hi.handofplayer(*partner))))
        || (   hi.game().is_solo()
            && (hi.team() == TEAM::CONTRA)
            && trick.has_played(hi.game().soloplayer())
            && !trick.startcard().istrump()
            && (   hi.handofplayer(trick.lastplayer()).can_jab(trick)
                || (   (trick.actcardno() == hi.game().playerno() - 2)
                    && hi.handofplayer(trick.secondlastplayer()).can_jab(trick))
               )
           )
       ) {
      // search a pfund card
      Card const card = Heuristics::choose_pfund_card(trick, hi);
      if (   !is_solo(trick.game().type())
          && !trick.winnercard().less(hi.value(Aiconfig::LIMITHIGH))
          && HandCard(hi.hand(), card).isfox()
          && (hi.hand().numberof(card) == 1)
          && hi.hand().contains(Card::DIAMOND_TEN))
        return Card::DIAMOND_TEN;
      if (card.value() >= Card::TEN)
        return card;
    }
  } // if !(trick.isstartcard())

  return Card::EMPTY;
} // Card Heuristics::choose_pfund_before_partner(Trick trick, HeuristicInterface hi)


/**
 ** return lowest allowed Jack or Queen if they are trump
 ** and there is an ace to play, which is not already jabbed,
 ** else return empty card
 **
 ** @param     t    current trick
 ** @param     hi   heuristic interface
 **
 ** @result    -> description
 **
 ** @author    Borg Enders
 **
 ** @version 0.6.9
 **/
Card
Heuristics::jab_for_ace( Trick const& t, HeuristicInterface const& hi )
{
  // @heuristic::name   jab for ace
  // @heuristic::idea   jab a trick to serve next trick a ace

  // must not be the first player
  if (t.isstartcard())
    return Card::EMPTY;

  // must be a trump trick or a color trick without having to serve
  if (   !t.startcard().istrump()
      && hi.hand().contains(t.startcard().tcolor()))
    return Card::EMPTY;

  // do not jab as marriage player in a decision trick
  if (   hi.game().is_undetermined_marriage()
      && hi.is_soloplayer()
      && (is_selector(t.startcard().tcolor(),
                      hi.game().marriage_selector()) ) )
    return Card::EMPTY;

  // do not overjab the partner
  if (   hi.guessed_same_team(t.winnerplayer())
      && t.winnercard().istrump()
      && !t.winnercard().less(hi.lowest_trump_card_limit()) ) {
    // check that all players behind do have no ace to play
    // since the winnerplayer is of the own team, the remaining players must be of the opposite team
    if (t.actcardno() == 2) {
      // the last player must serve the trick
      if (   !t.startcard().istrump()
          && hi.handofplayer(t.lastplayer()).contains(t.startcard().tcolor()))
        return Card::EMPTY;
      vector<Card::Color>::const_iterator c;
      for (c = hi.game().rule().card_colors().begin();
           c != hi.game().rule().card_colors().end();
           ++c)
        if (   (hi.color_runs(*c) == 0)
            && hi.handofplayer(t.lastplayer()).contains(*c, Card::ACE))
          break;
      if (c == hi.game().rule().card_colors().end())
        return Card::EMPTY;
    } // if (t.actcardno() == 2)
  } // do not overjab the partner


  // the ace to jab for
  Card const ace = Heuristics::choose_ace( Trick(t.startplayer()), hi );

  if (ace.is_empty())
    return Card::EMPTY;

  unsigned const remaining_cards
    = (hi.game().numberof(ace.color())
       - (hi.cards_information().played(ace.color()) + hi.hand().numberof(ace.color())));
  if(   GAMETYPE::is_solo( hi.game().type() )
     && !hi.is_soloplayer()
     && remaining_cards < 5 )
    return Card::EMPTY;


  // for a solo don't jab for ace, if there are players of my team behind me
  // and myself doesn't have that many trumps
  if (   hi.game().is_solo()
      && (t.winnerplayer() == hi.game().soloplayer())
      && t.winnercard().less( hi.trump_card_limit() )
      && !t.islastcard()
      && (hi.hand().numberoftrumps() < 4)
     )
    return Card::EMPTY;
  // solo game, color trick, goes to own team
  if (   hi.game().is_solo()
      && (hi.team() == TEAM::CONTRA)
      && !t.startcard().istrump()
      && (t.winnerplayer() != hi.game().soloplayer())
      && (   t.has_played(hi.game().soloplayer())
          || (   (   (t.winnercard().value() == Card::ACE)
                  || hi.same_team(t.player_of_card(t.actcardno() + 1)) )
              && (hi.color_runs(t.startcard().tcolor()) == 0)
              && hi.handofplayer(hi.game().soloplayer()).contains(t.startcard().tcolor())))
      && (hi.hand().numberoftrumps() < (hi.game().tricks_remaining_no() * 2) / 3)
     )
    return Card::EMPTY;

  if(   (hi.hand().numberof( ace )
         < hi.game().rule()(Rule::NUMBER_OF_SAME_CARDS))
     && (hi.color_runs(ace.color()) == 0)
     && (   (TEAM::maybe_to_team(hi.teamofplayer(t.winnerplayer())) != hi.team())
         || (   (TEAM::maybe_to_team(hi.teamofplayer(t.winnerplayer())) == hi.team())
             && t.winnercard().less( hi.trump_card_limit() )
            )
         || (hi.game().trick_current_no() == 0)
        )
    )
  {
    // do not overjab the partner, if he has played a high card
    // normal case: do not overjab a club queen
    if (   (   GAMETYPE::is_normal(hi.game().type())
            || GAMETYPE::is_marriage(hi.game().type())
            || GAMETYPE::is_poverty(hi.game().type())
            || GAMETYPE::is_color_solo(hi.game().type()))
        && (TEAM::maybe_to_team(hi.teamofplayer(t.winnerplayer())) == hi.team())
        && !t.winnercard().less(Card::CLUB_QUEEN) )
      return Card::EMPTY;

    Card c;
    if(   (hi.game().trick_current_no() == 0)
       && t.startcard().istrump()
       && !t.islastcard()
       && !checkAllMyTeam(t, hi )) {
      c = highest_trump_card( t, hi.hand() );
      if (!t.isjabbed(c))
        return Card::EMPTY;

    } else {
#ifdef OUTDATED
      // 0.7.11 (2011-09-04)
      c =  best_winning_card_old( t, hi,
                                 hi.value( Aiconfig::LIMITDOLLE ) - ( 4 - t.actcardno() ) + 10 );
#endif
      c = best_jabbing_card(t, hi);

    }

    if( !c.is_empty() ) {
      if (  !t.startcard().istrump()
          && (hi.color_runs(t.startcard().tcolor()) == 0))
        return c;

      if (   hi.game().is_undetermined_marriage()
          && (is_selector(t.startcard().color(), hi.game().marriage_selector()))
          && (hi.team() == TEAM::RE)
         )
        return c;

      Trick tempTrick = t;
      tempTrick += HandCard( hi.hand(), c );
      if (oppositeTeamCanWinTrick( tempTrick, hi ) ) {
        // as second last player or if both players behind are of the opposite team, try with the highest trump
        if (   (   (t.remainingcardno() == 2)
                && (hi.game().trumpcolor() != Card::NOCARDCOLOR))
            || (   (t.remainingcardno() == 3)
                && (hi.guessedteamofplayer(t.startplayer()) == hi.team())
                && t.winnercard().less(hi.trump_card_limit()))
           ) {
          c = hi.hand().highest_trump();
          tempTrick = t;
          tempTrick += HandCard(hi.hand(), c);
          if (oppositeTeamCanWinTrick(tempTrick, hi))
            c = hi.hand().highest_trump_till(Card::CLUB_QUEEN);
          if (!t.winnercard().less(c))
            return Card::EMPTY;
        } else {
          return Card::EMPTY;
        }

        tempTrick = t;
        tempTrick += HandCard(hi.hand(), c);
      } // if (opposite team can win the trick)

      bool trickLost = tempTrick.winnercard().less(highest_card_behind_of_opposite_team(tempTrick, hi));
      if(   trickLost
         && (!c.less(HandCard( hi.hand(), Card(Card::DIAMOND, Card::QUEEN))))
         && (hi.hand().numberof(Card::QUEEN) < 3)
        )
        return Card::EMPTY;
    }

    { // only take a high card if there are enough single aces or many points
      // count the single aces
      unsigned single_aces = 0;
      for (vector<Card::Color>::const_iterator
           color = hi.game().rule().card_colors().begin();
           color != hi.game().rule().card_colors().end();
           ++color) {
        Card const ace = Card(*color, Card::ACE);
        if (   !ace.istrump(hi.game())
            && (hi.hand().numberof(ace) == 1)
            && (hi.cards_information().played(*color) == 0))
          single_aces += 1;
      }
      if (   hi.game().less(Card::CLUB_QUEEN, c)
          && (single_aces < 2)
          && (t.points() < 2 + 4 * t.actcardno()))
        return Card::EMPTY;
      if (   hi.game().less(hi.trump_card_limit(), c)
          && (single_aces == 0)
          && (t.points() < 2 + 4 * t.actcardno()))
        return Card::EMPTY;
    } // only take a high card if there are enough single aces


    return c;
  } // if ( .... (hi.color_runs(ace.color()) == 0) .... )

  return Card::EMPTY;
} // Card Heuristics::jab_for_ace( Trick t, HeuristicInterface hi )


/**
 ** plays high and low trumps changing
 **
 ** @param        t         current trick
 ** @param        hi        heuristic interface
 **
 ** @return       card to play, 'Card()' if the heuristic does not match
 **
 ** @author    Borg Enders
 **
 ** @version      Beta
 **/
Card
Heuristics::play_low_high(Trick const& t, HeuristicInterface const& hi)
{
  // @heuristic::name   play low high
  // @heuristic::idea   play changing low and high trumps to force opponent to jab or either draw his trumps

  if(    GAMETYPE::is_solo( hi.game().type() )
     && !hi.is_soloplayer() )
    return Card::EMPTY;

  // @heuristic::condition do not use for undetermined marriage
  if (hi.game().is_undetermined_marriage())
    return Card::EMPTY;

  // @heuristic::condition trump cards at the opposite players still remain
  if (!opposite_trump_behind(t, hi))
    return Card::EMPTY;


  if(   t.isstartcard()
     && (hi.hand().numberoftrumps() > hi.game().tricks_remaining_no() / 2)
     && (hi.hand().higher_cards_no(Card(Card::DIAMOND, Card::JACK))
         + hi.hand().numberof(Card::DIAMOND, Card::JACK)
         > 3)
    )
  {
    if(  hi.next_low()
       || (hi.game().trick_current_no() < 4) )
    {
      if( hi.hand().numberof( Card::QUEEN ) < 2 * hi.hand().numberof( Card::JACK ) )
      {
        Card c = lowest_jack( hi.hand(), hi );

        if(   !c.is_empty()
           && !t.isvalid(c,hi.hand())) // ToDo: Hier wird doch die erste Karte im Trumpf betrachtet, daher dürfte die Bedingung nie greifen (siehe auch unten)
             c = Card::EMPTY;

        return c;
      }
    } else
    { // if !(hi.next_low())
      if( hi.hand().numberof( Card::QUEEN ) > 1)
      {
        Card c = lowest_queen( hi.hand(), hi );

        if(   (hi.hand().numberof( Card::QUEEN ) < 4)
           && (hi.hand().numberofclubqueens() == 1)
           && (hi.game().teaminfo( hi.player() ) == TEAM::UNKNOWN)
          )
          c = Card::CLUB_QUEEN;


        if(   !c.is_empty()
           && !t.isvalid( c, hi.hand() ) )
          c = Card::EMPTY;


        return c;
      }
    } // if !(hi.next_low())
  } // if (t.isstartcard())

  return Card::EMPTY;
} // Card Heuristics::play_low_high(Trick t,HeuristicInterface hi)


/**
 ** plays the second highest trump (usefull for color soli)
 **
 ** @param        hi        heuristic interface
 **
 ** @return       card to play, 'Card()' if the heuristic does not match
 **
 ** @author    Borg Enders
 **
 ** @version      Beta
 **/
Card
Heuristics::SecondBestTrump(HeuristicInterface const& hi)
{
  // @heuristic::name   ?  second best trump  ?
  // @heuristic::idea   play the seconmd highest trump (especially useful for solo player in picture soli)
  Hand h = hi.hand();

  Card c;
  Card bestcard;

  // there is a trump to draw
  if( hi.cards_information().remaining_trumps_others() == 0 )
    return c;

  // first find highest trump on hand
  unsigned i;
  unsigned best = 0;
  bestcard = h.card( best );
  for( i = 1; i < h.cardsnumber();i++ )
  {
    if(   h.card(i).istrump()
       && bestcard.less( h.card( i ) )
      )
    {
      bestcard = h.card( i );
      best = i;
    }
  } // for ( i < h.cardsnumber() )

  // find second best card
  for (i = 0; i < h.cardsnumber(); i++)
  {
    if(   h.card(i).istrump()
       && (  h.card(i).less(bestcard)
           || h.card(i) == bestcard
          )
       && c.less( h.card(i) )
       && i != best
      )
    {
      c = h.card( i );
    }
  } // for ( i < h.cardsnumber() )

  DEBUG_ASSERT(!c.is_empty());
  return c;

} // Card Heuristics::SecondBestTrump(HeuristicInterface const& hi)


/**
 ** puts a nice card to a trick of a partner
 ** returns a king, a ten (no dolle), an ace (no swine) or a nine
 ** the order depends on the card the partner has played
 **
 ** @param        t         current trick
 ** @param        hi        heuristic interface
 **
 ** @return       card to play, 'Card()' if the heuristic does not match
 **
 ** @author    Borg Enders
 **
 ** @version      Beta
 **/
Card
Heuristics::play_for_team( Trick const& t, HeuristicInterface const& hi )
{
  // @heuristic::name   play for team
  // @heuristic::idea   put a good card in a trick of the team mate

  // I am not the start player
  if( t.isstartcard() )
    return Card::EMPTY;

  bool const soloPlayer = soloPlayerBehind( t, hi );

  if (   soloPlayer
      && (t.game().type() != GAMETYPE::POVERTY) )
    return Card::EMPTY;

  // @heuristic::condition   partner has jabbed the trick with a high card
  if (maybe_to_team(hi.teamofplayer(t.winnerplayer())) != hi.team())
    return Card::EMPTY;

  if (t.winnercard().less(hi.value(Aiconfig::LIMITTHROWING)))
    return Card::EMPTY;

  Card c;
  for( unsigned i = 0; i < hi.hand().cardsnumber(); i++ ) {
    Trick tr = t;
    if( tr.isvalid( hi.hand().card(i), hi.hand() ) ) {
      tr += hi.hand().card( i );
      if (   !hi.same_player(tr.winnerplayer())
          && hi.hand().card(i).value()==Card::KING
          && !hi.hand().card(i).istrump())
        c = hi.hand().card( i );
    } // if ( tr.isvalid( ... ) )
  } // for ( i < hi.hand().cardsnumber() )

  if( !c.is_empty() )
    return c;


  for(unsigned i = 0; i < hi.hand().cardsnumber(); i++ ) {
    Trick tr = t;
    if( tr.isvalid( hi.hand().card( i ), hi.hand() ) )
    {
      tr += hi.hand().card( i );
      // a ten only, if the player has got enough points on his hand
      if(   !hi.same_player(tr.winnerplayer())
         && hi.hand().card(i).value() == Card::TEN
         && !tr.winnercard().less(hi.value(Aiconfig::LIMITHIGH))
         && (hi.hand().numberof(Card::TEN) + hi.hand().numberof(Card::ACE)
             - hi.hand().numberofdolle() - hi.hand().numberofswines()
             >= 3)
         && !hi.hand().card(i).is_special()
        )
        c=hi.hand().card(i);
    } // if ( tr.isvalid ( ... ) )
  } // for ( i < hi.hand().cardsnumber() )

  if( !c.is_empty() )
    return c;

  for(unsigned i = 0; i < hi.hand().cardsnumber(); i++ ) {
    Trick tr = t;
    if( tr.isvalid( hi.hand().card(i),hi.hand() ) )
    {
      tr += hi.hand().card( i );

      // last player after me is player with poverty
      bool lastPoverty = (soloPlayer
                          && tr.actcardno() == 3
                          && hi.game().soloplayer() == tr.player_of_card(3) );

      if(   !hi.same_player(tr.winnerplayer())
         && hi.hand().card(i).value() == Card::ACE
         && (   !tr.winnercard().less( hi.value( Aiconfig::LIMITHIGH ) )
             || t.islastcard()
             || lastPoverty
            )
         && !hi.hand().card( i ).isswine()
         && (hi.hand().numberof(Card::TEN) + hi.hand().numberof(Card::ACE)
             >= 3)
         && (   !hi.hand().card( i ).isfox()
             || t.islastcard()
             || lastPoverty
            )
        )
        c = hi.hand().card( i );
    } // if ( tr.isvalid ( ... ) )
  } // for ( i < hi.hand().cardsnumber() )

  if( !c.is_empty())
    return c;


#ifdef OUTDATED
  // DK 2011-10-15
  // ToDo: Wieso ist eine Neun ein Spiel 'für' das eigene Team? Da gibt doch ein Bube mehr Punkte
  for(unsigned i = 0; i < hi.hand().cardsnumber(); i++ ) {
    Trick tr = t;
    if( tr.isvalid( hi.hand().card(i), hi.hand() ) )
    {
      tr += hi.hand().card( i );
      if(   !hi.same_player(tr.winnerplayer())
         && hi.hand().card(i).value() == Card::NINE
         && !hi.hand().card(i).ishyperswine()
        )
        c = hi.hand().card( i );
    } // if ( tr.isvalid( ... ) )
  } // for ( i < hi.hand().cardsnumber() )
#endif

  // @heuristic::action   play a small card with some value
  return c;
} // Card Heuristics::play_for_team( Trick t, HeuristicInterface hi )


/**
 ** jabs a fox if in this in the trick
 **
 ** @param        t         current trick
 ** @param        hi        heuristic interface
 **
 ** @return       card to play, 'Card()' if the heuristic does not match
 **
 ** @author    Borg Enders
 ** @author    Diether Knof
 **
 ** @version      0.7.9
 **/
Card
Heuristics::jab_fox( Trick const& t, HeuristicInterface const& hi )
{
  // @heuristic::name   jab fox
  // @heuristic::idea   jab a fox if played in this trick

  if (!t.contains_fox())
    return Card::EMPTY;

  // @heuristic::condition   it must not be an undetermined marriage
  if (   hi.game().is_undetermined_marriage()
      && is_selector(t.startcard().color(), hi.game().marriage_selector()))
    return Card::EMPTY;

  // player can play a trump
  if (   !t.startcard().istrump()
      && hi.hand().contains(t.startcard().tcolor()))
    return Card::EMPTY;

  Card c;

  // lastcard and this is our trick nothing to do.
  if ( (    t.islastcard()
        || !oppositeTeamCanWinTrick( t, hi ) )
      && hi.same_team(t.winnerplayer())) {
    return Card::EMPTY;
  }

  // if lastcard just take the best winning card
  if (   t.islastcard()
      && !hi.same_team(t.winnerplayer())) {
    return best_jabbing_card(t, hi);
#ifdef OUTDATED
    // DK, 0.7.12 (2012-05-05)
    return best_winning_card_old(t, hi, 10 ); ///@todo 10 replace with AiConfig
#endif
  }


  { // If I can jab the fox surely, jab it.
    // take the best cards of all players behind me that are not in my team
    HandCard const& highest_opposite_card
      = highest_card_behind_of_opposite_team(t, hi);
    if (!hi.hand().highest_trump().less(highest_opposite_card)) {
      Card const c = optimized_card_to_play(lowest_highest_trump(t, hi), t, hi);
      if (!c.is_empty())
        return c;
    }
  } // If I can jab the fox surely, jab it.
#ifdef OUTDATED
  // DK, 5. Mai 2012 
  { // if I can jab the fox surely, take the card
    // take the best cards of all players behind me that are not in my team
    Trick const& trick = t;
    HandCard highest_card_behind(hi.hand());
    for (Player const* player = &(trick.game().player_following(trick.actplayer()));
         player != &trick.startplayer();
         player = &(trick.game().player_following(*player))) {
      if( hi.teamofplayer(*player) != hi.team() )
      {
        HandCard const highest_card = player->hand().highest_card();
        if(  highest_card_behind.is_empty()
           || highest_card_behind.less( highest_card )
          )
          highest_card_behind = highest_card;
      } // if (hi.teamofplayer(*player) != hi.team())
    } // for (player)

    if (!highest_card_behind.is_empty()) {
      // the partner has the best possible card
      if(   hi.teamofplayer(trick.winnerplayer()) == hi.team()
         && !trick.winnercard().less( highest_card_behind )
         /// @todo next_higher_card_of_other_team
        )
        return Card::EMPTY;

      // look whether I have the best card
      Card lowest_better_card;
      HandCards const valid_cards = hi.hand().validcards( t );
      for( HandCards::const_iterator c = valid_cards.begin();
          c != valid_cards.end();
          ++c )
      {
        Trick tempTrick = trick;
        tempTrick += *c;
        if (   trick.isjabbed(*c)
            && !tempTrick.isjabbed(highest_card_behind)
            && (   lowest_better_card.is_empty()
                || lowest_better_card.less(*c)
               )
           )
          lowest_better_card = *c;
      } // for (c)

      if (!lowest_better_card.is_empty())
        return lowest_better_card;
    } // if (highest_card_behind)
  } // if I can jab the fox surely, take the card
#endif

  if (   (hi.game().trick_current_no() == 0)
      && t.startcard().istrump()
      && !t.islastcard())
  {
    auto const card = optimized_card_to_play(highest_trump_card(t, hi.hand()), t, hi);
    if (t.isjabbed(card))
      return card;
  }
  {
    // for all other we take our highest queen
    HandCard const card(hi.hand(), highest_queen(hi.hand(), hi));
    if (!card)
      return Card::EMPTY;

    if (   t.isvalid(card)
        && t.isjabbed(card))
      return card;
  }

  return Card::EMPTY;
} // Card Heuristics::jab_fox( Trick t, HeuristicInterface hi )


/**
 ** -> result
 **
 ** @param        trick        trick
 ** @param        hi        heuristic interface
 **
 ** @return       card with ten points, if the heuristic matches
 **
 ** @author       Borg Enders
 ** @author       Diether Knof
 **
 ** @version      0.6.9
 **/
Card
Heuristics::try_for_doppelkopf( Trick const& trick,
                               HeuristicInterface const& hi )
{
  // @heuristic::name   try for doppelkopf
  // @heuristic::idea   try to get a doppelkopf for the own team

  // a doppelkopf gives a specialpoint (t.i. no solo game)
  if (GAMETYPE::is_solo(hi.game().type()))
    return Card::EMPTY;

  // at max one player behind me
  if( trick.actcardno() < hi.game().playerno() - 2 )
    return Card::EMPTY;

  // each player has to give ten points
  if( trick.points() < 10 * trick.actcardno() )
    return Card::EMPTY;


  // last player opposite team ?
  if (   !trick.islastcard()
      && hi.teamofplayer( trick.lastplayer() ) != hi.team() )
    return Card::EMPTY;

  Player const& player = hi.player();

  if (   !trick.islastcard()
      && player.announcement() == ANNOUNCEMENT::NOANNOUNCEMENT
      && !hi.game().announcement_valid( ANNOUNCEMENT::NO120, player )
     )
    return Card::EMPTY;

  // search a ten or an ace
  vector<Card> try_cards;
  try_cards.push_back( Card( hi.game().trumpcolor(), Card::TEN ) );
  try_cards.push_back( Card( hi.game().trumpcolor(), Card::ACE ) );
  try_cards.push_back( Card::DOLLE );

  for(vector<Card>::const_iterator c = try_cards.begin();
      c != try_cards.end();
      ++c ) {
    if(   hi.hand().numberof(*c) > 0
       && trick.isvalid( *c, hi.hand() )
       && trick.winnercard().less( *c )
      ) {
      // always select a non special card
      if (!HandCard(hi.hand(), *c).is_special())
        return *c;
      // select a special card if it is needed for winning the trick
      if (   !hi.guessed_same_team(trick.winnerplayer())
          && !trick.winnercard().less(hi.trump_card_limit()))
        return *c;
      // select a special card if it is not needed for an announcement
      if (secure_tricks_of_team(hi) == hi.game().tricks_remaining_no())
        return *c;

      // select it if a better announcement can be made
      Announcement const announcement = hi.game().announcement_of_team(hi.team());
      if (  (announcement == ANNOUNCEMENT::NO60)
          ? say_no30(hi)
          : (announcement == ANNOUNCEMENT::NO90)
          ? say_no60(hi)
          : (announcement == ANNOUNCEMENT::NO120)
          ? say_no90(hi)
          : ANNOUNCEMENT::is_reply(announcement)
          ? say_no90(hi)
          : (announcement == ANNOUNCEMENT::NOANNOUNCEMENT)
          ? make_announcement(hi)
          : false)
        return *c;
    } // if (c wins the trick)
  } // for (c)

  return Card::EMPTY;
} // Card Heuristics::try_for_doppelkopf(Trick trick,HeuristicInterface hi)


/**
 ** -> result
 **
 ** @param     trick       current trick
 ** @param     hi          heuristic interface
 ** @param     forceFehl   whether to force a fehl card
 **                        default: false
 **
 ** @return    low fehl card with many cards of the color remaining
 **
 ** @author    Borg Enders
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
Card
choose_best_fehl(Trick const& trick,
                 HeuristicInterface const& hi, bool const forceFehl)
{
  // the best card
  Card best_card;
  // the best number of remaining cards
  // There should be at lest so as many cards remaining
  // as there are other players
  unsigned best_remaining = 0;
  if (!forceFehl)
    best_remaining = (hi.game().playerno() - 1);

  // all valid cards
  HandCards const cards = hi.hand().validcards(trick);

  for (HandCards::const_iterator card = cards.begin();
       card != cards.end();
       ++card) {
    unsigned const remaining
      = hi.cards_information().remaining_others(card->color());
    if(   !card->istrump()
       && (remaining > best_remaining)
       && (hi.hand().numberof(card->color()) <= 1)
       && (   card->value() == Card::KING
           || card->value() == Card::NINE )
      ) {
      best_card = *card;
      best_remaining = remaining;
    } // if (found better card)
  } // for (card \in cards)

  return best_card;
} // Card choose_best_fehl( Trick trick, HeuristicInterface hi, bool forceFehl = false )


/**
 ** play single color nine or king to get fehl in second run of a color
 **
 ** @param     trick   current trick
 ** @param     hi      heuristic interface
 **
 ** @return    card to play, 'Card()' if the heuristic does not match
 **
 ** @author    Borg Enders
 **
 ** @version   0.7.6
 **/
Card
Heuristics::create_fehl(Trick const& trick, HeuristicInterface const& hi)
{
  // @heuristic::name   create fehl
  // @heuristic::idea   create a missing color (only allowed for first card depending on Aiconfig::FEHLCREATIONONFIRSTCARD)

  if (hi.game().is_undetermined_marriage())
    return Card::EMPTY;

  // @heuristic::condition   this trick is still worth fehl creation
  if (!(static_cast<int>(hi.game().trick_current_no())
        < hi.value(Aiconfig::LASTFEHLCREATION)))
    return Card::EMPTY;

  // @heuristic::condition   this is the firstcard
  if (hi.value(Aiconfig::FEHLCREATIONONFIRSTCARD))
    if (trick.isstartcard())
      return choose_best_fehl(trick, hi);

  // @heuristic::condition   this is not the first colorrun and there are not more then 10 points
  if(   !trick.isstartcard()
     && (hi.color_runs(trick.startcard().color()))
     && (static_cast<int>(trick.points())
         < hi.value(Aiconfig::LIMIT_THROW_FEHL))
    ) {
    // @heuristic::condition   - the trick is jabbed high enough
    if(   trick.winnercard().istrump()
       && !trick.winnercard().less(hi.lowest_trump_card_limit())
      ) {
      return choose_best_fehl(trick, hi);
    }

    // @heuristic::condition   - the winnercard is already a no trump ace
    if(   !trick.winnercard().istrump()
       && (trick.winnercard().value() == Card::ACE))
      return choose_best_fehl(trick, hi);
  }
  // @heuristic::action   play the best fehl card

  return Card::EMPTY;
} // Card Heuristics::create_fehl(Trick trick, HeuristicInterface hi)


/**
 ** value of cards for announcement
 **
 ** @param        hi        heuristic interface
 **
 ** @return       the value of the hand
 **
 ** @author    Diether Knof
 **
 ** @version      0.7.13
 **/
int
Heuristics::CalcHandValue( HeuristicInterface const& hi)
{
  return Heuristics::CalcHandValue(hi, hi.game());
} // int Heuristics::CalcHandValue( HeuristicInterface hi)

/**
 ** value of cards for announcement
 **
 ** @param        hi        heuristic interface
 ** @param        g         current game
 **
 ** @return       the value of the hand
 **
 ** @author    Borg Enders
 **
 ** @version      Beta
 **/
int
Heuristics::CalcHandValue( HeuristicInterface const& hi, const Game& g )
{
  // @heuristic::name   ?  calc hand value  ?
  // @heuristic::idea calculates the value of a hand for decision making of announcements, soli decisions,...
  int value = 0;
  unsigned i;

  unsigned v_queen = 0;
  unsigned v_jack = 0;
  unsigned v_highest = 0;
  unsigned v_king = 0;
  unsigned v_ace = 0;
  unsigned v_fehl = 0;

  if( g.type() == GAMETYPE::NORMAL )
  { v_queen = 2; v_jack = 1; v_highest = 3; v_king = 0; v_ace = 1; v_fehl = 2;

    if (hi.value(Aiconfig::AGGRESSIVE))
      v_highest += 1;
    if (hi.team() == TEAM::RE)
    {
      value = 1;
    } else
    {
      value = 0;
    }
  }

  if( g.type() == GAMETYPE::POVERTY )
  { v_queen = 2; v_jack = 1; v_highest = 3; v_king = 0; v_ace = 0; v_fehl = 2;
    value = -3;
  }

  if( g.type() == GAMETYPE::MARRIAGE )
  { v_queen = 2; v_jack = 1; v_highest = 3; v_king = 0; v_ace = 1; v_fehl = 2; }

  if( g.type() == GAMETYPE::SOLO_JACK )
  { v_queen = 0; v_jack = 3; v_highest = 0; v_king = 0; v_ace = 2; v_fehl = 1;
    if (hi.team() == TEAM::RE)
    {
      value = -6;
    } else
    {
      value = -2;
    }
  }

  if (g.type()==GAMETYPE::SOLO_QUEEN)
  { v_queen = 3; v_jack = 0; v_highest = 0; v_king = 0; v_ace = 2; v_fehl = 1;
    if (hi.team() == TEAM::RE)
    {
      value = -6;
    } else
    {
      value = -2;
    }
  }

  if( g.type() == GAMETYPE::SOLO_KING )
  { v_queen = 0; v_jack = 0; v_highest = 0; v_king = 3; v_ace = 2; v_fehl = 1;
    if (hi.team() == TEAM::RE)
    {
      value = -6;
    } else
    {
      value = -2;
    }
  }

  if( g.type() == GAMETYPE::SOLO_QUEEN_JACK )
  { v_queen = 3; v_jack = 2; v_highest = 0; v_king = 0; v_ace = 1; v_fehl = 1;
    if (hi.team() == TEAM::RE)
    {
      value = -6;
    } else
    {
      value = -3;
    }
  }

  if( g.type() == GAMETYPE::SOLO_KING_JACK )
  { v_queen = 0; v_jack = 2; v_highest = 0; v_king = 3; v_ace = 1; v_fehl = 1;
    if (hi.team() == TEAM::RE)
    {
      value = -6;
    } else
    {
      value = -3;
    }
  }

  if( g.type() == GAMETYPE::SOLO_KING_QUEEN )
  { v_queen = 2; v_jack = 0; v_highest = 0; v_king = 3; v_ace = 1; v_fehl = 1;
    if (hi.team() == TEAM::RE)
    {
      value = -6;
    } else
    {
      value = -3;
    }
  }

  if( g.type() == GAMETYPE::SOLO_KOEHLER )
  { v_queen = 2; v_jack = 1; v_highest = 0; v_king = 3; v_ace = 0; v_fehl = 0;
    if (hi.team() == TEAM::RE)
    {
      value = -5;
    } else
    {
      value = -2;
    }
  }

  if(    g.type()==GAMETYPE::SOLO_CLUB
     || g.type()==GAMETYPE::SOLO_HEART
     || g.type()==GAMETYPE::SOLO_SPADE
     || g.type()==GAMETYPE::SOLO_DIAMOND )
  { v_queen = 2; v_jack = 1; v_highest = 3; v_king = 0; v_ace = 1; v_fehl = 2;
    if (hi.team() == TEAM::RE)
    {
      value = -7;
    } else
    {
      value = 0;
    }
  }


  if( g.type() == GAMETYPE::SOLO_MEATLESS ) {
    v_queen = 0; v_jack = 0; v_highest = 0; v_king = 0; v_ace = 4; v_fehl = 1; // missing aces
    value = -2;
  }

  if ( !hi.game().rule()(Rule::WITH_NINES) )
  {
    v_ace= max(0,(int)v_ace-1);
    value += 1; // compensate for reduced ace value
  }

#ifdef ANNOUNCE_DEBUG_DETAIL
  COUT << "CalcHandValue(1)\t" << hi.player().name() << "\t" << value << endl;
#endif


  Player const& player = hi.player();

  for( i = 0; i < hi.hand().cardsnumber(); i++ )
  {

    if(   hi.hand().card(i).isdolle()
       || hi.hand().card(i).isswine()
       || hi.hand().card(i).possible_swine()
       || hi.hand().card(i).ishyperswine()
       || hi.hand().card(i).possible_hyperswine()
      )
      value += v_highest;
    else if( hi.hand().card(i).value()==Card::QUEEN
            || hi.hand().card(i).possible_genscher()  )
    {
      value += v_queen;
      if( g.type()==GAMETYPE::SOLO_QUEEN &&
         hi.hand().card(i).color() == Card::CLUB )
        value +=1;
    }
    else if( hi.hand().card(i).value()==Card::JACK )
    {
      value += v_jack;
      if( g.type()==GAMETYPE::SOLO_JACK &&
         hi.hand().card(i).color() == Card::CLUB )
        value +=1;
    }
    else if( hi.hand().card(i).value()==Card::KING )
    {
      value += v_king;
      if( g.type()==GAMETYPE::SOLO_KING &&
         hi.hand().card(i).color() == Card::CLUB )
        value +=1;
    }
    else if(    hi.hand().card(i).value()==Card::ACE
            && !hi.hand().card(i).istrump()
            && !hi.jabbedbyotherteam(hi.hand().card(i).color()))
    {
      value += v_ace;
      if (::in_running_game()
          && (hi.game().trick_current().cardno_of_player( player ) == 0 ) )
      {
        if (hi.hand().numberof( hi.hand().card(i).color() ) == 1)
        {
          value += v_ace;
          if( hi.color_runs( hi.hand().card(i).color() ) == 0 )
          {
            value += v_ace;
          }

        } else
        {
          value += 1;
          if( hi.color_runs( hi.hand().card(i).color() ) == 0 )
          {
            value += v_ace;
          }
        }

      }
      if ( hi.hand().card(i).color() == Card::HEART
          && hi.hand().numberof( hi.hand().card(i).color() ) > 1
          && hi.game().rule()(Rule::DOLLEN))
        value -=1;
    }

  }
#ifdef ANNOUNCE_DEBUG_DETAIL
  COUT << "CalcHandValue(2)\t" << hi.player().name() << "\t" << value << endl;
#endif

  for (vector<Card::Color>::const_iterator
       c = hi.game().rule().card_colors().begin();
       c != hi.game().rule().card_colors().end();
       ++c)
  {
    Card::Color const color = *c;
    if (   !Card( color, Card::ACE ).istrump( hi.game() )
        && !hi.hand().contains( color )
        && !hi.jabbedbyotherteam(color)
       )
      if( ::in_running_game() )
      {
        if(     hi.game().trick_current().actcardno()>0
           && hi.color_runs( color ) == 0
           && (hi.game().trick_current().startcard().tcolor()
               != color) )
          value += v_fehl;
      } else {
        value += v_fehl;
      }
  }

  if( g.type() == GAMETYPE::SOLO_MEATLESS ) {
    for (vector<Card::Color>::const_iterator
         c = hi.game().rule().card_colors().begin();
         c != hi.game().rule().card_colors().end();
         ++c)
    {
      if( hi.hand().contains( *c ) )
        value -=  (v_fehl
                   * ( 2 - hi.hand().numberof( *c, Card::ACE ) ));
    }
    if( hi.is_soloplayer())
    {
      unsigned longColor = 0;
      for (vector<Card::Color>::const_iterator
           c = hi.game().rule().card_colors().begin();
           c != hi.game().rule().card_colors().end();
           ++c)
      {
        if(   hi.hand().numberof(*c,Card::ACE)  == 2
           && hi.hand().numberof(*c ) > longColor )
          longColor = hi.hand().numberof(*c );
      }
      value += longColor;
    }
  } else  {

    for( vector<Card::Color>::const_iterator
        c = hi.game().rule().card_colors().begin();
        c != hi.game().rule().card_colors().end();
        ++c )
    {
      Card::Color color = *c;

      if( color == hi.game().trumpcolor() )
        continue;

      // reduction for long colors
      if (         hi.hand().numberof( color ) > 2
          && !hi.jabbedbyownteam(color) )
        value -= hi.hand().numberof( color ) - (2 + hi.hand().numberof( color, Card::ACE ));

      // including this trick the player does not have the color
      if (    !hi.hand().contains( color )
          && !(    ::in_running_game()
               && ( hi.game().trick_current().actcardno()
                   > hi.game().trick_current().cardno_of_player( player ) )
               && ( hi.game().trick_current().card_of_player( player ).tcolor()
                   == color )
              )
         )
        continue;

      if (  ( hi.hand().numberof( color, Card::ACE ) == 0 )
          && !hi.jabbedbyownteam(color) )
      {
        // previous        value -= v_ace;
        value -= v_fehl;
        //unprotected tens on hand
        if (    hi.hand().numberof( color, Card::TEN ) > 0
            &&  hi.hand().numberof( color, Card::TEN ) == hi.hand().numberof( color ) )
          value -= v_fehl;
      }

      if( (   hi.hand().numberof( color )
           + hi.color_runs( color )
           + (   ::in_running_game()
              && hi.game().trick_current().actcardno() > 0
              && ( hi.game().trick_current().startcard().color()
                  == color
                 ) ? 1 : 0 )
           > ( hi.game().different_cards_no( color ) - 1
              - ( hi.game().rule()(Rule::WITH_NINES) ? 0 : 1 ) )
          ) && !hi.jabbedbyownteam(color) )
        value -= v_fehl;

      if ( (   hi.hand().numberof( color )
            + hi.color_runs( color )
            + ( (   ::in_running_game()
                 && hi.game().trick_current().actcardno()>0
                 && ( hi.game().trick_current().startcard().color()
                     == color)
                ) ? 1 : 0)
            >= 3)
          && ( hi.hand().numberof( Card( color, Card::ACE ) ) < 2 )
          && !hi.jabbedbyownteam(color) )
        value -= v_ace;

    } // (for color)
  } // if (...)
#ifdef ANNOUNCE_DEBUG_DETAIL
  COUT << "CalcHandValue(3)\t" << hi.player().name() << "\t" << value << endl;
#endif

  if( g.type() == GAMETYPE::SOLO_KOEHLER ) {
    for( vector<Card::Color>::const_iterator
        c = hi.game().rule().card_colors().begin();
        c != hi.game().rule().card_colors().end();
        ++c )
    {
      if( hi.hand().contains(*c)
         && !hi.jabbedbyownteam(*c) )
        value-=5;
    }
  } // if( g.type() == GAMETYPE::SOLO_KOEHLER )


  if(   (   hi.game().swines_owner()
         && *hi.game().swines_owner() != hi.player() )
     || hi.game().swines_announcement_valid( hi.player() )
    )
  {
    Player const& swines_player = ( hi.game().swines_owner()
                                   ? *hi.game().swines_owner()
                                   : hi.player() );
    if(   hi.teamofplayer(swines_player) == hi.team()
       || hi.game().swines_announcement_valid( swines_player )
      )
    {
      if(swines_player != hi.player() )
      {
        value += v_highest;
      } else
      {
        value += 1;
      }
    } else
    {
      value -= 2* v_highest;
    }
  }

#ifdef ANNOUNCE_DEBUG_DETAIL
  COUT << "CalcHandValue(4)\t" << hi.player().name() << "\t" << value << endl;
#endif

  if(  (   hi.game().hyperswines_owner()
        && *hi.game().hyperswines_owner() != hi.player()
       )
     || hi.game().hyperswines_announcement_valid( hi.player() )
    )
  {
    Player const& hyperswines_player = ( hi.game().hyperswines_owner()
                                        ? *hi.game().hyperswines_owner()
                                        : hi.player() );
    if(   hi.teamofplayer(hyperswines_player) == hi.team()
       || hi.game().hyperswines_announcement_valid( hyperswines_player )
      )
    {
      if(hyperswines_player != hi.player() )
      {
        value += v_highest;
      } else
      {
        value += 1;
      }
    } else
    {
      value -= 2* v_highest;
    }
  }

#ifdef ANNOUNCE_DEBUG_DETAIL
  COUT << "CalcHandValue(5)\t" << hi.player().name() << "\t" << value << endl;
#endif


  if( !GAMETYPE::is_solo( hi.game().type() )
     || hi.is_soloplayer())
  {
    if (hi.hand().numberoftrumps()
        >= ( (hi.cards_information().remaining_trumps_others() / 3)
            + round(  (::in_running_game()
                       ? hi.game().tricks_remaining_no()
                       : hi.game().trickno()
                      )/ 3)
           ) )
      value += 2;
  }

  if (::in_running_game()
      && (hi.game().trick_current().cardno_of_player( player ) == 0 ) )
  { // count aces like trumps, if I serve
    unsigned cardno = hi.hand().numberoftrumps();

    for( vector<Card::Color>::const_iterator
        c = hi.game().rule().card_colors().begin();
        c != hi.game().rule().card_colors().end();
        ++c )
    {
      Card::Color color = *c;

      if( color == hi.game().trumpcolor() )
        continue;

      if(  ((hi.color_runs( color ) == 0)
            || hi.jabbedbyownteam(color) )
         && (hi.hand().numberof( color ) <= 1 + hi.hand().numberof( Card( color, Card::ACE ) ) )
        )
      {
        cardno += hi.hand().numberof( Card( color, Card::ACE ) );

      }

    }

    if (cardno >= 9 )
      value += 1;
    if (cardno >= hi.game().tricks_remaining_no() - 1 )
      value += 1;
    if (cardno >= hi.game().tricks_remaining_no() )
      value += 2;
    if (cardno == hi.hand().cardsnumber() )
      value += 2;


    if ( g.is_solo()
        && cardno >= 9 )
      value += 2; // reference 096567

  } else
  {
    if (hi.hand().numberoftrumps() >= 9 )
      value += 1;
    if (hi.hand().numberoftrumps() >= hi.game().tricks_remaining_no() - 1 )
      value += 2;
    if (hi.hand().numberoftrumps() >= hi.game().tricks_remaining_no() )
      value += 2;
    if (hi.hand().numberoftrumps() == hi.hand().cardsnumber() )
      value += 2;

    if ( g.is_solo()
        && hi.hand().numberoftrumps() >= 9 )
      value += 1;
  }



  if (hi.hand().numberoftrumps() -1 < (hi.hand().cardsnumber()+1) / 2 )
    value -= 1;
  if (hi.hand().numberoftrumps() -1 < (hi.hand().cardsnumber()+1) / 3 )
    value -= 1;
  if (hi.hand().numberoftrumps() -1 < (hi.hand().cardsnumber()+1) / 4 )
    value -= 1;

  if ( g.type() == GAMETYPE::NORMAL ) // remove a value if a lot of small trumps are on hand
  {
    value -= (  hi.hand().numberoftrumps() //2do
              - hi.hand().numberof( Card::QUEEN )
              - hi.hand().numberof( Card::JACK )
              - hi.hand().numberofswines()
              - hi.hand().numberofhyperswines()
              - hi.hand().numberofdolle()) / 4;
  }

  if ( g.type() == GAMETYPE::NORMAL ) // remove a value for two heart aces
  {
    if(    hi.game().rule()(Rule::DOLLEN)
       && hi.game().trumpcolor() != Card::HEART
       && hi.hand().numberof(Card::HEART,Card::ACE) == 2)
      value -=1;
  }

  if (    (::game_status > GAMESTATUS::GAME_START)
      && (hi.game().type() == GAMETYPE::POVERTY)
      && (hi.game().poverty_partner() == hi.player()) )
    value -= 2;

  if ( (   g.type() == GAMETYPE::NORMAL
        || g.type() == GAMETYPE::POVERTY )
      && hi.team() == TEAM::RE )
  {
    if( hi.hand().numberof( Card( Card::SPADE, Card::QUEEN ) ) == 2 )
      value += 2;
    if( hi.hand().numberof( Card( Card::HEART, Card::QUEEN ) ) == 2 )
      value += 1;
  }

  if (   ( hi.hand().numberof( Card::QUEEN ) < 3 ) // check for some queens on hand
      ||(    hi.team() == TEAM::CONTRA
         && hi.hand().numberof( Card::QUEEN ) < 4 )
     )
  {
    value -= v_queen;
  }


  if( ::in_running_game() )
  {
    if( (   hi.game().trick_current().isfull()  // lost trick
         && TEAM::maybe_to_team(hi.teamofplayer(hi.game().trick_current().winnerplayer())) != hi.team()
        ) ||
       (    !hi.game().trick_current().isempty() // lost color trick
        && !hi.game().trick_current().startcard().istrump()
        && !hi.is_winnerplayer(hi.game().trick_current())
       )
      )
      value -= 1;
    if(   hi.game().trick_current().isempty()
       &&  hi.same_player(hi.game().trick_current().startplayer()) )
      value += 1;

    //first tricks, other player serves
    if ( hi.game().trick_current().no() < 2
        &&  !hi.same_player(hi.game().trick_current().startplayer()) )
      value -= 1;
  }


#ifdef ANNOUNCE_DEBUG_DETAIL
  COUT << "CalcHandValue(6)\t" << hi.player().name() << "\t" << value << endl;
#endif

  return value;
} // Heuristics::CalcHandValue( HeuristicInterface hi, Game g )


/**
 ** If the poverty partner is the last player play many points
 **
 ** @param        trick        current trick
 ** @param        hi        heuristic interface
 **
 ** @return       card to play, 'Card()' if the heuristic does not match
 **
 ** @author       Borg Enders
 ** @author       Diether Knof
 **
 ** @version      0.7.2
 **
 ** @todo       if the opposite team has played a high card (p.e. dolle)
 **             do _not_ pfund although the own teammate is last player.
 ** @todo       if an opposite player is behind, check, that the winnercard
 **             of the teammate is high enough
 **                (-> play_pfund?)
 **/
Card
Heuristics::poverty_special_play_pfund(Trick const& trick,
                                       HeuristicInterface const& hi)
{
  // @heuristic::name        poverty: special: play pfund
  // @heuristic::idea        If the trick goes or will go to the own team, play a pfund. This heuristic should only be used by the poverty player.

  if (trick.isstartcard())
    return Card::EMPTY;

  // @heuristic::condition   poverty player
  if (hi.game().type() != GAMETYPE::POVERTY)
    return Card::EMPTY;
  if (!hi.is_soloplayer())
    return Card::EMPTY;

  // @heuristic::condition   the last player must be of the own team (oneself or the partner)
  if (hi.teamofplayer(trick.lastplayer()) != hi.team() )
    return Card::EMPTY;

  // the teammate
  vector<Player*>::const_iterator teammate;
  for (teammate = hi.game().players_begin();
       teammate != hi.game().players_end();
       ++teammate)
    if (   (**teammate != hi.player())
        && (hi.teamofplayer(**teammate) == TEAM::RE) )
      break;

  // @heuristic::condition   the own team has already the trick or the partner is behind
  if (   (TEAM::maybe_to_team(hi.teamofplayer(trick.winnerplayer())) != hi.team())
      && (trick.cardno_of_player(**teammate) < trick.actcardno()) )
    return Card::EMPTY;

  // @heuristic::condition   if the partner is behind, he can still jab the trick
  if (!trick.winnercard().less(hi.handofplayer(**teammate).highest_trump()))
    return Card::EMPTY;

  // @heuristic::action      play a pfund
  // ToDo: Borg: use 'play_pfund'
  Card c;
  HandCards const ha = hi.hand().validcards( trick );

  // find any card that's allowed for this trick
  for( unsigned i = 0; i < ha.cardsnumber(); i++ )
    if( !ha.card(i).istrump() ) {
      c = ha.card(i);
      break;
    }

  if (c.is_empty())
    return Card::EMPTY;

  // find a better card
  for( unsigned i = 0; i < ha.cardsnumber(); i++ ) {
    if(   !ha.card(i).istrump()
       && (ha.card(i).value() != Card::ACE)
       && (ha.card(i).value() > c.value())
       && (   (ha.numberof( ha.card(i).color() ) >  ha.numberof(c.color()))
           || (ha.numberof( ha.card(i).color(), Card::ACE ) == 0)
          )
      )
      c = ha.card(i);
  } // for ( i < ha.cardsnumber() )

  return c;
} // Card Heuristics::poverty_special_play_pfund(Trick trick, HeuristicInterface hi)


/**
 ** If the poverty partner has played high and still many points on hand play many points.
 **
 ** @param        trick        current trick
 ** @param        hi        heuristic interface
 **
 ** @return       card to play, 'Card()' if the heuristic does not match
 **
 ** @author       Borg Enders
 **
 ** @version      0.7.6
 **/
Card
Heuristics::poverty_special_offer_pfund(Trick const& trick,
                                        HeuristicInterface const& hi)
{
  // @heuristic::name        poverty: special: offer pfund
  // @heuristic::idea        If the poverty partner has played high and still many points on hand play many points.

  if (trick.isstartcard())
    return Card::EMPTY;

  // @heuristic::condition   poverty player
  if (hi.game().type() != GAMETYPE::POVERTY)
    return Card::EMPTY;
  if (!hi.is_soloplayer())
    return Card::EMPTY;

  // @heuristic::condition   the partner has jabbed with a high card
  if (   (hi.teamofplayer(trick.winnerplayer()) != hi.team())
      || trick.winnercard().less(hi.value(Aiconfig::LIMITHIGH) )  )
    return Card::EMPTY;

  // @heuristic::condition   the player has many high cards
  if (hi.hand().numberof(Card::TEN) + hi.hand().numberof(Card::ACE)
      < hi.hand().cardsnumber() / 2)
    return Card::EMPTY;


  Card c;
  HandCards const ha = hi.hand().validcards( trick );

  // find any card that's allowed for this trick
  for (unsigned i = 0; i < ha.cardsnumber(); i++) {
    if (!ha.card(i).istrump()) {
      c = ha.card(i);
      break;
    } // if (!ha.card(i).istrump())
  } // for ( i < ha.cardsnumber())

  if (c.is_empty())
    return Card::EMPTY;

  // find a better card
  for (unsigned i = 0; i < ha.cardsnumber(); i++) {
    if (   !ha.card(i).istrump()
        && (ha.card(i).value() != Card::ACE)
        && (ha.card(i).value() >= c.value())
        && (   (   ha.numberof( ha.card(i).color() ) >  ha.numberof(c.color()))
            || (ha.numberof( ha.card(i).color(), Card::ACE ) == 0)
           )
       )
      c = ha.card(i);
  }   // for (i < ha.cardsnumber())

  if (c.value() < Card::TEN) {
    for (unsigned i = 0; i < ha.cardsnumber(); i++) {
      if (   !ha.card(i).istrump()
          && (ha.card(i).value() >= c.value())
          && ha.numberof( ha.card(i).color() ) > ha.numberof(c.color())
         )
        c = ha.card(i);
    } // for ( i < ha.cardsnumber() )
  } // if (c.value() < Card::TEN)

  // @heuristic::action      play a ten or ace of the color with the most cards
  return c;
} // Card Heuristics::poverty_special_offer_pfund(Trick trick, HeuristicInterface hi)


/**
 ** 1) poverty player
 ** 2) contra players get trick
 ** give as few points as possible
 **
 ** reason: do not give points to the opposite team
 **
 ** @param        trick        current trick
 ** @param        hi        heuristic interface
 **
 ** @return        card to play, 'Card()' if the heuristic does not match
 **
 ** @author        Diether Knof
 **
 ** @version        0.7.3
 **
 ** @todo      choose a 'good' color (to keep as much freedom as possible)
 ** @todo      'anfüttern' of a high re-card with a king
 ** @todo      not in the first color run, if the re has or can get the trick
 **/
Card
Heuristics::poverty_special_give_no_points(Trick const& trick,
                                           HeuristicInterface const& hi)
{

  // @heuristic::name        poverty: special: give no points
  // @heuristic::idea        If the contra team gets the trick, give as few points as possible.


  // @heuristic::condition   the player must be the poverty player
  if (hi.game().type() != GAMETYPE::POVERTY)
    return Card::EMPTY;
  if (!hi.is_soloplayer())
    return Card::EMPTY;

  // the accepter of the poverty
  Player const& re_player = hi.game().poverty_partner();

  // check whether the contra team gets the trick:
  // @heuristic::condition   the partner has played already
  // @heuristic::condition   contra already has the trick or the re player can be overjabbed
  if (trick.cardno_of_player(re_player) > trick.actcardno())
    return Card::EMPTY;

  if (   trick.islastcard()
      && (trick.winnerplayer() == re_player) )
    return Card::EMPTY;

  if (   trick.startcard().istrump()
      && hi.hand().hastrump())
    return Card::EMPTY;

  /// @todo   Use 'cards information' for checking that the contra team can jab.
  if (   (trick.winnerplayer() == re_player)
      && (!trick.winnercard().less(hi.value(Aiconfig::TRUMPLIMIT_NORMAL))
          || (   !trick.startcard().istrump()
              && (hi.color_runs(trick.startcard().tcolor()) == 0) ) ) ) {
    return Card::EMPTY;
  }

  // @heuristic::action   play the lowest card
  // search the lowest card
  // ToDo: check for a 'good' color
  // ToDo: create and use 'best_loosing_card'
  HandCards const cards = hi.hand().cards_single();
  HandCard card; // the card to play
  for (HandCards::const_iterator c = cards.begin();
       c != cards.end();
       ++c) {
    if (!trick.isvalid(*c))
      continue ;
    if (card.is_empty()) {
      card = *c;
      continue;
    }
    if (c->value() < card.value())
      card = *c;
  } // for (c \in cards)

  return card;
} // Card Heuristics::poverty_special_give_no_points(Trick trick, HeuristicInterface hi)


/**
 ** 1) re player in a poverty
 ** 2) color trick, cannot serve it
 ** play high trump (small queen or so)
 **
 ** reason: jab for the points
 **
 ** note: Should be called after 'serve color trick'
 **       because this is overreacted for the first color run.
 **
 ** @param        trick        current trick
 ** @param        hi        heuristic interface
 **
 ** @return        card to play, 'Card()' if the heuristic does not match
 **
 ** @author        Diether Knof
 **
 ** @version        0.7.3
 **
 ** @todo      for more points take a higher card
 **/
Card
Heuristics::poverty_re_trump_color_trick_high(Trick const& trick,
                                              HeuristicInterface const& hi)
{
  // @heuristic::name   poverty: re: trump color trick high
  // @heuristic::idea   Jab the trick with a small queen in order to get the points

  // @heuristic::condition   re player (not poverty player) in a poverty
  if (hi.game().type() != GAMETYPE::POVERTY)
    return Card::EMPTY;
  if (   (hi.team() != TEAM::RE)
      || hi.is_soloplayer())
    return Card::EMPTY;

  // @heuristic::condition only my team behind me, I don't need to jab high
  if (checkAllMyTeam( trick, hi ) )
    return Card::EMPTY;

  // @heuristic::condition   neither first nor last card in the trick
  if (trick.isstartcard())
    return Card::EMPTY;
  if (trick.islastcard())
    return Card::EMPTY;
  // @heuristic::condition   color trick which must not be served
  if (   trick.startcard().istrump()
      || hi.hand().contains(trick.startcard().tcolor()))
    return Card::EMPTY;

  // @heuristic::condition not first color run
  if (hi.color_runs(trick.startcard().color()) == 0)
    return Card::EMPTY;

  // @heuristic::action   play high trump (small queen)
  // ToDo: Borg: use 'choose_best_card'
  // search in the order one of the following cards to play
  vector<Card> cards_to_play;

  if(   hi.color_runs( trick.startcard().color() ) > 0
     && !trick.startcard().istrump()
     && trick.points() >= 10 )
  {
    cards_to_play.push_back(Card(Card::SPADE,   Card::QUEEN));
    cards_to_play.push_back(Card(Card::HEART,   Card::QUEEN));
    cards_to_play.push_back(Card(Card::DIAMOND, Card::QUEEN));
    cards_to_play.push_back(Card(Card::CLUB,    Card::JACK));
    cards_to_play.push_back(Card(Card::SPADE,   Card::JACK));
    cards_to_play.push_back(Card(Card::HEART,   Card::JACK));
    cards_to_play.push_back(Card(Card::DIAMOND, Card::JACK));
  } else {
    cards_to_play.push_back(Card(Card::HEART,   Card::QUEEN));
    cards_to_play.push_back(Card(Card::DIAMOND, Card::QUEEN));
    cards_to_play.push_back(Card(Card::CLUB,    Card::JACK));
    cards_to_play.push_back(Card(Card::SPADE,   Card::JACK));
    cards_to_play.push_back(Card(Card::SPADE,   Card::QUEEN));
    cards_to_play.push_back(Card(Card::HEART,   Card::JACK));
    cards_to_play.push_back(Card(Card::DIAMOND, Card::JACK));
  }

  for (vector<Card>::const_iterator c = cards_to_play.begin();
       c != cards_to_play.end();
       ++c)
    if (   (hi.hand().numberof(*c) > 0)
        && trick.winnercard().less(*c))
      return *c;

  return Card::EMPTY;
} // Card Heuristics::poverty_re_trump_color_trick_high(Trick trick, HeuristicInterface hi)


/**
 ** 1) re player in a poverty
 ** 2) first player
 ** play a trump (diamond jack, ..., club queen)
 **
 ** reason: draw trump from the contra players
 **
 ** note: this heuristic shall be called after 'draw trump'
 **
 ** @param        trick        current trick
 ** @param        hi        heuristic interface
 **
 ** @return        card to play, 'Card()' if the heuristic does not match
 **
 ** @author        Diether Knof
 **
 ** @version        0.7.3
 **/
Card
Heuristics::poverty_re_play_trump(Trick const& trick,
                                  HeuristicInterface const& hi)
{
  // @heuristic::name   poverty: re: play trump
  // @heuristic::idea   As first player draw trump fro the contra players.

  // @heuristic::condition   re player (not poverty player) in a poverty
  if (hi.game().type() != GAMETYPE::POVERTY)
    return Card::EMPTY;

  if (   (hi.team() != TEAM::RE)
      || hi.is_soloplayer())
    return Card::EMPTY;


  // @heuristic::condition   first player
  if (!trick.isstartcard())
    return Card::EMPTY;


  vector<Card> cards_to_play;

  // @heuristic::condition   partner has no trump
  if (hi.cards_information().of_player(hi.game().soloplayer()).does_not_have(Card::TRUMP))
  {
    // @heuristic::action   play a small jack
    bool jacksAdded = false;
    if(   (hi.game().tricks_remaining_no() > hi.game().trickno() / 2)
       || (hi.hand().numberof( Card::JACK ) > hi.hand().numberof( Card::QUEEN) )
      )
    {
      jacksAdded = true;
      cards_to_play.push_back(Card(Card::CLUB,    Card::JACK));
      cards_to_play.push_back(Card(Card::SPADE,   Card::JACK));
      cards_to_play.push_back(Card(Card::HEART,   Card::JACK));
      cards_to_play.push_back(Card(Card::DIAMOND, Card::JACK));
    }
    cards_to_play.push_back(Card(Card::DIAMOND, Card::QUEEN));
    cards_to_play.push_back(Card(Card::HEART,   Card::QUEEN));
    cards_to_play.push_back(Card(Card::SPADE,   Card::QUEEN));
    cards_to_play.push_back(Card(Card::CLUB,    Card::QUEEN));
    if( ! jacksAdded )
    {
      cards_to_play.push_back(Card(Card::CLUB,    Card::JACK));
      cards_to_play.push_back(Card(Card::SPADE,   Card::JACK));
      cards_to_play.push_back(Card(Card::HEART,   Card::JACK));
      cards_to_play.push_back(Card(Card::DIAMOND, Card::JACK));
    } // if ( ! jacksAdded )
  } else // partner has trump
  {
    // @heuristic::comment   \n\n
    // @heuristic::condition   partner has trump
    // @heuristic::action   play a high trump
    if( hi.hand().numberofdolle() == 2 )
      cards_to_play.push_back(Card::DOLLE);

    cards_to_play.push_back(Card(Card::CLUB,    Card::QUEEN));
    cards_to_play.push_back(Card(Card::SPADE,   Card::QUEEN));
    cards_to_play.push_back(Card(Card::HEART,   Card::QUEEN));
    cards_to_play.push_back(Card(Card::DIAMOND, Card::QUEEN));
    cards_to_play.push_back(Card(Card::CLUB,    Card::JACK));
    cards_to_play.push_back(Card(Card::SPADE,   Card::JACK));
    cards_to_play.push_back(Card(Card::HEART,   Card::JACK));
    cards_to_play.push_back(Card(Card::DIAMOND, Card::JACK));
  } // end of: partner has trump

  for (vector<Card>::const_iterator c = cards_to_play.begin();
       c != cards_to_play.end();
       ++c)
    if (hi.hand().numberof(*c) > 0)
      return *c;


  return Card::EMPTY;
} // Card Heuristics::poverty_re_play_trump(Trick trick, HeuristicInterface hi)


/**
 ** 1) contra player in a poverty
 ** 2) first player
 ** play a color
 **
 ** reason: do not draw trump
 **
 ** @param        trick        current trick
 ** @param        hi        heuristic interface
 **
 ** @return       card to play, 'Card()' if the heuristic does not match
 **
 ** @author       Diether Knof
 **
 ** @version       0.7.3
 **/
Card
Heuristics::poverty_contra_play_color(Trick const& trick,
                                      HeuristicInterface const& hi)
{
  // @heuristic::name   poverty: contra: play color
  // @heuristic::idea   Start with a color.

  // @heuristic::condition   contra player in a poverty
  if (hi.game().type() != GAMETYPE::POVERTY)
    return Card::EMPTY;
  if (hi.team() != TEAM::CONTRA)
    return Card::EMPTY;

  // @heuristic::condition   first player
  if (!trick.isstartcard())
    return Card::EMPTY;


  // the player with the poverty
  Player const& poverty_player = hi.game().soloplayer();
  // the accepter of the poverty
  Player const& re_player = hi.game().poverty_partner();
  // the partner
  // ToDo: this formula does only work with four players
  Player const& partner = hi.game().player( (0 + 1 + 2 + 3)
                                           - hi.game().no(poverty_player)
                                           - hi.game().no(re_player)
                                           - hi.game().no(hi.player()));

  // @heuristic::action   Play a color the partner does not have; if the partner is behind the re player, take a high card, else take a low one. Else take the longest color.
  { // search a color to play
    vector<Card::Color> const& colors = hi.game().rule().card_colors();
    vector<Card::Color>::const_iterator c;
    // first search for a color the partner does not have

    for (c = colors.begin();
         c != colors.end();
         ++c)
      if (   hi.cards_information().of_player(partner).does_not_have(*c)
          && hi.hand().contains(*c))
        break;

    if (c != colors.end()) {
      // There is a color the partner does not have.
      HandCards const cards = hi.hand().cards_single();
      HandCard card; // the card to play
      for (HandCards::const_iterator c = cards.begin();
           c != cards.end();
           ++c) {
        if (c->istrump())
          continue;
        if (!hi.cards_information().of_player(partner).does_not_have(c->tcolor()))
          continue;
        if (card.is_empty()) {
          card = *c;
          continue;
        }
        // If the partner is behind the re player, take the highest card
        // else take the lowest
        if (   (c->value() > card.value())
            == (trick.cardno_of_player(partner)
                > trick.cardno_of_player(re_player)) )
          card = *c;
      } // for (c \in cards)

      return card;

    } else { // if !(c != colors.end())
      // There is no color the partner does not have.
      { // search an ace
        Card::TColor best_color = Card::NOCARDCOLOR;
        for (vector<Card::Color>::const_iterator c = colors.begin();
             c != colors.end();
             ++c)
          if (   hi.hand().contains(*c, Card::ACE)
              && hi.cards_information().remaining_others(*c)) {
            if (best_color == Card::NOCARDCOLOR)
              best_color = *c;
            else if (hi.handofplayer(re_player).contains(*c)
                     && !hi.handofplayer(re_player).contains(best_color))
              best_color = *c;
            else if (   (   hi.handofplayer(re_player).contains(*c)
                         || !hi.handofplayer(re_player).contains(best_color))
                     && (hi.cards_information().remaining_others(*c)
                         < hi.cards_information().remaining_others(best_color)
                        ) )
              best_color = *c;
          } // if (hi.hand().contains(*c, Card::ACE))
        if (best_color != Card::NOCARDCOLOR)
          return Card(best_color, Card::ACE);
      }

      // Play the color I have the most number of.
      Card::TColor color = Card::NOCARDCOLOR;
      unsigned max_num = 0;
      for (vector<Card::Color>::const_iterator c = colors.begin();
           c != colors.end();
           ++c)
        if (hi.hand().numberof(*c) > max_num)
          color = *c, max_num = hi.hand().numberof(*c);

      // play the smallest card of the color
      HandCards const cards = hi.hand().cards_single();
      HandCard card; // the card to play
      for (HandCards::const_iterator c = cards.begin();
           c != cards.end();
           ++c) {
        if (c->tcolor() != color)
          continue;
        if (c->value() < card.value())
          card = *c;
      } // for (c \in cards)

      return card;
    } // if !(c != colors.end())

  } // search a color to play

  return Card::EMPTY;
} // Card Heuristics::poverty_contra_play_color(Trick trick, HeuristicInterface hi)


/**
 ** 1) contra player in a poverty
 ** 2) color trick, cannot serve it
 ** 3) re player is behind
 ** 4) a) partner before:
 **       - partner has played low
 **       - there are some points in the trick
 **    b) partner behind:
 **       - partner can have the color
 ** play high trump (small queen or so)
 **
 ** reason: The re player does in most cases not have the color
 **         so make the trick expensive for him.
 **
 ** @param        trick        current trick
 ** @param        hi        heuristic interface
 **
 ** @return       card to play, 'Card()' if the heuristic does not match
 **
 ** @author       Diether Knof
 **
 ** @version      0.7.3
 **
 ** @todo       use 'best_winning_card()'
 **/
Card
Heuristics::poverty_contra_trump_color_trick_high(Trick const& trick,
                                                  HeuristicInterface const& hi)
{
  // @heuristic::name   poverty: contra: trump color trick high
  // @heuristic::idea   Play a high trump (small queen) in color tricks, so that the trick is expensive for the re player.

  // @heuristic::condition   contra player in a poverty
  if (hi.game().type() != GAMETYPE::POVERTY)
    return Card::EMPTY;
  if (hi.team() != TEAM::CONTRA)
    return Card::EMPTY;

  // @heuristic::condition   a color trck the player must not serve
  if (trick.isstartcard())
    return Card::EMPTY;
  if (   trick.startcard().istrump()
      || hi.hand().contains(trick.startcard().tcolor()))
    return Card::EMPTY;


  // the player with the poverty
  Player const& poverty_player = hi.game().soloplayer();
  // the accepter of the poverty
  Player const& re_player = hi.game().poverty_partner();
  // the partner
  // Bug: this formula does only work with four players
  Player const& partner = hi.game().player( (0 + 1 + 2 + 3)
                                           - hi.game().no(poverty_player)
                                           - hi.game().no(re_player)
                                           - hi.game().no(hi.player()));


  // @heuristic::condition   the re player is behind
  if (trick.cardno_of_player(re_player) < trick.actcardno())
    return Card::EMPTY;

  if (trick.cardno_of_player(partner) < trick.actcardno()) {
    // @heuristic::condition   a) the partner has played low (trump limit)
    if (!(   !trick.card_of_player(partner).istrump()
          || trick.card_of_player(partner).less(hi.value(Aiconfig::TRUMPLIMIT_NORMAL)))
       )
      return Card::EMPTY;

    // @heuristic::condition   a) there are some points (>= 8) in the trick
    if (trick.points() < 8) // *Value*
      return Card::EMPTY;
  } else { // if !(partner before)
    // @heuristic::condition   b) the partner is behind
    // @heuristic::condition   b) the partner can have the color
    if (hi.cards_information().of_player(partner).does_not_have(trick.startcard().tcolor()))
      return Card::EMPTY;
  } // if !(partner before)
  // the partner was before and has played low

  // @heuristic::action   play high trump (small queen)
  // ToDo: Borg: use 'best_winning_card'
  // search in the order one of the following cards to play
  vector<Card> cards_to_play;
  cards_to_play.push_back(Card(Card::HEART,   Card::QUEEN));
  cards_to_play.push_back(Card(Card::DIAMOND, Card::QUEEN));
  cards_to_play.push_back(Card(Card::CLUB,    Card::JACK));
  cards_to_play.push_back(Card(Card::SPADE,   Card::JACK));
  cards_to_play.push_back(Card(Card::SPADE,   Card::QUEEN));
  cards_to_play.push_back(Card(Card::HEART,   Card::JACK));
  cards_to_play.push_back(Card(Card::DIAMOND, Card::JACK));

  for (vector<Card>::const_iterator c = cards_to_play.begin();
       c != cards_to_play.end();
       ++c)
    if (hi.hand().numberof(*c) > 0)
      return *c;

  return Card::EMPTY;
} // Card Heuristics::poverty_contra_trump_color_trick_high(Trick trick, HeuristicInterface hi)


/**
 ** 1) contra player in a poverty
 ** 2) re has played trump
 ** 3) partner is behind and can jab the trick
 ** play a small trump so that the partner can get the trick
 **
 ** reason: make myself be behind the re player
 **
 ** @param        trick        current trick
 ** @param        hi        heuristic interface
 **
 ** @return       card to play, 'Card()' if the heuristic does not match
 **
 ** @author       Diether Knof
 **
 ** @version      0.7.3
 **/
Card
Heuristics::poverty_leave_to_partner(Trick const& trick,
                                     HeuristicInterface const& hi)
{
  // @heuristic::name   poverty: contra: leave to partner
  // @heuristic::idea   Make myself be behind the re player.

  // @heuristic::condition   contra player in a poverty
  if (hi.game().type() != GAMETYPE::POVERTY)
    return Card::EMPTY;
  if (   (hi.team() != TEAM::CONTRA)
      && !hi.is_soloplayer())
    return Card::EMPTY;

  // the player with the poverty
  Player const& poverty_player = hi.game().soloplayer();
  // the accepter of the poverty
  Player const& re_player = hi.game().poverty_partner();
  // the partner
  // Bug: this formula does only work with four players
  Player const& partner = hi.game().player( (0 + 1 + 2 + 3)
                                           - hi.game().no(poverty_player)
                                           - hi.game().no(re_player)
                                           - hi.game().no(hi.player()));


  // @heuristic::condition   the re player has played a trump
  // more precise: it has been started with trump and re has already played
  // (contains the case that the poverty player has started with trump)
  if (trick.isstartcard())
    return Card::EMPTY;
  if (!trick.startcard().istrump())
    return Card::EMPTY;
  if (trick.cardno_of_player(re_player) > trick.actcardno())
    return Card::EMPTY;

  // @heuristic::condition   the partner is behind and can jab the trick
  if (trick.cardno_of_player(partner) < trick.actcardno())
    return Card::EMPTY;
  if (!trick.winnercard().less(hi.handofplayer(partner).highest_trump()))
    return Card::EMPTY;
  // better: check over TRUMPLIMIT_NORMAL, whether I know, my player has a higher card (p.e. club queen, swine)
  if (!trick.winnercard().less(hi.value(Aiconfig::TRUMPLIMIT_NORMAL)))
    return Card::EMPTY;

  // @heuristic::action   play a low trump
  // ToDo: Borg: use 'play_pfund'?
  vector<Card> cards_to_play;

  // special case:
  //   poverty player behind has still trump and
  //   re player has played a small trump
  if (!(   (hi.game().type() == GAMETYPE::POVERTY)
        && (trick.startplayer().team() == TEAM::RE)
        // ToDo: better card value
        && trick.startcard().less(hi.value(Aiconfig::LIMITHIGH))
        && (trick.cardno_of_player(hi.game().soloplayer()) > trick.actcardno())
        && !hi.cards_information().of_player(hi.game().soloplayer()).does_not_have(Card::TRUMP)) ) {
    if (!hi.hand().has_swines())
      cards_to_play.push_back(Card(Card::DIAMOND, Card::ACE));
    cards_to_play.push_back(Card(Card::DIAMOND, Card::TEN));
  } // if (special case poverty)
  cards_to_play.push_back(Card(Card::DIAMOND, Card::KING));
  cards_to_play.push_back(Card(Card::DIAMOND, Card::NINE));
  // ToDo: Hand function 'next greater card'
  cards_to_play.push_back(Card(Card::DIAMOND, Card::JACK));
  cards_to_play.push_back(Card(Card::HEART,   Card::JACK));
  cards_to_play.push_back(Card(Card::SPADE,   Card::JACK));
  cards_to_play.push_back(Card(Card::CLUB,    Card::JACK));
  cards_to_play.push_back(Card(Card::DIAMOND, Card::QUEEN));
  cards_to_play.push_back(Card(Card::HEART,   Card::QUEEN));
  cards_to_play.push_back(Card(Card::SPADE,   Card::QUEEN));
  cards_to_play.push_back(Card(Card::CLUB,    Card::QUEEN));

  for (vector<Card>::const_iterator c = cards_to_play.begin();
       c != cards_to_play.end();
       ++c)
    if (hi.hand().numberof(*c) > 0) {
      if (*c == trick.winnercard()) {
        if (hi.hand().contains(hi.game().next_higher_card(*c)))
          return hi.game().next_higher_card(*c);
        else if (hi.hand().contains(hi.game().next_higher_card(*c, 2)))
          return hi.game().next_higher_card(*c, 2);
      }
      return *c;
    }

  return Card::EMPTY;
} // Card Heuristics::poverty_leave_to_partner(Trick trick, HeuristicInterface hi)


/**
 ** 1) contra player in a poverty
 ** 2) re player has won with a trump
 ** 3) can play a better card than the re player
 ** play next higher card
 **
 ** reason: If you can, overjab the re player in order to get points.
 **
 ** note: Should be called after 'leave to partner'
 **
 ** @param        trick        current trick
 ** @param        hi        heuristic interface
 **
 ** @return       card to play, 'Card()' if the heuristic does not match
 **
 ** @author       Diether Knof
 **
 ** @version      0.7.3
 **/
Card
Heuristics::poverty_overjab_re(Trick const& trick,
                               HeuristicInterface const& hi)
{
  // @heuristic::name   poverty: contra: overjab re
  // @heuristic::idea   Overjab the re player in order to get the points.

  // @heuristic::condition   contra player in a poverty
  if (hi.game().type() != GAMETYPE::POVERTY)
    return Card::EMPTY;
  if (hi.team() != TEAM::CONTRA)
    return Card::EMPTY;

  // @heuristic::condition   the re player has won so far with a trump
  if (trick.winnerplayer() != hi.game().poverty_partner())
    return Card::EMPTY;
  if (!trick.winnercard().istrump())
    return Card::EMPTY;

  // @heuristic::condition   the player can play a better card then the re player
  if (!(   trick.startcard().istrump()
        || !hi.hand().contains(trick.startcard().tcolor()) ) )
    return Card::EMPTY;

  // @heuristic::action   Take the best winning card.
  //return Heuristics::best_jabbing_card(trick, hi);
  return hi.hand().next_jabbing_card(trick.winnercard());
} // Card Heuristics::poverty_overjab_re(Trick trick, HeuristicInterface hi)


/**
 ** best winning card for the last card
 *
 ** @param        trick        current trick
 ** @param        hi        heuristic interface
 **
 ** @return       card to play, 'Card()' if the heuristic does not match
 **
 ** @author       Diether Knof
 **
 ** @version      0.7.3
 **/
Card
Heuristics::poverty_best_winning_card(Trick const& trick,
                                      HeuristicInterface const& hi)
{
  // @heuristic::name   poverty: best winning card
  // @heuristic::idea   As last player in a trick take the lowest card to win the trick.

  // @heuristic::condition   only for the not poverty player in a poverty
  if (hi.game().type() != GAMETYPE::POVERTY)
    return Card::EMPTY;
  if (   (hi.team() != TEAM::RE)
      || hi.is_soloplayer())
    return Card::EMPTY;

  // @heuristic::condition   last player in the trick
  if (!trick.islastcard() )
    return Card::EMPTY;

  // @heuristic::action   Play the best winning card.
  return Heuristics::best_winning_card(trick, hi);
} // Card Heuristics::poverty_best_winning_card(Trick trick, HeuristicInterface hi)

/**
 ** calculates all points of own team in Game g
 **
 ** @param     hi        heuristic interface
 ** @param     with_current_trick   whether to take the current trick into account
 **
 ** @return    points of own team in the game (minimum estimator)
 **
 ** @author    Borg Enders
 ** @author    Diether Knof
 **
 ** @version   0.7.10
 **/
unsigned
calcPointsOfOwnTeam( HeuristicInterface const& hi,
                    bool const with_current_trick)
{
  if (!::in_running_game())
    return 0;

  Game const& g = hi.game();
  unsigned points = 0;
  unsigned players = 0;

  for (unsigned p = 0; p < g.playerno(); ++p) {
    if (hi.guessed_same_team(p)) {
      points += g.points_of_player(p);
      players += 1;
    }
  } // for (p < g.playerno())

  if (   (players == 1)
      && !hi.game().is_solo()) {
    // add the smallest points of the unknown players
    unsigned min_points = UINT_MAX;
    for (unsigned p = 0; p < g.playerno(); ++p) {
      if (   !hi.guessed_same_team(p)
          && !hi.guessed_opposite_team(p)
          && (g.points_of_player(p) <= min_points))
        min_points = g.points_of_player(p);
    } // for (p < g.playerno())
    if (min_points < UINT_MAX)
      points += min_points;
  } // if (players == 1)

  Trick const& tr = g.trick_current();
  if (tr.isfull()) {
    if (hi.guessed_same_team(tr.winnerplayer()))
      points += tr.points();
  } else if (with_current_trick) {
    if (   checkAllMyTeam(tr, hi)
        && (   hi.guessed_same_team(tr.winnerplayer())
            || (   !tr.has_played(g.player(hi.game().no(hi.player())))
                && tr.winnercard().less(hi.hand().validcards(tr).highest_card()))
           )
       )
      points += tr.points() + hi.hand().validcards(tr).lowest_value();
  }

  return points;
} // unsigned calcPointsOfOwnTeam( HeuristicInterface hi, bool with_current_trick = true )


/**
 ** calculates all points of opposite team in Game g
 **
 ** @param     hi        heuristic interface
 ** @param     g         current game
 ** @param     with_current_trick   whether to take the current trick into account
 **
 ** @return    points of other team in game (maximum estimator)
 **
 ** @author    Borg Enders
 ** @author    Diether Knof
 **
 ** @version   0.7.10
 **/
unsigned
calcPointsOfOppositeTeam(HeuristicInterface const& hi,
                         bool const with_current_trick)
{
  if (!::in_running_game())
    return 0;

  Game const& g = hi.game();
  Trick const& tr = g.trick_current();
  // calculation
  // * without current trick:
  //   played points
  //   - points got by own team (without current trick)
  //   - points of the current trick
  // * with current trick:
  //   played points
  //   - points got by own team (with current trick)
  //   + highest value of the player (worst case), if he has not played in the current trick
  if (with_current_trick)
    return (g.played_points() // all points in the game
            - calcPointsOfOwnTeam(hi, with_current_trick) // points potentially in opposite game
            + (!tr.has_played(hi.player()) // if player has not played, add card.
               ? hi.hand().validcards(tr).highest_value() : 0) // worst case: most point in opposite team
           );
  else
    return (g.played_points() // all points in the game
            - calcPointsOfOwnTeam(hi, with_current_trick) // points potentially in opposite game
            - tr.points() // without current trick remove points of this trick => still remaining
           );

#ifdef OUTDATED // minimum estimator
  // 0.7.10
  unsigned points = 0;
  unsigned players = 0;

  for (unsigned p = 0; p < g.playerno(); ++p) {
    if (hi.guessed_opposite_team(p)) {
      points += g.points_of_player(p);
      players += 1;
    }
  } // for (p < g.playerno())

  if (   (players == 1)
      && !hi.game().is_solo()) {
    // add the smallest points of the unknown players
    unsigned min_points = UINT_MAX;
    for (unsigned p = 0; p < g.playerno(); ++p) {
      if (   !hi.guessed_opposite_team(p)
          && !hi.guessed_same_team(p)
          && (g.points_of_player(p) <= min_points))
        min_points = g.points_of_player(p);
    } // for (p < g.playerno())
    if (min_points < UINT_MAX)
      points += min_points;
  } // if (players == 1)

  Trick const& tr = g.trick_current();
  if (tr.isfull()) {
    if (hi.guessed_opposite_team(tr.winnerplayer()))
      points += tr.points();
  } else if (with_current_trick) {
    if (   checkAllOppositeTeam(tr, hi)
        || hi.guessed_opposite_team(tr.winnerplayer()))
      points += tr.points() + hi.hand().validcards(tr).lowest_value();
  }
  return points;
#endif
} // unsigned calcPointsOfOppositeTeam( HeuristicInterface hi, bool with_current_trick = true)

/**
 ** checks if an announcement should be made
 **
 ** @param        hi        heuristic interface
 **
 ** @return       returns true if an announcement should be made
 **
 ** @author    Borg Enders
 **
 ** @version      Beta
 **/
bool
Heuristics::make_announcement( HeuristicInterface const& hi)
{
  // @heuristic::name   ?  make announcement  ?
  // @heuristic::idea announcement decision bases on Heuristics::CalcHandValue
  Game const& g = hi.game();

  if (hi.player().next_announcement() == ANNOUNCEMENT::NOANNOUNCEMENT)
    return false;

  if (   (game_status >= GAMESTATUS::GAME_PLAY)
      && ( hi.game().trick_current().startplayer() == hi.player())
      && hi.game().trick_current().isempty() )
  {
    return false;

  }

  int const opp_p = calcPointsOfOppositeTeam(hi);
  int const own_p = calcPointsOfOwnTeam(hi);
  if( own_p > 120 )
    return true;
  if( opp_p >= 120 )
    return false;

  int value = 0;

  value = CalcHandValue( hi );

  value += own_p / 20; // previous 25

  if (   !hi.game().is_solo()
      && (hi.game().swines_owner() == NULL)
      && (hi.game().hyperswines_owner() == NULL))
  {
    value += ((hi.hand().numberofclubqueens()+1)*hi.hand().numberofdolle())*calcPointsOfOwnTeam(hi) / 25;
  }

  value -= opp_p / 15; // previous 20


  if( ( hi.team() == TEAM::CONTRA
       && (hi.hand().numberofdolle() == 0
           && !hi.hand().has_swines()
           && !hi.hand().has_hyperswines())
      )
    )
    value -=1;

  if( g.announcement_of_team( opposite( hi.team() ) ).announcement
     != ANNOUNCEMENT::NOANNOUNCEMENT  )
  {
    value -= 1;
    if (    GAMETYPE::is_solo(hi.game().type())
        &&  (hi.team() == TEAM::RE) )
    {
      value -= 6;

    }

  }
  if (    GAMETYPE::is_solo(hi.game().type())
      &&  (hi.team() == TEAM::RE) )
  {
    value -= 2;

    if( hi.game().type() == GAMETYPE::SOLO_MEATLESS )
      value -=2;

    if ( hi.game().swines_owner()
        && hi.game().soloplayer() != *hi.game().swines_owner() )
    {
      value -=5;

    }
    if ( hi.game().hyperswines_owner()
        && hi.game().soloplayer() != *hi.game().hyperswines_owner() )
      value -=2;
  }

  if (    !GAMETYPE::is_solo(hi.game().type())
      &&  (hi.team() == TEAM::CONTRA)
      && hi.hand().numberofdolle() == 2 )
    value +=1; // reference 235643

  if(   (game_status < GAMESTATUS::GAME_PLAY)
     || (   (g.trick_current().no() == 0)
         && g.trick_current().isempty() ) ) // before first card I should be 100% sure what I do
    value -= 1;


  // (trumps - no trumps) = trumps - (cards on hand - trumps ) = 2* trumps - cards on hand
  if ( 2*hi.hand().numberoftrumps() > hi.hand().cardsnumber() )
  {

    if (   hi.value(Aiconfig::AGGRESSIVE) )
    {
      value += (2*hi.hand().numberoftrumps() - hi.hand().cardsnumber() ) /6; // previously /5
    } else
      value += (2*hi.hand().numberoftrumps() - hi.hand().cardsnumber() ) /9;
  }


  Trick const t = (game_status < GAMESTATUS::GAME_PLAY
                   ? Trick(g.startplayer())
                   : g.trick_current());
  if (   hi.teamofplayer(t.winnerplayer()) != hi.team()
      ||  oppositeTeamCanWinTrick( t, hi ) )
  {
    value -= 1;
    if ( GAMETYPE::is_solo( hi.game().type() )
        && ( hi.team() == TEAM::RE ) )
      value -= 5;
  }
  if (   hi.teamofplayer(t.winnerplayer()) == hi.team()
      &&  !oppositeTeamCanWinTrick( t, hi ) )
    value += 2;

  if (   (hi.game().type() == GAMETYPE::MARRIAGE)
      && (hi.game().marriage_selector() == MARRIAGE_SELECTOR::TEAM_SET)
      && ( hi.team() == TEAM::RE ) )
  {
    if ( hi.is_soloplayer())
      value += 1;
    else
      value += 3; // previous 6 // 2do should be 3*numberofqueens still in game
    if( hi.is_winnerplayer(t))
      value +=  hi.hand().numberof( Card::CLUB, Card::ACE )
        + hi.hand().numberof( Card::SPADE, Card::ACE )
        + hi.hand().numberof( Card::HEART, Card::ACE );

    if ( hi.game().swines_owner() != NULL
        && hi.game().soloplayer() == *hi.game().swines_owner())
    {
      value +=2 + 2*hi.hand().numberofdolle();
      if( hi.game().hyperswines_owner() == NULL)
        value +=2; // reference 040594
    }
    if ( hi.game().hyperswines_owner() != NULL
        && hi.game().soloplayer() == *hi.game().hyperswines_owner() )
      value +=2;
    if (   hi.game().swines_owner() == NULL
        && hi.game().hyperswines_owner() == NULL )
    {
      value += 2 + 2*hi.hand().numberofdolle();
    }
  } else {
    if ( hi.team() == TEAM::RE )
    {
      if ( hi.hand().has_swines() )
      {
        value +=2 * hi.hand().numberofdolle();
      }
      if ( hi.game().swines_owner() == NULL
          && hi.game().hyperswines_owner() == NULL)
      {
        value +=2*hi.hand().numberofdolle();
      }
    }

  }

  if (   (hi.game().type() == GAMETYPE::POVERTY)
      && (hi.player() == hi.game().poverty_partner()) )
  {
    value -= 15 - hi.hand().numberofdolle() - hi.hand().numberofclubqueens(); // previously: 7, 8, 13
    // 13 Reference 243666


    if( hi.game().trick_current().isfull() )
      value -= 6; // reference 243666
  }

  if (   (hi.game().type() == GAMETYPE::NORMAL
          || hi.game().type() == GAMETYPE::MARRIAGE) )
  {

    if( hi.hand().numberoftrumps() < 1.5* hi.hand().cardsnumber() )
      value -=  ( hi.hand().cardsnumber() - hi.hand().numberoftrumps() - 1);
  }




  if( (    GAMETYPE::is_solo( hi.game().type() )
       ||  (hi.game().marriage_selector() == MARRIAGE_SELECTOR::SILENT))
     && (!g.last_chance_to_announce(ANNOUNCEMENT::NO120, hi.player() )))
    return false;


  if (::in_running_game()) {
    if( (   hi.game().trick_current().isfull()  // trick won
         && hi.is_winnerplayer(hi.game().trick_current())
         && hi.game().trick_current().points() > 15
        )
      )
      value += 1;

    if (   hi.game().trick_current().isfull()  // trick lost
        && !hi.is_winnerplayer(hi.game().trick_current()) )
      value -= 1;

    if( hi.game().trick_current().startplayer() != hi.player()
       && hi.game().trick_current().isempty() )
    {
      value -= 1;

    }
  }


#ifdef ANNOUNCE_DEBUG
  if( !g.isvirtual() )
    cout << "RE/CO: " << hi.player().name() << "\t" << value << "\t"
      << (int)hi.value(Aiconfig::ANNOUNCELIMIT) << " + "
      << (int)hi.value(Aiconfig::ANNOUNCECONFIG) << std::endl;
#endif

  return ( value >=  (static_cast<int>(hi.value(Aiconfig::ANNOUNCELIMIT))
                      + static_cast<int>(hi.value(Aiconfig::ANNOUNCECONFIG))));
} // bool Heuristics::make_announcement( HeuristicInterface hi)





/**
 ** checks if No90 should be said
 **
 ** @param        hi        heuristic interface
 **
 ** @return       true if hi should say No90
 **
 ** @author    Borg Enders
 **
 ** @version      Beta
 **/
bool
Heuristics::say_no90( HeuristicInterface const& hi )
{
  // @heuristic::name   ?  say no 90  ?
  // @heuristic::idea decision to make the annoncement no 90 depending on Heuristics::CalcHandValue
  int value = 0;
  Game const& g = hi.game();

  int own_p = calcPointsOfOwnTeam(hi);
  int opp_p = calcPointsOfOppositeTeam(hi);

  if( own_p > 150 )
    return true;

  if( g.announcement_of_team( opposite( hi.team() ) ).announcement
     > ANNOUNCEMENT::NO120 )
    return false;

  value = CalcHandValue( hi )
    - max( 0, (int)g.tricks_remaining_no() - 7)
    * static_cast<int>(hi.value(Aiconfig::ANNOUNCELIMITDEC));

  if( g.announcement_of_team( opposite( hi.team() ) ).announcement
     != ANNOUNCEMENT::NOANNOUNCEMENT  )
    value -= 2;

  if  (   hi.team_information().guessed_partner()
       && (hi.team_information().guessed_partner()->announcement()
           != ANNOUNCEMENT::NOANNOUNCEMENT)
       && (hi.team() == TEAM::RE) ) {
    if (   hi.game().swines_owner() == NULL
        && hi.game().hyperswines_owner() == NULL ) {
      value += 8*hi.hand().numberofdolle(); // 7: reference 123343, 8: 139722_1
    }
    opp_p -= 21* hi.hand().numberofdolle(); // reference 237268
    value += 2; // reference 139722_1,
  }

  if (   (hi.game().type() == GAMETYPE::POVERTY )
      && (hi.game().poverty_partner() == hi.player()))
  {
    value -= 2;
    value -= (2-hi.hand().numberofdolle());
    value -= 5 - hi.hand().numberofdolle() - hi.hand().numberofclubqueens();
    value -= 7 - hi.hand().numberof(Card::QUEEN);
  }

  value -= (2-hi.hand().numberofdolle());

  if( hi.game().swines_owner() == NULL
     && hi.game().hyperswines_owner() == NULL)
  { // reference 101732
    value += hi.hand().numberofdolle()*hi.hand().numberofclubqueens();
    if( (hi.cards_information().played(Card::CLUB_QUEEN) == 0)
       && (hi.team() == TEAM::RE)
       && (!GAMETYPE::is_solo( hi.game().type() ))
      )
      value += 1 * hi.hand().numberofdolle(); // reference 139722
  }

  if( (   hi.game().type() == GAMETYPE::MARRIAGE
       && hi.game().marriage_selector() == MARRIAGE_SELECTOR::TEAM_SET
       &&  hi.team() == TEAM::RE ) )

  {
    if (   !hi.value(Aiconfig::AGGRESSIVE) )
    {
      value -=3;
    }

    if( hi.cards_information().played(Card::CLUB_QUEEN) == 0
       && hi.hand().numberoftrumps() >= g.tricks_remaining_no() - 1
      )

    {

      value += 1 + hi.hand().numberofdolle();
      if (hi.hand().numberofclubqueens() == 2)
        value -=3;
    }
  }
  for( vector<Card::Color>::const_iterator
      c = hi.game().rule().card_colors().begin();
      c != hi.game().rule().card_colors().end();
      ++c )
  {
    Card::Color color = *c;

    if( color == hi.game().trumpcolor() )
      continue;

    if ( !hi.jabbedbyownteam(color)
        && hi.hand().numberof( color ) > 0 )
    {
      if( ( hi.hand().numberof( color, Card::ACE ) == 0 ) )
      {
        value -= (hi.color_runs( color )+hi.hand().numberof( color ));
      } else
        value -= 1;
    }

  } // (for color)

  if(    !g.hastrick(::opposite(hi.team())) )
    value += 2 * static_cast<int>(hi.value(Aiconfig::ANNOUNCELIMITDEC));





  opp_p+= 3 * g.tricks_remaining_no(); //previous: 5,4
  own_p+= 8 * g.tricks_remaining_no(); // previous: 6, 7

  own_p += (int)(1.9 * hi.hand().numberoftrumps()
                 * hi.hand().numberofswines());
  own_p += (int)(1.4 * hi.hand().numberoftrumps()
                 * hi.hand().numberofhyperswines());


  if( GAMETYPE::is_solo( hi.game().type() ) )
  {
    own_p += 20 * hi.hand().numberoftrumps();
    opp_p += 20 * hi.cards_information().remaining_trumps_others();
  }



  Trick const t = (game_status < GAMESTATUS::GAME_PLAY
                   ? Trick(g.startplayer())
                   : g.trick_current());

  if (   hi.teamofplayer(t.winnerplayer()) != hi.team()
      ||  oppositeTeamCanWinTrick( t, hi ) )
    value -= 2;

  if (   hi.teamofplayer(t.winnerplayer()) == hi.team()
      &&  !oppositeTeamCanWinTrick( t, hi ) )
  {
    value += 1;

  }

  if( g.announcement_of_team( opposite( hi.team() ) ).announcement
     != ANNOUNCEMENT::NOANNOUNCEMENT  )
  {
    value -= 3;
  }

  if (!hi.value(Aiconfig::AGGRESSIVE))
  {
    value -= 1;
    if( hi.team() == TEAM::CONTRA)
      value -= 1;
  }




#ifdef ANNOUNCE_DEBUG
  if( !g.isvirtual() )
    cout << "NO90: " << hi.player().name() << "\t" << value << " + "
      << 2 * (int)hi.value(Aiconfig::ANNOUNCELIMITDEC ) << "\t"
      << (int)hi.value(Aiconfig::ANNOUNCELIMIT) << " + "
      << 2 * (int)hi.value(Aiconfig::ANNOUNCECONFIG) << std::endl;
  cout << "\t" << own_p << "\t" << 1.3 * opp_p << std::endl;
#endif
  return (   (    value + 2 * (int)hi.value(Aiconfig::ANNOUNCELIMITDEC )
              >     (int)hi.value(Aiconfig::ANNOUNCELIMIT)
              + 2*(int)hi.value(Aiconfig::ANNOUNCECONFIG)
             )
          && (   (hi.game().played_points() <= 60)
              || (own_p > 1.3 * opp_p) )  // previous: 2, 1.5
          && (opp_p < 70)
          && (   opp_p > 0
              || g.last_chance_to_announce(ANNOUNCEMENT::NO90,
                                           hi.player())
             )
         );
} // bool Heuristics::say_no90( HeuristicInterface hi )


/**
 ** checks if No60 should be said
 **
 ** @param        hi        heuristic interface
 **
 ** @return       true if hi should say No60
 **
 ** @author    Borg Enders
 **
 ** @version      Beta
 **/
bool
Heuristics::say_no60( HeuristicInterface const& hi)
{
  // @heuristic::name   ?  say no 60  ?
  // @heuristic::idea decision to make the annoncement no 60 depending on Heuristics::CalcHandValue
  int value = 0;
  Game const& g = hi.game();

  if( g.announcement_of_team( opposite( hi.team() ) ).announcement
     > ANNOUNCEMENT::NO120 )
    return false;



  value = CalcHandValue( hi )
    - max( 0, (int)g.tricks_remaining_no() - 5)
    * static_cast<int>(hi.value(Aiconfig::ANNOUNCELIMITDEC));

  if( g.announcement_of_team( opposite( hi.team() ) ).announcement
     != ANNOUNCEMENT::NOANNOUNCEMENT  )
    value -= 2;

  int own_p = calcPointsOfOwnTeam(hi);
  int opp_p = calcPointsOfOppositeTeam(hi);


  if( own_p > 180 )
    return true;

  if (   (hi.game().type() == GAMETYPE::POVERTY
          && hi.game().poverty_partner() == hi.player()) )
  {
    value -= 7;

  }

  if (    GAMETYPE::is_solo(hi.game().type()) )
  {
    value -= 6;
  }

  value -= g.tricks_remaining_no() / 4;


  opp_p+= 3 * g.tricks_remaining_no(); //previous: 5, 4
  opp_p-=4; // reference 233652
  own_p+= 6 * g.tricks_remaining_no(); // previous: 5

  own_p += 3 * hi.hand().numberoftrumps() * hi.hand().numberofswines();
  own_p += 3 * hi.hand().numberoftrumps() * hi.hand().numberofhyperswines();

  Trick const t = (game_status < GAMESTATUS::GAME_PLAY
                   ? Trick(g.startplayer())
                   : g.trick_current());

  if (   hi.teamofplayer(t.winnerplayer()) != hi.team()
      ||  oppositeTeamCanWinTrick( t, hi ) )
    value -= 2;

  if (   hi.teamofplayer(t.winnerplayer()) == hi.team()
      &&  !oppositeTeamCanWinTrick( t, hi ) )
    value += 1;

  if (   (hi.game().type() == GAMETYPE::POVERTY)
      && (hi.player() == hi.game().poverty_partner()) )
  {
    value -= 4 - hi.hand().numberofdolle() - hi.hand().numberofclubqueens();
  }
  if ( hi.game().rule()(Rule::DOLLEN) )
  {
    value -= 5 - 2*hi.hand().numberofdolle();
    if ( hi.cards_information().played(Card::DOLLE)+hi.hand().numberofdolle() < 2 )
      value -=2;
  }
#ifdef ANNOUNCE_DEBUG
  if( !g.isvirtual() )
    cout << "NO60: " << hi.player().name() << "\t" << value << " + "
      << 3 * (int)hi.value(Aiconfig::ANNOUNCELIMITDEC ) << "\t"
      << (int)hi.value(Aiconfig::ANNOUNCELIMIT) << " + "
      << 3 * (int)hi.value(Aiconfig::ANNOUNCECONFIG) << std::endl;
  cout << own_p << "\t" << opp_p << std::endl;
#endif
  return (   (  value + 3 * (int)hi.value(Aiconfig::ANNOUNCELIMITDEC)
              >   (int)hi.value(Aiconfig::ANNOUNCELIMIT)
              + 3 * (int)hi.value(Aiconfig::ANNOUNCECONFIG)
             )
          && own_p > 1.7 * opp_p  // previous: 3, 2 (1.7 reference:233652)
          && opp_p < 60
          && (   opp_p > 0
              || g.last_chance_to_announce(ANNOUNCEMENT::NO60,
                                           hi.player())
             )
         );
} // bool Heuristics::say_no60( HeuristicInterface hi)


/**
 ** checks if No30 should be said
 **
 ** @param        hi        heuristic interface
 **
 ** @return       true if hi should say No30
 **
 ** @author    Borg Enders
 **
 ** @version      Beta
 **/
bool
Heuristics::say_no30( HeuristicInterface const& hi)
{
  // @heuristic::name   ?  say no 30  ?
  // @heuristic::idea decision to make the annoncement no 30 depending on Heuristics::CalcHandValue
  int value = 0;
  Game const& g = hi.game();

  if( g.announcement_of_team( opposite( hi.team() ) ).announcement
     > ANNOUNCEMENT::NO120 )
    return false;
  if( g.announcement_of_team( hi.team() ) != ANNOUNCEMENT::NO60 ) // do not skip an announcement
    return false;


  value = CalcHandValue( hi )
    - max( 0, (int)g.tricks_remaining_no() - 3)
    * static_cast<int>(hi.value(Aiconfig::ANNOUNCELIMITDEC));

  if( g.announcement_of_team( opposite( hi.team() ) ).announcement
     != ANNOUNCEMENT::NOANNOUNCEMENT  )
    value -= 2;

  int own_p = calcPointsOfOwnTeam(hi);
  int opp_p = calcPointsOfOppositeTeam(hi);

  if( own_p > 210 )
    return true;


  opp_p+= 4 * g.tricks_remaining_no();
  own_p+= 5 * g.tricks_remaining_no();

  value -=   hi.cards_information().remaining_others(Card::DOLLE);

#ifdef ANNOUNCE_DEBUG
  if( !g.isvirtual() )
    cout << "NO30: " << hi.player().name() << "\t" << value << " + "
      << 6 * (int)hi.value(Aiconfig::ANNOUNCELIMITDEC ) << "\t"
      << (int)hi.value(Aiconfig::ANNOUNCELIMIT) << " + "
      << (int)hi.value(Aiconfig::ANNOUNCECONFIG) << std::endl;
#endif
  return (  (   value + 6 * (int)hi.value(Aiconfig::ANNOUNCELIMITDEC)
             >   (int)hi.value(Aiconfig::ANNOUNCELIMIT)
             + (int)hi.value(Aiconfig::ANNOUNCECONFIG)
            )
          && own_p > 7 * opp_p
          && opp_p < 30
          && (    opp_p > 0
              || g.last_chance_to_announce(ANNOUNCEMENT::NO30,
                                           hi.player())
             )
         );
} // bool Heuristics::say_no30( HeuristicInterface hi)


/**
 ** test whether a 'no 0' could be announced
 ** checks, that the other team does not have a trick
 ** and that the hand value is big enoug.
 **
 ** @param     hi     heuristic interface
 **
 ** @return    whether to announce 'no 0'
 **
 ** @author    Borg Enders
 ** @author    Diether Knof
 **
 ** @version   0.7.3
 **/
bool
Heuristics::say_no0( HeuristicInterface const& hi)
{
  // @heuristic::name   ?  say no 0  ?
  // @heuristic::idea decision to make the annoncement no 0 depending on Heuristics::CalcHandValue
  Game const& game = hi.game();

  if( game.announcement_of_team( opposite( hi.team() ) ).announcement
     != ANNOUNCEMENT::NOANNOUNCEMENT )
    return false;

  if( game.announcement_of_team( hi.team() ) != ANNOUNCEMENT::NO30 ) // do not skip an announcement
    return false;

  // make sure, that the other team does not have a trick
  for (unsigned p = 0; p < game.playerno(); ++p)
    if (   (hi.teamofplayer(game.player(p)) != hi.team())
        && game.hastrick(game.player(p)))
      return false;

  // as long as the current trick is not closed, it has to be taken into account
  if (   game.trick_current().isfull()
      && (hi.teamofplayer(game.trick_current().winnerplayer())
          != hi.team()) )
    return false;

  // wait for the last possible moment
  if (!game.last_chance_to_announce(ANNOUNCEMENT::NO0,
                                    hi.player()))
    return false;

  if( highest_trump_of_opposite_team(hi).less(hi.hand().lowest_trump())
     && hi.hand().numberoftrumps()
     >= hi.game().tricks_remaining_no())
    return true;

  // ToDo: check that no opposite player has the highest trump card
  // Test: force a player by 'ANNOUNCELIMITDEC' high



  int const value = CalcHandValue( hi )
    - max( 0, (int)game.tricks_remaining_no() )
    * static_cast<int>(hi.value(Aiconfig::ANNOUNCELIMITDEC));

#ifdef ANNOUNCE_DEBUG
  if( !game.isvirtual() )
    cout << "NO0: " << hi.player().name() << "\t" << value << " + "
      << 4 * (int)hi.value(Aiconfig::ANNOUNCELIMITDEC ) << "\t"
      << (int)hi.value(Aiconfig::ANNOUNCELIMIT) << " + "
      << 3 * (int)hi.value(Aiconfig::ANNOUNCECONFIG) << std::endl;
#endif

  return (value + 4 * static_cast<int>(hi.value(Aiconfig::ANNOUNCELIMITDEC))
          > static_cast<int>(hi.value(Aiconfig::ANNOUNCELIMIT)
                             + 3 * hi.value(Aiconfig::ANNOUNCECONFIG))
         );
} // bool Heuristics::say_no0( HeuristicInterface hi)


/**
 ** finds highest card of color c
 **
 ** @param     trick    trick to check
 ** @param     hi       heuristic interface
 ** @param     tcolor   color to choose
 **
 ** @return    the card to play ( or 'Card()' if no card was found )
 **
 ** @author    Borg Enders
 ** @author    Diether Knof
 **
 ** @version   0.7.6
 **
 ** @todo      play the lowest highest card
 **/
Card
Heuristics::play_highest_color(Trick const& trick,
                               HeuristicInterface const& hi,
                               Card::TColor const& tcolor)
{
  // @heuristic::name   meatless: play highest color
  // @heuristic::idea   Heuristic for solo::MEATLESS to play the longest color or follow previous color

  HandCard const& card = hi.hand().highest_card(tcolor);

  if (card.value() == Card::ACE)
    return card;

  // @heuristic::condition   the player can jab the trick
  if (!(   trick.isempty()
        || trick.winnercard().less(card)))
    return Card::EMPTY;

  // @heuristic::condition   the player has the highest card
  if (card.less(hi.cards_information().highest_remaining_card_of_others(tcolor)))
    return Card::EMPTY;

  // @heuristic::action   play the highest card of the color
  return card;
} // Card Heuristics::play_highest_color( Trick t, HeuristicInterface hi, Card::Color co )

/**
 ** As first player and the partner in backhand, take a small trump.
 ** prerequisites:
 **   1. first player in the trick
 **   2. last player is partner
 ** take a small trump
 ** If the partner has the highest card still in the game, take a trump
 ** with a high value (fox or ten)
 **
 ** @param     trick   current trick
 ** @param     hi      heuristic interface
 **
 ** @return    card to play, 'Card()' if the heuristic does not match
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **
 ** @todo      check that the partner has no problems with his trumps
 **/
Card
Heuristics::partner_backhand_draw_trump(Trick const& trick,
                                        HeuristicInterface const& hi)
{
  // @heuristic::name   partner backhand draw trump
  // @heuristic::idea   as first player and the partner in backhand, draw trump

  // @heuristic::condition   first player in the trick
  if (!trick.isstartcard() )
    return Card::EMPTY;

  // @heuristic::condition   last player is partner
  if (!(hi.teamofplayer(trick.lastplayer()) == hi.team()))
    return Card::EMPTY;

  // @heuristic::condition   the opposite team has not made any better announcement then the own team
  if (  hi.game().announcement_of_team(TEAM::opposite(hi.team()))
      > hi.game().announcement_of_team(hi.team()))
    return Card::EMPTY;

  // the partner
  Player const& partner = trick.lastplayer();

  bool partner_has_highest_trump = false;
  { // check whether the partner has the highest trump in the game
    HandCard const highest_trump
      = HandCard(hi.hand(),
                 hi.cards_information().highest_remaining_trump_of_others());
    if (hi.handofplayer(partner).contains(highest_trump))
    {
      // check that no other player does have the card, also
      if (   highest_trump.isdolle()
          && hi.game().second_dolle_over_first()
          && hi.cards_information().of_player(partner).must_have(Card::DOLLE)) {
        partner_has_highest_trump = true;
      } else {

        partner_has_highest_trump = true;
        for (unsigned p = 0; p < hi.game().playerno(); ++p) {
          Player const& player = hi.game().player(p);
          if (   (hi.teamofplayer(player) != hi.team())
              && (hi.handofplayer(player).contains(highest_trump))) {
            partner_has_highest_trump = false;
            break;
          }
        } // for (p < hi.game().playerno())
      } // if !(second dolle over first)
    } // if (partner has highest trump)
  } // check whether the partner has the highest trump in the game

  vector<Card> cards_to_play;
  if (partner_has_highest_trump) {
    cards_to_play.push_back(Card(Card::DIAMOND, Card::ACE));
    cards_to_play.push_back(Card(Card::DIAMOND, Card::TEN));
  } else if (hi.hand().numberoftrumps() >= 3) { // *Value*
    cards_to_play.push_back(Card(Card::DIAMOND, Card::KING));
    cards_to_play.push_back(Card(Card::DIAMOND, Card::JACK));
    cards_to_play.push_back(Card(Card::HEART,   Card::JACK));
    cards_to_play.push_back(Card(Card::SPADE,   Card::JACK));
    cards_to_play.push_back(Card(Card::DIAMOND, Card::NINE));
  }

  // @heuristic::condition   at least three trump on the hand if the partner does not have the highest card
  // ToDo: improve

  for (vector<Card>::const_iterator c = cards_to_play.begin();
       c != cards_to_play.end();
       ++c)
    if (  hi.hand().contains(*c)
        && !hi.game().is_special(HandCard(hi.hand(), *c)))
      return *c;

  return Card::EMPTY;
} // static Card Heuristics::partner_backhand_draw_trump(Trick trick, HeuristicInterface hi)

/**
 ** play trumps to draw the last trumps
 **
 ** @param     trick   current trick
 ** @param     hi      heuristic interface
 **
 ** @return    card to play
 **
 ** @author    Borg Enders
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
Card
Heuristics::draw_trump(Trick const& trick, HeuristicInterface const& hi)
{
  // @heuristic::name   draw trump
  // @heuristic::idea play trumps to force other players to play also trumps

  // @heuristic::condition   first player
  if( !trick.isstartcard() )
    return Card::EMPTY;


  // @heuristic::condition   more trumps then remaining trumps * 2
  // ToDo: check 'cannot have' for players
  if (! (   (hi.hand().numberoftrumps() * 2
             > hi.cards_information().remaining_trumps_others())
         && hi.cards_information().remaining_trumps_others() ) )
    return Card::EMPTY;

  if(  GAMETYPE::is_solo( hi.game().type() )
     && hi.is_soloplayer()
     && (hi.hand().numberoftrumps() * 1.5 < hi.cards_information().remaining_trumps_others())
     && (hi.game().trick_current().no() < 1)
    )
    return Card::EMPTY;

  if (hi.cards_information().remaining_trumps_others()
      >= hi.hand().numberoftrumps() + 6)
    return Card::EMPTY;

  // @heuristic::condition   the opposite team has not made any announcement
  if (   !TEAM::is_real(hi.team())
      || hi.game().announcement_of_team(TEAM::opposite(hi.team())))
    return Card::EMPTY;

  if (   !TEAM::is_real(hi.team())
      || hi.game().announcement_of_team(TEAM::opposite(hi.team())))
    return Card::EMPTY;

  // @heuristic::condition   the opposite team still has trump
  if (highest_trump_of_opposite_team(hi) == Card::EMPTY)
    return Card::EMPTY;

  HandCard const c = lowest_highest_trump(trick, hi);
  if (c.is_special()
      && c.less(highest_card_behind_of_opposite_team(trick, hi)))
    return Card::EMPTY;

  return c;
} // Card Heuristics::draw_trump(Trick trick, HeuristicInterface hi)


/**
 **
 ** prerequisites:
 **   1. startplayer
 **   2. no trumps remain in the game in the opposite team
 **   3. the player has the highest color card of a color
 ** take the highest color card
 **
 ** @param     trick   current trick
 ** @param     hi      heuristic interface
 **
 ** @return    card to play, 'Card()' if the heuristic does not match
 **
 ** @author    Borg Enders
 ** @author    Diether Knof
 **
 ** @version   0.7.6
 **/
Card
Heuristics::play_highest_color_card_in_game(Trick const& trick,
                                            HeuristicInterface const& hi)
{
  // @heuristic::name   play highest color card in game
  // @heuristic::idea   Make a sure trick with a color card

#ifdef OUTDATED
  // DK: 1. Mai 12 
  // @heuristic::condition   startplayer
  if (!trick.isempty())
    return Card::EMPTY;
#endif

  HandCards const cards = hi.hand().cards_single();
  Card best_card; // the card to play
  for (HandCards::const_iterator c = cards.begin();
       c != cards.end();
       ++c) {
    if (c->istrump())
      continue;
    if (!trick.isvalid(*c))
      continue;

    Trick t(trick);
    t += *c;
    if (oppositeTeamCanWinTrick(t, hi))
      continue;

    if (   best_card.is_empty()
        || (c->points() > best_card.points()))
      best_card = *c;

    // ToDo: points equal, take the color with more points remaining
  } // for (c \in cards)

  // @heuristic::action   take color card the opposite team can not get with the most points
  return best_card;
} // Card Heuristics::play_highest_color_card_in_game(Trick trick, HeuristicInterface hi)

/**
 ** as last player grab the trick
 ** prerequisites:
 **   1. the player must be the last player or the players behind cannot get the trick
 **   2. the player can get the trick
 ** take the best card to win the trick
 **
 ** @param     trick   current trick
 ** @param     hi      heuristic interface
 **
 ** @return    card to play, 'Card()' if the heuristic does not match
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
Card
Heuristics::grab_trick(Trick const& trick,
                       HeuristicInterface const& hi)
{
  // @heuristic::name   grab trick
  // @heuristic::idea   as last player jab the trick if possible

  // @heuristic::condition   last player in the trick
  if (!trick.islastcard()
      && oppositeTeamCanWinTrick(trick, hi))
    return Card::EMPTY;

  // @heuristic::condition   the trick does not go to the own team
  if (hi.guessedteamofplayer(trick.winnerplayer()) == hi.team())
    return Card::EMPTY;

  // @heuristic::action   Play the best winning card.
  return Heuristics::best_winning_card_old(trick, hi,
                                           Card::ACE * hi.game().playerno());
} // Card Heuristics::grab_trick(Trick trick, HeuristicInterface hi)


/**
 ** With only trump on hand play best trump card
 **
 ** @param     trick   current trick
 ** @param     hi      heuristic interface
 **
 ** @return    card to play, 'Card()' if the heuristic does not match
 **
 ** @author    Borg Enders
 ** @author    Diether Knof
 **
 ** @version   0.7.9
 **
 ** @todo      see ToDo at end of function
 **/
Card
Heuristics::play_trump(Trick const& trick,
                       HeuristicInterface const& hi)
{
  // @heuristic::name        play trump
  // @heuristic::idea        with only trump on the hand play the best trump card

  // @heuristic::condition   no startcard
  if (trick.isstartcard())
    return Card::EMPTY;

  // no soloplayer behind
  if (soloPlayerBehind(trick, hi))
    return Card::EMPTY;

  if (hi.game().type() == GAMETYPE::POVERTY) {
    if (   (hi.team() == TEAM::RE)
        && (hi.color_runs(trick.startcard().tcolor()) == 0) ) {
      return best_winning_card(trick, hi);
      //return lowest_best_trump_card( trick, hi.hand(), hi );
    } // if (hi.team() == TEAM::RE)
    if (   (hi.team() == TEAM::CONTRA)
        && hi.hand().numberof(trick.startcard().tcolor()) == 0
       ) {
      Player const& re_player = hi.game().poverty_partner();
      if (!trick.has_played(re_player)) {
        if( hi.hand().contains(Card(hi.game().trumpcolor(), Card::TEN)) > 0 )
          return Card(hi.game().trumpcolor(), Card::TEN );

        return lowest_best_trump_card( trick, hi.hand(), hi );
      }
    }
  } // if (hi.game().type() == GAMETYPE::POVERTY)

  // @heuristic::condition   only trumps
  if (hi.hand().hascolor())
    return Card::EMPTY;

  // @heuristic::action      if the trick goes to the own team: play low card
  if (   (trick.winnerteam() == hi.team())
      && (hi.game().less(hi.trump_card_limit(), trick.winnercard())))
    return Heuristics::lowest_best_trump_card(trick, hi.hand(), hi);

  Card const card = Heuristics::best_winning_card_old(trick, hi,
                                                      Card::ACE * hi.game().playerno());

  // @heuristic::action      if the trick has enough points or the player enough high cards: jab it
  if (   (hi.hand().higher_cards_no(trick.winnercard())
          + hi.hand().numberof(trick.winnercard())
          > hi.hand().cardsnumber() / 2)
      || (trick.points()
          > ((1.2 * hi.game().remaining_points())
             / hi.game().tricks_remaining_no()) ) )
    return card;

  // special case: two fox on the hand, not many trumps and partner behind: try fox
  if (   !hi.game().is_solo()
      && (hi.hand().numberoftrumpaces() == 2)
      && !hi.game().rule()(Rule::SWINES)
      && (hi.hand().numberoftrumps() <= 4)
      && hi.team_information().guessed_partner()
      && !trick.has_played(*hi.team_information().guessed_partner())
      && (   trick.startcard().istrump()
          || !hi.handofplayer(*hi.team_information().guessed_partner()).contains(trick.startcard().tcolor()) )
     )
    return Card(hi.game().trumpcolor(), Card::ACE);

  // @heuristic::action      else: play low card
  if (   hi.trump_card_limit().less(trick.winnercard())
      && (trick.points() + 3 * trick.remainingcardno() < 16)) // *Value*
    // ToDo: if there is no card greater then a fox in the game, play such
    return Heuristics::lowest_best_trump_card(trick, hi.hand(), hi);

  if (hi.game().less(hi.trump_card_limit(), card))
    return Card::EMPTY;

  // ToDo: perhaps always return no card, because the player should have enough high trumps
  return card;
} // Card Heuristics::play_trump(Trick trick, HeuristicInterface hi)

/**
 ** I can throw fehl and the trick is not worth taking; I'am third or forth
 **
 ** @param     trick   current trick
 ** @param     hi      heuristic interface
 **
 ** @return    card to play, 'Card()' if the heuristic does not match
 **
 ** @author    Borg Enders
 ** @author    Diether Knof
 **
 ** @version   0.7.6
 **/
Card
Heuristics::let_trick_pass(Trick const& trick,
                           HeuristicInterface const& hi)
{
  // @heuristic::name   let trick pass
  // @heuristic::idea   not enough points to grab trick, throw fehl

  // @heuristic::condition   third or forth player in trick
  if (trick.remainingcardno() > 2 )
    return Card::EMPTY;

  Card::TColor const tcolor = trick.startcard().tcolor();
  // @heuristic::condition  color trick
  if (tcolor == Card::TRUMP)
    return Card::EMPTY;

  // @heuristic::condition  can't serve color
  if ( hi.hand().contains(tcolor))
    return Card::EMPTY;

  // @heuristic::condition  There are not many points in this trick
  if (static_cast<int>(trick.points()) > hi.value(Aiconfig::LIMIT_THROW_FEHL) )
    return Card::EMPTY;

  int modi = 0;
  if (!trick.islastcard()) {
    // ToDo: analyse the cards (and team) the last player has (t.i. estimate, what he will play)
    Player const& player = trick.lastplayer();
    Hand const& hand = hi.handofplayer(player);
    if (hi.guessedteamofplayer(player) == hi.team()) {
      if (hand.contains(tcolor))
        modi += hand.lowest_value(tcolor);
      else
        modi += hand.lowest_value();
    } else { // if !(same team)
      // @heuristic::condition   Do not leave the trick to the opposite team
      if (!hand.contains(tcolor)
          || (hi.guessedteamofplayer(trick.winnerplayer()) != hi.team()) )
        return Card::EMPTY;
      if (hand.contains(tcolor))
        modi += hand.highest_value(tcolor);
      else
        modi += hand.highest_value();
    } // if !(same team)
  } // if (!trick.islastcard())


  // There are not many points in this trick
  if (static_cast<int>(trick.points()) + modi > hi.value(Aiconfig::LIMIT_THROW_FEHL) )
    return Card::EMPTY;

  // the card to play (or none)
  Card const card = choose_best_fehl(trick, hi, true);
  modi += card.value();

  // There are not many points in this trick
  if (static_cast<int>(trick.points()) + modi > hi.value(Aiconfig::LIMIT_THROW_FEHL) )
    return Card::EMPTY;

  // @heuristic::action   Play the best fehl
  return card;
} // Card Heuristics::let_trick_pass(Trick trick, HeuristicInterface hi)


/**
 ** get a trick for announcement
 **
 ** @param     t         trick to check
 ** @param     hi        heuristic interface
 **
 ** @return    the card to play ( or 'Card()' if no card was found )
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.10
 **/
Card
Heuristics::get_trick_for_keeping_announcement(Trick const& t,
                                               HeuristicInterface const& hi)
{
  // @heuristic::name   get trick for announcement
  // @heuristic::idea   If this trick would open the possibility for the other team to win the announcement try to win this trick for the own team

  // @heuristic::condition   the trick goes to the opposite team
  if (hi.guessed_same_team(t.winnerplayer()))
    return Card::EMPTY;

  unsigned const own_points = calcPointsOfOwnTeam(hi, false);
  unsigned const opp_points = calcPointsOfOppositeTeam(hi, false);

  // @heuristic::condition   the trick is critical for keeping the announcement or for deflecting the announcement of the opposite team
  Card const card = best_jabbing_card(t, hi);
#ifdef OUTDATED
  = best_winning_card_old(t, hi, 30);
#endif
  if (   (   hi.game().needed_points_to_win(hi.team())
          > own_points)
      && (   hi.game().needed_points_to_win(hi.team())
          <= (own_points + t.points() + card.value()) ) )
    return card;
  if (   (   hi.game().needed_points_to_win(opposite(hi.team()))
          > opp_points)
      && (   hi.game().needed_points_to_win(opposite(hi.team()))
          <= (opp_points + t.points()
              + hi.hand().validcards(t).lowest_value()
              + ((t.remainingcardno() - 1) * 4) )
         ) )
    // @heuristic::action   take the best winning card
    return card;

  return Card::EMPTY;
} // Card Heuristics::get_trick_for_keeping_announcement(Trick t, HeuristicInterface hi)

/**
 ** Gives the partner a chance to drop his worries
 **
 ** @param     trick current trick
 ** @param     hi    heuristic interface
 **
 ** @return    card to play, 'Card()' if the heuristic does not match
 **
 ** @author    Florian Sundermann
 **
 ** @version   0.7.5
 **/
Card
Heuristics::play_for_partner_worries(Trick const& trick,
                                     HeuristicInterface const& hi)
{
  // @heuristic::name   play for partner worries
  // @heuristic::idea   Plays a card that will win the trick so that the partner can drop a "worry-card"
  Player const* partner = hi.team_information().guessed_partner();
  if( !partner )
    return Card::EMPTY;

  if ( trick.isstartcard() )
    return Card::EMPTY;

  if( !trick.startcard().istrump() )
    return Card::EMPTY;

  if( trick.cardno_of_player(*partner) < trick.actcardno() )
    return Card::EMPTY;

  // the player can win the trick
  if (!cardWillWinTheTrick( trick, hi, hi.hand().highest_card()) )
    return Card::EMPTY;

  { // check for worries
    unsigned worries_no = 0;
    HandCards const validcards = partner->hand().validcards(trick);
    for (HandCards::const_iterator card = validcards.begin();
         card != validcards.end();
         card++) {
      HandCard const& actCard = *card;
      if (actCard.value() >= 10 && (!actCard.isdolle())
          && !actCard.is_special())
      { // partner has worries so search for the lowest card that will win the trick
        worries_no += 1;
      } // partner has worries
    } // search for worries

    // the player has worries
    if (worries_no == 0)
      return Card::EMPTY;
  } // check for worries

  // if the partner has almost only trump, we do have time to get the worrie
  if (   (hi.handofplayer(*partner).numberoftrumps() + 1
          >= hi.handofplayer(*partner).cardsnumber())
      || (hi.cards_information().of_player(*partner).must_have(Card::TRUMP) + 1
          >= partner->cards_to_play()) )
    return Card::EMPTY;

  { // search the best winning card
    Card winningCard;

    HandCards const validcards = hi.hand().validcards(trick);
    for ( HandCards::const_iterator owncard =validcards.begin();
         owncard != validcards.end();
         owncard++)
    {
      HandCard const& wincard = *owncard;
      if ( cardWillWinTheTrick(trick, hi, wincard)
          && ( winningCard.is_empty() || wincard.less(winningCard)) )
      {
        winningCard = wincard;
      }
    } // search for lowest winning card

    return winningCard; // return the lowest winningcard
  } // search the best winning card

} // Card Heuristics::play_for_partner_worries(Trick trick, HeuristicInterface hi)

/**
 ** Start a new color hoping the partner has an ace
 ** prerequisites:
 **   1. it must be the first card in the trick
 **   2. first run of the color
 **   3. nobody has played the color
 ** Then take the card with the greatest value in the shortest color
 **
 **
 ** @param        trick        current trick
 ** @param        hi        heuristic interface
 **
 ** @return       card to play, 'Card()' if the heuristic does not match
 **
 ** @author       Diether Knof
 **
 ** @version      0.7.4
 **/
Card
Heuristics::start_with_color(Trick const& trick,
                             HeuristicInterface const& hi)
{
  // @heuristic::name   start with color
  // @heuristic::idea   play a color hoping the partner has the ace

  // @heuristic::condition   startplayer in the trick
  if (!trick.isstartcard())
    return Card::EMPTY;

  // best card so far to play
  Card best_card;
  // maximal remaining cards of the selected color
  // @heuristic::condition   or there are enough remaining cards for all players
  //                         (players - me)
  unsigned max_remaining_cards_no = hi.game().playerno() - 1;

  for (vector<Card::Color>::const_iterator
       c = hi.game().rule().card_colors().begin();
       c != hi.game().rule().card_colors().end();
       ++c) {
    Card::Color const& color = *c;
    // test that the color is no trumpcolor
    // @heuristic::condition   first run of the color
    if (   (color == hi.game().trumpcolor())
        || (hi.cards_information().played(color)
            > hi.cards_information().of_player(hi.player()).played(color)) )
      continue;

    // the player has the color
    if (hi.hand().numberof(color) == 0)
      continue;

    // @heuristic::condition   following player is not of the opposite team
    if (   (TEAM::maybe_to_team(hi.teamofplayer(trick.player_of_card(1)))
            != hi.team())
        && hi.handofplayer(trick.player_of_card(1)).contains(Card(color, Card::ACE)))
      continue;

    unsigned const remaining_cards_no
      = hi.cards_information().remaining_others(*c);
    // there are at least as many remaining cards as the best so far
    if (remaining_cards_no < max_remaining_cards_no)
      continue;

    // search the card with the highest value
    HandCard const& card = hi.hand().highest_card(color);

    if (   (remaining_cards_no > max_remaining_cards_no)
        || (card.value() > best_card.value()) ) {
      best_card = card;
      max_remaining_cards_no = remaining_cards_no;
    } // if (found better card)
  } // for (c \in possible colors)

  // @heuristic::action   play the card with the highest value in the smallest color
  return best_card;
} // Card Heuristics::start_with_color(Trick trick, HeuristicInterface hi)

/**
 ** Start with a low color to find the partner / in a solo game
 **
 ** @param     trick   current trick
 ** @param     hi      heuristic interface
 **
 ** @return    card to play, 'Card()' if the heuristic does not match
 **
 ** @author    Florian Sundermann
 ** @author    Diether Knof
 **
 ** @version   0.7.6
 **
 ** @todo      split for normal game and for solo
 **/
Card
Heuristics::start_with_low_color(Trick const& trick,
                                 HeuristicInterface const& hi)
{
  // @heuristic::name   start with low color
  // @heuristic::idea   Start the trick with a low color (in a normal game to find the partner, in a solo game always)

  // @heuristic::condition   first card in trick
  if (!trick.isstartcard())
    return Card::EMPTY;

  // no known partner in normal game or a solo game
  if (   !GAMETYPE::is_solo(hi.game().type())
      && hi.team_information().guessed_partner() )
    return Card::EMPTY;

  // do not play for the opposite team
  if (hi.guessed_opposite_team(trick.player_of_card(1)))
    return Card::EMPTY;

  if (!hi.hand().hascolor())
    return Card::EMPTY; // Everything below need color cards!

  Card c;
  { // @heuristic::action   when a singleton color card is found, take it
    for (HandCards::const_iterator card = hi.hand().cards().begin();
         card != hi.hand().cards().end();
         card++)
    {
      HandCard const& actCard = *card;
      if ((! actCard.istrump())
          && (hi.hand().numberof(actCard.color()) == 1)) {
        if (c.is_empty()) {
          c = actCard;
        } else if (hi.cards_information().remaining_others(actCard.color())
                   > hi.cards_information().remaining_others(c.color()) ) {
          c = actCard;
        } else if (hi.color_runs(c.color()) > 0) {
          if (hi.color_runs(actCard.tcolor()) == 0)
            c = actCard;
          else if (c.value() > actCard.value())
            c = actCard;
        } // if ( color_runs > 0)
      } // if( ! actCard.istrump && numberof(actCard.color) == 1)
    } // for (actCard in hi.hand().cards())

    if (   !c.is_empty()
        && (c.points() < 10999))
      return c;
  } // take singleton color card

  { // @heuristic::action   take smallest card from the longest color
    Card::Color longestcolor = Card::NOCARDCOLOR;
    // Determine the longest color
    for (vector<Card::Color>::const_iterator
         col = hi.game().rule().card_colors().begin();
         col != hi.game().rule().card_colors().end();
         ++col)
    {
      Card::Color const& color = *col;
      if (longestcolor == Card::NOCARDCOLOR
          || hi.hand().numberof(color) > hi.hand().numberof(longestcolor))
        longestcolor = color;
    } // for each color

    c = hi.hand().lowest_card(longestcolor); // choose the lowest card
  } // take smallest card from the longest color

  if (c.points() >= 10)
    return Card::EMPTY;
  return c;
} // Card Heuristics::start_with_low_color(Trick trick, HeuristicInterface hi)

/**
 ** Start with a low trump to find the partner
 **
 ** @param     trick   current trick
 ** @param     hi      heuristic interface
 **
 ** @return    card to play, 'Card()' if the heuristic does not match
 **
 ** @author    Florian Sundermann
 ** @author    Diether Knof
 **
 ** @version   0.7.9
 **/
Card
Heuristics::start_with_low_trump(Trick const& trick,
                                 HeuristicInterface const& hi)
{
  // @heuristic::name   start with low trump
  // @heuristic::idea   Start the trick with a low trump to find the partner
  if (GAMETYPE::is_solo(hi.game().type())
      || !trick.isstartcard()
      || hi.team_information().guessed_partner() )
    return Card::EMPTY;

  // @heuristic::condition   enough trumps or re and first card in the game
  if (!(   (hi.game().type() == GAMETYPE::NORMAL)
        && (hi.team() == TEAM::RE)
        && (hi.game().played_cards_no() == 0) ) ) {
    // 6 trump cards or 60% trump on hand
    if (    (hi.hand().numberoftrumps() < 6)
        && ((double)hi.hand().numberoftrumps()
            / (double)hi.hand().cardsnumber()
            < 0.6 ) )/// @todo add a constant or configuration for this
      return Card::EMPTY;
  }

  // @heuristic::action   with enough trumps take a small trump
  HandCard const card
    = HandCard(hi.hand(),
               Heuristics::lowest_best_trump_card(trick, hi.hand(), hi));
  // do not play high trumps or many points
  if (   card.less(hi.value(Aiconfig::TRUMPLIMIT_NORMAL))
      && (card.value() < 10))
    return card;

  return Card::EMPTY;
} // Card Heuristics::start_with_low_trump(Trick trick, HeuristicInterface hi)

/**
 ** play low trump to jab later with high trump
 **
 ** @param     trick   current trick
 ** @param     hi      heuristic interface
 **
 ** @return    card to play, 'Card()' if the heuristic does not match
 **
 ** @author    Borg Enders
 **
 ** @version   0.7.6
 **/
Card
Heuristics::play_to_jab_later(Trick const& trick,
                              HeuristicInterface const& hi)
{
  // @heuristic::name      play to jab later
  // @heuristic::idea      play low trump to jab later with high trump


  // @heuristic::condition   startcard
  if (!trick.isstartcard() )
    return Card::EMPTY;

  // @heuristic::condition   only trumps on the hand
  if (hi.hand().hascolor())
    return Card::EMPTY;

  vector<Card> cards_to_play;
  cards_to_play.push_back(Card(Card::DIAMOND, Card::JACK));
  cards_to_play.push_back(Card(Card::HEART,   Card::JACK));
  if (hi.game().rule()(Rule::WITH_NINES))
    cards_to_play.push_back(Card(Card::DIAMOND, Card::NINE));
  cards_to_play.push_back(Card(Card::DIAMOND, Card::KING));


  // @heuristic::action      play a low trump
  for (vector<Card>::const_iterator c = cards_to_play.begin();
       c != cards_to_play.end();
       ++c)
    if (   hi.hand().contains(*c)
        && !hi.game().is_special(HandCard(hi.hand(), *c)))
      return *c;

  return Card::EMPTY;
} //Card Heuristics::play_to_jab_later(Trick trick, HeuristicInterface hi)


/**
 ** checks if an announcement should be replied with an other announcement
 **
 ** @param     hi        heuristic interface
 **
 ** @return    Announcement::Reply if wanted
 **
 ** @author    Borg Enders
 ** @author    Diether Knof
 **
 ** @version   0.6.3
 **/
Announcement
Heuristics::make_reply( HeuristicInterface const& hi)
{
  // @heuristic::name   ?  make reply  ?
  // @heuristic::idea announcement reply decision bases on Heuristics::CalcHandValue

  Game const& g = hi.game();
  Announcement const announcement_opp
    = g.announcement_of_team( opposite( hi.team() ) ).announcement ;
  if( announcement_opp == ANNOUNCEMENT::NOANNOUNCEMENT)
    return ANNOUNCEMENT::NOANNOUNCEMENT;

  unsigned const opp_p = calcPointsOfOppositeTeam(hi);
  unsigned const own_p = calcPointsOfOwnTeam(hi);
  // The opposite team has not fulfilled its announcement -> make replay
  if (own_p >= point_limit_for_opposite_team(announcement_opp))
    return ANNOUNCEMENT::REPLY;
  // Still 30 points to make for the announcement -> no reply
  if (opp_p + 30 >= needed_points(announcement_opp))
    return ANNOUNCEMENT::NOANNOUNCEMENT;
  // no 0 -> no reply
  if (announcement_opp == ANNOUNCEMENT::NO0)
    return ANNOUNCEMENT::NOANNOUNCEMENT;

  if (hi.hand().numberoftrumps() - 1
      <= (hi.game().tricks_remaining_no() + 1) / 2 )
    return ANNOUNCEMENT::NOANNOUNCEMENT;

  {
    int value = 0;

    value = CalcHandValue( hi );


    if (   (hi.game().type() == GAMETYPE::MARRIAGE)
        && (hi.game().marriage_selector() == MARRIAGE_SELECTOR::TEAM_SET)
        && ( hi.team() != TEAM::RE ) )
    {
      value -=5; // previous 2
    }


    if (      (hi.team() == TEAM::RE) )
    {
      value -= 4; // previous 2
      if( GAMETYPE::is_solo(hi.game().type()) )
        value -= 3;
    }
    for( int a = (int)ANNOUNCEMENT::NO90;
        a <= announcement_opp;
        a++ )
      value += (int)hi.value(Aiconfig::ANNOUNCECONFIGREPLY);

    Trick t = g.trick_current();
    if (   hi.teamofplayer(t.winnerplayer()) != hi.team()
        ||  oppositeTeamCanWinTrick( t, hi ) )
      value -= 1;

    for( unsigned i = 0; i < hi.hand().cardsnumber(); i++ )
    {
      if(   hi.hand().card(i).isdolle()
         || hi.hand().card(i).isswine()
         || hi.hand().card(i).possible_swine()
         || hi.hand().card(i).ishyperswine()
         || hi.hand().card(i).possible_hyperswine() )
        value += (int)hi.value(Aiconfig::ANNOUNCECONFIGREPLY);
    } // for (i < hi.hand().cardsnumber())

    if (   (hi.game().type() == GAMETYPE::POVERTY)
        && (hi.player() == hi.game().poverty_partner()) )
    {
      value -= 6 - hi.hand().numberofdolle() - hi.hand().numberofclubqueens();
      if( g.announcement_of_team( opposite( hi.team() ) ).announcement
         != ANNOUNCEMENT::NOANNOUNCEMENT  )
      {
        value -= 5;
      }
    }

#ifdef ANNOUNCE_DEBUG
    if( !g.isvirtual() )
      cout << "REPLY: " << hi.player().name() << "\t" << value << "\t"
        << hi.value(Aiconfig::ANNOUNCELIMITREPLY) << std::endl;
#endif
    if( value >  static_cast<int>(hi.value(Aiconfig::ANNOUNCELIMITREPLY)) )
      return ANNOUNCEMENT::REPLY;
  }

  return ANNOUNCEMENT::NOANNOUNCEMENT;
} // Announcement Heuristics::make_reply( HeuristicInterface hi)


/**
 ** -> result
 **
 ** @param        old_card        best card till now
 ** @param        test_card        card to test
 ** @param        hi                hi (with the configuration)
 **
 ** @return        whether the card 'test_card' is to preferred to 'old_card'
 **                that is has more points
 **
 ** @author        Borg Enders
 **
 ** @version        0.6.9
 **/
bool
better_points_optimize(HandCard const& old_card,
                       HandCard const& test_card,
                       HeuristicInterface const& hi )
{
  return (  (  static_cast<int>(hi.game().trick_current_no())
             >= hi.value(Aiconfig::FIRST_TRICK_FOR_TRUMP_POINTS_OPTIMIZATION)
            )
          &&   ( old_card.value() < test_card.value() )
          || ( test_card.less( old_card ) && old_card.value() == test_card.value() )
         );
} // bool better_points_optimize(HandCard old_card, HandCard test_card, Ai hi)


/**
 ** -> result
 **
 ** @param     card               card to optimize
 ** @param     trick              current trick
 ** @param     hi                 heuristics interface
 ** @param     only_same_points   whether only cards with the same points should be checked (default: false)
 **
 ** @return    the given card or a lower one, if they are equal in the sense of jabbing
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.9
 **/
HandCard
Heuristics::optimized_card_to_play(Card const& c,
                                   Trick const& trick,
                                   HeuristicInterface const& hi,
                                   bool const only_same_points)
{
  if (c == Card::EMPTY)
    return HandCard::EMPTY;

  Hand const& hand = hi.hand();
  Game const& game = trick.game();
  Player const& player = hi.player();
  HandCard const card(hand, c);

  // do only optimize trump cards
  if (!card.istrump())
    return card;

  // do not optimize club queen in normal games
  if (   (game.type() == GAMETYPE::NORMAL)
      && (card == Card::CLUB_QUEEN)
      && (game.teaminfo(player) != TEAM::RE))
    return card;

  // optimize to a lower card, of there is no partner before the opposite team
  bool optimize_to_lower = false;
  // last or second last in trump
  if (trick.remainingcardno() <= 2)
    optimize_to_lower = true;
  // own team in the back
  if (hi.guessed_same_team(trick.lastplayer()))
    optimize_to_lower = true;
  // only opposite team behind
  if (checkAllOppositeTeam(trick, hi))
    optimize_to_lower = true;
  // I play the highest card in the game
  if (!hi.cards_information().higher_card_exists(card))
    optimize_to_lower = true;
  // in a solo: the soloplayer is following directly
  if (   (optimize_to_lower == false)
      && is_solo(game.type())
      && (hi.teamofplayer(game.player_following(trick.actplayer())) == TEAM::RE)
     )
    optimize_to_lower = true;

  if (optimize_to_lower) {
    Card lower_card = card;
    HandCards const opposite_cards
      = valid_cards_behind_of_opposite_team(trick, hi);
    bool const winnercard = trick.winnercard().less(card);

    for (Card c = card;
         c != Card::EMPTY;
         c = game.next_lower_card(c)) {
      if (   !card.is_special()
          && hand.contains(c)
          && HandCard(hi.hand(), c).is_special())
        continue;
      if (   only_same_points
          && c.value() != lower_card.value())
        break;
      // only optimize in jack/queen/king/special cards
      if (   (c.value() != Card::JACK)
          && (c.value() != Card::QUEEN)
          && !(   (game.type() == GAMETYPE::SOLO_KOEHLER)
               && (c.value() == Card::KING))
          && !HandCard(hi.hand(), c).is_special())
        break;
      if (   winnercard
          && !trick.winnercard().less(c))
        break;
      if (   opposite_cards.contains(c)
          && HandCard(hi.hand(), c).less(c)) {
        break;
      }
      if (hand.contains(c)) {
        // save a charlie
        if (!(   (c == Card::CHARLIE)
              && game.rule()(Rule::EXTRAPOINT_CHARLIE)
              && (game.type() == GAMETYPE::NORMAL) ) )
          lower_card = c;
      }
      if (opposite_cards.contains(c)) {
        // special case: club queen, but partner unknown
        if (!(   (game.type() == GAMETYPE::NORMAL)
              && (game.teaminfo(player) == TEAM::RE)
              && (c == Card::CLUB_QUEEN) ) )
          break;
      }
    } // for (c)

    return HandCard(hi.hand(), lower_card);
  } // if (optimize_to_lower)

  return card;
} // Card Heuristics::optimized_card_to_play(Card card, Trick trick, HeuristicInterface hi, bool only_same_points = false);

/**
 ** -> result
 **
 ** @param     trick     current trick
 ** @param     hi        heuristic interface
 **
 ** @return    the lowest card to serve the trick
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.9
 **/
Card
Heuristics::lowest_serving_card(Trick const& trick, HeuristicInterface const& hi)
{
  if (trick.isempty())
    return Card::EMPTY;

  Card card = lowest_card(trick, hi.hand());
  if (!card.is_empty())
    return card;

  if (hi.hand().contains(trick.startcard().tcolor())) {
    return hi.hand().lowest_card(trick.startcard().tcolor());
  } else {
    for (vector<Card::Color>::const_iterator
         c = hi.game().rule().card_colors().begin();
         c != hi.game().rule().card_colors().end();
         ++c) {
      Card::Color const& color = *c;
      if (hi.hand().contains(color)) {
        if (   card.is_empty()
            || (   (hi.hand().lowest_card(color).value()
                    < card.value())
                || (   (hi.hand().lowest_card(color).value()
                        == card.value())
                    && (hi.hand().numberof(color)
                        < hi.hand().numberof(card.color())) ) ) )
          card = hi.hand().lowest_card(color);
      }
    }
    if (!card.is_empty())
      return card;
  }

  card = hi.hand().lowest_trump();
  if (!card.is_empty())
    return card;

  return hi.hand().card(0);
} // Card Heuristics::lowest_serving_card(Trick trick, HeuristicInterface hi)

/**
 ** -> result
 **
 ** @param     trick     current trick
 ** @param     hi        heuristic interface
 **
 ** @return    a card to jab the trick
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.9
 **/
Card
Heuristics::jabbing_card(Trick const& trick, HeuristicInterface const& hi)
{
  if (trick.isempty())
    return Card::EMPTY;

  {
    Card const card = best_winning_card(trick, hi);
    if (!card.is_empty())
      return card;
  }

  {
    Card const card = hi.hand().next_higher_card(trick.winnercard());
    if (   !card.is_empty()
        && trick.isvalid(card, hi.hand()))
      return card;
  }

  return Card::EMPTY;
} // Card Heuristics::jabbing_card(Trick trick, HeuristicInterface hi)

