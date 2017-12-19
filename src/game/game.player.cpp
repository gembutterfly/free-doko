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

#include "constants.h"

#include "game.h"

#include "../party/party.h"
#include "../party/rule.h"
#include "../ui/ui.h"
#include "../misc/setting.h"
#include "../player/human/human.h"

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	the players
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.13
 **/
vector<Player*> const&
Game::players_const() const
{
  return this->players_;
} // vector<Player*> Game::players_const()

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	the players
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.13
 **/
vector<Player*>&
Game::players()
{
  return this->players_;
} // vector<Player*>& Game::players()

/**
 ** set the players
 **
 ** @param	the players
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.13
 **/
void
Game::set_players(vector<Player*> const& players)
{
  DEBUG_ASSERTION((players.size() == this->number_of_players()),
                  "Game::set_players(players)\n"
                  "  Size of players = " << players.size() << " != " 
                  << this->number_of_players() << " = number of players in game");
  for (vector<Player*>::const_iterator p = players.begin();
       p != players.end();
       ++p)
    DEBUG_ASSERTION((*p != NULL),
                    "Game::set_players(players)\n"
                    "  one player is NULL");
  DEBUG_ASSERTION((::game_status <= GAMESTATUS::GAME_INIT),
                  "Game::set_players(players)\n"
                  "  game status = " << ::game_status << " is not game init.");
  this->players_ = players;
  for (unsigned p = 0; p < players.size(); ++p) {
    if (this->players_[p] != players[p]) {
      this->players_[p] = players[p];
      Player& player = *players[p];
      player.new_game(*this);
      player.game_open(*this);
      player.new_game(*this);
    }
  }

  return;
} // void Game::set_players(vector<Player*> players)

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	number of players in the game
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.13
 **/
unsigned
Game::number_of_players() const
{
  return this->rule()(Rule::NUMBER_OF_PLAYERS_IN_GAME);
} // unsigned Game::number_of_players() const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	number of humans in the game
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.13
 **/
unsigned
Game::number_of_humans() const
{
  // count the number of human players
  unsigned human_no = 0;
  for (vector<Player*>::const_iterator player = this->players_begin();
       player != this->players_end();
       player++)
    if ((*player)->type() == Player::HUMAN)
      human_no += 1;

  return human_no;
} // unsigned Game::number_of_humans() const

/**
 **
 ** -> result
 **
 ** @param	-
 **
 ** @return	human player if there exists exactly one,
 **		else 'NULL'
 **
 ** @version	0.6.2
 **
 ** @author	Diether Knof
 **
 **/
Human*
Game::human_player()
{
  if (this->number_of_humans() != 1)
    return NULL;

  for (vector<Player*>::iterator p = this->players().begin();
       p != this->players().end();
       ++p)
    if ((*p)->type() == Player::HUMAN)
      return static_cast<Human*>(*p);

  return NULL;
} // Human* Game::human_player()

/**
 **
 ** -> result
 **
 ** @param	-
 **
 ** @return	human player if there exists exactly one,
 **		else 'NULL'
 **
 ** @version	0.6.2
 **
 ** @author	Diether Knof
 **
 **/
Human const*
Game::human_player() const
{
  if (this->number_of_humans() != 1)
    return NULL;

  for (vector<Player*>::const_iterator p = this->players().begin();
       p != this->players().end();
       ++p)
    if ((*p)->type() == Player::HUMAN)
      return static_cast<Human*>(*p);

  return NULL;
} // Human const* Game::human_player() const

/**
 **
 ** -> result
 **
 ** @param	p	number of player which should be result of this method
 **
 ** @return	player number 'p'
 **
 ** @version	0.4.4
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 **
 **/
Player const&
Game::player(unsigned const& p) const
{
  DEBUG_ASSERTION((p < this->number_of_players()),
                  "Game::player(p):\n"
                  "  'p' too great: " << p);

  return *this->players_[p];
} // Player const& Game::player(unsigned const& p) const

/**
 **
 ** -> result
 **
 ** @param	p	number of player which should be result of this method
 **
 ** @return	player number 'p'
 **
 ** @version	0.4.4
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 **
 **/
Player&
Game::player(unsigned const& p)
{
  DEBUG_ASSERTION((p < this->number_of_players()),
                  "Game::player(p):\n"
                  "  'p' too great: " << p);

  return *this->players_[p];
} // Player& Game::player(unsigned const& p)

