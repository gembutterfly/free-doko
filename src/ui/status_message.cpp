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

#include "status_message.h"
#include "ui.h"

/** Constructor
 **
 ** @param     ui            corresponding ui
 ** @param     translation   text for the status message
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.14
 **/
StatusMessage::StatusMessage(UI& ui,
                             Translator::Translation const& translation) :
  ui(&ui), t(translation)
{ this->ui->add_status_message(this->t); }

/** Destructor
 **
 ** @param     -
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.14
 **/
StatusMessage::~StatusMessage()
{ this->ui->remove_status_message(this->t); }

/** Constructor
 **
 ** @param     ui            corresponding ui
 ** @param     translation   text for the status message
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.14
 **/
StatusMessages::StatusMessages(UI& ui) :
  ui(&ui)
{ }

/** Destructor
 **
 ** @param     -
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.14
 **/
StatusMessages::~StatusMessages()
{ 
  while (!this->empty()) {
    delete this->top();
    this->pop();
  }
} // StatusMessages::~StatusMessages()

/** add a status message
 **
 ** @param     translation    text to add
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.14
 **/
void
StatusMessages::add(Translator::Translation const& translation)
{
  this->push(new StatusMessage(*this->ui, translation));
}
