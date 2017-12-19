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

#include "gameplay.h"

#include "../party/party.h"
#include "../party/rule.h"
#include "../game/game.h"
#include "../game/game_summary.h"
#include "../card/trick.h"

namespace OS_NS {

  /**
   **
   ** constructor
   **
   ** @param	ostr	output stream
   **
   ** @return	-
   ** 
   ** @version	0.6.7
   **
   ** @author	Diether Knof
   **
   **/
  Gameplay::Gameplay(ostream* ostr) :
    OS(OS_TYPE::GAMEPLAY),
    ostr(ostr)
  {
    return ;
  } // Gameplay::Gameplay()

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
  Gameplay::~Gameplay()
  {
    return ;
  } // Gameplay::~Gameplay()

  /**
   **
   ** the party is opened
   **
   ** @param	party	party that is opened
   **
   ** @return	-
   ** 
   ** @version	0.6.7
   **
   ** @author	Diether Knof
   **
   **/
  void
    Gameplay::party_open(Party const& party)
    {
      this->OS::party_open(party);

      *this->ostr << "party open\n";

      return ;
    } // void Gameplay::party_open(Party& party)

  /**
   **
   ** the party is started
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
  void
    Gameplay::party_start()
    { 
      this->OS::party_start();

      *this->ostr << "party start\n";
      *this->ostr << ::party.rule();

      return ;
    } // void Gameplay::party_start()

  /**
   **
   ** the party is finished
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
  void
    Gameplay::party_finish()
    {
      this->OS::party_finish();

      *this->ostr << "party finish\n";

      return ;
    } // void Gameplay::party_finish()

  /**
   **
   ** the party is closed
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
  void
    Gameplay::party_close()
    {
      this->OS::party_close();

      *this->ostr << "party close\n";

      return ;
    } // void Gameplay::party_close()

  /**
   **
   ** the game is opened
   **
   ** @param	game	game that is opened
   **
   ** @return	-
   ** 
   ** @version	0.6.7
   **
   ** @author	Diether Knof
   **
   **/
  void
    Gameplay::game_open(Game const& game)
    {
      this->OS::game_open(game);

      *this->ostr << "game open: " << this->party().gameno() << "\n";
      *this->ostr << "seed: " << this->game().seed() << "\n";

      return ;
    } // void Gameplay::game_open(Game const& game)

  /**
   **
   ** the cards are distributed
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
    Gameplay::game_cards_distributed()
    {
      this->OS::game_cards_distributed();

      *this->ostr << "cards distributed\n";

      return ;
    } // void Gameplay::game_cards_distributed()

  /**
   **
   ** the game is redistributed
   **
   ** @param	-
   **
   ** @return	-
   ** 
   ** @version	0.6.7b
   **
   ** @author	Diether Knof
   **
   **/
  void
    Gameplay::game_redistribute()
    {
      this->OS::game_redistribute();

      *this->ostr << "game redistribute\n";

      return ;
    } // void Gameplay::game_redistribute()

  /**
   ** a reservation has been made
   **
   ** @param	-
   **
   ** @return	-
   **
   ** @author	Diether Knof
   ** 
   ** @version	0.7.13
   **/
  void
    Gameplay::reservation_got(Reservation const& reservation,
                              Player const& player)
    {
      this->OS::reservation_got(reservation, player);

      *this->ostr << "reservation: "
        << player.game().no(player) << " " << player.name() << ": "
        << reservation
        << "\n";

      return ;
    } // void Gameplay::reservation_got(Reservation reservation, Player player)

