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

#include "connection.h"
#include "../server.h"
#include "../types.h"

namespace Network {
  namespace File {

    /**
     ** constructor
     **
     ** @param     server    the corresponding server
     ** @param     filename  file to load the traffic from
     ** @param     port      0
     ** @param     type      interpreter type
     **
     ** @return    -
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.13
     **/
    Connection::Connection(::Network::Server& server,
                           string const& filename, unsigned const port,
                           InterpreterType const type) :
      Network::Connection(server, filename, port, type),
      istr(new ifstream(filename.c_str()))
    {
      this->set_relation(PARENT);
    } // Connection::Connection(Server& server, string address, unsigned port, InterpreterType type)

    /**
     ** destructor
     **
     ** @param     -
     **
     ** @return    -
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.13
     **/
    Connection::~Connection()
    {
      delete istr;
    } // Connection::~Connection()

    /**
     ** check for network traffic
     **
     ** @param     -
     **
     ** @return    -
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.13
     **/
    void
      Connection::update()
      {
        if (istr->good() && !istr->eof()) {
          string line;
          std::getline(*istr, line);
          if (istr->good() || istr->eof())
            this->received(line);
          if (istr->eof())
            this->received("EOF");
        } // if (istr)
        return ;
      } // void Connection::update()

  } // namespace File
} // namespace Network

#endif // #ifdef USE_NETWORK
