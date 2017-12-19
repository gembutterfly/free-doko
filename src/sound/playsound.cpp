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

#ifdef USE_SOUND_PLAYSOUND

#include "playsound.h"

/**
 ** constructor
 **
 ** @param     -
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.5
 **/
Sound::WinPlaySound::WinPlaySound() :
  Sound(),
  file_queue_()
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
 ** @version   0.7.5
 **/
Sound::WinPlaySound::~WinPlaySound()
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
 ** @version   0.7.5
 **/
void
Sound::WinPlaySound::play(Position const& position,
                          string const& voice,
                          Type const type)
{
  string const filename = Sound::filename(position, voice, type);
  if (!filename.empty())
    this->file_queue_.push(Sound::filename(position, voice, type));

  return ;
} // void Sound::WinPlaySound::play(Position position, string voice, Type type)

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
Sound::WinPlaySound::playing() const
{
  return (PlaySound(TEXT(""), NULL,
                    SND_FILENAME | SND_ASYNC | SND_NODEFAULT | SND_NOSTOP)
          == FALSE);
} // bool Sound::WinPlaySound::playing() const

/**
 ** update the current playing source
 **
 ** @param     -
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.5
 **/
void
Sound::WinPlaySound::update()
{
  this->check_current_sound();

  return ;
} // void Sound::WinPlaySound::update()

/**
 ** check the current sound
 **
 ** @param     -
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.5
 **/
void
Sound::WinPlaySound::check_current_sound()
{
  if (!this->playing())
    this->play_next_sound();

  return ;
} // void Sound::WinPlaySound::check_current_sound()

/**
 ** play the next sound
 **
 ** @param     -
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.5
 **/
void
Sound::WinPlaySound::play_next_sound()
{
  if (this->file_queue_.empty())
    return ;

  PlaySound(TEXT(this->file_queue_.front().c_str()), NULL,
            SND_FILENAME | SND_ASYNC | SND_NODEFAULT | SND_NOWAIT);

  this->file_queue_.pop();

  return ;
} // void Sound::WinPlaySound::play_next_sound()

#endif // #ifdef USE_SOUND_PLAYSOUND
