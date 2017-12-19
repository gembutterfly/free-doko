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

#include "w_monte_carlo.h"


#include "VirtualGamesInterface.h"
#include "trickweighting.h"
#include "heuristics.h"
#include "ai.h"
#include "../ai.random/ai.h"
#include "../../party/rule.h"
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
 ** @param      vgi            the vgi
 ** @param      future_limit   the future limit
 ** @param      rating_type    the rating type
 **
 ** @return     -
 **/
WMonteCarlo::WMonteCarlo(VirtualGamesInterface const& vgi,
                         AiType const ai_type,
                         unsigned const future_limit,
                         Rating::Type const rating_type) :
  vgi(&vgi),
  ai_type(ai_type),
  future_limit(future_limit),
  rating_type(rating_type)
{ }

/**
 ** -> result
 **
 ** @param      -
 **
 ** @return     the card that leads to the most points
 **/
Card
WMonteCarlo::best_card()
{
  // calculate the weightings
  auto const weighting = this->weightings_calc();

  // search the best valid card
  auto best_card = card_with_max_weighting(weighting);

  auto const& game = this->vgi->game();
  auto const& hand = this->vgi->hand();

  // improve the card
  if (   (   best_card.isdolle() 
          || best_card.isswine() ) 
      && hand.highest_card()
      && game.trick_current().isstartcard() ) {
    best_card = hand.highest_card();
  }
  if   (   best_card.istrump()
        && game.trick_current().isstartcard() ) {
    auto const c = Heuristics::lowest_free_card(this->vgi->hand());
    if (c) {
      best_card = c;
    } else {
      best_card = Heuristics::lowest_trump_card(this->vgi->hand());
    }
  }

  // optimize the result a bit
  if (   (best_card.value() >= 10)
      && best_card.istrump()
      && !best_card.is_special())
    return Heuristics::lowest_trump_card(this->vgi->hand());

  return Heuristics::optimized_card_to_play(best_card,
                                            game.trick_current(),
                                            dynamic_cast<Ai const&>(this->vgi->player()));
} // Card WMonteCarlo::best_card()

/**
 ** -> result
 **
 ** @param      -
 **
 ** @return     the cards to check
 **/
HandCards
WMonteCarlo::cards() const
{
  auto const& game = this->vgi->game();
  auto cards = this->vgi->hand().validcards(game.trick_current());
  // set the cards to the valid cards
  if (this->ai_type == AiType::MONTE_CARLO_JAB_OR_SERVE) {
    if (game.trick_current().isempty()) {
      // remove some special cards
      if (   !game.is_solo()
          && cards.contains(Card::FOX)
          && (cards.size() > 1))
        cards.remove(Card::FOX);
      if (   !game.is_solo()
          && cards.contains(Card::DIAMOND_TEN)
          && (cards.size() > 1))
        cards.remove(Card::DIAMOND_TEN);
      if (   game.rule()(Rule::DOLLEN)
          && cards.contains(Card::DOLLE)
          && (cards.size() > 1))
        cards.remove(Card::DOLLE);
    } else {
      cards = HandCards();
      auto const& ai = dynamic_cast<Ai const&>(this->vgi->player());
      {
        auto card = HandCard(this->vgi->hand(),
                             Heuristics::jabbing_card(game.trick_current(), ai));
        if (   !game.is_real_solo()
            && (game.trumptricks_no() == 1)
            && (game.trick_current().startcard().istrump())
            && card.less(Card::DIAMOND_JACK)) {
          card = this->vgi->hand().same_or_higher_card(Card::DIAMOND_JACK);
        }
        if (   card
            && !(   this->vgi->same_team(game.trick_current().winnerplayer())
                 // ToDo: check whether the opposite playes behind can have a card between the winnercard and 'card'
                 && (card == game.trick_current().winnercard())) )
          cards.add(card);
      }
      {
        auto const card = HandCard(this->vgi->hand(),
                                   Heuristics::lowest_serving_card(game.trick_current(), ai));
        if (   card
            && !cards.contains(card))
          cards.add(card);
      }
      if (cards.size() == 0)
        cards = this->vgi->hand().validcards(game.trick_current());
    } // if (game.trick_current().isempty())
  } // if (this->ai_type == AiType::MONTE_CARLO_JAB_OR_SERVE) {

  if (cards.size() == 1)
    return cards;

  // never take a dolle as first card in the rule 'second over first'
  if (   game.rule()(Rule::DOLLEN_SECOND_OVER_FIRST)
      && game.trick_current().isstartcard()
      && (cards.contains(game.dolle()))
      && this->vgi->cards_information().remaining_others(game.dolle())
     ) {
    cards.remove(game.dolle());
  }

  if (cards.size() == 1)
    return cards;

  // never take a fox but in the last three tricks
  if (   (game.tricks_remaining_no() > 3)
      && cards.contains(game.fox())) {
    cards.remove(game.fox());
  }

  return cards;
} // HandCards WMonteCarlo::cards() const

