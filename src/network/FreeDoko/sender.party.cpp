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
#include "../../party/rule.h"

#include "../../utils/string.h"

namespace Network {
  namespace FreeDoko {
    // ToDo: seed change
    // ToDo: startplayer change

   /**
    ** send the type of the player
    **
    ** @param     player     player to send the type of
    **
    ** @return    -
    **
    ** @author    Diether Knof
    **
    ** @version   0.7.2
    **/
   void
     Interpreter::Sender::player_type(Player const& player)
     {
       this->send("player " + DK::Utils::String::to_string(::party.no(player))
		  + ": type = " + ( this->connection().manages_player(player)
				   ? "local"
				   : "network") );
       return ;
     } // void Interpreter::Sender::player_type(Player const& player)

   /**
    ** send the name of the player
    **
    ** @param     player     player to send the name of
    **
    ** @return    -
    **
    ** @author    Diether Knof
    **
    ** @version   0.7.2
    **/
   void
     Interpreter::Sender::player_name(Player const& player)
     {
       this->send("player " + DK::Utils::String::to_string(::party.no(player))
		  + ": name = " + player.name());
       return ;
     } // void Interpreter::Sender::player_name(Player const& player)

   /**
    ** the position of two players has switched
    **
    ** @param     player_a   first player
    ** @param     player_b   second player
    **
    ** @return    -
    **
    ** @author    Diether Knof
    **
    ** @version   0.7.3
    **/
   void
     Interpreter::Sender::players_switched(unsigned const player_a,
					   unsigned const player_b)
     {
       this->send("players switched: "
		  + DK::Utils::String::to_string(player_a)
		  + " "
		  + DK::Utils::String::to_string(player_b));

       return ;
     } // void Interpreter::Sender::players_switched(unsigned player_a, unsigned player_b)

   /**
    ** send all rules
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
     Interpreter::Sender::rules()
     {
       ostringstream ostr;
       ostr << "rules\n";
       ostr << "{\n";
       ostr << this->connection().server().party().rule();
       ostr << "}\n";
       this->send_lines(ostr.str());

       return ;
     } // void Interpreter::Sender::rules()

   /**
    ** send the rule 'type'
    **
    ** @param     type   type to send
    **
    ** @return    -
    **
    ** @author    Diether Knof
    **
    ** @version   0.7.1
    **/
   void
     Interpreter::Sender::rule(int const type)
     {
       Rule const& rule = this->connection().server().party().rule();

       if (   (type >= Rule::BOOL_FIRST)
	   && (type <= Rule::BOOL_LAST) ) {
	 this->send("rule: " + ::name(static_cast<Rule::TypeBool>(type))
		    + " = " + (rule(static_cast<Rule::TypeBool>(type))
			       ? "1" : "0") );
       } else if (   (type >= Rule::UNSIGNED_FIRST)
		  && (type <= Rule::UNSIGNED_LAST) ) {
	 this->send("rule: " + ::name(static_cast<Rule::TypeUnsigned>(type))
		    + " = " + DK::Utils::String::to_string(rule(static_cast<Rule::TypeUnsigned>(type)))
		   );
       } else if (type == Rule::COUNTING) {
	 this->send("rule: " + ::name(Rule::COUNTING)
		    + " = " + ::name(rule(Rule::COUNTING)) );
       } else {
	 cerr << "rule '" << type << "' unknown" << endl;
       } // if (type == ...)

       return ;
     } // void Interpreter::Sender::rule(int type)

   /**
    ** the party is started
    **
    ** @param     -
    **
    ** @return    -
    **
    ** @author    Diether Knof
    **
    ** @version   0.7.3
    **/
   void
     Interpreter::Sender::party_start()
     {
       this->send("party: seed: "
		  + DK::Utils::String::to_string(this->connection().server().party().real_seed_first()));
       this->send("party: startplayer: "
		  + DK::Utils::String::to_string(this->connection().server().party().real_startplayer_first()));
       this->send("party: start");

       return ;
     } // void Interpreter::Sender::party_start()

   /**
    ** a new round is started
    ** do nothing
    **
    ** @param     roundno   number of round to start
    **
    ** @return    -
    **
    ** @author    Diether Knof
    **
    ** @version   0.7.2
    **/
   void
     Interpreter::Sender::party_start_round(unsigned const roundno)
     {
       this->send("party: start round: "
		  + DK::Utils::String::to_string(roundno));

       return ;
     } // void Interpreter::Sender::party_start_round(unsigned roundno)

   /**
    ** the party is finished
    **
    ** @param     -
    **
    ** @return    -
    **
    ** @author    Diether Knof
    **
    ** @version   0.7.2
    **/
   void
     Interpreter::Sender::party_finish()
     {
       this->send("party: finish");

       return ;
     } // void Interpreter::Sender::party_finish()

  } // namespace FreeDoko
} // namespace Network

#endif // #ifdef USE_NETWORK
