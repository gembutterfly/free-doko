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

#include "cards_information.of_player.h"
#include "cards_information.heuristics.h"
#include "ai.h"

#include "../../party/rule.h"
#ifdef DEBUG_ASSERT
#include "../../card/trick.h"
#endif
#include "../../card/weighted_card_list.h"

#ifdef CHECK_RUNTIME
#include "../../runtime.h"
#endif

// whether to print information of the throwing
#ifndef RELEASE
#ifdef DKNOF
//#define INFO_THROW
// whether to print information of the information flow
#define CONDITION_INFORMATION_FLOW \
  false \
&& (this->game().no(this->cards_information().player()) == 0) \
&& (this->playerno() == 1)
#define CONDITION_INFORMATION_ESTIMATED_HAND \
  false \
&& (this->game().no(this->cards_information().player()) == 1) \
&& (this->playerno() == 3)

#endif
#endif

/*
 * CardsInformation::OfPlayer contains the information an ai has about the
 * cards a specific player can / must / cannot have.
 * The class contains four counters and a weighting
 * - played
 *   what cards the player has played
 * - must have
 *   which and how many cards the player must have
 *   (p.e. if he has announced 're' and not played a club queen, he must
 *    have one (but can have two) )
 * - can have
 *   which and how many cards the player can have at max
 *   (p.e. if another player has annouced 're' he can only have one club queen)
 * - does not have tcolor
 *   just remembers the colors the player cannot have anymore
 *   (because he could not serve).
 *   This should not really be necessary anymore, but I feel more sure with this
 *   code (should check someday)
 * - cards weighting
 *   According to the gameplay the cards are weighted.
 *   positive weight means higher probability for the card,
 *   negative weight means lower probability for the card.
 *
 * The easy part of the code is to add the information gotten by the game
 * (see CardsInformation).
 * Complicated is to update the information over the players.
 * - if the cards a player has to play == the number he must have
 *   the player can only have the cards he must have
 *   (must have --> can have)
 *   A bit more information is gained, like: 
 *     If the player has only one free card (the rest to play are 'must') he
 *     can only have at most one card from each card he must not have).
 * - if the cards a player has to play == the number he can have
 *   the player must have the cards he can have
 *   (can have --> must have)
 *   This is analogous to the previous case, so here also a bit more information
 *   can be gained (and yes, there was at least one case I needed this), p.e.
 *     The player has to play two cards
 *     and he can have one diamond jack and two heart jacks,
 *     he must have at least one heart jack.
 * - count data for a single card
 *   Sum up how many times the card was played and the other players must have
 *   it. Then the player can at max have the remaining number of cards.
 *   If p.e. a player has announced 're' he must have a club queen. So the other
 *     players can only have at max one club queen in their hands.
 *   And again this can be viewed from the opposite side:
 *   If for a card the sum of 'played' and 'can have' of the other players
 *   are less than two, then the player must have the remaining ones.
 *   A simple (and most common) example is that three players do not have
 *   spade, so the fourth player must have all remaining spade cards.
 */

/**
 ** constructor
 **
 ** @param	cards_information	corresponding cards information
 ** @param	playerno		number of the player of this information
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.9
 **/
CardsInformation::OfPlayer::OfPlayer(CardsInformation& cards_information,
                                     unsigned const playerno) :
  cards_information_(&cards_information),
  playerno_(playerno),
  played_(),
  must_have_(),
  can_have_(),
  tcolor_played_(),
  tcolor_must_have_(),
  tcolor_can_have_(),
  cards_weighting_()
{ this->reset(); }

/**
 ** copy constructor
 **
 ** @param	of_player	object to be copied
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.9
 **/
CardsInformation::OfPlayer::OfPlayer(OfPlayer const& of_player) :
  cards_information_(of_player.cards_information_),
  playerno_(of_player.playerno_),
  played_(of_player.played_),
  must_have_(of_player.must_have_),
  can_have_(of_player.can_have_),
  tcolor_played_(of_player.tcolor_played_),
  tcolor_must_have_(of_player.tcolor_must_have_),
  tcolor_can_have_(of_player.tcolor_can_have_),
  cards_weighting_(of_player.cards_weighting_)
{ }

/**
 ** copy operator
 **
 ** @param	of_player	object to be copied
 **
 ** @return	object
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.9
 **/
CardsInformation::OfPlayer&
CardsInformation::OfPlayer::operator=(OfPlayer const& of_player)
{
  this->cards_information_ = of_player.cards_information_;
  this->playerno_ = of_player.playerno_;
  this->played_ = of_player.played_;
  this->must_have_ = of_player.must_have_;
  this->can_have_ = of_player.can_have_;
  this->tcolor_played_ = of_player.tcolor_played_;
  this->tcolor_must_have_ = of_player.tcolor_must_have_;
  this->tcolor_can_have_ = of_player.tcolor_can_have_;
  this->cards_weighting_ = of_player.cards_weighting_;

  return *this;
} // CardsInformation::OfPlayer& CardsInformation::OfPlayer::operator=(OfPlayer of_player)

/**
 ** destructor
 **
 ** @param	-
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.9
 **/
CardsInformation::OfPlayer::~OfPlayer()
{ }

/**
 ** resets all information
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
CardsInformation::OfPlayer::reset()
{
  // cards
  this->played_.clear();
  this->must_have_.clear();
  this->cards_weighting_.clear();
  for (vector<Card>::const_iterator
       c = this->game().rule().cards().begin();
       c != this->game().rule().cards().end();
       ++c) {
    this->can_have_.set(*c, this->game().rule()(Rule::NUMBER_OF_SAME_CARDS));
    this->cards_weighting_[*c] = 0;
  } // for (c \in cards)

  // tcolors
  this->tcolor_played_.clear();
  this->tcolor_must_have_.clear();
  for (unsigned c = 0; c < Card::NUMBER_OF_TCOLORS; ++c)
    this->tcolor_can_have_.set(static_cast<Card::TColor>(c),
                               min(this->game().rule()(Rule::NUMBER_OF_CARDS_ON_HAND),
                                   this->game().cards_no(static_cast<Card::TColor>(c))));

  return ;
} // void CardsInformation::OfPlayer::reset()

/**
 ** writes 'of_player' in 'ostr'
 **
 ** @param	ostr		output stream
 ** @param	of_player	object to write into 'ostr'
 **
 ** @return	the output stream
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.9
 **/
ostream&
operator<<(ostream& ostr, CardsInformation::OfPlayer const& of_player)
{
  of_player.write(ostr);
  return ostr;
} // ostream& operator<<(ostream& ostr, CardsInformation::OfPlayer of_player)

/**
 ** writes 'of_player' in 'ostr'
 **
 ** @param	ostr		output stream
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.9
 **/
void
CardsInformation::OfPlayer::write(ostream& ostr) const
{
  ostr << "player = " << this->playerno() << " (" << this->player().name() << ")\n";

  ostr << "{\n";

  ostr << "  played\n"
    << "  {\n"
    << this->played_
    << "  }\n";

  ostr << "  must_have\n"
    << "  {\n"
    << this->must_have_
    << "  }\n";

  ostr << "  can_have\n"
    << "  {\n"
    << "cardno = " << this->can_have_.cards_no() << '\n';
  for (vector<Card>::const_iterator
       c = this->game().rule().cards().begin();
       c != this->game().rule().cards().end();
       ++c) {
    if (this->can_have(*c) > 0) {
      ostr << setw(14)
        << *c << " = "
        << this->can_have(*c) << ' '
        << "(" << this->weighting(*c, false) << ")\n";
    }
  } // for (c \in cards)
  ostr << "  }\n";

  ostr << "  tcolors played\n"
    << "  {\n"
    << this->tcolor_played_
    << "  }\n";

  ostr << "  tcolors must_have\n"
    << "  {\n"
    << this->tcolor_must_have_
    << "  }\n";

  ostr << "  tcolors can_have\n"
    << "  {\n"
    << this->tcolor_can_have_
    << "  }\n";

  ostr << '\n';
  ostr << "estimated hand\n"
    << this->estimated_hand() << '\n';
  ostr << "}\n";

  return ;
} // void CardsInformation::OfPlayer::write(ostream& ostr) const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	the corresponding game
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.9
 **/
Game const&
CardsInformation::OfPlayer::game() const
{
  return this->cards_information().game();
} // Game CardsInformation::OfPlayer::game() const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	the corresponding player
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.9
 **/
Player const&
CardsInformation::OfPlayer::player() const
{
  return this->game().player(this->playerno());
} // Player CardsInformation::OfPlayer::player() const

/**
 ** sets the corresponding cards information
 **
 ** @param	cards_information	new cards information
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.9
 **/
void
CardsInformation::OfPlayer::set_cards_information(CardsInformation&
                                                  cards_information)
{
  this->cards_information_ = &cards_information;
  return ;
} // void CardsInformation::OfPlayer::set_cards_information(CardsInformation& cards_information)

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	whether all cards are known
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
bool
CardsInformation::OfPlayer::all_known() const
{
  return (this->can_have_.cards_no() == this->must_have_.cards_no());
} // bool CardsInformation::OfPlayer::all_known() const

/**
 ** -> result
 **
 ** @param	card	card
 **
 ** @return	how many of 'card' the player has played
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.9
 **/
unsigned
CardsInformation::OfPlayer::played(Card const& card) const
{
  return this->played_[card];
} // unsigned CardsInformation::OfPlayer::played(Card card) const

/**
 ** -> result
 **
 ** @param	tcolor	tcolor
 **
 ** @return	how many cards of the tcolor 'tcolor' the player has played
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.9
 **/
unsigned
CardsInformation::OfPlayer::played(Card::TColor const& tcolor) const
{
  DEBUG_ASSERTION(tcolor != Card::UNKNOWNCARDCOLOR,
                  "CardsInformation::OfPlayer::played(" << tcolor << ")\n"
                  "  tcolor 'unknown' is forbidden");

  return this->tcolor_played_[tcolor];
} // unsigned CardsInformation::OfPlayer::played(Card::TColor tcolor) const

/**
 ** -> result
 **
 ** @param	card	card
 **
 ** @return	how many cards of 'card' the player must still have on the hand
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.9
 **/
unsigned
CardsInformation::OfPlayer::must_have(Card const& card) const
{
  return this->must_have_[card];
} // unsigned CardsInformation::OfPlayer::must_have(Card const& card) const

/**
 ** -> result
 **
 ** @param	tcolor    tcolor
 **
 ** @return	how many cards of 'tcolor' the player must still have on the hand
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
unsigned
CardsInformation::OfPlayer::must_have(Card::TColor const& tcolor) const
{
  DEBUG_ASSERTION(tcolor != Card::UNKNOWNCARDCOLOR,
                  "CardsInformation::OfPlayer::must_have(" << tcolor << ")\n"
                  "  tcolor 'unknown' is forbidden");
  return this->tcolor_must_have_[tcolor];
} // unsigned CardsInformation::OfPlayer::must_have(Card::TColor tcolor) const

/**
 ** -> result
 **
 ** @param	card	card
 **
 ** @return	how many cards of 'card' the player can still have on the hand
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.9
 **/
unsigned
CardsInformation::OfPlayer::can_have(Card const& card) const
{
  return this->can_have_[card];
} // unsigned CardsInformation::OfPlayer::can_have(Card const& card) const

/**
 ** -> result
 **
 ** @param	tcolor    tcolor
 **
 ** @return	how many cards of 'tcolor' the player can still have on the hand
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
unsigned
CardsInformation::OfPlayer::can_have(Card::TColor const& tcolor) const
{
  DEBUG_ASSERTION(tcolor != Card::UNKNOWNCARDCOLOR,
                  "CardsInformation::OfPlayer::can_have(" << tcolor << ")\n"
                  "  tcolor 'unknown' is forbidden");
  return this->tcolor_can_have_[tcolor];
} // unsigned CardsInformation::OfPlayer::can_have(Card::TColor tcolor) const

/**
 ** -> result
 **
 ** @param	card	card
 **
 ** @return	whether the player cannot have the card still on the hand
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.9
 **/
bool
CardsInformation::OfPlayer::cannot_have(Card const& card) const
{
  return (this->can_have(card) == 0);
} // bool CardsInformation::OfPlayer::cannot_have(Card const& card) const

/**
 ** -> result
 **
 ** @param	tcolor	tcolor to check
 **
 ** @return	whether the player does not have anymore the tcolor
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.9
 **/
