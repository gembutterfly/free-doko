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

#ifdef USE_UI_GTKMM

#include "table.h"
#include "poverty.h"
#include "trick.h"
#include "hand.h"
#include "trickpile.h"
#include "icongroup.h"
#include "name.h"

#include "ui.h"
#include "cards.h"

#include "../../player/player.h"
#include "../../player/human/human.h"
#include "../../party/party.h"
#include "../../game/game.h"
#include "../../misc/setting.h"

#include <gtkmm/drawingarea.h>

namespace UI_GTKMM_NS {

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    the poverty
 **/
Poverty&
  Table::poverty()
  {
    return *this->poverty_;
  } //Poverty& Table::poverty()

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    the trick
 **/
Trick&
  Table::trick()
  {
    return *this->trick_;
  } // Trick& Table::trick()

/**
 ** -> result
 **
 ** @param     player   the player
 **
 ** @return    the hand object of 'player'
 **/
Hand&
  Table::hand(Player const& player)
  {
    return this->hand(this->position(player));
  } // Hand& Table::hand(Player player)

/**
 ** -> result
 **
 ** @param     player   the player
 **
 ** @return    the hand object of 'player'
 **/
Hand const&
  Table::hand(Player const& player) const
  {
    return this->hand(this->position(player));
  } // Hand const& Table::hand(Player player) const

/**
 ** -> result
 **
 ** @param     position   the position
 **
 ** @return    the hand object at 'position'
 **/
Hand&
  Table::hand(Position const position)
  {
    //return *this->hand_[position]; // see corresponding const-function
    return *this->hand_.find(position)->second;
  } // Hand& Table::hand(Position position)

/**
 ** -> result
 **
 ** @param     position   the position
 **
 ** @return    the hand object at 'position'
 **/
Hand const&
  Table::hand(Position const position) const
  {
    //return *this->hand_[position]; // needs write access
    return *this->hand_.find(position)->second;
  } // Hand const& Table::hand(Position position) const

/**
 ** -> result
 **
 ** @param     player   the player
 **
 ** @return    the trickpile object of 'player'
 **/
TrickPile&
  Table::trickpile(Player const& player)
  {
    return this->trickpile(this->position(player));
  } // TrickPile& Table::trickpile(Player player)

/**
 ** -> result
 **
 ** @param     player   the player
 **
 ** @return    the trickpile object of 'player'
 **/
TrickPile const&
  Table::trickpile(Player const& player) const
  {
    return this->trickpile(this->position(player));
  } // TrickPile const& Table::trickpile(Player player) const

/**
 ** -> result
 **
 ** @param     position   the position
 **
 ** @return    the trickpile object at 'position'
 **/
TrickPile&
  Table::trickpile(Position const position)
  {
    //return *this->trickpile_[position]; // see corresponding const-function
    return *this->trickpile_.find(position)->second;
  } // TrickPile& Table::trickpile(Position position)

/**
 ** -> result
 **
 ** @param     position   the position
 **
 ** @return    the trickpile object at 'position'
 **/
TrickPile const&
  Table::trickpile(Position const position) const
  {
    //return *this->trickpile_[position]; // needs write access
    return *this->trickpile_.find(position)->second;
  } // TrickPile const& Table::trickpile(Position position) const

/**
 ** -> result
 **
 ** @param     player   the player
 **
 ** @return    the icongroup object of 'player'
 **/
Icongroup&
  Table::icongroup(Player const& player)
  {
    return this->icongroup(this->position(player));
  } // Icongroup& Table::icongroup(Player player)

/**
 ** -> result
 **
 ** @param     player   the player
 **
 ** @return    the icongroup object of 'player'
 **/
Icongroup const&
  Table::icongroup(Player const& player) const
  {
    return this->icongroup(this->position(player));
  } // Icongroup const& Table::icongroup(Player player) const

/**
 ** -> result
 **
 ** @param     position   the position
 **
 ** @return    the icongroup object at 'position'
 **/
Icongroup&
  Table::icongroup(Position const position)
  {
    //return *this->icongroup_[position]; // see corresponding const-function
    return *this->icongroup_.find(position)->second;
  } // Icongroup& Table::icongroup(Position position)

/**
 ** -> result
 **
 ** @param     position   the position
 **
 ** @return    the icongroup object at 'position'
 **/
Icongroup const&
  Table::icongroup(Position const position) const
  {
    //return *this->icongroup_[position]; // needs write access
    return *this->icongroup_.find(position)->second;
  } // Icongroup const& Table::icongroup(Position position) const

/**
 ** -> result
 **
 ** @param     player   the player
 **
 ** @return    the name object of 'player'
 **/
Name&
  Table::name(Player const& player)
  {
    return this->name(this->position(player));
  } // Name& Table::name(Player player)

/**
 ** -> result
 **
 ** @param     player   the player
 **
 ** @return    the name object of 'player'
 **/
Name const&
  Table::name(Player const& player) const
  {
    return this->name(this->position(player));
  } // Name const& Table::name(Player player) const

/**
 ** -> result
 **
 ** @param     position   the position
 **
 ** @return    the name object at 'position'
 **/
Name&
  Table::name(Position const position)
  {
    //return *this->name_[position]; // see corresponding const-function
    return *this->name_.find(position)->second;
  } // Name& Table::name(Position position)

/**
 ** -> result
 **
 ** @param     position   the position
 **
 ** @return    the name object at 'position'
 **/
Name const&
  Table::name(Position const position) const
  {
    //return *this->name_[position]; // needs write access
    return *this->name_.find(position)->second;
  } // Name const& Table::name(Position position) const

/**
 ** -> result
 **
 ** @param     player   the player
 **
 ** @return    the reservation object of 'player'
 **/
Reservation&
  Table::reservation(Player const& player)
  {
    return this->reservation(this->position(player));
  } // Reservation& Table::reservation(Player player)

/**
 ** -> result
 **
 ** @param     player   the player
 **
 ** @return    the reservation object of 'player'
 **/
Reservation const&
  Table::reservation(Player const& player) const
  {
    return this->reservation(this->position(player));
  } // Reservation const& Table::reservation(Player player) const

/**
 ** -> result
 **
 ** @param     position   the position
 **
 ** @return    the reservation object at 'position'
 **/
Reservation&
  Table::reservation(Position const position)
  {
    //return *this->reservation_[position]; // see corresponding const-function
    return *this->reservation_.find(position)->second;
  } // Reservation& Table::reservation(Position position)

/**
 ** -> result
 **
 ** @param     position   the position
 **
 ** @return    the reservation object at 'position'
 **/
Reservation const&
  Table::reservation(Position const position) const
  {
    //return *this->reservation_[position]; // needs write access
    return *this->reservation_.find(position)->second;
  } // Reservation const& Table::reservation(Position position) const

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    whether a human player shall get a card
 **/
bool
  Table::get_card() const
  { return this->get_card_; }


/**
 ** -> result
 **
 ** @param     player   the player
 **
 ** @return    the position of 'player'
 **/
Position
  Table::position(Player const& player) const
  {
    return ::setting.position(this->ui->game().no(player));
  } // Position Table::position(Player player) const

/**
 ** -> result
 **
 ** @param     position   position of the player
 **
 ** @return    the player at 'position'
 **
 ** @todo      remove the for-loop
 **/
Player const&
  Table::player(Position const position) const
  {
    for (unsigned p = 0; p < this->ui->game().number_of_players(); ++p)
      if (::setting.position(p) == position)
        return this->ui->game().player(p);

    DEBUG_ASSERTION(false,
                    "Table::player(position)\n"
                    "  could not find player at position "
                    << "'" << position << "'");

    return this->ui->game().player(0);
  } // Player const& Table::player(Position position) const

/**
 ** -> result
 **
 ** @param     player   the player
 **
 ** @return    the rotation of 'player'
 **/
Rotation
  Table::rotation(Player const& player) const
  {
    return this->rotation(this->position(player));
  } // Rotation Table::rotation(Player player) const

/**
 ** -> result
 **
 ** @param     position   the position
 **
 ** @return    the rotation of the player at 'position'
 **/
Rotation
  Table::rotation(Position const position) const
  {
    switch (position) {
    case Position::NORTH:
      return Rotation::DOWN;
    case Position::SOUTH:
      return Rotation::UP;
    case Position::WEST:
      return Rotation::RIGHT;
    case Position::EAST:
      return Rotation::LEFT;
    case Position::CENTER:
      DEBUG_ASSERTION(false,
                      "Table::rotation(position):\n"
                      "  wrong position CENTER (" << position << ")");
    } // switch(this->position())

    return Rotation::UP;
  } // Rotation Table::rotation(Position position) const

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    cairo context
 **/
Cairo::RefPtr<Cairo::Context> 
  Table::create_cairo_context()
  {
    return this->get_window()->create_cairo_context();
  } // Cairo::RefPtr<Cairo::Context> Table::create_cairo_context()

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    the width of the table
 **/
unsigned
  Table::width() const
  {
    return this->get_width();
  } // unsigned Table::width() const

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    the height of the table
 **/
unsigned
  Table::height() const
  {
    return this->get_height();
  } // unsigned Table::height() const

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    border on the left and right
 **/
unsigned
  Table::border_x() const
  {
    return (this->ui->cards->height() / 10);
  } // unsigned Table::border_x() const

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    border on the top and bottom
 **/
unsigned
  Table::border_y() const
  {
    return (this->ui->cards->height() / 10);
  } // unsigned Table::border_y() const

} // namespace UI_GTKMM_NS

#endif // #ifdef USE_UI_GTKMM
