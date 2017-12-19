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

#include "constants.h"

#ifdef CHECK_RUNTIME

#include "runtime.h"

// the runtime
Runtime runtime;

/**
 ** constructor
 ** 
 ** @param     -
 ** 
 ** @return    -
 ** 
 ** @author    Diether Knof
 ** 
 ** @version   0.7.6
 **/ 
Runtime::Runtime() :
  map<string, DK::Utils::StopWatch>()
{
  (*this)["program"].start();
} // Runtime::Runtime()

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
Runtime::~Runtime()
{
  (*this)["program"].stop();
  for (map<string, DK::Utils::StopWatch>::iterator i = this->begin();
       i != this->end();
       ++i) {
    if (i->second.running()) {
#ifdef DKNOF
      cerr << "Stop watch not finished: " << i->first << endl;
#endif
      i->second.stop();
    } // if (i->second.running())
  } // for (i \in this)
  cout << *this;
} // Runtime::~Runtime()

/**
 ** writes the runtimes in 'ostr'
 ** 
 ** @param     ostr   stream to write into
 ** 
 ** @return    -
 ** 
 ** @author    Diether Knof
 ** 
 ** @version   0.7.6
 **/ 
void
Runtime::write(ostream& ostr) const
{
  ostr << "runtime  calls  name\n";
  ostr << "------+-------+-------\n";
  for (Runtime::const_iterator sw = this->begin();
       sw != this->end();
       ++sw)
    ostr << setw(6) << sw->second.msec() << ": "
    << setw(6) << sw->second.calls() << "  "
      << sw->first << '\n';

  return ;
} // void Runtime::write(ostream& ostr) const

/**
 ** writes the runtimes in 'ostr'
 ** 
 ** @param     ostr      stream to write into
 ** @param     runtime   runtime to write
 ** 
 ** @return    output stream
 ** 
 ** @author    Diether Knof
 ** 
 ** @version   0.7.6
 **/ 
ostream&
operator<<(ostream& ostr, Runtime const& runtime)
{
  runtime.write(ostr);
  return ostr;
} // ostream& operator<<(ostream& ostr, Runtime runtime)

#endif // #ifdef CHECK_RUNTIME
