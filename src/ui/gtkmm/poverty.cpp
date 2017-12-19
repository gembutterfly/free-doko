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

#include "constants.h"

#ifdef USE_UI_GTKMM

#include "poverty.h"

#include "table.h"
#include "hand.h"
#include "icongroup.h"
#include "ui.h"
#include "cards.h"

#include "../../game/game.h"
#include "../../party/rule.h"
#include "../../player/player.h"
#include "../../card/hand.h"
#include "../../os/bug_report_replay.h"

#include <gtkmm/main.h>
#include <gdkmm/general.h>

namespace UI_GTKMM_NS {
/**
 ** constructor
 **
 ** @param     table   parent object
 ** @param     game    corresponding game
 **
 ** @return    -
 **/
Poverty::Poverty(Table* const table, Game& game) :
  Base(table),
  table(table),
  game(&game)
{ }

/**
 ** Destructor
 **
 ** @param     -
 **
 ** @return    -
 **/
Poverty::~Poverty()
{ }

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    whether the active player can shift
 **/
bool
Poverty::middle_full() const
{
  for (auto const& c : this->cards)
    if (c == Card())
      return false;

  return true;
} // bool Poverty::middle_full() const

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    whether the active player can shift
 **/
bool
Poverty::shifting_valid() const
{
  if (this->player->type() != Player::HUMAN)
    return false;

  switch (this->status) {
  case Status::INVALID:
    return false;
  case Status::ASKING:
    return true;
  case Status::SHIFTING:
  case Status::SHIFTING_BACK:
    return this->middle_full();
  case Status::GETTING_BACK:
    return false;
  case Status::ACCEPTED:
  case Status::DENIED_BY_ALL:
  case Status::FINISHED:
    return false;
  } // switch (this->status)

  return false;
} // bool Poverty::shifting_valid() const

/**
 ** 'player' shifts 'cardno' cards
 **
 ** @param     player   the player who shifts the cards
 ** @param     cardno   the number of cards that are shifted
 **
 ** @return    -
 **/
void
Poverty::shift(Player const& player, unsigned const cardno)
{ 
  this->status = Status::SHIFTING;

  if (   (player.type() != Player::HUMAN)
      //&& (player.game().humanno() > 0)
      && !(FAST_PLAY & FAST_NS::PAUSE)
      && !(::bug_report_replay && ::bug_report_replay->auto_action()) ) {
    this->shift_cards = false;
    this->cards = player.game().poverty_cards();
    this->player = &player;

    this->table->draw_all();
    while (!this->ui->thrower
           && (::game_status == GAMESTATUS::GAME_POVERTY_SHIFT)
           && (this->shift_cards == false))
      ::ui->wait();
    if (this->ui->thrower)
      return ;
  } // if (player.type() != Player::HUMAN)

  this->status = Status::ASKING;

  return ;
} // void Poverty::shift(Player player, unsigned cardno)

/**
 ** 'player' denied to take the shifted cards
 **
 ** @param     player   the player who has denied to take the cards
 **
 ** @return    -
 **/
void
Poverty::take_denied(Player const& player)
{ 
  return ;
} // void Poverty::take_denied(Player player)

/**
 ** all players have denied to take the cards
 **
 ** @param     -
 **
 ** @return    -
 **/
void
Poverty::take_denied_by_all()
{ 
  this->player = &(this->player->game().soloplayer());
  this->status = Status::DENIED_BY_ALL;

  return ;
} // void Poverty::take_denied_by_all()

/**
 ** 'player' accepts to take the shifted cards
 ** and returns 'cardno' cards with 'trumpno' trumps
 ** show the cards and wait for a click
 **
 ** @param     player   the player who has denied to take the cards
 ** @param     cardno   number of cards that are given back
 ** @param     trumpno   number of trumps of the cards
 **
 ** @return    -
 **/
void
Poverty::take_accepted(Player const& player,
                       unsigned const cardno, unsigned const trumpno)
{ 
  this->status = Status::ACCEPTED;
  return ;
} // void Poverty::take_accepted(Player player, unsigned cardno, unsigned trumpno)

/**
 ** returns which cards the player shifts
 **
 ** @param     player   the player who shifts the cards (the soloplayer)
 **
 ** @return    the cards that are to be shifted
 **/
HandCards
Poverty::shift(Player& player)
{ 
  this->status = Status::SHIFTING;

  this->cards = HandCards(player.hand().numberofpovertycards());
  this->player = &player;

  this->shift_cards = false;

  // put all trump in the middle
  unsigned j = 0;
  for (unsigned i = 0; i < player.sorted_hand().cardsnumber_all(); i++)
    if (player.sorted_hand().card_all(i).istrump()) {
      this->cards[j] = player.sorted_hand().card_all(i);
      player.sorted_hand().playcard(i - j);
      j += 1;
    }

#ifdef WORKAROUND
  // The names of the poverty player and the last player of the reservation have to be redrawn.
  this->table->draw_all();
#else
  this->draw();
  this->table->hand(player).draw();
  this->table->icongroup(player).draw();
#endif

  while (!this->ui->thrower
         && (::game_status == GAMESTATUS::GAME_POVERTY_SHIFT)
         && (this->shift_cards == false))
    ::ui->wait();
  if (this->ui->thrower)
    return HandCards();

  if (::game_status != GAMESTATUS::GAME_POVERTY_SHIFT)
    return this->cards;

  // remove the cards to shift from the hand of the player
  player.sorted_hand().remove(this->cards);

  return this->cards;
} // HandCards Poverty::shift(Player& player)

/**
 ** ask 'player' whether to accept the poverty
 **
 ** @param     player   the player who is asked
 ** @param     cardno   the number of shifted cards
 **
 ** @return    -
 **/
void
Poverty::ask(Player const& player, unsigned const cardno)
{ 
  //this->cards = HandCards(cardno);
  this->cards = player.game().poverty_cards();
  this->player = &player;

  this->table->draw_all();

  return ;
} // void Poverty::ask(Player player, unsigned cardno)

/**
 ** returns whether 'player' accepts the shifted cards
 **
 ** @param     player   the player who shifts the cards
 ** @param     cardno   the number of shifted cards
 **
 ** @return    whether to accept the cards
 **/
bool
Poverty::take_accept(Player& player, unsigned const cardno)
{ 
  this->status = Status::ASKING;

  //this->cards = HandCards(cardno);
  this->cards = player.game().poverty_cards();
  this->player = &player;

  this->accept_cards = false;
  this->shift_cards = false;

  this->draw();
  this->table->hand(player).draw();

  while (!this->ui->thrower
         && (::game_status == GAMESTATUS::GAME_POVERTY_SHIFT)
         && (this->accept_cards == false)
         && (this->shift_cards == false))
    ::ui->wait();
  if (this->ui->thrower)
    return false;

  return this->accept_cards;
} // bool Poverty::take_accept(Player& player, unsigned cardno)

/**
 ** changes the cards from the poverty-player
 **
 ** @param     player          the player who has accepted the cards
 ** @param     cards_shifted   the cards that are given to the player
 **
 ** @return    the cards that are returned to the poverty-player
 **/
HandCards
Poverty::cards_change(Player& player, HandCards const& cards_shifted)
{ 
  this->status = Status::SHIFTING_BACK;

  this->player = &player;

  this->accept_cards = false;
  this->shift_cards = false;

  // add the cards to the hand, but mark all cards in the middle as played
  player.sorted_hand().add(cards_shifted);
  player.hand_sort();
  for (auto const&  c : cards_shifted)
    player.hand().playcard({player.hand(), c});
  this->cards.clear();
  for (unsigned c = 0; c < player.sorted_hand().cardsnumber_all(); ++c)
    if (player.sorted_hand().played(c))
      this->cards.push_back(player.sorted_hand().card_all(c));

  this->draw();
  this->table->hand(player).draw();

  while (!this->ui->thrower
         && (::game_status == GAMESTATUS::GAME_POVERTY_SHIFT)
         && (this->shift_cards == false))
    ::ui->wait();
  if (this->ui->thrower)
    return HandCards();

  // remove the cards to shift from the hand of the player
  player.sorted_hand().remove(this->cards);

  return this->cards;
} // HandCards Poverty::cards_change(Player& player, HandCards cards_shifted)

/**
 ** changes the cards from the poverty-player
 **
 ** @param     player           the player who has accepted the cards
 ** @param     cards_returned   the cards that are given to the player
 **
 ** @return    the cards that are returned to the poverty-player
 **/
void
Poverty::cards_get_back(Player& player,
                        HandCards const& cards_returned)
{ 
  if (player.type() != Player::HUMAN)
    return ;

  this->status = Status::GETTING_BACK;

  this->player = &player;

  this->accept_cards = false;

  // add the cards to the hand, but mark all cards in the middle as played
  player.sorted_hand().add(cards_returned);
  player.hand_sort();
  for (auto const& c : cards_returned)
    player.hand().playcard({player.hand(), c});
  this->cards.clear();
  for (unsigned c = 0; c < player.sorted_hand().cardsnumber_all(); ++c)
    if (player.sorted_hand().played(c))
      this->cards.push_back(player.sorted_hand().card_all(c));

  this->table->draw_all();

  while (!this->ui->thrower
         && (::game_status == GAMESTATUS::GAME_POVERTY_SHIFT)
         && (this->accept_cards == false))
    ::ui->wait();
  if (this->ui->thrower)
    return ;

  for (auto const& c : this->cards)
    player.sorted_hand().unplaycard(c);

  this->status = Status::FINISHED;

  return ;
} // void Poverty::cards_get_back(Player& player, HandCards cards_returned)

/**
 ** -> result
 **
 ** @param     cardno   number of card
 **
 ** @return    position of the card
 **/
Gdk::Point
Poverty::card_pos(unsigned const cardno) const
{
  DEBUG_ASSERTION((cardno <= 5),
                  "Poverty::card_pos(player, cardno):\n"
                  "  `cardno` not valid: " << cardno);

  Gdk::Point pos;
  Gdk::Rectangle const arrow_outline = this->arrow_outline();
  Player const& player = *this->player;

  switch (this->table->position(player)) {
  case Position::SOUTH: // player position
    switch (this->cards.size()) {
    case 0: // this->cards.size()
      pos.set_x(this->table->hand(player).start_x()
                + this->table->hand(player).width() / 2
                - this->ui->cards->height() / 2);
      pos.set_y(arrow_outline.get_y() - arrow_outline.get_height() / 2
                - this->ui->cards->width());
      break;
    case 1: // this->cards.size()
      switch(cardno) {
      case 0:
        pos.set_x(this->table->hand(player).start_x()
                  + this->table->hand(player).width() / 2
                  - this->ui->cards->width() / 2);
        pos.set_y(arrow_outline.get_y() - arrow_outline.get_height() / 2
                  - this->ui->cards->height());
        break;
      default:
        break;
      } // switch(cardno)
      break;

    case 2: // this->cards.size()
      switch(cardno) {
      case 0:
        pos.set_x(this->table->hand(player).start_x()
                  + this->table->hand(player).width() / 4);
        pos.set_y(arrow_outline.get_y() - arrow_outline.get_height() / 2
                  - this->ui->cards->height());
        break;
      case 1:
        pos.set_x(this->table->hand(player).start_x()
                  + this->table->hand(player).width()
                  - this->table->hand(player).width() / 4
                  - this->ui->cards->width());
        pos.set_y(arrow_outline.get_y() - arrow_outline.get_height() / 2
                  - this->ui->cards->height());
        break;
      default:
        break;
      } // switch(cardno)
      break;

    case 3: // this->cards.size()
      switch(cardno) {
      case 0:
        pos.set_x(this->table->hand(player).start_x()
                  + this->table->hand(player).width() / 8);
        pos.set_y(arrow_outline.get_y() - arrow_outline.get_height() / 2
                  - this->ui->cards->height());
        break;
      case 1:
        pos.set_x(this->table->hand(player).start_x()
                  + this->table->hand(player).width() / 2
                  - this->ui->cards->width() / 2);
        pos.set_y(arrow_outline.get_y() - arrow_outline.get_height() / 2
                  - this->ui->cards->height() / 3
                  - this->ui->cards->height());
        break;
      case 2:
        pos.set_x(this->table->hand(player).start_x()
                  + this->table->hand(player).width()
                  - this->table->hand(player).width() / 8
                  - this->ui->cards->width());
        pos.set_y(arrow_outline.get_y() - arrow_outline.get_height() / 2
                  - this->ui->cards->height());
        break;
      default:
        break;
      } // switch(cardno)
      break;

    case 4:
      pos.set_x(this->table->hand(player).start_x()
                + this->table->hand(player).width() * (1 + 2 * cardno) / 8
                - this->ui->cards->width() / 2);
      switch(cardno) {
      case 0:
      case 3:
        pos.set_y(arrow_outline.get_y() - arrow_outline.get_height() / 2
                  - this->ui->cards->height());
        break;
      case 1:
      case 2:
        pos.set_y(arrow_outline.get_y() - arrow_outline.get_height()
                  - this->ui->cards->height());
        break;
      default:
        break;
      } // switch(cardno)
      break;

    case 5:
      pos.set_x(this->table->hand(player).start_x()
                + (this->table->hand(player).width()
                   * (1 + 2 * cardno) / 10)
                - this->ui->cards->width() / 2);
      switch(cardno) {
      case 0:
      case 4:
        pos.set_y(arrow_outline.get_y() - arrow_outline.get_height() / 2
                  - this->ui->cards->height());
        break;
      case 1:
      case 3:
        pos.set_y(arrow_outline.get_y() - arrow_outline.get_height() / 2
                  - this->ui->cards->height()
                  - this->ui->cards->height() / 4);
        break;
      case 2:
        pos.set_y(arrow_outline.get_y() - arrow_outline.get_height() / 2
                  - this->ui->cards->height()
                  - this->ui->cards->height() / 3);
        break;
      default:
        break;
      } // switch(cardno)
      break;

    default: // this->cards.size()
      DEBUG_ASSERTION((this->cards.size() <= 5),
                      "UI_GTKMM::Poverty::card_pos(cardno)\n"
                      "  too many cards: " << this->cards.size() << " > 5 ");
      break;
    } // switch (this->cards.size())
    break;

  case Position::NORTH: // player.no()
    switch(this->cards.size()) {
    case 0: // this->cards.size()
      pos.set_x(this->table->hand(player).start_x()
                + this->table->hand(player).width() / 2
                - this->ui->cards->height() / 2);
      pos.set_y(arrow_outline.get_y()
                + arrow_outline.get_height() * 3 / 2);
      break;

    case 1: // this->cards.size()
      switch(cardno) {
      case 0:
        pos.set_x(this->table->hand(player).start_x()
                  + this->table->hand(player).width() / 2
                  - this->ui->cards->width() / 2);
        pos.set_y(arrow_outline.get_y()
                  + arrow_outline.get_height() * 3 / 2);
        break;
      default:
        break;
      } // switch(cardno)
      break;

    case 2: // this->cards.size()
      switch(cardno) {
      case 0:
        pos.set_x(this->table->hand(player).start_x()
                  + this->table->hand(player).width()
                  - this->table->hand(player).width() / 4
                  - this->ui->cards->width());
        pos.set_y(arrow_outline.get_y()
                  + arrow_outline.get_height() * 3 / 2);
        break;
      case 1:
        pos.set_x(this->table->hand(player).start_x()
                  + this->table->hand(player).width() / 4);
        pos.set_y(arrow_outline.get_y()
                  + arrow_outline.get_height() * 3 / 2);
        break;
      default:
        break;
      } // switch(cardno)
      break;

    case 3: // this->cards.size()
      switch(cardno) {
      case 0:
        pos.set_x(this->table->hand(player).start_x()
                  + this->table->hand(player).width()
                  - this->table->hand(player).width() / 8
                  - this->ui->cards->width());
        pos.set_y(arrow_outline.get_y()
                  + arrow_outline.get_height() * 3 / 2);
        break;
      case 1:
        pos.set_x(this->table->hand(player).start_x()
                  + this->table->hand(player).width() / 2
                  - this->ui->cards->width() / 2);
        pos.set_y(arrow_outline.get_y()
                  + arrow_outline.get_height() * 3 / 2
                  + this->ui->cards->height() / 3);
        break;
      case 2:
        pos.set_x(this->table->hand(player).start_x()
                  + this->table->hand(player).width() / 8);
        pos.set_y(arrow_outline.get_y()
                  + arrow_outline.get_height() * 3 / 2);
        break;
      default:
        break;
      } // switch(cardno)
      break;

    case 4: // this->cards.size()
      pos.set_x(this->table->hand(player).start_x()
                + (this->table->hand(player).width()
                   * (1 + 2 * (3 - cardno)) / 8)
                - this->ui->cards->width() / 2);
      switch(cardno) {
      case 0:
      case 3:
        pos.set_y(arrow_outline.get_y()
                  + arrow_outline.get_height() * 3 / 2);
        break;
      case 1:
      case 2:
        pos.set_y(arrow_outline.get_y()
                  + arrow_outline.get_height() * 4 / 2);
        break;
      default:
        break;
      } // switch(cardno)
      break;

    case 5: // this->cards.size()
      pos.set_x(this->table->hand(player).start_x()
                + (this->table->hand(player).width()
                   * (9 - 2 * cardno) / 10)
                - this->ui->cards->width() / 2);
      switch(cardno) {
      case 0:
      case 4:
        pos.set_y(arrow_outline.get_y()
                  + arrow_outline.get_height() * 3 / 2);
        break;
      case 1:
      case 3:
        pos.set_y(arrow_outline.get_y()
                  + arrow_outline.get_height() * 3 / 2
                  + this->ui->cards->height() / 4);
        break;
      case 2:
        pos.set_y(arrow_outline.get_y()
                  + arrow_outline.get_height() * 3 / 2
                  + this->ui->cards->height() / 3);
        break;
      default:
        break;
      } // switch(cardno)
      break;

    default: // this->cards.size()
      DEBUG_ASSERTION((this->cards.size() <= 5),
                      "UI_GTKMM::Poverty::card_pos(cardno)\n"
                      "  too many cards: " << this->cards.size() << " > 5 ");
      break;
    } // switch(this->cards.size())
    break;

  case Position::WEST: // player.no()
    switch(this->cards.size()) {
    case 0: // this->cards.size()
      pos.set_x(arrow_outline.get_x()
                + arrow_outline.get_width() * 3 / 2);
      pos.set_y(this->table->hand(player).start_y()
                + this->table->hand(player).height() / 2
                - this->ui->cards->height() / 2);
      break;

    case 1: // this->cards.size()
      switch(cardno) {
      case 0:
        pos.set_x(arrow_outline.get_x()
                  + arrow_outline.get_width() * 3 / 2);
        pos.set_y(this->table->hand(player).start_y()
                  + this->table->hand(player).height() / 2
                  - this->ui->cards->width() / 2);
        break;
      default:
        break;
      } // switch(cardno)
      break;

    case 2: // this->cards.size()
      switch(cardno) {
      case 0:
        pos.set_x(arrow_outline.get_x()
                  + arrow_outline.get_width() * 3 / 2);
        pos.set_y(this->table->hand(player).start_y()
                  + this->table->hand(player).height()
                  - this->table->hand(player).height() / 4
                  - this->ui->cards->width());
        break;
      case 1:
        pos.set_x(arrow_outline.get_x()
                  + arrow_outline.get_width() * 3 / 2);
        pos.set_y(this->table->hand(player).start_y()
                  + this->table->hand(player).height() / 4);
        break;
      default:
        break;
      } // switch(cardno)
      break;

    case 3: // this->cards.size()
      switch(cardno) {
      case 0:
        pos.set_x(arrow_outline.get_x()
                  + arrow_outline.get_width() * 3 / 2);
        pos.set_y(this->table->hand(player).start_y()
                  + this->table->hand(player).height()
                  - this->table->hand(player).height() / 8
                  - this->ui->cards->width());
        break;
      case 1:
        pos.set_x(arrow_outline.get_x() + arrow_outline.get_width() * 3 / 2
                  + this->ui->cards->height() / 3);
        pos.set_y(this->table->hand(player).start_y()
                  + this->table->hand(player).height() / 2
                  - this->ui->cards->width() / 2);
        break;
      case 2:
        pos.set_x(arrow_outline.get_x()
                  + arrow_outline.get_width() * 3 / 2);
        pos.set_y(this->table->hand(player).start_y()
                  + this->table->hand(player).height() / 8);
        break;
      default:
        break;
      } // switch(cardno)
      break;

    case 4: // this->cards.size()
      pos.set_y(this->table->hand(player).start_y()
                + (this->table->hand(player).height()
                   * (1 + 2 * cardno) / 8)
                - this->ui->cards->width() / 2);
      switch(cardno) {
      case 0:
      case 3:
        pos.set_x(arrow_outline.get_x()
                  + arrow_outline.get_width() * 3 / 2);
        break;
      case 1:
      case 2:
        pos.set_x(arrow_outline.get_x()
                  + arrow_outline.get_width() * 4 / 2);
        break;
      default:
        break;
      } // switch(cardno)
      break;

    case 5: // this->cards.size()
      pos.set_y(this->table->hand(player).start_y()
                + (this->table->hand(player).height()
                   * (1 + 2 * cardno) / 10)
                - this->ui->cards->width() / 2);
      switch(cardno) {
      case 0:
      case 4:
        pos.set_x(arrow_outline.get_x()
                  + arrow_outline.get_width() * 3 / 2);
        break;
      case 1:
      case 3:
        pos.set_x(arrow_outline.get_x()
                  + arrow_outline.get_width() * 3 / 2
                  + this->ui->cards->height() / 4);
        break;
      case 2:
        pos.set_x(arrow_outline.get_x()
                  + arrow_outline.get_width() * 3 / 2
                  + this->ui->cards->height() / 3);
        break;
      default:
        break;
      } // switch(cardno)
      break;

    default: // this->cards.size()
      DEBUG_ASSERTION((this->cards.size() <= 5),
                      "UI_GTKMM::Poverty::card_pos(cardno)\n"
                      "  too many cards: " << this->cards.size() << " > 5 ");
      break;
    } // switch(this->cards.size())
    break;

  case Position::EAST: // player.no()
    switch(this->cards.size()) {
    case 0: // this->cards.size()
      pos.set_x(arrow_outline.get_x() - arrow_outline.get_width() / 2
                - this->ui->cards->width());
      pos.set_y(this->table->hand(player).start_y()
                + this->table->hand(player).height() / 2
                - this->ui->cards->height() / 2);
      break;

    case 1: // this->cards.size()
      switch(cardno) {
      case 0:
        pos.set_x(arrow_outline.get_x() - arrow_outline.get_width() / 2
                  - this->ui->cards->height());
        pos.set_y(this->table->hand(player).start_y()
                  + this->table->hand(player).height() / 2
                  - this->ui->cards->width() / 2);
        break;
      default:
        break;
      } // switch(cardno)
      break;

    case 2: // this->cards.size()
      switch(cardno) {
      case 0:
        pos.set_x(arrow_outline.get_x() - arrow_outline.get_width() / 2
                  - this->ui->cards->height());
        pos.set_y(this->table->hand(player).start_y()
                  + this->table->hand(player).height()
                  - this->table->hand(player).height() / 4
                  - this->ui->cards->width());
        break;
      case 1:
        pos.set_x(arrow_outline.get_x() - arrow_outline.get_width() / 2
                  - this->ui->cards->height());
        pos.set_y(this->table->hand(player).start_y()
                  + this->table->hand(player).height() / 4);
        break;
      default:
        break;
      } // switch(cardno)
      break;

    case 3: // this->cards.size()
      switch(cardno) {
      case 0:
        pos.set_x(arrow_outline.get_x() - arrow_outline.get_width() / 2
                  - this->ui->cards->height());
        pos.set_y(this->table->hand(player).start_y()
                  + this->table->hand(player).height() / 8);
        break;
      case 1:
        pos.set_x(arrow_outline.get_x() - arrow_outline.get_width() / 2
                  - this->ui->cards->height()
                  - this->ui->cards->height() / 3);
        pos.set_y(this->table->hand(player).start_y()
                  + this->table->hand(player).height() / 2
                  - this->ui->cards->width() / 2);
        break;
      case 2:
        pos.set_x(arrow_outline.get_x() - arrow_outline.get_width() / 2
                  - this->ui->cards->height());
        pos.set_y(this->table->hand(player).start_y()
                  + this->table->hand(player).height()
                  - this->table->hand(player).height() / 8
                  - this->ui->cards->width());
        break;
      default:
        break;
      } // switch(cardno)
      break;

    case 4: // this->cards.size()
      pos.set_y(this->table->hand(player).start_y()
                + ( this->table->hand(player).height()
                   * (7 - 2 * cardno) / 8)
                - this->ui->cards->width() / 2);
      switch(cardno) {
      case 0:
      case 3:
        pos.set_x(arrow_outline.get_x()
                  - arrow_outline.get_width() / 2
                  - this->ui->cards->height());
        break;
      case 1:
      case 2:
        pos.set_x(arrow_outline.get_x()
                  - arrow_outline.get_width() * 2 / 2
                  - this->ui->cards->height());
        break;
      default:
        break;
      } // switch(cardno)
      break;

    case 5: // this->cards.size()
      pos.set_y(this->table->hand(player).start_y()
                + (this->table->hand(player).height()
                   * (9 - 2 * cardno) / 10)
                - this->ui->cards->width() / 2);
      switch(cardno) {
      case 0:
      case 4:
        pos.set_x(arrow_outline.get_x() - arrow_outline.get_width() / 2
                  - this->ui->cards->height());
        break;
      case 1:
      case 3:
        pos.set_x(arrow_outline.get_x() - arrow_outline.get_width() / 2
                  - this->ui->cards->height()
                  - this->ui->cards->height() / 4);
        break;
      case 2:
        pos.set_x(arrow_outline.get_x() - arrow_outline.get_width() / 2
                  - this->ui->cards->height()
                  - this->ui->cards->height() / 3);
        break;
      default:
        break;
      } // switch(cardno)
      break;

    default: // this->cards.size()
      DEBUG_ASSERTION((this->cards.size() <= 5),
                      "UI_GTKMM::Poverty::card_pos(cardno)\n"
                      "  too many cards: " << this->cards.size() << " > 5 ");
      break;
    } // switch(this->cards.size())
    break;

  case Position::CENTER:
    DEBUG_ASSERTION(false,
                    "Poverty::card_pos(unsigned cardno)\n"
                    "  wrong position 'CENTER' "
                    << "(" << this->table->position(player) << ")");
  } // switch (this->table->position(player))

  return pos;
} // Gdk::Pos Poverty::card_pos(unsigned cardno) const

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    outline rectangle of the arrow
 **/
Gdk::Rectangle
Poverty::arrow_outline() const
{
  Gdk::Rectangle rectangle;
  Player const& player = *(this->player);

  switch (this->table->position(player)) {
  case SOUTH:
    rectangle.set_width(this->table->hand(player).width() / 2);
    rectangle.set_height(this->ui->cards->height() / 5);
    rectangle.set_x(this->table->hand(player).start_x()
                    + this->table->hand(player).width() / 4);
    rectangle.set_y(this->table->hand(player).start_y()
                    - this->table->icongroup(player).margin_y()
                    - this->table->icongroup(player).height()
                    - rectangle.get_height()
                    - rectangle.get_height() / 2);
    break;
  case NORTH:
    rectangle.set_width(this->table->hand(player).width() / 2);
    rectangle.set_height(this->ui->cards->height() / 5);
    rectangle.set_x(this->table->hand(player).start_x()
                    + this->table->hand(player).width() / 4);
    rectangle.set_y(this->table->hand(player).start_y()
                    + this->table->hand(player).height()
                    + this->table->icongroup(player).margin_y()
                    + this->table->icongroup(player).height()
                    + rectangle.get_height() / 2);
    break;
  case WEST:
    rectangle.set_width(this->ui->cards->height() / 5);
    rectangle.set_height(this->table->hand(player).height() / 2);
    rectangle.set_x(this->table->hand(player).start_x()
                    + this->table->hand(player).width()
                    + this->table->icongroup(player).margin_x()
                    + this->table->icongroup(player).width()
                    + rectangle.get_width() / 2);
    rectangle.set_y(this->table->hand(player).start_y()
                    + this->table->hand(player).height() / 4);
    break;
  case EAST:
    rectangle.set_width(this->ui->cards->height() / 5);
    rectangle.set_height(this->table->hand(player).height() / 2);
    rectangle.set_x(this->table->hand(player).start_x()
                    - this->table->icongroup(player).margin_x()
                    - this->table->icongroup(player).width()
                    - rectangle.get_width()
                    - rectangle.get_width() / 2);
    rectangle.set_y(this->table->hand(player).start_y()
                    + this->table->hand(player).height() / 4);
    break;

  case CENTER:
    DEBUG_ASSERTION(false,
                    "Poverty::arrow_outline()\n"
                    "  wrong position 'CENTER' "
                    << "(" << this->table->position(player) << ")");
  } // switch (this->table->position(player))

  return rectangle;
} // Gdk::Rectangle Poverty::arrow_outline() const

/**
 ** draws the poverty elements in the table
 **
 ** @param     -
 **
 ** @return    -
 **/
void
Poverty::draw()
{
  this->draw(this->table->create_cairo_context(), true);
} // void Poverty::draw()

/**
 ** draws the poverty elements in the table
 **
 ** @param     cr       cairo context
 ** @param     update   whether the window should be updated (default: true)
 **
 ** @return    -
 **/
void
Poverty::draw(Cairo::RefPtr<::Cairo::Context> cr, bool const update)
{
  if (this->player == nullptr)
    return ;

  if (   (this->status == Status::INVALID)
      || (this->status == Status::ACCEPTED)
      || (this->status == Status::FINISHED))
    return ;

#ifdef WINDOWS
#ifdef WORKAROUND
  // Der aktuelle Fensterinhalt wird nicht uebertragen
  if (update) {
	  this->table->queue_resize();
    return ;
  }
#endif
#endif

  Player const& player = *this->player;

  cr->push_group();
  { // draw the arrow
    cr->save();
    if (player.type() == Player::HUMAN) {
      Gdk::Rectangle outline = this->arrow_outline();

      if (this->shifting_valid()) {

        bool const mirror = (this->status == Status::SHIFTING_BACK);

        switch(this->table->position(player)) {
        case SOUTH: {
          cr->translate(outline.get_x(),
                        outline.get_y() + outline.get_height() / 4); // Pfeilspitze
          cr->scale(outline.get_width(), outline.get_height() / 2);
          break;
        }
        case NORTH: {
          cr->translate(outline.get_x() + outline.get_width(),
                        outline.get_y() + outline.get_height() / 4); // Pfeilspitze
          cr->scale(-outline.get_width(), outline.get_height() / 2);
          break;
        }
        case WEST: {
          cr->translate(outline.get_x() + outline.get_width() / 4,
                        outline.get_y()); // Pfeilspitze
          cr->scale(-outline.get_width() / 2, outline.get_height());
          cr->transform(Cairo::Matrix(0, 1, 1, 0, 0, 0));
          break;
        }
        case EAST: {
          cr->translate(outline.get_x() + outline.get_width() / 4,
                        outline.get_y() + outline.get_height()); // Pfeilspitze
          cr->scale(outline.get_width() / 2, -outline.get_height());
          cr->transform(Cairo::Matrix(0, 1, 1, 0, 0, 0));
          break;
        }
        default:
          DEBUG_ASSERTION(false,
                          "Poverty::draw():\n"
                          "  `player.no()` not valid (not 0, 1, 2, 3)");
          break;
        } // switch(player->no())
        if (mirror) {
          cr->translate(1, 0);
          cr->scale(-1, 1);
        }
        cr->move_to(0, 0);
        cr->rel_line_to(1.0 / 6, -1.25); // Obere Ecke
        cr->rel_line_to(0, 0.75);
        cr->rel_line_to(5.0 / 6, 0);
        cr->rel_line_to(0, 1);
        cr->rel_line_to(-5.0 / 6, 0);
        cr->rel_line_to(0, 0.75);
        cr->close_path();
        cr->fill();

      } // if (this->shifting_valid())
    } // if (player.type() == Player::HUMAN)
    cr->restore();
  } // draw the arrow

  { // draw the cards
    auto const cardno = this->cards.size();

    if (cardno == 0) {
      auto const pos = this->card_pos(0);
      auto card = this->ui->cards->back(this->table->rotation(player));
      Gdk::Cairo::set_source_pixbuf(cr, card,
                                    pos.get_x(), pos.get_y());
      cr->paint();
    } else { // if !(cardno == 0)
      for (unsigned i = 0; i < cardno; i++) {
        auto const pos = this->card_pos(i);
        auto card = this->ui->cards->card(Card(), this->table->rotation(player));
        if (   (   (this->status != Status::ASKING)
                && !(   (this->status == Status::SHIFTING)
                     && (this->player->type() != Player::HUMAN)) )
            || (   this->game->rule()(Rule::SHOW_ALL_HANDS)
                && ::setting(Setting::SHOW_ALL_HANDS))
            || (   this->game->rule()(Rule::POVERTY_FOX_SHIFT_OPEN)
#ifdef WORKAROUND
                // Bug:
                // 'isfox' needs a corresponding hand,
                // but 'cards' does not belong anymore to the hand of 
                // the poverty player.
                // The only problem exists, if the card is a swine, but
                // then the swines are already announced
                && this->cards[i].istrumpace()
                && !this->ui->game().swines_announced()
#else
                && this->cards[i].isfox()
#endif
               ) )
          card = this->ui->cards->card(this->cards[i],
                                       this->table->rotation(player));

        Gdk::Cairo::set_source_pixbuf(cr, card,
                                      pos.get_x(), pos.get_y());
        cr->paint();
      } // for (i < cardno)
    } // if !(cardno == 0)
  } // draw the cards
  cr->pop_group_to_source();
  cr->paint();

  return ;
} // void Poverty::draw(Cairo::RefPtr<::Cairo::Context> cr, bool update = true)

/**
 ** if the arrow is clicked, shift the cards
 ** if a card in the hand is clicked, move it in the cards-to-be-shifted
 ** if a card in the middle is clicked, move it in the hand
 ** no release: if the right button is clicked on an empty card, add cards
 **             according to the bug report
 **
 ** @param     button_event   data of the button event
 **
 ** @return    whether the click has been accepted
 **/
bool
Poverty::button_press_event(GdkEventButton* const button_event)
{
  // whether the mouse click leads to an action
  bool accepted = false;

  switch (this->possible_action(static_cast<int>(button_event->x),
                                static_cast<int>(button_event->y))) {
  case Action::NONE:
    accepted = false;
    break;
  case Action::SHIFT_CARDS:
    switch (button_event->button) {
    case 1: // left mouse button
      this->shift_cards = true;
      accepted = true;
      break;
    default:
      break;
    } // switch (button_event->button)
    break;
  case Action::ACCEPT_CARDS:
    switch (button_event->button) {
    case 1: // left mouse button
      this->accept_cards = true;
      accepted = true;
      break;
    default:
      break;
    } // switch (button_event->button)
    break;
  case Action::TAKE_CARD:
    {
      Player& player = const_cast<Player&>(*(this->player));

      // test whether a card in the middle is clicked
      unsigned const cardno
        = this->cardno_at_position(static_cast<int>(button_event->x), static_cast<int>(button_event->y));
      DEBUG_ASSERTION((cardno != UINT_MAX),
                      "Poverty::button_press_event():\n"
                      "  no card under the mouse");

      // move a card in the hand
      switch (button_event->button) {
      case 1: // left mouse button
        if (this->cards[cardno].is_empty())
          break;
        player.sorted_hand().unplaycard(this->cards[cardno]);
        this->cards[cardno] = Card();
        accepted = true;
        break;
      case 3: // right mouse button

        // put all cards in the hand
        for (auto& c : this->cards) {
          if (!c.is_empty()) {
            if (   (this->status != Status::SHIFTING)
                || (!c.istrump())) {
              player.sorted_hand().unplaycard(c);
              c = Card();
            } // if (card not trump when soloplayer)
          }
        }
        accepted = true;
      default:
        break;
      } // switch (button_event->button)
      this->draw();
      this->table->hand(player).draw();
    }
    break;

  case Action::PUT_CARD:
    switch (button_event->button) {
    case 1: // left mouse button
      {
        unsigned const cardno
          = this->table->hand(*this->player).cardno_at_position(static_cast<int>(button_event->x), static_cast<int>(button_event->y));
        DEBUG_ASSERTION((cardno != UINT_MAX),
                        "Poverty::button_press_event():\n"
                        "  no card under the mouse");
        accepted = true;
        this->add_card_to_shift(cardno);
      }
      break;
    default:
      break;
    } // switch (button_event->button)
    break;

  case Action::FILL_UP:
    // for a bug report: fill up according to the bug report
    switch (button_event->button) {
    case 3: { // right mouse button
      Player& player = const_cast<Player&>(*(this->player));

      // test whether a card in the middle is clicked
      unsigned const cardno
        = this->cardno_at_position(static_cast<int>(button_event->x), static_cast<int>(button_event->y));

      if (!(   ::bug_report_replay
            && this->cards[cardno].is_empty()) )
        break;

      accepted = true;
      // fill up according to the bug report
      for (auto& c : this->cards) {
        if (!c.is_empty()) {
          player.sorted_hand().unplaycard(c);
          c = Card();
        }
      }

      switch (status) {
      case Status::SHIFTING:
        DEBUG_ASSERTION(::bug_report_replay->poverty_cards_shifted(),
                        "UI_GTKMM:Poverty::on_mouse_click_event()\n"
                        "  the bug report does not contain shifted cards");
        this->cards.clear();
        for (auto const& c: *::bug_report_replay->poverty_cards_shifted())
          this->cards.push_back({player.hand(), c});
        break;

      case Status::SHIFTING_BACK:
        DEBUG_ASSERTION(::bug_report_replay->poverty_cards_returned(),
                        "UI_GTKMM:Poverty::on_mouse_click_event()\n"
                        "  the bug report does not contain returned cards");
        this->cards.clear();
        for (auto const& c : *::bug_report_replay->poverty_cards_returned())
          this->cards.push_back({player.hand(), c});
        break;
      default:
        break;
      } // switch(status)

      for (auto const& c : this->cards)
        player.sorted_hand().playcard(c);

      this->draw();
      this->table->hand(player).draw();

      break;
    }

    default:
      break;
    } // switch (button_event->button)
    break;

  case Action::RETURN_CARDS:
    switch (button_event->button) {
    case 1: // left mouse button
      this->shift_cards = true;
      accepted = true;
      break;
    default:
      break;
    } // switch (button_event->button)
    break;
  case Action::GET_CARDS_BACK:
    switch (button_event->button) {
    case 1: // left mouse button
      this->accept_cards = true;
      accepted = true;
      break;
    default:
      break;
    } // switch (button_event->button)
    break;
  }; // switch (this->possible_actions(button_event->x, button_event->y)) {

  return accepted;
} // bool Poverty::button_press_event(GdkEventButton* button_event)

/**
 ** -> result
 **
 ** @param     x   x position
 ** @param     y   y position
 **
 ** @return    what action a can be made at the position (for the mouse)
 **/
Poverty::Action
Poverty::possible_action(int const x, int const y) const
{
  if (   (this->status == Status::INVALID)
      || (this->status == Status::FINISHED))
    return Action::NONE;

  if (!this->player)
    return Action::NONE;

  Player const& player = *this->player;

  if (   (player.type() != Player::HUMAN)
      && (this->status == Status::SHIFTING))
    return Action::SHIFT_CARDS;

  if (player.type() != Player::HUMAN)
    return Action::NONE;

  if (this->shifting_valid()) {
    // test whether mouse is over the arrow
    Gdk::Rectangle outline = this->arrow_outline();
    if (   (x >= outline.get_x())
        && (x <= outline.get_x() + outline.get_width())
        && (y >= outline.get_y())
        && (y <= outline.get_y() + outline.get_height())
       ) {
      switch(this->status) {
      case Status::INVALID:
        return Action::NONE;
      case Status::SHIFTING:
        return Action::SHIFT_CARDS;
      case Status::SHIFTING_BACK:
        return Action::RETURN_CARDS;
      case Status::ASKING:
        return Action::SHIFT_CARDS;
      case Status::GETTING_BACK:
      case Status::ACCEPTED:
      case Status::DENIED_BY_ALL:
      case Status::FINISHED:
        return Action::NONE;
      } // switch(this->status)
    } // if (arrow clicked)
  } // test whether the arrow is clicked

  { // test whether a card in the middle is clicked
    unsigned cardno = UINT_MAX;
    for (int i = this->cards.size() - 1; i >= 0; --i) {
      Gdk::Point const pos = this->card_pos(i);
      if (   (x >= pos.get_x())
          && (x < (pos.get_x()
                   + this->ui->cards->card(this->cards[i],
                                           this->table->rotation(player)
                                          ).get_width()))
          && (y >= pos.get_y())
          && (y < (pos.get_y()
                   + this->ui->cards->card(this->cards[i],
                                           this->table->rotation(player)
                                          ).get_height()))

          // ToDo: check for transparency
#ifdef POSTPONED
          && !(this->ui->cards->card(cards_drawn[i],
                                     this->table->rotation(player))
               .is_transparent(unsigned(x - pos.get_x()),
                               unsigned(y - pos.get_y())))
#endif
         ) {
        cardno = i;
        break;
      } // if ()
    } // for (i)

    // special case: fill up the cards in the middle
    if (   ::bug_report_replay
        && (   (this->status == Status::SHIFTING)
            || (this->status == Status::SHIFTING_BACK) )
        && (cardno != UINT_MAX)
        && this->cards[cardno].is_empty() )
      return Action::FILL_UP;

    if (cardno != UINT_MAX) {
      switch(this->status) {
      case Status::INVALID:
        DEBUG_ASSERTION(false,
                        "Poverty::possible_action():\n"
                        "  'this->status' == INVALID");
        break;
      case Status::SHIFTING:
        // move a non-trump card in the hand
        if (this->cards[cardno].is_empty())
          return Action::NONE;

        if (this->cards[cardno].istrump())
          return Action::NONE;

        return Action::TAKE_CARD;

      case Status::ASKING:
        return Action::ACCEPT_CARDS;

      case Status::SHIFTING_BACK:
        // move a card in the hand
        if (this->cards[cardno].is_empty())
          return Action::NONE;

        return Action::TAKE_CARD;

      case Status::GETTING_BACK:
        return Action::GET_CARDS_BACK;

      case Status::ACCEPTED:
      case Status::DENIED_BY_ALL:
      case Status::FINISHED:
        break;
      } // switch(this->status)
    } // if (cardno != UINT_MAX)
  } // test whether a card in the middle is clicked

  { // test whether the player wants to move a card from the hand
    // into the middle
    if ((this->status == Status::SHIFTING)
        || (this->status == Status::SHIFTING_BACK))
      if (!(this->middle_full())) {
        unsigned const cardno
          = this->table->hand(player).cardno_at_position(x, y);
        if (cardno != UINT_MAX)
          return Action::PUT_CARD;
      } // if (middle has a free place)

    // look, whether there is space
  } // test whether the player wants to move a card from the hand

  return Action::NONE;
} // Poverty::Action Poverty::possible_action(int x, int y) const

/**
 ** moves the card at 'pos' in the middle
 **
 ** @param     cardno   number of cards to shift
 **
 ** @return    -
 **/
void
Poverty::add_card_to_shift(unsigned const cardno)
{
  Player& player = const_cast<Player&>(*(this->player));

  // search the first free card in the middle
  auto c = std::find(std::begin(this->cards), std::end(this->cards),
                     Card());

  if (c == this->cards.end())
    // no free card found - ignore the taking of the card
    return ;

  DEBUG_ASSERTION((player.sorted_hand().played(cardno) == false),
                  "Poverty::add_card_to_shift(cardno):\n"
                  "  card number '" << cardno << "' is already played:\n"
                  "Hand:\n"
                  << player.sorted_hand());

  // move the card in the middle
  *c = player.sorted_hand().card_all(cardno);
  player.sorted_hand().playcard(player.sorted_hand().pos_all_to_pos(cardno));

  this->draw();
  this->table->hand(player).draw();

#ifdef WORKAROUND
  { // just testing
    unsigned selected_cards = 0;
    for (auto const& c : this->cards)
      if (!c.is_empty())
        selected_cards += 1;

    DEBUG_ASSERTION((player.sorted_hand().cardsnumber() + selected_cards
                     == player.sorted_hand().cardsnumber_all()),
                    "Poverty::add_card_to_shift(cardno = " << cardno
                    << ")\n"
                    "  'player.sorted_hand().cardsnumber() + this->cards.size()' "
                    " = " << player.sorted_hand().cardsnumber() + this->cards.size()
                    << " != " << player.sorted_hand().cardsnumber_all()
                    << " = player.sorted_hand().cardsnumber_all()\n"
                    << "  shifted cards: " << this->cards << '\n'
                    << " Hand: " << player.sorted_hand());
  }
#endif // #ifdef WORKAROUND

  return ;
} // void Poverty::add_card_to_shift(unsigned cardno)

/**
 ** -> result
 **
 ** @param     x   x position
 ** @param     y   y position
 **
 ** @return    the number of the card at the position
 **/
unsigned
Poverty::cardno_at_position(int const x, int const y) const
{
  for (int i = this->cards.size() - 1; i >= 0; --i) {
    auto const pos = this->card_pos(i);
    if (   (x >= pos.get_x())
        && (x < (pos.get_x()
                 + this->ui->cards->card(this->cards[i],
                                         this->table->rotation(*this->player)
                                        ).get_width()))
        && (y >= pos.get_y())
        && (y < (pos.get_y()
                 + this->ui->cards->card(this->cards[i],
                                         this->table->rotation(*this->player)
                                        ).get_height()))

        // ToDo: check for transparency
#ifdef POSTPONED
        && !(this->ui->cards->card(cards_drawn[i],
                                   this->table->rotation(player))
             .is_transparent(unsigned(x - pos.get_x()),
                             unsigned(y - pos.get_y())))
#endif
       )
      return i;
  } // for (i)

  return UINT_MAX;
} // unsigned Poverty::cardno_at_position(int x, int y) const

} // namespace UI_GTKMM_NS

#endif // #ifdef USE_UI_GTKMM
