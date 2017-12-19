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

#ifndef CLASS_OS_NETWORK_LOG_HEADER
#define CLASS_OS_NETWORK_LOG_HEADER
#ifdef USE_NETWORK

#include "os.h"
#include "../network/types.h"

namespace OS_NS {

  /**
   ** a network logger (in a file)
   **
   ** @author	Diether Knof
   **
   ** @version	0.7.13
   **/
  class NetworkLog : public OS {
    public:
      NetworkLog();
      ~NetworkLog();

      // there was network traffic over the given connection
      void network_traffic(Network::Connection const& connection,
                           Network::TrafficType const traffic_type,
                           string const& text);

    private:
      ostream* outgoing; // outgoing traffic
      ostream* incoming; // incoming traffic
      ostream* full; // full traffic
    private: // not implemented
      NetworkLog(NetworkLog const&);
      NetworkLog& operator=(NetworkLog const&);

  }; // class NetworkLog : public OS

} // namespace OS_NS

#endif // #ifdef USE_NETWORK
#endif // #ifndef CLASS_OS_NETWORK_LOG_HEADER
