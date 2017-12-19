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

#ifndef CLASS_W_VIRTUAL_GAMES_HEADER
#define CLASS_W_VIRTUAL_GAMES_HEADER


#include "weighting.h"

#include "../../card/hand.h"

class VirtualGamesInterface;
class Player;
class Trick;

/// create for each card a virtual game and look, how many points the ai makes
class WVirtualGames : public Weighting {
  public:
    // constructor
    WVirtualGames(Ai const& ai);
    // destructor
    ~WVirtualGames();

    // returns the weighting for the card 'card'
    int weighting(Card const& card) const;


  private:
    // calculates the end depth
    void calculate_end_depth();

  private:
    // the future limit
    PRIV_VAR_R(unsigned, future_limit);
    // how many tricks to calc in the future
    PRIV_VAR_R(unsigned, end_depth);

  private: // not implemeted
    WVirtualGames();
    WVirtualGames(WVirtualGames const&);
    WVirtualGames& operator=(WVirtualGames const&);
}; // class WVirtualGames : public Weighting

#endif // #ifndef CLASS_W_VIRTUAL_GAMES_HEADER
