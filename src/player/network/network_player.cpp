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

#ifdef USE_NETWORK

#include "constants.h"

#include "network_player.h"

#include "../../network/connection.h"

#include "../../game/game.h"
#include "../../party/rule.h"
#include "../../card/trick.h"

#include "../../ui/ui.h"

/**
 ** constructor
 **
 ** @param     no   number of the player
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.13
 **/
NetworkPlayer::NetworkPlayer(Network::Connection& connection) :
  Ai(),
  connection_(&connection),
  pending_reservation(NULL),
  pending_cards_to_shift(NULL),
  pending_poverty_accept(UINT_MAX),
  pending_cards_to_play(),
  pending_genscher_partnerno(UINT_MAX),
  pending_trick_taken(false)
{
  this->set_type(Player::NETWORK);
  return ;
} // NetworkPlayer::NetworkPlayer(unsigned no, Network::Connection& connection)

/**
 ** Destructor
 **
 ** @param     -
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **      
 ** @version   0.7.2
 **/
NetworkPlayer::~NetworkPlayer()
{
  delete this->pending_reservation;
  delete this->pending_cards_to_shift;

  return ;
} // NetworkPlayer::~NetworkPlayer()

/**
 ** clone the player
 ** shall not be called
 **
 ** @param       -
 **
 ** @return      NULL
 **
 ** @author      Diether Knof
 **
 ** @version     0.7.2
 **/
NetworkPlayer*
NetworkPlayer::clone() const
{  
#ifdef POSTPONED
  // error when calling the 'players' window
  DEBUG_ASSERTION(false,
                  "call of 'NetworkPlayer::clone()");
#endif
  return NULL;
} // NetworkPlayer* NetworkPlayer::clone() const

/**
 ** remove the connection (t.i. convert the player into an ai)
 **
 ** @param     -
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.3
 **/
void
NetworkPlayer::remove_connection()
{
  DEBUG_ASSERTION((this->connection_ != NULL),
                  "NetworkPlayer::remove_connection()\n"
                  "  the player has no corresponding connection");

  this->connection_ = NULL;
  this->set_type(Player::AI);

  return ;
} // void NetworkPlayer::remove_connection()

/**
 ** a new game has opened
 **
 ** @param     game    opened game
 **
 ** @return    the reservation
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.13
 **/
void
NetworkPlayer::game_open(Game& game)
{
  this->Ai::game_open(game);

  if (this->pending_reservation != NULL) {
    delete this->pending_reservation;
    this->pending_reservation = NULL;
  }

  return ;
} // void NetworkPlayer::game_open(Game& game)

/**
 ** -> result
 **
 ** @param     duty_solo	whether the player has to play a solo
 ** 				(default: false)
 **
 ** @return    the reservation
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.5
 **
 ** @todo	overwork
 **/
Reservation const&
NetworkPlayer::reservation_get(bool const duty_solo)
{
  while (   !this->pending_reservation
         && (this->connection_ != NULL)
        )
    ::ui->wait();

  if (this->connection_ == NULL) {
    return this->Ai::reservation_get(duty_solo);
  }

  this->reservation() = *this->pending_reservation;

  delete this->pending_reservation;
  this->pending_reservation = NULL;

  // ToDo: move to 'game_start'
  this->pending_genscher_partnerno = UINT_MAX;

  return this->reservation();
} // Reservation const& NetworkPlayer::reservation_get(bool duty_solo = false)

/**
 ** returns which cards the player shifts
 **
 ** @param      -
 **
 ** @return     the cards that are to be shifted
 **
 ** @author     Diether Knof
 **
 ** @version    0.7.3
 **/
HandCards
NetworkPlayer::poverty_shift()
{ 
  while (   !this->pending_cards_to_shift
         && (this->connection_ != NULL)
        )
    ::ui->wait();

  if (this->connection_ == NULL)
    return this->Ai::poverty_shift();

  HandCards const cards_to_shift(this->hand(), *this->pending_cards_to_shift);

  delete this->pending_cards_to_shift;
  this->pending_cards_to_shift = NULL;

  this->sorted_hand().remove(cards_to_shift);

  return cards_to_shift;
} // HandCards NetworkPlayer::poverty_shift(Player player)

/**
 **
 ** returns whether 'player' accepts the shifted cards
 **
 ** @param      cardno  the number of shifted cards
 **
 ** @return     whether to accept the cards
 **
 ** @author     Diether Knof
 **
 ** @version    0.7.3
 **
 **/
bool
NetworkPlayer::poverty_take_accept(unsigned const cardno)
{ 
  while (   (this->pending_poverty_accept == UINT_MAX)
         && (this->connection_ != NULL)
        )
    ::ui->wait();

  if (this->connection_ == NULL)
    return this->Ai::poverty_take_accept(cardno);

  return (this->pending_poverty_accept == 1);
} // void NetworkPlayer::poverty_take_accept(unsigned cardno)

/**
 ** changes the cards from the poverty-player
 **
 ** @param      cards   the cards that are given to the player
 **
 ** @return     the cards that are returned to the poverty-player
 **
 ** @author     Diether Knof
 **
 ** @version    0.7.3
 **/
HandCards
NetworkPlayer::poverty_cards_change(HandCards const& cards)
{ 
  while (   !this->pending_cards_to_shift
         && (this->connection_ != NULL)
        )
    ::ui->wait();

  if (this->connection_ == NULL)
    return this->Ai::poverty_shift();

  HandCards const cards_to_shift(this->hand(), *this->pending_cards_to_shift);

  delete this->pending_cards_to_shift;
  this->pending_cards_to_shift = NULL;

  this->sorted_hand().add(cards);
  this->sorted_hand().remove(cards_to_shift);

  return cards_to_shift;
} // HandCards NetworkPlayer::poverty_cards_change(HandCards cards)

