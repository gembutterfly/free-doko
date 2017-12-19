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

// debugging information
#ifndef RELEASE
#ifdef DKNOF
// best jabbing card (card selection)
//#define DEBUG_BJC_OSTR cout << "[#" << __LINE__ << "] "
// best winning card (decision, whether to take the card)
//#define DEBUG_BWC_OSTR cout << "[#" << __LINE__ << "] "
#endif
#endif
#ifndef DEBUG_BJC_OSTR
#define DEBUG_BJC_OSTR null_ostr
#endif
#ifndef DEBUG_BWC_OSTR
#define DEBUG_BWC_OSTR null_ostr
#endif

#include "HeuristicInterface.h"
#include "cards_information.h"
#include "cards_information.of_player.h"
#include "team_information.h"
#include "../../game/game.h"
#include "../../party/party.h"
#include "../../party/rule.h"
#include "../../card/trick.h"

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
unsigned calcPointsOfOwnTeam( HeuristicInterface const& hi, Game const& g, bool const with_current_trick = true );
unsigned calcPointsOfOppositeTeam( HeuristicInterface const& hi, Game const& g, bool const with_current_trick = true );

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



namespace BestWinningCard {
  Card poverty_re(Trick const& trick,
                  HeuristicInterface const& hi);
  Card poverty_contra(Trick const& trick,
                      HeuristicInterface const& hi);
  Card color_solo_re(Trick const& trick,
                     HeuristicInterface const& hi);
  Card color_solo_contra(Trick const& trick,
                         HeuristicInterface const& hi);
  Card single_picture_solo_re(Trick const& trick,
                              HeuristicInterface const& hi);
  Card single_picture_solo_contra(Trick const& trick,
                                  HeuristicInterface const& hi);
  Card double_picture_solo_re(Trick const& trick,
                              HeuristicInterface const& hi);
  Card double_picture_solo_contra(Trick const& trick,
                                  HeuristicInterface const& hi);
  Card koehler_solo_re(Trick const& trick,
                       HeuristicInterface const& hi);
  Card koehler_solo_contra(Trick const& trick,
                           HeuristicInterface const& hi);
  Card meatless_solo_re(Trick const& trick,
                        HeuristicInterface const& hi);
  Card meatless_solo_contra(Trick const& trick,
                            HeuristicInterface const& hi);
}; // namespace BestWinningCard


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
 ** @version   0.7.11
 **
 ** @todo      aiconfig aggressive
 **/
