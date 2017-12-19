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
 **********************************************************************/

#include <iterator>

#include "constants.h"
#include "party.h"

#include "../utils/string.h"

#include "rule.h"
#include "../game/game_summary.h"
#include "../player/player.h"
#include "../player/ai/ai.h"
#include "../player/human/human.h"
#include "../card/trick.h"

#include "../misc/setting.h"
#include "../misc/translations.h"

#include "../ui/ui.h"
#include "../ui/ui.wrap.h"

#ifdef CHECK_RUNTIME
#include "../runtime.h"
#endif

#include <ctime>


/**
 ** constructor:
 ** load the players from the private data directory
 **
 ** @param	-
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **/
Party::Party() :
  rule_(new Rule),
  players_(0),
  seed_first_(SEED_START),
  startplayer_first_(STARTPLAYER),
  real_seed_first_(SEED_START),
  real_startplayer_first_(STARTPLAYER),
  round_startgame_(),
  duty_soli_round_(UINT_MAX),
  points_(0),
  game_(),
  seed_(this->seed_first_),
  startplayer_(this->startplayer_first_),
  game_summaries_(),
  bock_multipliers_()
{
  // load the players from the private data directory
  // or take default ones
  for (unsigned p = 0;
       (this->playerno() < this->rule()(Rule::NUMBER_OF_PLAYERS));
       p++) {
    this->players_.push_back(Player::create((p == 0)
                                            ? Player::HUMAN
                                            : Player::AI));
  } // for (n; this->playerno() < this->rule()(Rule::NUMBER_OF_PLAYERS))

  return ;
} // Party::Party()

/**
 ** destructor
 **
 ** @param	-
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **/
Party::~Party()
{
  delete this->game_;
  delete this->rule_;

  return ;
} // Party::~Party()

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	the number of players
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **/
unsigned
Party::playerno() const
{
  return this->players().size();
} // unsigned Party::playerno() const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	number of humans in the party
 **
 ** @version	0.6.7
 **
 ** @author	Diether Knof
 **/
unsigned
Party::humanno() const
{
  // count the number of human players
  unsigned human_no = 0;
  for (vector<Player*>::const_iterator player = this->players().begin();
       player != this->players().end();
       player++)
    if ((*player)->type() == Player::HUMAN)
      human_no += 1;

  return human_no;
} // unsigned Party::humanno() const

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    human player if there exists exactly one,
 **            else 'NULL'
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.5
 **/
Human*
Party::human_player()
{
  if (this->humanno() != 1)
    return NULL;

  for (vector<Player*>::iterator p = this->players().begin();
       p != this->players().end();
       ++p)
    if ((*p)->type() == Player::HUMAN)
      return static_cast<Human*>(*p);

  return NULL;
} // Human* Party::human_player()

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    human player if there exists exactly one,
 **            else 'NULL'
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.5
 **/
Human const*
Party::human_player() const
{
  if (this->humanno() != 1)
    return NULL;

  for (vector<Player*>::const_iterator p = this->players().begin();
       p != this->players().end();
       ++p)
    if ((*p)->type() == Player::HUMAN)
      return static_cast<Human*>(*p);

  return NULL;
} // Human const* Party::human_player() const


/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	the players
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.0
 **/
vector<Player*>&
Party::players()
{
  return this->players_;
} // vector<Player*>& Party::players()

/**
 ** -> result
 **
 ** @param	p	number of the player
 **
 ** @return	the player with the number 'p'
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **/
Player&
Party::player(unsigned const p)
{
  DEBUG_ASSERTION((p < this->playerno()),
                  "Party::player(p):\n"
                  "'p' is not valid (" << p << ")");

  return *(this->players()[p]);
} // Player& Party::player(unsigned const p)

/**
 ** -> result
 **
 ** @param	p	number of the player
 **
 ** @return	the player with the number 'p'
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **/
Player const&
Party::player(unsigned const p) const
{
  DEBUG_ASSERTION((p < this->playerno()),
                  "Party::player(p):\n"
                  "'p' is not valid (" << p << ")");

  return *(this->players()[p]);
} // Player const& Party::player(unsigned const p) const

/**
 ** -> result
 **
 ** @param	player   the player
 **
 ** @return	the position of the player, UINT_MAX if not found
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.13
 **/
unsigned
Party::no(Player const& player) const
{
  for (unsigned p = 0; p < this->playerno(); ++p)
    if (&this->player(p) == &player)
      return p;
  return UINT_MAX;
} // unsigned Party::no(Player player) const

/**
 ** -> result
 **
 ** @param	name   player name
 **
 ** @return	the player with the name
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.13
 **/
Player*
Party::player_by_name(string const& name)
{
  for (vector<Player*>::const_iterator p = this->players_.begin();
       p != this->players_.end();
       ++p)
    if ((*p)->name() == name)
      return *p;
  return NULL;
} // Player* Party::player_by_name(string name)

/**
 ** -> result
 **
 ** @param	name   player name
 **
 ** @return	the player with the name
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.13
 **/
Player const*
Party::player_by_name(string const& name) const
{
  for (vector<Player*>::const_iterator p = this->players_.begin();
       p != this->players_.end();
       ++p)
    if ((*p)->name() == name)
      return *p;
  return NULL;
} // Player const* Party::player_by_name(string name) const

/**
 ** Sets the player in the party
 ** The player is taken directly, 'party' deletes it afterwards.
 **
 ** @param	n	the number of the player
 ** @param	player	the player to be set
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.13
 **/
void
Party::set_player(unsigned const n, Player* player)
{
  DEBUG_ASSERTION((n < this->playerno()),
                  "Party::set_player(player, n):\n"
                  "'n' is not valid");

  DEBUG_ASSERTION(!(::game_status & GAMESTATUS::GAME),
                  "Party::set_player(player, n):\n"
                  "  game_status is in the game.");

  delete this->players_[n];
  this->players_[n] = player;

  if (::ui != NULL)
    ::ui->player_changed(this->player(n));

  return ;
} // void Party::set_player(unsigned const n, Player* player)

/**
 ** add a player to the party
 **
 ** @param	player	the player to add
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.13
 **/
void
Party::add_player(Player* player)
{
  this->players_.push_back(player);
  if (::ui != NULL)
    ::ui->player_added(this->player(this->playerno() - 1));
  return ;
} // void Party::add_player(Player* player)

/**
 ** remove all players but the first four
 **
 ** @param	-
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.13
 **/
void
Party::remove_spare_players()
{
  DEBUG_ASSERTION((::game_status <= GAMESTATUS::PARTY_LOADED),
                  "Party::remove_spare_players()\n"
                  "  ::game_status = " << ::name(::game_status) << " is not valid");
  for (unsigned p = 4; p < this->playerno(); ++p)
    delete this->players_[p];
  this->players_.resize(4);
  if (::ui != NULL)
    ::ui->sparse_players_removed();
  return ;
} // void Party::remove_spare_players()

/**
 ** replace the player in the party
 ** The player is taken directly, the replaced player is returned
 **
 ** @param	n	the number of the player
 ** @param	player	the player to be replaced
 **
 ** @return	replaced player
 **		(has to be deleted!)
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.13
 **/
Player*
Party::replace_player(unsigned const n, Player* player)
{
  DEBUG_ASSERTION((n < this->playerno()),
                  "Party::replace_player(player, n):\n"
                  "'n' is not valid: " << n);

  DEBUG_ASSERTION(!(   (::game_status & GAMESTATUS::GAME)
                    && (::game_status != GAMESTATUS::GAME_INIT)
                    && this->game().contains(*this->players_[n])),
                  "Party::replace_player(player = " << player->name() << ", n = " << n << "):\n"
                  "  game_status = " << ::name(::game_status) << " is in the game.");

  Player* const player_bak = this->players_[n];
  this->players_[n] = player;

  if (::ui != NULL)
    ::ui->player_changed(this->player(n));

  return player_bak;
} // Player* Party::replace_player(unsigned n, Player* player)

/**
 ** switches the position of the players
 **
 ** @param	n0	the first player
 ** @param	n1	the second player
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.2
 **/
void
Party::switch_players(unsigned const n0, unsigned const n1)
{
  DEBUG_ASSERTION((n0 < this->playerno()),
                  "Party::switch_players(n0, n1):\n"
                  "'n0' is not valid");

  DEBUG_ASSERTION((n1 < this->playerno()),
                  "Party::switch_players(n0, n1):\n"
                  "'n1' is not valid");

  DEBUG_ASSERTION(!(::game_status & GAMESTATUS::GAME),
                  "Party::switch_players(n0, n1):\n"
                  "  game_status is in the game.");

  // first set the names, so that empty names are replaced by the default names
  this->player(n0).set_name(this->player(n0).name());
  this->player(n1).set_name(this->player(n1).name());
  std::swap(this->players_[n0], this->players_[n1]);
  ::ui->players_switched(this->player(n0), this->player(n1));

  return ;
} // void Party::switch_players(unsigned const n0, unsigned const n1)