bool
CardsInformation::OfPlayer::does_not_have(Card::TColor const& tcolor) const
{
  return (this->can_have(tcolor) == 0);
} // bool CardsInformation::OfPlayer::does_not_have(Card::TColor tcolor) const


/**
 ** -> result
 **
 ** @param	card   card
 **
 ** @return	the number of unkown cards 'card'
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
unsigned
CardsInformation::OfPlayer::unknown(Card const& card) const
{
  return (this->can_have(card) - this->must_have(card));
} // unsigned CardsInformation::OfPlayer::unknown(Card card) const

/**
 ** -> result
 **
 ** @param	tcolor    tcolor
 **
 ** @return	the number of unknown cards of 'tcolor'
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
unsigned
CardsInformation::OfPlayer::unknown(Card::TColor const& tcolor) const
{
  return (this->can_have(tcolor) - this->must_have(tcolor));
} // unsigned CardsInformation::OfPlayer::unknown(Card::TColor tcolor) const

/**
 ** -> result
 **
 ** @param	card     card
 ** @param	modify   whether to modify the weighting (t.i. of the club queen) (default: true)
 **
 ** @return	weighting for the card
 **             positive means more probability for having the card
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.3
 **/
int
CardsInformation::OfPlayer::weighting(Card const& card, bool const modify) const
{
  if (this->cannot_have(card))
    return 0;

  int modifier = 0;

  // take the announcement into account:
  // if the player has announced, he will likely have less color cards
  if (!card.istrump(this->game())) {
    switch (this->game().announcement_of_player(this->player()).announcement) {
    case ANNOUNCEMENT::REPLY:
    case ANNOUNCEMENT::NO120_REPLY:
      modifier -= 10;
      break;
    case ANNOUNCEMENT::NO120:
      modifier -= 15;
      break;
    case ANNOUNCEMENT::NO90:
      modifier -= 30;
      break;
    case ANNOUNCEMENT::NO60:
      modifier -= 100;
      break;
    case ANNOUNCEMENT::NO30:
      modifier -= 500;
      break;
    case ANNOUNCEMENT::NO0:
      modifier -= 1000;
      break;
    default:
      break;
    } // switch (announcement)
  } // if (!card.istrump(this->game()))

  if (modify) {
    if (   (card == Card::CLUB_QUEEN)
        && (this->game().type() == GAMETYPE::NORMAL) ) {
      modifier += (10 // *Value*
                   * this->cards_information().player().team_information().team_value(this->playerno()));
      if (TEAM::maybe_to_team(this->cards_information().player().team_information().team(this->playerno())) == TEAM::RE) {
        if (this->played(Card::CLUB_QUEEN))
          modifier -= 500;
        else
          modifier += 1000;
      } else if (TEAM::maybe_to_team(this->cards_information().player().team_information().team(this->playerno())) == TEAM::CONTRA)
        modifier -= 1000;
    } // if (club queen in normal game)
  } // if (modify)

  return (this->cards_weighting_[card] + modifier);
} // int CardsInformation::OfPlayer::weighting(Card card, bool modify = true) const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	the cards the player has played
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.12
 **/
Hand
CardsInformation::OfPlayer::played_cards() const
{
  Hand cards(this->player());

  for (vector<Card>::const_iterator c = this->played_.cards().begin();
       c != this->played_.cards().end();
       ++c)
    cards.add(*c);

  return cards;
} // Hand CardsInformation::OfPlayer::played_cards() const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	the cards the player must have
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.9
 **/
Hand
CardsInformation::OfPlayer::must_have_cards() const
{
  Hand cards(this->player());

  for (vector<Card>::const_iterator c = this->must_have_.cards().begin();
       c != this->must_have_.cards().end();
       ++c)
    cards.add(*c);

  return cards;
} // Hand CardsInformation::OfPlayer::must_have_cards() const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	the cards the player cannot have
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.9
 **/
Hand
CardsInformation::OfPlayer::cannot_have_cards() const
{
  Hand cards(this->player());

  for (vector<Card>::const_iterator
       c = this->game().rule().cards().begin();
       c != this->game().rule().cards().end();
       ++c)
    for (int i = 0; i < this->cannot_have(*c); ++i)
      cards.add(*c);

  return cards;
} // Hand CardsInformation::OfPlayer::cannot_have_cards() const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	the possible hand of the player
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.9
 **
 ** @todo	check whether the cards the player must have are as many as the
 **		player can only have
 ** @todo	add the played cards
 **/
Hand
CardsInformation::OfPlayer::possible_hand() const
{
  DEBUG_ASSERTION((this->can_have_.cards_no()
                   >= this->player().cards_to_play()),
                  "CardsInformation(" << this->game().no(this->cards_information().player()) << ")::OfPlayer(" << this->playerno() << ")::possible_hand():\n"
                  "  can_have.cards_no() = " << this->can_have_.cards_no()
                  << " < " << this->player().cards_to_play()
                  << " = this->player().cards_to_play()");

  DEBUG_ASSERTION((this->can_have_.cards().size()
                   == this->can_have_.cards_no()),
                  "CardsInformation::OfPlayer::possible_hand():\n"
                  "  the number of cards is not valid");

  return Hand(this->player(),
              this->can_have_.cards(), this->played_.cards());
} // Hand CardsInformation::OfPlayer::possible_hand() const

/**
 ** -> result
 ** the estimation is based by the weighting
 **
 ** @param	-
 **
 ** @return	estimated hand for the player
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.3
 **
 ** @todo       check tcolor (can have, must have)
 ** @todo       better alogrithm for the estimation
 ** @todo       ensure, that all cards are distributed
 ** @todo       ensure, that each player has enough cards
 ** @todo       maybe contra -> no club queen
 ** @todo       two re -> both one club queen
 **/
Hand
CardsInformation::OfPlayer::estimated_hand() const
{
#ifdef CONDITION_INFORMATION_ESTIMATED_HAND 
  if (CONDITION_INFORMATION_ESTIMATED_HAND) {
    clog << __FILE__ << '#' << __LINE__ << "  " << "start CardsInformation(" << this->game().no(this->cards_information().player()) << ")::OfPlayer(" << this->game().no(this->player()) << ")::estimate_hand()\n";
  }
#endif

  if (this->cards_information().player().value(Aiconfig::HANDS_KNOWN))
    return this->player().hand();

  Hand hand(this->player());

  Game const& game = this->game();
  Rule const& rule = game.rule();

  // start with the played cards
  hand.add_played(this->played_.cards());

  { // 1) add all 'must have' cards
    // ToDo: use must_have for the cards directly
    for (vector<Card>::const_iterator
         c = rule.cards().begin();
         c != rule.cards().end();
         ++c) {
      hand.add(*c, this->must_have(*c));
    }
#ifdef CONDITION_INFORMATION_ESTIMATED_HAND 
    if (CONDITION_INFORMATION_ESTIMATED_HAND) {
      //clog << __FILE__ << '#' << __LINE__ << "  " << "hand after 1) must have\n" << hand << '\n';
    }
#endif
  } // 1) add all 'must have' cards

  // all cards of the player are known
  if (hand.cardsnumber() == this->player().cards_to_play())
    return hand;

  // the cards to distribute
  WeightedCardList cards(this->game());
  { // Put all remaining can_have-cards in the list 'cards'.
    for (vector<Card>::const_iterator
         c = rule.cards().begin();
         c != rule.cards().end();
         ++c) {
      if (this->cannot_have(*c))
        continue;
      int w = INT_MIN; // the weighting from the other players
      // take the weighting of the other players into account
      for (unsigned p = 0; p < game.playerno(); ++p) {
        if (   (p != this->playerno())
            && this->cards_information().of_player(p).unknown(*c)) {
          w = max(w, this->cards_information().of_player(p).weighting(*c));
        }
      } // for (p)

      // '2 * i' reflects the probability to have two of the same cards
      for (int i = 0;
#ifdef WORKAROUND
           // ToDo: remaining(*c) should always be >= can_have(*c)
           i < static_cast<int>(min(this->can_have(*c) - this->must_have(*c),
                                    this->cards_information().remaining_unknown(*c)));
#else
           i < static_cast<int>(this->can_have(*c) - this->must_have(*c));
#endif
           ++i) {
        if (w == INT_MIN) {
          cards.add(*c, INT_MAX);
          continue;
        }
        if (   (*c == Card::CLUB_QUEEN)
            && (this->game().type() == GAMETYPE::NORMAL) ) {
          // in a normal game, a player will most surely not have two club queens
          cards.add(*c,
                    this->weighting(*c)
                    - w / static_cast<int>(this->cards_information().remaining_unknown(*c))
                    - 10000 * (i + this->must_have(*c))); // *Value*
        } else {
          cards.add(*c,
                    this->weighting(*c)
                    - w / static_cast<int>(this->cards_information().remaining_unknown(*c))
                    - 10 * i); // *Value*
        }
      }
    } // for (c \in cards)
  } // Put all remaining can_have-cards in the list 'cards'.
#ifdef CONDITION_INFORMATION_ESTIMATED_HAND 
  if (CONDITION_INFORMATION_ESTIMATED_HAND) {
    clog << __FILE__ << '#' << __LINE__ << "  " << "weighted cards\n" << cards << '\n';
  }
#endif

  { // 2) add to each player the cards with the greates weighting
    // (a card can be distributed multible times)
    // Now fill up the hand in the order of weighting.
    while (   !cards.empty()
           // '- 1' here, so that in the next block 'w' is the right weighting
           // (the next card is always added)
           && (hand.cardsnumber() + 1 < this->player().cards_to_play()) ) {
#ifdef CONDITION_INFORMATION_ESTIMATED_HAND 
      if (CONDITION_INFORMATION_ESTIMATED_HAND) {
        clog << __FILE__ << '#' << __LINE__ << "  " << "2a) add card " << cards.highest() << '\n';
      }
#endif
      hand.add(cards.highest());
      cards.pop_highest();
    }

    // Also add the cards with a weighting nearly the same as the lowest so far.
    int const w = cards.highest_weighting();
    while (   !cards.empty()
           && (cards.highest_weighting() > w - 40)) { // *Value*
#ifdef CONDITION_INFORMATION_ESTIMATED_HAND 
      if (CONDITION_INFORMATION_ESTIMATED_HAND) {
        clog << __FILE__ << '#' << __LINE__ << "  " << "2b) add card " << cards.highest() << '\n';
      }
#endif
      hand.add(cards.highest());
      cards.pop_highest();
    }
  } // 2) add each player the cards with the greates weighting

  { // 3) for each color add enough cards according to 'tcolor_must_have' and 'tcolor_can_have'
    // single tcolors
    for (unsigned tc = 0; tc < Card::NUMBER_OF_TCOLORS; ++tc) {
      Card::TColor tcolor = static_cast<Card::TColor>(tc);
      unsigned n = hand.numberof(tcolor);
      unsigned const min = this->must_have(tcolor);
      if (n >= min)
        continue;
      // add at least 'min' cards of 'tcolor'
      while (!cards.highest(tcolor).is_empty()
             && (n + 1 < min)) {
#ifdef CONDITION_INFORMATION_ESTIMATED_HAND 
        if (CONDITION_INFORMATION_ESTIMATED_HAND) {
          clog << __FILE__ << '#' << __LINE__ << "  " << "3) add card " << cards.highest(tcolor) << '\n';
        }
#endif
        hand.add(cards.highest(tcolor));
        cards.pop_highest(tcolor);
        n += 1;
      }

      // Also add the cards with a weighting nearly the same as the lowest so far.
      int const w = cards.highest_weighting(tcolor);
      while (   !cards.highest(tcolor).is_empty()
             && (cards.highest_weighting(tcolor) > w - 40)) { // *Value*
#ifdef CONDITION_INFORMATION_ESTIMATED_HAND 
        if (CONDITION_INFORMATION_ESTIMATED_HAND) {
          clog << __FILE__ << '#' << __LINE__ << "  " << "3) add card " << cards.highest(tcolor) << '\n';
        }
#endif
        hand.add(cards.highest(tcolor));
        cards.pop_highest(tcolor);
      }
    } // for (tcolor)

    // single tcolors inverted
    for (unsigned tc = 0; tc < Card::NUMBER_OF_TCOLORS; ++tc) {
      Card::TColor tcolor = static_cast<Card::TColor>(tc);
      if (this->player().cards_to_play() <= this->can_have(tcolor))
        continue;
      unsigned n = hand.cardsnumber() - hand.numberof(tcolor);
      unsigned const min = (this->player().cards_to_play()
                            - this->can_have(tcolor));
      if (n >= min)
        continue;
      if (cards.cards_no() < min - n) {
#ifdef INFO_THROW
        clog << __FILE__ << '#' << __LINE__ << "  " << "\n  CardsInformation(" << this->game().no(this->cards_information().player())
          << ")::OfPlayer(" << this->playerno() << ")::"
          << "throw in estimated_hand()\n";
#endif
        DEBUG_THROW(InvalidGameException, InvalidGameException());
      } // if (cards.size() < min - n)

      // add at least 'min' cards of not 'tcolor'
      while (!cards.highest_not_of(tcolor).is_empty()
             && (n + 1 < min)) {
#ifdef CONDITION_INFORMATION_ESTIMATED_HAND 
        if (CONDITION_INFORMATION_ESTIMATED_HAND) {
          clog << __FILE__ << '#' << __LINE__ << "  " << "3) add card " << cards.highest_not_of(tcolor) << '\n';
        }
#endif
        hand.add(cards.highest_not_of(tcolor));
        cards.pop_highest_not_of(tcolor);
        n += 1;
      }

      // Also add the cards with a weighting nearly the same as the lowest so far.
      int const w = cards.highest_weighting(tcolor);
      while (   !cards.highest_not_of(tcolor).is_empty()
             && (cards.highest_weighting_not_of(tcolor) > w - 40)) { // *Value*
#ifdef CONDITION_INFORMATION_ESTIMATED_HAND 
        if (CONDITION_INFORMATION_ESTIMATED_HAND) {
          clog << __FILE__ << '#' << __LINE__ << "  " << "3) add card " << cards.highest_not_of(tcolor) << '\n';
        }
#endif
        hand.add(cards.highest_not_of(tcolor));
        cards.pop_highest_not_of(tcolor);
      }
    } // for (tcolor)
  } // 3) for each color add enough cards according to 'tcolor_must_have' and 'tcolor_can_have'

  { // 4) add the cards this player has the greates weighting of
    // this must be the last part to make use of 'cards', since 'cards' is empty after this part

    while (!cards.empty()) {
      Card const& card = cards.highest();
      // player with the maximal weighting
      int weighting_max = INT_MIN;
      for (unsigned p = 0; p < game.playerno(); ++p) {
        if (   !this->cards_information().of_player(p).all_known()    
            && this->cards_information().of_player(p).can_have(card)
            && (this->cards_information().of_player(p).weighting(card)
                > weighting_max) )
          weighting_max
            = this->cards_information().of_player(p).weighting(card);
      }

#ifdef CONDITION_INFORMATION_ESTIMATED_HAND 
      if (CONDITION_INFORMATION_ESTIMATED_HAND) {
        clog << __FILE__ << '#' << __LINE__ << "  " << "4) " << setw(5) << cards.highest_weighting() << " -- "
          << setw(5) << weighting_max << " - 40: " << card << '\n';
      }
#endif
      if (cards.highest_weighting() >= weighting_max - 40) { // *Value* (same below, 2 times)
#ifdef CONDITION_INFORMATION_ESTIMATED_HAND 
        if (CONDITION_INFORMATION_ESTIMATED_HAND) {
          clog << __FILE__ << '#' << __LINE__ << "  " << "4) add card " << card << '\n';
        }
#endif
        hand.add(card);
      }

      cards.pop_highest();
    } // while (!cards.empty())
  } // 4) add the cards this player has the greates weighting of

#ifdef CONDITION_INFORMATION_ESTIMATED_HAND 
  if (CONDITION_INFORMATION_ESTIMATED_HAND) {
    clog << __FILE__ << '#' << __LINE__ << "  " << "end CardsInformation(" << this->game().no(this->cards_information().player()) << ")::OfPlayer(" << this->game().no(this->player()) << ")::estimate_hand()\n";
  }
#endif

  return hand;
} // Hand CardsInformation::OfPlayer::estimated_hand() const

