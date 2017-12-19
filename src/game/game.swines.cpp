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

#include "gameplay_actions/swines.h"

#include "../player/player.h"
#include "../party/party.h"
#include "../party/rule.h"
#include "../ui/ui.h"
#ifdef NETWORK
#include "../network/server.h"
#endif

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	whether the second dolle goes over the first in the current trick
 **/
bool
Game::second_dolle_over_first() const
{
  return (   this->rule()(Rule::DOLLEN)
	  && !(   this->rule()(Rule::DOLLEN_FIRST_OVER_SECOND_WITH_SWINES)
	       && this->swines_owner())
	  && (  !this->is_last_trick()
	      ? this->rule()(Rule::DOLLEN_SECOND_OVER_FIRST)
	      : (this->rule()(Rule::DOLLEN_SECOND_OVER_FIRST)
		 != this->rule()(Rule::DOLLEN_CONTRARY_IN_LAST_TRICK))
	     )
	 );
} // bool Game::second_dolle_over_first() const

/**
 ** -> result
 **
 ** @param	card   card to check
 **
 ** @return	whether the card is a special card
 **             (according to the rules and the hand)
 **/
bool
Game::is_special(HandCard const& card) const
{
  if (card.is_empty())
    return false;

  // dolle
  if (   this->rule()(Rule::DOLLEN)
      && (card == Card::DOLLE))
    return true;

  // swines
  if (   this->rule()(Rule::SWINES)
      && (card == this->swine())
      && (   this->swines_owner()
	  || this->swines_announcement_valid(card.player())) )
    return true;

  // hyperswines
  if (   this->rule()(Rule::HYPERSWINES)
      && (card == this->hyperswine())
      && (   this->hyperswines_owner()
	  || this->hyperswines_announcement_valid(card.player())) )
    return true;

  return false;
} // bool Game::is_special(HandCard card) const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	a fox card
 **/
Card
Game::fox() const
{
  if (   this->rule()(Rule::DOLLEN)
      && (   GAMETYPE::is_normal(this->type())
	  || GAMETYPE::is_poverty(this->type())
	  || GAMETYPE::is_marriage(this->type())))
    return {this->trumpcolor(), Card::ACE};
  else
    return {};
} // Card Game::fox() const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	a dolle card
 **/
Card
Game::dolle() const
{
  if (   this->rule()(Rule::DOLLEN)
      && (   GAMETYPE::is_normal(this->type())
	  || GAMETYPE::is_poverty(this->type())
	  || GAMETYPE::is_marriage(this->type())
	  || GAMETYPE::is_color_solo(this->type())) )
    return Card::DOLLE;
  else
    return {};
} // Card Game::dolle() const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	a swine card
 **/
Card
Game::swine() const
{
  if (   this->rule()(Rule::SWINES)
      && (   GAMETYPE::is_normal(this->type())
	  || GAMETYPE::is_poverty(this->type())
	  || GAMETYPE::is_marriage(this->type())
	  || (   this->rule()(Rule::SWINES_IN_SOLO)
	      && GAMETYPE::is_color_solo(this->type())) ) )
    return {this->trumpcolor(), Card::ACE};
  else
    return {};
} // Card Game::swine()

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	a hyperswine card
 **/
Card
Game::hyperswine() const
{
  if (   this->rule()(Rule::HYPERSWINES)
      && (   GAMETYPE::is_normal(this->type())
	  || GAMETYPE::is_poverty(this->type())
	  || GAMETYPE::is_marriage(this->type())
	  || (   this->rule()(Rule::HYPERSWINES_IN_SOLO)
	      && GAMETYPE::is_color_solo(this->type())) ) )
    return {this->trumpcolor(),
      this->rule()(Rule::WITH_NINES) ? Card::NINE : Card::KING};
  else
    return {};
} // Card Game::hyperswine()

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	the player who has swines
 **		NULL, if no player has swines	
 **/
Player const*
Game::swines_owner() const
{
  if (!this->swines_announced())
    return nullptr;

  return this->swines_owner_;
} // Player const* Game::swines_owner() const

