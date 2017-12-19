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

#ifndef CLASS_GAMEPLAY_ACTION_PLAYER1_HEADER
#define CLASS_GAMEPLAY_ACTION_PLAYER1_HEADER

#include "../gameplay_action.h"

/**
 ** base class for gameplay action of one player
 ** t.i. there is one player who has initiated the action
 ** example: Swines
 **/
class GameplayAction::Player1 : public GameplayAction {
  public:
    // constructor
    Player1(Type const& type, unsigned const playerno);
    // constructor
    Player1(Type const& type, string const& line);

    // copy constructor
    Player1(Player1 const& action);
    // copy operator
    Player1& operator=(Player1 const& action);

    // destructor
    ~Player1();

    // write the action into the output stream
    virtual void write(ostream& ostr) const;

    // a translation representing the data
    virtual Translator::Translation data_translation() const;

  private: // unused
    Player1();
}; // class GameplayAction::Player1 : public GameplayAction

// macro for simple child classes construction
//
// @param   ActionClassName   name for the class as subclass of 'GameplayAction'
// @param   ClassName         name for the class
// @param   type              type of the action
//
// example: GAMEPLAY_ACTION_PLAYER1(GameplayAction::PovertyDeniedByAll, PovertyDeniedByAll, POVERTY_DENIED_BY_ALL)
//
// @bug     'GameplayAction:: ## ClassName' does not work
#define GAMEPLAY_ACTION_PLAYER1(ActionClassName, ClassName, type) \
  class ActionClassName : public GameplayAction::Player1 { \
    public:  \
      ClassName(unsigned const playerno) : Player1(type, playerno) { } \
      ClassName(string const& line)      : Player1(type, line)     { } \
      ClassName(ClassName const& action) : Player1(action)         { } \
      ClassName& operator=(ClassName const& action) \
        { this->Player1::operator=(action); return *this; } \
      ~ClassName() { } \
  };

#endif // #ifndef CLASS_GAMEPLAY_ACTION_PLAYER1_HEADER
