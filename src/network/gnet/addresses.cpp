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

#include "addresses.h"

namespace Network {
  namespace GNet {

    /**
     ** -> result
     **
     ** @param     -
     **
     ** @return    the names of the addresses of this computer
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.2
     **/
    vector<string>
      local_addresses()
      {
	set<string> addresses;

	{
	  GList* interfaces = gnet_inetaddr_list_interfaces();
	  for (GList* i = g_list_first(interfaces);
	       i != NULL;
	       i = g_list_next(i)) {
	    GInetAddr* const address = static_cast<GInetAddr*>(i->data);
	    addresses.insert(gnet_inetaddr_get_canonical_name(address));
#ifndef WINDOWS
	    addresses.insert(gnet_inetaddr_get_name(address));
#endif
	  }
	  g_list_free(interfaces);
	}

	{ // localhost
	  GInetAddr* const address = gnet_inetaddr_get_host_addr();
	  if (address) {
	    addresses.insert(gnet_inetaddr_get_canonical_name(address));
#ifndef WINDOWS
	    addresses.insert(gnet_inetaddr_get_name(address));
#endif
	  }
	} // localhost

	addresses.erase(string());
	addresses.erase("127.0.0.1");
	addresses.erase("localhost");

	return vector<string>(addresses.begin(), addresses.end());
      } // vector<string> local_addresses()

  } // namespace GNet

} // namespace Network

#endif // #ifdef USE_NETWORK