/**
 ** -> result
 **
 ** @param	hand   hand to check
 **
 ** @return	whether the hand is valid
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.4
 **/
bool
CardsInformation::OfPlayer::hand_valid(Hand const& hand) const
{
  map<Card, unsigned> const counter = hand.counted_cards();

  for (map<Card, unsigned>::const_iterator
       c = counter.begin();
       c != counter.end();
       ++c) {
    Card const& card = c->first;
    unsigned const& n = c->second;
    if (!(   (this->must_have(card) <= n)
          && (this->can_have(card) >= n)) )
      return false;
  } // for (c \in counter)

  for (unsigned c = 0; c < Card::NUMBER_OF_TCOLORS; ++c) {
    Card::TColor const tcolor = static_cast<Card::TColor>(c);
    unsigned const n = hand.numberof(tcolor);
    if (!(   (this->must_have(tcolor) <= n)
          && (this->can_have(tcolor) >= n)) )
      return false;
  } // for (c < Card::NUMBER_OF_TCOLORS)

  return true;
} // bool CardsInformation::OfPlayer::hand_valid(Hand hand) const

/**
 ** the game starts
 **
 ** @param	-
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.4
 **/
void
CardsInformation::OfPlayer::game_start()
{
#ifdef CONDITION_INFORMATION_FLOW
  if (CONDITION_INFORMATION_FLOW)
    cout << "CardsInformation(" << this->game().no(this->cards_information().player()) << ")::OfPlayer(" << this->playerno() << ")::game_start()\n";
#endif
  switch (this->game().type()) {
  case GAMETYPE::MARRIAGE:
    if (this->player() == this->game().soloplayer()) {
      this->add_must_have(Card::CLUB_QUEEN, 2);
      // tcolor is set in CardsInformation::do_update()
    }
    break;
  case GAMETYPE::POVERTY:
    /// the player who takes the poverty
    if (this->player() == this->game().poverty_partner())
      CardsInformationHeuristics::poverty_returned
        (this->player(),
         this->cards_information().player(),
         this->cards_weighting_,
         this->game().poverty_trumpno_returned());
    if (this->player() == this->game().soloplayer())
      if (this->game().poverty_trumpno_returned() == 0)
        this->add_must_exactly_have(Card::TRUMP, this->game().poverty_trumpno_returned());
    break;
  default:
    break;
  } // switch (this->game().type())

  return ;
} // void CardsInformation::OfPlayer::game_start()

/**
 ** the player has played 'card'
 **
 ** @param	card	card that has been played
 ** @param	trick   current trick
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.3
 **/
void
CardsInformation::OfPlayer::card_played(HandCard const& card,
                                        Trick const& trick)
{
  if (card.is_unknown()) {
    this->played_.inc(card);
    this->tcolor_played_.inc(card.tcolor());
    return ;
  }

#ifdef CONDITION_INFORMATION_FLOW
  if (CONDITION_INFORMATION_FLOW)
    cout << "CardsInformation(" << this->game().no(this->cards_information().player()) << ")::OfPlayer(" << this->playerno() << ")::"
      << "card_played(card = " << card << ", trick)\n";
#endif
  if (!this->can_have(card)) {
#ifdef DEBUG_ASSERT
    if (!this->cards_information().is_virtual()) {
      cerr << "--\n"
        << __FILE__ << "#" << __LINE__ << ": "
        << "Error in 'CardsInformation(" << this->game().no(this->cards_information().player()) << ")::OfPlayer(" << this->playerno() << ")"
        << "::card_played(" << card << ")\n";
      cerr << '\n';
      cerr << "trick " << this->game().trick_current_no() << '\n';
      cerr << card.game().trick_current() << '\n';
      cerr << '\n';
      cerr << this->cards_information() << endl;
      cerr << "Hand:\n" << this->player().hand();
      cerr << '\n';
      //cerr << *this;
      cerr << endl;
    }
#endif
    DEBUG_ASSERTION(this->cards_information().is_virtual(),
                    "CardsInformation"
                    << "(" << this->game().no(this->cards_information().player()) << ")"
                    << "::OfPlayer(" << this->playerno() << ")"
                    << "::card_played(" << card << "):\n"
                    "  card cannot be on the hand.\n"
                    << "  trick = " << this->game().trick_current_no() << '\n'
                    //<< "  Game:\n" << this->game()
                    //<< "  Information:\n" << this->cards_information()
                    //<< "  Hand:\n" << this->player().hand()
                   );
#ifdef INFO_THROW
    clog << __FILE__ << '#' << __LINE__ << "  " << "\n  CardsInformation(" << this->game().no(this->cards_information().player())
      << ")::OfPlayer(" << this->playerno() << ")::"
      << "throw in card_played(" << card << ", trick" << ")\n"
      << trick;
#endif
    DEBUG_THROW(InvalidGameException, InvalidGameException());
  } // if (!this->can_have(card))

  this->played_.inc(card);
  if (this->must_have(card))
    this->must_have_.dec(card);
  this->can_have_.dec(card);
  Card::TColor const tcolor = card.tcolor();
  this->tcolor_played_.inc(tcolor);
  if (this->must_have(tcolor))
    this->tcolor_must_have_.dec(tcolor);
  this->tcolor_can_have_.dec(tcolor);

  if (!this->cards_information().in_recalcing)
    this->update_remaining_cards();

  return ;
} // void CardsInformation::OfPlayer::card_played(HandCard card, Trick trick)

/**
 ** change the weightings according to the played card
 ** call some heuristics which 
 **
 ** @param     card    played_card
 ** @param     trick   current trick
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.3
 **
 ** @todo      better name: all cards can be weighted
 **/
void
CardsInformation::OfPlayer::weight_played_card(HandCard const& card,
                                               Trick const& trick)
{
  // no information from unknown cards
  if (card.is_unknown())
    return ;
  // I do not get more information from my own cardplay
  if (card.player() == this->cards_information().player())
    return ;

#ifdef CONDITION_INFORMATION_FLOW
  if (CONDITION_INFORMATION_FLOW)
    cout << "CardsInformation(" << this->game().no(this->cards_information().player()) << ")::OfPlayer(" << this->playerno() << ")::"
      << "weight_played_card(card = " << card << ", trick)\n";
#endif

  CardsInformationHeuristics::card_played(card, trick,
                                          this->cards_information().player(),
                                          this->cards_weighting_);

  return ;
} // void CardsInformation::OfPlayer::weight_played_card(HandCard const& card)

/**
 ** adds the information that the player must have the card 'no' times
 **
 ** @param	card	card that the player must have
 ** @param	no	how many times the player must have the card
 **			default: 1
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.9
 **/
void
CardsInformation::OfPlayer::add_must_have(Card const& card, unsigned const no)
{
#ifdef CONDITION_INFORMATION_FLOW
  if (CONDITION_INFORMATION_FLOW) {
    if (this->must_have(card) < no) {
      cout << "CardsInformation(" << this->game().no(this->cards_information().player()) << ")::OfPlayer(" << this->playerno() << ")::"
        << "add_must_have(card = " << card << ", no = " << no << ")\n";
    }
  }
#endif
  if (no > this->can_have(card)) {
    DEBUG_ASSERTION(this->cards_information().is_virtual(),
                    "CardsInformation"
                    << "(" << this->game().no(this->cards_information().player()) << ")"
                    << "::OfPlayer(" << this->playerno() << ")"
                    << "::add_must_have(" << card << ", " << no << ")\n"
                    "  player = " << this->playerno() << "\n"
                    "  can_have(" << card << ") = " << this->can_have(card)
                    << " < " << no << " = no");
#ifdef INFO_THROW
    clog << __FILE__ << '#' << __LINE__ << "  " << "\n  CardsInformation(" << this->game().no(this->cards_information().player())
      << ")::OfPlayer(" << this->playerno() << ")::"
      << "throw in add_must_have(" << card << ", " << no << ")\n";
#endif
    DEBUG_THROW(InvalidGameException, InvalidGameException());
  } // if (no > this->can_have(card))

  if (this->must_have_.min_set(card, no))
    this->cards_information().queue_update(card);

  return ;
} // void CardsInformation::OfPlayer::add_must_have(Card card, unsigned no = 1)

