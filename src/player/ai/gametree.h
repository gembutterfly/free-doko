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

#ifndef CLASS_GAMETREE_HEADER
#define CLASS_GAMETREE_HEADER

#include "weighting.h"

#include "../../card/hand.h"
#include "../aiconfig.h"
#include "../rating/rating.h"

class VirtualGamesInterface;
typedef VirtualGamesInterface GametreeInterface;
class Trick;

/**
 ** tries all possibilities to play and take the card with the maximal worst card (max-min).
 ** In contrary to 'VirtualGames' a following player does not decide his
 ** card but all cards are tested.
 ** When used with heuristics, it is first testet, whether a heuristic does
 ** match, else all cards are tested.
 **
 ** @author     Diether Knof
 **
 ** @todo	use 'CardsInformation' for setting the hands in the recursion
 **/
class Gametree : public Weighting {
  public:
    // constructor
    Gametree(Ai const& ai);
    // destructor
    ~Gametree();


    // returns the weighting for the card 'card'
    int weighting(Card const& card) const;


  private:
    // calculates and sets the end depth
    void calculate_end_depth();

    // whether to use heuristics
    bool with_heuristics() const;
    // whether the teampartner plays for the player
    bool for_team() const;

#ifdef WORKAROUND
    // for threads
  public:
#endif
    int rated_modus(Game& virt_game, unsigned const tricks_to_calc) const;
    int full_trick_modi(Game& virt_game, unsigned const tricks_to_calc) const;

  private:
    // how many tricks to calc in the future
    PRIV_VAR_R(unsigned, end_depth);
    // whether to use heuristics
    PRIV_VAR_R(AiType, aitype);
    // the future limit
    PRIV_VAR_R(Rating::Type, rating);

  private: // not implemeted
    Gametree();
    Gametree(Gametree const&);
    Gametree& operator=(Gametree const&);
}; // class Gametree : public Weighting

#endif // #ifndef CLASS_GAMETREE_HEADER
