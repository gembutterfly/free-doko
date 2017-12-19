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
 ********************************************************************/

#ifdef USE_NETWORK

#include "constants.h"

#include "sender.h"

#include "../server.h"
#include "../../game/game.h"
#include "../../game/gameplay.h"
#include "../../game/gameplay_action.h"
#include "../../player/player.h"

#include "../../utils/string.h"

namespace Network {
  namespace FreeDoko {

   /**
    ** the cards are distributed
    **
    ** @param     hands   distributed hands
    **
    ** @return    -
    **
    ** @author    Diether Knof
    **
    ** @version   0.7.3
    **/
   void
     Interpreter::Sender::gameplay_action(GameplayAction const& action)
     {
       this->send("gameplay action: " + action.str());

       return ;
     } // void Interpreter::Sender::gameplay_action(GameplayAction action)

   /**
    ** the game is opened
    **
    ** @param     game   game that is opened
    **
    ** @return    -
    **
    ** @author    Diether Knof
    **
    ** @version   0.7.12
    **/
   void
     Interpreter::Sender::game_open(Game const& game)
     {
       this->send("game: open");
       this->send("game: startplayer: "
		  + DK::Utils::String::to_string(game.no(game.startplayer()))
		  );
       this->send("game: seed: "
		  + DK::Utils::String::to_string(game.seed())
		  );

       return ;
     } // void Interpreter::Sender::game_open(Game const& game)
     
   /**
    ** the cards are distributed
    **
    ** @param     hands   distributed hands
    **
    ** @return    -
    **
    ** @author    Diether Knof
    **
    ** @version   0.7.3
    **/
   void
     Interpreter::Sender::game_distribute_cards(vector<Hand const*> hands)
     {
       for (unsigned i = 0; i < hands.size(); ++i) {
	 ostringstream ostr;
	 ostr << "game: hand " << i << ": ";
	 HandCards const& cards = hands[i]->cards();
	 HandCards::const_iterator c = cards.begin();
	 if (c != cards.end()) {
	   ostr << *c;
	   for (; c != cards.end(); ++c)
	     ostr << ", " << *c;
	 }
	 this->send(ostr.str());
       }

       return ;
     } // void Interpreter::Sender::game_distribute_cards(vector<Hand const*> hands)

   /**
    ** the game is started
    **
    ** @param     -
    **
    ** @return    -
    **
    ** @author    Diether Knof
    **
    ** @version   0.7.1
    **/
   void
     Interpreter::Sender::game_start()
     {
       this->send("game: start");
       return ;
     } // void Interpreter::Sender::game_start()

   /**
    ** the game is finished
    **
    ** @param     -
    **
    ** @return    -
    **
    ** @author    Diether Knof
    **
    ** @version   0.7.1
    **/
   void
     Interpreter::Sender::game_finish()
     {
       this->send("game: finish");
       return ;
     } // void Interpreter::Sender::game_finish()

   /**
    ** synchronize the game
    ** send the gameplay actions
    **
    ** @param     -
    **
    ** @return    -
    **
    ** @author    Diether Knof
    **
    ** @version   0.7.5
    **/
   void
     Interpreter::Sender::synchronize_game()
     {
	 ostringstream ostr;
         ostr << "sync: gameplay actions\n"
           << "{\n";
         for (vector<GameplayAction*>::const_iterator
              a = this->connection().server().game().gameplay().actions().begin();
              a != this->connection().server().game().gameplay().actions().end();
              ++a)
           ostr << **a << '\n';
         ostr << "}\n";

         this->send_lines(ostr.str());
         return ;
     } // void Interpreter::Sender::synchronize_game()

  } // namespace FreeDoko
} // namespace Network

#endif // #ifdef USE_NETWORK
