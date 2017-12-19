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

#ifdef USE_UI_GTKMM

#ifndef GTKMM_POVERTY_HEADER
#define GTKMM_POVERTY_HEADER

#include "base.h"

#include "../../card/card.h"
#include "../../card/hand_cards.h"
class Game;
class Hand;
class Player;

#include <gdkmm/rectangle.h>
#include <gdkmm/types.h>
#include <cairomm/context.h>

namespace UI_GTKMM_NS {

/**
 ** @brief	the graphic representation of the poverty cards
 **/
class Poverty : public Base {
  friend class Table;
  protected:
  // The status while shifting a poverty
  enum class Status {
    INVALID,
    SHIFTING,
    ASKING,
    SHIFTING_BACK,
    GETTING_BACK,
    ACCEPTED,
    DENIED_BY_ALL,
    FINISHED
  }; // enum Status
  // The actions in shifting a poverty
  enum class Action {
    NONE,
    SHIFT_CARDS,
    ACCEPT_CARDS,
    TAKE_CARD,
    PUT_CARD,
    FILL_UP,
    RETURN_CARDS,
    GET_CARDS_BACK
  }; // enum Action

  public:
  // constructor
  Poverty(class Table* table, Game& game);
  // destructor
  ~Poverty();

  // returns whether the middle is full with cards
  bool middle_full() const;
  // returns whether the shifting is valid
  bool shifting_valid() const;

  // 'player' shifts 'cardno' cards
  void shift(Player const& player, unsigned cardno);
  // ask 'player' whether to accept the poverty
  void ask(Player const& player, unsigned cardno);
  // the player 'player' has denied the poverty trumps
  void take_denied(Player const& player);
  // all players have denied to take the cards
  void take_denied_by_all();
  // the player 'player' has accepted the poverty trumps
  // and has returned 'cardno' cards with 'trumpno' trumps
  void take_accepted(Player const& player,
                     unsigned cardno,
                     unsigned trumpno);
  // 'player' shifts cards
  HandCards shift(Player& player);
  // returns whether 'player' accepts the poverty
  bool take_accept(Player& player, unsigned cardno);
  // the player changes the cards of the poverty
  HandCards cards_change(Player& player,
                         HandCards const& cards_shifted);
  // the poverty player 'player' gets 'cards'
  void cards_get_back(Player& player,
                      HandCards const& cards_returned);

  // start and end position of the card / the arrow
  Gdk::Point card_pos(unsigned cardno) const;
  // outline rectangle of the arrow
  Gdk::Rectangle arrow_outline() const;

  // draws the poverty elements
  void draw();
  void draw(Cairo::RefPtr<::Cairo::Context> cr, bool update = false);

  // a mouse click
  bool button_press_event(GdkEventButton* event);
  // removes the card from the hand and adds it in the middle
  void add_card_to_shift(unsigned cardno);

  // what action a can be made at the position (for the mouse)
  Action possible_action(int x, int y) const; 
  // which card is under the mouse
  unsigned cardno_at_position(int x, int y) const;

#if 0
  // sets the color of the font (from ::settting)
  void arrow_color_set();
#endif

  private:
  class Table* const table = nullptr; // the table, this poverty belongs to

  class Game* const game = nullptr; // the game
  class Player const* player = nullptr; // the active player

  HandCards cards; // the cards that are shifted
  Status status = Status::INVALID; // which acitivy is made
  bool shift_cards = false; // whether to shift the cards
  bool accept_cards = false; // whether to accept the cards

  private: // unused
  Poverty() = delete;
  Poverty(Poverty const&) = delete;
  Poverty& operator=(Poverty const&) = delete;
}; // class Poverty

} // namespace UI_GTKMM_NS

#endif // #ifndef GTKMM_POVERTY_HEADER

#endif // #ifdef USE_UI_GTKMM