/**
 ** set the name of the player 'player_no' to 'new_name'
 **
 ** @param	player_no	the number of the player
 ** @param	new_name	the new name
 **
 ** @return	-
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **/
void
Party::set_playername(unsigned const player_no,
                      string const& new_name)
{
  DEBUG_ASSERTION((player_no < this->playerno()),
                  "Party::set_playername():  Illegal Value for player number: "
                  << player_no);

  this->players_[player_no]->set_name(new_name);

  return ;
} // void Party::set_playername(player_no, new_name)

/**
 ** set the voice of the player 'player_no' to 'voice'
 **
 ** @param     player_no   the number of the player
 ** @param     voice       the new voice
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.5
 **/
void
Party::set_playervoice(unsigned const player_no,
                       string const& voice)
{
  DEBUG_ASSERTION((player_no < this->playerno()),
                  "Party::set_playervoice():  Illegal Value for player number: "
                  << player_no);

  this->players_[player_no]->set_voice(voice);

  return ;
} // void Party::set_playervoice(unsigned player_no, string voice)

/**
 ** sets the type of the player 'player_no' to 'new_type'
 **
 ** @param	player_no	the number of the player
 ** @param	new_type	the new type
 **
 ** @return	-
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **
 ** @todo	keep the aiconfig
 **/
void
Party::set_playertype(unsigned const player_no,
                      Player::Type const new_type)
{
  DEBUG_ASSERTION((player_no < this->playerno()),
                  "Party::set_playertype(): "
                  << "  Illegal Value for player number: "
                  << player_no); 

  DEBUG_ASSERTION(this->players()[player_no],
                  "Party::set_playertype(): "
                  << "  player " << player_no << " does not exists");

  Player const& player = this->player(player_no);

  if (player.type() == new_type)
    return ;

  bool const default_name
    = (   Player::default_name(player.type(), player_no)
       == player.name());
  // create the new player
  this->set_player(player_no, Player::create(new_type, player));

  if (default_name)
    this->player(player_no).set_default_name();

  return ;
} // void Party::set_playertype(unsigned const player_no, Player::Type const new_type)

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    the seed
 **
 ** @author    Diether Knof
 **
 ** @version   0.7,3
 **/
unsigned
Party::seed() const
{
  if (this->in_configuration())
    return this->seed_first();

  return this->seed_;
} // unsigned Party::seed() const

/**
 ** sets the seed
 **
 ** @param     seed   new seed
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7,3
 **/
void
Party::set_seed(unsigned const seed)
{
  if (this->in_configuration()) {
    if (this->seed_first() != seed) {
      this->seed_first_ = seed;
      ::ui->party_seed_changed();
    }
  }
  if (!::setting(Setting::ADDITIONAL_PARTY_SETTINGS))
    this->set_random_seed();
  else if (seed == UINT_MAX)
    this->set_random_seed();
  else
    this->seed_ = seed % PSEUDO_RAND_MAX;

  return ;
} // void Party::set_seed(unsigned seed)

/**
 ** sets the seed of the party to a random value
 ** (uses the system clock)
 **
 ** @param     -
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.3
 **
 ** @todo      'better' random value
 **/
void
Party::set_random_seed()
{
  if (this->in_configuration()) {
    if (this->seed_first() != UINT_MAX) {
      this->seed_first_ = UINT_MAX;
      ::ui->party_seed_changed();
    }
  }

  // cannot use 'seed_next' since that would take 'seed_first' for 'seed()'
  ::pseudo_rand_set(static_cast<unsigned>(time(NULL) % PSEUDO_RAND_MAX));
  for (unsigned i = 0; i < 10; i++)
    ::pseudo_rand_next();
  this->seed_ = ::pseudo_rand_next();

  return ;
} // void Party::set_random_seed()

/**
 ** calculates the next seed with the old one
 **
 ** @param     -
 **
 ** @return    the new seed
 **/
unsigned
Party::seed_next()
{
  ::pseudo_rand_set(this->seed());

  // just increase the seed by one for a new cards distribution
  this->seed_ = (this->seed_ + 1) % PSEUDO_RAND_MAX;

  if (FAST_PLAY & FAST_NS::SEED_INFO) {
    // count the seeds
    static int seed_count = 0;
    seed_count += 1;

    // when a random number has already been tested, stop
    static vector<bool> tested(PSEUDO_RAND_MAX, false);
    if (tested[this->seed()] || (seed_count == PSEUDO_RAND_MAX)
        || (seed_count == 10000)) {
      cout << "\nNumber of seeds: " << seed_count
        << " (" << PSEUDO_RAND_MAX << ")" << endl;
      ::game_status = GAMESTATUS::QUIT;
      ::exit(EXIT_SUCCESS);
    }

    tested[this->seed()] = true;
  } // if (FAST_PLAY & FAST_NS::SEED_INFO)

  { // check whether the seed was already used and skip it then
    bool already_used = false;
    do { // while (already_used) ;
      already_used = false;
      for (unsigned g = 0; g < this->game_summaries().size(); ++g) {
        unsigned seed = this->game_summary(g).seed();
        ::pseudo_rand_set(seed);
        for (unsigned i = 0; i < 49; i++) {
          if (this->seed() == seed) {
            already_used = true;
            break;
          }
          seed = ::pseudo_rand_next();
        }
        if (already_used)
          break;
      } // for (g < this->game_summries().size())

      if (already_used)
        this->seed_ += 1;
    } while (already_used) ;
  } // check whether the seed was already used

  return this->seed();
} // unsigned Party::seed_next()

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    the startplayer
 **
 ** @author    Diether Knof
 **
 ** @version   0.7,3
 **/
unsigned
Party::startplayer() const
{
  if (this->in_configuration())
    return this->startplayer_first();

  return this->startplayer_;
} // unsigned Party::startplayer() const

/**
 ** sets the first startplayer
 **
 ** @param     startplayer   number of the startplayer
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.3
 **/
void
Party::set_startplayer(unsigned const startplayer)
{
  if (this->in_configuration()) {
    if (this->startplayer_first() != startplayer) {
      this->startplayer_first_ = startplayer;
      ::ui->party_startplayer_changed();
    }
  }
  if (!::setting(Setting::ADDITIONAL_PARTY_SETTINGS))
    this->startplayer_ = 1;
  else if (startplayer == UINT_MAX)
    this->set_random_startplayer();
  else
    this->startplayer_ = startplayer;

  return ;
} // void Party::set_startplayer(unsigned const startplayer)

/**
 ** sets the startplayer to random
 **
 ** @param     -
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.3
 **
 ** @todo      better 'random' value
 **/
void
Party::set_random_startplayer()
{
  if (this->in_configuration()) {
    if (this->startplayer_first() != UINT_MAX) {
      this->startplayer_first_ = UINT_MAX;
      ::ui->party_startplayer_changed();
    }
  }

  this->startplayer_ = static_cast<unsigned>(time(NULL) % this->playerno());

  return ;
} // void Party::set_random_startplayer_first()

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	whether the party can be configured (seed, startplayer)
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.4
 **/
bool
Party::in_configuration() const
{
  switch (::game_status) {
  case GAMESTATUS::PROGRAMSTART:
  case GAMESTATUS::PARTY_NEW:
    return true;
  case GAMESTATUS::PARTY_INITIAL_LOADED:

    return (this->gameno() == 0);
  default:
    return false;
  } // switch (::game_status)
} // bool Party::in_configuration() const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	the number of the current round
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.9
 **/
unsigned
Party::roundno() const
{
  if (this->round_startgame_.empty())
    return 0;

  if (::game_status == GAMESTATUS::PARTY_FINISHED)
    return this->round_startgame_.size();

  return (this->round_startgame_.size() - 1);
} // unsigned Party::roundno() const

/**
 ** -> result
 ** Also started rounds are counted.
 ** If the last game of a round is finished, the round is counted as full.
 **
 ** @param	-
 **
 ** @return	the number of remaining rounds, excluding a duty soli round
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.12
 **/
unsigned
Party::remaining_rounds() const
{
  if (::game_status == GAMESTATUS::PARTY_FINISHED)
    return 0;

  if (!this->rule()(Rule::NUMBER_OF_ROUNDS_LIMITED))
    return 0;

  if (this->is_duty_soli_round())
    return 0;

  if (   (this->rule()(Rule::NUMBER_OF_ROUNDS)
          == this->roundno())
      && this->starts_new_round(this->gameno()))
    return 0;

  return (this->rule()(Rule::NUMBER_OF_ROUNDS)
          - this->roundno()
          - ((   this->starts_new_round(this->gameno() + 1)
              && (::game_status == GAMESTATUS::GAME_FINISHED)
             ) ? 1 : 0));
} // unsigned Party::remaining_rounds() const

