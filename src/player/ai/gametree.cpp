/**********************************************************************
 *
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

#include "gametree.h"


#include "trickweighting.h"
#include "VirtualGamesInterface.h"
#include "ai.h"
#include "cards_information.h"
#include "team_information.h"

#include "../../party/party.h"
#include "../../card/trick.h"
#include "../../game/exception.h"
#include "../../game/game_summary.h"
#include "../../ui/ui.h"
#include "../../misc/setting.h"

#include "../../utils/string.h"

static unsigned counter = 0;

// whether to save the runtime
//#define SAVE_RUNTIME

#ifdef RELEASE
#undef SAVE_RUNTIME
#endif

// the multiplier for a game point
#define GAME_POINTS_MULTIPLIER 10000

//#define INFO_THROW

#define CHECK_MODUS(modus) \
  if (   ((modus >=  100 * GAME_POINTS_MULTIPLIER) && (modus != INT_MAX)) \
      || ((modus <= -100 * GAME_POINTS_MULTIPLIER) && (modus != INT_MIN))) { \
    cerr << '\n'; \
    cerr << "bad modus : " << modus << '\n'; \
    DEBUG_ASSERTION(false, "GameTree: bad modus = " << modus); \
  } else

// if a type does always lead to an invalid game exception, take the next one
namespace CARDS_TESTS {
  enum CardsTest {
    VALID_CARDS,
    ALL_CARDS,
    FINISHED
  }; // enum CardsTest
} // namespace CARDS_TESTS 
using CARDS_TESTS::CardsTest;

/**
 ** constructor
 **
 ** @param      ai   the ai
 **
 ** @return     -
 **
 ** @author     Diether Knof
 **
 ** @version    0.7.5
 **/
Gametree::Gametree(Ai const& ai) :
  Weighting(ai),
  end_depth_(0),
  aitype_(ai.aitype()),
  rating_(ai.rating())
{
  this->calculate_end_depth();

  return ;
} // Gametree::Gametree(Ai const& ai)

/**
 ** destructor
 **
 ** @param      -
 **
 ** @return     -
 **
 ** @author     Diether Knof
 **
 ** @version    0.7.5
 **/
Gametree::~Gametree()
{ }

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    whether to use heuristics
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.3
 **/
bool
Gametree::with_heuristics() const
{
  switch (this->aitype()) {
  case AiType::GAMETREE:
  case AiType::GAMETREE_FOR_TEAM:
    return false;
  case AiType::GAMETREE_WITH_HEURISTICS:
    return true;
  default:
    DEBUG_ASSERTION(false,
                    "Gametree::with_heuristics()\n"
                    "  unsupported aitype '" << this->aitype() << "'");
    return false;
  } // switch (this->aitype())
} // bool Gametree::with_heuristics() const

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    whether the teampartner plays for the player
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.3
 **/
bool
Gametree::for_team() const
{
  switch (this->aitype()) {
  case AiType::GAMETREE:
  case AiType::GAMETREE_WITH_HEURISTICS:
    return false;
  case AiType::GAMETREE_FOR_TEAM:
    return true;
  default:
    DEBUG_ASSERTION(false,
                    "Gametree::for_team()\n"
                    "  unsupported aitype '" << this->aitype() << "'");
    return false;
  } // switch (this->aitype())
} // bool Gametree::for_team() const

/**
 ** Calculates and sets the end depth (how many tricks in the future are
 ** to be calculated).
 **
 ** @param      -
 **
 ** @return     the calculated end depth
 **
 ** @author     Diether Knof
 **
 ** @version    0.5.4
 **/
