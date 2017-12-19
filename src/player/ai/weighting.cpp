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

#include "weighting.h"

#include "w_virtual_games.h"
#include "gametree.h"
#include "w_monte_carlo.h"
#include "heuristics.h"
#include "cards_information.h"
#include "cards_information.of_player.h"

#include "../../party/party.h"
#include "../../game/game.h"
#include "../../card/trick.h"
#include "../../ui/ui.h"
#include "../../misc/setting.h"
#include "../../utils/string.h"
#ifdef CHECK_RUNTIME
#include "../../runtime.h"
#endif

#ifdef USE_THREADS
#include <pthread.h>
#include <list>
#endif

// whether to print the weighting information
#ifndef RELEASE
#ifdef BENDERS
#define INFO_WEIGHTING
#endif
#endif

#ifdef USE_THREADS

class WeightingThreadData {
  public:
    // constructor
    WeightingThreadData(Weighting const& weighting, Card const& card) :
      weighting(&weighting),
      card(card),
      card_weighting(INT_MIN),
      finished(false)
  { }
    // destructor
    ~WeightingThreadData()
    { }

  public:
    // the corresponding gametree class
    Weighting const* const weighting;
    // the played card
    Card card;
    // calculated weighting
    int card_weighting;
    // whether the thread is finished
    bool finished;

  private: // unused
    WeightingThreadData();
    WeightingThreadData(WeightingThreadData const&);
    WeightingThreadData& operator=(WeightingThreadData const&);
}; // class GametreeData

static void* Weighting_thread_routine(void* arg);

#endif // #ifdef USE_THREADS

/**
 ** -> result
 **
 ** @param     aitype   the ai type
 ** @param     ai       the corresponding ai
 **
 ** @return    the best card according to the weighting of 'aitype'
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.5
 **/
Card
Weighting::best_card(AiType const aitype, Ai const& ai)
{
#ifdef CHECK_RUNTIME
  auto const ssp = (  !ai.game().isvirtual()
                    ? ::runtime["ai choosebestcard"].start_stop_proxy()
                    : nullptr);
#endif

  if (!ai.game().isvirtual())
    ::ui->set_busy();

  Card card;
#ifndef RELEASE
  Ai* ai_bak = ai.clone();
#endif

  try {
    switch(aitype) {
    case AiType::NO_CHOOSEBESTCARD:
      break;
    case AiType::RANDOM:
      {
        HandCards validcards = ai.hand().validcards(ai.game().trick_current());
#ifdef OUTDATED
        card = validcards.card(RAND(validcards.cardsnumber()));
#endif
        card = validcards.card(0);
      }
      break;
    case AiType::VIRTUAL_GAMES:
      card = WVirtualGames(ai).best_card();
      break;
    case AiType::GAMETREE:
    case AiType::GAMETREE_WITH_HEURISTICS:
    case AiType::GAMETREE_FOR_TEAM:
      card = Gametree(ai).best_card();
      break;
    case AiType::MONTE_CARLO:
    case AiType::MONTE_CARLO_JAB_OR_SERVE:
      card = WMonteCarlo(ai, aitype,
                         ai.future_limit(), ai.rating()).best_card();
      break;

    default:
      DEBUG_ASSERTION(false,
                      "Weighting::choosebestcard():\n"
                      "  no aitype"
                     );
      break;
    } // switch(aitype())
    if (::game_status != GAMESTATUS::GAME_PLAY) {
      ::ui->set_not_busy();
      return Card();
    }
  } catch (InvalidGameException const& e) {
    if (ai.game().isvirtual())
      throw;
#ifndef DEBUG_ASSERT
    cerr << "Weighting: invalid game\n"
      << "  line " << __LINE__ << '\n'
      << "  exception: " << e.what() << '\n'
      << "  taking random card"
      << endl;
    {
      HandCards validcards = ai.hand().validcards(ai.game().trick_current());
      card = validcards.card(0);
    }
    DEBUG_CAUGHT();
#endif
    DEBUG_ASSERTION(false,
                    "Weighting::choosebestcard(ai)\n"
                    "  no valid game found\n"
                    "  exception: " << e.what());
  } catch (...) { // try
    if (!ai.game().isvirtual())
      ::ui->set_not_busy();
    throw; 
  } // try

  DEBUG_ASSERTION( (aitype == AiType::NO_CHOOSEBESTCARD)
                  || !card.is_empty(),
                  "Weighting::choosebestcard()\n"
                  "  '" << aitype << "' returns no card");

#ifndef RELEASE
  DEBUG_ASSERTION(ai.isequal(*ai_bak),
                  "Weighting::choosebestcard():\n"
                  "  ai has changed!\n"
                  "  type = " << aitype);
  delete ai_bak;
#endif

  if (!ai.game().isvirtual())
    ::ui->set_not_busy();

  return card;
} // static Card Weighting::best_card(AiType aitype, Ai ai)