/**
 ** -> result
 ** A started game is counted as remaining.
 ** A finished game (gamestatus = GAME_FINISHED) is not counted as remaining.
 **
 ** @param	-
 **
 ** @return	the number of remaining games (without duty solo games)
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.7
 **/
unsigned
Party::remaining_normal_games() const
{
  if (!this->rule()(Rule::NUMBER_OF_ROUNDS_LIMITED))
    return 0;
  if (this->is_duty_soli_round())
    return 0;

  return (  (this->remaining_rounds()
             * this->rule()(Rule::NUMBER_OF_GAMES_PER_ROUND))
          + ( this->playerno()
             - ( (this->finished_games() + this->playerno()
                  - this->real_startplayer_first())
                % this->playerno()) )
         );
} // unsigned Party::remaining_normal_games() const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	the number of remaining games (with duty solo games)
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.7
 **/
unsigned
Party::remaining_games() const
{
  if (!this->rule()(Rule::NUMBER_OF_ROUNDS_LIMITED))
    return 0;

  return (this->remaining_normal_games() + this->remaining_duty_soli());
} // unsigned Party::remaining_games() const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	the number of the current game
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.9
 **/
unsigned
Party::gameno() const
{
  if (::game_status == GAMESTATUS::GAME_FINISHED)
    return (this->game_summaries().size() - 1);
  else
    return this->game_summaries().size();
} // unsigned Party::gameno() const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	the number of finished games
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
unsigned
Party::finished_games() const
{
  return this->game_summaries().size();
} // unsigned Party::finished_games() const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	whether this is the duty soli round
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.7
 **/
bool
Party::is_duty_soli_round() const
{
  if (   (game_status == GAMESTATUS::PARTY_FINISHED)
      && !this->game_summaries().empty()
      && !GAMETYPE::is_real_solo(this->last_game_summary().game_type()))
    return false;

  return (this->roundno() >= this->duty_soli_round());
} // bool Party::is_duty_soli_round() const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	whether this is the last game
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.7
 **/
bool
Party::is_last_game() const
{
  // must be in a running party
  if (!(::game_status >= GAMESTATUS::PARTY_PLAY))
    return false;

  // if there are still duty soli to play
  if (this->remaining_duty_soli()
      > ( (   (::game_status > GAMESTATUS::GAME_RESERVATION)
           && (::game_status < GAMESTATUS::GAME_FINISHED)
           && this->game().is_duty_solo())
         ? 1 : 0))
    return false;

  // if the number of rounds is limited
  // it must be the last round and the next player to start
  // is the startplayer of the party
  if (   this->rule()(Rule::NUMBER_OF_ROUNDS_LIMITED)
      && (this->remaining_rounds()
          <= (  (   (::game_status == GAMESTATUS::GAME_FINISHED)
                 || (::game_status <= GAMESTATUS::PARTY_LOADED))
              ? 0u
              : 1u))
      && ((this->startplayer() + 1) % this->playerno()
          == this->startplayer_first() )
      && (   (::game_status <= GAMESTATUS::GAME_RESERVATION)
          || !this->game().startplayer_stays() )
      && (this->remaining_duty_soli() == 0)
     )
    return true;

  // last game in duty soli round
  if (this->is_duty_soli_round()
      && (this->remaining_duty_soli()
          == ((::game_status < GAMESTATUS::GAME_FINISHED)
              ? 1
              : 0) )
      && (   (::game_status <= GAMESTATUS::GAME_RESERVATION)
          || this->game().is_duty_solo() ) )
    return true;

  // the points are limited
  if (   this->rule()(Rule::POINTS_LIMITED)
      && (this->remaining_points() <= 0)
      && (this->remaining_duty_soli() == 0) )
    return true;

  return false;
} // bool Party::is_last_game() const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	whether the party is finished
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.7
 **/
bool
Party::is_finished() const
{
  return (   (this->remaining_duty_soli() == 0)
          && (   (   this->rule()(Rule::NUMBER_OF_ROUNDS_LIMITED)
                  && (this->remaining_rounds() == 0) )
              || (   this->rule()(Rule::POINTS_LIMITED)
                  && (this->remaining_points() <= 0) ) ) );
} // bool Party::is_finished() const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	the remaining points
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.4
 **/
int
Party::remaining_points() const
{
  if (!this->rule()(Rule::POINTS_LIMITED))
    return -1;

  return (static_cast<int>(this->rule()(Rule::POINTS)) - this->points());
} // unsigned Party::remaining_points() const

/**
 ** -> result
 **
 ** @param	gameno	number of the game
 **
 ** @return	whether game 'gameno' starts a new round
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
bool
Party::starts_new_round(unsigned const gameno) const
{
  if ((gameno == 0) || this->round_startgame_.empty())
    return true;
  if (gameno <= this->round_startgame().back()) {
    for (vector<unsigned>::const_iterator s = this->round_startgame().begin();
         s != this->round_startgame().end();
         ++s) {
      if (*s == gameno) {
        return true;
      }
      else if (*s > gameno) {
        return false;
      }
    }
    return false;
  } else { // if !(gameno <= this->gameno())
    // the startgame of the next round
    unsigned next_round_startgame
      = this->round_startgame().back() + this->playerno();

    // check for played solo games in the current round
    for (unsigned g = this->round_startgame().back(); g < this->gameno(); ++g)
      if (this->game_summary(g).startplayer_stays())
        next_round_startgame += 1;

    // check whether the current game is a solo
    if (   (::game_status & GAMESTATUS::GAME)
        //&& (::game_status < GAMESTATUS::GAME_FINISHED)
        && this->game().startplayer_stays())
      next_round_startgame += 1;

    if (next_round_startgame > gameno)
      return false;
    if (this->rule()(Rule::NUMBER_OF_ROUNDS_LIMITED)
        && (  (this->roundno() 
               + (gameno - next_round_startgame) / this->playerno())
            > this->rule()(Rule::NUMBER_OF_ROUNDS)) )
      return false;
    return ((gameno - next_round_startgame) % this->playerno()
            == 0);
  } // if !(gameno <= this->gameno())
} // bool Party::starts_new_round(unsigned const gameno) const

/**
 ** -> result
 **
 ** @param	gameno	number of the game
 **
 ** @return	the round game 'gameno' is in
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **/
unsigned
Party::round_of_game(unsigned const gameno) const
{
  if (gameno == 0)
    return 0;

  if (gameno == this->gameno()) {
    return this->roundno();
  }
  if (gameno < max(this->round_startgame().back(), this->gameno())) {
    for (unsigned r = 1; r <= this->roundno(); ++r) {
      if (this->round_startgame()[r] > gameno) {
        return r - 1;
      }
    }
    return this->roundno();
  } // if (gameno <= this->gameno())

  // the startgame of the next round
  unsigned next_round_startgame
    = this->round_startgame().back() + this->playerno();

  // check for played solo games in the current round
  for (unsigned g = this->round_startgame().back(); g < this->gameno(); ++g)
    if (this->game_summary(g).startplayer_stays())
      next_round_startgame += 1;

  // check whether the current game is a solo
  if (   (::game_status & GAMESTATUS::GAME)
      //&& (::game_status < GAMESTATUS::GAME_FINISHED)
      && this->game().startplayer_stays())
    next_round_startgame += 1;

  if (this->is_duty_soli_round())
    next_round_startgame = gameno;
  if (gameno < next_round_startgame)
    return this->roundno();

  unsigned const round
    = (this->roundno() + 1
       + (gameno - next_round_startgame) / this->playerno());

#ifdef OUTDATED
  // for future games, we have to estimate
  unsigned const round
    = (  (this->roundno() == 0)
       ? (gameno / this->playerno())
       : (  (gameno * this->roundno())
          / this->round_startgame()[this->roundno()]) );
#endif

  // do not exceed the limited rounds (+ duty soli round)
  if (this->rule()(Rule::NUMBER_OF_ROUNDS_LIMITED))
    return min(round,
               this->rule()(Rule::NUMBER_OF_ROUNDS)
               + ( (this->remaining_duty_soli() || this->is_duty_soli_round())
                  ? 1 : 0));
  else
    return round;
} // unsigned Party::round_of_game(unsigned const gameno) const

/**
 ** add a game summary
 ** add the points and manage the bock rounds
 **
 ** @param	game_summary   game summary to add
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.13
 **/