Card 
Heuristics::best_winning_card(Trick const& trick,
                                  HeuristicInterface const& hi)
{
  // @heuristic::name   best winning card new
  // @heuristic::idea   take a card to win the trick

  Game const& game = hi.game();

  // @heuristic::condition   not first player
  if (trick.isempty())
    return Card::EMPTY;

  // @heuristic::condition   can overjab the trick
  if (!hi.hand().can_jab(trick))
    return Card::EMPTY;

  // call the 
  switch (game.type()) {
  case GAMETYPE::NORMAL:
    break;
  case GAMETYPE::THROWN_NINES:
  case GAMETYPE::THROWN_KINGS:
  case GAMETYPE::THROWN_NINES_AND_KINGS:
  case GAMETYPE::THROWN_RICHNESS:
  case GAMETYPE::FOX_HIGHEST_TRUMP:
    // should not happen
    break;
  case GAMETYPE::POVERTY:
    if (hi.team() == TEAM::RE)
      return (  hi.same_player(game.soloplayer())
              ? Card::EMPTY
              : BestWinningCard::poverty_re(trick, hi) );
    else
      return BestWinningCard::poverty_contra(trick, hi);
  case GAMETYPE::GENSCHER:
    break;
  case GAMETYPE::MARRIAGE:
    // do not jab a decision trick as bride
    if (   hi.same_player(game.soloplayer())
        && is_selector(trick.startcard().tcolor(),
                       game.marriage_selector()) )
      return Card::EMPTY;
    break;
  case GAMETYPE::MARRIAGE_SOLO:
    return ( (hi.team() == TEAM::RE)
            ? BestWinningCard::color_solo_re(trick, hi)
            : BestWinningCard::color_solo_contra(trick, hi) );
  case GAMETYPE::MARRIAGE_SILENT:
    if (hi.team() == TEAM::RE)
      return BestWinningCard::color_solo_re(trick, hi);
    break;
  case GAMETYPE::SOLO_MEATLESS:
    return ( (hi.team() == TEAM::RE)
            ? BestWinningCard::meatless_solo_re(trick, hi)
            : BestWinningCard::meatless_solo_contra(trick, hi) );
  case GAMETYPE::SOLO_JACK:
  case GAMETYPE::SOLO_QUEEN:
  case GAMETYPE::SOLO_KING:
    return ( (hi.team() == TEAM::RE)
            ? BestWinningCard::single_picture_solo_re(trick, hi)
            : BestWinningCard::single_picture_solo_contra(trick, hi) );
  case GAMETYPE::SOLO_QUEEN_JACK:
  case GAMETYPE::SOLO_KING_JACK:
  case GAMETYPE::SOLO_KING_QUEEN:
    return ( (hi.team() == TEAM::RE)
            ? BestWinningCard::double_picture_solo_re(trick, hi)
            : BestWinningCard::double_picture_solo_contra(trick, hi) );
  case GAMETYPE::SOLO_KOEHLER:
    return ( (hi.team() == TEAM::RE)
            ? BestWinningCard::koehler_solo_re(trick, hi)
            : BestWinningCard::koehler_solo_contra(trick, hi) );
  case GAMETYPE::SOLO_CLUB:
  case GAMETYPE::SOLO_HEART:
  case GAMETYPE::SOLO_SPADE:
  case GAMETYPE::SOLO_DIAMOND:
    return ( (hi.team() == TEAM::RE)
            ? BestWinningCard::color_solo_re(trick, hi)
            : BestWinningCard::color_solo_contra(trick, hi) );
  } // switch (game.type())

  // 1) startplayer
  // 2) second player
  // 3) second last player
  // 4) last player

  // card to choose
  HandCard card = HandCard(hi.hand(), best_jabbing_card(trick, hi));
  DEBUG_BWC_OSTR << card << '\n';
  if (card == HandCard::EMPTY)
    return card;
  DEBUG_ASSERTION(trick.isjabbed(card),
                  "Heuristics::best_wining_card(trick, hi)\n"
                  "  card = " << card << " = jabbing_card(trick, hi) does not jab the trick:\n" << trick);

  // 2), 3), color trick, has color
  if (   !trick.startcard().istrump()
      && hi.hand().contains(trick.startcard().color())) {
    Card::Color const color = trick.startcard().color();
    // the opposite team behind me has the color
    for (unsigned c = trick.actcardno() + 1; c < game.playerno(); ++c) {
      Player const& p = trick.player_of_card(c);
      if (!hi.guessed_same_team(p)
          && (   !hi.handofplayer(p).contains(color)
              || card.less(hi.handofplayer(p).highest_card(color))) )
        return HandCard::EMPTY;
    } // for (c)

    // There are enough remaining color cards
    if (hi.cards_information().remaining_others(color) + 1
        < trick.remainingcardno())
      return HandCard::EMPTY;

    return card;
  } // 2), 3), color trick, has color

  // 2), 3), color trick, first round, no color
  // 'choose pfund' must be selected before
  if (   !trick.startcard().istrump()
      && (hi.color_runs(trick.startcard().tcolor()) == 0)
      && !hi.hand().contains(trick.startcard().tcolor())) {
    // if the trick already goes to my team and the opposite team does have the color, leave it
    if (   hi.guessed_same_team(trick.winnerplayer())
        && (   (trick.winnercard().value() == Card::ACE)
            || trick.winnercard().istrump())
        && hi.handofplayer(trick.lastplayer()
                          ).contains(trick.startcard().tcolor())
        && (   (trick.actcardno() == 2)
            || (hi.handofplayer(trick.player_of_card(2)
                               ).contains(trick.startcard().tcolor()))) )
      return Card::EMPTY;

    return card;
  } // 2), 3), color trick, first round, no color

  // 4)
  if (trick.islastcard()) {
    DEBUG_BWC_OSTR << " 4) lastplayer\n";
    // do not jab if the partner gets the trick and does not get at the last position
    if (   hi.guessed_same_team(trick.winnerplayer())
        && !hi.guessed_same_team(game.player_previous(hi.player()))) {
      DEBUG_BWC_OSTR << "   partner gets the trick\n";
      return Card::EMPTY;
    }

    // stay in the back, if the partner gets the trick
    if (   (trick.winnerplayer() == trick.startplayer())
        && hi.guessed_same_team(trick.startplayer())) { {
      DEBUG_BWC_OSTR << "   partner gets the trick\n";
      return Card::EMPTY;
    }
#ifdef OUTDATED
    // always jab a trick with a card under the trump card limit
    if (card.less(hi.trump_card_limit()))
      return card;
#endif
    }
    if (!hi.guessed_same_team(trick.winnerplayer())) {
      // jab a trick according to the aiconfig
      if (   (card.value() == Card::QUEEN)
          && (static_cast<int>(trick.points())
              >= hi.value(Aiconfig::LIMITQUEEN)) ) {
        DEBUG_BWC_OSTR << "   points >= limitqueen\n";
        return card;
      }
      if (static_cast<int>(trick.points()) >= hi.value(Aiconfig::LIMITDOLLE)) {
        DEBUG_BWC_OSTR << "   points >= limitdolle\n";
        return card;
      }
    } // if (!hi.guessed_same_team(trick.winnerplayer()))

    // not many trumps
    if (   (hi.hand().numberoftrumps() * 2 <= hi.hand().cardsnumber())
        && (hi.cards_information().higher_cards_no_of_others(hi.hand().highest_trump())
            >= 2 * hi.hand().numberoftrumps()) ) {
      DEBUG_BWC_OSTR << "   get a trick with few and low trumps\n";
      return card;
    }

    // do not jab a trick with few points
    if (   (hi.hand().rel_pos_trump(card) >= 0.75)
        && (   (trick.points() <= 4)
            || (   (trick.points() <= 6)
                && hi.guessed_same_team(game.player_previous(trick.winnerplayer())))
           )
       ) {
      DEBUG_BWC_OSTR << "   too few points\n";
      return Card::EMPTY;
    }

    // jab a trick, if there are enough trumps
    if (hi.hand().rel_pos_trump(card)
        <= ((trick.points() / 20.0)
            + (hi.value(Aiconfig::AGGRESSIVE) ? 0.15 : 0)
            + (hi.guessed_same_team(trick.winnerplayer()) ? 0 : 0.3))) {
      DEBUG_BWC_OSTR << "   enough trumps\n";
      return card;
    }

    DEBUG_BWC_OSTR << "   do not jab\n";
    return HandCard::EMPTY;
  } // 4)

  if (trick.remainingcardno() == 2) { // 3)
    if (hi.guessed_same_team(trick.lastplayer())) { // 3) + partner behind
      DEBUG_BWC_OSTR << "  3), partner behind\n";
      // show myself
      if (   (game.type() == GAMETYPE::NORMAL)
          // ToDo: check the own gameplay for team informations
          && (game.teaminfo(hi.player()) == TEAM::UNKNOWN)
          && (card == Card::CLUB_QUEEN))
        return card;
      // do not jab, if the partner behind has a higher card and I do not have very many high cards
      if (   hi.handofplayer(trick.lastplayer()
                            ).higher_card_exists(trick.winnercard())
          && (hi.hand().higher_cards_no(card) + hi.hand().numberof(card)
              < (hi.hand().numberoftrumps() / 2
                 + (hi.value(Aiconfig::AGGRESSIVE) ? 0 : 1)))
          && (hi.hand().numberoftrumps()
              > 3 + hi.hand().higher_cards_no(card)) )
        return HandCard::EMPTY;
      return card;
    } // 3) + partner behind
    else { // 3) + opposite behind
      if (hi.guessed_same_team(trick.winnerplayer())) {
        DEBUG_BWC_OSTR << "  3), opposite player behind, own team has trick\n";
        // only jab the trick, if the partner has played a small card or there are many points in the trick
        if (trick.isjabbed(Card::DIAMOND_JACK)) {
          DEBUG_BWC_OSTR << "  jab with low trump\n";
          return card;
        }
        if (   (static_cast<int>(trick.points())
                >= hi.value(Aiconfig::LIMITQUEEN))
            && (   trick.isjabbed(Card::DIAMOND_QUEEN)
                || (hi.hand().rel_pos_trump(card) <= 0.4)
                || (hi.hand().higher_cards_no(card) + hi.hand().numberof(card)
                    >= (hi.hand().numberoftrumps() / 2
                        + (hi.value(Aiconfig::AGGRESSIVE) ? 0 : 1))
                   ) ) ) {
          DEBUG_BWC_OSTR << "  jab limitqueen\n";
          return card;
        }
        // jab to get the partner in backhand
        if (   hi.guessed_same_team(game.player_previous(hi.player()))
            && (   !hi.guessed_same_team(trick.winnerplayer())
                || trick.winnercard().less(hi.trump_card_limit()) )
            && (hi.hand().higher_cards_no(card) + hi.hand().numberof(card)
                >= (hi.hand().numberoftrumps() / 2 
                    + (hi.value(Aiconfig::AGGRESSIVE) ? 0 : 1))) ) {
          DEBUG_BWC_OSTR << "  get partner in backhand\n";
          return card;
        }
        return HandCard::EMPTY;
      } else { // if !(hi.guessed_same_team(trick.winnerplayer()))
        DEBUG_BWC_OSTR << "  3), opposite player behind, opposite team has trick\n";
        // use a club queen
        if (   (card == Card::CLUB_QUEEN)
            && (hi.game().teaminfo(hi.player()) == TEAM::UNKNOWN)
            && (hi.guessedteamofplayer(trick.lastplayer()) == TEAM::UNKNOWN) ) {
          DEBUG_BWC_OSTR << "  try club queen for partner behind\n";
          return card;
        }
        // do not jab a small trick with a high card
        if (    (trick.points() >= 10)
            && (hi.hand().numberof_ge(card) >= 2) )
          return card;
        if (   (static_cast<int>(trick.points())
                < hi.value(Aiconfig::LIMITQUEEN))
            && hi.trump_card_limit().less(card))
          return HandCard::EMPTY;
        if (   (static_cast<int>(trick.points())
                < hi.value(Aiconfig::LIMITDOLLE))
            && Card::CLUB_QUEEN.less(card))
          return HandCard::EMPTY;
        return card;
      } // if !(hi.guessed_same_team(trick.winnerplayer()))
    } // 3) + opposite behind
  } // 3)

  if (trick.actcardno() == 1) { // 2)
    if (   hi.guessed_same_team(trick.player_of_card(2))
        && !hi.guessed_same_team(trick.player_of_card(3)) ) {
      // 2), 3: same team, 4: opposite team
      DEBUG_BWC_OSTR << "  2), 3: same team, 4: opposite team\n";
      // if card is not so high and the partner has a higher card, let him overjab
      if (   card.less(hi.trump_card_limit())
          && hi.handofplayer(trick.player_of_card(2)).higher_card_exists(card)) {
        DEBUG_BWC_OSTR << "  let the partner have the trick\n";
        return HandCard::EMPTY;
      }
      if (   !card.less(hi.trump_card_limit())
          && (hi.hand().numberof_ge(card) >= hi.hand().numberoftrumps() / 2) ) {
        DEBUG_BWC_OSTR << "  have many high trumps\n";
        return card;
      }
      if (   !card.less(hi.trump_card_limit())
          && (trick.points() >= 10) ) {
        DEBUG_BWC_OSTR << "  many points\n";
        return card;
      }
      // ToDo
#ifdef OUTDATED
      DEBUG_BWC_OSTR << "  best winning card old\n";
      return best_winning_card_old(trick, hi);
#endif
    } // 2), 3: same team, 4: opposite team
    else if (   !hi.guessed_same_team(trick.player_of_card(2))
             && hi.guessed_same_team(trick.player_of_card(3)) ) {
      // 2), 3: opposite team, 4: same team
      DEBUG_BWC_OSTR << "  2), 3: opposite team, 4: same team\n";
      if (   !card.less(hi.trump_card_limit())
          && (hi.hand().numberof_le(card) <= hi.hand().numberoftrumps() / 3) ) {
        DEBUG_BWC_OSTR << "  have many high trumps\n";
        return card;
      }
      // ToDo
#ifdef OUTDATED
      DEBUG_BWC_OSTR << "  best winning card old\n";
      return best_winning_card_old(trick, hi);
#endif
    } // 2), 3: opposite team, 4: same team
    else { // 2, two times opposite team behind
      DEBUG_BWC_OSTR << "  2), 3: opposite team, 4: opposite team\n";
      // always jab many points
      if (trick.points() >= 10) {
        DEBUG_BWC_OSTR << "  jab many points\n";
        return card;
      }
      // always jab for a high announcement
      if (game.announcement_of_team(hi.team()) > ANNOUNCEMENT::NO120) {
        DEBUG_BWC_OSTR << "  jab for high announcement\n";
        return card;
      }
      // show team with club queen
      if (   (card == Card::CLUB_QUEEN)
          && (game.teaminfo(hi.player()) == TEAM::UNKNOWN)) {
        DEBUG_BWC_OSTR << "  show team with club queen\n";
        return card;
      }
      // let the trick pass, if the player in the front has played a high card
      if (hi.trump_card_limit().less(trick.startcard()) ) {
        DEBUG_BWC_OSTR << "  the startplayer has already played a high card\n";
        return HandCard::EMPTY;
      }
      // jab, if the team has made an annoucement
      if (game.announcement_of_team(hi.team())) {
        DEBUG_BWC_OSTR << "  jab because of announcement\n";
        return card;
      }
      // as aggressive: jab, if the player has enough high trumps
      if (hi.value(Aiconfig::AGGRESSIVE)
          && (hi.hand().higher_cards_no(card) + hi.hand().numberof(card)
              >= hi.hand().numberoftrumps() / 2)
          && (hi.hand().numberoftrumps() >= 4) ) {
        DEBUG_BWC_OSTR << "  jab because of aggressive\n";
        return card;
      }

      // ToDo
#ifdef OUTDATED
      DEBUG_BWC_OSTR << "  best winning card old\n";
      return best_winning_card_old(trick, hi);
#endif
    } // 2), two times opposite team behind
  } // 2)

  return HandCard::EMPTY;
} // Card Heuristics::best_winning_card(Trick trick, HeuristicInterface hi)


