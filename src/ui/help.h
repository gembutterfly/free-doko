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

#ifndef HELP_HEADER
#define HELP_HEADER

#include "constants.h"

#include "../basistypes.h"
#include "../party/rule.h"
#include "../misc/setting.h"

// functions for the help
namespace Help {

  void show(string const& location);


  // the homepage location
  string homepage_location();
  // the location of cardsets to download
  string cardsets_download_location();

  // the location of the helpfile in the manual

  string manual_location(string const& page = "overview.html");
  string operation_location(string const& page);


  // the location of the specific help

  string location(Rule::TypeBool const type);
  string location(Rule::TypeUnsigned const type);

  string location(Setting::TypeBool const type);
  string location(Setting::TypeUnsigned const type);
  string location(Setting::TypeString const type);
  string location(Setting::TypeCardsOrder const type);

}; // namespace Help

#endif // #ifndef HELP_HEADER