void
Party::add_game_summary(GameSummary* const game_summary)
{
  this->game_summaries_.push_back(game_summary);
  if (!this->bock_multipliers().empty()
      && !game_summary->startplayer_stays() )
    this->bock_multipliers_.erase(this->bock_multipliers_.begin());

  for (unsigned p = 0; p < game_summary->players_number(); ++p) {
    if (this->rule()(Rule::COUNTING) == COUNTING_NS::PLUS)
      this->points_ += game_summary->points(p);
    else
      this->points_ -= game_summary->points(p);
  } // for (p < this->playerno())

  // add the bock multipliers
  if (this->rule()(Rule::BOCK)) {
    if (!game_summary->bock_triggers().empty()) {
      if (this->rule()(Rule::BOCK_APPEND)) {
        for (unsigned g = 0;
             g < this->rule()(Rule::NUMBER_OF_GAMES_PER_ROUND);
             ++g)
          this->bock_multipliers_.push_back(this->rule()(Rule::BOCK_MULTIPLIER));
      } else { // if !(this->rule()(Rule::BOCK_APPEND))
        for (unsigned g = 0;
             g < this->rule()(Rule::NUMBER_OF_GAMES_PER_ROUND);
             ++g) {
          if (this->bock_multipliers().size() > g)
            this->bock_multipliers_[g] *= this->rule()(Rule::BOCK_MULTIPLIER);
          else
            this->bock_multipliers_.push_back(this->rule()(Rule::BOCK_MULTIPLIER));
        } // for (g < this->rule(NUMBER_OF_GAMES_PER_ROUND))
      } // if !(this->rule()(Rule::BOCK_APPEND))
    } // if (!this->game().game_summaries().bock_triggers().empty())

    if (   this->rule()(Rule::NUMBER_OF_ROUNDS_LIMITED)
        && (this->bock_multipliers().size()
            > (this->remaining_normal_games()
               + (this->remaining_duty_soli() ? 1 : 0) ) ) ) {
      this->bock_multipliers_.resize(this->remaining_normal_games()
                                     + (this->remaining_duty_soli() ? 1 : 0));
    }
  } // if (this->rule()(Rule::BOCK))

  return ;
} // void Party::add_game_summary(GameSummary* const game_summary)

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	the game summary of the last game
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **/
GameSummary const&
Party::last_game_summary() const
{
  return *this->game_summaries().back();
} // GameSummary const& Party::last_game_summary() const;

/**
 ** -> result
 **
 ** @param	gameno	number of the game
 **
 ** @return	the game summary of game 'gameno'
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **/
GameSummary const&
Party::game_summary(unsigned const gameno) const
{
  DEBUG_ASSERTION((gameno < this->game_summaries().size()),
                  "Party::game_summary(gameno):\n"
                  "  gameno = " << gameno << " is too great "
                  "(maximal " << this->game_summaries().size() << ")");
  return *this->game_summaries()[gameno];
} // GameSummary const& Party::game_summary(unsigned const gameno) const;

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	the bock multiplier for the current game
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
unsigned
Party::current_bock_multiplier() const
{
  if (this->bock_multipliers().empty())
    return 1;

  DEBUG_ASSERTION(this->bock_multipliers()[0] > 1,
                  "Party::current_bock_multiplier()\n"
                  "  bock multiplier is not > 1: " << this->bock_multipliers()[0]);
  return this->bock_multipliers()[0];
} // unsigned Party::current_bock_multiplier() const;

/**
 ** -> result
 **
 ** @param	gameno   game number
 **
 ** @return	the bock multiplier for the game gameno
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
unsigned
Party::bock_multiplier(unsigned const gameno) const
{
  if (gameno < this->finished_games())
    return this->game_summary(gameno).bock_multiplier();

  if (gameno >= this->finished_games() + this->bock_multipliers().size())
    return 1;

  return this->bock_multipliers()[gameno - this->finished_games()];
} // unsigned Party::bock_multiplier(unsigned gameno) const

/**
 ** -> result
 **
 ** @param	roundno	number of round
 **
 ** @return	the number of the first game in the round
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.3
 **/
unsigned
Party::round_startgame(unsigned const roundno) const
{
  if (roundno <= this->roundno())
    return this->round_startgame()[roundno];
  if (roundno == 0)
    return 0;

  // if roundno > current round we have to estimate
  if (::game_status & GAMESTATUS::GAME) {
    return (this->gameno()
            + (this->playerno()
               - ((this->playerno()
                   + this->startplayer() - this->startplayer_first()
                  ) % this->playerno())
              )
            + (this->game().startplayer_stays() ? 1 : 0)
            + (roundno - this->roundno() - 1) * this->playerno()
           );
  } else {
    return (this->gameno()
            + (this->last_game_summary().startplayer_stays() ? 1 : 0)
            + (this->playerno()
               - ((this->startplayer() - this->startplayer_first()
                   + this->playerno()) % this->playerno())
              )
            + (roundno - this->roundno() - 1) * this->playerno()
           );
  }
} // unsigned Party::round_startgame(unsigned const roundno) const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	how many points has been made so far
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **/
int
Party::pointsum() const
{
  if (this->game_summaries().empty())
    return 0;
  return this->pointsum_till_game(this->game_summaries().size() - 1);
} // int Party::pointsum() const

/**
 ** -> result
 **
 ** @param	gameno   game number
 **
 ** @return	how many points has been made till game 'gameno'
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **/
int
Party::pointsum_till_game(unsigned const gameno) const
{
  int points = 0;

  for (unsigned g = 0; g <= min(gameno, this->finished_games()); ++g)
    points += this->game_summary(g).points();

  return points;
} // int Party::pointsum_till_game(unsigned const gameno) const

/**
 ** -> result
 **
 ** @param	roundno		number of round
 **
 ** @return	how many points has been made till round 'roundno'
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **/
int
Party::pointsum_till_round(unsigned const roundno) const
{
  int points = 0;

  for (unsigned r = 0; r <= roundno; ++r)
    points += this->pointsum_in_round(r);

  return points;
} // int Party::pointsum_till_round(unsigned const roundno) const

/**
 ** -> result
 **
 ** @param	roundno		number of round
 **
 ** @return	how many points has been made in round 'roundno'
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **/
int
Party::pointsum_in_round(unsigned const roundno) const
{
  unsigned const gameno_end
    = ( (roundno == this->roundno())
       ? this->gameno() + ((::game_status == GAMESTATUS::GAME_FINISHED)
                           ? 1
                           : 0)
       : this->round_startgame(roundno + 1) );

  int points = 0;

  for (unsigned g = this->round_startgame(roundno);
       g < min(gameno_end, this->finished_games());
       ++g) {
    points += this->game_summary(g).points();
  }

  return points;
} // int Party::pointsum_in_round(unsigned const roundno) const

/**
 ** -> result
 **
 ** @param	player	player
 **
 ** @return	how many points 'player' has made so far
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **/
int
Party::pointsum(Player const& player) const
{
  return this->pointsum(this->no(player));
} // int Party::pointsum(Player player) const

/**
 ** -> result
 **
 ** @param	playerno	number of player
 **
 ** @return	how many points player 'playerno' has made so far
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **/
int
Party::pointsum(unsigned const playerno) const
{
  if (this->game_summaries().empty())
    return 0;
  return this->pointsum_till_game(this->game_summaries().size() - 1, playerno);
} // int Party::pointsum(unsigned const playerno) const

/**
 ** -> result
 **
 ** @param	gameno		number of the game
 ** @param	playerno	number of player
 **
 ** @return	how many points player 'playerno' has made till game 'gameno'
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **/
int
Party::pointsum_till_game(unsigned const gameno, unsigned const playerno) const
{
  int points = 0;

  for (unsigned g = 0; g < gameno; ++g) {
    if (this->game_summary(g).party_player_has_played(playerno))
      points += this->game_summary(g).points(this->game_summary(g).game_player(playerno));
  }
  if (   (gameno < this->gameno())
      || (::game_status == GAMESTATUS::GAME_FINISHED) ) {
    if (this->game_summary(gameno).party_player_has_played(playerno))
      points += this->game_summary(gameno).points(this->game_summary(gameno).game_player(playerno));
  }

  return points;
} // int Party::pointsum_till_game(unsigned const gameno, unsigned const playerno) const

/**
 ** -> result
 **
 ** @param	roundno		number of round
 ** @param	playerno	number of player
 **
 ** @return	how many points player 'playerno' has made till round 'roundno'
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **/
int
Party::pointsum_till_round(unsigned const roundno,
                           unsigned const playerno) const
{
  int points = 0;

  for (unsigned r = 0; r <= roundno; ++r)
    points += this->pointsum_in_round(r, playerno);

  return points;
} // int Party::pointsum_till_round(unsigned const roundno, unsigned const playerno) const

/**
 ** -> result
 **
 ** @param	roundno		number of round
 ** @param	playerno	number of player
 **
 ** @return	how many points player 'playerno' has made in round 'roundno'
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **/
int
Party::pointsum_in_round(unsigned const roundno, unsigned const playerno) const
{
  unsigned const gameno_end
    = ( (roundno == this->roundno())
       ? this->gameno() + ((::game_status == GAMESTATUS::GAME_FINISHED)
                           ? 1
                           : 0)
       : this->round_startgame(roundno + 1) );

  int points = 0;

  for (unsigned g = this->round_startgame(roundno);
       g < min(gameno_end, this->finished_games());
       ++g)
    if (this->game_summary(g).party_player_has_played(playerno))
      points += this->game_summary(g).points(this->game_summary(g).game_player(playerno));

  return points;
} // int Party::pointsum_in_round(unsigned const roundno, unsigned const playerno) const

