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

#ifndef CLASS_STATUS_MESSAGE_HEADER
#define CLASS_STATUS_MESSAGE_HEADER

#include "../basistypes.h"

#include "../misc/translation.h"
class UI;

#include <stack>

/**
 ** status message
 ** adds a status message as long as the object exists
 **
 ** @author   Diether Knof
 **/
class StatusMessage {
  public:
    // Constructor
    StatusMessage(UI& ui, Translator::Translation const& translation);
    // Destructor
    ~StatusMessage();
  private:
    UI* const ui;
    Translator::Translation const t;
  private: // unused
    StatusMessage();
}; // class StatusMessage

/**
 ** list of status messages
 **
 ** @author   Diether Knof
 **/
class StatusMessages : private std::stack<StatusMessage*> {
  public:
    // Constructor
    StatusMessages(UI& ui);
    // Destructor
    ~StatusMessages();

    // add a status massage
    void add(Translator::Translation const& translation);
  private:
    UI* const ui;
}; // class StatusMessages

#endif // #ifndef CLASS_STATUS_MESSAGE_HEADER