/**
 ** adds the information that the player must have the cards
 **
 ** @param	cards	cards the player must have
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
void
CardsInformation::OfPlayer::add_must_have(vector<Card> const& cards)
{
  map<Card, unsigned> number;
  for (vector<Card>::const_iterator c = cards.begin();
       c != cards.end();
       ++c)
    number[*c] += 1;

  for (map<Card, unsigned>::const_iterator n = number.begin();
       n != number.end();
       ++n)
    this->add_must_have(n->first, n->second);

  return ;
} // void CardsInformation::OfPlayer::add_must_have(vector<Card> cards)

/**
 ** adds the information that the player must have the tcolor 'no' times
 **
 ** @param	tcolor   tcolor that the player must have
 ** @param	no       how many times the player must have the tcolor 
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
void
CardsInformation::OfPlayer::add_must_have(Card::TColor const& tcolor,
                                          unsigned const no)
{
#ifdef CONDITION_INFORMATION_FLOW
  if (CONDITION_INFORMATION_FLOW) {
    if (this->must_have(tcolor) < no) {
      cout << "CardsInformation(" << this->game().no(this->cards_information().player()) << ")::OfPlayer(" << this->playerno() << ")::"
        << "add_must_have(tcolor = " << tcolor << ", no = " << no << ")\n";
    }
  }
#endif

  if (no > this->can_have(tcolor)) {
    DEBUG_ASSERTION(this->cards_information().is_virtual(),
                    "CardsInformation"
                    << "(" << this->game().no(this->cards_information().player()) << ")"
                    << "::OfPlayer(" << this->playerno() << ")"
                    << "::add_must_have(" << tcolor << ", " << no << "):\n"
                    "  no = " << no << " < " << this->can_have(tcolor) << " = can have\n"
                    "  player = " << this->playerno());
#ifdef INFO_THROW
    clog << __FILE__ << '#' << __LINE__ << "  " << "\n  CardsInformation(" << this->game().no(this->cards_information().player())
      << ")::OfPlayer(" << this->playerno() << ")::"
      << "throw in add_must_have(" << tcolor << ", " << no << ")\n";
#endif
    DEBUG_THROW(InvalidGameException, InvalidGameException());
  } // if (no < this->must_have(card))

  if (this->tcolor_must_have_.cards_no()
      - this->must_have(tcolor)
      + no
      > this->player().cards_to_play() ) {
    DEBUG_ASSERTION(this->cards_information().is_virtual(),
                    "CardsInformation"
                    << "(" << this->game().no(this->cards_information().player()) << ")"
                    << "::OfPlayer(" << this->playerno() << ")"
                    << "::add_must_have(" << tcolor << ", " << no << "):\n"
                    "  no too great" << '\n');
#ifdef INFO_THROW
    clog << __FILE__ << '#' << __LINE__ << "  " << "\n  CardsInformation(" << this->game().no(this->cards_information().player())
      << ")::OfPlayer(" << this->playerno() << ")::"
      << "throw in add_must_have(" << tcolor << ", " << no << ")\n";
#endif
    DEBUG_THROW(InvalidGameException, InvalidGameException());
  } // if (tcolor_must_have - must_have(tcolor) + no > cards to play)

  if (this->tcolor_must_have_.min_set(tcolor, no)) {
    this->cards_information().queue_update(tcolor);
  }

  return ;
} // void CardsInformation::OfPlayer::add_must_have(Card::TColor tcolor, unsigned no)

/**
 ** adds the information that the player must have the tcolor exactly 'no' times
 **
 ** @param	tcolor   tcolor that the player must have
 ** @param	no       how many times exactly the player must have the tcolor 
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
void
CardsInformation::OfPlayer::add_must_exactly_have(Card::TColor const& tcolor,
                                                  unsigned const no)
{
#ifdef CONDITION_INFORMATION_FLOW
  if (CONDITION_INFORMATION_FLOW)
    if (   (this->must_have(tcolor) < no)
        || (this->can_have(tcolor) > no) )
      cout << "CardsInformation(" << this->game().no(this->cards_information().player()) << ")::OfPlayer(" << this->playerno() << ")::"
        << "add_must_exactly_have(tcolor = " << tcolor << ", no = " << no << ")\n";
#endif

  this->add_can_have(tcolor, no);
  this->add_must_have(tcolor, no);

  return ;
} // void CardsInformation::OfPlayer::add_must_exactly_have(Card::TColor tcolor, unsigned no)

/**
 ** adds the information that the player can have the card at max 'no' times
 **
 ** @param	card	card that the player can have
 ** @param	no	how many times the player can have the card at max
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.9
 **/
void
CardsInformation::OfPlayer::add_can_have(Card const& card, unsigned const no)
{
#ifdef CONDITION_INFORMATION_FLOW
  if (CONDITION_INFORMATION_FLOW)
    if (this->can_have(card) > no)
      cout << "CardsInformation(" << this->game().no(this->cards_information().player()) << ")::OfPlayer(" << this->playerno() << ")::"
        << "add_can_have(card = " << card << ", no = " << no << ")\n";
#endif
  if (no < this->must_have(card)) {
    DEBUG_ASSERTION(this->cards_information().is_virtual(),
                    "CardsInformation"
                    << "(" << this->game().no(this->cards_information().player()) << ")"
                    << "::OfPlayer(" << this->playerno() << ")"
                    << "::add_can_have(" << card << ", " << no << "):\n"
                    "  no < " << this->must_have(card) << " = must have\n"
                    "  player = " << this->playerno());
#ifdef INFO_THROW
    clog << __FILE__ << '#' << __LINE__ << "  " << "\n  CardsInformation(" << this->game().no(this->cards_information().player())
      << ")::OfPlayer(" << this->playerno() << ")::"
      << "throw in add_can_have(" << card << ", " << no << ")\n";
#endif
    DEBUG_THROW(InvalidGameException, InvalidGameException());
  } // if (no < this->must_have(card))

  if (this->can_have_.max_set(card, no))
    this->cards_information().queue_update(card);

  // now erase the 'can_have' for all cards of 'tcolor'

  return ;
} // void CardsInformation::OfPlayer::add_can_have(Card card, unsigned no)

/**
 ** adds the information that the player can have the cards at max 'no' times
 **
 ** @param	cards	cards that the player can have
 ** @param	no	how many times the player can have the card at max
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
void
CardsInformation::OfPlayer::add_can_have(vector<Card> const& cards,
                                         unsigned const no)
{
  for (vector<Card>::const_iterator c = cards.begin();
       c != cards.end();
       ++c)
    this->add_can_have(*c, no);

  return ;
}  // void CardsInformation::OfPlayer::add_can_have(vector<Card> cards, unsigned no)

/**
 ** adds the information that the player can only have the cards
 **
 ** @param	cards	cards the player can only have
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.9
 **/
void
CardsInformation::OfPlayer::add_can_only_have(vector<Card> const& cards)
{
#ifdef CONDITION_INFORMATION_FLOW
  if (CONDITION_INFORMATION_FLOW) {
#if 0
    cout << "CardsInformation(" << this->game().no(this->cards_information().player()) << ")::OfPlayer(" << this->playerno() << ")::"
      << "add_can_only_have(cards)\n";
#else
    cout << "CardsInformation(" << this->game().no(this->cards_information().player()) << ")::OfPlayer(" << this->playerno() << ")::"
      << "add_can_only_have(cards = ";
    for (vector<Card>::const_iterator c = cards.begin(); c != cards.end(); ++c)
      cout << ((c == cards.begin()) ? "" : ", ") << *c;
    cout << ")" << endl;
#endif
  }
#endif
  map<Card, unsigned> number;
  for (vector<Card>::const_iterator c = cards.begin();
       c != cards.end();
       ++c) {
    number[*c] += 1;
  }

  unsigned const unknown_no = std::count(cards.begin(), cards.end(), Card::UNKNOWN);
  if (unknown_no == 0) {
    // remove all entries in 'can_have' which have the value '0'
    // count the number of cards the player can have
    vector<Card> to_remove;
    for (CardCounter::const_iterator x = this->can_have_.begin();
         x != this->can_have_.end();
         ++x) {
      map<Card, unsigned>::const_iterator n = number.find(x->first);
      if (n == number.end()) {
        if (this->must_have(x->first)) {
          DEBUG_ASSERTION(this->cards_information().is_virtual(),
                          "CardsInformation"
                          << "(" << this->game().no(this->cards_information().player()) << ")"
                          << "::OfPlayer(" << this->playerno() << ")"
                          "::add_can_only_have():\n"
                          "  card '" << x->first << "':\n"
                          "    new number = 0 < "
                          << this->must_have(x->first) << " = must_have");
#ifdef INFO_THROW
          clog << __FILE__ << '#' << __LINE__ << "  " << x->first << ": " << this->must_have(x->first) << endl;
          clog << *this << endl;
          clog << __FILE__ << '#' << __LINE__ << "  " << "\n  CardsInformation(" << this->game().no(this->cards_information().player())
            << ")::OfPlayer(" << this->playerno() << ")::"
            << "throw in add_can_only_have(cards): " << x->first << "\n";
#endif
          DEBUG_THROW(InvalidGameException, InvalidGameException());
        } // if (this->must_have(x->first))
        to_remove.push_back(x->first);
        continue;
      }

      if (n->second < this->must_have(x->first)) {
        DEBUG_ASSERTION(this->cards_information().is_virtual(),
                        "CardsInformation"
                        << "(" << this->game().no(this->cards_information().player()) << ")"
                        << "::OfPlayer(" << this->playerno() << ")"
                        "::add_can_only_have():\n"
                        "  card '" << x->first << "':\n"
                        "    new number = " << n->second << " < "
                        << this->must_have(x->first) << " = must_have");
#ifdef INFO_THROW
        clog << __FILE__ << '#' << __LINE__ << "  " << "\n  CardsInformation(" << this->game().no(this->cards_information().player())
          << ")::OfPlayer(" << this->playerno() << ")::"
          << "throw in add_can_only_have(cards)\n";
#endif
        DEBUG_THROW(InvalidGameException, InvalidGameException());
      } // if (n->second < this->must_have(x->first))
    } // for (x \in can_have)

    // remove the cards with value '0' from the list
    if (!to_remove.empty()) {
      for (vector<Card>::const_iterator c = to_remove.begin();
           c != to_remove.end();
           ++c) {
        if (this->must_have(*c)) {
          DEBUG_ASSERTION(this->cards_information().is_virtual(),
                          "CardsInformation"
                          << "(" << this->game().no(this->cards_information().player()) << ")"
                          << "::OfPlayer(" << this->playerno() << ")"
                          << "):\n"
                          "  card '" << *c << "':\n"
                          "    new number = 0 < "
                          << this->must_have(*c) << " = must_have");
#ifdef INFO_THROW
          clog << __FILE__ << '#' << __LINE__ << "  " << "\n  CardsInformation(" << this->game().no(this->cards_information().player())
            << ")::OfPlayer(" << this->playerno() << ")::"
            << "throw in add_can_only_have(cards)\n";
#endif
          DEBUG_THROW(InvalidGameException, InvalidGameException());
        } // if (this->must_have(*c))

        this->add_cannot_have(*c);
      } // for (c \in to_remove)
    } // if (!to_remove.empty())
  } // if (no unknown card)

  // set the maximal number of the cards
  for (map<Card, unsigned>::const_iterator n = number.begin();
       n != number.end();
       ++n) {
    if (this->can_have_.max_set(n->first, n->second + unknown_no))
      this->cards_information().queue_update(n->first);
  } // for (n \in number)

  if (   (::game_status != GAMESTATUS::GAME_REDISTRIBUTE)
      && (this->can_have_.cards_no() < this->player().cards_to_play())) {
    DEBUG_ASSERTION(this->cards_information().is_virtual(),
                    "CardsInformation"
                    << "(" << this->game().no(this->cards_information().player()) << ")"
                    << "::OfPlayer(" << this->playerno() << ")"
                    << "::add_can_only_have(cards)\n"
                    "  can_have.cards_no() = " << this->can_have_.cards_no()
                    << " < "
                    << this->player().cards_to_play() << " = cards to play");
#ifdef INFO_THROW
    clog << __FILE__ << '#' << __LINE__ << "  " << "\n  CardsInformation(" << this->game().no(this->cards_information().player())
      << ")::OfPlayer(" << this->playerno() << ")::"
      << "throw in add_can_only_have(cards)\n";
#endif
    DEBUG_THROW(InvalidGameException, InvalidGameException());
  }

  return ;
} // void CardsInformation::OfPlayer::add_can_only_have(vector<Card> cards)

/**
 ** adds the information that the player cannot have the card
 **
 ** @param	card	card the player cannot have
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.9
 **/
