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
#include <fstream>

#include "w_virtual_games.h"

#include "VirtualGamesInterface.h"
#include "trickweighting.h"
#include "ai.h"
#include "cards_information.h"
#include "cards_information.of_player.h"
#include "team_information.h"
#include "heuristics.h"
#include "../../game/game.h"
#include "../../game/game_summary.h"
#include "../../game/exception.h"
#include "../../card/trick.h"
#include "../../misc/setting.h"
#include "../../ui/ui.h"

// whether to save the runtime
//#define SAVE_RUNTIME

#ifdef RELEASE
#undef SAVE_RUNTIME
#endif

/**
 ** constructor
 **
 ** @param      ai   the corresponding ai
 **
 ** @return     -
 **
 ** @author     Diether Knof
 **
 ** @version    0.7.5
 **/
WVirtualGames::WVirtualGames(Ai const& ai) :
  Weighting(ai),
  future_limit_(ai.future_limit()),
  end_depth_(0)
{
  this->calculate_end_depth();
} // WVirtualGames::WVirtualGames(Ai const& ai)

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
WVirtualGames::~WVirtualGames()
{ }

/**
 ** calculates the maximal depth of tricks,
 ** so that no more of 'future limit' tricks are considered
 **
 ** @param      -
 **
 ** @return     -
 **
 ** @author     Borg Enders
 **
 ** @version    0.7.5
 **/