/**
 ** calculates the weightings
 ** and writes the values into the vector 'weighting_p;
 **
 ** @param      -
 **
 ** @return     the weighting
 **/
map<HandCard, WMonteCarlo::Weighting>
WMonteCarlo::weightings_calc()
{
  auto weighting = map<HandCard, Weighting>();
  auto const& game = this->vgi->game();
  auto const cards = this->cards();

  if (cards.size() == 1) {
    weighting[cards.front()] = 0;
    return weighting;
  }

  unsigned counter = 0;

  if (!game.isvirtual())
    ::ui->update();

#ifdef SAVE_RUNTIME
  // for measuring the runtime
  ofstream ostr("MonteCarlo.time", ios::app);
  clock_t time_begin = 0;
  if (!game.isvirtual())
    time_begin = clock();
#endif

  if (!game.isvirtual())
    ::ui->set_max_progress(cards.size());

  try {
    int cn = 0;
    for (auto const& c : cards) {
      ::ui->ai_test_card(c, this->vgi->no());
      auto rating = Rating::create(this->rating_type);
      if (INFO_W_VIRTUAL_GAMES)
        if (!game.isvirtual())
          cout << "MonteCarlo: calculating card " << c << '\n';
      auto const i_max = (this->future_limit
                          / cards.size()
                          / (game.trickno() - game.trick_current_no() + 1));
      for (unsigned i = 0; i < i_max; ++i) {
        ::ui->ai_test_card(c, this->vgi->no());
        ::ui->update();

        // create the players
        vector<unique_ptr<Player>> ai_virt;
        for (size_t p = 0; p < game.playerno(); ++p) {
          ai_virt.push_back(make_unique<AiRandom>());
          auto& ai_virtual = *ai_virt.back();
          ai_virtual.set_hand(this->vgi->handofplayer(p) );
          DEBUG_ASSERTION((ai_virtual.hand().cardsnumber() + 1
                           // hack (if the player has already played a card in the trick)
                           >= game.tricks_remaining_no()),
                          "WMonteCarlo::init():\n"
                          "  the hand of the virtual player " << p
                          << " is too small: "
                          << ai_virtual.hand().cardsnumber()
                          << " < " << game.tricks_remaining_no()
                          << ":\n"
                          << ai_virtual.hand());
        }
        // create the virtual game
        Game game_virt(this->vgi->game(), ai_virt);
        for (unsigned p = 0; p < game.playerno(); ++p)  {
          if (this->vgi->guessedteamofplayer(p) == TEAM::UNKNOWN)
            game_virt.player(p).set_team(TEAM::opposite(this->vgi->team()));
          else
            game_virt.player(p).set_team(this->vgi->guessedteamofplayer(p));
          game_virt.set_teaminfo(game_virt.player(p),
                                 game_virt.player(p).team());
        }


        game_virt.play_card(c);

        try {
          this->play_till_end(game_virt);
          auto const w = this->weighting(game_virt, c);
          rating->add(w);
          ::ui->ai_card_weighting(w);
          counter += game.trickno() - game.trick_current_no();
        } catch (InvalidGameException e) {
          // if the game is invalid, don't take the card
          DEBUG_CAUGHT();
        } // try

        if (!game.isvirtual()) {
          ::ui->set_progress(cn + i / static_cast<double>(i_max));
        } // if (!game.isvirtual())
      } // for (i)

      weighting[c] = rating->value();
      if (INFO_W_VIRTUAL_GAMES)
        if (!game.isvirtual())
          cout << "weighting " << c << ": " << weighting[c] << "\n";

      ::ui->ai_card_weighting(weighting[c]);
      cn += 1;
      ::ui->set_progress(cn);
    } // for (c : cards)

  } catch (...) {
    if (!game.isvirtual())
      ::ui->finish_progress(false);
    throw;
  }
  if (!game.isvirtual())
    ::ui->finish_progress();

#ifdef SAVE_RUNTIME
  if (!game.isvirtual())
    if (counter >= 2000) {
      auto const used_time = ((clock() - time_begin)
                              / (CLOCKS_PER_SEC / 1000));
      ostr << setw(8) << counter
        << "\t" << setw(8) << used_time
        << "\t" << setw(8) << (used_time * 1000 / counter) 
        << endl;
    } // if (counter >= 2000)
#endif // #ifdef SAVE_RUNTIME

  return weighting;
} // map<HandCard, Weighting> WMonteCarlo::weightings_calc()