/**
 ** constructor
 **
 ** @param	ai   the ai
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.5
 **/
Weighting::Weighting(Ai const& ai) :
  ai_(ai)
{ }

/**
 ** destructor
 **
 ** @param	-
 **
 ** @return	-
 **
 ** @author	Borg Enders
 **
 ** @version	0.7.0
 **/
Weighting::~Weighting()
{ }

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	the ai interface
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.5
 **/
VirtualGamesInterface const&
Weighting::vgi() const
{
  return this->ai();
} // VirtualGamesInterface const& Weighting::vgi() const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	the best card according to the weighting
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 **
 ** @version	0.7.5
 **/
Card
Weighting::best_card() const
{
#ifdef USE_THREADS
  if (::setting(Setting::THREADS_MAX) > 1)
    if (!this->vgi().game().isvirtual())
      return this->best_card_threads();
#endif

  // 'local_hand' is needed because the cards of 'valid_cards'
  // need a reference hand.
  Hand const local_hand
    = this->vgi().handofplayer(this->vgi().player());
  HandCards const valid_cards
    = local_hand.validcards(this->vgi().game().trick_current());

#ifdef SAVE_RUNTIME
  // for measuring the runtime
  ofstream ostr("gametree.time", ios::app);
  clock_t const time_begin = clock();
#endif

  HandCard best_card(local_hand);
  HandCards best_cards;
  int weighting_best = INT_MIN;

  try {
    if (!this->vgi().game().isvirtual()) {
      ::ui->set_max_progress(valid_cards.size());
    }

    unsigned i = 0;
    // now calculate for all cards the weighting
    for (HandCards::const_iterator c = valid_cards.begin();
         c != valid_cards.end();
         ++c) {
      HandCard const& card = *c;
      ::ui->ai_test_card(card, this->vgi().no());
      int const card_weighting = this->weighting(card);
      ::ui->ai_card_weighting(card_weighting);

#ifdef INFO_WEIGHTING
      if (!this->vgi().game().isvirtual())
        cout << setw(12) << card << ": " << setw(8) << card_weighting << endl;
#endif

      // this is needed when 'weighting' is called from 'virtual games'
      if (this->vgi().game().isvirtual()
          && (   (card_weighting == INT_MIN)
              || (card_weighting == INT_MAX))
         )
        DEBUG_THROW( InvalidGameException, InvalidGameException() );

#ifdef RELEASE
      DEBUG_ASSERTION((   (card_weighting != INT_MIN)
                       && (card_weighting != INT_MAX)),
                      "Weighting::best_card()\n"
                      "  No valid game found.\n"
                      "  Card: " << card);
#else
      if (   (card_weighting == INT_MIN)
          || (card_weighting == INT_MAX)) {
        cerr << "Weighting-error\n";

        for (unsigned p = 0; p < this->vgi().game().playerno(); ++p)
          cerr << "player " << p << ":\n"
            << this->vgi().handofplayer(p) << endl;
        DEBUG_ASSERTION((   (card_weighting != INT_MIN)
                         && (card_weighting != INT_MAX)),
                        "Weighting::best_card()\n"
                        "  No valid game found.\n"
                        "  Card: " << card);
      }
#endif

      // test, whether the card is better:
      // - the card weighting is better
      // - the card weighting is equal take the lower card
      // - the card weighting is equal, take the color card with the greater/lower value
      //                       if the trick goes to the own/opposite team so far
      if (card_weighting > weighting_best) {
        weighting_best = card_weighting;
        best_cards.clear();
        best_cards.push_back(*c);
      } else if (card_weighting == weighting_best) {
        best_cards.push_back(*c);
      }

      i += 1;
      if (!this->vgi().game().isvirtual()) {
        ::ui->add_progress(static_cast<double>(i)
                           - ::ui->progress() * valid_cards.size());
      } // if (!this->vgi().game().isvirtual())
      if (this->vgi().game().isvirtual()
          && this->vgi().game().played_cards_no()
          == ::party.game().played_cards_no() + 1) {
        // first depth in virtual games
        ::ui->add_progress(1.0 / (valid_cards.size() + 1));
      } // if (!this->vgi().game().isvirtual())
    } // for (c \in valid_cards)

    best_card = Heuristics::choose_best_prefiltered_card(this->vgi().game().trick_current(), this->ai(), local_hand, best_cards);
    DEBUG_ASSERTION(best_cards.numberof(best_card) > 0,
                    "Weighting::best_card()\n"
                    "  best card not in the cards:"
                    << best_card << " (" << best_cards.size() << ")\n"
                    << best_cards);
  } catch (...) {
    if (!this->vgi().game().isvirtual())
      ::ui->finish_progress(false);

    throw;
  } // try
  if (!this->vgi().game().isvirtual())
    ::ui->finish_progress();

#ifdef SAVE_RUNTIME
  if (counter >= 2000) {
    unsigned const used_time = ((clock() - time_begin)
                                / (CLOCKS_PER_SEC / 1000));
    ostr << setw(8) << counter
      << "\t" << setw(8) << used_time
      << "\t" << setw(8) << (used_time * 1000 / counter) 
      << endl;
  } // if (counter >= 200)
#endif // #ifdef SAVE_RUNTIME

  DEBUG_ASSERTION(!best_card.is_empty(),
                  "Weighting::best_card():\n"
                  "  no best card found.\n"
                  << "\n"
                  //<< "Hand:\n"
                  //<< this->vgi().hand()
                 );

  return best_card;
} // Card Weighting::best_card() const


