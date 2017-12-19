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

#ifndef DK_UTILS_VERSION_HEADER
#define DK_UTILS_VERSION_HEADER

#include "date.h"

#include <iosfwd>
#include <string>

namespace DK {
namespace Utils {
class Version {
public:
  static Version* create_from_file(std::string filename);
  static Version* create(std::istream& istr);
  static Version* create(std::string const& s);

public:
  friend bool operator==(Version const&, Version const&);
  friend bool operator<(Version const&, Version const&);
public:
  Version(unsigned major, unsigned minor,
          unsigned micro, char patch = '\0');
  Version(unsigned major, unsigned minor,
          unsigned micro,
          Date const& date);
  Version(unsigned major, unsigned minor,
          unsigned micro, char patch,
          Date const& date);
  Version(unsigned major, unsigned minor,
          unsigned micro,
          Date const& date,
          bool release,
          std::string const& description = "");
  Version(unsigned major, unsigned minor,
          unsigned micro, char patch,
          Date const& date,
          bool release,
          std::string const& description = "");
  Version(unsigned major, unsigned minor,
          unsigned micro,
          std::string const& description);
  Version(Version const& version);
  ~Version();

  std::ostream& write(std::ostream& ostr) const;

  operator std::string() const;
  std::string number_to_string() const;

private:
  unsigned const major_; // main version number
  unsigned const minor_; // subversion number
  unsigned const micro_; // subsubversion number
  char const patch_; // character for the patch version
  Date const date_; // the date
  bool const release_; // whether this is a release
  std::string const description_; // a (short) description
private:
  Version();
  Version& operator=(Version const&);
}; // class Version

bool operator==(Version const& v1, Version const& v2);
bool operator<(Version const& v1, Version const& v2);
bool operator<=(Version const& v1, Version const& v2);
bool operator>(Version const& v1, Version const& v2);
bool operator>=(Version const& v1, Version const& v2);

std::ostream& operator<<(std::ostream& ostr, Version const& version);

} // namespace Utils
} // namespace DK

#endif // #ifndef DK_UTILS_VERSION_HEADER