  /**
   **
   ** the game is started
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
  void
    Gameplay::game_start()
    {
      this->OS::game_start();

      *this->ostr << "game start: " << this->game().type() << "\n";

      if (is_solo(this->game().type()))
	*this->ostr << "Soloplayer: "
	  << this->game().no(this->game().soloplayer())
	  << "\n";

      return ;
    } // void Gameplay::game_start()

  /**
   **
   ** the game is finished
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
  void
    Gameplay::game_finish()
    { 
      this->OS::game_finish();

      *this->ostr << "game finish\n";

      *this->ostr << this->party().last_game_summary();

      return ;
    } // void Gameplay::game_finish()

  /**
   **
   ** the game is closed
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
  void
    Gameplay::game_close()
    {
      this->OS::game_close();

      *this->ostr << "game close\n";

      return ;
    } // void Gameplay::game_close()

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
    Gameplay::card_played(HandCard const& card)
    { 
      this->OS::card_played(card);

      *this->ostr << this->game().trick_current().actcardno() << ". "
	<< card.player().name() << ": "
	<< card << "\n";

      return ;
    } // void Gameplay::card_played(HandCard const& card)

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
    Gameplay::announcement_made(Announcement const& announcement,
				Player const& player)
    {
      this->OS::announcement_made(announcement, player);

      *this->ostr << "announcement: "
	<< "player " << player.game().no(player) << ": "
	<< announcement
	<< "\n";

      return ;
    } // void Gameplay::announcement_made(Announcement const& announcement, Player const& player)

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
    Gameplay::swines_announced(Player const& player)
    {
      this->OS::swines_announced(player);

      *this->ostr << "swines announced: "
	<< "player " << player.game().no(player)
	<< "\n";

      return ;
    } // void Gameplay::swines_announced(Player const& player)

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
    Gameplay::hyperswines_announced(Player const& player)
    {
      this->OS::hyperswines_announced(player);

      *this->ostr << "hyperswines announced: "
	<< "player " << player.game().no(player)
	<< "\n";

      return ;
    } // void Gameplay::hyperswines_announced(Player const& player)

  /**
   **
   ** 'player' shifts 'cardno' cards
   **
   ** @param	player	the player who shifts the cards
   ** @param	cardno	the number of cards that are shifted
   **
   ** @return	-
   **
   ** @version	0.6.7
   **
   ** @author	Diether Knof
   **
   **/
  void
    Gameplay::poverty_shift(Player const& player, unsigned const cardno)
    { 
      this->OS::poverty_shift(player, cardno);

      *this->ostr << "poverty: "
	<< "player " << player.game().no(player)
	<< " shifts " << cardno << " cards.\n";

      return ;
    } // void Gameplay::poverty_shift(Player const& player, unsigned const cardno)

  /**
   **
   ** all players have denied to take the cards
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
  void
    Gameplay::poverty_take_denied_by_all()
    { 
      this->OS::poverty_take_denied_by_all();

      *this->ostr << "poverty: "
	<< "denied by all players\n";

      return ;
    } // void Gameplay::poverty_take_denied_by_all()

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
   ** @version	0.6.7
   **
   ** @author	Diether Knof
   **
   **/
  void
    Gameplay::poverty_take_accepted(Player const& player,
				    unsigned const cardno,
				    unsigned const trumpno)
    { 
      this->OS::poverty_take_accepted(player, cardno, trumpno);

      *this->ostr << "poverty: "
	<< "player " << player.game().no(player) << " accepts poverty and returns "
	<< cardno << " cards with " << trumpno << " trumps.\n";

      return ;
    } // void Gameplay::poverty_take_accepted(Player const& player, unsigned const cardno, unsigned const trumpno)

  /**
   **
   ** marriage: Information of the new team
   **
   ** @param	bridegroom	the player with the marriage
   ** @param	bride		the bride
   **
   ** @return	-
   **
   ** @author	Diether Knof
   **
   ** @version	0.6.7
   **
   **/
  void
    Gameplay::marriage(Player const& bridegroom,
		       Player const& bride)
    {
      this->OS::marriage(bridegroom, bride);

      *this->ostr << "Marriage: "
	<< "player " << bridegroom.game().no(bridegroom)
	<< " with player " << bridegroom.game().no(bride)
	<< '\n';

      return ;
    } // void Gameplay::marriage(Player const& bridegroom, Player const& bride)

  /**
   **
   ** genscher: Information of the new team
   **
   ** @param	player	genscher player
   ** @param	partner	partner of the gensher player
   **
   ** @return	-
   **
   ** @author	Diether Knof
   **
   ** @version	0.6.7
   **
   **/
  void
    Gameplay::genscher(Player const& player,
		       Player const& partner)
    {
      this->OS::genscher(player, partner);

      *this->ostr << "Genscher: "
	<< "player " << player.game().no(player)
	<< " with player " << player.game().no(partner)
	<< '\n';

      return ;
    } // void Gameplay::genscher(Player const& player, Player const& partner)

  /**
   **
   ** 'trick' has been opened
   **
   ** @param	trick	opened trick
   **
   ** @return	-
   **
   ** @author	Diether Knof
   **
   ** @version	0.6.7
   **
   **/
  void
    Gameplay::trick_open(Trick const& trick)
    {
      this->OS::trick_open(trick);

      *this->ostr << "\n"
	<< "Trick: " << this->game().trick_current_no() << "\n";

      return ;
    } // void Gameplay::trick_open()

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
    Gameplay::trick_full()
    {
      this->OS::trick_full();

      *this->ostr << "Winner: "
	<< this->trick().winnerplayer().name()
	<< "\n";

      return ;
    } // void Gameplay::trick_full()

} // namespace OS_NS