/**
 ** -> result
 ** for the rules look into the code
 ** In a reservation 'true' can be returned although the announcement cannot
 ** be made (see Game::swines_announce).
 ** This is needed for the sorting in solo games.
 **
 ** @param	player	the player who wants to announce swines
 **
 ** @return	whether 'player' can announce swines
 **/
bool
Game::swines_announcement_valid(Player const& player) const
{
  // rule must allow swines
  if (!this->rule()(Rule::SWINES))
    return false;

  // the swines must not be announced already
  if (   this->swines_announced()
      && (player == *this->swines_owner()))
    return false;

  // check game
  switch (this->type()) {
  case GAMETYPE::NORMAL:
    break;
  case GAMETYPE::THROWN_NINES:
  case GAMETYPE::THROWN_KINGS:
  case GAMETYPE::THROWN_NINES_AND_KINGS:
  case GAMETYPE::THROWN_RICHNESS:
  case GAMETYPE::FOX_HIGHEST_TRUMP:
    return false;
  case GAMETYPE::POVERTY:
    if (!this->rule()(Rule::SWINES_IN_POVERTY))
      return false;
    break;
  case GAMETYPE::GENSCHER:
  case GAMETYPE::MARRIAGE:
  case GAMETYPE::MARRIAGE_SOLO:
  case GAMETYPE::MARRIAGE_SILENT:
    break;
  case GAMETYPE::SOLO_JACK:
  case GAMETYPE::SOLO_QUEEN:
  case GAMETYPE::SOLO_KING:
  case GAMETYPE::SOLO_QUEEN_JACK:
  case GAMETYPE::SOLO_KING_JACK:
  case GAMETYPE::SOLO_KING_QUEEN:
  case GAMETYPE::SOLO_KOEHLER:
    return false;
  case GAMETYPE::SOLO_CLUB:
  case GAMETYPE::SOLO_SPADE:
  case GAMETYPE::SOLO_HEART:
  case GAMETYPE::SOLO_DIAMOND:
    if (!this->rule()(Rule::SWINES_IN_SOLO))
      return false;
    break;
  case GAMETYPE::SOLO_MEATLESS:
    return false;
  } // switch (this->type())

  // check 'fox do not count' in poverty
  if (   (this->type() == GAMETYPE::POVERTY)
      && (player == (::game_status == GAMESTATUS::GAME_RESERVATION
                     ? this->player_current()
                     : this->soloplayer()) )
      && (player.hand().numberofpovertycards()
          > this->rule()(Rule::MAX_NUMBER_OF_POVERTY_TRUMPS)) )
    return false;

  // check cards number
  if (   (this->rule()(Rule::SWINE_ONLY_SECOND)
          ? (player.hand().numberoftrumpaces() != 1)
          : (player.hand().numberoftrumpaces()
             != this->rule()(Rule::NUMBER_OF_SAME_CARDS))) )
    return false;

  // special case: shifted swines
  if (   (::game_status == GAMESTATUS::GAME_POVERTY_SHIFT)
      && this->swines_announced())
    return true;

  // 'swines announcement begin': must be in the reservation
  // (in a poverty before the shifting/accepting)
  if (   this->rule()(Rule::SWINES_ANNOUNCEMENT_BEGIN)
      && (   (::game_status == GAMESTATUS::GAME_START)
          || (::game_status == GAMESTATUS::GAME_INIT)
          || (::game_status == GAMESTATUS::GAME_RESERVATION)
          || (   (::game_status == GAMESTATUS::GAME_POVERTY_SHIFT)
              && (   this->poverty_cards().empty()
                  || (player.team() == TEAM::UNKNOWN))
              && !this->poverty_shifted())
          || (   (this->type() != GAMETYPE::POVERTY)
              && (this->played_cards_no() == 0) )
     )
      && !this->poverty_shifted()) {
    return true;
  }

  // normal case, announcement in the game: can announce
  if (   !this->rule()(Rule::SWINES_ANNOUNCEMENT_BEGIN)
      && !this->rule()(Rule::SWINE_ONLY_SECOND))
    return true;

  // 'swine only second': first fox must be caught by the same team
  if (   this->rule()(Rule::SWINE_ONLY_SECOND)
      && (player.hand().numberofalltrumpaces()
          == this->rule()(Rule::NUMBER_OF_SAME_CARDS))
      && (this->first_fox_catcher())
      && (   (this->teaminfo(*this->first_fox_catcher())
              == this->teaminfo(player))
          && (   ::is_real(this->teaminfo(player))
              || (*this->first_fox_catcher() == player) )
         )
     )
    return true;

  return false;
} // bool Game::swines_announcement_valid(Player player) const

