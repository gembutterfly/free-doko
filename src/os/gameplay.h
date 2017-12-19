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

#ifndef CLASS_OS_GAMEPLAY_HEADER
#define CLASS_OS_GAMEPLAY_HEADER

#include "os.h"

namespace OS_NS {

  /// The default Gameplay
  class Gameplay : public OS {
    public:
      Gameplay(ostream* ostr = &cout);

      ~Gameplay();

      // the parts of a party
      void party_open(Party const& party);
      void party_start();
      void party_finish();
      void party_close();

      // the parts of a game
      void game_open(Game const& game);
      void game_cards_distributed();
      void game_redistribute();
      void reservation_got(Reservation const& reservation, Player const& player);
      void game_start();
      void game_finish();
      void game_close();

      void trick_open(Trick const& trick);
      void trick_full();
      // the card is played
      void card_played(HandCard const& card);
      // an announcement is made
      void announcement_made(Announcement const& announcement,
				     Player const& player);
      // the player has swines
      void swines_announced(Player const& player);
      // the player has hyperswines
      void hyperswines_announced(Player const& player);

      // 'player' shifts 'cardno' cards
      void poverty_shift(Player const& player, unsigned const cardno);
      // all players have denied to take the cards
      void poverty_take_denied_by_all();
      // the player 'player' has accepted the poverty trumps
      // and has returned 'cardno' cards with 'trumpno' trumps
      void poverty_take_accepted(Player const& player,
					 unsigned const cardno,
					 unsigned const trumpno);

      // the marriage partner has found a bride
      void marriage(Player const& bridegroom, Player const& bride);
      // a player has announced a genscher
      void genscher(Player const& genscher, Player const& partner);
    private:
      ostream* ostr;

    private: // not implemented
      Gameplay(Gameplay const&);
      Gameplay& operator=(Gameplay const&);
  }; // class Gameplay

} // namespace OS_NS

#endif // #ifndef CLASS_OS_GAMEPLAY_HEADER