#ifdef USE_THREADS

/**
 ** calculates the best card (with thread support)
 **
 ** @param      -
 **
 ** @return     the best card to play
 **
 ** @author     Diether Knof
 **
 ** @version    0.7.5
 **
 ** @todo       use new checking of 'best card'
 **/
Card
Weighting::best_card_threads() const
{
  // change the ui so that there is no thread clashing
  UI* main_ui = ::ui;
  ::ui = UI::new_(UI_TYPE::DUMMY);

  // the thread ids and the corresponding data
  std::list<pair<pthread_t, WeightingThreadData*> > threads;

  Game const& game = this->vgi().game();

  // 'local_hand' is needed because the cards of 'valid_cards'
  // need a reference hand.
  Hand const local_hand
    = this->vgi().handofplayer(this->vgi().no());
  HandCards const valid_cards
    = local_hand.validcards(this->vgi().game().trick_current());

#ifdef SAVE_RUNTIME
  // for measuring the runtime
  ofstream ostr("weighting.time", ios::app);
  clock_t const time_begin = clock();
#endif

  HandCard best_card(local_hand);
  HandCards best_cards;
  int weighting_best = INT_MIN;

  try {

    // now calculate the weighting for all cards
    for (HandCards::const_iterator c = valid_cards.begin();
         c != valid_cards.end();
         ++c) {
      HandCard const& card = *c;

      { // check that the number of threads does not exceed 'THREADS'

        // the number of active threads
        unsigned threads_num = 0;
        do { // while (the number of threads exceeds 'THREADS')
          if (threads_num > 0)
            main_ui->wait();
          // count the number of active threads
          threads_num = 0;
          for (vector<pair<pthread_t, WeightingThreadData*> >::iterator
               t = threads.begin();
               t != threads.end();
               ++t) {
            if (!t->second->finished)
              threads_num += 1;
          } // for (t \in threads)
        } while (threads_num >= ::setting(Setting::THREADS_MAX));
      } // check that the number of threads does not exceed 'THREADS_MAX'

      { // create a new thread
        // the thread id
        pthread_t thread;
        // the data
        WeightingThreadData* data = new WeightingThreadData(*this, card);
        int const error = pthread_create(&thread, NULL,
                                         Weighting_thread_routine, data);
        if (error) {
          main_ui->error("error creating a thread: "
                         + DK::Utils::String::to_string(error));
        }
        threads.push_back(pair<pthread_t, WeightingThreadData*>(thread, data));
      } // create a new thread
    } // for (c \in valid_hand.cards())

    { // wait for all remaining threads
      while (!threads.empty()) {
        main_ui->wait();

        // check whether a thread is finished
        for (vector<pair<pthread_t, WeightingThreadData*> >::iterator
             t = threads.begin();
             t != threads.end();
            ) {
          if (t->second->finished) {
            pthread_join(t->first, NULL);
            { // fetch the thread data
              int const card_weighting = t->second->card_weighting;
              Card const& card = t->second->card;

              main_ui->ai_test_card(card, this->vgi().no());
              main_ui->ai_card_weighting(card_weighting);

              // test, whether the card is better
              if (card_weighting > weighting_best) {
                weighting_best = card_weighting;
                best_cards.clear();
                best_cards.push_back(HandCard(local_hand, card));
              } else if (card_weighting == weighting_best) {
                best_cards.push_back(HandCard(local_hand, card));
              }
            } // fetch the thread data

            delete t->second;
            t = threads.erase(t);
          } else { // if !(t->second->finished)
            t++;
          }
        } // for (t \in threads)
      } // while (!threads.empty())
    } // wait for all remaining threads

    best_card = Heuristics::choose_best_prefiltered_card(game.trick_current(), this->ai(), local_hand, best_cards);
  } catch (...) {

    // cancel all remaining threads
    while (!threads.empty()) {
      pthread_cancel(threads.front().first);
      pthread_join(threads.front().first, NULL);
      delete threads.front().second;
      threads.pop_front();
    } // while (!threads.empty())

    // set the ui back
    delete ::ui;
    ::ui = main_ui;

    throw;
  } // try

  // set the ui back
  delete ::ui;
  ::ui = main_ui;

#ifdef SAVE_RUNTIME
  if (counter >= 2000) {
    unsigned const used_time = ((clock() - time_begin)
                                / (CLOCKS_PER_SEC / 1000));
    ostr << setw(8) << counter
      << "\t" << setw(8) << used_time
      << "\t" << setw(8) << (used_time * 1000 / counter) 
      << endl;
  } // if (counter >= 200)
#endif // #ifdef SAVE_RUNTIME

  DEBUG_ASSERTION(!best_card.is_empty(),
                  "Weighting::best_card_threads():\n"
                  "  no best card found.\n"
                  << "\n"
                  //<< "Hand:\n"
                  //<< this->vgi().hand()
                 );

  return best_card;
} // Card Weighting::best_card_threads() const

/**
 ** calculates the weighting for the given game
 **
 ** @param     arg   thread data
 **
 ** @return    NULL
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.5
 **/
void*
Weighting_thread_routine(void* arg)
{
  WeightingThreadData* data = static_cast<WeightingThreadData*>(arg);

  data->card_weighting = data->weighting->weighting(data->card);

  data->finished = true;

  return NULL;
} // static void* Weighting_thread_routine(void* arg)

#endif // #ifdef USE_THREADS
