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

#include "team_information.heuristics.h"

#include "ai.h"
#include "team_information.h"
#include "cards_information.of_player.h"

#include "../../card/trick.h"
#include "../../party/rule.h"

// For configuration values search for '*Value*'

#ifndef RELEASE
#ifdef DKNOF
// the ai whose changes of the weightings are written
// (what the ai assumes of the other players)
// undefine for no output
#define DEBUG_AI 4
#endif
#endif

namespace TeamInformationHeuristic {

/* Further ideas
 *
 * - if the last player has a known team and he pfunds, than the winnerplayer
 *   could be in the same team
 *   (this assumes, that the last player knows more then I do)
 *
 * - opposite of 'choose pfund for partner'
 *   if a player has played his fox as startcard:
 *   o teams unknown: the player is re
 *   o teams known: the partner is the one with the highest card
 *
 */

// Return: > 0 for re, < 0 for contra
typedef int(*CardPlayedHeuristic)(HandCard const&, Trick const&, Ai const&);
#define CARD_PLAYED_HEURISTIC(name) \
int name(HandCard const& card, \
         Trick const& trick, \
         Ai const& ai)
// the startplayer in a trick
CARD_PLAYED_HEURISTIC(startplayer_color);
// the penultimate (second last) player has pfund
CARD_PLAYED_HEURISTIC(penultimate_pfund);
// the last player in a trick
CARD_PLAYED_HEURISTIC(last_player);

// overjabbed a player
CARD_PLAYED_HEURISTIC(overjabbed_player);

// the player does not have the color
CARD_PLAYED_HEURISTIC(does_not_have_color);
// the player has pfund in a color trick (with the same color)
CARD_PLAYED_HEURISTIC(pfund_in_color_trick);

// the player has started with a low color card in the first run of the color
CARD_PLAYED_HEURISTIC(started_with_low_color_first_run);
// the player has started with a low trump in the first trump trick
CARD_PLAYED_HEURISTIC(started_with_low_trump_first_run);
// the player has not jabbed the first trump run with the club queen
CARD_PLAYED_HEURISTIC(first_trump_run_no_jabbing_with_club_queen);

#undef CARD_PLAYED_HEURISTIC


// the value according to the team
static int team_value(int const value, Player const& player, Ai const& ai);

/**
 ** the player has played a card
 **
 ** @param    played_card    card that was played
 ** @param    trick          current trick
 ** @param    ai             the ai that analyses
 ** @param    weightings     the card weightings
 **
 ** @return    -
 **/
int card_played(HandCard const& played_card,
                Trick const& trick,
                Ai const& ai)
{
  // if it is the only card allowed, there is no information to get
  if (   (trick.actcardno() > 1)
      && (trick.startcard().tcolor() == played_card.tcolor())
      && !ai.cards_information().of_player(played_card.player()).estimated_hand().contains(trick.startcard().tcolor()))
    return 0;

  static const vector<pair<CardPlayedHeuristic, string> > heuristic_list
    = { {pair<CardPlayedHeuristic, string>(startplayer_color, "startplayer color")},
      {pair<CardPlayedHeuristic, string>(penultimate_pfund, "penultimate pfund")},
      {pair<CardPlayedHeuristic, string>(last_player, "last player")},
      {pair<CardPlayedHeuristic, string>(overjabbed_player, "overjabbed player")},
      {pair<CardPlayedHeuristic, string>(does_not_have_color, "does not have color")},
      {pair<CardPlayedHeuristic, string>(pfund_in_color_trick, "pfund in color trick")},
      {pair<CardPlayedHeuristic, string>(started_with_low_color_first_run, "started with low color first run")},
      {pair<CardPlayedHeuristic, string>(started_with_low_trump_first_run, "started with low trump first run")},
      {pair<CardPlayedHeuristic, string>(first_trump_run_no_jabbing_with_club_queen, "first trump run no jabbing with club queen")}
    };

  int value = 0;

  for (auto const& f : heuristic_list) {
    int const v = (*f.first)(played_card, trick, ai);
#ifdef DEBUG_AI
    if (   (ai.game().no(ai) == DEBUG_AI)
        && (v != 0) ) {
      cout << "team info(" << ai.game().no(ai) << ") " << ai.game().no(played_card.player()) << " - " << played_card << ": " << f.second << ": " << v << endl;
    }
#endif
    value += v;
  }

  return value;
} // void card_played(HandCard played_card, Trick trick, Ai ai)

/**
 ** the value according to the team
 ** RE:     +value
 ** CONTRA: -value
 ** else:   0
 **
 ** @param    value    value
 ** @param    player   player for the value
 ** @param    ai       ai of the team information
 **
 ** @return   the value according to the team of 'player'
 **
 ** @todo     other function name
 **/
int
team_value(int const value, Player const& player, Ai const& ai)
{
  if (value == 0)
    return 0;

  auto const team = ai.teaminfo(player);
  auto modifier = (TEAM::is_real(team)
                   ? 1.0
                   : min(1.0,
                         ai.team_information().certainty()
                         / (20.0 * 2000000)));
  if (   (player == ai)
      && !::is_real(ai.game().teaminfo(ai))
      && (  (ai.team() == TEAM::RE)
          ? (ai.team_information().team_value(ai)
             < (ai.value(Aiconfig::TRUSTING)
                ? 10 : 20) ) // *Value*
          : (ai.team_information().team_value(ai)
             > -(ai.value(Aiconfig::TRUSTING)
                 ? 10 : 20) ) // *Value*
         ) )
    modifier *= (0.2
                 + ((0.1 * player.game().trick_current_no())
                    / player.game().trickno())
                 + ((0.1 * player.game().trumps_played_no())
                    / player.game().trumps_no())); // *Value*

  switch (team) {
  case TEAM::MAYBE_RE:
    if (ai.team_information().team_value(player) < 5) // *Value*
      return 0;
  case TEAM::RE:
    return static_cast<int>(modifier * value);
  case TEAM::MAYBE_CONTRA:
    if (ai.team_information().team_value(player) > -5) // *Value*
      return 0;
  case TEAM::CONTRA:
    return static_cast<int>(-modifier * value);
  default:
    return 0;
  }
} // static int team_value(int value, Player player, Ai ai);

/**
 ** the startplayer in a trick
 ** if the startplayer plays a color which has already been played
 ** the team of the last player gets a value
 **
 ** @param    card    the card played by the player
 ** @param    trick   the current trick
 ** @param    ai      the ai that analyses
 **
 ** @return   value according to this heuristic
 **
 ** @todo     other function name
 **/
int
startplayer_color(HandCard const& card,
                  Trick const& trick,
                  Ai const& ai)
{
  /* Idea:
   * If a player starts with a color another player is known not to have,
   * it is likely the two players play in the same team.
   */

  // startplayer
  if (trick.startplayer() != card.player())
    return 0;

  // color trick
  if (card.istrump())
    return 0;

  if (ai.color_runs(card.color()) == 0) {

    // first color run and no ace: add some points for the second player
    if (card.value() != Card::ACE)
      return team_value(2, card.player().game().player_following(card.player()), ai); // *Value*

    return 0;
  } else { // if !(first run)

    // * not first color run: add some points for the last player who has
    //   jabbed the color so far
    for (auto p = &card.player().game().player_previous(card.player());
         p != &card.player();
         p = &card.player().game().player_previous(*p))
      if (ai.cards_information().of_player(*p).does_not_have(card.color()))
        return team_value(3, *p, ai); // *Value*
  } // if !(first run)

  return 0;
} // static int startplayer_color(...)

/**
 ** the penultimate (second last) player has pfund
 ** either the player has pfund for the player behind
 ** or the winnerplayer so far
 **
 ** @param    card    the card played by the player
 ** @param    trick   the current trick
 ** @param    ai      the ai that analyses
 **
 ** @return   value according to this heuristic
 **
 ** @todo     improve (use hand information of the last player)
 **/
int
penultimate_pfund(HandCard const& card,
                  Trick const& trick,
                  Ai const& ai)
{
  /* Idea:
   * The second last player pfunds.
   * If the winnercard so far is low, the last player is in the same team.
   * If the winnercard so far is high, the winnerplayer is in the same team.
   */

  // the player must have played the penultimate card
  if (trick.actcardno() != card.player().game().playerno() - 1)
    return 0;

  // the player must not be the winner
  if (trick.winnerplayer() == card.player())
    return 0;

  // the player must have pfund at least ten points
  if (card.points() < 10)
    return 0;

  // it must be a trump trick (but it need not be a trump pfund)
  // or it is a color trick but the player has not served the color
  if (!(   trick.startcard().istrump()
        || (card.tcolor() != trick.startcard().tcolor())))
    return 0;

  // if the winnercard is small assume that the last player jabs
  // else assume that the last player cannot jab
  // ToDo: check whether there are cards above the winner card
  // ToDo: check fox
  if (trick.winnercard().less(Card(Card::HEART, Card::QUEEN))) // *Value*
    return team_value(10, card.player().game().player_following(card.player()), ai); // *Value*
  else
    return team_value(10, trick.winnerplayer(), ai); // *Value*

  return 0;
} // static int penultimate_pfund(...)

/**
 ** the last player in a trick
 ** if the player jabs, the value is -winnerplayer
 ** if the player pfunds, the value is +winnerplayer
 **
 ** @param    card    the card played by the player
 ** @param    trick   the current trick
 ** @param    ai      the ai that analyses
 **
 ** @return   value according to this heuristic
 **
 ** @todo     check for special point
 **/
int
last_player(HandCard const& card,
            Trick const& trick,
            Ai const& ai)
{
  /* Idea:
   * If the last player in a trick jabs the trick, he does play against
   * the winnerplayer so far,
   * else if he even makes a pfund, he plays with the winnerplayer.
   * Giving a card for a special point gives a much higher credit.
   */

  // the player must have played the last card of the trick
  if (!trick.isfull())
    return 0;

  // it must not be a color trick
  // or the player must not have played the same color
  if (   (trick.startcard().tcolor() != Card::TRUMP)
      && (card.tcolor() == trick.startcard().tcolor()) )
    return 0;

  if (trick.winnerplayer() == card.player()) {
    // the player has jabbed, so check who was the winnerplayer before
    auto const& winnercard = trick.winnercard_before();

    // ToDo: check whether the player has jabbed in order to play
    //       the ace of a color

    // the value depends on the heigh of the card
    if (winnercard.value() == Card::JACK)
      return team_value(-15, winnercard.player(), ai); // *Value*
    if (winnercard.value() == Card::QUEEN)
      return team_value(-25, winnercard.player(), ai); // *Value*
    if (   (winnercard == Card::DOLLE)
        || winnercard.isswine() )
      return team_value(-35, winnercard.player(), ai); // *Value*

  } else { // if !(trick.winnerplayerno() == card.player().no())
    // the player has not jabbed
    // check whether the player could jab the trick
    // or if the player has even made a pfund
    // or, better, added a card for a special point

    // check whether the player has added a card for a special point
    // especially a fox is taken into account
    if (!trick.specialpoints().empty()) {
      int value = 0;
      Specialpointsvector const specialpoints = trick.specialpoints();
      for (auto const& sp : specialpoints) {
        if (sp.player_of_no == ai.game().no(card.player()))
          value += 100 * sp.value(); // *Value*
        else if (sp.type == SPECIALPOINT::DOPPELKOPF)
          value += 80; // *Value*
      }
      return team_value(value, trick.winnerplayer(), ai);
    } // if (!trick.specialpoints().empty())

    // check whether the player has made a pfund
    if (card.points() >= 10) {
      return team_value(card.points() // *Value*
                        + (card.isfox() ? 50 : 0) // *Value*
                        + static_cast<int>(trick.points()) / 4, // *Value*
                        trick.winnerplayer(), ai);
    } // if (trick.card(card.player()).value() >= 10)

    // check whether the player could jab the trick
    auto const limit_card = Card(Card::HEART, Card::QUEEN); // *Value*
    if (trick.winnercard().less(limit_card))
      return team_value(static_cast<int>(trick.points()) / 3, // *Value*
                        trick.winnerplayer(), ai);
  } // if !(trick.winnerplayerno() == card.player().no())

  // ToDo
  // * player jabs with card > club queen ==> contra

  return 0;
} // static int last_player(...)

/**
 ** the player has overjabbed a player
 **
 ** @param    card    the card played by the player
 ** @param    trick   the current trick
 ** @param    ai      the ai that analyses
 **
 ** @return   value according to this heuristic
 **/
int
overjabbed_player(HandCard const& card,
                  Trick const& trick,
                  Ai const& ai)
{
  /* Idea:
   * The player has overjabbed another player.
   * If the other player has played a high card, he is assumed to be of the opposite team.
   */

  // not the first player
  if (trick.actcardno() == 1)
    return 0;
  // the player has overjabbed
  if (trick.winnerplayer() != card.player())
    return 0;
  // the winnercard but without the last player
  auto const& winnercard = trick.winnercard_before();

  // the player has overjabbed a high card
  // ToDo: use right trumplimit according to the game type
  if (winnercard.less(ai.value(Aiconfig::TRUMPLIMIT_NORMAL)))
    return 0;

  return -team_value(trick.points() / 2 + card.value()
                     + (trick.remainingcardno() * 2), // *Value*
                     winnercard.player(), ai);
} // int overjabbed_player(...)

/**
 ** the player does not have the color
 ** when an ace is the winnercard: jab -> opposite, no jab -> same
 ** perhaps only for the first color run
 **
 ** @param    card    the card played by the player
 ** @param    trick   the current trick
 ** @param    ai      the ai that analyses
 **
 ** @return   value according to this heuristic
 **
 ** @todo     improve (check color run, search ToDo in code)
 **/
int
does_not_have_color(HandCard const& card,
                    Trick const& trick,
                    Ai const& ai)
{
  /* Idea:
   * A player does not have a color and the winnercard of the color is
   * an ace.
   * If he jabs, he does not play with the winnerplayer
   * (different behaviour for Fox, trump ten because the player may
   *  want to bring home the points)
   * If the playe does not jab, he plays with the winnerplayer.
   */

  // not the first player
  if (   (trick.actcardno() != 1)
      // a color trick
      && (trick.startcard().tcolor() != Card::TRUMP)
      // the player does not have the color
      && (card.tcolor() != trick.startcard().tcolor())
     ) {

    if (!is_real(trick.game().teaminfo(trick.winnerplayer())))
      return 0;

    // the winnercard but without the last player
    auto const& winnercard = trick.winnercard_before();

    // the winner card must be an ace
    if (winnercard.value() == Card::ACE) {

      if (card.istrump()) {
        // ToDo
        // The player could have jabbed in order to play a color ace
        // so check whether all colors have already been run
        // and check in the next trick whether the player does play a color ace
        if (card.isfox())
          return team_value(-11, winnercard.player(), ai);
        return team_value(trick.isfull()          
                          ? -2 * (35 - card.value())
                          : (trick.actcardno() == 3) 
                          ? -2 * (20 - card.value())
                          : -2 * (15 - card.value()),
                          winnercard.player(),
                          ai); // *Value*
      } else {
        return team_value(trick.points() / 2 + card.value()
                          + (trick.remainingcardno() * 2), // *Value*
                          winnercard.player(), ai);
      }
    } // if (conditions met)
  } // if (conditions met)

  return 0;
} // static int does_not_have_color(...)

/**
 ** the player pfunds in a color trick (in the same color)
 **
 ** @param    card    the card played by the player
 ** @param    trick   the current trick
 ** @param    ai      the ai that analyses
 **
 ** @return   value according to this heuristic
 **/
int
pfund_in_color_trick(HandCard const& card,
                     Trick const& trick,
                     Ai const& ai)
{
  /* Idea:
   * In the first run of a color trick:
   * If the player pfunds (p.e. ten), he plays with the winnerplayer
   * (t.i. the one who has played the ace),
   * else (p.e. nine) he plays against the player.
   *
   * Note: In order to return something != 0 the team of the winnerplayer
   * must be known. So the normal case will be that a player has announced
   * 're' and played a color ace.
   */

  // not the first player
  if (trick.actcardno() == 1)
    return 0;

  // a color trick
  if (trick.startcard().tcolor() == Card::TRUMP)
    return 0;

  // first run of the color trick or second run with enough remaining cards
  if (   (ai.color_runs(trick.startcard().tcolor()) > 1)
      || (ai.cards_information().remaining(trick.startcard().tcolor())
          < trick.remainingcardno()))
    return 0;

  // the player does not play a trump
  if (card.istrump())
    return 0;

  // search the winnercard but without the last player
  auto const& winnercard = trick.winnercard_before();

  // the winnercard must be an ace or a trump
  if (   (winnercard.value() != Card::ACE)
      && !winnercard.istrump() )
    return 0;

  // ToDo: search whether a player behind does not have the color

  // there are negative values because a small card shows that the player
  // does not wish to pfund
  // ToDo: test with nines
  if (card.tcolor() == trick.startcard().tcolor()) {
    if (ai.cards_information().of_player(card.player()).must_have_cards().lower_card_exists(card)) {
      static const map<Card::Value, int> value
        = { {Card::ACE, 50},
          {Card::TEN,   45},
          {Card::KING,  40},
          {Card::NINE,   0} }; // *Value*
      return team_value(value.find(card.value())->second,
                        winnercard.player(), ai);
    } else {
      static const map<Card::Value, int> value
        = { {Card::ACE, 9},
          {Card::TEN,   7},
          {Card::KING,  0},
          {Card::NINE, -5} }; // *Value*
      return team_value(value.find(card.value())->second,
                        winnercard.player(), ai);
    }
  } else { // if (other color)
      static const map<Card::Value, int> value
        = { {Card::ACE, 50},
          {Card::TEN,   45},
          {Card::KING,  30},
          {Card::NINE,  10} }; // *Value*
      return team_value(value.find(card.value())->second,
                        winnercard.player(), ai);
  }

  return 0;
} // static int pfund_in_color_trick(...)

/**
 ** the player starts with a low color in the first run
 **
 ** @param    card    the card played by the player
 ** @param    trick   the current trick
 ** @param    ai      the ai that analyses
 **
 ** @return   value according to this heuristic
 **/
int
started_with_low_color_first_run(HandCard const& card,
                                 Trick const& trick,
                                 Ai const& ai)
{
  /* Idea:
   * As startplayer: started with a non ace in a color trick, which has not run before.
   * This is normally for contra players.
   */

  // the first player
  if (trick.actcardno() != 1)
    return 0;

  // color trick
  if (trick.startcard().istrump())
    return 0;

  // first run of the color
  if (ai.color_runs(trick.startcard().tcolor()) != 0)
    return 0;

  // not played the ace
  if (card.value() != Card::ACE)
    return -10; // *Value*

  return 0;
} // int started_with_low_color_first_run(...)

/**
 ** the player starts with a low trump in the first run
 **
 ** @param    card    the card played by the player
 ** @param    trick   the current trick
 ** @param    ai      the ai that analyses
 **
 ** @return   value according to this heuristic
 **/
int
started_with_low_trump_first_run(HandCard const& card,
                                 Trick const& trick,
                                 Ai const& ai)
{
  /* Idea:
   * As startplayer: started with a small trump in a trump trick, which has not run before.
   * This is normally for contra players.
   */

  // the first player
  if (trick.actcardno() != 1)
    return 0;

  // trump trick
  if (!trick.startcard().istrump())
    return 0;

  // first trump trick
  if (ai.cards_information().trump_runs() > 0)
    return 0;

  { // not all color played so far
    auto c = ai.game().rule().card_colors().begin();
    for (;
         c != ai.game().rule().card_colors().end();
         ++c)
      if (ai.color_runs(*c) == 0)
        break;
    if (c == ai.game().rule().card_colors().end())
      return 0;
  }

  // small trump
  if (card.less(Card(Card::SPADE, Card::JACK)))
    return max(0, 10 - 2 * static_cast<int>(trick.game().trick_current_no())); // *Value*

  return 0;
} // int started_with_low_trump_first_run(...)

/**
 ** the player has not jabbed the first trump run with the club queen
 **
 ** @param    card    the card played by the player
 ** @param    trick   the current trick
 ** @param    ai      the ai that analyses
 **
 ** @return   value according to this heuristic
 **/
int
first_trump_run_no_jabbing_with_club_queen(HandCard const& card,
                                           Trick const& trick,
                                           Ai const& ai)
{
  /* Idea:
   * As second/third player in the first trump run: The player has not jabbed with the club queen.
   * Assume, he has a color ace, he then would jab in order to play a color ace.
   */

  // neither first nor last player
  if (   (trick.actcardno() == 1)
      || trick.isfull())
    return 0;

  // trump trick
  if (!trick.startcard().istrump())
    return 0;

  // first trump trick
  if (ai.cards_information().trump_runs() > 0)
    return 0;

  // winnercard before is below club queen
  if (!trick.before_last_played_card().winnercard().less(Card::CLUB_QUEEN))
    return 0;
  // the player has not played the highest card in the game
  if (trick.game().next_higher_card(trick.winnercard()) == Card::EMPTY)
    return 0;

  { // not all color have run
    bool all_colors = true;
    for (auto c : ai.game().rule().card_colors()) {
      if (   (c != ai.game().trumpcolor())
          && (ai.color_runs(c) == 0)) {
        all_colors = false;
        break;
      }
    }
    if (all_colors)
      return 0;
  } // not all color have run

  // contra
  return min(0, -6 + 1 * static_cast<int>(trick.game().trick_current_no())); // *Value*
} // int first_trump_run_no_jabbing_with_club_queen(...)

} // namespace TeamInformationHeuristic