void
CardsInformation::OfPlayer::add_cannot_have(Card const& card)
{
#ifdef CONDITION_INFORMATION_FLOW
  if (CONDITION_INFORMATION_FLOW)
    if (this->can_have(card) > 0)
      cout << "CardsInformation(" << this->game().no(this->cards_information().player()) << ")::OfPlayer(" << this->playerno() << ")::"
        << "add_cannot_have(card = " << card << ")\n";
#endif
  if (this->must_have(card)) {
    DEBUG_ASSERTION(this->cards_information().is_virtual(),
                    "CardsInformation"
                    << "(" << this->game().no(this->cards_information().player()) << ")"
                    << "::OfPlayer(" << this->playerno() << ")"
                    << "::add_cannot_have(" << card << ")\n"
                    "  player = " << this->playerno() << "\n"
                    "  must_have(card) = " << this->must_have(card) << " > 0");
#ifdef INFO_THROW
    clog << __FILE__ << '#' << __LINE__ << "  " << "\n  CardsInformation(" << this->game().no(this->cards_information().player())
      << ")::OfPlayer(" << this->playerno() << ")::"
      << "throw in add_cannot_have(" << card << ")\n";
#endif
    DEBUG_THROW(InvalidGameException, InvalidGameException());
  } // if (this->must_have(card))

  if (this->can_have_.erase(card)) {
    if (this->must_have(card) > 0) {
      DEBUG_ASSERTION(this->cards_information().is_virtual(),
                      "CardsInformation"
                      << "(" << this->game().no(this->cards_information().player()) << ")"
                      << "::OfPlayer(" << this->playerno() << ")"
                      << "::add_cannot_have(" << card << "):\n"
                      << "  must_have = " << this->must_have(card) << '\n'
                      << "  player = " << this->playerno());
#ifdef INFO_THROW
      clog << __FILE__ << '#' << __LINE__ << "  " << "\n  CardsInformation(" << this->game().no(this->cards_information().player())
        << ")::OfPlayer(" << this->playerno() << ")::"
        << "throw in add_cannot_have(" << card << ")\n";
#endif
      DEBUG_THROW(InvalidGameException, InvalidGameException());
    } // if (this->must_have(card))
    if (this->can_have_.cards_no() < this->player().cards_to_play()) {
      DEBUG_ASSERTION(this->cards_information().is_virtual(),
                      "CardsInformation"
                      << "(" << this->game().no(this->cards_information().player()) << ")"
                      << "::OfPlayer(" << this->playerno() << ")"
                      << "::add_cannot_have(" << card << ")\n"
                      "  player = " << this->playerno() << "\n"
                      "  can_have.cards_no() = " << this->can_have_.cards_no()
                      << " < "
                      << this->player().cards_to_play() << " = cards to play");
#ifdef INFO_THROW
      clog << __FILE__ << '#' << __LINE__ << "  " << "\n  CardsInformation(" << this->game().no(this->cards_information().player())
        << ")::OfPlayer(" << this->playerno() << ")::"
        << "throw in add_cannot_have(" << card << ")\n"
        << "can have = " << this->can_have_.cards_no()
        << " < " << this->player().cards_to_play() << " = cards to play\n";
#endif
      DEBUG_THROW(InvalidGameException, InvalidGameException());
    } // if (this->can_have_.cards_no() < this->player().cards_to_play())

#ifdef WORKAROUND
    // if only one player can have the card, he must have it (see 'update_card')
    { // check whether all cards of 'card' are either played,
      // one player has it or it can be forgotten
      unsigned distributed = this->cards_information().played(card);
      for (unsigned p = 0; p < this->game().playerno(); ++p)
        distributed += this->cards_information().of_player(p).can_have(card);
      distributed += this->cards_information().forgotten_cards_no();

      if (distributed < this->game().rule()(Rule::NUMBER_OF_SAME_CARDS)) {
        DEBUG_ASSERTION(this->cards_information().is_virtual(),
                        "CardsInformation"
                        << "(" << this->game().no(this->cards_information().player()) << ")"
                        << "::OfPlayer(" << this->playerno() << ")"
                        << "::add_cannot_have"
                        "(" << card << ")\n"
                        "  player = " << this->playerno() << "\n"
                        "  the card is not distributed");
#ifdef INFO_THROW
        clog << __FILE__ << '#' << __LINE__ << "  " << "\n  CardsInformation(" << this->game().no(this->cards_information().player())
          << ")::OfPlayer(" << this->playerno() << ")::"
          << "throw in add_cannot_have(" << card << ")\n";
#endif
        DEBUG_THROW(InvalidGameException, InvalidGameException());
      } // if (distributed < this->game().rule()(Rule::NUMBER_OF_SAME_CARDS))


    } // check whether all cards of 'card' are somewhere
#endif
    this->cards_information().queue_update(card);
  } // if (this->can_have_.erase(card))

  return ;
} // void CardsInformation::OfPlayer::add_cannot_have(Card card)

/**
 ** adds the information that the player can have the tcolor at max 'no' times
 **
 ** @param	tcolor   tcolor that the player can have
 ** @param	no       how many times the player can have the tcolor at max
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
void
CardsInformation::OfPlayer::add_can_have(Card::TColor const& tcolor,
                                         unsigned const no)
{
#ifdef CONDITION_INFORMATION_FLOW
  if (CONDITION_INFORMATION_FLOW)
    if (this->can_have(tcolor) > no)
      cout << "CardsInformation(" << this->game().no(this->cards_information().player()) << ")::OfPlayer(" << this->playerno() << ")::"
        << "add_can_have(tcolor = " << tcolor << ", no = " << no << ")\n";
#endif
  DEBUG_ASSERTION(no <= this->game().cards_no(),
                  "CardsInformation"
                  << "(" << this->game().no(this->cards_information().player()) << ")"
                  << "::OfPlayer(" << this->playerno() << ")"
                  << "::add_can_have(" << tcolor << ", " << no << "):\n"
                  "  no = " << no << " too great");
  if (no < this->must_have(tcolor)) {
    DEBUG_ASSERTION(this->cards_information().is_virtual(),
                    "CardsInformation"
                    << "(" << this->game().no(this->cards_information().player()) << ")"
                    << "::OfPlayer(" << this->playerno() << ")"
                    << "::add_can_have(" << tcolor << ", " << no << "):\n"
                    "  no = " << no << " < " << this->must_have(tcolor) << " = must have\n"
                    "  player = " << this->playerno() << '\n'
                    << *this
                   );
#ifdef INFO_THROW
    clog << this->game() << endl;
    clog << *this << endl;
    clog << __FILE__ << '#' << __LINE__ << "  " << "\n  CardsInformation(" << this->game().no(this->cards_information().player())
      << ")::OfPlayer(" << this->playerno() << ")::"
      << "throw in add_can_have(" << tcolor << ", " << no << ")\n";
#endif
    DEBUG_THROW(InvalidGameException, InvalidGameException());
  } // if (no < this->must_have(card))

  if (this->tcolor_can_have_.max_set(tcolor, no)) {
    this->cards_information().queue_update(tcolor);
  }

  return ;
} // void CardsInformation::OfPlayer::add_can_have(Card::TColor tcolor, unsigned no)

/**
 ** adds the information that the player cannot have the cards
 **
 ** @param	cards	cards the player cannot have
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.9
 **/
void
CardsInformation::OfPlayer::add_cannot_have(vector<Card> const& cards)
{
  for (vector<Card>::const_iterator c = cards.begin();
       c != cards.end();
       ++c)
    this->add_cannot_have(*c);

  return ;
} // void CardsInformation::OfPlayer::add_cannot_have(vector<Card> cards)

/**
 ** adds the information that the player cannot have the tcolor
 **
 ** @param	tcolor	tcolor the player cannot have
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
void
CardsInformation::OfPlayer::add_cannot_have(Card::TColor const tcolor)
{
#ifdef CONDITION_INFORMATION_FLOW
  if (CONDITION_INFORMATION_FLOW) {
    cout << "CardsInformation(" << this->game().no(this->cards_information().player()) << ")::OfPlayer(" << this->playerno() << ")::"
      << "add_does_not_have(tcolor = " << tcolor << ")\n";
    COUT << this->player().hand() << endl;
  }
#endif

  this->add_must_exactly_have(tcolor, 0);

  return ;
} // void CardsInformation::OfPlayer::add_cannot_have(Card::TColor tcolor)

/**
 ** updates the information of the card
 ** throws InvalidGameException
 **
 ** @param	card	card to update the information of
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.9
 **/
void
CardsInformation::OfPlayer::update_information(Card const& card)
{
  if (card.is_unknown())
    return;

#ifdef CONDITION_INFORMATION_FLOW
  if (CONDITION_INFORMATION_FLOW) {
    cout << "CardsInformation(" << this->game().no(this->cards_information().player()) << ")::OfPlayer(" << this->playerno() << ")::"
      << "update_information(card = " << card << ")\n";
  }
#endif
  if (this->cards_information().in_recalcing)
    return ;

  // how many times this card was played
  unsigned const played = this->cards_information().played(card);
  // the number of forgotten cards
  unsigned const forgotten_cards
    = this->cards_information().forgotten_cards_no();
  // count how many of the card the other player must have
  unsigned must_have = 0;
  // count how many of the card the other player can have
  unsigned can_have = 0;
  for (unsigned p = 0; p < this->game().playerno(); ++p) {
    if (p != this->playerno()) {
      must_have += this->cards_information().of_player(p).must_have(card);
      can_have += this->cards_information().of_player(p).can_have(card);
    }
  } // for (p < this->game().playerno())

#ifdef WORKAROUND
  // see bug report 232744
  // the cards distribution shall be sufficient
  // so that this case does not happen
  if (   (played + must_have > this->game().rule()(Rule::NUMBER_OF_SAME_CARDS))
      && !card.is_unknown()
      && this->cards_information().is_virtual()) {
#ifdef INFO_THROW
    clog << __FILE__ << '#' << __LINE__ << "  " << "\n  CardsInformation(" << this->game().no(this->cards_information().player())
      << ")::OfPlayer(" << this->playerno() << ")::"
      << "throw in update_information(" << card << ")\n";
#endif
    DEBUG_THROW(InvalidGameException, InvalidGameException());
  }
#endif

  if (played + must_have
      > this->game().rule()(Rule::NUMBER_OF_SAME_CARDS)) {
    DEBUG_ASSERTION(this->cards_information().is_virtual(),
                    "CardsInformation"
                    << "(" << this->game().no(this->cards_information().player()) << ")"
                    << "::OfPlayer(" << this->playerno() << ")"
                    << "::update_information(" << card << ")\n"
                    "  played + must_have > max number:\n"
                    "  " << played
                    << " + " << must_have << " > "
                    << this->game().rule()(Rule::NUMBER_OF_SAME_CARDS)
                   );
#ifdef INFO_THROW
    clog << __FILE__ << '#' << __LINE__ << "  " << "\n  CardsInformation(" << this->game().no(this->cards_information().player())
      << ")::OfPlayer(" << this->playerno() << ")::"
      << "throw in update_information(" << card << ")\n";
#endif
    DEBUG_THROW(InvalidGameException, InvalidGameException());
  } // if (played + must_have > NUMBER_OF_SAME_CARDS)


  // the number of cards this player can at most have
  unsigned const n = min(this->game().rule()(Rule::NUMBER_OF_SAME_CARDS)
                         - played
                         - must_have,
                         this->can_have(card.tcolor(this->game()))
                         - this->must_have_(card.tcolor(this->game()),
                                            this->game())
                         + this->must_have(card)
                        );
  if (n < this->must_have(card)) {
    DEBUG_ASSERTION(this->cards_information().is_virtual(),
                    "CardsInformation"
                    << "(" << this->game().no(this->cards_information().player()) << ")"
                    << "::OfPlayer(" << this->playerno() << ")"
                    << "::update_information(" << card << ")\n"
                    "  player = " << this->playerno() << "\n"
                    "  card = " << card << "\n"
                    "  number of same cards - played - must_have total"
                    " < must_have(card):\n"
                    << this->game().rule()(Rule::NUMBER_OF_SAME_CARDS)
                    << " - " << played
                    << " - " << must_have
                    << " < " << this->must_have(card));
#ifdef INFO_THROW
    clog << __FILE__ << '#' << __LINE__ << "  " << "\n  CardsInformation(" << this->game().no(this->cards_information().player())
      << ")::OfPlayer(" << this->playerno() << ")::"
      << "throw in update_information(" << card << ")\n";
#endif
    DEBUG_THROW(InvalidGameException, InvalidGameException());
  } // if (n < this->must_have(card))

  if (this->can_have_.max_set(card, n))
    this->cards_information().queue_update(card);

  // check whether a player must have the remaining cards of 'card'
  if (can_have + played + this->must_have(card) + forgotten_cards
      < this->game().rule()(Rule::NUMBER_OF_SAME_CARDS)) {
    this->add_must_have(card,
                        this->game().rule()(Rule::NUMBER_OF_SAME_CARDS)
                        - can_have - played - forgotten_cards);
  }


  if (this->cards_information().is_virtual()) {
    if (   (this->can_have_.cards_no() < this->player().cards_to_play())
        || (this->must_have_.cards_no() > this->player().cards_to_play()) ) {
#ifdef INFO_THROW
      clog << __FILE__ << '#' << __LINE__ << "  " << "\n  CardsInformation(" << this->game().no(this->cards_information().player())
        << ")::OfPlayer(" << this->playerno() << ")::"
        << "throw in update_information(" << card << ")\n";
#endif
      DEBUG_THROW(InvalidGameException, InvalidGameException());
    }
  } // if (this->cards_information().is_virtual(),

  if (this->can_have_.cards_no() < this->player().cards_to_play()) {
    DEBUG_ASSERTION(this->cards_information().is_virtual(),
                    "CardsInformation"
                    << "(" << this->game().no(this->cards_information().player()) << ")"
                    << "::OfPlayer(" << this->playerno() << ")"
                    << "::update_information(" << card << ")\n"
                    << "  can_have.cards_no() = "
                    << this->can_have_.cards_no()
                    << " < "
                    << this->player().cards_to_play() << " = cards to play");
#ifdef INFO_THROW
    clog << __FILE__ << '#' << __LINE__ << "  " << "\n  CardsInformation(" << this->game().no(this->cards_information().player())
      << ")::OfPlayer(" << this->playerno() << ")::"
      << "throw in update_information(" << card << ")\n";
#endif
    DEBUG_THROW(InvalidGameException, InvalidGameException());
  } // if (this->can_have_.cards_no() < this->player().cards_to_play())

  if (this->must_have_.cards_no() > this->player().cards_to_play()) {
    DEBUG_ASSERTION(this->cards_information().is_virtual(),
                    "CardsInformation"
                    << "(" << this->game().no(this->cards_information().player()) << ")"
                    << "::OfPlayer(" << this->playerno() << ")"
                    << "::update_information(" << card << ")\n"
                    << "  must_have.cards_no() = "
                    << this->must_have_.cards_no()
                    << " > "
                    << this->player().cards_to_play() << " = cards to play");
#ifdef INFO_THROW
    clog << __FILE__ << '#' << __LINE__ << "  " << "\n  CardsInformation(" << this->game().no(this->cards_information().player())
      << ")::OfPlayer(" << this->playerno() << ")::"
      << "throw in update_information(" << card << ")\n";
#endif
    DEBUG_THROW(InvalidGameException, InvalidGameException());
  } // if ((this->must_have_.cards_no() > this->player().cards_to_play())

  return ;
} // void CardsInformation::OfPlayer::update_information(Card card)

