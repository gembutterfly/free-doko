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
 *   GNU General Public License for more detai_dummyls. 
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

#ifndef CLASS_NETWORK_PLAYER_HEADER
#define CLASS_NETWORK_PLAYER_HEADER

#ifdef USE_NETWORK

#include "../ai/ai.h"

namespace Network {
  class Connection;
}

// Class for a network player
// ToDo: use 'pending_gameplay_action_type'
class NetworkPlayer : public Ai {
  public:
    // constructor
    NetworkPlayer(Network::Connection& connection);

    // destructor
    ~NetworkPlayer();


    // remove the connection (t.i. convert the player into an ai)
    void remove_connection();

    // a new game is opened
    void game_open(Game& game);
    // the game request a reservation
    Reservation const& reservation_get(bool const duty_solo = false);
    // 'player' shifts cards
    HandCards poverty_shift();
    // returns whether 'player' accepts the poverty
    bool poverty_take_accept(unsigned const cardno);
    // the player changes the cards of the poverty
    HandCards poverty_cards_change(HandCards const& cards);
    // the game request a card to play
    HandCard card_get();
    // return a partner for a genscher
    Player const* genscher_partner();
    // the trick is taken -- wait for the player that he has closed the trick
    void trick_taken();

    // set the reservation to make
    void set_reservation(Reservation const& reservation);
    // set the cards shifted in a poverty
    void set_poverty_cards_to_shift(vector<Card> const& cards);
    // set whether to accept the poverty
    void set_poverty_accept(bool const accept);

    // set the announcement to make
    void set_announcement(Announcement const& announcement);
    // set partner for the genscher
    void set_genscher_partner(unsigned const partnerno);
    // set the card to play
    void set_card_to_play(Card const& card);
    // set that swines are to be announced
    void set_swines_announcement();
    // set that hyperswines are to be announced
    void set_hyperswines_announcement();
    // set that the trick has been taken
    void set_trick_taken();


  private:
    // the corresponding connection
    PRIV_VAR_P_R(Network::Connection, connection);

    // the reservation the player will make
    mutable Reservation* pending_reservation;
    // the cards to shift in a poverty
    mutable vector<Card>* pending_cards_to_shift;
    // whether the player has decided whether to accept a poverty
    mutable unsigned pending_poverty_accept;
    // the card the player will play
    mutable queue<Card> pending_cards_to_play;
    // the number of the genscher partner
    mutable unsigned pending_genscher_partnerno;
    // the trick has been taken
    mutable bool pending_trick_taken;

  private: // unused
    // constructor
    NetworkPlayer();
    // clone the player
    NetworkPlayer* clone() const;
    // copy constructor
    NetworkPlayer(NetworkPlayer const&);
    // copy operator
    NetworkPlayer& operator=(NetworkPlayer const&);
}; // class NetworkPlayer : public Ai

#endif // #ifdef USE_NETWORK

#endif // #ifndef CLASS_NETWORK_PLAYER_HEADER
