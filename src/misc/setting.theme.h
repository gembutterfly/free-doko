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

#ifndef CLASS_SETTING_THEME_HEADER
#define CLASS_SETTING_THEME_HEADER

#include "../basistypes.h"

#include "setting.h"
#include "translation.h"

/**
 ** a theme for settings
 **
 ** @author   Diether Knof
 **/
class Setting::Theme {
    friend class Setting;

  public:
    // constructor
    Theme(string const& name);
    // constructor
    Theme(Translator::Translation const& name);
    // copy constructor
    Theme(Theme const& theme);
    // copy operator
    Theme& operator=(Theme const& theme);

    // destructor
    ~Theme();


    // the name
    Translator::Translation const& name() const;

    // set the bool setting
    void set(TypeBool const& type, bool const& value);
    // set the unsigned setting
    void set(TypeUnsigned const& type, unsigned const& value);
    // set the string setting
    void set(TypeString const& type, string const& value);


    // check whether the theme can be applied
    bool can_be_applied() const;

  private:
    // the name of the theme
    Translator::Translation name_;
    // boolean settings
    map<TypeBool, bool> bool_;
    // unsigned settings
    map<TypeUnsigned, unsigned> unsigned_;
    // string settings
    map<TypeString, string> string_;

  private: // unused
    Theme();
}; // class Setting::Theme


#endif // #ifndef CLASS_SETTING_THEME_HEADER