/**
 ** -> result
 **
 ** @param	player   player to search
 **
 ** @return	position of the player, UINT_MAX if the player is not in the game
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.13
 **/
unsigned
Game::no(Player const& player) const
{
  unsigned n = 0;
  for (vector<Player*>::const_iterator p = this->players_begin();
       p != this->players_end();
       p++, n++)
    if (*p == &player)
      return n;
  return UINT_MAX;
} // unsigned Game::no(Player player) const

/**
 ** -> result
 **
 ** @param	player   player to search
 **
 ** @return	whether the player is in the game
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.13
 **/
bool
Game::contains(Player const& player) const
{
  for (vector<Player*>::const_iterator p = this->players_begin();
       p != this->players_end();
       p++)
    if (*p == &player)
      return true;
  return false;
} // bool Game::contains(Player player) const

/**
 **
 ** -> result
 **
 ** @param	player	player, whose following player is returned
 **
 ** @return	the next player after 'player'
 **
 ** @version	0.4.4
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 **
 **/
Player const&
Game::player_following(Player const& player) const
{
  unsigned n = 0;
  for (vector<Player*>::const_iterator p = this->players_begin();
       p != this->players_end();
       p++, n++)
    if (**p == player) {
      p++;
      if (p == this->players_end())
        // this was the last player - return the first
        return this->player(0);
      else
        return **p;
    } // if (**p == player)

  DEBUG_ASSERTION(false,
                  "Game::player_following(player):\n"
                  "  player not found in the game-players");

  return player;
} // Player const& Game::player_following(Player const& player) const

/**
 **
 ** -> result
 **
 ** @param	player	player, whose following player is returned
 **
 ** @return	the next player after 'player'
 **
 ** @version	0.4.4
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 **
 **/
Player&
Game::player_following(Player& player)
{
  unsigned n = 0;
  for (vector<Player*>::iterator p = this->players().begin();
       p != this->players().end();
       p++, n++)
    if (**p == player) {
      p++;
      if (p == this->players().end())
        // this was the last player - return the first
        return this->player(0);
      else
        return **p;
    } // if (**p == player)

  DEBUG_ASSERTION(false,
                  "Game::player_following(player):\n"
                  "  player not found in the game-players");

  return player;
} // Player& Game::player_following(Player& player)

/**
 ** -> result
 **
 ** @param	player	player, whose previous player is returned
 **
 ** @return	the player before 'player'
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.11
 **/
Player const&
Game::player_previous(Player const& player) const
{
  unsigned n = 0;
  for (vector<Player*>::const_reverse_iterator p = this->players_.rbegin();
       p != this->players_.rend();
       p++, n++)
    if (**p == player) {
      p++;
      if (p == this->players_.rend())
        // this was the first player - return the last
        return this->player(this->number_of_players() - 1);
      else
        return **p;
    } // if (**p == player)

  DEBUG_ASSERTION(false,
                  "Game::player_previous(player):\n"
                  "  player not found in the game-players");

  return player;
} // Player const& Game::player_previous(Player const& player) const

/**
 ** -> result
 **
 ** @param	p	number of player, whose previous player is returned
 **
 ** @return	the previous player of player p
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.11
 **/
Player const&
Game::player_previous(unsigned const& p) const
{
  return this->player_previous(this->player(p));
} // Player const& Game::player_previous(unsigned p) const

/**
 **
 ** -> result
 **
 ** @param	-
 **
 ** @return	begin-iterator of the player-vector
 **
 ** @version	0.4.4
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 **
 **/
vector<Player*>::iterator
Game::players_begin()
{
  return this->players().begin();
} // vector<Player*>::iterator Game::players_begin() const

/**
 **
 ** -> result
 **
 ** @param	-
 **
 ** @return	end-iterator of the player-vector
 **
 ** @version	0.4.4
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 **
 **/
vector<Player*>::iterator
Game::players_end()
{
  return this->players().end();
} // vector<Player*>::iterator Game::players_end() const

/**
 **
 ** -> result
 **
 ** @param	-
 **
 ** @return	begin-iterator of the player-vector
 **
 ** @version	0.4.4
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 **
 **/
vector<Player*>::const_iterator
Game::players_begin() const
{
  return this->players().begin();
} // vector<Player*>::const_iterator Game::players_begin() const

/**
 **
 ** -> result
 **
 ** @param	-
 **
 ** @return	end-iterator of the player-vector
 **
 ** @version	0.4.4
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 **
 **/
vector<Player*>::const_iterator
Game::players_end() const
{
  return this->players().end();
} // vector<Player*>::const_iterator Game::players_end() const