/**
 ** -> result
 **
 ** @param     trick     current trick
 ** @param     hi        heuristic interface
 **
 ** @return    best winning card for the re player in a poverty
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.11
 **
 ** @todo      all
 **/
Card
BestWinningCard::poverty_re(Trick const& trick,
                            HeuristicInterface const& hi)
{
  if (trick.islastcard())
    return Heuristics::best_jabbing_card(trick, hi);
  return Card::EMPTY;
} // Card BestWinningCard::poverty_re(Trick trick, HeuristicInterface hi)

/**
 ** -> result
 **
 ** @param     trick     current trick
 ** @param     hi        heuristic interface
 **
 ** @return    best winning card for the contra player in a poverty
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.11
 **
 ** @todo      all
 **/
Card
BestWinningCard::poverty_contra(Trick const& trick,
                                HeuristicInterface const& hi)
{
#ifndef OUTDATED
  return Heuristics::poverty_best_winning_card(trick, hi);
#endif
  return Card::EMPTY;
} // Card BestWinningCard::poverty_contra(Trick trick, HeuristicInterface hi)

/**
 ** -> result
 **
 ** @param     trick     current trick
 ** @param     hi        heuristic interface
 **
 ** @return    best winning card for the re player in a color solo
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.11
 **
 ** @todo      all
 **/
