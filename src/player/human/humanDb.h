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


#ifndef CLASS_HUMANDB_HEADER
#define CLASS_HUMANDB_HEADER

#ifdef POSTPHONED

#include "../playersDb.h"

class HumanDb : public PlayersDb
{
  public:
    HumanDb() :
      PlayersDb(),
    description_(),
    private_( false ),
    pixmap_( "" )  
    {
    } 

    HumanDb( HumanDb& h ) :
      PlayersDb( h ),
    description_( h.description_ ),
    private_( h.private_ ),
    pixmap_( h.pixmap_ )  
    {
    } 

    /// short description of this player
    std::string description()
    { return description_; }

    /// if is values should be accesible for other players
    bool isPrivate()
    { return private_; }

    /// a picture 
    char* pixmap()
    { return pixmap_; }

  private:
    std::string description_;    
    bool private_;
    char* pixmap_;
}; // class HumanDb : public PlayersDb

#endif // #ifdef POSTPHONED

#endif // #ifndef CLASS_HUMANDB_HEADER