void
WVirtualGames::calculate_end_depth()
{
  // #combination for all tricks
  unsigned long int alltricks = 1;
  // #combination for one trick
  unsigned long int onetrick = 1;

  Trick const& trick = this->vgi().game().trick_current();

  // count the number of card-combinations for the current trick
  for (unsigned i = trick.actcardno();
       i < this->vgi().game().playerno();
       i++)
    onetrick *= this->vgi().handofplayer(trick.player_of_card(i)
                                        ).validcards(trick).cardsnumber();

  alltricks *= onetrick;

  // count the number of card-combinations for the other tricks
  // (since I don't know what color is played, I take all cards of the hand)
  for (this->end_depth_ = 1;
       // '<' because also the current trick is counted in tricks_remaining_no
       (this->end_depth_ < this->vgi().game().tricks_remaining_no());
       this->end_depth_ += 1){

    onetrick = 1;
    for (unsigned i = 0; i < this->vgi().game().playerno(); i++) {
#ifndef POSTPONED
      if (this->vgi().game().trick_current().cardno_of_player(this->vgi().game().player(i))
          >= this->vgi().game().trick_current().actcardno())
        // the player still has to play a card in the current trick
        onetrick *= (this->vgi().handofplayer(this->vgi().game().player(i)).cardsnumber()
                     - this->end_depth_);
      else
        // the player has already played a card, so he has one card less 
        // on the hand
        onetrick *= (this->vgi().handofplayer(this->vgi().game().player(i)).cardsnumber()
                     - (this->end_depth_ - 1));

      DEBUG_ASSERTION((onetrick > 0),
                      "WVirtualGames::end_depth_calculate():\n"
                      "  onetrick == 0\n"
                      << "cardsnumber = " << this->vgi().handofplayer(this->vgi().game().player(i)).cardsnumber() << '\n'
                      << "end_depth = " << this->end_depth_ << '\n'
                      << "trickno = " << this->vgi().game().trick_current_no() << '\n'
                      << "remaining trickno = " << this->vgi().game().tricks_remaining_no() << '\n'
                     );
#else
      onetrick *= this->vgi().hand().cardsnumber() - this->end_depth_;
#endif
    }

    if (alltricks < this->future_limit() / onetrick) {
      alltricks *= onetrick; 
    } else  {
      break;
    }
  } // for (this->end_depth_)

  this->end_depth_ = min(this->end_depth(),
                              ((this->vgi().last_trick_to_calculate()
                                >= this->vgi().game().trick_current_no())
                               ? (this->vgi().last_trick_to_calculate()
                                  - this->vgi().game().trick_current_no())
                               : 0));

#ifdef POSTPONED
  this->end_depth_ = max(unsigned(2), this->end_depth());
#endif


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
WVirtualGames::weighting(Card const& card_) const
{
  // the weighting
  int weighting = 0;

  // create the virtual game
  Game* const game_virt_p = this->vgi().create_virtual_game();
  Game& game_virt = *game_virt_p;

  // the players
  vector<Player*>& ai_virt = game_virt.players();
  // the copy of the original ai
  Ai* ai_copy = static_cast<Ai*>(ai_virt[this->vgi().no()]);
  // the card in the virtual game
  HandCard const card(game_virt.player_current().hand(), card_);

  { // play the card 'card'
    // make swines announcements
    if (   card.possible_swine()
        && game_virt.swines_announcement_valid(game_virt.player_current())) {
      game_virt.swines_announce(game_virt.player_current());
    }
    if (   card.possible_hyperswine()
        && game_virt.hyperswines_announcement_valid(game_virt.player_current()))
      game_virt.hyperswines_announce(game_virt.player_current());

    // play the card
    // code copied from 'Game::nextcard()'
    game_virt.player_current().hand().playcard(card);
    game_virt.trick_current() += card;
    game_virt.teaminfo_update();
    game_virt.player_current_
      = &game_virt.player_following(game_virt.player_current());
    for (vector<Player*>::iterator p = game_virt.players_begin();
         p != game_virt.players_end();
         p++) {
      (*p)->card_played(card);
    }
  } // play the card 'card'

  try {
    // the other virtual players play a card
    while (!game_virt.trick_current().isfull()) {

      { // update the cards information for all players
        CardsInformation& cards_information(ai_copy->cards_information());

        // save the hands, because 'cards_information' changes, when the hand of 'ai_copy' is set
        vector<Hand*> hands;
        for (vector<Player*>::iterator player
             = game_virt.players_begin();
             player != game_virt.players_end();
             player++) {
          hands.push_back(new Hand(ai_copy->handofplayer(**player)));
#ifdef WORKAROUND
          // as long as the cards information uses much processing time
          ::ui->update();
#endif
        } // for (player)

        unsigned p = 0;
        for (vector<Player*>::iterator player
             = game_virt.players_begin();
             player != game_virt.players_end();
             ++player, ++p) {
          cards_information.set_hand(**player, *hands[p]);
        } // for (player)

        p = 0;
        for (vector<Player*>::iterator ai = ai_virt.begin();
             ai != ai_virt.end();
             ++ai, ++p) {
          static_cast<Ai*>(*ai)->set_cards_information(cards_information);
          (*ai)->set_hand(cards_information.estimated_hand(p));
#ifdef WORKAROUND
          // as long as the cards information uses much processing time
          ::ui->update();
#endif
        } // for (ai)
        for (vector<Hand*>::iterator hand = hands.begin();
             hand != hands.end();
             ++hand)
          delete *hand;

      } // update the cards information for all players

      DEBUG_ASSERTION((game_virt.player_current().hand().cardsnumber() > 0),
                      "WVirtualGames::weightings_calc():\n"
                      "  current player " << game_virt.player_current().name()
                      << " has an empty hand");
      try {
#ifdef DKNOF
        game_virt.player_current().self_check();
#endif
        game_virt.nextplayer();
      } catch (InvalidGameException const& e) {
        // ToDo: fix cards information
        // or:   remove last played card,
        //       remove it from the hand of the player
        //       and try again
        //       This is not possible, because the information is missing here. -> possible in gametree
        cerr << "WVirtualGames: invalid game\n"
          << "  line " << __LINE__ << '\n'
          << "  ignoring card"
          << endl;
        throw;
      } // try
    } // while (!game_virt.trick_current().isfull())

    // first trick is full
    game_virt.evaluatetrick();


    // play tricks till the end depth
    for (unsigned t = 1;
         ((t < this->end_depth())
          && (game_virt.trick_current_no() < game_virt.trickno()));
         t++) {
      if (INFO_W_VIRTUAL_GAMES)
        if (!this->vgi().game().isvirtual())
          cout << "  trick depth " << t
            << " (" << this->end_depth() - 1 << ")" << endl;


      // set the future limit of the ai, so that they don't calculate too much
      for (vector<Player*>::iterator ai = ai_virt.begin();
           ai != ai_virt.end();
           ai++)
        static_cast<Ai*>(*ai)->set_last_trick_to_calculate(game_virt.trick_current_no()
                                                           + (this->end_depth()
                                                              - t));

      // start a new trick
      game_virt.tricks().push_back(new Trick(game_virt.player_current()));
      for (vector<Player*>::iterator p = game_virt.players().begin();
           p != game_virt.players().end();
           p++) {
        (*p)->trick_open(game_virt.trick_current());
      }

      while(!game_virt.trick_current().isfull()) {

        { // update the cards information for all players
          CardsInformation& cards_information(ai_copy->cards_information());

          // save the hands, because 'cards_information' changes, when the hand of 'ai_copy' is set
          vector<Hand*> hands;
          for (vector<Player*>::iterator player
               = game_virt.players_begin();
               player != game_virt.players_end();
               player++) {
            hands.push_back(new Hand(ai_copy->handofplayer(**player)));
#ifdef WORKAROUND
            // as long as the cards information uses much processing time
            ::ui->update();
#endif
          } // for (player)

          unsigned p = 0;
          for (vector<Player*>::iterator player
               = game_virt.players_begin();
               player != game_virt.players_end();
               ++player, ++p) {
            cards_information.set_hand(**player, *hands[p]);
          } // for (player)

p = 0;
          for (vector<Player*>::iterator ai = ai_virt.begin();
               ai != ai_virt.end();
               ++ai, ++p) {
            static_cast<Ai*>(*ai)->set_cards_information(cards_information);
            (*ai)->set_hand(*hands[p]);
#ifdef WORKAROUND
            // as long as the cards information uses much processing time
            ::ui->update();
#endif
          } // for (ai)
          for (vector<Hand*>::iterator hand = hands.begin();
               hand != hands.end();
               ++hand)
            delete *hand;
        } // update the cards information for all players

        try {
          game_virt.nextplayer();
        } catch (InvalidGameException const& e) {
          // ToDo: fix cards information
          // or:   remove last played card,
          //       remove it from the hand of the player
          //       and try again
          //       This is not possible, because the information is missing here. -> possible in gametree
          cerr << "WVirtualGames: invalid game\n"
            << "  line " << __LINE__ << '\n'
            << "ignoring card\n"
            << endl;
          throw;
        } // try
      }
      game_virt.evaluatetrick();
    } // for (t < this->end_depth())

    // finished with creating tricks,
    // now calc the modi for each trick
    for (unsigned t = this->vgi().game().trick_current_no();
         t < game_virt.trick_current_no();
         t++) {
      // add the modi to the weighting
      weighting
        += TrickWeighting::modi(this->vgi(), game_virt.trick(t),
                                ai_copy->team(),
                                game_virt.trick(t).card_of_player(*ai_copy) );
    } // for (t < trick_current_no())

    if (game_virt.trick_current_no() < game_virt.trickno() - 1) {
      // add some points, if the own team is in the back,
      weighting += TrickWeighting::backhand(this->vgi(),
                                            card,
                                            game_virt);
    } // if (game_virt.trick_current_no() < game_virt.trickno() - 1)


    // game finished, the main reason is the final result...

    if( game_virt.tricks_remaining_no() == 0 ) {
      game_virt.finish();
      GameSummary const game_summary(game_virt);
      // the correspoding virtual player to the ai
      if (game_summary.winnerteam() == TEAM::NOTEAM) {
        weighting
          += 10000 * (game_summary.points(ai_copy->team())
                      - game_summary.points(opposite(ai_copy->team())));
      } else { // if !(game_summary.winnerteam() == TEAM::NOTEAM)
        if ( game_summary.winnerteam() == ai_copy->team())
          weighting += 10000 * game_summary.points();
        else
          weighting -= 10000 * game_summary.points();
      } // if !(game_summary.winnerteam() == TEAM::NOTEAM)
    }

#if 0
    // output of the modi
    if (this->vgi().game() == ::party.game()) {
      cout << this->hand().card(c)
        << ": "
        << weighting
        << endl;

    } // if (this->vgi().game() == ::party.gam())

#endif // output of the modi

  } catch (InvalidGameException const& e) {
    // if the game is invalid, don't take the card
    weighting = INT_MIN;
#ifndef RELEASE
    if (!this->vgi().game().isvirtual()) {

#ifdef DEBUG_ASSERT
#if 0
      cerr << "cards information\n";
      //cerr << static_cast<Ai const&>(game_virt.player_current()).cards_information() << endl;
      cerr << '\n';
      cerr << "hands\n";
      for (vector<Player*>::const_iterator p = game_virt.players_begin();
           p != game_virt.players_end();
           ++p)
        cerr << '\n' << (*p)->name() << '\n'
          << (*p)->hand();
      cerr << '\n';
      cerr << "previous trick = " << game_virt.trick_current_no() - 1 << endl;
      //cerr << game_virt.trick(game_virt.trick_current_no() - 1);
      cerr << '\n';
      cerr << "current trick = " << game_virt.trick_current_no() << endl;
      cerr << game_virt.trick_current();
      cerr << '\n';
      cerr << "Hand:\n";
      //cerr << game_virt.player_current().hand();
      cerr << '\n';
      cerr << "last heuristic: " << static_cast<Ai const&>(game_virt.player_current()).lastHeuristic_ << endl;
      cerr << '\n';
      cerr << __FILE__ << " line " << __LINE__ << '\n';
      cerr << "no valid game found for card '" << card << "'"
        << endl;
#endif
#endif
    }
    DEBUG_ASSERTION(this->vgi().game().isvirtual(),
                    "WVirtualGames::weightings_calc():\n"
                    "  no valid game found for card '" << card
                    << "'");
#endif
    DEBUG_CAUGHT();
  } catch (...) {
    // free the memory
    for (vector<Player*>::iterator ai = ai_virt.begin();
         ai != ai_virt.end();
         ai++)
      delete *ai;

    throw;
  } // try

  // free the memory
  for (vector<Player*>::iterator ai = ai_virt.begin();
       ai != ai_virt.end();
       ai++)
    delete *ai;

  if (INFO_W_VIRTUAL_GAMES)
    if (!this->vgi().game().isvirtual())
      cout << "weighting " << card << ": "
        << weighting << "\n";

  return weighting;
} // int WVirtualGames::weighting(Card card_) const