Card
BestWinningCard::color_solo_re(Trick const& trick,
                               HeuristicInterface const& hi)
{
#ifndef OUTDATED
  return Heuristics::best_winning_card_old(trick, hi);
#endif
  return Card::EMPTY;
} // Card BestWinningCard::color_solo_re(Trick trick, HeuristicInterface hi)

/**
 ** -> result
 **
 ** @param     trick     current trick
 ** @param     hi        heuristic interface
 **
 ** @return    best winning card for the contra player in a color solo
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.11
 **
 ** @todo      all
 **/
Card
BestWinningCard::color_solo_contra(Trick const& trick,
                                   HeuristicInterface const& hi)
{
#ifndef OUTDATED
  return Heuristics::best_winning_card_old(trick, hi);
#endif
  return Card::EMPTY;
} // Card BestWinningCard::color_solo_contra(Trick trick, HeuristicInterface hi)

/**
 ** -> result
 **
 ** @param     trick     current trick
 ** @param     hi        heuristic interface
 **
 ** @return    best winning card for the re player in a single picture solo
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.11
 **
 ** @todo      all
 **/
Card
BestWinningCard::single_picture_solo_re(Trick const& trick,
                                        HeuristicInterface const& hi)
{
#ifndef OUTDATED
  return Heuristics::best_winning_card_old(trick, hi);
#endif
  return Card::EMPTY;
} // Card BestWinningCard::single_picture_solo_re(Trick trick, HeuristicInterface hi)

/**
 ** -> result
 **
 ** @param     trick     current trick
 ** @param     hi        heuristic interface
 **
 ** @return    best winning card for the contra player in a single picture solo
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.11
 **
 ** @todo      all
 **/
Card
BestWinningCard::single_picture_solo_contra(Trick const& trick,
                                            HeuristicInterface const& hi)
{
#ifndef OUTDATED
  return Heuristics::best_winning_card_old(trick, hi);
#endif
  return Card::EMPTY;
} // Card BestWinningCard::single_picture_solo_contra(Trick trick, HeuristicInterface hi)

/**
 ** -> result
 **
 ** @param     trick     current trick
 ** @param     hi        heuristic interface
 **
 ** @return    best winning card for the re player in a double picture solo
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.11
 **
 ** @todo      all
 **/
Card
BestWinningCard::double_picture_solo_re(Trick const& trick,
                                        HeuristicInterface const& hi)
{
#ifndef OUTDATED
  return Heuristics::best_winning_card_old(trick, hi);
#endif
  return Card::EMPTY;
} // Card BestWinningCard::double_picture_solo_re(Trick trick, HeuristicInterface hi)

/**
 ** -> result
 **
 ** @param     trick     current trick
 ** @param     hi        heuristic interface
 **
 ** @return    best winning card for the contra player in a double picture solo
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.11
 **
 ** @todo      all
 **/
