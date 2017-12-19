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

#ifndef CLASS_GAMEPLAY_ACTION_PURE_HEADER
#define CLASS_GAMEPLAY_ACTION_PURE_HEADER

#include "../gameplay_action.h"

/**
 ** base class for pure gameplay actions
 ** t.i. there is no player who has initiated the action
 ** example: PovertyDeniedByAll
 **/
class GameplayAction::Pure : public GameplayAction {
  public:
    // constructor
    Pure(Type const& type);
    // constructor
    Pure(Type const& type, string const& line);

    // copy constructor
    Pure(Pure const& action);
    // copy operator
    Pure& operator=(Pure const& action);

    // destructor
    ~Pure();

    // write the action into the output stream
    void write(ostream& ostr) const;

    // a translation representing the data
    Translator::Translation data_translation() const;

  private: // unused
    Pure();
}; // class GameplayAction::Pure : public GameplayAction

// macro for simple child classes construction
//
// @param   ActionClassName   name for the class as subclass of 'GameplayAction'
// @param   ClassName         name for the class
// @param   type              type of the action
//
// example: GAMEPLAY_ACTION_PURE(GameplayAction::Swines, Swines, SWINES)
//
// @bug     'GameplayAction:: ## ClassName' does not work
#define GAMEPLAY_ACTION_PURE(ActionClassName, ClassName, type) \
  class ActionClassName : public GameplayAction::Pure { \
    public:  \
      ClassName()                   : Pure(type)       { } \
      ClassName(string const& line) : Pure(type, line) { } \
      ClassName(ClassName const& action) : Pure(action)         { } \
      ClassName& operator=(ClassName const& action) \
        { this->Pure::operator=(action); return *this; } \
      ~ClassName() { } \
  };

#endif // #ifndef CLASS_GAMEPLAY_ACTION_PURE_HEADER
