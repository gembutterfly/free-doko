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

#include "constants.h"

#ifdef USE_SOUND_COMMAND

#include "command.h"
#include "../misc/setting.h"

#ifndef PLAY_SOUND_COMMAND
#ifdef OSX
#define PLAY_SOUND_COMMAND "afplay"
#else
#define PLAY_SOUND_COMMAND "aplay"
#endif
#endif
//#define PLAY_SOUND_COMMAND "ogg123"

/**
 ** constructor
 **
 ** @param     -
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
Sound::Command::Command() :
  Sound()
{ }

/**
 ** destructor
 **
 ** @param     -
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
Sound::Command::~Command()
{ }


/**
 ** play the sound (global group)
 **
 ** @param     position   position of the sound
 ** @param     voice      voice (directory to take the sound from)
 ** @param     type       sound to play
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
void
Sound::Command::play(Position const& position,
                   string const& voice, Type const type)
{
  string const path = Sound::path(position, voice, type);
  if (path.empty())
    return ;

  system((PLAY_SOUND_COMMAND
          + (" " + Sound::path(position, voice, type))
          + " >/dev/null &").c_str());

  return ;
} // void Sound::Command::play(Position position, string voice, Type text)

/**
 ** -> result
 ** @param     -
 **
 ** @return    whether the sound is still playing
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **
 ** @todo      all
 **/
bool
Sound::Command::playing() const
{
  return false;
} // bool Sound::Command::playing() const

#endif // #ifdef USE_SOUND_COMMAND
