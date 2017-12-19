//////////////////////////////////////////////////////////////////////////////
//
//   Copyright (C) 2002  by Diether Knof
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
//
//////////////////////////////////////////////////////////////////////////////

#include "time.h"
#ifndef WINDOWS
#include <sys/time.h>
#endif

namespace DK {
  namespace Utils {

    /**
     ** -> result
     **
     ** @param		-
     **
     ** @return		the current time
     **
     ** @author		Diether Knof
     **
     ** @version	2005-10-31
     **/
    Time
      Time::current()
      {
#ifndef WINDOWS
	struct timeval tv;
	struct timezone tz;
	if (gettimeofday(&tv, &tz) == 0)
	  return Time(tv.tv_sec, tv.tv_usec / 1000);
#endif

	clock_t const time_clock = clock();
	if (time_clock == static_cast<clock_t>(-1))
	  return Time();

	// under Microsoft Windows 'CLOCKS_PER_SEC' is 1000,
	// under GNU/Linux (Posix) 1000000,
	// under FreeBSD < 1000
	// So in order to do integer calculation we have to split the code

	// The variable is needed because 'CLOCKS_PER_SEC' need not to be a
	// constant (so no '#if CLOCKS_PER_SEC < 1000' can be used).
	// But with a constant in FreeBSD 'CLOCKS_PER_SEC / 1000' equals 0,
	// this leads to the warning division with zero (g++-3.4.4).
	clock_t clocks_per_sec = CLOCKS_PER_SEC;
	if (clocks_per_sec < 1000)
	  return Time(time_clock / clocks_per_sec,
		      (time_clock * (1000 / clocks_per_sec)) % 1000);
	else
	  return Time(time_clock / clocks_per_sec,
		      (time_clock / (clocks_per_sec / 1000)) % 1000);
      } // static Time Time::current()

    /**
     ** constructor
     **
     ** @param		-
     **
     ** @return		-
     **
     ** @author		Diether Knof
     **
     ** @version	2005-10-31
     **/
    Time::Time() :
      sec(0),
      msec(0)
    { }

    /**
     ** constructor
     **
     ** @param		sec	seconds
     ** @param		msec	mili seconds
     **
     ** @return		-
     **
     ** @author		Diether Knof
     **
     ** @version	2005-10-31
     **/
    Time::Time(time_t const sec, unsigned const msec) :
      sec(sec),
      msec(msec)
    { }

  } // namespace Utils
} // namespace DK
