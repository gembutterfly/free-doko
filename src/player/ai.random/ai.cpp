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

#include "ai.h"

#include "../../game/game.h"
#include "../../party/rule.h"
#include "../../card/trick.h"

/**
 ** Constructor
 **
 ** @param	-
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.13
 **/
AiRandom::AiRandom() :
  Player(Player::AI_RANDOM),
  Aiconfig()
{ }

/**
 ** Construktor
 **
 ** @param	istr	stream with the infos
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.5.2
 **
 ** @todo	all
 **/
AiRandom::AiRandom(istream& istr) :
  Player(AI_RANDOM),
  Aiconfig()
{
  (void)istr;
  DEBUG_ASSERTION(false,
		  "AiRandom::AiRandom(istr):\n"
		  "  Contructor not implemented");

  return ;
} // AiRandom::AiRandom(istream& istr)

/**
 ** copy constructor
 **
 ** @param	player	player to copy
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.4
 **/
AiRandom::AiRandom(Player const& player) :
  Player(player),
  Aiconfig()
{
  this->set_type(AI_RANDOM);

  return ;
} // AiRandom::AiRandom(Player const& player)

/**
 ** copy constructor
 **
 ** @param	player		player to copy
 ** @param	aiconfig	configuration to copy
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **/
AiRandom::AiRandom(Player const& player, Aiconfig const& aiconfig) :
  Player(player),
  Aiconfig(aiconfig)
{
  this->set_type(AI_RANDOM);

  return ;
} // AiRandom::AiRandom(Player const& player, Aiconfig const& aiconfig)

/**
 ** copy constructor
 **
 ** @param	ai_random	ai to copy
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.5.3
 **/
AiRandom::AiRandom(AiRandom const& ai_random) :
  Player(ai_random),
  Aiconfig(ai_random)
{
  this->set_type(AI_RANDOM);

  return ;
} // AiRandom::AiRandom(AiRandom const& ai_random)

/**
 ** clone the player
 **
 ** @param       -
 **
 ** @return      pointer of a clone
 **
 ** @author      Diether Knof
 **
 ** @version     0.5.3
 **/
AiRandom*
AiRandom::clone() const
{  
  return new AiRandom(*this);
} // virtual AiRandom* AiRandom::clone() const

/**
 ** copy operator
 **
 ** @param	ai_random	ai to copy
 **
 ** @return	copied ai
 **
 ** @author	Diether Knof
 **
 ** @version	0.5.4
 **/
AiRandom&
AiRandom::operator=(AiRandom const& ai_random)
{
  static_cast<Player&>(*this) = static_cast<Player const&>(ai_random);
  static_cast<Aiconfig&>(*this) = static_cast<Aiconfig const&>(ai_random);

  return *this;
} // AiRandom& AiRandom::operator=(AiRandom const& ai_random)

/**
 ** destructor
 **
 ** @param     -
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
AiRandom::~AiRandom()
{ }

/**
 ** -> result
 ** (test of poverty, marriage, swines, hyperswines)
 **
 ** @param	duty_solo	whether the player has to play a solo
 ** 				(default: false)
 **
 ** @return	simple Reservation
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.9
 **/
Reservation const&
AiRandom::reservation_get(bool const duty_solo)
{
  this->reservation() = this->Player::reservation_get_default();

  if (   duty_solo
      || (RAND(this->game().playerno() * this->game().playerno()) == 0) ) {
    vector<GameType> soli;
    for (int t = GAMETYPE::FIRST; t <= GAMETYPE::LAST; ++t) {
      if (GAMETYPE::is_real_solo(static_cast<GameType>(t))
          && this->game().rule()(static_cast<GameType>(t)) )
        soli.push_back(static_cast<GameType>(t));
    } // for (t)
    this->reservation().game_type = soli[RAND(soli.size())];
  } // if (duty_solo)

  return this->reservation();
} // Reservation const& AiRandom::reservation_get(bool duty_solo)

/**
 ** -> result:
 ** take a random card of all valid ones
 **
 ** @param	-
 **
 ** @return	valid card
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.8
 **/
HandCard
AiRandom::card_get()
{
  HandCard card;
  do
    card = this->hand().card(RAND(this->hand().cardsnumber()));
  while (!this->game().trick_current().isvalid(card));

  return card;
} // HandCard AiRandom::card_get()

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	announcement of the player
 **
 ** @author	Diether Knof
 **
 ** @version	0.5.4
 **/
Announcement
AiRandom::announcement_request() const
{
  // points of the own team
  unsigned points = 0;

  // count the points, the own team has made 
  for(vector<Player*>::const_iterator player = this->game().players_begin();
      player != this->game().players_end();
      player++)
    if (this->game().teaminfo(**player) == this->team())
      points += this->game().points_of_player(**player);

  if (points == 240)
    return ANNOUNCEMENT::NO0;
  if (points > 240 - 30)
    return ANNOUNCEMENT::NO30;
  if (points > 240 - 60)
    return ANNOUNCEMENT::NO60;
  if (points > 240 - 90)
    return ANNOUNCEMENT::NO90;
  if (points > 240 - 120)
    return ANNOUNCEMENT::NO120;

  return ANNOUNCEMENT::NOANNOUNCEMENT;
} // Announcement AiRandom::announcement_request() const

/**
 ** genscher: -> result
 **
 ** @param	-
 **
 ** @return	NULL (no new partner)
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.4
 **/
Player const*
AiRandom::genscher_partner()
{ 
  return NULL;
} // Player const* AiRandom::genscher_partner()

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	shifted cards
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.3
 **/
HandCards
AiRandom::poverty_shift()
{
  HandCards hand_cards = this->hand().cards();
  HandCards cards_to_shift;

  // first add all trump
  for (HandCards::iterator c = hand_cards.begin(); c != hand_cards.end(); ) {
    if (c->istrump()) {
      cards_to_shift.push_back(*c);
      c = hand_cards.erase(c);
    } else {
      ++c;
    }
  } // for (c \in hand_cards)

  // then fill up with cards randomly chosen
  while (cards_to_shift.size() < this->hand().numberofpovertycards()) {
    unsigned const i = RAND(hand_cards.size());
    cards_to_shift.push_back(hand_cards[i]);
    hand_cards.erase(hand_cards.begin() + i);
  }

  this->sorted_hand().remove(cards_to_shift);

  return cards_to_shift;
} // HandCards AiRandom::poverty_shift()

/**
 ** -> result
 **
 ** @param	cardno   number of shifted cards (ignored)
 **
 ** @return	false (do not accept the poverty)
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.3
 **/
bool
AiRandom::poverty_take_accept(unsigned const cardno)
{
  return false;
} // bool AiRandom::poverty_take_accept(unsigned cardno)


// the player changes the cards of the poverty
/**
 ** should not be called
 **
 ** @param	cards   the cards the player takes
 **
 ** @return	empty cards
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.3
 **/
HandCards
AiRandom::poverty_cards_change(HandCards const& cards)
{
  DEBUG_ASSERTION(false,
                  "AiRandom::poverty_cards_change(cards)\n"
                  "  function should not be called!");
  return HandCards();
} // HandCards AiRandom::poverty_cards_change(HandCards cards)

/**
 ** the poverty player gets 'cards'
 **
 ** @param	cards   cards the player gets back
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.3
 **/
void
AiRandom::poverty_cards_get_back(HandCards const& cards)
{
  this->sorted_hand().add(cards);
  return ;
} // void AiRandom::poverty_cards_get_back(HandCards cards)