/**
 ** -> result
 **
 ** @param	player	player
 **
 ** @return	the rang of 'player'
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.8
 **/
unsigned
Party::rang(Player const& player) const
{
  return this->rang(this->no(player));
} // unsigned Party::rang(Player player) const

/**
 ** -> result
 **
 ** @param	playerno	number of player
 **
 ** @return	the rang of the player
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.8
 **/
unsigned
Party::rang(unsigned const playerno) const
{
  unsigned rang = 0;
  int const points = this->pointsum(playerno);

  for (unsigned p = 0; p < this->playerno(); ++p)
    if ((p != playerno)
        && (points < this->pointsum(p)))
      rang += 1;

  return rang;
} // unsigned Party::rang(unsigned const playerno) const

/**
 ** -> result
 **
 ** @param	player	player
 **
 ** @return	the number of duty solo games 'player' has played
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.7
 **/
unsigned
Party::played_duty_soli(Player const& player) const
{
  return this->played_duty_soli(this->no(player));
} // unsigned Party::played_duty_soli(Player player) const

/**
 ** -> result
 **
 ** @param	playerno	number of player
 **
 ** @return	the number of duty solo games the player has played
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.7
 **/
unsigned
Party::played_duty_soli(unsigned const playerno) const
{
  unsigned n = 0;

  for (vector<GameSummary*>::const_iterator
       game_summary = this->game_summaries().begin();
       game_summary != this->game_summaries().end();
       ++game_summary)
    if (   (*game_summary)->is_duty_solo()
        && ((*game_summary)->soloplayer_no() == playerno) )
      n += 1;

  return min(n, this->rule()(Rule::NUMBER_OF_DUTY_SOLI));
} // unsigned Party::played_duty_soli(unsigned playerno) const

/**
 ** -> result
 **
 ** @param	player	player
 **
 ** @return	the number of duty color solo games 'player' has played
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.7
 **/
unsigned
Party::played_duty_color_soli(Player const& player) const
{
  return this->played_duty_color_soli(this->no(player));
} // unsigned Party::played_duty_color_soli(Player player) const

/**
 ** -> result
 **
 ** @param	playerno	number of player
 **
 ** @return	the number of duty color solo games the player has played
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.7
 **/
unsigned
Party::played_duty_color_soli(unsigned const playerno) const
{
  unsigned n = 0;

  for (vector<GameSummary*>::const_iterator
       game_summary = this->game_summaries().begin();
       game_summary != this->game_summaries().end();
       ++game_summary)
    if (   (*game_summary)->is_duty_solo()
        && GAMETYPE::is_color_solo((*game_summary)->game_type())
        && ((*game_summary)->soloplayer_no() == playerno) )
      n += 1;

  return min(n, this->rule()(Rule::NUMBER_OF_DUTY_COLOR_SOLI));
} // unsigned Party::played_duty_color_soli(unsigned playerno) const

/**
 ** -> result
 **
 ** @param	player	player
 **
 ** @return	the number of duty picture solo games 'player' has played
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.7
 **/
unsigned
Party::played_duty_picture_soli(Player const& player) const
{
  return this->played_duty_picture_soli(this->no(player));
} // unsigned Party::played_duty_picture_soli(Player player) const

/**
 ** -> result
 **
 ** @param	playerno	number of player
 **
 ** @return	the number of duty picture solo games the player has played
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.7
 **/
unsigned
Party::played_duty_picture_soli(unsigned const playerno) const
{
  unsigned n = 0;

  for (vector<GameSummary*>::const_iterator
       game_summary = this->game_summaries().begin();
       game_summary != this->game_summaries().end();
       ++game_summary)
    if (   (*game_summary)->is_duty_solo()
        && GAMETYPE::is_picture_solo((*game_summary)->game_type())
        && ((*game_summary)->soloplayer_no() == playerno) )
      n += 1;

  return min(n, this->rule()(Rule::NUMBER_OF_DUTY_PICTURE_SOLI));
} // unsigned Party::played_duty_picture_soli(unsigned playerno) const

/**
 ** -> result
 **
 ** @param	player	player
 **
 ** @return	the number of free duty solo games 'player' has played
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.7
 **/
unsigned
Party::played_duty_free_soli(Player const& player) const
{
  return this->played_duty_free_soli(this->no(player));
} // unsigned Party::played_duty_free_soli(Player player) const

/**
 ** -> result
 **
 ** @param	playerno	number of player
 **
 ** @return	the number of free duty solo games the player has played
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.7
 **/
unsigned
Party::played_duty_free_soli(unsigned const playerno) const
{
  return (  this->played_duty_soli(playerno)
          - this->played_duty_color_soli(playerno)
          - this->played_duty_picture_soli(playerno));
} // unsigned Party::played_duty_free_soli(unsigned playerno) const

/**
 ** -> result
 **
 ** @param	player	player
 **
 ** @return	the number of lust solo games 'player' has played
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.8
 **
 ** @todo	not calculate duty soli
 **/
unsigned
Party::played_lust_soli(Player const& player) const
{
  return this->played_lust_soli(this->no(player));
} // unsigned Party::played_lust_soli(Player player) const

/**
 ** -> result
 **
 ** @param	playerno	number of player
 **
 ** @return	the number of lust solo games the player has played
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.8
 **
 ** @todo	not calculate duty soli
 **/
unsigned
Party::played_lust_soli(unsigned const playerno) const
{
  unsigned n = 0;

  for (vector<GameSummary*>::const_iterator
       game_summary = this->game_summaries().begin();
       game_summary != this->game_summaries().end();
       ++game_summary)
    if (   (*game_summary)->is_lust_solo()
        && ((*game_summary)->soloplayer_no() == playerno) )
      n += 1;

  return n;
} // unsigned Party::played_lust_soli(unsigned const playerno) const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	the number of remaining duty picture solo games still to be played
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.7
 **/
unsigned
Party::remaining_duty_soli() const
{
  unsigned n = 0;
  for (unsigned p = 0; p < this->playerno(); ++p)
    n += this->remaining_duty_soli(p);

  return n;
} // unsigned Party::remaining_duty_soli() const

/**
 ** -> result
 **
 ** @param	player	player
 **
 ** @return	the number of remaining duty picture solo games 'player' has to play
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.7
 **/
unsigned
Party::remaining_duty_soli(Player const& player) const
{
  return this->remaining_duty_soli(this->no(player));
} // unsigned Party::remaining_duty_soli(Player player) const

/**
 ** -> result
 **
 ** @param	playerno	number of player
 **
 ** @return	the number of remaining free duty solo games the player has to play
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.7
 **/
unsigned
Party::remaining_duty_soli(unsigned const playerno) const
{
  return (this->rule()(Rule::NUMBER_OF_DUTY_SOLI)
          - this->played_duty_soli(playerno));
} // unsigned Party::remaining_duty_soli(unsigned playerno) const

/**
 ** -> result
 **
 ** @param	player	player
 **
 ** @return	the number of remaining color duty picture solo games 'player' has to play
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.7
 **/
unsigned
Party::remaining_duty_color_soli(Player const& player) const
{
  return this->remaining_duty_color_soli(this->no(player));
} // unsigned Party::remaining_duty_color_soli(Player player) const

/**
 ** -> result
 **
 ** @param	playerno	number of player
 **
 ** @return	the number of remaining color duty solo games the player has to play
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.7
 **/
unsigned
Party::remaining_duty_color_soli(unsigned const playerno) const
{
  return (this->rule()(Rule::NUMBER_OF_DUTY_COLOR_SOLI)
          - this->played_duty_color_soli(playerno));
} // unsigned Party::remaining_duty_color_soli(unsigned playerno) const

/**
 ** -> result
 **
 ** @param	player	player
 **
 ** @return	the number of remaining picture duty picture solo games 'player' has to play
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.7
 **/
unsigned
Party::remaining_duty_picture_soli(Player const& player) const
{
  return this->remaining_duty_picture_soli(this->no(player));
} // unsigned Party::remaining_duty_picture_soli(Player player) const

/**
 ** -> result
 **
 ** @param	playerno	number of player
 **
 ** @return	the number of remaining picture duty solo games the player has to play
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.7
 **/
unsigned
Party::remaining_duty_picture_soli(unsigned const playerno) const
{
  return (this->rule()(Rule::NUMBER_OF_DUTY_PICTURE_SOLI)
          - this->played_duty_picture_soli(playerno));
} // unsigned Party::remaining_duty_picture_soli(unsigned playerno) const

/**
 ** -> result
 **
 ** @param	player	player
 **
 ** @return	the number of remaining free duty picture solo games 'player' has to play
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.7
 **/
