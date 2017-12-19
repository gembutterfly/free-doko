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

#ifndef CLASS_GAMEPLAY_ACTION_HEADER
#define CLASS_GAMEPLAY_ACTION_HEADER

#include "../misc/translation.h"

class GameplayAction {
  public:
    enum Type {
      // CARDS_DISTRIBUTED,
      RESERVATION,
      POVERTY_SHIFT,
      POVERTY_ACCEPTED,
      POVERTY_RETURNED,
      POVERTY_DENIED,
      POVERTY_DENIED_BY_ALL,
      CARD_PLAYED,
      ANNOUNCEMENT,
      SWINES,
      HYPERSWINES,
      MARRIAGE,
      GENSCHER,
      TRICK_OPEN,
      TRICK_FULL,
      TRICK_TAKEN,
      CHECK,
      PRINT
    }; // enum Type
    enum Discrepancy {
      FUTURE = -1,
      NONE = 0,
      SKIPPED = 1,
      PLAYER,
      CARD,
      OTHER
    }; // enum Discrepancy

    // base classes
    class Pure;
    class Player1;
    class BaseCards;

    // real actions
    class Reservation;
    class Marriage;
    class Genscher;
    class PovertyShift;
    class PovertyAccepted;
    class PovertyReturned;
    class PovertyDenied;
    class PovertyDeniedByAll;
    class Announcement;
    class Swines;
    class Hyperswines;
    class CardPlayed;
    class TrickOpen;
    class TrickFull;
    class TrickTaken;
    class Check;
    class Print;

  public:
    // read the data from the line
    static GameplayAction* new_(string const& line);
    // read the data from the line
    static GameplayAction* new_(string const& line, istream& istr);

  protected:
    // constructor
    GameplayAction(Type const type, unsigned const playerno);

    // copy constructor
    GameplayAction(GameplayAction const& action); // default
    // copy operator
    GameplayAction& operator=(GameplayAction const& action); // default

  public:
    // destructor
    virtual ~GameplayAction();

    // create a clone
    GameplayAction* clone() const;

    // whether the action is equal to this
    virtual bool equal(GameplayAction const& action) const;

    // write the action into the output stream
    virtual void write(ostream& ostr) const = 0;

    // a string representing the action
    virtual string str() const;

    // a translation representing the data
    virtual Translator::Translation data_translation() const = 0;

  public:
    Type type; // which action
    unsigned player; // which player has made the action

  private: // unused
    GameplayAction();
}; // class GameplayAction

// the name of the action type
string name(GameplayAction::Type const type);
// write the type of the action in the output stream
ostream& operator<<(ostream& ostr, GameplayAction::Type const type);
// the name of the discrepancy
string name(GameplayAction::Discrepancy const discrepancy);
// write the discrepancy in the output stream
ostream& operator<<(ostream& ostr,
		    GameplayAction::Discrepancy const discrepancy);

// write the actio in the output stream
ostream& operator<<(ostream& ostr, GameplayAction const& action);

// compares the two actions
bool operator==(GameplayAction const& action_a, GameplayAction const& action_b);
// compares the two actions
bool operator!=(GameplayAction const& action_a, GameplayAction const& action_b);

inline
Translator::Translation translation(GameplayAction::Type const& action_type)
{ return Translator::Translation(::translator,
				 "GameplayAction::"
				 + ::name(action_type)); }

#endif // #ifndef CLASS_GAMEPLAY_ACTION_HEADER
