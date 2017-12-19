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

namespace UI_TEXT_NS {

  /**
   **
   ** let the user select a card for the given player
   **
   ** @param	player	the player who is to play a card
   **
   ** @return	the selected card
   **
   ** @version	0.5.4
   **
   ** @author	Diether Knof
   **
   ** @todo	all
   **
   **/
  HandCard
    UI_Text::card_get(Player const& player)
    {
      return HandCard::EMPTY;
    } // HandCard UI_Text::card_get(Player const& player)

  /**
   **
   ** the given player has played 'card.
   ** remove the card from the hand
   **
   ** @param	card	the played card
   ** @param	player	the player who has played the card
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
    UI_Text::card_played(Card const& card, Player const& player)
    { 
      return ;
    } // void UI_Text::card_player(Card const& card, Player const& player)

  /**
   **
   ** the given player has made the announcement 'announcement'
   **
   ** @param	announcement	the annoucnement
   ** @param	player		the player who has made the announcement
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
    UI_Text::announcement_made(Announcement const& announcement,
				Player const& player)
    {
      return ;
    } // void UI_Text::announcement_made(Announcement const& announcement, Player const& player)

} // namespace UI_TEXT_NS

#endif // #ifdef USE_UI_TEXT