unsigned
Party::remaining_duty_free_soli(Player const& player) const
{
  return this->remaining_duty_free_soli(this->no(player));
} // unsigned Party::remaining_duty_free_soli(Player player) const

/**
 ** -> result
 **
 ** @param	playerno	number of player
 **
 ** @return	the number of remaining free duty solo games the player has to play
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.7
 **/
unsigned
Party::remaining_duty_free_soli(unsigned const playerno) const
{
  return (  this->remaining_duty_soli(playerno)
          - this->remaining_duty_color_soli(playerno)
          - this->remaining_duty_picture_soli(playerno));
} // unsigned Party::remaining_duty_free_soli(unsigned playerno) const

/**
 ** write the party into the output stream
 ** The current game is not written.
 **
 ** @param	ostr	output stream
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.9
 **
 ** @todo	output of the game?
 **/
void
Party::write(ostream& ostr) const
{
  if (::game_status < GAMESTATUS::PARTY_PLAY) {
    ostr << "first seed = " << Unsigned(this->seed_first()) << "\n"
      << "first startplayer = " << Unsigned(this->startplayer_first()) << "\n"
      << "\n";
  } else {
    ostr << "# first seed (setting) = " << Unsigned(this->seed_first()) << "\n"
      << "# first startplayer (setting) = " << Unsigned(this->startplayer_first()) << "\n"
      << "first seed = " << Unsigned(this->real_seed_first()) << "\n"
      << "first startplayer = " << Unsigned(this->real_startplayer_first()) << "\n"
      << "\n";
  }

  ostr << "rules \n"
    << "{\n"
    << this->rule()
    << "}\n";

  ostr<< '\n';

  ostr << "players\n"
    << "{\n";
  for (vector<Player*>::const_iterator player
       = this->players().begin();
       player != this->players().end();
       player++)
    ostr << "{\n" << **player << "}\n\n";
  ostr << "}\n";

  ostr << '\n';

  ostr << "game summaries\n";
  ostr << "{\n";
  for (unsigned i = 0; i < this->game_summaries().size(); ++i) {
    if (this->starts_new_round(i))
      ostr << "# round " << this->round_of_game(i) << '\n';
    ostr << "#  game " << i << "\n"
      << "{\n" << this->game_summary(i) << "}\n";
  } // for (i)
  ostr << "}\n";

  ostr << '\n';
  ostr << "points\n";
  ostr << "{\n";
  for (vector<Player*>::const_iterator player
       = this->players().begin();
       player != this->players().end();
       player++)
    ostr << (*player)->name() << ": " << this->pointsum(**player) << endl;
  ostr << "}\n";

  return ;
} // void Party::write(ostream& ostr) const

/**
 ** write the party points into the output stream
 **
 ** @param	ostr	output stream
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.12
 **/
void
Party::write_pointstable(ostream& ostr) const
{
  ostr << '\n';
  ostr << "party\n" ;
  ostr << "gameno: " << this->gameno() << '\n';
  ostr << "round startgame" << '\n';
  for (unsigned r = 0; r < this->round_startgame().size(); ++r)
    ostr << r << ": " << this->round_startgame(r) << '\n';

  ostr << "game ";
  for (unsigned p = 0; p < this->playerno(); ++p)
    ostr << "| " << this->player(p).name() << ' ';
  ostr << "| bm ";
  ostr << "| gametype \n";
  ostr << "-----";
  for (unsigned p = 0; p < this->playerno(); ++p)
    ostr << "+-" << setw(this->player(p).name().length())
      << "-" << '-';
  ostr << "+----";
  ostr << "+-----\n";
  for (unsigned g = 0; g < this->gameno(); ++g) {
    if (this->starts_new_round(g)) {
      if (this->round_of_game(g) == this->duty_soli_round())
        ostr << "-duty";
      else
        ostr << "-" << setw(3) << this->round_of_game(g) << " ";
      for (unsigned p = 0; p < this->playerno(); ++p)
        ostr << "+-" << setw(this->player(p).name().length())
          << "-" << '-';
      ostr << "+----";
      ostr << "+-----\n";
    }
    ostr << setw(3) << g << "  ";
    for (unsigned p = 0; p < this->playerno(); ++p)
      ostr << "| " << setw(this->player(p).name().length())
        << this->game_summary(g).points(p)
        << ' ';
    ostr << "|" << setw(3);
    if (this->bock_multiplier(g) == 1)
      ostr << " ";
    else
      ostr << this->bock_multiplier(g);
    ostr << ' ';
    ostr << "| ";
    ostr << (this->game_summary(g).is_solo() ? '*' : ' ');
    if (!this->game_summary(g).bock_triggers().empty())
      ostr << this->game_summary(g).bock_triggers().size();
    ostr << '\n';
  } // for (g < this->gameno())

  return ;
} // void Party::write_pointstable(ostream& ostr) const

/**
 ** write the players in short form (name, type) in the output stream
 **
 ** @param	ostr	output stream
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.13
 **/
void
Party::write_players_short(ostream& ostr) const
{
  ostr << "party players:\n";
  for (unsigned p = 0; p < this->players().size(); ++p) {
    ostr << p << ": ";
    this->player(p).write_short(ostr);
    ostr << '\n';
  }

  return ;
} // void Party::write_players_short(ostream& ostr) const

/**
 ** reads the party from the input stream
 **
 ** @param	istr	input stream
 **
 ** @return	whether the reading was successful
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.9
 **/
