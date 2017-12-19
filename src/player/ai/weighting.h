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

#ifndef CLASS_WEIGHTING_HEADER
#define CLASS_WEIGHTING_HEADER

#include "ai.h"
class Trick;

class Weighting {
  public:
    // returns the best card
    static Card best_card(AiType const aitype, Ai const& ai);

  public:
    // constructor
    Weighting(Ai const& ai);
    //destructor
    virtual ~Weighting();

    Ai const& ai() const { return this->ai_; }
    // the interface
    VirtualGamesInterface const& vgi() const;

    // the best card according to the weighting
    Card best_card() const;
#ifdef USE_THREADS
    // choose the best card for the ai (with thread support)
    Card best_card_threads() const;
#endif

    // the weighting
    virtual int weighting(Card const& card) const = 0;

  private:
    // the corresponding ai
    //PRIV_VAR_R(Ai, ai);
    Ai const& ai_;

  private: // unused
    Weighting();
    Weighting(Weighting const& weighting);
    Weighting& operator=(Weighting const& weighting);
}; // class Weighting

#endif // #ifndef CLASS_WEIGHTING_HEADER
