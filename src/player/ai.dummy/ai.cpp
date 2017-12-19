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
 ** constructor
 **
 ** @param     -
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.13
 **/
AiDummy::AiDummy() :
Player(Player::AI_DUMMY),
Aiconfig()
{ }

/**
 ** Construktor
 **
 ** @param     istr   stream with the infos
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.13
 **/
AiDummy::AiDummy(istream& istr) :
Player(AI_DUMMY),
Aiconfig()
{
  (void)istr;
  DEBUG_ASSERTION(false,
		  "AiDummy::AiDummy(istr):\n"
		  "  Contructor not implemented");

  return ;
} // AiDummy::AiDummy(istream& istr)

/**
 ** copy constructor
 **
 ** @param     player   player to copy
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **      
 ** @version   0.7.13
 **/
AiDummy::AiDummy(Player const& player) :
Player(player),
Aiconfig()
{
  this->set_type(AI_DUMMY);

  return ;
} // AiDummy::AiDummy(Player const& player)

/**
 ** copy constructor
 **
 ** @param     player     player to copy
 ** @param     aiconfig   configuration to copy
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
AiDummy::AiDummy(Player const& player, Aiconfig const& aiconfig) :
Player(player),
Aiconfig(aiconfig)
{
  this->set_type(AI_DUMMY);

  return ;
} // AiDummy::AiDummy(Player const& player, Aiconfig const& aiconfig)

/**
 **
 ** copy constructor
 **
 ** @param     ai_dummy   ai to copy
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
AiDummy::AiDummy(AiDummy const& ai_dummy) :
Player(ai_dummy),
Aiconfig(ai_dummy)
{
  this->set_type(AI_DUMMY);

  return ;
} // AiDummy::AiDummy(AiDummy const& ai_dummy)

/**
 ** clone the player
 **
 ** @param     -
 **
 ** @return    pointer of a clone
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
AiDummy*
AiDummy::clone() const
{  
  return new AiDummy(*this);
} // virtual AiDummy* AiDummy::clone() const

/**
 ** copy operator
 **
 ** @param     ai_dummy   ai to copy
 **
 ** @return    copied ai
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
AiDummy&
AiDummy::operator=(AiDummy const& ai_dummy)
{
  static_cast<Player&>(*this) = static_cast<Player const&>(ai_dummy);
  static_cast<Aiconfig&>(*this) = static_cast<Aiconfig const&>(ai_dummy);

  return *this;
} // AiDummy& AiDummy::operator=(AiDummy const& ai_dummy)

/**
 **
 ** Destructor
 **
 ** @param	-
 **
 ** @return	-
 **
 ** @version	0.5.4
 **
 ** @author	Diether Knof
 **      
 **
 **/
AiDummy::~AiDummy()
{
  return ;
} // AiDummy::~AiDummy()

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
AiDummy::reservation_get(bool const duty_solo)
{
  this->reservation() = this->Player::reservation_get_default();

  if (duty_solo) {
    for (int t = GAMETYPE::FIRST; t <= GAMETYPE::LAST; ++t) {
      if (GAMETYPE::is_real_solo(static_cast<GameType>(t))
          && this->game().rule()(static_cast<GameType>(t)) ) {
        this->reservation().game_type = static_cast<GameType>(t);
        break;
      }
    } // for (t)
  } // if (duty_solo)

  return this->reservation();
} // Reservation const& AiDummy::reservation_get(bool duty_solo = false)

/**
 ** -> result
 ** non trumps, color aces and lowest color cards
 **
 ** @param	-
 **
 ** @return	cards to shift
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.4
 **/
HandCards
AiDummy::poverty_shift()
{
  // the number of cards to shift
  unsigned const numberofpovertycards = this->hand().numberofpovertycards();
  // the cards to shift
  HandCards cards_to_shift;

  // first add all poverty cards
  for (HandCards::const_iterator c = this->hand().cards().begin();
       c != this->hand().cards().end();
       ++c)
    if (c->istrump())
      cards_to_shift.push_back(*c);

  // second add all color cards in the order: ace, nine, king, ten
  vector<Card::Value> values;
  values.push_back(Card::ACE);
  values.push_back(Card::NINE);
  values.push_back(Card::KING);
  values.push_back(Card::TEN);
  for (vector<Card::Value>::const_iterator v = values.begin();
       (   (v != values.end())
        && (cards_to_shift.size() < numberofpovertycards));
       ++v)
    for (HandCards::const_iterator c = this->hand().cards().begin();
         (   (c != this->hand().cards().end())
          && (cards_to_shift.size() < numberofpovertycards));
         ++c)
      if (!c->istrump())
        if (c->value() == *v)
          cards_to_shift.push_back(*c);

  this->sorted_hand().remove(cards_to_shift);

  return cards_to_shift;
} // HandCards AiDummy::poverty_shift()

