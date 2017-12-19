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

#ifndef CLASS_RUNTIME_HEADER
#define CLASS_RUNTIME_HEADER

#include <map>
#include "utils/stop_watch.h"

class Runtime;
extern Runtime runtime;

/**
 ** class to test the runtime of the program
 ** just use '::runtime["marker"].start()' and '...stop()'
 **
 ** @author   Diether Knof
 **
 ** @todo     ui wrapper
 **/
class Runtime : public std::map<string, DK::Utils::StopWatch> {
  public:
    // constructor
    Runtime();
    // destructor
    ~Runtime();

    // write the runtime
    void write(ostream& ostr) const;

  private: // unused
    Runtime(Runtime const&);
    Runtime& operator=(Runtime const&);
}; // class Runtime

// write the runtime
std::ostream& operator<<(std::ostream& ostr, Runtime const& runtime);

#endif // #ifndef CLASS_RUNTIME_HEADER