Card
BestWinningCard::double_picture_solo_contra(Trick const& trick,
                                            HeuristicInterface const& hi)
{
#ifndef OUTDATED
  return Heuristics::best_winning_card_old(trick, hi);
#endif
  return Card::EMPTY;
} // Card BestWinningCard::double_picture_solo_contra(Trick trick, HeuristicInterface hi)

/**
 ** -> result
 **
 ** @param     trick     current trick
 ** @param     hi        heuristic interface
 **
 ** @return    best winning card for the re player in a koeher solo
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.11
 **
 ** @todo      all
 **/
Card
BestWinningCard::koehler_solo_re(Trick const& trick,
                                 HeuristicInterface const& hi)
{
#ifndef OUTDATED
  return Heuristics::best_winning_card_old(trick, hi);
#endif
  return Card::EMPTY;
} // Card BestWinningCard::koehler_solo_re(Trick trick, HeuristicInterface hi)

/**
 ** -> result
 **
 ** @param     trick     current trick
 ** @param     hi        heuristic interface
 **
 ** @return    best winning card for the contra player in a koeher solo
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.11
 **
 ** @todo      all
 **/
Card
BestWinningCard::koehler_solo_contra(Trick const& trick,
                                     HeuristicInterface const& hi)
{
#ifndef OUTDATED
  return Heuristics::best_winning_card_old(trick, hi);
#endif
  return Card::EMPTY;
} // Card BestWinningCard::koehler_solo_contra(Trick trick, HeuristicInterface hi)

/**
 ** -> result
 **
 ** @param     trick     current trick
 ** @param     hi        heuristic interface
 **
 ** @return    best winning card for the re player in a meatless solo
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.11
 **
 ** @todo      all
 **/
Card
BestWinningCard::meatless_solo_re(Trick const& trick,
                                  HeuristicInterface const& hi)
{
#ifndef OUTDATED
  return Heuristics::best_winning_card_old(trick, hi);
#endif
  return Card::EMPTY;
} // Card BestWinningCard::meatless_solo_re(Trick trick, HeuristicInterface hi)

/**
 ** -> result
 **
 ** @param     trick     current trick
 ** @param     hi        heuristic interface
 **
 ** @return    best winning card for the contra player in a meatless solo
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.11
 **
 ** @todo      all
 **/
Card
BestWinningCard::meatless_solo_contra(Trick const& trick,
                                      HeuristicInterface const& hi)
{
#ifndef OUTDATED
  return Heuristics::best_winning_card_old(trick, hi);
#endif
  return Card::EMPTY;
} // Card BestWinningCard::meatless_solo_contra(Trick trick, HeuristicInterface hi)


/**
 ** -> result
 **
 ** @param     trick     current trick
 ** @param     hi        heuristic interface
 ** @param     jab_always   whether to jab always -- else return empty if jabbing is senseless (default: false)
 **
 ** @return    the "best" card to jab the trick
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.11
 **
 ** @todo      aiconfig aggressive
 **/