/**
 ** announce swines if they can be announced and they have not yet been
 ** announced
 **
 ** @param	player	player who wants to announce swines
 **
 ** @return	whether the swines are announced
 **/
bool
Game::swines_announce(Player& player)
{
  if (!this->swines_announcement_valid(player))
    return false;

  // in a reservation no announcement is allowed,
  // but 'valid' returns 'true', because of the cards ordering.
  // This is needed when solo games are selected
  if (::game_status == GAMESTATUS::GAME_RESERVATION)
    return false;

  this->swines_owner_ = &player;
  this->swines_announced_ = true;
  this->swines_owner_->hand_sort();
  // sort the hand of all players because of possible hyperswines

  if (this->rule()(Rule::SWINE_ONLY_SECOND))
    // the swines owner has to say his team
    this->teaminfo_update();

  if (!this->isvirtual()) {
    ::ui->swines_announced(*this->swines_owner());
    ::ui->gameplay_action(GameplayAction::Swines(this->no(*this->swines_owner())));
  } // if (!this->isvirtual())

  for (auto& p : this->players()) {
    p->swines_announced(player);
    if (p->hand().has_hyperswines())
      p->hand_sort();
  }

  { // update 'this->cards_[Card::TRUMP]'
    for (auto t = this->cards_[Card::TRUMP].begin();
         t != this->cards_[Card::TRUMP].end();
         ++t) {
      if (*t == this->swine()) {
        this->cards_[Card::TRUMP].erase(t);
        break;
      }
    } // for (t \in this->trump_)
    this->cards_[Card::TRUMP].push_back(this->swine());
  } // update 'this->cards_[Card::TRUMP]'

  return true;
} // bool Game::swines_announce(Player& player)

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	the player who has hyperswines
 **		NULL, if no player has hyperswines	
 **/
Player const*
Game::hyperswines_owner() const
{
  if (!this->hyperswines_announced())
    return nullptr;

  return this->hyperswines_owner_;
} // Player const* Game::hyperswines_owner() const

/**
 ** -> result
 ** for the rules look into the code
 **
 ** @param	player	the player who wants to announce hyperswines
 **
 ** @return	whether 'player' can announce hyperswines
 **/
bool
Game::hyperswines_announcement_valid(Player const& player) const
{
  // check rule 'hyperswines'
  if (!this->rule()(Rule::HYPERSWINES))
    return false;

  // check rule 'hyperswines in solo'
  if (   GAMETYPE::is_solo(this->type())
      && !this->rule()(Rule::HYPERSWINES_IN_SOLO))
    return false;

  // check rule 'hyperswines in poverty'
  if (   GAMETYPE::is_poverty(this->type())
      && !this->rule()(Rule::HYPERSWINES_IN_POVERTY))
    return false;

#ifdef WORKAROUND
  // in the following case we have a problem
  // * virtual games
  // * cards informations
  //   - the player can have diamond ace and diamond nine 
  //   - the player cannot have all four cards (2 diamond ace, 2 diamond nines)
  // @todo   use 'remaining_cards_no(player) < 4'
  if (   this->isvirtual()
      && !this->hyperswines_announced())
    return false;
#endif

  // swines must be announced or must be valid
  if (!(   this->swines_announced()
        || this->swines_announcement_valid(player)))
    return false;

  // check for joint swines and hyperswines
  if (   !this->rule()(Rule::SWINES_AND_HYPERSWINES_JOINT)
      && (   (this->swines_owner() == &player)
          || (   !this->swines_announced()
              && this->swines_announcement_valid(player))))
    return false;

  // check for enough cards on the hand
  if (player.hand().numberof(this->hyperswine())
      != this->rule()(Rule::NUMBER_OF_SAME_CARDS))
    return false;

  // special case: shifted hyperswines
  if (   (::game_status == GAMESTATUS::GAME_POVERTY_SHIFT)
      && (   (this->player_current().team() == TEAM::UNKNOWN)
          || this->hyperswines_owner_) )
    return true;

  // the hyperswines must not be announced
  if (this->hyperswines_announced())
    return false;

  // check for right announcement time
  if (   this->rule()(Rule::HYPERSWINES_ANNOUNCEMENT_BEGIN)
      && (   (::game_status == GAMESTATUS::GAME_INIT)
          || (::game_status == GAMESTATUS::GAME_RESERVATION)
          || (   (::game_status == GAMESTATUS::GAME_POVERTY_SHIFT)
              && (   this->poverty_cards().empty()
                  || (player.team() == TEAM::UNKNOWN))
              && !this->poverty_shifted())
          || (::game_status == GAMESTATUS::GAME_START) )
      && !this->poverty_shifted())
    return true;

  // normal case, announcement in the game: can announce
  if (!this->rule()(Rule::HYPERSWINES_ANNOUNCEMENT_BEGIN))
    return true;

  return false;
} // bool Game::hyperswines_announcement_valid(Player const& player) const

