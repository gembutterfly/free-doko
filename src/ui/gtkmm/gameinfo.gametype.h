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

#ifndef GTKMM_GAMEINFO_DIALOG_INFORMATION_GAMETYPE_HEADER
#define GTKMM_GAMEINFO_DIALOG_INFORMATION_GAMETYPE_HEADER

#include "gameinfo.h"

class Game;

namespace UI_GTKMM_NS {

/**
 ** @brief	gametype information
 **
 ** @author	Diether Knof
 **/
class GameInfoDialog::Information::GameType
  : public GameInfoDialog::Information
{
  public:
    // constructor
    GameType(Base* parent, Game const& game);
    // destructor
    ~GameType();

  private:
    // update the text
    void update_texts();

    // whether the information is blocking the gameplay
    bool is_blocking() const;

  private:
    // the corresponding game
    Game const* game;

  private: // unused
    GameType() = delete;
    GameType(GameType const&) = delete;
    GameType& operator=(GameType const&) = delete;
}; // class GameInfoDialog::Information::GameType : public GameInfoDialog::Information

} // namespace UI_GTKMM_NS

#endif // #ifdef GTKMM_GAMEINFO_DIALOG_INFORMATION_GAMETYPE_HEADER

#endif // #ifdef USE_UI_GTKMM
