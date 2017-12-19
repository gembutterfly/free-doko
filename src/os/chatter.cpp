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

#include "chatter.h"

#include "../basistypes.h"

#include "../party/party.h"
#include "../game/game.h"
#include "../game/game_summary.h"
#include "../card/trick.h"
#include "../ui/ui.h"
#include "../misc/translations.h"

namespace OS_NS {

  /**
   ** constructor
   **
   ** @param	-
   **
   ** @return	-
   ** 
   ** @version	0.6.7
   **
   ** @author	Diether Knof
   **
   **/
  Chatter::Chatter() :
    OS(OS_TYPE::CHATTER)
  {
    return ;
  } // Chatter::Chatter()

  /**
   **
   ** destructor
   **
   ** @param	-
   **
   ** @return	-
   ** 
   ** @version	0.6.7
   **
   ** @author	Diether Knof
   **
   **/
  Chatter::~Chatter()
  {
    return ;
  } // Chatter::~Chatter()

  /**
   **
   ** the party is started
   ** greet the players
   **
   ** @param	-
   **
   ** @return	-
   ** 
   ** @version	0.6.8
   **
   ** @author	Diether Knof
   **
   **/
  void
    Chatter::party_start()
    { 
      for (vector<Player*>::const_iterator p = this->party().players().begin();
	   p != this->party().players().end();
	   ++p) {
	// disallow '%' in the name because of the parsing
	if ((*p)->name().find('%') != string::npos)
	  continue;
	if (::translator.contains("chat: greeting: " + (*p)->name()))
	  ::ui->chat("",
		     ::translation("chat: greeting: " + (*p)->name()
				  ).translation());
      }

      return ;
    } // void Chatter::party_start()

  /**
   **
   ** the game is finished
   **
   ** @param	-
   **
   ** @return	-
   ** 
   ** @version	0.6.8
   **
   ** @author	Diether Knof
   **
   **/
  void
    Chatter::game_finish()
    { 
      for (vector<Player*>::const_iterator p = this->game().players_begin();
	   p != this->game().players_end();
	   ++p)
	if ((*p)->team() == this->game().winnerteam())
	  ::ui->chat((*p)->name(),
		     ::translation("chat: game: won").translation());

      return ;
    } // void Chatter::game_finish()

  /**
   **
   ** 'player' has played 'card'
   **
   ** @param	card	the played card
   **
   ** @return	-
   ** 
   ** @version	0.6.8
   **
   ** @author	Diether Knof
   **
   **/
  void
    Chatter::card_played(HandCard const& card)
    { 
      switch (this->game().type()) {
      case GAMETYPE::NORMAL:
	if ((card == Card::CLUB_QUEEN)
	   && (card.player().announcement() == ANNOUNCEMENT::NOANNOUNCEMENT)) {
	  ::ui->chat(this->game().trick_current().winnerplayer().name(),
		     ::translation("chat: played: club queen").translation());
	}
	break;
      default:
	break;
      } // switch (this->game().type())

      return ;
    } // void Chatter::card_played(HandCard const& card)

  /**
   **
   ** the trick is full
   **
   ** @param	-
   **
   ** @return	-
   **
   ** @author	Diether Knof
   **
   ** @version	0.6.7
   **
   **/
  void
    Chatter::trick_full()
    {
      if (!this->trick().specialpoints().empty())
	::ui->chat(this->trick().winnerplayer().name(),
		   ::translation("chat: trick: specialpoints").translation());
      else if (this->trick().points() >= 30)
	::ui->chat(this->trick().winnerplayer().name(),
		   ::translation("chat: trick: ge 30 points").translation());

      return ;
    } // void Chatter::trick_full()

  /**
   **
   ** the announcement 'announcement' has been made by player 'player'
   **
   ** @param	announcement	the announcement
   ** @param	player		the player, who has made the announcement
   **
   ** @return	-
   **
   ** @version	0.6.7
   **
   ** @author	Diether Knof
   **
   **/
  void
    Chatter::announcement_made(Announcement const& announcement,
			       Player const& player)
    {
      if (ANNOUNCEMENT::is_real(announcement))
      ::ui->chat(player.name(),
		 ::translation(::name(announcement)).translation());
      else if (ANNOUNCEMENT::is_reply(announcement))
      ::ui->chat(player.name(),
		 ::translation("chat: announcement: reply").translation());

      return ;
    } // void Chatter::announcement_made(Announcement announcement, Player player)

  /**
   **
   ** the player has swines
   **
   ** @param	player		the player with the swines
   **
   ** @return	-
   **
   ** @version	0.6.7
   **
   ** @author	Diether Knof
   **
   **/
  void
    Chatter::swines_announced(Player const& player)
    {
      ::ui->chat(player.name(),
		 ::translation("swines").translation());

      return ;
    } // void Chatter::swines_announced(Player const& player)

  /**
   **
   ** the player has hyperswines
   **
   ** @param	player		the player with the swines
   **
   ** @return	-
   **
   ** @version	0.6.7
   **
   ** @author	Diether Knof
   **
   **/
  void
    Chatter::hyperswines_announced(Player const& player)
    {
      ::ui->chat(player.name(),
		 ::translation("hyperswines").translation());

      return ;
    } // void Chatter::hyperswines_announced(Player const& player)

} // namespace OS_NS
