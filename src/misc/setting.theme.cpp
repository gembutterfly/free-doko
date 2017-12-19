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
 ********************************************************************/

#include "constants.h"

#include "setting.theme.h"

#include "translations.h"

/**
 ** constructor
 **
 ** @param     name   name of the theme
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
Setting::Theme::Theme(string const& name) :
  name_(::translation("") + name),
  bool_(),
  unsigned_(),
  string_()
{ }

/**
 ** constructor
 **
 ** @param     name   name of the theme
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
Setting::Theme::Theme(Translator::Translation const& name) :
  name_(name),
  bool_(),
  unsigned_(),
  string_()
{ }

/**
 ** copy constructor
 **
 ** @param     theme   theme to copy
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
Setting::Theme::Theme(Theme const& theme) :
  name_(theme.name_),
  bool_(theme.bool_),
  unsigned_(theme.unsigned_),
  string_(theme.string_)
{ }

/**
 ** copy operator
 **
 ** @param     theme   theme to copy
 **
 ** @return    this
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
Setting::Theme&
Setting::Theme::operator=(Theme const& theme)
{
  this->name_ = theme.name_;
  this->bool_ = theme.bool_;
  this->unsigned_ = theme.unsigned_;
  this->string_ = theme.string_;

  return *this;
} // Setting::Theme& Setting::Theme::operator=(Theme theme)

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
// destructor
Setting::Theme::~Theme()
{}


/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    the name of the theme
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
Translator::Translation const&
Setting::Theme::name() const
{
  return this->name_;
} // string const& Setting::Theme::name() const

/**
 ** set the bool setting
 **
 ** @param     type    type to set
 ** @param     value   value of 'type'
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
void
Setting::Theme::set(TypeBool const& type, bool const& value)
{
  this->bool_[type] = value;
  return ;
} // void Setting::Theme::set(TypeBool type, bool value)

/**
 ** set the unsigned setting
 **
 ** @param     type    type to set
 ** @param     value   value of 'type'
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
void
Setting::Theme::set(TypeUnsigned const& type, unsigned const& value)
{
  this->unsigned_[type] = value;
  return ;
} // void Setting::Theme::set(TypeUnsigned type, unsigned value)

/**
 ** set the string setting
 **
 ** @param     type    type to set
 ** @param     value   value of 'type'
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
void
Setting::Theme::set(TypeString const& type, string const& value)
{
  this->string_[type] = value;
  return ;
} // void Setting::Theme::set(TypeString type, string value)

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    whether the theme can be applied
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **
 ** @todo      search only the selected cardset
 **/
bool
Setting::Theme::can_be_applied() const
{
  Setting setting_tmp(setting);
  setting_tmp.set(*this);
  for (int t = Setting::STRING_FIRST; t <= Setting::STRING_LAST; ++t) {
    if (this->string_.find(static_cast<Setting::TypeString>(t))
        == this->string_.end())
      continue;

    switch (t) {
    case CARDSET:
    case BACKGROUND:
    case LANGUAGE:
    case CARDS_BACK:
      if (   setting_tmp.path(static_cast<Setting::TypeString>(t)).empty()
          || (setting_tmp.value(static_cast<Setting::TypeString>(t))
              != (*this->string_.find(static_cast<Setting::TypeString>(t))).second)
         ) {
        return false;
      }
    default:
      break;
    } // switch(t)
  } // for (t \in Setting::TypeString)

  return true;
} // bool Setting::Theme::can_be_applied() const
