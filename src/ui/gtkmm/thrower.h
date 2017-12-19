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

#ifdef USE_UI_GTKMM

#ifndef GTKMM_THROWER_HEADER
#define GTKMM_THROWER_HEADER

#include "../../basistypes.h"
#include "../../card/card.h"

namespace UI_GTKMM_NS {

/**
 ** @brief	a thrower for exceptions
 **
 ** since gtkmm does not support exceptions, this class delays the throwing
 ** of exceptions
 **
 ** @author	Diether Knof
 **/
class Thrower {
  public:
    // throwing type
    enum Type {
      NONE,
      GAMESTATUS,
      CARD
    }; // enum Type

    // exception data
    union Exception {
      GameStatus game_status;
      Card const* card;
    }; // union Exception

  public:
    // constructor
    Thrower();
    // destructor
    ~Thrower();

    // whether an exception will be thrown
    operator bool() const;
    // whether an exception will be thrown
    bool will_throw() const;
    // throws the exception
    void throw_it() throw(GameStatus, Card);
    // removes the exception
    void clear();

    // increment the depth
    void inc_depth();
    // decrement the depth, throw when '0' is reached
    void dec_depth();
    // set the depth -- only use if you know exactly why you need it!
    void set_depth(unsigned depth);

    // saves the exception to throw later
    void operator()(GameStatus gamestatus,
                    string const& file,
                    unsigned line);
    void operator()(Card const& card,
                    string const& file,
                    unsigned line);

  private:
    // the exception to throw
    PRIV_VAR_R(Exception, exception);
    // the type of exception
    PRIV_VAR_R(Type, exception_type);

    // the depth in the ui calls
    PRIV_VAR_R(unsigned, depth);

    // the following variables are used for bug tracking (double throwing)
    // the file of the throw
    string file;
    // the line number of the throw
    unsigned line = 0;
}; // class Thrower

} // namespace UI_GTKMM_NS

#endif // #ifdef GTKMM_THROWER_HEADER

#endif // #ifdef USE_UI_GTKMM