/**
 ** updates the information of the tcolor
 **
 ** @param	tcolor   tcolor to update the information of
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
void
CardsInformation::OfPlayer::update_information(Card::TColor const& tcolor)
{
  if (tcolor == Card::UNKNOWNCARDCOLOR)
    return ;

#ifdef CONDITION_INFORMATION_FLOW
  if (CONDITION_INFORMATION_FLOW)
    cout << "CardsInformation(" << this->game().no(this->cards_information().player()) << ")::OfPlayer(" << this->playerno() << ")::"
      << "update_information(" << tcolor << ")\n";
#endif

  // the number of forgotten cards
  unsigned const forgotten_cards = this->cards_information().forgotten_cards_no();

  { // maximum can have
    // the number of cards still to be played minus must have of other colors
    if (this->player().cards_to_play()
        + this->must_have(tcolor)
        < this->tcolor_must_have_.cards_no()) {
      DEBUG_ASSERTION(this->game().isvirtual(),
                      "CardsInformation(" << this->game().no(this->cards_information().player())
                      << ")::OfPlayer(" << this->playerno() << ")::"
                      << "update_information(" << tcolor << ")\n"
                      << " cards to play + must have(" << tcolor << ") = "
                      << this->player().cards_to_play()
                      << " + " << this->must_have(tcolor)
                      << " < " << this->tcolor_must_have_.cards_no() << " = "
                      << tcolor_must_have_.cards_no());

#ifdef INFO_THROW
      clog << __FILE__ << '#' << __LINE__ << "  " << "\n  CardsInformation(" << this->game().no(this->cards_information().player())
        << ")::OfPlayer(" << this->playerno() << ")::"
        << "update_information(" << tcolor << ")\n";
#endif
      DEBUG_THROW(InvalidGameException, InvalidGameException());
    }

    this->add_can_have(tcolor,
                       this->player().cards_to_play()
                       - this->tcolor_must_have_.cards_no()
                       + this->must_have(tcolor));
  } // maximum can have
  { // minimum must have
    // the number of cards still to be played minus can have of other colors
    if (this->player().cards_to_play()
        > this->tcolor_can_have_.cards_no()
        - this->can_have(tcolor)) {
      this->add_must_have(tcolor,
                          this->player().cards_to_play()
                          - (this->tcolor_can_have_.cards_no()
                             - this->can_have(tcolor)));
    }
  } // minimum must have
  { // maximum can have
    // number of cards still in game but not must have of the other players
    unsigned sum_must_have = 0;
    for (unsigned p = 0; p < this->game().playerno(); ++p)
      if (p != this->playerno())
        sum_must_have += this->cards_information().of_player(p).must_have(tcolor);
    if (this->game().cards_no(tcolor)
        < this->cards_information().played(tcolor)
        + sum_must_have) {
      DEBUG_ASSERTION(this->game().isvirtual(),
                      "CardsInformation::OfPlayer::update_information(" << tcolor << ")\n"
                      << "  numberof = " << this->game().cards_no(tcolor)
                      << " > " << this->cards_information().played(tcolor)
                      << " + " << sum_must_have
                      << " = played + sum_must_have");
      DEBUG_THROW(InvalidGameException, InvalidGameException());
    }
    this->add_can_have(tcolor,
                       this->game().cards_no(tcolor)
                       - this->cards_information().played(tcolor)
                       - sum_must_have);
  } // maximum can have
  { // minimum must have
    // number of cards of the color - played - can have of others
    unsigned sum_can_have = 0;
    for (unsigned p = 0; p < this->game().playerno(); ++p)
      if (p != this->playerno())
        sum_can_have += this->cards_information().of_player(p).can_have(tcolor);
    if (this->game().cards_no(tcolor)
        > (forgotten_cards
           + this->cards_information().played(tcolor)
           + sum_can_have) ) {

      this->add_must_have(tcolor,
                          this->game().cards_no(tcolor)
                          - forgotten_cards
                          - this->cards_information().played(tcolor)
                          - sum_can_have);
    }
  } // minimum must have
  // set the can have and must have according to the information of the cards
  this->add_can_have(tcolor, this->can_have_(tcolor, this->game()));
  this->add_must_have(tcolor, this->must_have_(tcolor, this->game()));
  this->check_can_is_must(tcolor);
  return ;
} // void CardsInformation::OfPlayer::update_information(Card::TColor tcolor)

/**
 ** updates the information of the tcolors
 **
 ** @param	-
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
void
CardsInformation::OfPlayer::update_tcolor_information()
{
#ifdef CONDITION_INFORMATION_FLOW
  if (CONDITION_INFORMATION_FLOW)
    cout << "CardsInformation(" << this->game().no(this->cards_information().player()) << ")::OfPlayer(" << this->playerno() << ")::"
      << "update_tcolor_information()\n";
#endif

  for (unsigned c = 0; c < Card::NUMBER_OF_TCOLORS; ++c)
    this->update_information(static_cast<Card::TColor>(c));

  if (!this->all_known()) {
    { // check can is must
      if (this->tcolor_can_have_.cards_no()
          == this->player().cards_to_play()) {
        for (unsigned c = 0; c < Card::NUMBER_OF_TCOLORS; ++c)
          this->add_must_have(static_cast<Card::TColor>(c),
                              this->can_have(static_cast<Card::TColor>(c)));
      }
    } // check can is must

    { // check must is can
      if (this->tcolor_must_have_.cards_no()
          == this->player().cards_to_play()) {
        for (unsigned c = 0; c < Card::NUMBER_OF_TCOLORS; ++c)
          this->add_can_have(static_cast<Card::TColor>(c),
                             this->must_have(static_cast<Card::TColor>(c)));
      }
    } // check must is can
  } // if (!this->all_known())

  // ToDo: if the player has only one free of a color, then he cannot have two of a card (p.e. three trumps, two swines so he cannot have two club kings)

  return ;
} // void CardsInformation::OfPlayer::update_tcolor_information()

/**
 ** updates 'can have' according to 'remaining cards'
 **
 ** @param	-
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.1
 **/
void
CardsInformation::OfPlayer::update_remaining_cards()
{
#ifdef CONDITION_INFORMATION_FLOW
  if (CONDITION_INFORMATION_FLOW)
    cout << "CardsInformation(" << this->game().no(this->cards_information().player()) << ")::OfPlayer(" << this->playerno() << ")::"
      << "update_remaining_cards()\n";
#endif
  unsigned const remaining_cards_no
    = (this->player().cards_to_play() - this->must_have_.cards_no());

  if ( !(remaining_cards_no
         < this->game().rule()(Rule::NUMBER_OF_SAME_CARDS)) )
    return ;
  if ( !(this->must_have_.cards_no() != this->can_have_.cards_no()) )
    return ;

  vector<Card> cards;
  for (CardCounter::const_iterator c = this->can_have_.begin();
       c != this->can_have_.end();
       ++c) {
    if (this->can_have(c->first)
        > remaining_cards_no - this->must_have(c->first))
      cards.push_back(c->first);
  } // for (c \in this->can_have_)
  for (vector<Card>::const_iterator c = cards.begin();
       c != cards.end();
       ++c)
    this->add_can_have(*c, remaining_cards_no + this->must_have(*c));

  return ;
} // void CardsInformation::OfPlayern::update_remaining_cards()

/**
 ** checks whether the cards which the player can have are the ones
 ** he has to have
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
CardsInformation::OfPlayer::check_can_is_must()
{
#ifdef CONDITION_INFORMATION_FLOW
  if (CONDITION_INFORMATION_FLOW)
    cout << "CardsInformation(" << this->game().no(this->cards_information().player()) << ")::OfPlayer(" << this->playerno() << ")::"
      << "check_can_is_must()\n";
#endif
  if (this->can_have_.cards_no() == this->must_have_.cards_no())
    return ;
  if (this->must_have_.cards_no() == this->player().cards_to_play())
    return ;

  // update the tcolor information
  for (unsigned c = 0; c < Card::NUMBER_OF_TCOLORS; ++c)
    this->check_can_is_must(static_cast<Card::TColor>(c));

  if (this->can_have_.cards_no() != this->player().cards_to_play()) {
    if (this->can_have_.cards_no() <
        (this->player().cards_to_play()
         + this->game().rule()(Rule::NUMBER_OF_SAME_CARDS)) ) {
      unsigned const cards_surplus
        = this->can_have_.cards_no() - this->player().cards_to_play();
      for (CardCounter::const_iterator c = this->can_have_.begin();
           c != this->can_have_.end();
           ++c) {
        if (c->second > cards_surplus)
          if (this->must_have_.min_set(c->first, c->second - cards_surplus))
            this->cards_information().queue_update(c->first);
      } // for (c \in this->can_have)
    } // if (surplus less than number of cards)
    { // only one card unknown

      // Check whether there is only one card in 'can have' unknown.
      // Then the remaining number must be the difference between
      // 'cards to play' and 'must have'.

      if (this->can_have_.cards_no()
          >= (this->must_have_.cards_no()
              + this->game().rule()(Rule::NUMBER_OF_SAME_CARDS)))
        return ;

      Card unknown_card;
      for (CardCounter::const_iterator c = this->can_have_.begin();
           c != this->can_have_.end();
           ++c) {
        if (this->must_have_[c->first] != c->second) {
          // found a second unknown card
          if (!unknown_card.is_empty())
            return ;

          unknown_card = c->first;
        } // if (must_have != can_have)
      } // for (c \in this->can_have_)

      this->can_have_.max_set(unknown_card,
                              this->player().cards_to_play()
                              - this->must_have_.cards_no()
                              + this->must_have_[unknown_card]);
    } // only one card unknown
  } // if (can_have != cards_to_play)

  // so all 'can_have' are 'must_have'

  for (CardCounter::const_iterator c = this->can_have_.begin();
       c != this->can_have_.end();
       ++c) {
    if (this->must_have_[c->first] != c->second) {
      if (this->must_have_.min_set(c->first, c->second))
        this->cards_information().queue_update(c->first);
    }
  } // for (c \in this->can_have_)

  return ;
} // void CardsInformation::OfPlayer::check_can_is_must()

/**
 ** checks whether the cards which the player can have are the ones
 ** he has to have
 **
 ** @param	tcolor   tcolor to check
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.9
 **
 ** @todo       information of only one remaining unknown card (see below)
 **/