bool
Party::read(istream& istr)
throw (ReadException)
{
  // party config values
  Unsigned first_seed = UINT_MAX - 1;
  Unsigned first_startplayer = UINT_MAX - 1;
  Rule* rule = NULL;
  vector<Player*> players;
  vector<GameSummary*> game_summaries;

  Config config; // the config read
  int depth = 0; // the depth in the blocks
  try {
    do { // while (istr)
      istr >> config;

      if (istr.eof())
        break;

      if (istr.fail())
        break;

      if ((config.name == "") && (config.value == ""))
        break;

      if (config.value.empty()) {
        if (config.name == "{") {
          depth += 1;
        } else if (config.name == "}") {
          depth -= 1;
          // finished with the party
          if (depth == -1)
            istr.putback('}');
          if (depth <= 0)
            break;

#ifndef OUTDATED
          // 0.7.4   2006-09-14
        } else if (   (config.name == "rules")
                   || (config.name == "Rules") ) {
#else
        } else if (config.name == "rules") {
#endif
          if (rule)
            throw ReadException("Party read: rule already read");
          rule = new Rule();
          if (!rule->read(istr))
            throw ReadException("Party read: rule failed");

#ifndef OUTDATED
          // 0.7.4   2006-09-14
        } else if (   (config.name == "players")
                   || (config.name == "Players") ) {
#else
        } else if (config.name == "players") {
#endif
          if (!players.empty())
            throw ReadException("Party read: multiple players");

          istr >> config;
          if (! (config.value.empty()
                 && (config.name == "{")) )
            throw ReadException("Party read: players: expected '{'");
          while (istr.good()
                 && istr.peek() != '}') {
            Player* player = Player::create(istr);
            if (player == NULL)
              throw ReadException("Party read: players: read of player failed");

            players.push_back(player);

            // overread empty lines
            while (   (istr.peek() == '\n')
                   || (istr.peek() == '\r'))
              istr.get();
          } // while (istr.peek() != '}')

          istr >> config;
          if (! (config.value.empty()
                 && (config.name == "}")) )
            throw ReadException("Party read: players: expected '}'");

        } else if (config.name == "game summaries") {
          {
            string const line = read_line(istr);
            if (line != "{")
              throw ReadException("Party read: game summaries: expected '{', "
                                  " found '" + line + "'");
          }
          GameSummary* game_summary = NULL;
          while (istr.good()
                 && istr.peek() != '}') {
            try {
              game_summary = new GameSummary(*rule, istr);
            } catch(...) {
              game_summary = NULL;
              throw;
            }
            game_summaries.push_back(game_summary);
          } // while (istr.good() && istr.peek() !=- '}')
          {
            string const line = read_line(istr);
            if (line != "}")
              throw ReadException("Party read: game summaries: expected '}', "
                                  " found '" + line + "'");
          }

        } else if (config.name == "points") {
          // ignore the block
          {
            string const line = read_line(istr);
            if (line != "{")
              throw ReadException("Party read: points: expected '{', "
                                  " found '" + line + "'");
          }
          while (istr.good()
                 && istr.peek() != '}') {
            Config points_config;
            istr >> points_config;
          } // while (istr.good() && istr.peek() != '}')
          {
            string const line = read_line(istr);
            if (line != "}")
              throw ReadException("Party read: points: expected '}', "
                                  " found '" + line + "'");
          }

        } else { // if (config.name == ...)
          throw ReadException("Party read: unknown line "
                              "'" + config.name + "'");
        } // if (config.name == ...)
      } else { // if !(config.value.empty())
        if (config.name == "first seed") {
          istringstream istr2(config.value);
          istr2 >> first_seed;
          if (!istr2.good() && !istr2.eof())
            throw ReadException("Party read: error loading first seed");
        } else if (config.name == "first startplayer") {
          istringstream istr2(config.value);
          istr2 >> first_startplayer;
          if (!istr2.good() && !istr2.eof())
            throw ReadException("Party read: error loading first seed");
        } else { // if (config.name == ...)
          throw ReadException("Party read: unknown line "
                              "'" + config.name + " = " + config.value + "'");
        } // if (config.name == ...)
      } // if !(config.value.empty())

    } while (istr.good());

    if (!istr.good() && !istr.eof())
      throw ReadException("Party read: unknown error in the input stream");

    if (first_seed == UINT_MAX - 1)
      throw ReadException("Party read: first seed not loaded");

    if (first_startplayer == UINT_MAX - 1)
      throw ReadException("Party read: first startplayer not loaded");

    if (!rule)
      throw ReadException("Party read: rule not loaded");

    if (players.size() != (*rule)(Rule::NUMBER_OF_PLAYERS_IN_GAME))
      throw ReadException("Party read: incorrect number of players");

  } catch (...) {
    delete rule;
    for (vector<Player*>::iterator p = players.begin();
         p != players.end();
         ++p)
      delete *p;
    for (vector<GameSummary*>::iterator g = game_summaries.begin();
         g != game_summaries.end();
         ++g) {
      delete *g;
    }
    throw;
  } // try

  ::game_status = GAMESTATUS::PARTY_LOADED;
  delete this->rule_;
  this->rule_ = rule;

  for (vector<Player*>::iterator p = this->players_.begin();
       p != this->players_.end();
       ++p)
    delete *p;
  this->players_ = players;

  this->seed_first_
    = this->real_seed_first_
    = first_seed;
  this->startplayer_first_
    = this->real_startplayer_first_
    = first_startplayer;

  for (vector<GameSummary*>::iterator g = this->game_summaries_.begin();
       g != this->game_summaries_.end();
       ++g)
    delete *g;
  this->game_summaries_.clear();
  this->bock_multipliers_.clear();

  if (game_summaries.empty()) {
    this->seed_ = this->seed_first();
    this->startplayer_ = this->startplayer_first();
    this->round_startgame_.clear();
  } else { // if !(game_summaries.empty())
    this->real_seed_first_ = game_summaries.front()->seed();
    this->real_startplayer_first_ = game_summaries.front()->startplayer_no();
    this->seed_ = game_summaries.back()->seed();
    this->startplayer_ = game_summaries.back()->startplayer_no();

    { // calculate round startgame
      unsigned gameno = 0;
      this->round_startgame_.clear();
      this->round_startgame_.push_back(0);
      for (vector<GameSummary*>::const_iterator
           gs = game_summaries.begin();
           gs != game_summaries.end();
           ++gs, ++gameno) {
        // check for a new round
        // ToDo: in the duty soli round not all players do play, so the check of the startplayer should be able to accept skipped players
        if (   (gameno > 0)
            && (this->last_game_summary().startplayer_no()
                != this->real_startplayer_first())
            && (this->last_game_summary().next_startplayer_no()
                == this->real_startplayer_first())
            && !this->is_duty_soli_round()) {
          this->round_startgame_.push_back(this->finished_games());
        }
        // check for duty soli round
        if (!this->is_duty_soli_round()
            && (   (   this->rule()(Rule::NUMBER_OF_ROUNDS_LIMITED)
                    && (this->roundno()
                        >= this->rule()(Rule::NUMBER_OF_ROUNDS))
                    && !this->last_game_summary().startplayer_stays()
                    && (this->last_game_summary().next_startplayer_no()
                        == this->real_startplayer_first())
                   )
                || (   this->rule()(Rule::POINTS_LIMITED)
                    && (this->remaining_points() <= 0) )
               ) ) {
          this->duty_soli_round_ = this->roundno();
        }
        this->add_game_summary(*gs);
      } // for (gs \in game_summaries)

      // We have to check for a new round because of the summary window after loading a party
      // check for a new round
      if (   (gameno > 0)
          && (this->last_game_summary().startplayer_no()
              != this->real_startplayer_first())
          && (this->last_game_summary().next_startplayer_no()
              == this->real_startplayer_first())
          && !this->is_duty_soli_round()) {
        this->round_startgame_.push_back(this->finished_games());
      }
      if (!this->is_duty_soli_round()
          && (   (   this->rule()(Rule::NUMBER_OF_ROUNDS_LIMITED)
                  && (this->roundno()
                      >= this->rule()(Rule::NUMBER_OF_ROUNDS)) )
              || (   this->rule()(Rule::POINTS_LIMITED)
                  && (this->remaining_points() <= 0) )
             ) ) {
        this->duty_soli_round_ = this->roundno();
      }

      if (this->is_duty_soli_round()) {
        if (   !this->game_summaries_.empty()
            && !this->last_game_summary().is_duty_solo()
            && !this->round_startgame_.empty()
            && (this->round_startgame_.back() != this->gameno())
           ) {
          this->round_startgame_.push_back(this->gameno());
        }
      } // if (this->is_duty_soli_round())
    } // calculate round startgame

    // we want to start the following game
    if (   (   !this->is_duty_soli_round()
            && !this->last_game_summary().startplayer_stays())
        || (   this->is_duty_soli_round()
            && (this->last_game_summary().soloplayer_no()
                == this->startplayer()) )
       )
      (this->startplayer_ += 1) %= this->playerno();
    this->seed_next();
  } // if !(this->game_summaries_.empty())

  return (istr.good() || istr.eof());
} // bool Party::read(istream& istr)

/**
 ** saves the party in the given file
 **
 ** @param	filename	file to save the party in
 **
 ** @return	whether the party could be saved
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.9
 **/
bool
Party::save(string const& filename) const
{
  string filename_tmp = filename + ".tmp";
  ofstream ostr(filename_tmp.c_str());
  if (!ostr.good()) {
    ::ui->information(::translation("Error::Party::save: Error opening temporary file %stmpfile%. Aborting saving.", filename_tmp), INFORMATION::PROBLEM);
    return false;
  }

  ostr
    << "#!FreeDoko\n"
    << "# FreeDoko party (version " << *::version << ")\n"
    << '\n';
  this->write(ostr);

  if (!ostr.good()) {
    ::ui->information(::translation("Error::Party::save: Error saving in temporary file %stmpfile%. Keeping temporary file (for bug tracking).", filename_tmp), INFORMATION::PROBLEM);
    return false;
  }
  ostr.close();

#ifdef WINDOWS
  unlink(filename.c_str());
#endif
  if (rename(filename_tmp.c_str(), filename.c_str())) {
    ::ui->information(::translation("Error::Party::save: Could not rename temporary file %stmpfile% to requested file %sfile%. Keeping temporary file.", filename_tmp, filename), INFORMATION::PROBLEM);
    return false;
  }

  return true;
} // bool Party::save(string const& filename) const

/**
 ** loads the party in the given file
 **
 ** @param	filename	file to load the party from
 **
 ** @return	whether the loading was successful
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.9
 **/
bool
Party::load(string const& filename)
throw (ReadException)
{
  try {
    ifstream istr(filename.c_str());
    if (istr.fail())
      throw ReadException("Party load: open failed");

    this->read(istr);

    if (!istr.good() && !istr.eof())
      throw ReadException("Party load: unknown error");
  } catch (ReadException const& exception) {
    ::ui->information(::translation("Message::party load: load error '%sfilename%'",
                                    filename)
                      + "\n"
                      + exception.message(),
                      INFORMATION::PROBLEM);
    return false;
  } // try

  return true;
} // bool Party::load(string const& filename)

/**
 ** load the players
 ** loads the files 'player.?' from the FreeDoko private directory
 **
 ** @param	-
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.4
 **/
bool
Party::load_players() throw (ReadException)
{
  bool loaded_fail = false;
  for (unsigned p = 0; p < this->playerno(); ++p) {
    Player* player = Player::create(::setting(Setting::PRIVATE_DATA_DIRECTORY)
                                    + "/player."
                                    + DK::Utils::String::to_string(p + 1));
    if (player)
      this->set_player(p, player);
    else
      loaded_fail = true;
  } // for (n < this->playerno())

  return !loaded_fail;
} // bool Party::load_players()

