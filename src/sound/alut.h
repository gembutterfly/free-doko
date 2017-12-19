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

#ifdef USE_SOUND_ALUT

#ifndef CLASS_SOUND_ALUT_HEADER
#define CLASS_SOUND_ALUT_HEADER

#include "sound.h"

#include <AL/alut.h>

/**
 ** @brief	the sound class using the program 'aplay'
 **
 ** @author	Diether Knof
 **/
class Sound::Alut : public Sound {
  private:
    // number of instances
    static unsigned instances_no;

  public:
    // constructor
    Alut();
    // destructor
    ~Alut();


    // play the sound of the soundgroup
    void play(Position const& position,
              string const& soundgroup,
              Type const type);
    // whether the sound is still playing
    bool playing() const;

    // update the current playing source
    void update();

  private:
    // check the current sound
    void check_current_sound();
    // play the next file
    void play_next_sound();

  private:
    // current playing source
    ALuint current_source_;
    // list with files to play
    queue<string> file_queue_;


  private: // unused
    Alut(Alut const&);
    Alut& operator=(Alut const&);
}; // class Alut : public Sound

#endif // #ifndef CLASS_SOUND_ALUT_HEADER

#endif // #ifdef USE_SOUND_ALUT
