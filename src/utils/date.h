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

#ifndef DK_UTILS_DATE_HEADER
#define DK_UTILS_DATE_HEADER

#include <iosfwd>
#include <string>

#include "compare.h"

namespace DK {
  namespace Utils {

    /**
     ** a date class
     **/
    class Date {
      public:
	// read the date from the stream
	static Date* create(std::istream& istr);
	// read the date from the string
	static Date* create(std::string const& s);

      public:
	// constructor (current date)
	Date();
	// constructor
	Date(int const year, unsigned const month, unsigned const day);
	// constructor
	Date(std::string const& s);
	// copy constructor
	Date(Date const&);
	// destructor
	~Date();

	// write the date in ostr
	std::ostream& write(std::ostream& ostr) const;

	// convert in a string, p.e. 2006-10-11
	operator std::string const() const;

	// the year
	int year;
	// the month
	unsigned month;
	// the day
	unsigned day;

      private:
	Date& operator=(Date const& date);
    }; // class Version

    // whether the dates are equal
    bool operator==(Date const& d1, Date const& d2);
    // whether d1 is previous to d2
    bool operator<(Date const& d1, Date const& d2);
    // the rest of the compare functions
    CREATE_COMPARE_FUNCTIONS(Date)

    std::ostream& operator<<(std::ostream& ostr, Date const& date);
  } // namespace Utils
} // namespace DK

#endif // #ifndef DK_UTILS_DATE_HEADER