/** Returns the card with the maximal weighting
 **
 ** @param   weighting   weightings of the cards
 **
 ** @return  card with the highest weighting
 **/
HandCard
WMonteCarlo::card_with_max_weighting(map<HandCard, Weighting> const& weighting)
{
  if (weighting.empty())
    return {};
  auto k = &(*weighting.begin());
  for (auto const& i : weighting)
    if (i.second > k->second)
      k = &i;
  return k->first;
} // HandCard WMonteCarlo::card_with_max_weighting(std::map<HandCard::Card, Weighting> weighting)

/** Play the game till the and
 **
 ** @param    game_virt   game to finish
 **
 ** @return   -
 **/
void
WMonteCarlo::play_till_end(Game& game_virt) const
{
  { // finish the current trick
    while(!game_virt.trick_current().isfull()) {
      game_virt.nextplayer();
    }
    game_virt.evaluatetrick();
  } // finish the current trick

  { // play tricks till the game ends
    while( (game_virt.trick_current_no() < game_virt.trickno()) 
          && game_virt.tricks_remaining_no() != 0) {
      game_virt.tricks().push_back(new Trick(game_virt.player_current()));
      while(!game_virt.trick_current().isfull())
        game_virt.nextplayer();
      game_virt.evaluatetrick();
    } // while(game not finished)
  } // play tricks till the game ends
  game_virt.finish();
} // void WMonteCarlo::play_till_end(Game& game_virt) const

/** Returns the weighting for the game
 **
 ** @param    game_virt   game to weight
 ** @param    card        card that is played
 **
 ** @return   weighting for the game
 **/
WMonteCarlo::Weighting
WMonteCarlo::weighting(Game const& game_virt, HandCard const& card) const
{
  Weighting w = 0;
  // finished with creating tricks,
  // now calc the modi for each trick
  for (unsigned t = this->vgi->game().trick_current_no();
       t < game_virt.trick_current_no();
       t++) {
    // add the modi to the weighting
    w += TrickWeighting::modi(*this->vgi, game_virt.trick(t),
                              game_virt.player(this->vgi->no()).team(),
                              game_virt.trick(t).card_of_player(game_virt.player(this->vgi->no()) ) );
  }

  // add some points, if the own team is in the back,
  if (game_virt.trick_current_no() < game_virt.trickno() - 1)
    w += TrickWeighting::backhand(*this->vgi, card, game_virt);

  // game finished, the main reason is the final result...
  auto const game_summary = GameSummary(game_virt);
  if (game_summary.winnerteam() == this->vgi->team())
    w += game_summary.points() * 10000;
  else
    w -= game_summary.points() * 10000;
  return w;
} // Weighting WMonteCarlo::weighting(Game game, HandCard card) const
