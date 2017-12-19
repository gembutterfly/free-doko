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

#ifndef GTKMM_GAME_REVIEW_HEADER
#define GTKMM_GAME_REVIEW_HEADER

#include "base.h"

class Game;
class Trick;

#include <gdk/gdk.h>

namespace UI_GTKMM_NS {
class GameFinished;

/**
 ** @brief	window for reviewing the game (trickwise)
 **/
class GameReview : public Base {
  public:
    // constructor
    GameReview(GameFinished* game_finished);
    // destructor
    ~GameReview() = default;

    // set the game
    void set_game(Game const& game);

    // the current trick
    ::Trick const& trick() const;
    // whether the trick is visible
    bool trick_visible() const;

    // go to the previous trick
    void previous_trick();
    // go to the following trick
    void following_trick();
    // go to a specific trick
    void set_trickno(unsigned trickno);

    // a key is pressed
    bool key_press(GdkEventKey const* key);

  private:
    PRIV_VAR_R(unsigned, trickno);

  private: // unused
    GameReview() = delete;
    GameReview(GameReview const&) = delete;
    GameReview& operator=(GameReview const&) = delete;
}; // class GameReview : public Base

} // namespace UI_GTKMM_NS

#endif // #ifdef GTKMM_GAME_REVIEW_HEADER

#endif // #ifdef USE_UI_GTKMM
