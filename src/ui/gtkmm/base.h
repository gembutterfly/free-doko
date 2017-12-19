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

#ifndef GTKMM_BASE_HEADER
#define GTKMM_BASE_HEADER

namespace UI_GTKMM_NS {
class UI_GTKMM;

/**
 **
 ** @brief	the base class
 **
 ** @author	Diether Knof
 **
 **/
class Base {
  public:
    Base(Base* parent);
    Base(Base* parent, UI_GTKMM* ui);

    virtual ~Base();

    virtual void cards_update();
    virtual void cards_back_update();
    virtual void setting_update(int type);
    virtual void language_update();

  public:
    UI_GTKMM* const ui = nullptr;
    Base* const parent = nullptr;

  private: // unused
    Base() = delete;
    Base(Base const&) = delete;
    Base& operator=(Base const&) = delete;
}; // class Base

} // namespace UI_GTKMM_NS

#endif // #ifdef GTKMM_BASE_HEADER

#endif // #ifdef USE_UI_GTKMM
