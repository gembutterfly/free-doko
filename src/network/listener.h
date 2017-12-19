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

#ifndef CLASS_NETWORK_LISTENER_HEADER
#define CLASS_NETWORK_LISTENER_HEADER

#include "server.h"

namespace Network {

  /**
   ** base class for a network listener
   ** the listener listens for connections
   **
   ** @author	Diether Knof
   **/
  class Listener {
    public:
      // returns a new listener
      static Listener* new_(Server& server, unsigned const port);

    public: // standard constructor
      Listener(Server& server, unsigned const port);
      // destructor
      virtual ~Listener();

      // check for network traffic
      virtual void update();

    protected:
      // the corresponding server
      Server* server;

    private:
      // port
      PRIV_VAR_R(unsigned, port);

    private: // unused
      Listener();
      Listener(Listener const&);
      Listener& operator=(Listener const&);
  }; // class Listener

} // namespace Network

#endif // #ifndef CLASS_NETWORK_SERVER_HEADER

#endif // #ifdef USE_NETWORK
