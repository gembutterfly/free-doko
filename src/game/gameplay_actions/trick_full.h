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

#ifndef CLASS_GAMEPLAY_ACTION_TRICK_FULL_HEADER
#define CLASS_GAMEPLAY_ACTION_TRICK_FULL_HEADER

#include "../gameplay_action.h"
#include "../../card/trick.h"

class GameplayAction::TrickFull : public GameplayAction {
  public:
    // constructor
    TrickFull(Trick const& trick);
    // constructor
    TrickFull(string const& line);
    // constructor
    TrickFull(string const& line, istream& istr);

    // copy constructor
    TrickFull(TrickFull const& trick_full);
    // copy operator
    TrickFull& operator=(TrickFull const& trick_full);

    // destructor
    ~TrickFull();

    // whether the action is equal to this
    bool equal(GameplayAction const& action) const;

    // write the action into the output stream
    void write(ostream& ostr) const;

    // a translation representing the data
    Translator::Translation data_translation() const;

  public:
    // the trick
    Trick trick;

  private: // unused
    TrickFull();
}; // class GameplayAction::TrickFull : public GameplayAction

#endif // #ifndef CLASS_GAMEPLAY_ACTION_TRICK_FULL_HEADER