void
Gametree::calculate_end_depth()
{
  unsigned future_limit = this->ai().future_limit();
  this->end_depth_ = 0;

  { // count the number of card-combinations for the current trick
    Trick const& current_trick
      = this->vgi().game().trick_current();
    for (unsigned i = current_trick.actcardno();
         i < this->vgi().game().playerno();
         i++) {
      if (this->vgi().value(Aiconfig::HANDS_KNOWN)) {
        future_limit /= this->vgi().handofplayer(current_trick.player_of_card(i)
                                                ).validcards(current_trick).cardsnumber();
      } else {
        future_limit /= this->vgi().handofplayer(current_trick.player_of_card(i)
                                                ).cards_single().cardsnumber();
      }
    } // for (i) 
  } // count the number of card-combinations for the current trick


  // calculate for all following tricks the number of cards of each player
  while (future_limit > 0) {
    this->end_depth_ += 1;
    if (this->end_depth_
        == this->vgi().game().tricks_remaining_no())
      break;

    for (auto player = this->vgi().game().players_begin();
         player != this->vgi().game().players_end();
         player++) {
      if (this->vgi().game().trick_current().cardno_of_player(**player)
          >= this->vgi().game().trick_current().actcardno()) {
        // the player still has to play a card in the current trick
        future_limit /= (this->vgi().handofplayer(**player
                                                 ).cardsnumber()
                         - this->end_depth_);
      } else {
        // the player has already played a card, so he has one card less 
        // on the hand
        future_limit /= (this->vgi().handofplayer(**player
                                                 ).cardsnumber()
                         - (this->end_depth_ - 1));
      }
    } // for (player \in this->vgi.game().player)
  } // while (future_limit > 0)

  this->end_depth_ = min(this->end_depth_,
                         ( (this->vgi().last_trick_to_calculate()
                            >= this->vgi().game().trick_current_no())
                          ? (this->vgi().last_trick_to_calculate()
                             - this->vgi().game().trick_current_no())
                          : 0)
                        );

  // always calculate the current trick
  if (this->end_depth_ == 0)
    this->end_depth_ = 1;

  return ;
} // void VirtualGames::calculate_end_depth()


/**
 ** -> result
 **
 ** @param      card_   the card
 **
 ** @return     the weighting for the card 'card'
 **
 ** @author     Diether Knof
 **
 ** @version    0.7.5
 **/
int
Gametree::weighting(Card const& card_) const
{
  // the real ai
  Ai const& real_ai = this->ai();
  // the modus
  int modus = INT_MIN;

  // Create new players.
  // The hand is set during the recursion.
  vector<Player*> player_virt;
  for (auto player = this->vgi().game().players_begin();
       player != this->vgi().game().players_end();
       player++) {
    if (this->with_heuristics()) {
      player_virt.push_back(real_ai.Ai::clone());
      dynamic_cast<Aiconfig*>(player_virt.back())->set_aitype_for_all_tricks(AiType::NO_CHOOSEBESTCARD);
    } else { // if !(this->with_heuristics())
      if (this->vgi().same_player(**player)) {
        player_virt.push_back(real_ai.Ai::clone());
      } else {
        player_virt.push_back(real_ai.Player::clone());
      }
    } // if !(this->with_heuristics())
  } // for (player)

  // create a virtual game
  Game virt_game(this->vgi().game(), player_virt);

  // set all hands
  unsigned p = 0;
  for (auto player = virt_game.players_begin();
       player != virt_game.players_end();
       ++player, ++p) {
    (*player)->set_name(this->vgi().game().player(p).name());
    (*player)->set_team(real_ai.team_information().guessed_team(p));
    (*player)->set_hand(this->vgi().handofplayer(p));

    (*player)->self_check();
  }

  try {
    { // play the card
      HandCard const card(virt_game.player_current().hand(), card_);
      if (   card.possible_swine()
          && !card.isswine()) {
        virt_game.swines_announce(virt_game.player_current());
      }
      if (   card.possible_hyperswine()
          && !card.ishyperswine())
        virt_game.hyperswines_announce(virt_game.player_current());
      virt_game.player_current().hand().playcard(card);
      virt_game.trick_current() += card;
      virt_game.teaminfo_update();
      virt_game.player_current_
        = &virt_game.player_following(virt_game.player_current());
      for (auto p = virt_game.players_begin();
           p != virt_game.players_end();
           p++)
        (*p)->card_played(card);
    } // play the card 'card_'

    modus = this->rated_modus(virt_game, this->end_depth());

  } catch (...) {
    // delete the virtual players
    for (auto& player : player_virt)
      delete player;

    throw;
  } // try

  // delete the virtual players
  for (auto& player : player_virt)
    delete player;

  return modus;
} // int Gametree::weighting(Card card) const

