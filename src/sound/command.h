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
 *********************************************************************/

#ifdef USE_SOUND_COMMAND

#ifndef CLASS_SOUND_COMMAND_HEADER
#define CLASS_SOUND_COMMAND_HEADER

#include "sound.h"

/**
 ** @brief	the sound class using the program 'aplay'
 **
 ** @author	Diether Knof
 **/
class Sound::Command : public Sound {
  public:
    // constructor
    Command();
    // destructor
    ~Command();


    // play the sound of the voice
    void play(Position const& position,
              string const& voice,
              Type const type);
    // whether the sound is still playing
    bool playing() const;


  private: // unused
    Command(Command const&);
    Command& operator=(Command const&);
}; // class Command : public Sound

#endif // #ifndef CLASS_SOUND_COMMAND_HEADER

#endif // #ifdef USE_SOUND_COMMAND
