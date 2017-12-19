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

#ifndef DK_UTILS_TIME_HEADER
#define DK_UTILS_TIME_HEADER

#include <time.h>

namespace DK {
  namespace Utils {
    class Time {
      public:
	static Time current();

      public:
        // constructor
	Time();
        // constructor
	Time(time_t const sec, unsigned const msec);

        // the seconds
	time_t sec;
        // the mili seconds
	unsigned msec;

      private:
    }; // class Time
  } // namespace Utils
} // namespace DK

#endif // #ifndef DK_UTIL_TIME_HEADER
