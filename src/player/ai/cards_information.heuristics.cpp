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

#include "cards_information.heuristics.h"

#include "cards_information.of_player.h"

#include "../../card/hand_card.h"
#include "../../card/trick.h"
#include "ai.h"
#include "team_information.h"

#include "../../game/game.h"
#include "../../party/rule.h"

// For configuration values search for '*Value*'
// -100 means: shall not have it (p.e. second fox, second queen, color ace when not played)
//  100 means: should have it    (p.e. second dolle)
//
// Info
// Ai::handofplayer() or CardsInformation::estimated_hand() should not be called because of logical (but not technical) recursion. But CardsInformation::possible_hand() can be used.

#ifndef RELEASE
#ifdef DKNOF
// the ai whose information are written
// (what the ai assumes of the other players)
// undefine for no output
#define DEBUG_AI 4
#endif
#endif


#ifdef DEBUG_AI
#define CHANGE_WEIGHTING(card, value) \
  (((ai.game().no(ai) == DEBUG_AI) && !ai.game().isvirtual()) \
   ? (cout << "weighting change: " << ai.game().no(played_card.player()) << ": " << setw(13) << card << ": " << setw(4) << value << "   (" << heuristic_name << ")\n") \
   : cout), \
(weightings[card] += value)
#else
#define CHANGE_WEIGHTING(card, value) \
  (void)heuristic_name, \
weightings[card] += value
#endif

