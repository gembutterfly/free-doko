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

#ifndef CLASS_GAMEPLAY_HEADER
#define CLASS_GAMEPLAY_HEADER

#include "../os/os.h"

class Card;
class Hand;
class Game;
class Player;
class GameplayAction;

// The gameplay, saved as actions
class Gameplay : public OS {
  friend class Game;
  public:
    // constructor
    Gameplay();
    // destructor
    ~Gameplay();

    // write the gameplay in the output stream
    ostream& write(ostream& ostr) const;

    // the parts of a game

    // a game is opened
    void game_open(Game const& game);
    // the cards are distributed
    void game_cards_distributed();
    // the game is redistributed
    void game_redistribute();
    // the game is started
    void game_start();
    // the game is closed
    void game_close();

    // a gameplay action has happened
    void gameplay_action(GameplayAction const& action);

  private:
    // the seed
    PRIV_VAR_R(unsigned, seed);
    // the game type
    PRIV_VAR_R(GameType, gametype);
    // the solo player
    PRIV_VAR_R(unsigned, soloplayer);
    // the hands of the players
    PRIV_VAR_R(vector<Hand>, hands);
    // the gameplay actions
    PRIV_VAR_R(vector<GameplayAction*>, actions);

  private: // not implemented
    Gameplay(Gameplay const&);
    Gameplay& operator=(Gameplay const&);
}; // class Gameplay

// write the gameplay in the output stream
ostream& operator<<(ostream& ostr, Gameplay const& gameplay);

#endif // #ifndef CLASS_GAMEPLAY_HEADER
