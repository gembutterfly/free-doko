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

#ifndef GTKMM_GAMEINFO_DIALOG_INFORMATION_ANNOUNCEMENT_HEADER
#define GTKMM_GAMEINFO_DIALOG_INFORMATION_ANNOUNCEMENT_HEADER

#include "gameinfo.h"

#include "../../basistypes.h"

namespace UI_GTKMM_NS {

/**
 ** @brief	announcement information
 **
 ** @author	Diether Knof
 **/
class GameInfoDialog::Information::Announcement
  : public GameInfoDialog::Information
{
  friend class GameInfoDialog;

  public:
  // constructor
  Announcement(Base* parent,
               Player const& player,
               ANNOUNCEMENT::Announcement announcement);
  // destructor
  ~Announcement();

  private:
  // update the text
  void update_texts();

  private:
  // the player
  Player const* player = nullptr;
  // the announcement
  ANNOUNCEMENT::Announcement const announcement;

  private: // unused
  Announcement() = delete;
  Announcement(Announcement const&) = delete;
  Announcement& operator=(Announcement const&) = delete;
}; // class GameInfoDialog::Information::Announcement : public GameInfoDialog::Information

} // namespace UI_GTKMM_NS

#endif // #ifdef GTKMM_GAMEINFO_DIALOG_INFORMATION_ANNOUNCEMENT_HEADER

#endif // #ifdef USE_UI_GTKMM