void
CardsInformation::OfPlayer::check_can_is_must(Card::TColor const& tcolor)
{
#ifdef CONDITION_INFORMATION_FLOW
  if (CONDITION_INFORMATION_FLOW)
    cout << "CardsInformation(" << this->game().no(this->cards_information().player()) << ")::OfPlayer(" << this->playerno() << ")::"
      << "check_can_is_must(" << tcolor << ")\n";
#endif
  if (this->can_have(tcolor) > this->must_have(tcolor))
    return ;
  unsigned const can_have_tcolor = this->can_have_(tcolor, this->game());
  unsigned const must_have_tcolor = this->must_have_(tcolor, this->game());
  if (can_have_tcolor == must_have_tcolor)
    return ;
#ifdef POSTPONED
  // ToDo: check, whether only one card is unknown:
  // either set 'can have' or 'must have' according to the free 
  // can_have(tcolor) - must_have_(tcolor, game()) or
  // can_have_(tcolor, game()) - must_have(tcolor)
  if (can_have_tcolor > this->must_have(tcolor)) {
    if (can_have_tcolor - this->must_have(tcolor) < 
        this->game().rule()(Rule::NUMBER_OF_SAME_CARDS)) {
      // check whether there is only one unknown card
      for (CardCounter::const_iterator c = this->can_have_.begin();
           c != this->can_have_.end();
           ++c) {
        if (c->first.tcolor(this->game()) == tcolor) {
          if (this->must_have_[c->first] != c->second) {
            this->must_have_[c->first].min_set(c->first,
                                               c->second
                                               - (can_have_tcolor
                                                  - this->must_have(tcolor)));
          } // if (must_have != can_have)
        }
      } // for (c \in this->can_have_)

    }
  } // if (can_have_tcolor > this->must_have(tcolor))
#endif

  if (can_have_tcolor == this->must_have(tcolor)) {
    // all cards of tcolor are must have
    for (CardCounter::const_iterator c = this->can_have_.begin();
         c != this->can_have_.end();
         ++c) {
      if (c->first.tcolor(this->game()) == tcolor) {
        if (this->must_have_.min_set(c->first, c->second))
          this->cards_information().queue_update(c->first);
      } // if (c->tcolor(this->game()) == tcolor)
    } // for (c \in this->can_have_)
  } // if (this->can_have_(tcolor, this->game()) == this->must_have(tcolor)) 

  if (must_have_tcolor == this->can_have(tcolor)) {
    // all cards of tcolor are must have
    // this->can_have_ is changed in the loop!
    CardCounter const can_have = this->can_have_;
    for (CardCounter::const_iterator c = can_have.begin();
         c != can_have.end();
         ++c) {
      if (c->first.tcolor(this->game()) == tcolor) {
        if (this->can_have_.max_set(c->first, this->must_have(c->first))) {
          this->cards_information().queue_update(c->first);
        }
      } // if (c->tcolor(this->game()) == tcolor)
    } // for (c \in this->can_have_)
  } // if (this->can_have_(tcolor, this->game()) == this->must_have(tcolor)) 

  return ;
} // void CardsInformation::OfPlayer::check_can_is_must(Card::TColor tcolor)

/**
 ** check whether the cards the player has to have are the only ones
 ** he can have
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
CardsInformation::OfPlayer::check_must_is_can()
{
#ifdef CONDITION_INFORMATION_FLOW
  if (CONDITION_INFORMATION_FLOW)
    cout << "CardsInformation(" << this->game().no(this->cards_information().player()) << ")::OfPlayer(" << this->playerno() << ")::"
      << "check_must_is_can()\n";
#endif

  // the number of unknown cards of the player
  unsigned const unknown_cards_no
    = (this->player().cards_to_play() - this->must_have_.cards_no());

  if (unknown_cards_no >= this->game().rule()(Rule::NUMBER_OF_SAME_CARDS))
    return ;

  // Example:
  // We know now that there is only one unknown card.
  // So the player can only have one 'can have' more than 'must have'.

  vector<Card> update_cards;
  for (CardCounter::const_iterator c = this->can_have_.begin();
       c != this->can_have_.end();
       ++c) {
    unsigned const must_have = this->must_have(c->first);
    if (c->second > must_have + unknown_cards_no) {
      //if (this->can_have_.max_set(c->first, must_have))
      update_cards.push_back(c->first);
    }
  }

  // limit the can have
  for (vector<Card>::const_iterator c = update_cards.begin();
       c != update_cards.end();
       ++c) {
    if (this->can_have_.max_set(*c, this->must_have(*c) + unknown_cards_no))
      this->cards_information().queue_update(*c);
  } // for (c \in update_cards)

  // special case: poverty
  if (   (this->game().type() == GAMETYPE::POVERTY)
      && (this->playerno() == this->game().no(this->game().soloplayer())) ) {
    // check whether all trumps of the player are known
    if (   !this->does_not_have(Card::TRUMP)
        && (this->can_have(Card::TRUMP) > this->must_have(Card::TRUMP))
        && (this->played(Card::TRUMP) + this->must_have(Card::TRUMP)
            == this->game().poverty_trumpno_returned())) {
      // search the trumps
      vector<Card> update_cards;
      for (CardCounter::const_iterator c = this->can_have_.begin();
           c != this->can_have_.end();
           ++c) {
        if (   (c->first.istrump(this->game()))
            && (c->second > this->must_have(c->first)) )
          update_cards.push_back(c->first);
      } // for (c \in this->can_have_)
      // limit the can have
      for (vector<Card>::const_iterator c = update_cards.begin();
           c != update_cards.end();
           ++c) {
        if (this->can_have_.max_set(*c, this->must_have(*c)))
          this->cards_information().queue_update(*c);
      } // for (c \in update_cards)
    } // if (all trumps known)

  } // if (poverty player)


  return ;
} // void CardsInformation::OfPlayer::check_must_is_can()

/**
 ** -> result
 **
 ** @param	of_player_a	first object
 ** @param	of_player_b	second object
 **
 ** @return	whether the two objects are equal
 **		(the correspondign cards information may differ)
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.9
 **/
bool
operator==(CardsInformation::OfPlayer const& of_player_a,
           CardsInformation::OfPlayer const& of_player_b)
{
  return ( (of_player_a.playerno_
            == of_player_b.playerno_)
          && (of_player_a.played_
              == of_player_b.played_)
          && (of_player_a.must_have_
              == of_player_b.must_have_)
          && (of_player_a.can_have_
              == of_player_b.can_have_)
          && (of_player_a.tcolor_played_
              == of_player_b.tcolor_played_)
          && (of_player_a.tcolor_must_have_
              == of_player_b.tcolor_must_have_)
          && (of_player_a.tcolor_can_have_
              == of_player_b.tcolor_can_have_)
         );
} // bool operator==(CardsInformation::OfPlayer of_player_a, CardsInformation::OfPlayer of_player_b)

/**
 ** -> result
 **
 ** @param	of_player_a	first object
 ** @param	of_player_b	second object
 **
 ** @return	whether the two objects are different
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.9
 **/
bool
operator!=(CardsInformation::OfPlayer const& of_player_a,
           CardsInformation::OfPlayer const& of_player_b)
{
  return !(of_player_a == of_player_b);
} // bool operator!=(CardsInformation::OfPlayer of_player_a, CardsInformation::OfPlayer of_player_b)

/**
 ** self check
 ** when an error is found, an ASSERTION is created
 **
 ** @param	-
 **
 ** @return	whether the self-check was successful (no error)
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.9
 **/