/**
 ** -> result
 ** taken from the network connection
 **
 ** @param     -
 **
 ** @return    valid card
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.2
 **
 ** @todo	overwork
 **/
HandCard
NetworkPlayer::card_get()
{
  while (   this->pending_cards_to_play.empty()
         && (this->connection_ != NULL)) {
    ::ui->wait();
  }

  if (this->connection_ == NULL)
    return this->Ai::card_get();

  HandCard const card = HandCard(this->hand(),
                                 this->pending_cards_to_play.front());
  this->lastHeuristic_ = Aiconfig::NETWORK;

  this->pending_cards_to_play.pop();


  return card;
} // HandCard NetworkPlayer::card_get()

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    partner for a genscher, 'NULL' if no partner
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.3
 **/
Player const*
NetworkPlayer::genscher_partner()
{
  if (this->connection_ == NULL)
    return this->Ai::genscher_partner();

  // The information is always already here, because the order of the actions
  // is 'genscher, card played', but this function is called after the
  // diamond king is played, so after the 'card played' action was received.
  if (this->pending_genscher_partnerno == UINT_MAX)
    return NULL;

  Player const* partner
    = ((this->pending_genscher_partnerno == this->game().no(*this))
       ? static_cast<Player const*>(NULL)
       : &this->game().player(this->pending_genscher_partnerno));

  this->pending_genscher_partnerno = UINT_MAX;

  return partner;
} // Player const* NetworkPlayer::genscher_partner()

/**
 ** the trick is taken
 ** wait for the player to have it closed
 ** 
 ** @param     -
 ** 
 ** @return    -
 ** 
 ** @author    Diether Knof
 ** 
 ** @version   0.7.4
 **/ 
void
NetworkPlayer::trick_taken()
{
  while (   !this->pending_trick_taken
         && (this->connection_ != NULL)
        )
    ::ui->wait();

  this->pending_trick_taken = false;

  return ;
} // void NetworkPlayer::trick_taken()

/**
 ** set the reservation to make
 **
 ** @param     reservation   reservation to make
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.3
 **/
void
NetworkPlayer::set_reservation(Reservation const& reservation)
{
  DEBUG_ASSERTION((this->pending_reservation == NULL),
                  "NetworkPlayer::set_reservation(reservation)\n"
                  "  the reservation is already set");

  this->pending_reservation = new Reservation(reservation);

  return ;
} // void NetworkPlayer::set_reservation(Reservation reservation)

/**
 ** set the cards shifted in a poverty
 **
 ** @param     cards   cards to shift
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.3
 **/
void
NetworkPlayer::set_poverty_cards_to_shift(vector<Card> const& cards)
{
  DEBUG_ASSERTION((this->pending_cards_to_shift == NULL),
                  "NetworkPlayer::set_poverty_cards_to_shift(cards)\n"
                  "  the cards are already set");

  this->pending_cards_to_shift = new vector<Card>(cards);

  return ;
} // void NetworkPlayer::set_poverty_cards_to_shift(vector<Card> cards)

/**
 ** set whether to accept the poverty
 **
 ** @param     accept   whether to accept the poverty
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.3
 **/
void
NetworkPlayer::set_poverty_accept(bool const accept)
{
  this->pending_poverty_accept = (accept ? 1 : 0);

  return ;
} // void NetworkPlayer::set_poverty_accept(bool accept)

/**
 ** set the announcement to make
 **
 ** @param     reservation   reservation to make
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.3
 **/
void
NetworkPlayer::set_announcement(Announcement const& announcement)
{
#ifdef WORKAROUND
  // seed: 5
  if (this->game().announcement_of_player(*this) == announcement)
    return ;
#endif
  DEBUG_ASSERTION(this->game().announcement_valid(announcement, *this),
                  "NetworkPlayer::set_announcement(announcement)\n"
                  "  announcement '" << announcement << "' not valid "
                  "for player " << this->game().no(*this) << '\n'
                  << "  announcement: " << this->game().announcement_of_player(*this) << '\n'
                  << "  game status: " << ::game_status);
  this->game().make_announcement(announcement, *this);

  return ;
} // void NetworkPlayer::set_announcement(Announcement announcement)

/**
 ** set the partner for the genscher
 **
 ** @param     partnerno
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.3
 **/
void
NetworkPlayer::set_genscher_partner(unsigned const partnerno)
{
  if (partnerno == UINT_MAX)
    this->pending_genscher_partnerno = this->game().no(*this);
  else
    this->pending_genscher_partnerno = partnerno;

  return ;
} // void NetworkPlayer::set_genscher_partner(unsigned const partnerno)

/**
 ** set the card to play
 **
 ** @param     card   card to play
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.3
 **/
void
NetworkPlayer::set_card_to_play(Card const& card)
{
  this->pending_cards_to_play.push(card);

  return ;
} // void NetworkPlayer::set_card_to_play(Card card)

/**
 ** set that swines are to be announced
 **
 ** @param     -
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.3
 **/
void
NetworkPlayer::set_swines_announcement()
{
  this->game().swines_announce(*this);

  return ;
} // void NetworkPlayer::set_swines_announcement()

/**
 ** set that hyperswines are to be announced
 **
 ** @param     -
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.3
 **/
void
NetworkPlayer::set_hyperswines_announcement()
{
  this->game().hyperswines_announce(*this);

  return ;
} // void NetworkPlayer::set_hyperswines_announcement()

/**
 ** set that the trick has been taken
 **
 ** @param     -
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
void
NetworkPlayer::set_trick_taken()
{
  this->pending_trick_taken = true;

  return ;
} // void NetworkPlayer::set_trick_taken()

#endif // #ifdef USE_NETWORK