namespace CardsInformationHeuristics {

typedef void(*CardPlayedHeuristic)(HandCard const&, Trick const&, Ai const&, map<Card, int>&);

#define CARD_PLAYED_HEURISTIC(name) \
void name(HandCard const& played_card, \
          Trick const& trick, \
          Ai const& ai, \
          map<Card, int>& weightings)

// not started with a color ace
CARD_PLAYED_HEURISTIC(no_color_ace);
// a color trick has been served
CARD_PLAYED_HEURISTIC(served_color);
// a color trick has been jabbed (first run)
CARD_PLAYED_HEURISTIC(color_trick_jabbed_first_run);
// the last player has played a pfund
CARD_PLAYED_HEURISTIC(last_player_played_pfund);

// the last player in a trump trick (player has jabbed)
CARD_PLAYED_HEURISTIC(last_player_jabbed_trump);
// in a trump trick (player has not jabbed)
CARD_PLAYED_HEURISTIC(not_jabbed_trump);
// the player has served a trump trick
CARD_PLAYED_HEURISTIC(served_trump);

// the player has thrown a trump in a color trick
CARD_PLAYED_HEURISTIC(threw_trump_in_color_trick);

// the first player has played a pfund
CARD_PLAYED_HEURISTIC(first_player_played_pfund);
// the first player has played a club queen
CARD_PLAYED_HEURISTIC(first_player_played_club_queen);

// the bride in the marriage plays a pfund in the decision trick
CARD_PLAYED_HEURISTIC(bride_pfunds_in_decision_trick);

// the player has no second diamond ace because he has not announced swines
CARD_PLAYED_HEURISTIC(no_swines);
// assumption: no silent marriage
CARD_PLAYED_HEURISTIC(no_silent_marriage);

// the player will start with a dolle, if he has the second one
CARD_PLAYED_HEURISTIC(both_dollen);

// the re player in a poverty starts with a trump
CARD_PLAYED_HEURISTIC(poverty_re_starts_with_trump);

// the soloplayer will probably start with a ten when he has both aces
CARD_PLAYED_HEURISTIC(soloplayer_startcard_color);
// the soloplayer will probably start with a ten when he has both aces
CARD_PLAYED_HEURISTIC(soloplayer_startcard_color);

// the re player has played the second best trump in a picture solo
CARD_PLAYED_HEURISTIC(picture_solo_re_played_second_best_trump);
// a contra player has played a trump in a picture solo
CARD_PLAYED_HEURISTIC(picture_solo_contra_played_trump);

#undef CARD_PLAYED_HEURISTIC


/* Further ideas
 *
 * - opposite of 'choose pfund for partner'
 *   if a player has played his fox as startcard:
 *   o teams unknown: the player has a club queen and all higher cards
 * - club queen played
 *   o no second club queen
 */


/**
 ** the player has accepted a poverty
 ** we will most surly have no color
 **
 ** @param    player       the player who has accepted the poverty
 ** @param    ai           the ai that analyses
 ** @param    weightings   the card weightings
 ** @param    trumpno      the number of returnes trumps
 **
 ** @return    -
 **
 ** @todo      use the information of the returned cards
 **            (if trump, than there is a better chance)
 **/
void poverty_returned(Player const& player,
                      Ai const& ai,
                      map<Card, int>& weightings,
                      unsigned const trumpno)
{
  /* Idea
   *
   * the player who accepts the poverty will have only trump
   */

  // the name of the heuristic
  auto const heuristic_name = "poverty returned";

  // the game
  auto const& game = player.game();

  for (auto const& c : game.rule().cards()) {
    if (!c.istrump(game)) {
      HandCard const played_card(player.hand(), c);
      CHANGE_WEIGHTING(c, -80 - 40 * static_cast<int>(trumpno)); // *Value*
    }
  } // for (c : cards)
} // void poverty_returned(Player player, Ai ai, map<Card, int>& weightings, unsigned trumpno)

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
void card_played(HandCard const& played_card,
                 Trick const& trick,
                 Ai const& ai,
                 map<Card, int>& weightings)
{
  static vector<CardPlayedHeuristic> heuristic_list;
  //vector<void (*)(HandCard const&, Trick const&, Ai const&, map<Card, int>&) >
  //heuristic_list;

  if (heuristic_list.empty()) {
    heuristic_list.push_back(no_color_ace);
    heuristic_list.push_back(served_color);
    heuristic_list.push_back(color_trick_jabbed_first_run);
    heuristic_list.push_back(last_player_played_pfund);
    heuristic_list.push_back(last_player_jabbed_trump);
    heuristic_list.push_back(not_jabbed_trump);
    heuristic_list.push_back(served_trump);
    heuristic_list.push_back(threw_trump_in_color_trick);
    heuristic_list.push_back(first_player_played_pfund);
    heuristic_list.push_back(first_player_played_club_queen);
    heuristic_list.push_back(bride_pfunds_in_decision_trick);
    heuristic_list.push_back(no_swines);
    heuristic_list.push_back(no_silent_marriage);
    heuristic_list.push_back(both_dollen);
    heuristic_list.push_back(poverty_re_starts_with_trump);
    heuristic_list.push_back(soloplayer_startcard_color);
    heuristic_list.push_back(picture_solo_re_played_second_best_trump);
    heuristic_list.push_back(picture_solo_contra_played_trump);
  } // if (heuristic_list.empty())

  for (auto const& f : heuristic_list)
    (*f)(played_card, trick, ai, weightings);
} // void card_played(HandCard const& played_card, Trick const& trick, Ai const& ai, map<Card, int>& weightings)

/**
 ** a player has not started with a color ace
 ** If the player is startplayer and has not started with a color ace,
 ** it is assumed that he does not have one.
 ** Problem: He has so many cards of the color so that he knows he cannot
 **          get the trick
 **
 ** @param     played_card    the card played by the player
 ** @param     trick          the current trick
 ** @param     ai             the ai that analyses
 ** @param     weightings     the card weightings
 **
 ** @return    -
 **
 ** @todo      marriage
 **/
void
no_color_ace(HandCard const& played_card,
             Trick const& trick,
             Ai const& ai,
             map<Card, int>& weightings)
{
  /* Idea
   *
   * A player would start with a color ace if he has one.
   * And in the first run of a color he would play his ace.
   */

  // the name of the heuristic
  auto const heuristic_name = "no color ace";

  auto const& player = played_card.player();
  auto const& game = trick.game();

  // a normal game or a marriage
  if (!(   (game.type() == GAMETYPE::NORMAL)
        || (game.type() == GAMETYPE::GENSCHER)
        || (game.type() == GAMETYPE::MARRIAGE)))
    return ;

  // startplayer
  if (trick.startplayer() == player) {

    // In a marriage not the solo player,
    // if he determines the marriage with his card.
    if (   (game.type() == GAMETYPE::MARRIAGE)
        && (player == game.soloplayer())
        && is_selector(played_card.tcolor(),
                       game.marriage_selector()) )
      return ;

    // has not played a color ace
    if (played_card.value() == Card::ACE)
      return ;

    // If it is the solo player, he will have the color ace, if he played a ten
    if (   game.is_solo()
        && (player == game.soloplayer())
        && (played_card.value() == Card::TEN)) {
      CHANGE_WEIGHTING(Card(played_card.color(), Card::ACE), 50); // *Value*
      return ;
    }

    // For all colors which have not run, yet, the player is assumed to not
    // have the ace.
    // ToDo: check also, that noone has thrown the color
    for (auto const& c : ai.game().rule().card_colors())
      if (!Card(c, Card::ACE).istrump(game))
        if (!ai.cards_information().played(Card(c, Card::ACE)))
          CHANGE_WEIGHTING(Card(c, Card::ACE),
                           -50 * static_cast<int>(game.cards_no(c)
                                                  - (game.rule()(Rule::NUMBER_OF_SAME_CARDS))
                                                 )); // *Value*
  } else { // if !(startplayer)
    // color trick
    if (trick.startcard().istrump())
      return ;

    auto const color = trick.startcard().tcolor();

    // first color run
    if (!(ai.cards_information().color_runs(color) == 0))
      return ;

    // player has not played an ace
    if (played_card.value() == Card::ACE)
      return ;

    // winner card is no ace and no trump
    if (   (trick.winnercard().value() == Card::ACE)
        || trick.winnercard().istrump())
      return ;

    CHANGE_WEIGHTING(Card(color, Card::ACE), -100); // *Value*
  } // if !(startplayer
} // void no_color_ace(...)

/**
 ** a player has served a color trick
 ** if the winnerplayer is not of the own team and there is no player in
 ** the own team, the player does have no card below the played one
 **
 ** @param     played_card    the card played by the player
 ** @param     trick          the current trick
 ** @param     ai             the ai that analyses
 ** @param     weightings     the card weightings
 **
 ** @return    -
 **
 ** @todo      other gametypes
 **/
void
served_color(HandCard const& played_card,
             Trick const& trick,
             Ai const& ai,
             map<Card, int>& weightings)
{
  /* Idea:
   * If no team is known, the player plays the lowest card in a color trick
   */

  // the name of the heuristic
  auto const heuristic_name = "served color";

  auto const& player = played_card.player();
  auto const& game = trick.game();

  // not startplayer
  if (trick.startplayer() == player)
    return ;

  // color trick
  if (trick.startcard().istrump())
    return ;

  // color trick served
  if (played_card.tcolor() != trick.startcard().tcolor())
    return ;

  // it is not the winnerplayer
  if (trick.winnerplayer() == player)
    return ;

  // the winnerplayer is not of the team as the player
  // or a higher card (an ace) then the winnercard exists
  // note: 'TeamInfo' makes the opposite assumption:
  //       a high card played is a sign for the same team
  if (   (ai.teaminfo(trick.winnerplayer()) != ::TEAM::UNKNOWN)
      && (maybe_to_team(ai.teaminfo(trick.winnerplayer()))
          == maybe_to_team(ai.teaminfo(player))) 
      && (   trick.winnercard().istrump()
          || (trick.winnercard() == Card(played_card.tcolor(), Card::ACE))
          || (ai.cards_information().played(played_card.tcolor(),
                                            Card::ACE)
              < game.rule()(Rule::NUMBER_OF_SAME_CARDS)) )
     )
    return ;

  // special case: undetermined marriage
  if (   game.is_undetermined_marriage()
      && (player == game.soloplayer()) )
    return ;

  // in a poverty: this is the poverty player
  // and the soloplayer is behind
  if (   (game.type() == GAMETYPE::POVERTY)
      && (player == game.soloplayer())
      && !trick.has_played(game.poverty_partner())) {
    return ;
  }

  // no team of the players behind is known to be of the own team
  for (unsigned c = game.playerno() - 1; c > trick.actcardno(); --c)
    if (   (ai.team_information().guessed_team(trick.player_of_card(c))
            == ai.team_information().guessed_team(player))
        && ai.handofplayer(trick.player_of_card(c)).can_jab(trick))
      return ;

  switch (game.type()) {
  case GAMETYPE::NORMAL:
  case GAMETYPE::POVERTY:
  case GAMETYPE::GENSCHER:
  case GAMETYPE::MARRIAGE:
  case GAMETYPE::MARRIAGE_SOLO:
  case GAMETYPE::MARRIAGE_SILENT:
  case GAMETYPE::SOLO_CLUB:
  case GAMETYPE::SOLO_HEART:
  case GAMETYPE::SOLO_SPADE:
  case GAMETYPE::SOLO_DIAMOND:
    switch (played_card.value()) {
    case Card::ACE:
      // add value: 80 + 10 * point difference
      if (!Card(played_card.color(), Card::TEN).istrump(game))
        CHANGE_WEIGHTING(Card(played_card.color(), Card::TEN), -90); // *Value*
      if (!Card(played_card.color(), Card::KING).istrump(game))
        CHANGE_WEIGHTING(Card(played_card.color(), Card::KING), -150); // *Value*
      if (game.rule()(Rule::WITH_NINES))
        if (!Card(played_card.color(), Card::NINE).istrump(game))
          CHANGE_WEIGHTING(Card(played_card.color(), Card::NINE), -190); // *Value*
      break;
    case Card::TEN:
      // add value: 40 + 10 * point difference
      if (!Card(played_card.color(), Card::KING).istrump(game))
        CHANGE_WEIGHTING(Card(played_card.color(), Card::KING), -140); // *Value*
      if (game.rule()(Rule::WITH_NINES))
        if (!Card(played_card.color(), Card::NINE).istrump(game))
          CHANGE_WEIGHTING(Card(played_card.color(), Card::NINE), -140); // *Value*
      break;
    case Card::KING:
      // add value: 40 + 10 * point difference
      if (game.rule()(Rule::WITH_NINES))
        if (!Card(played_card.color(), Card::NINE).istrump(game))
          CHANGE_WEIGHTING(Card(played_card.color(), Card::NINE), -80); // *Value*
      break;
    default:
      break;
    } // switch (played_card.value())
    break;

  default:
    // ToDo
    break;
  } // switch (game.type())
} // void served_color(...)

/**
 ** a player has jabbed a color trick in the first run
 ** The order of jabbing would be fox, trump ten, trump king, jacks, ...
 **
 ** @param     played_card    the card played by the player
 ** @param     trick          the current trick
 ** @param     ai             the ai that analyses
 ** @param     weightings     the card weightings
 **
 ** @return    -
 **
 ** @todo      check, that no other player has jabbed
 ** @todo      other order for heart (without nines)
 **/
void
color_trick_jabbed_first_run(HandCard const& played_card,
                             Trick const& trick,
                             Ai const& ai,
                             map<Card, int>& weightings)
{
  /* Idea:
   * If the first color run is jabbed, the player will take low trump.
   */

  // the name of the heuristic
  auto const heuristic_name = "color trick jabbed first run";

  auto const& player = played_card.player();
  auto const& game = trick.game();

  auto const color = trick.startcard().color();

  // normal game
  if (   GAMETYPE::is_solo(game.type())
      || (game.type() == GAMETYPE::POVERTY))
    return ;

  // not startplayer
  if (trick.startplayer() == player)
    return ;

  // color trick
  if (trick.startcard().istrump())
    return ;

  // first run or last card
  if (   (ai.cards_information().color_runs(color) > 0)
      && !trick.isfull())
    return ;

  // no (following) player has played/thrown the color
  for (unsigned c = trick.actcardno() + 1; c < game.playerno(); ++c)
    if (ai.cards_information().of_player(trick.player_of_card(c)).played(color))
      return ;

  // color trick jabbed
  if (!played_card.istrump())
    return ;

  // the cards in the order to play
  vector<pair<Card, int> > cards; // *Value*
  if (!game.swines_announced())
    cards.push_back(std::make_pair(Card(game.trumpcolor(), Card::ACE), -200));
  if (!(   (game.trumpcolor() == Card::HEART)
        && game.rule()(Rule::DOLLEN)))
    cards.push_back(std::make_pair(Card(game.trumpcolor(), Card::TEN), -100));
  if (!(   !game.rule()(Rule::WITH_NINES)
        && game.hyperswines_announced()) )
    cards.push_back(std::make_pair(Card(game.trumpcolor(), Card::KING), -90));
  cards.push_back(std::make_pair(Card::DIAMOND_JACK, -60));
  cards.push_back(std::make_pair(Card::HEART_JACK,   -60));
  cards.push_back(std::make_pair(Card::SPADE_JACK,   -60));
  cards.push_back(std::make_pair(Card::CLUB_JACK,    -40));
  if (   game.rule()(Rule::WITH_NINES)
      && !game.hyperswines_announced())
    cards.push_back(std::make_pair(Card(game.trumpcolor(), Card::NINE), -50));
  cards.push_back(std::make_pair(Card::DIAMOND_QUEEN, -50));
  cards.push_back(std::make_pair(Card::HEART_QUEEN,   -50));
  cards.push_back(std::make_pair(Card::SPADE_QUEEN,   -50));

  // search the former winnercard
  Trick former_trick(trick.startplayer());
  for (unsigned c = 0; c < trick.actcardno() - 1; ++c)
    former_trick += trick.card(c);

  // the former winnercard
  auto const& former_winnercard = former_trick.winnercard();

  // the weighting
  double weighting = 1;
  // there must be at least one more card remaining
  switch (ai.cards_information().remaining(color)
          - (trick.remainingcardno() - 1)) {
  case 0:
  case 1:
    weighting = 0;
    break;
  case 2:
    switch (trick.remainingcardno()) {
    case 0:
      weighting = 1;
      break;
    case 1:
      weighting = 0.9;
      break;
    case 2:
      weighting = 0.7;
      break;
    default:
      weighting = 1;
      break;
    } // switch (trick.remainingcardno())
    break;
  default:
    weighting = 1;
    break;
  } // switch(free color cards)

  for (auto const& card : cards) {
    if (played_card == card.first)
      break;
    else if (former_winnercard.less(card.first))
      CHANGE_WEIGHTING(card.first,
                       static_cast<int>(weighting * card.second));
  }
} // void color_trick_jabbed_first_run(...)

/**
 ** The last player has played a pfund.
 ** He will have no better pfund or he will have played a color blank
 **
 ** @param     played_card    the card played by the player
 ** @param     trick          the current trick
 ** @param     ai             the ai that analyses
 ** @param     weightings     the card weightings
 **
 ** @return    -
 **
 ** @todo      trump
 **/
void
last_player_played_pfund(HandCard const& played_card,
                         Trick const& trick,
                         Ai const& ai,
                         map<Card, int>& weightings)
{
  /* Idea:
   * The last player will have played his best pfund if the trick goes to his partner
   */

  // the name of the heuristic
  auto const heuristic_name = "last player played pfund";

  auto const& player = played_card.player();
  auto const& game = trick.game();

  // last player
  if (!trick.isfull())
    return ;

  // the last player is not the winnerplayer
  if (trick.winnerplayer() == trick.lastplayer())
    return ;

  // winnerplayer is of the team as the last player
  if (   (ai.teaminfo(player) == TEAM::UNKNOWN)
      || game.is_undetermined_marriage()
      || (ai.team_information().guessed_team(player)
          != ai.team_information().guessed_team(trick.winnerplayer())))
    return ;

  if (played_card.istrump()) {
    if (game.trumpcolor() != Card::NOCARDCOLOR) {
      if (played_card != Card(game.trumpcolor(), Card::ACE)) {
        if (!HandCard(ai.hand(), game.trumpcolor(), Card::ACE).is_special())
          CHANGE_WEIGHTING(Card(game.trumpcolor(), Card::ACE), -100); // *Value*
        if (played_card != Card(game.trumpcolor(), Card::TEN)) {
          if (!HandCard(ai.hand(), game.trumpcolor(), Card::TEN).is_special())
            CHANGE_WEIGHTING(Card(game.trumpcolor(), Card::TEN), -90); // *Value*
        } // if (played_card != TEN)
      } // if (played_card != ACE)
    } // if (game.trumpcolor())
  } else { // if !(played_card.tcolor() == Card::TRUMP)
    for (auto v : game.rule().card_values()) {
      if (   (v > played_card.value())
          && !Card(played_card.color(), v).istrump(game))
        CHANGE_WEIGHTING(Card(played_card.color(), v), -100); // *Value*
    }
  } // if !(played_card.tcolor() == Card::TRUMP)
} // void last_player_played_pfund(...)

/**
 ** the player has jabbed the trick as last player
 ** He will not have the cards between the highest card before
 ** and his played card
 **
 ** @param     played_card    the card played by the player
 ** @param     trick          the current trick
 ** @param     ai             the ai that analyses
 ** @param     weightings     the card weightings
 **
 ** @return    -
 **/
void
last_player_jabbed_trump(HandCard const& played_card,
                         Trick const& trick,
                         Ai const& ai,
                         map<Card, int>& weightings)
{
  /* Idea:
   * If the player has jabbed a trick,
   * he will not have a cards between the former winnercard and his card.
   */

  // the name of the heuristic
  auto const heuristic_name = "last player jabbed trump";

  auto const& player = played_card.player();

  // last player in the trick
  if (!trick.isfull())
    return ;

  // winnerplayer is the player
  if (!(trick.winnerplayer() == player))
    return ;

  // the player does not show himself with a club queen
  if (   (played_card == Card::CLUB_QUEEN)
      && (player.game().type() == GAMETYPE::NORMAL)
      && (player.announcement() == ANNOUNCEMENT::NOANNOUNCEMENT) )
    return ;

  // search the former winnercard
  Trick former_trick(trick.startplayer());
  for (unsigned c = 0; c < trick.actcardno() - 1; ++c)
    former_trick += trick.card(c);

  // the former winnercard
  auto const& former_winnercard = former_trick.winnercard();

  // the former winnercard is a trump card
  if (!former_winnercard.istrump())
    return ;

  // the cards between
  auto const cards_between
    = trick.game().cards_between(former_winnercard, played_card);

  for (auto const& c : cards_between)
    CHANGE_WEIGHTING(c, -40); // *Value*
} // void last_player_jabbed_trump(...)

/**
 ** the player has not jabbed
 ** If there are many points, he will not have a card higher
 ** than the winnercard
 **
 ** @param     played_card    the card played by the player
 ** @param     trick          the current trick
 ** @param     ai             the ai that analyses
 ** @param     weightings     the card weightings
 **
 ** @return    -
 **/
void
not_jabbed_trump(HandCard const& played_card,
                 Trick const& trick,
                 Ai const& ai,
                 map<Card, int>& weightings)
{
  /* Idea:
   * If the player has not jabbed a trick with many points,
   * he will not have a card higher than the winnercard.
   */

  // the name of the heuristic
  auto const heuristic_name = "not jabbed trump";

  // the player has not jabbed
  if (trick.winnerplayer() == played_card.player())
    return ;

  // last player: team not totally unknown
  if (   trick.isfull()
      && (ai.teaminfo(played_card.player()) == TEAM::UNKNOWN) )
    return ;

  // trick does not go to the team of the player
  if (  (trick.winnerteam() != TEAM::UNKNOWN)
      && (::maybe_to_team(trick.winnerteam())
          == ::maybe_to_team(ai.teaminfo(played_card.player())) ) )
    return ;

  // not: only partners behind, one has a higher card
  if (ai.teaminfo(played_card.player()) != TEAM::UNKNOWN) {
    Team const& team
      = ai.team_information().guessed_team(played_card.player());
    bool let_partner = false;
    for (unsigned c = trick.actcardno() + 1; c < trick.game().playerno(); ++c) {
      Player const& p = trick.player_of_card(c);
      if (ai.team_information().guessed_team(p) != team) {
        let_partner = false;
        break;
      }
      if (ai.cards_information().possible_hand(p).higher_card_exists(trick.winnercard()))
        let_partner = true;
    } // for (c)

    if (let_partner)
      return ;
  } // if (ai.teaminfo(played_card.player()) != TEAM::UNKNOWN)

  // not poverty, contra player, partner is behind, partner can have higher card
  if (   (trick.game().type() == GAMETYPE::POVERTY)
      && (played_card.player().team() == TEAM::CONTRA)
      && !trick.has_played(*ai.partner(played_card.player()))
      && ai.cards_information().possible_hand(*ai.partner(played_card.player())).higher_card_exists(trick.winnercard()))
    return ;

  // enough points in the trick
  if (!(trick.points() + 2 * trick.remainingcardno() - played_card.value()
        >= 10)) // *Value*
    return ;

  // the player has not played a pfund
  if (played_card.value() >= 10)
    return ;

  // trump trick
  if (!trick.startcard().istrump())
    return ;

  // winnercard small enough
  if (trick.isfull()) {
    // always take the trick (10 points)
  } else if (trick.islastcard()) {
    // take the trick (6 points), but not with a very high trump
    if (ai.trump_card_limit().less(trick.winnercard()))

      return ;
  } else {
    // take the trick (10 points), but not with a high trump
    if (!trick.winnercard().less(ai.trump_card_limit()))
      return ;
  }

  // the higher cards
  auto const higher_cards
    = trick.game().higher_cards(trick.winnercard());
  // the modification value
  auto value = -((static_cast<int>(trick.points()
                                  + 5 * trick.remainingcardno())
                 - 6) * 10); // *Value*
  value += -150 * trick.specialpoints().size();

  // ToDo: use 'TrickWeighting' for the value (if > 0)
  for (auto const& c : higher_cards)
    CHANGE_WEIGHTING(c, value);
} // void not_jabbed_trump(...)

/**
 ** the player has served
 ** He will not have a lower card with equal or less points.
 **
 ** @param     played_card    the card played by the player
 ** @param     trick          the current trick
 ** @param     ai             the ai that analyses
 ** @param     weightings     the card weightings
 **
 ** @return    -
 **/
void
served_trump(HandCard const& played_card,
             Trick const& trick,
             Ai const& ai,
             map<Card, int>& weightings)
{
  /* Idea:
   * If the player has served a trump trick,
   * he will have played his lowest trump.
   */

  // the name of the heuristic
  auto const heuristic_name = "served trump";

  // not the first player in the trick
  if (trick.actcardno() == 1)
    return ;

  // the player has not jabbed
  if (trick.winnerplayer() == played_card.player())
    return ;

  // whether the trick goes to the team of the player
  if (   (trick.winnerteam() != TEAM::UNKNOWN)
      && (::maybe_to_team(trick.winnerteam())
          == ai.teaminfo(played_card.player()) ) )
    return ;

  // trump trick
  if (!trick.startcard().istrump())
    return ;

  // the lower cards
  auto const lower_cards = trick.game().lower_trumps(played_card);

  // ToDo: use 'TrickWeighting' for the value (if > 0)
  for (auto const& c : lower_cards) {
    if (   (c.value() < played_card.value())
        && (   (c.value() < 10)
            || trick.isfull() ) )
      CHANGE_WEIGHTING(c,
                       -3 * static_cast<int>(c.value()) // *Value*
                       - (HandCard(played_card.hand(), c).isfox()
                          ? 100 : 0)); // *Value*
  } // for (c : lower_cards)
} // void served_trump(...)

/**
 ** the player has thrown a trump
 **
 ** @param     played_card    the card played by the player
 ** @param     trick          the current trick
 ** @param     ai             the ai that analyses
 ** @param     weightings     the card weightings
 **
 ** @return    -
 **/
void
threw_trump_in_color_trick(HandCard const& played_card,
                           Trick const& trick,
                           Ai const& ai,
                           map<Card, int>& weightings)
{
  /* Idea:
   * If the player has thrown a trump in a color trick without jabbing,
   * he will not have a color card of such a value.
   */

  // the name of the heuristic
  auto const heuristic_name = "threw trump in color trick";

  auto const& player = played_card.player();
  auto const& game = trick.game();

  // a color trick
  if (trick.startcard().istrump())
    return ;
  // player has played a trump
  if (!played_card.istrump())
    return ;
  // the player has not jabbed
  if (trick.winnercardno() == trick.actcardno() - 1)
    return ;

  { // first: the player will not have a color card with the same value
    for (auto const c : ai.game().rule().card_colors()) {
      if (!Card(c, played_card.value()).istrump(game))
        CHANGE_WEIGHTING(Card(c, played_card.value()),
                         -120); // *Value*
    }
  } // first: the player will not have a color card with the same value

  // now check whether the trick goes the the same team as the player

  // last player: the player is not of the same team as the winnerplayer
  if (   trick.isfull()
      && (ai.teaminfo(player) != TEAM::UNKNOWN)
      && (ai.teaminfo(player)
          == ai.teaminfo(trick.winnercard().player()) ) )
    return ;

  // third player: the player is not of the same team as the winnerplayer
  // ToDo: check, that the last player cannot overjab
  if (   trick.islastcard()
      && (ai.teaminfo(player) != TEAM::UNKNOWN)
      && (ai.teaminfo(player)
          == ai.teaminfo(trick.winnercard().player()) )
     )
    return ;

  { // second: the player will not have a color card with a smaller value
    for (auto const& c : game.rule().cards()) {
      if (   (c.value() < played_card.value())
          && !c.istrump(game)) {
        CHANGE_WEIGHTING(c, -100); // *Value*
      }
    } // for (c : cards)
  } // second: the player will not have a color card with a smaller value
} // void threw_trump_in_color_trick(...)

/**
 ** the first player has played a pfund
 **
 ** @param     played_card    the card played by the player
 ** @param     trick          the current trick
 ** @param     ai             the ai that analyses
 ** @param     weightings     the card weightings
 **
 ** @return    -
 **/
void
first_player_played_pfund(HandCard const& played_card,
                          Trick const& trick,
                          Ai const& ai,
                          map<Card, int>& weightings)
{
  /* Idea:
   * If the first player has played a pfund, his partner will have the
   * highest card.
   * If there is a club queen remaining and there are no swines/hyperswines,
   * the first player will be re and he has all cards over the club queen.
   */

  // the name of the heuristic
  auto const heuristic_name = "first player played pfund";

  auto const& player = played_card.player();
  auto const& game = trick.game();

  // normal game or marriage
  if (!(   (game.type() == GAMETYPE::NORMAL)
        || (game.type() == GAMETYPE::MARRIAGE) ) )
    return ;

  // the first player in the trick
  if (trick.actcardno() != 1)
    return ;

  // the first player is not contra
  if (ai.teaminfo(player) == TEAM::CONTRA)
    return ;

  // he has played a trump pfund
  if (!(   (played_card.value() >= 10)
        && !game.is_special(played_card)
        && played_card.istrump()) )
    return ;

  // another player has swines / hyperswines
  if (   game.hyperswines_announced()
      && (*game.hyperswines_owner() != player) )
    return ;
  if (   game.swines_announced()
      && (*game.swines_owner() != player) )
    return ;

  // there is a club queen remaining
  if (ai.cards_information().played(Card::CLUB_QUEEN)
      + ai.cards_information().of_player(ai).must_have(Card::CLUB_QUEEN)
      == game.rule()(Rule::NUMBER_OF_SAME_CARDS))
    return ;

  // if a club queen is played, it must be from the player himself
  if (ai.cards_information().of_player(player).played(Card::CLUB_QUEEN)
      < ai.cards_information().played(Card::CLUB_QUEEN))
    return ;

  // the player has all higher trumps from the club queen ...
  auto const higher_trumps
    = trick.game().higher_cards(Card(Card::SPADE, Card::QUEEN));

  // ... if the ai does not have it
  for (auto const& c : higher_trumps) {
    // ToDo: check for announced swines/hyperswines, ...
    if (   (c != Card::CLUB_QUEEN)
        && (ai.hand().contains(c)) )
      return ;
  }

  for (auto const& c : higher_trumps)
    CHANGE_WEIGHTING(c, 1000); // *Value*
} // void first_player_played_pfund(...)

/**
 ** the first player has played a club queen
 **
 ** @param     played_card    the card played by the player
 ** @param     trick          the current trick
 ** @param     ai             the ai that analyses
 ** @param     weightings     the card weightings
 **
 ** @return    -
 **/
void
first_player_played_club_queen(HandCard const& played_card,
                               Trick const& trick,
                               Ai const& ai,
                               map<Card, int>& weightings)
{
  /* Idea:
   * If the first player has played a club queen, he will have all higher
   * cards and -- if the other club queen has not been played -- no trump
   * pfund.
   */

  // the name of the heuristic
  auto const heuristic_name = "first player played club queen";

  auto const& player = played_card.player();
  auto const& game = trick.game();

  // normal game
  if (game.type() != GAMETYPE::NORMAL)
    return ;

  // the first player in the trick
  if (trick.actcardno() != 1)
    return ;

  // club queen played
  if (played_card != Card::CLUB_QUEEN)
    return ;

  // another player has swines / hyperswines
  if (   game.hyperswines_announced()
      && (*game.hyperswines_owner() != player) )
    return ;
  if (   game.swines_announced()
      && (*game.swines_owner() != player) )
    return ;

  // the re partner is not the last player
  if (ai.team_information().guessed_team(trick.lastplayer()) == TEAM::RE)
    return ;

  // the player has all higher trumps from the club queen ...
  auto const higher_trumps
    = trick.game().higher_cards(Card(Card::SPADE, Card::QUEEN));

  // ... if the ai does not have it
  for (auto const& c : higher_trumps) {
    // ToDo: check for announced swines/hyperswines, ...
    if (   (c != Card::CLUB_QUEEN)
        && (ai.hand().contains(c)) )
      return ;
  }

  for (auto const& c : higher_trumps)
    CHANGE_WEIGHTING(c, 1000); // *Value*

  // there is a club queen remaining -> the player does not have a trump pfund
  if (ai.cards_information().played(Card::CLUB_QUEEN)
      + ai.cards_information().of_player(ai).must_have(Card::CLUB_QUEEN)
      < game.rule()(Rule::NUMBER_OF_SAME_CARDS)) {
    if (!HandCard(ai.hand(), Card::DIAMOND_ACE).is_special())
      CHANGE_WEIGHTING(Card::DIAMOND_ACE, -500);
    if (!HandCard(ai.hand(), Card::DIAMOND_TEN).is_special())
      CHANGE_WEIGHTING(Card::DIAMOND_TEN, -400);
    if (!HandCard(ai.hand(), Card::DIAMOND_KING).is_special())
      CHANGE_WEIGHTING(Card::DIAMOND_KING, -100);
  }
} // void first_player_played_club_queen(...)

/**
 ** the bride plays in pfund in the decision trick
 **
 ** @param     played_card    the card played by the player
 ** @param     trick          the current trick
 ** @param     ai             the ai that analyses
 ** @param     weightings     the card weightings
 **
 ** @return    -
 **/
void
bride_pfunds_in_decision_trick(HandCard const& played_card,
                               Trick const& trick,
                               Ai const& ai,
                               map<Card, int>& weightings)
{
  /* Idea:
   * In the decision trick of a marriage, the bride plays a pfund.
   */

  // the name of the heuristic
  auto const heuristic_name = "bride pfunds in decision trick";

  auto const& player = played_card.player();
  auto const& game = trick.game();

  // the player is a bride in an undetermined marriage and this is the marriage decision trick
  if (!(   game.is_undetermined_marriage()
        && game.is_soloplayer(player)
        && is_selector(trick.startcard().tcolor(), game.marriage_selector())
       ) )
    return ;

  // the player has not played a color ace
  if (   !played_card.istrump()
      && (played_card.value() == Card::ACE))
    return ;

  // the player (bride) has no better pfund
  if (played_card.istrump()) {
    if (played_card != Card(game.trumpcolor(), Card::ACE)) {
      if (!HandCard(ai.hand(), game.trumpcolor(), Card::ACE).is_special())
        CHANGE_WEIGHTING(Card(game.trumpcolor(), Card::ACE), -100); // *Value*
      if (played_card != Card(game.trumpcolor(), Card::TEN)) {
        if (!HandCard(ai.hand(), game.trumpcolor(), Card::TEN).is_special())
          CHANGE_WEIGHTING(Card(game.trumpcolor(), Card::TEN), -90); // *Value*
      } // if (played_card != TEN)
    } // if (played_card != ACE)
  } else {
    for (auto const v : game.rule().card_values()) {
      if (   (v > played_card.value())
          && !Card(played_card.color(), v).istrump(game))
        CHANGE_WEIGHTING(Card(played_card.color(), v), -100); // *Value*
    }

    if (played_card.value() == Card::NINE)
      CHANGE_WEIGHTING(Card(played_card.color(), Card::NINE), -100); // *Value*
  } // if !(played_card.tcolor() == Card::TRUMP)
} // void bride_pfunds_in_decision_trick(...)

/**
 ** the player has no second diamond ace because he has not announced swines
 **
 ** @param     played_card    the card played by the player
 ** @param     trick          the current trick
 ** @param     ai             the ai that analyses
 ** @param     weightings     the card weightings
 **
 ** @return    -
 **/
void
no_swines(HandCard const& played_card,
          Trick const& trick,
          Ai const& ai,
          map<Card, int>& weightings)
{
  /* Idea:
   * If the player has played a fox and has not announced swines,
   * he has no second fox.
   * Same idea for hyperswines
   */

  // the name of the heuristic
  auto const heuristic_name = "no swines";

  auto const& game = trick.game();

  if (   game.rule()(Rule::SWINES)
      && (played_card == game.swine())
      && !game.rule()(Rule::SWINE_ONLY_SECOND)
      && (ai.cards_information().of_player(ai).played(played_card) == 1)
      && (  (game.type() == GAMETYPE::POVERTY)
          ? game.rule()(Rule::SWINES_IN_POVERTY)
          : GAMETYPE::is_solo(game.type())
          ? game.rule()(Rule::SWINES_IN_SOLO)
          : true)) {
    CHANGE_WEIGHTING(played_card, -300); // *Value*
  }

  {
    // the name of the heuristic
    char const* const heuristic_name = "no hyperswines";
    if (   game.rule()(Rule::HYPERSWINES)
        && (played_card == game.hyperswine())
        && (ai.cards_information().of_player(ai).played(played_card) == 1)
        && game.swines_announced()
        && (   game.rule()(Rule::SWINES_AND_HYPERSWINES_JOINT)
            || (*game.swines_owner() != ai))
        && (  (game.type() == GAMETYPE::POVERTY)
            ? game.rule()(Rule::HYPERSWINES_IN_POVERTY)
            : GAMETYPE::is_solo(game.type())
            ? game.rule()(Rule::HYPERSWINES_IN_SOLO)
            : true)) {
      CHANGE_WEIGHTING(played_card, -300); // *Value*
    }
  }
} // void no_swines(...)

/**
 ** assumption: the player does not play a silent marriage
 **
 ** @param     played_card    the card played by the player
 ** @param     trick          the current trick
 ** @param     ai             the ai that analyses
 ** @param     weightings     the card weightings
 **
 ** @return    -
 **/
void
no_silent_marriage(HandCard const& played_card,
                   Trick const& trick,
                   Ai const& ai,
                   map<Card, int>& weightings)
{
  /* Idea:
   * If the player has played one club queen he will not have a second.
   * We assume he does not play a silent marriage
   */

  // the name of the heuristic
  auto const heuristic_name = "no silent marriage";

  if (   (trick.game().type() == GAMETYPE::NORMAL)
      && (played_card == Card::CLUB_QUEEN)) {
    CHANGE_WEIGHTING(played_card, -200); // *Value*
  }
} // void no_silent_marriage(...)

/**
 ** second dolle over first: the player will start with the first dolle only if he has the second
 **
 ** @param     played_card    the card played by the player
 ** @param     trick          the current trick
 ** @param     ai             the ai that analyses
 ** @param     weightings     the card weightings
 **
 ** @return    -
 **
 ** @todo      check whether the players behind are of the same team
 **/
void
both_dollen(HandCard const& played_card,
            Trick const& trick,
            Ai const& ai,
            map<Card, int>& weightings)
{
  /* Idea:
   * second dolle over first: the player will start with the first dolle only if he has the second
   */

  // the name of the heuristic
  auto const heuristic_name = "both dollen";

  auto const& player = played_card.player();
  auto const& game = trick.game();

  // second dolle over first
  if (!game.rule()(Rule::DOLLEN_SECOND_OVER_FIRST))
    return ;

  if (!played_card.isdolle())
    return ;

  // first card
  if (trick.actcardno() == 1) {
    if (player.cards_to_play() <= 4)
      CHANGE_WEIGHTING(played_card, 40 + 40 * player.cards_to_play()); // *Value*
    else
      CHANGE_WEIGHTING(played_card, 200); // *Value*
  }

  // second card
  if (trick.actcardno() == 2)
    if (player.cards_to_play() <= 4)
      CHANGE_WEIGHTING(played_card, 40 * player.cards_to_play() - 20); // *Value*
    else
      CHANGE_WEIGHTING(played_card, 150); // *Value*

  // third card
  if (trick.actcardno() == 3) {
    if (   ai.team_information().guessed_team(trick.player_of_card(2))
        != ai.team_information().guessed_team(trick.player_of_card(3))) {
      if (player.cards_to_play() < 3)
        CHANGE_WEIGHTING(played_card, 20 * player.cards_to_play()); // *Value*
      else
        CHANGE_WEIGHTING(played_card, 50); // *Value*
    }
    else
      // if the player plays the dolle at the second last position, assume, he does not have the other dolle, else he would have played it before
      CHANGE_WEIGHTING(played_card, -20); // *Value*
  } // if (trick.actcardno() == 3)

  if (trick.actcardno() == 4)
    // if the player plays the dolle at the last position, assume, he does not have the other dolle, else he would have played it before
    CHANGE_WEIGHTING(played_card, -50); // *Value*
} // void (both_dollen(...))


/**
 ** the re player in a poverty starts with a trump
 **
 ** @param     played_card    the card played by the player
 ** @param     trick          the current trick
 ** @param     ai             the ai that analyses
 ** @param     weightings     the card weightings of the poverty player
 **
 ** @return    -
 **/
void
poverty_re_starts_with_trump(HandCard const& played_card,
                             Trick const& trick,
                             Ai const& ai,
                             map<Card, int>& weightings)
{
  /* Idea:
   * The re player in a poverty starts with a trump and the poverty
   * player is behind.
   * If he plays a high card (queen or above) the poverty player will not
   * have a high card.
   */

  // the name of the heuristic
  auto const heuristic_name = "poverty re starts with trump";

  auto const& player = played_card.player();
  auto const& game = trick.game();

  if (game.type() != GAMETYPE::POVERTY)
    return ;

  // only the re player
  if (   (player.team() != TEAM::RE)
      || (player == game.soloplayer()) )
    return ;

  // the poverty player is behind
  if (trick.has_played(game.soloplayer()))
    return ;

  if (Card::CHARLIE.less(played_card)) {
    // the poverty player has no queen or higher card
    auto c = game.trumps().begin();
    for ( ;
         c != game.trumps().end();
         ++c)
      if (*c == Card(Card::DIAMOND, Card::QUEEN))
        break;
    for ( ;
         c != game.trumps().end();
         ++c) {
#ifdef POSTPONED
      // ToDo: change the information of the poverty player
      CHANGE_WEIGHTING(*c, -70); // *Value*
#else
      (void)heuristic_name;
#endif
    }
  } // if (Card::CHARLIE.less(card))
} // void poverty_re_starts_with_trump(...)

/**
 ** the soloplayer will probably start with a ten when he has both aces
 **
 ** @param     played_card    the card played by the player
 ** @param     trick          the current trick
 ** @param     ai             the ai that analyses
 ** @param     weightings     the card weightings
 **
 ** @return    -
 **
 ** @todo      other gametypes
 **/
void
soloplayer_startcard_color(HandCard const& played_card,
                           Trick const& trick,
                           Ai const& ai,
                           map<Card, int>& weightings)
{
  /* Idea:
   * If the soloplayer in a solo starts with a lower color card,
   * he probably will have the higher ones.
   */

  // the name of the heuristic
  auto const heuristic_name = "soloplayer startcard color";

  auto const& player = played_card.player();
  auto const& game = trick.game();

  // sologame
  if (!GAMETYPE::is_real_solo(game.type()))
    return ;

  if (player != game.soloplayer())
    return ;

  if (trick.startplayer() != player)
    return ;

  if (played_card.istrump())
    return ;

  for (auto const v : game.rule().card_values()) {
    if (v > played_card.value()) {
      CHANGE_WEIGHTING(Card(played_card.color(), v), 110); // *Value*
    }
  } // for (v)
} // void soloplayer_startcard_color(...)

/**
 ** the re player has played the second best trump in a picture solo
 **
 ** @param     played_card    the card played by the player
 ** @param     trick          the current trick
 ** @param     ai             the ai that analyses
 ** @param     weightings     the card weightings
 **
 ** @return    -
 **/
void
picture_solo_re_played_second_best_trump(HandCard const& played_card,
                                         Trick const& trick,
                                         Ai const& ai,
                                         map<Card, int>& weightings)
{
  /* Idea:
   * If the re player starts with the second best trump, he will probably have the best one, also.
   */

  // the name of the heuristic
  auto const heuristic_name = "picture solo re played second best trump";

  auto const& player = played_card.player();
  auto const& game = trick.game();

  if (!GAMETYPE::is_picture_solo(game.type()))
    return ;

  if (player != game.soloplayer())
    return ;

  Card::Value value = Card::NOCARDVALUE;
  switch (game.type()) {
  case GAMETYPE::SOLO_JACK:
    value = Card::JACK;
    break;
  case GAMETYPE::SOLO_QUEEN:
  case GAMETYPE::SOLO_QUEEN_JACK:
    value = Card::QUEEN;
    break;
  case GAMETYPE::SOLO_KING:
  case GAMETYPE::SOLO_KING_QUEEN:
  case GAMETYPE::SOLO_KOEHLER:
    value = Card::KING;
    break;
  case GAMETYPE::SOLO_MEATLESS:
    value = Card::TEN;
    break;
  default:
    break;
  } // switch (game.type())
  if (played_card.value() == value) {
    if (game.type() == GAMETYPE::SOLO_MEATLESS)
      CHANGE_WEIGHTING(Card(played_card.color(), Card::ACE),
                       200); // *Value*
    else
      CHANGE_WEIGHTING(Card(Card::CLUB, value),
                       200); // *Value*
  } // if (played_card.value == value)
} // void picture_solo_re_played_second_best_trump(...)

/**
 ** a contra player has played a trump in a picture solo
 **
 ** @param     played_card    the card played by the player
 ** @param     trick          the current trick
 ** @param     ai             the ai that analyses
 ** @param     weightings     the card weightings
 **
 ** @return    -
 **
 ** @todo      other gametypes
 **/
void
picture_solo_contra_played_trump(HandCard const& played_card,
                                 Trick const& trick,
                                 Ai const& ai,
                                 map<Card, int>& weightings)
{
  /* Idea:
   * The contra players will most probably only have this one trump.
   */

  // the name of the heuristic
  auto const heuristic_name = "picture solo contra played trump";

  auto const& player = played_card.player();
  auto const& game = trick.game();

  if (!GAMETYPE::is_picture_solo(game.type()))
    return ;

  if (player == game.soloplayer())
    return ;

  if (!played_card.istrump())
    return ;

  int const trumpno = game.trumps().size();
  for (auto const& c : game.trumps())
    CHANGE_WEIGHTING(c, -(40 * 8 / trumpno)); // *Value*
} // void picture_solo_contra_played_trump(...)

} // namespace CardsInformationHeuristics