/**
 ** Returns the rated modi.
 ** From players of the same team, the modi is calculated positiv,
 ** for players of the opposite team, the modi is calculated negative
 **
 ** @param      virt_game	the virtual game
 ** @param      tricks_to_calc	how many tricks to be calced
 **
 ** @return     the maximal minimum modi
 **
 ** @author     Diether Knof
 **
 ** @version    0.7.3
 **/
int
Gametree::rated_modus(Game& virt_game, unsigned const tricks_to_calc) const
{
  ::ui->update();
  if (::game_status != GAMESTATUS::GAME_PLAY)
    return 0;

  // the correspoding virtual player to the ai
  Ai const& virt_ai
    = dynamic_cast<Ai const&>(virt_game.player(this->vgi().no()));
  // the team of the ai
  Team const ai_team = virt_ai.team();

  // full trick or finished game
  if (virt_game.trick_current().isfull()) {
    int const modus = full_trick_modi(virt_game, tricks_to_calc);
#ifdef DKNOF
    CHECK_MODUS(modus);
#endif
    return modus;
  }

  // Here we are in the middle of a trick.
  // Try all cards and take the worst/best modi if the player is in the
  // opposite/same team.

  //  The player is in the same team, if the team of the ai is known to all
  //  and the team of the player is the team of the ai.
  bool same_team = false;

  // first look, whether the team of the ai is known to all
  // or we are in the last three tricks (then the team should be known)
  // ToDo: use team information to check, whether the player has given enough information of his team
  if (   (virt_game.teaminfo(virt_ai) == ai_team)
      || (virt_game.tricks_remaining_no() <= 2) ) {
    // next test, whether the team of the player is the same of the ai
    if (virt_game.teaminfo(virt_game.player_current()) == ai_team)
      same_team = true;
    else if (   (virt_game.teaminfo(virt_game.player_current()) == TEAM::UNKNOWN)
             && (this->vgi().guessedteamofplayer(virt_game.no(virt_game.player_current()))
                 == ai_team))
      // The team of the player is not known,
      // but the interface says, that the teams are the same.
      same_team = true;
  } // if (team of virt_ai known)

  // Invert the modus if the counting is for the opposite players;
  bool const invert_modus = (  (virt_game.no(virt_game.player_current())
                                != this->vgi().no())
                             && (   !this->for_team()
                                 || !same_team) );

  // Now test all the cards and calculate the modus

  // first set the hand of the player
  Hand hand;
  try {
    hand = virt_ai.handofplayer(virt_game.player_current());
  } catch (InvalidGameException e) {
#ifdef DKNOF
#if 0
    cerr << "\n\n\n\n\n\n\n\n\n\n\n\n\n";
    cerr << "invalid handofplayer:\n";
    cerr << this->vgi().game();
    cerr << dynamic_cast<Ai const&>(this->vgi().game().player(this->vgi().no())).cards_information();
#endif
#endif
    // nothing to do, because the modus cannot get changed
    throw;
  } // catch()

  if (this->with_heuristics()) {

    // update/set the hand of the current player
    virt_game.player_current().set_hand(hand);

    // card that is played
    HandCard const card
      = dynamic_cast<Ai&>(virt_game.player_current()).card_get();
    if (!card.is_empty()) {
      // create a new virtual game
      vector<Player*> player_virt;
      for (auto player = virt_game.players_begin();
           player != virt_game.players_end();
           player++) {
        player_virt.push_back((*player)->clone());

        player_virt.back()->set_hand(virt_ai.handofplayer(virt_ai.game().player(player_virt.size() - 1)));
      } // for (player)
      auto virt_game_2 = make_unique<Game>(virt_game, player_virt);

      unsigned const counter_bak = counter;
      try { // play the card
        try {
          HandCard const card2(virt_game_2->player_current().hand(), card);
          if (   card2.possible_swine()
              && !card2.isswine())
            virt_game_2->swines_announce(virt_game_2->player_current());
          if (   card2.possible_hyperswine()
              && !card2.ishyperswine())
            virt_game_2->hyperswines_announce(virt_game_2->player_current());
          virt_game_2->play_card(card2);

          int modus;
          try {
            ::ui->ai_test_card(card, virt_game.no(virt_game.player_current()));
            modus = this->rated_modus(*virt_game_2, tricks_to_calc);
#ifdef DKNOF
            CHECK_MODUS(modus);
#endif

            if (   (modus == INT_MIN)
                || (modus == INT_MAX))
              DEBUG_THROW( InvalidGameException, InvalidGameException() );

            ::ui->ai_card_weighting(modus);
          } catch(...) {
            ::ui->ai_card_weighting(INT_MIN);
            throw;
          } // try

          // delete the created virtual players and virtual game
          for (auto& player : player_virt)
            delete player;

#ifdef DKNOF
          CHECK_MODUS(modus);
#endif
          return modus;
        } catch (...) {
          // delete the created virtual players and virtual game
          for (auto& player : player_virt)
            delete player;
        } // try
      } catch (InvalidGameException const& e) {
        counter = counter_bak;
        DEBUG_CAUGHT();
      } catch (...) {
        throw;
      } // try

      // The card could not be played,
      // so remove it from the hand and look at all other cards.
      hand.remove(card);
    } // if (card)

  } // if (this->with_heuristics())


  // this mark is taken if no valid game has been found and not all cards
  // have been tested (only the valid cards)
  CardsTest cards_test
    = ((   this->vgi().value(Aiconfig::HANDS_KNOWN)
        || (hand.cardsnumber() == virt_game.tricks_remaining_no())
        || (this->for_team() && same_team) )
       ? CARDS_TESTS::VALID_CARDS
       : CARDS_TESTS::ALL_CARDS);

  // When the ai does not 'know' the hand of the player
  // all cards have to be viewed, else there can result only invalid games.
  HandCards valid_cards
    = ((cards_test == CARDS_TESTS::VALID_CARDS)
       ? hand.validcards(virt_game.trick_current())
       : hand.cards_single());

  // the rating used for the weightings
  //Rating* const rating = Rating::new_(this->rating());
  Rating* const rating
    = Rating::new_(this->rating());
  DEBUG_ASSERTION(rating,
                  "Gametree::rated_modus()\n"
                  "  rating '" << this->rating() << "' not created");

  // The sum of the modi is used in order to catch special points
  // in the case that not all teams are known (see below)
  int modi_sum = 0;
  // the number of modi in 'modi_sum'
  int modi_num = 0;
  do {
    unsigned valid_cards_no = 0;
    // now test all cards
    for (auto c = valid_cards.begin();
         c != valid_cards.end();
         ++c) {
      // create a new virtual game
      vector<Player*> player_virt;
      for (auto player = virt_game.players_begin();
           player != virt_game.players_end();
           player++) {
        player_virt.push_back((*player)->clone());

          //player_virt.back()->set_hand(virt_ai.handofplayer(virt_ai.game().player(player_virt.size() - 1)));
      } // for (player)
      auto virt_game_2 = make_unique<Game>(virt_game, player_virt);
      for (unsigned p = 0; p < virt_game_2->playerno(); ++p)
          player_virt[p]->set_hand(virt_ai.handofplayer(virt_ai.game().player(p)));

      try {
        { // now play the card
          HandCard const card2(virt_game_2->player_current().hand(), *c);
          if (   card2.possible_swine()
              && !card2.isswine())
            virt_game_2->swines_announce(virt_game_2->player_current());
          if (   card2.possible_hyperswine()
              && !card2.ishyperswine())
            virt_game_2->hyperswines_announce(virt_game_2->player_current());
          virt_game_2->player_current().hand().playcard(card2);
          virt_game_2->trick_current() += card2;
          virt_game_2->teaminfo_update();

          virt_game_2->player_current_
            = &virt_game_2->player_following(virt_game_2->player_current());
          try {
            for (auto p = virt_game_2->players_begin();
                 p != virt_game_2->players_end();
                 p++)
              (*p)->card_played(card2);
          } catch (...) {
            throw;
          }
        } // play the card 'c'

#ifndef RELEASE
        if (virt_game_2->tricks_remaining_no() > 1)
          for (auto player = virt_game_2->players_begin();
               player != virt_game_2->players_end();
               player++)
            DEBUG_ASSERTION(((*player)->hand().cardsnumber() >= 1),
                            "Gametree::rated_modus():\n"
                            "  empty hand.\n"
                            "Game: \n");
#endif
        // get the modi through the recursion
        ::ui->ai_test_card(*c, virt_game.no(virt_game.player_current()));
        try {
          int const modus = this->rated_modus(*virt_game_2, tricks_to_calc);
          ::ui->ai_card_weighting(modus);
          if (   (modus != INT_MIN)
              && (modus != INT_MAX)) {
            for (unsigned i = 0; i < hand.numberof(*c); ++i) {
              valid_cards_no += 1;
              // the rating is from the view of the player of the card and not
              // from the ai
              if (invert_modus)
                rating->add(-modus);
              else
                rating->add(modus);
              modi_sum += modus;
              modi_num += 1;
            } // for (i)
          }
        } catch (...) {
          ::ui->ai_card_weighting(INT_MIN);
          throw;
        }
      } catch (InvalidGameException const& e) {
        // nothing to do
        DEBUG_CAUGHT();
      } catch (...) {
        // delete the created virtual players and virtual game
        for (auto& player : player_virt)
          delete player;
        delete rating;

        throw;
      } // try
      if (   !this->vgi().game().isvirtual()
          && (virt_game.played_cards_no()
              == this->vgi().game().played_cards_no() + 1) ) {
        // first depth in virtual games
        ::ui->add_progress((1.0 - 0.01) / valid_cards.size());
      } // if (!this->vgi().game().isvirtual())

      // delete the created virtual players and virtual game
      for (auto& player : player_virt)
        delete player;

    } // for (c \in valid_cards)

    // delete the worst values, since the player will only try his best ones
    if (virt_game.tricks_remaining_no() - 1 >
        hand.cardsnumber() - valid_cards_no)
      rating->delete_worst(virt_game.tricks_remaining_no() - 1
                           - (hand.cardsnumber() - valid_cards_no));

    if (   (cards_test == CARDS_TESTS::VALID_CARDS)
        && (   (rating->value() == INT_MIN)
            || (rating->value() == INT_MAX) )) {
      // test only the cards that have not been tested, yet
      HandCards remaining_cards = hand.cards_single();
      remaining_cards.remove(valid_cards);
      valid_cards = remaining_cards;
      cards_test = CARDS_TESTS::ALL_CARDS;
      ::ui->add_progress(-1.0);
      continue;
    } // if (no valid game found)
    cards_test = CARDS_TESTS::FINISHED;
  } while (cards_test < CARDS_TESTS::FINISHED);

  int const modus = (  invert_modus
                     ? -rating->value()
                     : rating->value());

#ifdef DKNOF
  CHECK_MODUS(modus);
#endif
  delete rating;
  if (   (modus == INT_MIN)
      || (modus == INT_MAX)) {
    return modus;
  } else {
    // the sum is added because else in the end when not all teams are known
    // special points (i.e. charlie) could be lost
    // note: when the game is not finished, 'modi_sum' < 1000
    return (modus + modi_sum / modi_num / 1000);
  }
} // int Gametree:rated_modus(Game& virt_game, unsigned tricks_to_calc) const

