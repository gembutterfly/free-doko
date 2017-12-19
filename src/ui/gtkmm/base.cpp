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

#ifdef USE_UI_GTKMM

#include "base.h"

#include "ui.h"

#define PROGRESS_PER_BASE (1.0 / this->ui->base_objects_number)

namespace UI_GTKMM_NS {

  /**
   ** constructor
   **
   ** @param     parent   the parent object
   **
   ** @return    -
   **/
  Base::Base(Base* const parent) :
    Base(parent, parent->ui)
  { }

  /**
   ** constructor
   **
   ** @param     parent   the parent object
   ** @param     ui   the ui
   **
   ** @return    -
   **/
  Base::Base(Base* const parent, UI_GTKMM* const ui) :
    ui(ui),
    parent(parent)
  {
    if (this != this->ui) {
      this->ui->parts.push_back(this);
    if (   (::game_status == GAMESTATUS::PROGRAMSTART)
        || (::game_status == GAMESTATUS::PARTY_INITIAL_LOADED) )
      this->ui->add_progress(PROGRESS_PER_BASE);
    }

    return ;
  } // Base::Base(Base* const parent, UI_GTKMM* const ui)

  /**
   ** destruktor
   **
   ** @param     -
   **
   ** @return    -
   **/
  Base::~Base()
  {
    if (this != this->ui)
      this->ui->parts.erase(std::find(this->ui->parts.begin(),
                                      this->ui->parts.end(),
                                      this));

    return ;
  } // virtual Base::~Base()

  /**
   ** update the cards
   **
   ** @param     -
   **
   ** @return    -
   **
   **/
  void
    Base::cards_update()
    { }

  /**
   ** update the cards back
   **
   ** @param     -
   **
   ** @return    -
   **/
  void
    Base::cards_back_update()
    { }

  /**
   ** the setting has changed
   **
   ** @param     type   the type that has changed
   **
   ** @return    -
   **/
  void
    Base::setting_update(int const type)
    { }

  /**
   ** update the language elements
   **
   ** @param     -
   **
   ** @return    -
   **/
  void
    Base::language_update()
    { }

} // namespace UI_GTKMM_NS

#endif // #ifdef USE_UI_GTKMM
