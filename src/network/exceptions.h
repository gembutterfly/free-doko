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

#ifndef NETWORK_EXCEPTIONS_HEADER
#define NETWORK_EXCEPTIONS_HEADER

#include "../exception.h"

namespace Network {

  class ServerFailureException : public StdException {
    public:
      ServerFailureException() throw() :
	StdException("connection failure")
      { }
      ServerFailureException(ServerFailureException const& exception) :
	StdException(exception)
      { }

      virtual ~ServerFailureException() throw() { }

    private: // unused
      ServerFailureException& operator=(ServerFailureException const&);
  }; // class ServerFailureException : public StdException

  class ListenerFailureException : public StdException {
    public:
      ListenerFailureException() throw() :
	StdException("connection failure")
      { }
      ListenerFailureException(ListenerFailureException const& exception) :
	StdException(exception)
      { }

      virtual ~ListenerFailureException() throw() { }

    private: // unused
      ListenerFailureException& operator=(ListenerFailureException const&);
  }; // class ListenerFailureException : public StdException

  class ConnectionFailureException : public StdException {
    public:
      ConnectionFailureException() throw() :
	StdException("connection failure")
      { }
      ConnectionFailureException(ConnectionFailureException const& exception) :
	StdException(exception)
      { }

      virtual ~ConnectionFailureException() throw() { }

    private: // unused
      ConnectionFailureException& operator=(ConnectionFailureException const&);
  }; // class ConnectionFailureException : public StdException

} // namespace Network

#endif // #ifndef NETWORK_EXCEPTIONS_HEADER

#endif // #ifdef USE_NETWORK
