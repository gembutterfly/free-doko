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
   ** change the rule
   **
   ** @param     text   'rule = value'
   **
   ** @return    whether the rule was changed
   **
   ** @author    Diether Knof
   **
   ** @version   0.7.2
   **/
  bool
    Connection::Actor::set_rule(string const& text)
    {
      if (this->connection().relation() != Connection::PARENT)
	return false;

      Config config = Config_(text);
      // Bug: const cast
      return const_cast<Party&>(this->party()).rule().set(config.name, config.value);
    } // bool Connection::Actor::set_rule(string text)

  /**
   ** change the rule
   **
   ** @param     type      rule type
   ** @param     value   new value
   **
   ** @return    whether the rule was changed
   **
   ** @author    Diether Knof
   **
   ** @version   0.7.2
   **/
  bool
    Connection::Actor::set_rule(int const type, int const value)
    {
      if (this->connection().relation() != Connection::PARENT)
	return false;

      // Bug: const cast
      if (   (type >= Rule::BOOL_FIRST)
	  && (type <= Rule::BOOL_LAST) )
	const_cast<Party&>(this->party()).rule().set(static_cast<Rule::TypeBool>(type), value);
      else if (   (type >= Rule::UNSIGNED_FIRST)
	       && (type <= Rule::UNSIGNED_LAST) )
	const_cast<Party&>(this->party()).rule().set(static_cast<Rule::TypeUnsigned>(type), static_cast<unsigned>(value));
      else if (type == Rule::COUNTING)
	const_cast<Party&>(this->party()).rule().set(static_cast<Rule::TypeCounting>(type), static_cast<Counting>(value));
      else
	return false;
      return true;
    } // bool Connection::Actor::set_rule(string text)

} // namespace Network

#endif // #ifdef USE_NETWORK