Card
Heuristics::best_jabbing_card(Trick const& trick,
                              HeuristicInterface const& hi,
                                  bool const jab_always)
{
  // notes
  // * first trick: duty announcement
  // * color trick: not below a fox 
  // * re: fox must not be saved (beware of dollen)
  // * small points: close overjab
  // * as contra: take heart instead of spade queen, since re will overjab with club queen
  // * high jabbing, also without a 10 in the trick: check remaining points
  // * overjab, so that the last player cannot pfund the partner
  // * check the position of the card according to the hand
  // * check the position of the card according to the remaining cards
  // * check few trumps (don't play the fox free)
  // * regard announcement of partner (take a high card)
  // * ensure announcement / winning
  //   -> heuristic 'get trick for announcement'
  // * senseless: overjab the partner close

  // card decision
  // a) narrow jabbing
  // b) high jabbing
  // c) medium
  // d) show team
  // e) save points (few trump)

  // @heuristic::condition   not first player
  if (trick.isempty())
    return Card::EMPTY;

  // @heuristic::condition   can overjab the trick
  if (!hi.hand().can_jab(trick))
    return Card::EMPTY;

  Game const& game = hi.game();
  // card to choose
  Card card; 

  DEBUG_BJC_OSTR << hi.player().name() << ": best jabbing_card()\n" << trick;
  if (game.isvirtual())
    DEBUG_BJC_OSTR << "** virtual game **\n";

  // 1) startplayer
  // 2) second player
  // 3) second last player
  // 4) last player

  // 4) / 3) + partner behind
  if (   trick.islastcard()
      || (   (trick.remainingcardno() == 2)
          && hi.guessed_same_team(trick.lastplayer()) )
     ) {
    DEBUG_BJC_OSTR << "4), 3) + partner behind\n";
    // ToDo: check for duty announcement
    // color ace / ten
    if (   !trick.startcard().istrump()
        && hi.hand().contains(trick.startcard().tcolor()))
      card = hi.hand().highest_card(trick.startcard().tcolor());
    if (!card) {
      // trump ace (fox) / trump ten / trump king
      vector<Card> cards;
      cards.push_back(Card(game.trumpcolor(), Card::ACE));
      cards.push_back(Card(game.trumpcolor(), Card::TEN));
      cards.push_back(Card(game.trumpcolor(), Card::KING));
      if (game.rule()(Rule::WITH_NINES))
        cards.push_back(Card(game.trumpcolor(), Card::NINE));
      cards.push_back(Card::DIAMOND_JACK);
      for (vector<Card>::const_iterator c = cards.begin();
           c != cards.end();
           ++c) {
        if (   hi.hand().contains(*c)
            && !HandCard(hi.hand(), *c).is_special()
            && trick.isjabbed(*c)) {
          card = *c;
          DEBUG_BJC_OSTR << "  " << card << " (manual order -- fox to nine, diamond jack)\n";
          break;
        }
      }
    } // if (trump is a color)
    // lowest winning card
    if (!card) {
      card = hi.hand().next_higher_card(trick.winnercard());
      DEBUG_BJC_OSTR << "  " << card << " (next higher card)\n";
    }
  } // 4) / 3) + partner behind
  else { // 2), 3)
    DEBUG_BJC_OSTR << "2), 3)\n";
    // 2), 3), color trick, has color
    if (!trick.startcard().istrump()) { //color trick
      DEBUG_BJC_OSTR << "color trick\n";
      Card::Color const color = trick.startcard().color();
      if (hi.hand().contains(color)) {
        DEBUG_BJC_OSTR << "has color\n";
        // highest card
        // ToDo: check whether to spare an ace
        card = hi.hand().highest_card(color);
        DEBUG_BJC_OSTR << "  " << card << " (highest card)\n";
      } else if (hi.color_runs(color) == 0) {
        // 2), 3), color trick, no color
        DEBUG_BJC_OSTR << "has not color\n";

        // first round, no color
        // ToDo: check whether another player has thrown the color / has the color in the hand
        // trump ace (fox) / trump ten / trump king
        {
          if (!(   (hi.cards_information().remaining(color)
                    >= trick.remainingcardno())
                && (   hi.guessed_same_team(trick.lastplayer())
                    || hi.handofplayer(trick.lastplayer()).contains(color))
                && (   (trick.actcardno() == 2)
                    || hi.guessed_same_team(trick.player_of_card(2))
                    || hi.handofplayer(trick.player_of_card(2)).contains(color)))
             ) {
            // if the trick can not get through, take a high card
            // ToDo: check short colors (without nines, dolle)
            card = hi.hand().same_or_higher_card(hi.trump_card_limit());
            if (!card)
              card = hi.hand().same_or_higher_card(hi.lowest_trump_card_limit());
            if (!card)
              card = hi.hand().same_or_higher_card(Card::DIAMOND_JACK);
            DEBUG_BJC_OSTR << "  " << card << " (manual order -- high trump)\n";
          }

          if (!card) {
            vector<Card> cards;
            if (!(   (   !hi.guessed_same_team(trick.lastplayer())
                      && hi.cards_information().of_player(trick.lastplayer()).played(color))
                  || (   (trick.actcardno() == 2)
                      && !hi.guessed_same_team(trick.player_of_card(2))
                      && hi.cards_information().of_player(trick.player_of_card(2)).played(color))
                 )) {
              // only play below diamond jack, if no opposite player has played the color before
              cards.push_back(Card(game.trumpcolor(), Card::ACE));
              cards.push_back(Card(game.trumpcolor(), Card::TEN));
              cards.push_back(Card(game.trumpcolor(), Card::KING));
              if (game.rule()(Rule::WITH_NINES))
                cards.push_back(Card(game.trumpcolor(), Card::NINE));
            }
            for (vector<Card>::const_iterator c = cards.begin();
                 c != cards.end();
                 ++c) {
              if (   hi.hand().contains(*c)
                  && !HandCard(hi.hand(), *c).is_special()
                  && trick.isjabbed(*c)) {
                card = *c;
                DEBUG_BJC_OSTR << "  " << card << " (manual order -- fox to nine, diamond jack)\n";
                break;
              }
            }
          }

          if (   !card
              && !trick.winnercard().istrump()) {
            card = hi.hand().same_or_higher_card(Card::DIAMOND_JACK);
            DEBUG_BJC_OSTR << "  " << card << " (same or higher card(diamond jack))\n";
          }
        } // if (trump is a color)

        if (   !card
            || !trick.isjabbed(card)) {
          card = hi.hand().next_higher_card(trick.winnercard());
          DEBUG_BJC_OSTR << "  " << card << " (next higher card)\n";
        }

        // show with club queen
        if (   (game.type() == GAMETYPE::NORMAL)
            && game.less(Card::CHARLIE, card)
            && (hi.team() == TEAM::RE)
            && (game.teaminfo(hi.player()) != TEAM::RE)
            && hi.hand().contains(Card::CLUB_QUEEN)
            && trick.isjabbed(Card::CLUB_QUEEN, hi.hand())) {
          card = Card::CLUB_QUEEN;
          DEBUG_BJC_OSTR << "  " << card << " (show with club queen)\n";
        }

      } else { // 2), 3), color trick (other cases)
        DEBUG_BJC_OSTR << "color trick (other cases)\n";
        // ToDo: jack / queen
        // check how many points are still out
        // check whether another player has thrown the color / has the color in the hand
        if (   (trick.points() >= 10)
            || (   hi.cards_information().remaining_others(color, Card::ACE)
                && !Card(color, Card::ACE).istrump(game))

            || (   hi.cards_information().remaining_others(color, Card::TEN)
                && !Card(color, Card::TEN).istrump(game)) ) {
          // make a very high jab
          if (   (trick.points() >= 20)
              || (   (trick.points() >= 10)
                  && (trick.actcardno() == 1) ) ) {
            card = hi.hand().highest_trump();
            DEBUG_BJC_OSTR << "  " << card << " (highest trump)\n";
          }
          // make a high jab
          if (   !card
              && (   (trick.points() >= 14)
                  || checkAllOppositeTeam(trick, hi) )
              ) {
            card = hi.hand().same_or_lower_card(Card::CLUB_QUEEN);
            DEBUG_BJC_OSTR << "  " << card << " (same or lower card(club queen))\n";
          }
          if (!card) {
            card = hi.hand().same_or_higher_card(hi.lowest_trump_card_limit());
            DEBUG_BJC_OSTR << "  " << card << " (same or higher card(lowest trump card limit))\n";
          }
        } else if (   (trick.actcardno() == 1)
                   && hi.guessed_same_team(trick.startplayer()) ) {
          // jab high, if both opposite players are behind
          card = hi.hand().same_or_higher_card(hi.trump_card_limit());
          DEBUG_BJC_OSTR << "  " << card << " (same or higher card(trump card limit))\n";
        } 

        if (!card) {
          vector<Card> cards;
          // ToDo: check short colors (without nines, dolle)
          if (hi.cards_information().remaining(color)
              >= trick.remainingcardno()) {
            cards.push_back(Card(game.trumpcolor(), Card::ACE));
            cards.push_back(Card(game.trumpcolor(), Card::TEN));
            cards.push_back(Card(game.trumpcolor(), Card::KING));
          }
          cards.push_back(Card::DIAMOND_JACK);
          for (vector<Card>::const_iterator c = cards.begin();
               c != cards.end();
               ++c) {
            if (   hi.hand().contains(*c)
                && !HandCard(hi.hand(), *c).is_special()
                && trick.isjabbed(*c)) {
              card = *c;
              DEBUG_BJC_OSTR << "  " << card << " (manual order -- fox to nine, diamond jack -- highest jack)\n";
              break;
            }
          }
        }
        if (!card) {
          // not many points or no high trump
          card = hi.hand().same_or_higher_card(hi.lowest_trump_card_limit());
          DEBUG_BJC_OSTR << "  " << card << " (same or higher card(lowest trump card limit))\n";
        }
        if (   !card
            || !trick.isjabbed(card) ) {
          card = hi.hand().next_higher_card(trick.winnercard());
          DEBUG_BJC_OSTR << "  " << card << " (next higher card)\n";
        }

      } // cases in color trick
    } else { // if !(color trick)
      if (trick.actcardno() == game.playerno() - 1) {
        DEBUG_BJC_OSTR << "last player\n";
        // 3), opposite team behind
        if (static_cast<int>(trick.points())
            >= hi.value(Aiconfig::LIMITQUEEN)) {
          card = hi.hand().same_or_higher_card(hi.trump_card_limit());
          DEBUG_BJC_OSTR << "  " << card << " (same or higher card(trump card limit))\n";
        }
      } else { // if !3 (opposite team behind)

        if (   hi.guessed_same_team(trick.player_of_card(2))
            && !hi.guessed_same_team(trick.player_of_card(3)) ) {
          // 2), 3: same team, 4: opposite team
          // take a high card (over trump card limit)
          // or the lowest winning card
          if (   (static_cast<int>(trick.points())
                  >= hi.value(Aiconfig::LIMITDOLLE))
              || (   (static_cast<int>(trick.points() >= 10))
                  && (hi.hand().higher_cards_no(Card::CLUB_QUEEN)
                      >= 2) ) ) {
            card = hi.hand().highest_trump();
            DEBUG_BJC_OSTR << "  " << card << " (highest trump (many points) )\n";
          }
          if (!card) {
            card = hi.hand().same_or_higher_card(hi.trump_card_limit());
            DEBUG_BJC_OSTR << "  " << card << " (same or higher card(trump card limit) )\n";
            if (   !card
                || !trick.isjabbed(card)
                || game.less(Card::CLUB_QUEEN, card)) {
              card = hi.hand().next_higher_card(Card::CHARLIE);
              DEBUG_BJC_OSTR << "  " << card << " (next higher card(club jack))\n";
            }
            if (!trick.isjabbed(card)) {
              card = hi.hand().next_higher_card(trick.winnercard());
              DEBUG_BJC_OSTR << "  " << card << " (next higher card)\n";
            }
          } // if (!card)
        } // 2), 3: same team, 4: opposite team
        else if (   !hi.guessed_same_team(trick.player_of_card(2))
                 && hi.guessed_same_team(trick.player_of_card(3)) ) {
          // 2), 3: opposite team, 4: same team
          // take lowest winning card
          card = hi.hand().next_jabbing_card(trick.winnercard());
          while (  (   (card.value() >= 10)
                    || (   (   !game.is_real_solo()
                            || GAMETYPE::is_color_solo(game.type()) )
                        && HandCard(hi.hand(), card).less(Card::DIAMOND_JACK)) )
                 && !HandCard(hi.hand(), card).is_special()
                 && card)
          {
            card = hi.hand().next_higher_card(card);
          }
          if (!card)
            card = hi.hand().next_jabbing_card(trick.winnercard());
          DEBUG_BJC_OSTR << "  " << card << " (next higher card)\n";
        } // 2), 3: opposite team, 4: same team
        else { // 2, two times opposite team behind
          // take the highest card, if the partner has played ten points
          if (trick.points() >= 10) {
            card = hi.hand().highest_trump();
            // do not take a dolle when it can be jabbed
            if (   game.rule()(Rule::DOLLEN_SECOND_OVER_FIRST)
                && (card == Card::DOLLE)
                && hi.handofplayer(trick.player_of_card(2)).contains(Card::DOLLE)
                && hi.handofplayer(trick.player_of_card(3)).contains(Card::DOLLE)
               ) {
              card = hi.hand().same_or_lower_card(Card::CLUB_QUEEN);
              if (   (card.value() != Card::QUEEN)
                  && (card.value() != Card::JACK))
                card = Card::EMPTY;
              DEBUG_BJC_OSTR << "  " << card << " (same or lower card(club queen))\n";
            } 
          } // if (trick.points() >= 10)
          // if there is an announcement higher then 'no 120', then take the highest queen
          if (game.announcement_of_team(hi.team()) > ANNOUNCEMENT::NO120) {
            card = hi.hand().same_or_lower_card(Card::CLUB_QUEEN);
            if (   (card.value() != Card::QUEEN)
                && (card.value() != Card::JACK))
              card = Card::EMPTY;
            DEBUG_BJC_OSTR << "  " << card << " (same or lower card(club queen))\n";
          }
          if (!card) {
            // take lowest queen, the highest jack or the lowest winnercard
            card = hi.hand().same_or_higher_card(Card::DIAMOND_QUEEN);
            DEBUG_BJC_OSTR << "  " << card << " (same or higher card(diamond queen))\n";
            if (   !card
                || !trick.isjabbed(card)
                || game.less(Card::CLUB_QUEEN, card)) {
              card = hi.hand().same_or_lower_card(Card::CLUB_JACK);
              if (card.value() != Card::JACK)
                card = Card::EMPTY;
              DEBUG_BJC_OSTR << "  " << card << " (same or lower card(club jack))\n";
            }
          }
          if (   !card
              || !trick.isjabbed(card)) {
            card = hi.hand().next_higher_card(trick.winnercard());
            DEBUG_BJC_OSTR << "  " << card << " (next higher card)\n";
          }
        } // 2), two times opposite team behind
      } // if !3 (opposite team behind)
    } // if !(color trick)
  } // 2), 3)

  { // special case: dolle
    if (   !card
        && (trick.winnercard() == Card::DOLLE)
        && game.rule()(Rule::DOLLEN_SECOND_OVER_FIRST)
        && hi.hand().contains(Card::DOLLE))
      card = Card::DOLLE;

    Card const card_bak = card;
    // do not play a dolle / swine if the opposite player can overjab
    if ( (  (card == Card::DOLLE)
          || HandCard(hi.hand(), card).isswine())
        && (   ( (trick.actcardno() <= 2)
                ? (   !hi.guessed_same_team(trick.lastplayer())
                   && hi.handofplayer(trick.lastplayer()).higher_card_exists(card))
                : false)
            || ( (trick.actcardno() <= 1)
                ? (   !hi.guessed_same_team(trick.player_of_card(2))
                   && hi.handofplayer(trick.player_of_card(2)).higher_card_exists(card))
                : false) )
       ) {
      card = hi.hand().next_lower_card(card);
      if (!trick.isjabbed(card))
        card = card_bak;
    }
  } // special case: dolle

  // ToDo: check for using a special card
  card = optimized_card_to_play(card, trick, hi);
  DEBUG_BJC_OSTR << "  " << card << " (optimized card to play)\n";

  // prefer a swine over a dolle with the rule 'dollen second over first'
  if (   card.isdolle(game)
      && game.rule()(Rule::DOLLEN_SECOND_OVER_FIRST)
      && (   ( (trick.actcardno() <= 2)
              ? (   !hi.guessed_same_team(trick.lastplayer())
                 && hi.handofplayer(trick.lastplayer()).contains(Card::DOLLE))
              : false)
          || ( (trick.actcardno() <= 1)
              ? (   !hi.guessed_same_team(trick.player_of_card(2))
                 && hi.handofplayer(trick.player_of_card(2)).contains(Card::DOLLE))
              : false) )
     ) {
    if (hi.hand().next_higher_card(card) != Card::EMPTY) {
      card = hi.hand().next_higher_card(card);
      DEBUG_BJC_OSTR << "  " << card << " (swine over a dolle)\n";
    }
  }

  // check whether to show the team (club queen)
  if (   (game.type() == GAMETYPE::NORMAL)
      // ToDo: check the own gameplay for team informations
      && (game.teaminfo(hi.player()) == TEAM::UNKNOWN)
      && (hi.team() == TEAM::RE)
      && hi.hand().contains(Card::CLUB_QUEEN)
      && trick.isvalid(Card::CLUB_QUEEN, hi.hand())
      && trick.isjabbed(Card::CLUB_QUEEN)) {
    if (   (card == Card::SPADE_QUEEN)
        || (card == Card::HEART_QUEEN)
        || (   (card == Card::DIAMOND_QUEEN)
            && !hi.cards_information().remaining_others(Card::HEART_QUEEN)
            && !hi.cards_information().remaining_others(Card::SPADE_QUEEN)
           )
       ) {
      card = Card::CLUB_QUEEN;
      DEBUG_BJC_OSTR << "  " << card << " (show with club queen)\n";
    }
    if (   !trick.startcard().istrump()
        && (hi.color_runs(trick.startcard().tcolor()) > 0)
        && !trick.islastcard()
        && hi.guessed_same_team(trick.lastplayer())) {
      card = Card::CLUB_QUEEN;
      DEBUG_BJC_OSTR << "  " << card << " (show with club queen)\n";
    }
  } // if (re, not shown)

  if (!jab_always) {
    // do not take 10 points (fox, dolle, swine) if the player behind can overjab
    if (   (card.points() >= 10)
        && card.istrump(game)
        && !(   !trick.startcard().istrump()
             && hi.cards_information().color_runs(trick.startcard().color()) == 0)
        && (   ( (trick.actcardno() <= 2)
                ? (   !hi.guessed_same_team(trick.lastplayer())
                   && hi.handofplayer(trick.lastplayer()).higher_card_exists(card))
                : false)
            || ( (trick.actcardno() <= 1)
                ? (   !hi.guessed_same_team(trick.player_of_card(2))
                   && hi.handofplayer(trick.player_of_card(2)).higher_card_exists(card))
                : false) ) ) {
      card = Card::EMPTY;
      DEBUG_BJC_OSTR << "  " << card << " (test for overjabbing)\n";
    }

    // do not overjab the partner close
    if (card
        && hi.guessed_same_team(trick.winnerplayer())
        && (   trick.islastcard()
            || (card == game.next_higher_card(trick.winnercard()))
           ) ) {
      card = Card::EMPTY;
      DEBUG_BJC_OSTR << "  " << card << " (no close overjabbing the partner)\n";
    }

  } // if (!jab_always)

  return card;
} // Card Heuristics::best_jabbing_card(Trick trick, HeuristicInterface hi, bool jab_always)