/**
 ** inits the party:
 ** get the info of the first startplayer, the first seed,
 ** the ruleset and the players
 **
 ** @param	-
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **/
void
Party::init()
{
  for (vector<Player*>::const_iterator player = this->players().begin();
       player != this->players().end();
       player++)
    ::ui->name_changed(**player);

  if (   dynamic_cast<UI_Wrap*>(::ui)
      && (dynamic_cast<UI_Wrap*>(::ui)->ui->type() == UI_TYPE::DUMMY)) {
    for (unsigned p = 0; p < this->playerno(); ++p)
      if (this->player(p).type() == Player::HUMAN)
        this->set_playertype(p, Player::AI);
  } // if (::ui->type() == UI_TYPE::NONE)


  // reset the points
  this->points_ = 0;
  this->duty_soli_round_ = UINT_MAX;

  // save the starting values
  this->real_seed_first_ = this->seed_;
  this->real_startplayer_first_ = this->startplayer_;

  if (::setting(Setting::SAVE_PARTY_CHANGES))
    this->save_changes();

  return ;
} // void Party::init()

/**
 ** the party is opened
 **
 ** @param	-
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **/
void
Party::open()
{
  return ;
} // void Party::open()

/**
 ** play the party:
 ** play games until the user ends the party
 ** or the party is ended according to the rules
 **
 ** @param	-
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **/
void
Party::play()
{
  using namespace GAMESTATUS;

  if (this->seed() == UINT_MAX)
    this->set_seed(this->seed());
  if (this->startplayer() == UINT_MAX)
    this->set_startplayer(this->startplayer());

  ::ui->party_start();
  if (this->gameno() == 0) {
    this->round_startgame_.push_back(0);
    ::ui->party_start_round(this->roundno());
  }

  try {
    while (::game_status == PARTY_PLAY) {
      // check for a new round
      if (   (this->gameno() > 0)
          && (this->startplayer() == this->real_startplayer_first())
          && !this->is_duty_soli_round()
          && !this->game_summaries().back()->startplayer_stays()) {
        // new round
        // when a party is loaded, the round startgame is already set
        if (this->round_startgame().back() < this->finished_games())
          this->round_startgame_.push_back(this->finished_games());
        ::ui->party_start_round(this->roundno());
      } // if (new round)

      // check for duty soli round
      if (!this->is_duty_soli_round()
          && (   (   this->rule()(Rule::NUMBER_OF_ROUNDS_LIMITED)
                  && (this->roundno()
                      >= this->rule()(Rule::NUMBER_OF_ROUNDS)) )
              || (   this->rule()(Rule::POINTS_LIMITED)
                  && (this->remaining_points() <= 0) )
             ) ) {
        this->duty_soli_round_ = this->roundno();
      }

      if (   this->is_duty_soli_round()
          && (this->remaining_duty_soli() == 0)) {
        ::game_status = GAMESTATUS::PARTY_FINISHED;
        break;
      }

      if (this->is_duty_soli_round()) {
        if (   !this->game_summaries_.empty()
            && !this->last_game_summary().is_duty_solo())
          if (!this->starts_new_round(this->gameno())) {
            this->round_startgame_.push_back(this->gameno());
          }
        // search the next player to play a duty solo
        while (!remaining_duty_soli(this->startplayer()))
          (this->startplayer_ += 1) %= this->playerno();
      } // if (this->is_duty_soli_round())

      // create a new game
      ::game_status = GAME_NEW;
      this->game_ = new Game(*this, *(this->players_[this->startplayer()]));
      try { // play the game
        this->game().play();
        if (FAST_PLAY & FAST_NS::SEED_INFO) {
          for (vector<Player*>::const_iterator p = this->players().begin();
               p != this->players().end();
               ++p)
            (*p)->game_close(this->game());
          ::ui->game_close();

          delete this->game_;
          this->game_ = NULL;
          this->seed_next();
          ::game_status = PARTY_PLAY;
          continue;
        } // if (FAST_PLAY & FAST_NS::SEED_INFO)

        DEBUG_ASSERTION((::game_status == GAME_PLAY),
                        "Party::play():\n"
                        "  wrong gamestatus (needs 'GAME_PLAY'): "
                        << ::game_status);

        ::game_status = GAME_FINISHED;
        this->game().finish();

        GameSummary* const game_summary = new GameSummary(this->game());
        this->add_game_summary(game_summary);
        for (unsigned p = 0; p < this->playerno(); ++p)
          this->player(p).db_->evaluate(this->player(p),
                                        *game_summary);

        if (::setting(Setting::SAVE_PARTY_CHANGES))
          this->save_changes();

        ::ui->game_finish();
        ::game_status = GAME_CLOSED;
      } catch (...) { // play the game
        for (vector<Player*>::const_iterator p = this->players().begin();
             p != this->players().end();
             ++p)
          (*p)->game_close(this->game());
        ::ui->game_close();

        delete this->game_;
        this->game_ = NULL;

        throw;
      } // try

      for (vector<Player*>::const_iterator p = this->players().begin();
           p != this->players().end();
           ++p)
        (*p)->game_close(this->game());
      ::ui->game_close();

#ifdef CHECK_RUNTIME
      cout << '\n' << ::runtime << endl;
#endif

      if (this->is_last_game())
        ::game_status = GAMESTATUS::PARTY_FINISHED;
      else
        ::game_status = PARTY_PLAY;

      if (!this->game().startplayer_stays()) {
        (this->startplayer_ += 1) %= this->playerno();
      }

      delete this->game_;
      this->game_ = NULL;

      if (this->rule()(Rule::MUTATE)) {
        this->rule_->mutate();
      }
      this->seed_next();
    } // while (game_status == PARTY_PLAY)

    this->round_startgame_.push_back(this->finished_games());
    ::game_status = GAMESTATUS::PARTY_FINISHED;
  } catch (GameStatus const& gamestatus) {
    if (gamestatus != GAMESTATUS::PARTY_FINISHED)
      throw;

    ::game_status = GAMESTATUS::PARTY_FINISHED;
  } // try

  if (::setting(Setting::SAVE_PARTY_CHANGES))
    this->remove_auto_save();

  return ;
} // void Party::play()

/**
 ** the party is closed
 **
 ** @param	-
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **/
void
Party::close()
{
  this->round_startgame_.clear();

  for (vector<GameSummary*>::iterator gs = this->game_summaries_.begin();
       gs != this->game_summaries_.end();
       ++gs)
    delete *gs;
  this->game_summaries_.clear();

  this->bock_multipliers_.clear();

  return ;
} // void Party::close()

/**
 ** the rule has changed
 **
 ** @param	type        rule type to have changed
 ** @param	old_value   the old value of the rule
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.8
 **/
void
Party::rule_changed(int const type, void const* const old_value)
{
  for (vector<Player*>::iterator player = this->players().begin();
       player != this->players().end();
       ++player)
    (*player)->rule_changed(type, old_value);

  return ;
} // void Party::rule_changed(int const type, void const* const old_value)

/**
 ** save the file
 **
 ** @param	-
 **
 ** @return	whether the saving was successful
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.2
 **
 ** @todo       autosave in network game
 **/
bool
Party::auto_save() const
{
  // save the party in the private data directory
  return this->save(::setting(Setting::PRIVATE_DATA_DIRECTORY)
                    + "/"
                    + ::setting(Setting::PARTIES_DIRECTORY)
                    + "/"
                    + "current");
} // bool Party::auto_save() const

/**
 ** remove the auto saved file
 **
 ** @param	-
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **/
void
Party::remove_auto_save() const
{
  std::remove((::setting(Setting::PRIVATE_DATA_DIRECTORY)
               + "/"
               + ::setting(Setting::PARTIES_DIRECTORY)
               + "/"
               + "current").c_str());

  return;
} // void Party::remove_auto_save() const

/**
 ** save the changes (rules and players)
 **
 ** @param	-
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.9
 **/
void
Party::save_changes() const
{
#ifdef NETWORK
  if (!::server->empty())
    return ;
#endif

  ::ui->set_busy();
  try {
    this->auto_save();
    // save the rules in the private data directory
    this->rule().save(::setting(Setting::PRIVATE_DATA_DIRECTORY)
                      + "/"
                      + ::setting(Setting::RULES_DIRECTORY)
                      + "/"
                      + "last");

    { // save the players in the private data directory
      unsigned n = 0;
      for (vector<Player*>::const_iterator player = this->players().begin();
           player != this->players().end();
           ++n, ++player) {
        if ((*player)->type() != Player::NETWORK) {
          (*player)->save(::setting(Setting::PRIVATE_DATA_DIRECTORY)
                          + "/player." + DK::Utils::String::to_string(n + 1));
        }
      }
    } // save the players in the private data directory
    ::ui->set_not_busy();
  } catch(...) {
    ::ui->set_not_busy();
    throw;
  } // try

  return ;
} // void Party::save_changes() const

/**
 ** write the party into the output stream
 **
 ** @param	ostr	output stream
 ** @param	party	party
 **
 ** @return	output stream
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.9
 **/
ostream&
operator<<(ostream& ostr, Party const& party)
{
  party.write(ostr);

  return ostr;
} // ostream& operator<<(ostream& ostr, Party const& party)
