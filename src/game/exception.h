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

#ifndef CLASS_INVALD_GAME_EXCEPTION_HEADER
#define CLASS_INVALD_GAME_EXCEPTION_HEADER

#include "../exception.h"

class InvalidGameException : public StdException {
  public:
    InvalidGameException() throw() :
      StdException("invalid game")
      { }
    InvalidGameException(InvalidGameException const& exception) :
      StdException(exception)
      { }

    virtual ~InvalidGameException() throw() { }

  private: // unused
    InvalidGameException& operator=(InvalidGameException const&);
}; // class InvalidGameException : public StdException

#endif // #ifndef CLASS_INVALID_GAME_EXCEPTION_HEADER
