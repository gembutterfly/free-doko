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

#ifndef CLASS_GAMEPLAY_ACTION_BASE_CARDS_HEADER
#define CLASS_GAMEPLAY_ACTION_BASE_CARDS_HEADER

#include "player1.h"
#include "../../card/card.h"

class GameplayAction::BaseCards : public Player1 {
  public:
    // constructor
    BaseCards(Type const& type,
	      unsigned const playerno, vector<Card> const& cards);
    // constructor
    BaseCards(Type const& type, string line);

    // copy constructor
    BaseCards(BaseCards const& base_cards);
    // copy operator
    BaseCards& operator=(BaseCards const& base_cards);

    // destructor
    ~BaseCards();

    // whether the action is equal to this
    bool equal(GameplayAction const& action) const;

    // write the action into the output stream
    void write(ostream& ostr) const;

    // a translation representing the data
    Translator::Translation data_translation() const;

  public:
    // the shifted cards
    vector<Card> cards;

  private: // unused
    BaseCards();
}; // class GameplayAction::BaseCards : public Player1

// macro for simple child classes construction
//
// @param   ActionClassName   name for the class as subclass of 'GameplayAction'
// @param   ClassName         name for the class
// @param   type              type of the action
//
// example: GAMEPLAY_ACTION_BASE_CARDS(GameplayAction::PovertyShift, PovertyShift, POVERTY_SHIFT)
//
// @bug     'GameplayAction:: ## ClassName' does not work
#define GAMEPLAY_ACTION_BASE_CARDS(ActionClassName, ClassName, type) \
  class ActionClassName : public GameplayAction::BaseCards { \
    public:  \
      ClassName(unsigned const playerno, vector<Card> const& cards) : \
        BaseCards(type, playerno, cards) { } \
      ClassName(string const& line)      : BaseCards(type, line)     { } \
      ClassName(ClassName const& action) : BaseCards(action)         { } \
      ClassName& operator=(ClassName const& action) \
        { this->BaseCards::operator=(action); return *this; } \
      ~ClassName() { } \
  };

#endif // #ifndef CLASS_GAMEPLAY_ACTION_BASE_CARDS_HEADER