/**
 ** Returns the rated modi.
 ** From players of the same team, the modi is calculated positiv,
 ** for players of the opposite team, the modi is calculated negative
 **
 ** @param      virt_game	the virtual game
 ** @param      tricks_to_calc	how many tricks to be calced
 **
 ** @return     the modi for the full trick
 **
 ** @author     Diether Knof
 **
 ** @version    0.7.3
 **/
int
Gametree::full_trick_modi(Game& virt_game, unsigned const tricks_to_calc) const
{
  counter += 1;

  // the correspoding virtual player to the ai
  Ai const& virt_ai
    = dynamic_cast<Ai const&>(virt_game.player(this->vgi().no()));

  try {
    virt_game.evaluatetrick();
  } catch (...) {
    throw;
  }

  if (tricks_to_calc > 1) {
    virt_game.tricks().push_back(new Trick(virt_game.player_current()));
    for (auto& p : virt_game.players())
      p->trick_open(virt_game.trick_current());
    int const modus = rated_modus(virt_game, tricks_to_calc - 1);
#ifdef DKNOF
    CHECK_MODUS(modus);
#endif
    return modus;
  }

  // game finished, the main interest is the final result...
  if (virt_game.tricks_remaining_no() == 0) {

    virt_game.finish();

    GameSummary const game_summary(virt_game);
    if (game_summary.winnerteam() == TEAM::NOTEAM) {
      return (GAME_POINTS_MULTIPLIER
              * (static_cast<int>(game_summary.points(virt_ai.team()))
                 - static_cast<int>(game_summary.points(opposite(virt_ai.team()))))
              / game_summary.bock_multiplier()
              + static_cast<int>(game_summary.trick_points(virt_ai.team())));
    } else { // if !(game_summary.winnerteam() == TEAM::NOTEAM)
      if (::maybe_to_team(game_summary.winnerteam()) == virt_ai.team()) {
        return (GAME_POINTS_MULTIPLIER
                * game_summary.points() / game_summary.bock_multiplier()
                + static_cast<int>(game_summary.trick_points(virt_ai.team())));
      } else {
        return (-GAME_POINTS_MULTIPLIER
                * game_summary.points() / game_summary.bock_multiplier()
                + static_cast<int>(game_summary.trick_points(virt_ai.team())));
      }
    } // if !(game_summary.winnerteam() == TEAM::NOTEAM)
  } // if (virt_game.tricks_remaining_no() == 0)

  // some tricks has been calculated, get the modus from them.
  //#define DKNOF_TEST
#ifdef DKNOF_TEST
  bool DEBUG_OUT = false;
  if (  (  (virt_game.trick_current_no() == 11)
         && (   (virt_game.trick_current().card(0)
                 == Card(Card::HEART, Card::KING))
             && (virt_game.trick_current().card(1)
                 == Card(Card::CLUB, Card::TEN))
             && (virt_game.trick_current().card(2)
                 == Card(Card::HEART, Card::TEN))
            ) )
     )
    DEBUG_OUT = true;
  if (DEBUG_OUT) {
    cout << "--\n";
    cout << virt_game.trick(9).card_of_player(virt_ai)
      << ", " << virt_game.trick_current().card_of_player(virt_ai) << endl;
    cout << virt_game.trick_current_no() << endl;
  }
#endif

  int modus = 0;
  // sum up the modi for all tricks
  for (unsigned t = this->vgi().game().trick_current_no();
       t < virt_game.trick_current_no();
       t++) {
    modus += TrickWeighting::modi(this->vgi(),
                                  virt_game.trick(t),
                                  virt_ai.team(),
                                  virt_game.trick(t).card_of_player(virt_ai));
#ifdef DKNOF_TEST
    if (DEBUG_OUT) {
      cout << virt_game.trick(t);
      cout << "modus = "
        << TrickWeighting::modi(this->vgi(),
                                virt_game.trick(t),
                                virt_ai.team(),
                                virt_game.trick(t).card_of_player(virt_ai))
        << " (" << virt_game.trick(t).winnerteam() << ")" << endl;
    }
#endif
  }

  // copied from 'WVirtualGames'
  // add some points, if the own team is in the back,
  if (virt_game.real_trick_current_no() < virt_game.trickno() - 1) {
    modus += TrickWeighting::backhand(this->vgi(),
                                      virt_game.trick_current().card_of_player(virt_ai),
                                      virt_game);
  } // if (virt_game.trick_current_no() < virt_game.trickno() - 1)

#ifdef DKNOF
  CHECK_MODUS(modus);
#endif
  return modus;
} // int Gametree::full_trick_modi(Game& virt_game, unsigned tricks_to_calc) const