/**
 **
 ** sort the hands of the players
 **
 ** @param	-
 **
 ** @return	-
 **
 ** @version	0.4.4
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 **
 **/
void
Game::players_hands_sort()
{
  if (::setting(Setting::CARDS_ORDER).sorted()) {
    for (vector<Player*>::iterator player = this->players().begin();
         player != this->players().end();
         player++) {
      (*player)->hand_sort();
    } // for (player)
  } // if (::setting(Setting::CARDS_ORDER).sorted())

  return ;
} // void Game::players_hands_sort()

/**
 **
 ** mix the hands of the players
 **
 ** @param	-
 **
 ** @return	-
 **
 ** @version	0.4.4
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 **
 **/
void
Game::players_hands_mix()
{
  if (!::setting(Setting::CARDS_ORDER).sorted()) {
    for (vector<Player*>::iterator player = this->players().begin();
         player != this->players().end();
         player++) {
      (*player)->hand_mix();
    } // for (player)
  } // if (::setting(Setting::CARDS_ORDER).mixed())

  return ;
} // void Game::players_hands_mix()

/**
 **
 ** -> result
 **
 ** @param	-
 **
 ** @return	whether a soloplayer exists
 **
 ** @version	0.6.7
 **
 ** @author	Diether Knof
 **
 **/
bool
Game::exists_soloplayer() const
{
  return this->soloplayer_;
} // bool Game::exists_soloplayer() const

/**
 **
 ** -> result
 **
 ** @param	-
 **
 ** @return	soloplayer
 **
 ** @version	0.5
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 **
 **/
Player const&
Game::soloplayer() const
{
  DEBUG_ASSERTION(this->soloplayer_,
                  "Game::soloplayer:\n"
                  "  this->soloplayer_ == NULL");

  return *this->soloplayer_;
} // Player const& Game::soloplayer() const

/**
 **
 ** -> result
 **
 ** @param	-
 **
 ** @return	soloplayer
 **
 ** @version	0.5
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 **
 **/
Player&
Game::soloplayer()
{
  DEBUG_ASSERTION(this->soloplayer_,
                  "Game::soloplayer:\n"
                  "  this->soloplayer_ == NULL");

  return *this->soloplayer_;
} // Player& Game::soloplayer()


/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	the player who takes the poverty
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 **
 ** @version	0.7.4
 **/
Player const&
Game::poverty_partner() const
{
  DEBUG_ASSERTION((this->type() == GAMETYPE::POVERTY),
                  "Game::poverty_partner()\n"
                  "  gametype is not poverty: " << this->type());
  for (vector<Player*>::const_iterator player = this->players_begin();
       player != this->players_end();
       player++)
    if ((*player)->team() == TEAM::RE && 
        !(*player == this->soloplayer_ )) 
      return **player;

  return  **this->players_end();
} // Player Game::poverty_partner() const

/**
 ** -> result
 **
 ** @param	player   player to check
 **
 ** @return	whether 'player' is the soloplayer (or poverty / marriage player)
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.10
 **/
bool
Game::is_soloplayer(Player const& player) const
{
  return (   this->exists_soloplayer()
          && (this->soloplayer() == player));
} // bool Game::is_soloplayer(Player player) const

/**
 **
 ** -> result
 **
 ** @param	-
 **
 ** @return	player whose has started the game
 **
 ** @version	0.5
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 **
 **/
Player const&
Game::startplayer() const
{
  DEBUG_ASSERTION(this->startplayer_,
                  "Game::startplayer:\n"
                  "  this->startplayer_ == NULL");

  return *this->startplayer_;
} // Player const& Game::startplayer() const

/**
 **
 ** -> result
 **
 ** @param	-
 **
 ** @return	player whose turn it is
 **
 ** @version	0.5
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 **
 **/
Player const&
Game::player_current() const
{
  DEBUG_ASSERTION(this->player_current_,
                  "Game::player_current:\n"
                  "  this->player_current_ == NULL");

  return *this->player_current_;
} // Player const& Game::player_current() const

/**
 **
 ** -> result
 **
 ** @param	-
 **
 ** @return	player whose turn it is
 **
 ** @version	0.5
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 **
 **/
Player&
Game::player_current()
{
  DEBUG_ASSERTION(this->player_current_,
                  "Game::player_current:\n"
                  "  this->player_current_ == NULL");

  return *this->player_current_;
} // Player& Game::player_current()
