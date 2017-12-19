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

#ifdef USE_NETWORK

#include "constants.h"

#include "../actor.h"

#include "../../party/party.h"
#include "../../party/rule.h"

namespace Network {

  /**
   ** start the party
   **
   ** @param     -
   **
   ** @return    -
   **
   ** @author    Diether Knof
   **
   ** @version   0.7.2
   **
   ** @todo      better changing of the status
   **/
  void
    Connection::Actor::party_start()
    {
      ::game_status = GAMESTATUS::PARTY_PLAY;
      return ;
    } // void Connection::Actor::party_start()

  /**
   ** start the round 'roundno'
   **
   ** @param     roundno   number of round
   **
   ** @return    -
   **
   ** @author    Diether Knof
   **
   ** @version   0.7.2
   **
   ** @todo      all
   **/
  void
    Connection::Actor::party_start_round(unsigned const roundno)
    {
      return ;
    } // void Connection::Actor::party_start_round(unsigned roundno)

  /**
   ** set the startplayer
   **
   ** @param     startplayer   the startplayer
   **
   ** @return    -
   **
   ** @author    Diether Knof
   **
   ** @version   0.7.3
   **/
  void
    Connection::Actor::party_set_startplayer(unsigned const startplayer)
    {
      if (::game_status != GAMESTATUS::PARTY_NEW)
	return ;

      ::party.set_startplayer(startplayer);

      return ;
    } // void Connection::Actor::party_start_round(unsigned startplayer)

  /**
   ** set the seed
   **
   ** @param     seed   the seed
   **
   ** @return    -
   **
   ** @author    Diether Knof
   **
   ** @version   0.7.3
   **/
  void
    Connection::Actor::party_set_seed(unsigned const seed)
    {
      if (::game_status != GAMESTATUS::PARTY_NEW)
	return ;

      ::party.set_seed(seed);

      return ;
    } // void Connection::Actor::party_start_round(unsigned seed)

  /**
   ** finish the party
   **
   ** @param     -
   **
   ** @return    -
   **
   ** @author    Diether Knof
   **
   ** @version   0.7.2
   **
   ** @todo      all
   **/
  void
    Connection::Actor::party_finish()
    {
      return ;
    } // void Connection::Actor::party_finish()

} // namespace Network

#endif // #ifdef USE_NETWORK
