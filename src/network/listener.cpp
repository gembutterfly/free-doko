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

#include "listener.h"

#include "gnet/listener.h"

#include "../ui/ui.h"
#include "../misc/translations.h"

namespace Network {

  /**
   ** constructor
   **
   ** @param     server   the corresponding server
   ** @param     port     port to listen on
   **
   ** @return    -
   **
   ** @author    Diether Knof
   **
   ** @version   0.7.2
   **/
  Listener*
    Listener::new_(Server& server, unsigned const port)
    {
      try {
	return new GNet::Listener(server, port);
      } catch (ListenerFailureException const&) {
	::ui->information(::translation("Error::could not create listener on port %uport%",
					port),
			  INFORMATION::PROBLEM);

	return NULL;
      }
      return NULL;
    } // static Listener* Listener::new_(Server& server, unsigned port)

  /**
   ** constructor
   **
   ** @param     server   corresponding server
   ** @param     port     port to listen on
   **
   ** @return    -
   **
   ** @author    Diether Knof
   **
   ** @version   0.7.2
   **/
  Listener::Listener(Server& server, unsigned const port) :
    server(&server),
    port_(port)
  { }

  /**
   ** destructor
   **
   ** @param     -
   **
   ** @return    -
   **
   ** @author    Diether Knof
   **
   ** @version   0.7.2
   **/
  Listener::~Listener()
  { }

  /**
   ** listen
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
    Listener::update()
    { }

} // namespace Network

#endif // #ifdef USE_NETWORK