/**
 ** -> result
 ** whether to accept the poverty
 ** 
 ** @param     cardno   the number of cards that are shifted
 ** 
 ** @return    false (do not accept)
 ** 
 ** @author    Diether Knof
 ** 
 ** @version   0.7.4
 **/ 
bool
AiDummy::poverty_take_accept(unsigned cardno)
{
  return false;
} // bool AiDummy::poverty_take_accept(unsigned cardno)

/**
 ** change the poverty cards
 ** should not be called
 ** 
 ** @param     cards   the cards the player takes
 ** 
 ** @return    cards to return
 ** 
 ** @author    Diether Knof
 ** 
 ** @version   0.7.4
 **/ 
HandCards
AiDummy::poverty_cards_change(HandCards const& cards)
{
  DEBUG_ASSERTION(false,
                  "AiDummy::poverty_cards_change(cards)\n"
                  "  call of this function althoug the dummy ai should reject any poverty");

  return cards;
} // HandCards AiDummy::poverty_cards_change(HandCards cards)

/**
 ** the poverty player gets 'cards'
 ** 
 ** @param     cards    cards the player gets back
 ** 
 ** @return    -
 ** 
 ** @author    Diether Knof
 ** 
 ** @version   0.7.4
 **/ 
void
AiDummy::poverty_cards_get_back(HandCards const& cards)
{
  this->sorted_hand().add(cards);
  return ;
} // void AiDummy::poverty_cards_get_back(HandCards cards)

/**
 ** -> result:
 ** if the dummy ai can get the trick, it takes its best card
 ** (also, if its the first card in the trick)
 ** else (if the dummy ai has no better than the winning card of the trick)
 ** the worst card is taken
 **
 **
 ** @param	-
 **
 ** @return	valid card
 **
 ** @author	Diether Knof
 **
 ** @version	0.5.2
 **
 **/
HandCard
AiDummy::card_get()
{
  // the first card -- play the highest
  // if the ai cannot get the trick: play the lowest
  // if the ai can get the trick: play the highest

  HandCard card;
  bool can_win = false;

  for (vector<HandCard>::const_iterator c = this->hand().cards().begin();
       c != this->hand().cards().end();
       c++) {
    if (this->game().trick_current().isvalid(*c, this->hand())) {
      if (card.is_empty()) {
        card = *c;
      } else { // if !(!card)
        bool const is_winnercard
          = (this->game().trick_current().isempty()
             ? true
             : this->game().trick_current().winnercard().less(*c)
            );
        can_win |= is_winnercard;

        if (this->game().trick_current().isempty()) {
          // take the smallest card
          if (card.less(*c))
            card = *c;
        } else if (this->game().trick_current().islastcard()) {
          // if the player can win the trick take the smallest winnercard,
          // else take the smallest card.

          if (can_win
              && is_winnercard
              && c->less(card))
            card = *c;
          if (!can_win
              && !is_winnercard
              && c->less(card))
            card = *c;
        } else { // if !(is first or last card in trick)
          // if the player can win the trick take the greatest winnercard,
          // else take the smallest card.

          if (can_win
              && is_winnercard
              && card.less(*c))
            card = *c;
          if (!can_win
              && !is_winnercard
              && c->less(card))
            card = *c;
        } // if !(is first or last card in trick)
      } // if !(!card)
    } // if (this->game().trick_current().isvalid(*c, this->hand()))
  } // for (c \in this->hand().cardsnumber())

  // valid_cards.card(RAND(valid_cards.cardsnumber()))
  return card;
} // HandCard AiDummy::card_get()

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
 **
 **/
Announcement
AiDummy::announcement_request() const
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
} // Announcement AiDummy::announcement_request() const

/**
 ** genscher: -> result
 ** take the player with the most points
 **
 ** @param	-
 **
 ** @return	pointer to the new teammate,
 **		NULL if the genscher is not to be announced
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.4
 **
 ** @todo       take current trick into account
 **/
Player const*
AiDummy::genscher_partner()
{ 
  // player with the most points
  unsigned best_player = UINT_MAX;
  // most points
  unsigned points_max = 0;
  for (unsigned p = 0; p < this->game().playerno(); ++p) {
    if (this->game().player(p) == *this)
      continue;

    if (   (this->game().points_of_player(p) > points_max)
        || (   (this->game().points_of_player(p) > points_max)
            && (this->game().teaminfo(this->game().player(p)) == this->team()) )
       ) {
      points_max = this->game().points_of_player(p);
      best_player = p;
    }
  } // for (p < this->game().playerno())

  if (   (best_player == UINT_MAX)
      || (this->game().teaminfo(best_player) == this->team()) )
    return NULL;
  else
    return &this->game().player(best_player);
} // Player const* AiDummy::genscher_partner()
