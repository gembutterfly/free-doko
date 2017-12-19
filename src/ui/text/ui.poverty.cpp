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

#ifdef USE_UI_TEXT

#include "ui.h"

#include "../../card/hand.h"

namespace UI_TEXT_NS {

  /**
   **
   ** 'player' shifts 'cardno' cards
   **
   ** @param	player	the player who shifts the cards
   ** @param	cardno	the number of cards that are shifted
   **
   ** @return	-
   **
   ** @version	0.5.4
   **
   ** @author	Diether Knof
   **
   ** @todo	all
   **
   **/
  void
    UI_Text::poverty_shift(Player const& player, unsigned const cardno)
    { 
      return ;
    } // void UI_Text::poverty_shift(Player const& player, unsigned const cardno)

  /**
   **
   ** ask 'player' whether to accept the poverty
   **
   ** @param	player	the player who is asked
   ** @param	cardno	the number of cards that are shifted
   **
   ** @return	-
   **
   ** @version	0.5.4
   **
   ** @author	Diether Knof
   **
   ** @todo	all
   **
   **/
  void
    UI_Text::poverty_ask(Player const& player, unsigned const cardno)
    { 
      return ;
    } // void UI_Text::poverty_ask(Player const& player, unsigned const cardno)

  /**
   **
   ** 'player' denied to take the shifted cards
   **
   ** @param	player	the player who has denied to take the cards
   **
   ** @return	-
   **
   ** @version	0.5.4
   **
   ** @author	Diether Knof
   **
   ** @todo	all
   **
   **/
  void
    UI_Text::poverty_take_denied(Player const& player)
    { 
      return ;
    } // void UI_Text::poverty_take_denied(Player const& player)

  /**
   **
   ** all players have denied to take the cards
   **
   ** @param	-
   **
   ** @return	-
   **
   ** @version	0.5.4
   **
   ** @author	Diether Knof
   **
   ** @todo	all
   **
   **/
  void
    UI_Text::poverty_take_denied_by_all()
    { 
      return ;
    } // void UI_Text::poverty_take_denied_by_all()

  /**
   **
   ** 'player' accepts to take the shifted cards
   ** and returns 'cardno' cards with 'trumpno' trumps
   **
   ** @param	player	the player who has denied to take the cards
   ** @param	cardno	number of cards that are given back
   ** @param	trumpno	number of trumps of the cards
   **
   ** @return	-
   **
   ** @version	0.5.4
   **
   ** @author	Diether Knof
   **
   ** @todo	all
   **
   **/
  void
    UI_Text::poverty_take_accepted(Player const& player,
                                   unsigned const cardno,
                                   unsigned const trumpno)
    { 
      return ;
    } // void UI_Text::poverty_take_accepted(Player const& player, unsigned const cardno, unsigned const trumpno)

  /**
   **
   ** returns which cards the player shifts
   **
   ** @param	player	the player who shifts the cards
   **
   ** @return	the cards that are to be shifted
   **
   ** @version	0.5.4
   **
   ** @author	Diether Knof
   **
   ** @todo	all
   **
   **/
  HandCards
    UI_Text::poverty_shift(Player& player)
    { 
      return HandCards();
    } // HandCards UI_Text::poverty_shift(Player& player)

  /**
   **
   ** returns whether 'player' accepts the shifted cards
   **
   ** @param	player	the player who shifts the cards
   ** @param	cardno	the number of shifted cards
   **
   ** @return	whether to accept the cards
   **
   ** @version	0.5.4
   **
   ** @author	Diether Knof
   **
   ** @todo	all
   **
   **/
  bool
    UI_Text::poverty_take_accept(Player& player, unsigned const cardno)
    { 
      return false;
    } // void UI_Text::poverty_take_accept(Player& player, unsigned cardno)

  /**
   **
   ** changes the cards from the poverty-player
   **
   ** @param	player	the player who has accepted the cards
   ** @param	cards	the cards that are given to the player
   **
   ** @return	the cards that are returned to the poverty-player
   **
   ** @version	0.5.4
   **
   ** @author	Diether Knof
   **
   ** @todo	all
   **
   **/
  HandCards
    UI_Text::poverty_cards_change(Player& player, HandCards const& cards)
    { 
      return HandCards();
    } // HandCards UI_Text::poverty_cards_change(Player& player, HandCards const& cards)

  /**
   **
   ** the poverty player 'player' gets 'cards'
   **
   ** @param	player	the player gets the cards
   ** @param	cards	the cards that are given to the player
   **
   ** @return	-
   **
   ** @version	0.5.4
   **
   ** @author	Diether Knof
   **
   ** @todo	all
   **
   **/
  void
    UI_Text::poverty_cards_get_back(Player& player, Hand const& cards)
    { 
      return ;
    } // void UI_Text::poverty_cards_get_back(Player& player, Hand const& cards)

} // namespace UI_TEXT_NS

#endif // #ifdef USE_UI_TEXT
