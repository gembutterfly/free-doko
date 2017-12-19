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

#ifdef USE_NETWORK
#include "network_log.h"

namespace OS_NS {

  /**
   ** constructor
   **
   ** @param	-
   **
   ** @return	-
   **
   ** @author	Diether Knof
   ** 
   ** @version	0.7.13
   **/
  NetworkLog::NetworkLog() :
    OS(OS_TYPE::NETWORK_LOG),
    outgoing(new ofstream("network.out")),
    incoming(new ofstream("network.in")),
    full(new ofstream("network.full"))
  {
    return ;
  } // NetworkLog::NetworkLog()

  /**
   ** destructor
   **
   ** @param	-
   **
   ** @return	-
   **
   ** @author	Diether Knof
   ** 
   ** @version	0.7.13
   **/
  NetworkLog::~NetworkLog()
  {
    return ;
  } // NetworkLog::~NetworkLog()

  /**
   ** there was network traffic over the connection 'connection'
   **
   ** @param     connection   connection with the traffic
   ** @param     traffic_type   the traffic type
   ** @param     text   	send text
   **
   ** @return    -
   **
   ** @author    Diether Knof
   **
   ** @version   0.7.13
   **/
  void
    NetworkLog::network_traffic(::Network::Connection const& connection,
                                ::Network::TrafficType const traffic_type,
                                string const& text)
    {
      if (    this->outgoing
          && (traffic_type == Network::TRAFFIC_OUTGOING))
        *this->outgoing << text << endl;
      if (    this->incoming
          && (traffic_type == Network::TRAFFIC_INCOMING))
        *this->incoming << text << endl;
      if (    this->full
          && (traffic_type == Network::TRAFFIC_OUTGOING))
        *this->full << "> " << text.size() << ' ' << text << '\n' << endl;
      if (    this->full
          && (traffic_type == Network::TRAFFIC_INCOMING))
        *this->full << "< " << text.size() << ' ' << text << '\n' << endl;
      return ;
    }

} // namespace OS_NS

#endif // #ifdef USE_NETWORK
