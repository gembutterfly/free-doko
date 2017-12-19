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
#include "types.h"

namespace Network {

  /**
   ** -> result
   **
   ** @param     type   the traffic type
   **
   ** @return    the name of the traffic type
   **
   ** @author    Diether Knof
   **
   ** @version   0.7.4
   **/
  string
    name(TrafficType const& type)
    {
      switch (type) {
      case TRAFFIC_CONNECTION:
	return "connection";
      case TRAFFIC_DISCONNECTION:
	return "disconnection";
      case TRAFFIC_INCOMING:
	return "incoming";
      case TRAFFIC_OUTGOING:
	return "outgoing";
      } // switch (type)
      return "";
    } // string name(TrafficType type)

  /**
   ** writes the name of the traffic type in the output stream
   **
   ** @param     ostr   output stream
   ** @param     type   the traffic type
   **
   ** @return    the output stream
   **
   ** @author    Diether Knof
   **
   ** @version   0.7.4
   **/
  ostream&
    operator<<(ostream& ostr, TrafficType const& type)
    {
      return (ostr << name(type));
    } // ostream& operator<<(ostream& ostr, TrafficType type)

} // namespace Network

#endif // #ifdef USE_NETWORK
