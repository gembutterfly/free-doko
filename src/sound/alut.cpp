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

#ifdef USE_SOUND_ALUT

#include "alut.h"
#include <cstring>

#include "../ui/ui.h"

#include "../utils/file.h"

// number of instances
unsigned Sound::Alut::instances_no = 0;

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
Sound::Alut::Alut() :
  Sound(),
  current_source_(0),
  file_queue_()
{
  if (Alut::instances_no == 0) {
    // the only option is the name of the program
      int argc_alut = 1;
      char** argv_alut = new char*[2];
      argv_alut[0] = static_cast<char*>(calloc(sizeof(char),
                                              strlen(PROGRAM_NAME) + 1));
      strcpy(argv_alut[0], PROGRAM_NAME);
      argv_alut[1] = NULL;

      alutInit(&argc_alut, argv_alut);
      if (int error = alutGetError())
        cerr << "ALUT error: " << alutGetErrorString(error) << endl;
  } // if (Alut::instances_no == 0)

#if 0
  { // print the supported audio file types
    const char * supports = alutGetMIMETypes(ALUT_LOADER_BUFFER);
    cout << supports << endl;
  } // print the supported audio file types
#endif


  Alut::instances_no += 1;

  return ;
} // Sound::Alut::Alut()

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
Sound::Alut::~Alut()
{
  Alut::instances_no -= 1;

  if (Alut::instances_no == 0) {
    alDeleteSources(1, &this->current_source_);
    alutExit ();
  }
} // Sound::Alut::~Alut()


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
Sound::Alut::play(Position const& position,
                  string const& voice,
                  Type const type)
{
  this->file_queue_.push(Sound::path(position, voice, type));

  return ;
} // void Sound::Alut::play(Position position, string voice, Type type)

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
Sound::Alut::playing() const
{
  ALint val;
  alGetSourcei(this->current_source_, AL_SOURCE_STATE, &val);
  return (val == AL_PLAYING);
} // bool Sound::Alut::playing() const

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
Sound::Alut::update()
{
  this->check_current_sound();

  return ;
} // void Sound::Alut::update()

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
Sound::Alut::check_current_sound()
{
  if (this->current_source_) {
    ALint val;
    alGetSourcei(this->current_source_, AL_SOURCE_STATE, &val);
    if (val != AL_PLAYING)
      this->current_source_ = 0;
  } // if (this->current_source_)

  if (!this->current_source_)
    this->play_next_sound();

  return ;
} // void Sound::Alut::check_current_sound()

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
Sound::Alut::play_next_sound()
{
  if (this->file_queue_.empty())
    return ;

  if (this->file_queue_.front() == "") {
    this->file_queue_.pop();
    this->play_next_sound();
    return ;
  }

  if (false)
  {
    ALuint helloBuffer, helloSource;
    helloBuffer = alutCreateBufferHelloWorld ();
    alGenSources (1, &helloSource);
    alSourcei (helloSource, AL_BUFFER, helloBuffer);
    alSourcePlay (helloSource);
    alutSleep (1);
    return ;
  }
  ALuint buffer, source;
  buffer = alutCreateBufferFromFile(this->file_queue_.front().c_str());
  if (int error = alutGetError()) {
    cerr << "ALUT error: " << alutGetErrorString(error) << endl;
    cerr << "Sound::Alut::play_next_sound()\n"
      << "  Sound file '" << this->file_queue_.front() << "' not found.\n"
      << "  Skipping it." << endl;
    this->file_queue_.pop();
    return ;
  }
  alGenSources(1, &source);
  alSourcei(source, AL_BUFFER, buffer);
  alSourcePlay(source);	

  this->current_source_ = source;

  this->file_queue_.pop();

  return ;
} // void Sound::Alut::play_next_sound()

#endif // #ifdef USE_SOUND_ALUT