bool
CardsInformation::OfPlayer::self_check() const
{
  if (this->player().cards_to_play() == UINT_MAX)
    return true;

  { // cards
    // the player has to have at least as many cards as he has to play
    if ( !(this->can_have_.cards_no() >= this->player().cards_to_play())) {
      DEBUG_ASSERTION(false,
                      "CardsInformation"
                      << "(" << this->game().no(this->cards_information().player()) << ")"
                      << "::OfPlayer(" << this->playerno() << ")"
                      << "::self_check()\n"
                      "  playerno = " << this->playerno() << "\n"
                      "  can_have < cards to play:\n"
                      << "  " << this->can_have_.cards_no()
                      << " < " << this->player().cards_to_play());
      return false;
    }

    if (   !this->cards_information().is_virtual()
        && !this->game().isvirtual()) {
      { // the player has all cards of his hand in 'can_have'
        if (!this->can_have_.contains(this->player().hand().counted_cards())) {
#if 0
          cerr << '\n'
            << "  cards information:\n" << this->cards_information()
            << "  hand:\n" << this->player().hand() << '\n'
            ;
#endif
          DEBUG_ASSERTION(false,
                          "CardsInformation"
                          << "(" << this->game().no(this->cards_information().player()) << ")"
                          << "::OfPlayer(" << this->playerno() << ")"
                          << "::self_check()\n"
                          "  the cards of the hand are not contained in the can have cards.\n");
          return false;
        }
      } // the player has all cards of his hand in 'can_have'
      { // the player has all 'must_have' cards in his hand
        if (!this->must_have_.is_contained(this->player().hand().counted_cards())) {
          DEBUG_ASSERTION(false,
                          "CardsInformation"
                          << "(" << this->game().no(this->cards_information().player()) << ")"
                          << "::OfPlayer(" << this->playerno() << ")"
                          << "::self_check()\n"
                          "  the must have cards are not contained in the cards of the hand.\n");
          return false;
        }
      } // the player has all 'must_have' cards in his hand
    } // if (!this->cards_information().is_virtual() && !this->game().isvirtual())


    // the player has to be able to play all 'must' cards
    if ( !(this->must_have_.cards_no() <= this->player().cards_to_play())) {
#ifdef DEBUG_ASSERT
      //cerr << this->cards_information() << endl;
#endif
      DEBUG_ASSERTION(false,
                      "CardsInformation"
                      << "(" << this->game().no(this->cards_information().player()) << ")"
                      << "::OfPlayer(" << this->playerno() << ")"
                      << "::self_check()\n"
                      "  playerno = " << this->playerno() << "\n"
                      "  must_have > cards to play:\n"
                      << "  " << this->must_have_.cards_no()
                      << " > " << this->player().cards_to_play());
      return false;
    }

    // check 'can have == cards to play' ==> 'can have == must have'
    if ( !(   (this->can_have_.cards_no() != this->player().cards_to_play())
           || (this->can_have_.cards_no() == this->must_have_.cards_no()) ) ) {
      DEBUG_ASSERTION(false,
                      "CardsInformation"
                      << "(" << this->game().no(this->cards_information().player()) << ")"
                      << "::OfPlayer(" << this->playerno() << ")"
                      << "::self_check()\n"
                      "  playerno = " << this->playerno() << "\n"
                      "  cards_to_play == can_have != must_have:\n"
                      << "  " << this->player().cards_to_play()
                      << " == " << this->can_have_.cards_no()
                      << " != " << this->must_have_.cards_no()
                     );
      return false;
    }

    // for each card check: 
    for (vector<Card>::const_iterator
         c = this->game().rule().cards().begin();
         c != this->game().rule().cards().end();
         ++c) {
      unsigned played = this->cards_information().played(*c);
      { // played(player) <= played(all)
        if (played < this->played(*c)) {
          DEBUG_ASSERTION(false,
                          "CardsInformation"
                          << "(" << this->game().no(this->cards_information().player()) << ")"
                          << "::OfPlayer(" << this->playerno() << ")"
                          << "::self_check()\n"
                          "  playerno = " << this->playerno() << "\n"
                          "  card '" << *c << "' has been played "
                          << played << " times, but by the player "
                          << this->playerno() << " '" << this->played(*c)
                          << "' times");
          return false;
        }
      } // played(player) <= played(all)

      unsigned const can_have = this->can_have(*c);
      { // check 'can have < must have'
        if ( !(can_have >= this->must_have(*c))) {
#ifdef DEBUG_ASSSERT
          //cerr << this->cards_information();
#endif
          DEBUG_ASSERTION(false,
                          "CardsInformation"
                          << "(" << this->game().no(this->cards_information().player()) << ")"
                          << "::OfPlayer(" << this->playerno() << ")"
                          << "::self_check()\n"
                          "  playerno = " << this->playerno() << "\n"
                          "  card '" << *c << "': "
                          "can_have = " << can_have << " < "
                          "must_have = " << this->must_have_[*c]
                         );
          return false;
        }
      } // check 'can have < must have'

      { // check 'can have < cards to play'
        if ( !(can_have <= this->player().cards_to_play())) {
#ifdef DEBUG_ASSSERT
          //cerr << this->cards_information();
#endif
          DEBUG_ASSERTION(false,
                          "CardsInformation"
                          << "(" << this->game().no(this->cards_information().player()) << ")"
                          << "::OfPlayer(" << this->playerno() << ")"
                          << "::self_check()\n"
                          "  playerno = " << this->playerno() << "\n"
                          "  card '" << *c << "': "
                          "can_have = " << can_have << " > "
                          "cards to play = " << this->player().cards_to_play()
                         );
          return false;
        }
      } // check 'can have < cards to play'

      { // check 'can have + played + sum must have <= #Cards'
        unsigned sum = 0;
        for (unsigned p = 0; p < this->game().playerno(); ++p)
          if (p != this->playerno())
            sum += this->cards_information().of_player(p).must_have(*c);

        if (can_have + played + sum
            > this->game().rule()(Rule::NUMBER_OF_SAME_CARDS) ) {
#ifdef DEBUG_ASSERT
          for (unsigned p = 0; p < this->game().playerno(); ++p) {
            if (p != this->playerno())
              if (this->cards_information().of_player(p).must_have(*c) > 0)
                cerr << p << ": must have = " << this->cards_information().of_player(p).must_have(*c) << endl;
          }
#endif
          DEBUG_ASSERTION(false,
                          "CardsInformation"
                          << "(" << this->game().no(this->cards_information().player()) << ")"
                          << "::OfPlayer(" << this->playerno() << ")"
                          << "::self_check()\n"
                          "  playerno = " << this->playerno() << "\n"
                          "  card '" << *c << "':\n"
                          "  can_have + played + Sum must_have > NUMBER_OF_CARDS:\n"
                          "  "
                          << can_have
                          << " + " << played << " + " << sum << " > "
                          << this->game().rule()(Rule::NUMBER_OF_SAME_CARDS)
                          //<< "\ngame:\n{\n" << this->game() << "}\n"
                          //<< "\n" << this->cards_information()
                         );

          return false;
        }
      } // check 'can have + played + sum must have <= #Cards'
      { // check 'can have(tcolor) >= can have'
        if (can_have > this->can_have(c->tcolor(this->game()))) {
          DEBUG_ASSERTION(false,
                          "CardsInformation"
                          << "(" << this->game().no(this->cards_information().player()) << ")"
                          << "::OfPlayer(" << this->playerno() << ")"
                          << "::self_check()\n"
                          "  playerno = " << this->playerno() << "\n"
                          "  card '" << *c << "':\n"
                          "  can_have > can_have(tcolor):\n"
                          "  "
                          << can_have << " > "
                          << this->can_have(c->tcolor(this->game()))
                          //<< "\ngame:\n{\n" << this->game() << "}\n"
                          //<< "\n" << this->cards_information()
                         );

          return false;
        } // if (can_have < this->can_have(c->tcolor(this->game())))
      } // check 'can have(tcolor) >= can have'
    } // for (c \in cards)

    { // checked '#played intern <= #played by player'
      unsigned played_cardno = 0;
      for (map<Card, unsigned>::const_iterator p = this->played_.begin();
           p != this->played_.end();
           ++p)
        played_cardno += p->second;
      unsigned const played_cardno_by_player
        = (this->game().trickno() - this->player().cards_to_play());
      if (played_cardno > played_cardno_by_player) {
        DEBUG_ASSERTION(false,
                        "CardsInformation"
                        << "(" << this->game().no(this->cards_information().player()) << ")"
                        << "::OfPlayer(" << this->playerno() << ")"
                        << "::self_check()\n"
                        "  " << played_cardno << " cards are marked as played, "
                        "but the player has played "
                        << played_cardno_by_player
                        << "\n" << this->game());
        return false;
      }
    } // checked '#played intern <= #played by player'

    if (   !this->cards_information().is_virtual()
        && (&this->cards_information()
            == &this->cards_information().player().cards_information())
       ) {
      // check that all cards on the hand of the player are 'can_be'
      for (HandCards::const_iterator
           card = this->player().hand().cards().begin();
           card != this->player().hand().cards().end();
           ++card) {
        if (*card != Card::UNKNOWN)
          DEBUG_ASSERTION((this->can_have(*card)
                           >= this->player().hand().numberof(*card)),
                          "CardsInformation"
                          << "(" << this->game().no(this->cards_information().player()) << ")"
                          << "::OfPlayer(" << this->playerno() << ")"
                          << "::self_check()\n"
                          "  card '" << *card << "' is "
                          << this->player().hand().numberof(*card)
                          << " times on the hand, but 'can have' = "
                          << this->can_have(*card));
      } // for (card \in this->player().hand().cards())
    } // if (!is_virtual_game)

  } // cards

  { // tcolor

    // played in tcolor is as in cards
    if (!(this->tcolor_played_.cards_no() == this->played_.cards_no())) {
      DEBUG_ASSERTION(false,
                      "CardsInformation"
                      << "(" << this->game().no(this->cards_information().player()) << ")"
                      << "::OfPlayer(" << this->playerno() << ")"
                      << "::self_check()\n"
                      "  playerno = " << this->playerno() << "\n"
                      "  tcolor played != played (cards):\n"
                      << "  " << this->tcolor_played_.cards_no()
                      << " != " << this->played_.cards_no());
      return false;
    }

    // the player has to be able to play all 'must' cards
    if (!(this->tcolor_must_have_.cards_no()
          <= this->player().cards_to_play())) {
      DEBUG_ASSERTION(false,
                      "CardsInformation"
                      << "(" << this->game().no(this->cards_information().player()) << ")"
                      << "::OfPlayer(" << this->playerno() << ")"
                      << "::self_check()\n"
                      "  playerno = " << this->playerno() << "\n"
                      "  tcolor must_have > cards to play:\n"
                      << "  " << this->tcolor_must_have_.cards_no()
                      << " > " << this->player().cards_to_play());
      return false;
    }

    // the player has to have at least as many cards as he has to play
    if (!(this->tcolor_can_have_.cards_no()
          >= this->player().cards_to_play())) {
      DEBUG_ASSERTION(false,
                      "CardsInformation"
                      << "(" << this->game().no(this->cards_information().player()) << ")"
                      << "::OfPlayer(" << this->playerno() << ")"
                      << "::self_check()\n"
                      "  playerno = " << this->playerno() << "\n"
                      "  tcolor can_have < cards to play:\n"
                      << "  " << this->tcolor_can_have_.cards_no()
                      << " < " << this->player().cards_to_play());
      return false;
    }
    // the player has to be able to play all 'must' cards
    if (!(this->tcolor_must_have_.cards_no()
          <= this->player().cards_to_play())) {
      DEBUG_ASSERTION(false,
                      "CardsInformation"
                      << "(" << this->game().no(this->cards_information().player()) << ")"
                      << "::OfPlayer(" << this->playerno() << ")"
                      << "::self_check()\n"
                      "  playerno = " << this->playerno() << "\n"
                      "  tcolor must_have > cards to play:\n"
                      << "  " << this->tcolor_must_have_.cards_no()
                      << " > " << this->player().cards_to_play());
      return false;
    }

    // check 'can have == cards to play' ==> 'can have == must have'
    if ( !(   (this->tcolor_can_have_.cards_no()
               != this->player().cards_to_play())
           || (this->tcolor_can_have_.cards_no()
               == this->tcolor_must_have_.cards_no()) ) ) {
      DEBUG_ASSERTION(false,
                      "CardsInformation"
                      << "(" << this->game().no(this->cards_information().player()) << ")"
                      << "::OfPlayer(" << this->playerno() << ")"
                      << "::self_check()\n"
                      "  playerno = " << this->playerno() << "\n"
                      "  cards_to_play == tcolor can_have != tcolor must_have:\n"
                      << "  " << this->player().cards_to_play()
                      << " == " << this->tcolor_can_have_.cards_no()
                      << " != " << this->tcolor_must_have_.cards_no()
                     );
      return false;
    }

    // for each tcolor check: 
    for (unsigned c = 0; c < Card::NUMBER_OF_TCOLORS; ++c) {
      Card::TColor const tcolor = static_cast<Card::TColor>(c);
      unsigned const played = this->cards_information().played(tcolor);
      { // played(player) <= played(all)
        if (played < this->played(tcolor)) {
          DEBUG_ASSERTION(false,
                          "CardsInformation"
                          << "(" << this->game().no(this->cards_information().player()) << ")"
                          << "::OfPlayer(" << this->playerno() << ")"
                          << "::self_check()\n"
                          "  playerno = " << this->playerno() << "\n"
                          "  tcolor '" << tcolor << "' has been played "
                          << played << " times, but by the player "
                          << this->playerno() << " '" << this->played(tcolor)
                          << "' times");
          return false;
        }
      } // played(player) <= played(all)
      unsigned const can_have = this->can_have(tcolor);
      { // check 'can have < must have'
        if ( !(can_have >= this->must_have(tcolor))) {
          DEBUG_ASSERTION(false,
                          "CardsInformation"
                          << "(" << this->game().no(this->cards_information().player()) << ")"
                          << "::OfPlayer(" << this->playerno() << ")"
                          << "::self_check()\n"
                          "  playerno = " << this->playerno() << "\n"
                          "  tcolor '" << tcolor << "': "
                          "can_have = " << can_have << " < "
                          "must_have = " << this->must_have(tcolor)
                         );
          return false;
        }
      } // check 'can have < must have'

      { // check 'can have < cards to play'
        if ( !(can_have <= this->player().cards_to_play())) {
          DEBUG_ASSERTION(false,
                          "CardsInformation"
                          << "(" << this->game().no(this->cards_information().player()) << ")"
                          << "::OfPlayer(" << this->playerno() << ")"
                          << "::self_check()\n"
                          "  playerno = " << this->playerno() << "\n"
                          "  tcolor '" << tcolor << "': "
                          "can_have = " << can_have << " > "
                          "cards to play = " << this->player().cards_to_play()
                         );
          return false;
        }
      } // check 'can have < cards to play'

      { // check 'can have + played + sum must have <= #Cards'
        unsigned sum = 0;
        for (unsigned p = 0; p < this->game().playerno(); ++p)
          if (p != this->playerno())
            sum += this->cards_information().of_player(p).must_have(tcolor);

        if (can_have + played + sum > this->game().cards_no(tcolor) ) {
#ifdef DEBUG_ASSERT
          for (unsigned p = 0; p < this->game().playerno(); ++p) {
            if (p != this->playerno())
              if (this->cards_information().of_player(p).must_have(tcolor) > 0)
                cerr << p << ": tcolor must have = " << this->cards_information().of_player(p).must_have(tcolor) << endl;
          }
#endif
          DEBUG_ASSERTION(false,
                          "CardsInformation"
                          << "(" << this->game().no(this->cards_information().player()) << ")"
                          << "::OfPlayer(" << this->playerno() << ")"
                          << "::self_check()\n"
                          "  playerno = " << this->playerno() << "\n"
                          "  tcolor '" << tcolor << "':\n"
                          "  can_have + played + sum must_have > possible number:\n"
                          "  "
                          << can_have
                          << " + " << played << " + " << sum << " > "
                          << this->game().cards_no(tcolor)
                          //<< "\ngame:\n{\n" << this->game() << "}\n"
                          << "\n" << *this
                          //<< "\n" << this->cards_information()
                         );

          return false;
        }
      } // check 'can have + played + sum must have <= #Cards'
    } // for (c \in tcolors)

  } // tcolor

  return true;
} // bool CardsInformation::OfPlayer::self_check() const
