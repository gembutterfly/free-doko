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

#ifndef CLASS_GAMEPLAY_ACTION_GENSCHER_HEADER
#define CLASS_GAMEPLAY_ACTION_GENSCHER_HEADER

#include "player1.h"

class GameplayAction::Genscher : public GameplayAction::Player1 {
  public:
    // constructor
    Genscher(unsigned const playerno, unsigned const partner);
    // constructor
    Genscher(string line);

    // copy constructor
    Genscher(Genscher const& genscher);
    // copy operator
    Genscher& operator=(Genscher const& genscher);

    // destructor
    ~Genscher();

    // whether the action is equal to this
    bool equal(GameplayAction const& action) const;

    // write the action into the output stream
    void write(ostream& ostr) const;

    // a translation representing the data
    Translator::Translation data_translation() const;

  public:
    // the genscher partner
    unsigned partner;

  private: // unused
    Genscher();
}; // class GameplayAction::Genscher : public GameplayAction::Player1

#endif // #ifndef CLASS_GAMEPLAY_ACTION_GENSCHER_HEADER
