///////////////////////////////////////////////////////////////////////
//
//   FreeDoko a Doppelkopf-Game
//    
//   Copyright (C) 2001-2017 by Diether Knof and Borg Enders
//
//   This program is free software; you can redistribute it and/or 
//   modify it under the terms of the GNU General Public License as 
//   published by the Free Software Foundation; either version 2 of 
//   the License, or (at your option) any later version.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details. 
//   You can find this license in the file 'gpl.txt'.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 59 Temple Place, Suite 330, Boston, 
//   MA  02111-1307  USA
//
//  Contact:
//    Diether Knof dknof@gmx.de
//    Borg Enders  borg@borgsoft.de
//
///////////////////////////////////////////////////////////////////////

#ifdef WINDOWS

#include "windows.h"

#include <iostream>

using std::string;
using std::cerr;
using std::endl;

namespace DK {
  namespace Utils {
    namespace Windows {

      /**
       **
       ** 
       ** Windows: the registry entry
       **
       ** @param	hkey	hkey of the registry entry
       ** @param	key	key name of the registry entry
       ** @param	name	name of the registry entry
       **
       ** @return	string value of the registry
       ** 
       ** @author	Diether Knof
       **
       ** @version	2003-04-02
       **
       **/
      string
	Reg_read(HKEY hkey, string const& key, string const& name)
	{
	  // reading the registry
	  long winapi;
	  HKEY handle;

	  winapi = RegOpenKeyEx(hkey, key.c_str(), 0, KEY_QUERY_VALUE, &handle);
	  if (winapi != 0) {
	    return "";
	  }

	  DWORD size = 0;
	  DWORD type = 0;
	  winapi = RegQueryValueEx(handle, name.c_str(), NULL, &type, NULL, &size);
	  // (HKEY,LPCSTR,PDWORD,PDWORD,LPBYTE,PDWORD);

	  if ((winapi != 0)
	      || (size == 0)) {
	    RegCloseKey(hkey);
	    return "";
	  }

#ifndef RELEASE
	  if (type != REG_SZ) {
	    cerr << "type '" << type << "' is not 'REG_SZ'." << endl;
	  }
#endif


	  BYTE* value = new BYTE[size];
	  winapi = RegQueryValueEx(handle, name.c_str(), NULL, NULL, value, &size);
	  if (winapi != 0) {
	    RegCloseKey(hkey);
	    return "";
	  }

	  RegCloseKey(hkey);

	  string result = (char*)value;
	  delete[] value;

	  return result;
	} // string Reg_read(HKEY& hkey, string const& key, string const& name)


    } // namespace Windows
  } // namespace Utils
} // namespace DK

#endif // #ifdef WINDOWS