/**
 ** announce hyperswines if they can be announced
 ** If the player has not annouced 'swines', they are announced automatically
 **
 ** @param	player	player who wants to announce hyperswines
 **
 ** @return	whether the hyperswines are announced
 **/
bool
Game::hyperswines_announce(Player& player)
{
  if (!this->hyperswines_announcement_valid(player))
    return false;

  if (!this->swines_announced())
    this->swines_announce(player);

  // this check is needed again, since after the 'swines announce' a player
  // can alread have announced hyperswines
  if (!this->hyperswines_announcement_valid(player))
    return false;

  this->hyperswines_owner_ = &player;
  this->hyperswines_announced_ = true;
  this->hyperswines_owner_->hand_sort();

  if (!this->isvirtual()) {
    ::ui->hyperswines_announced(*this->hyperswines_owner());
    ::ui->gameplay_action(GameplayAction::Hyperswines(this->no(*this->hyperswines_owner())));
  } // if (!this->isvirtual())

  for (auto& p : this->players())
    p->hyperswines_announced(player);

  { // update 'this->cards_[Card::TRUMP]'
    for (auto t = this->cards_[Card::TRUMP].begin();
         t != this->cards_[Card::TRUMP].end();
         ++t) {
      if (*t == this->hyperswine()) {
        this->cards_[Card::TRUMP].erase(t);
        break;
      }
    } // for (t \in this->trump_)
    this->cards_[Card::TRUMP].push_back(this->hyperswine());
  } // update 'this->cards_[Card::TRUMP]'

  return true;
} // bool Game::hyperswines_announce(Player& player)

/**
 ** test the reservations for swines
 **
 ** @param	-
 **
 ** @return	-
 **/
void
Game::test_swines_from_reservations()
{
#ifdef NETWORK
  // the parent calls directly 'announce_swines()'
  if (::server->has_parent_connection())
    return ;
#endif

  if (!this->rule()(Rule::SWINES))
    return ;

  for (unsigned i = 0;
       i < this->number_of_players();
       i++,
       this->player_current_
       = &(this->player_following(this->player_current()))) {
    if (   this->reservation(this->player_current()).swines
        && this->swines_announcement_valid(this->player_current()) )
      this->swines_announce(this->player_current());

#ifdef OUTDATED
    // check, whether this is necessary.
    // It should be checked by the player, when he get known the gametype
    if (  GAMETYPE::is_solo(this->type())
        && (this->player_current() != this->soloplayer())
        && this->swines_announcement_valid(this->player_current() ) )
      this->swines_announce(this->player_current());
#endif

  } // for (p < number_of_players())

  if (this->rule()(Rule::SWINES_ANNOUNCEMENT_BEGIN)) {
    for (auto& player : this->players())
      player->check_swines_announcement_at_game_start();
  } // if (this->rule()(Rule::SWINES_ANNOUNCEMENT_BEGIN))

  return ;
} // void Game::test_swines_from_reservations()
