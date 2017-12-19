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

#include "version.h"

#include <fstream>
#include <sstream>
using std::ostream;
using std::istream;
using std::ifstream;
using std::string;
using std::istringstream;
using std::ostringstream;

namespace DK {
namespace Utils {

/**
 ** Constructor
 **
 ** @param	filename	the filename
 **
 ** @return	pointer to a new created version
 **		NULL in the case of an error
 **/
Version*
  Version::create_from_file(string const filename)
  {
    ifstream istr(filename.c_str());
    if (istr.fail())
      return NULL;

    return Version::create(istr);
  } // static Version* create_from_file(string filename)

/**
 ** Constructor
 **
 ** @param	istr	the input stream
 **
 ** @return	pointer to a new created version
 **		NULL in the case of an error
 ** 
 ** @todo	read whether it is a release
 **/
Version*
  Version::create(istream& istr)
  {
    unsigned major = 0;
    unsigned minor = 0;
    unsigned micro = 0;
    char patch = '\0';


    // read the date from the stream.
    // Format: 'major.minor.micro  (date)'
    // Example: '0.6.1  (2003-12-21)'
    istr >> major;
    if (istr.get() != '.')
      return NULL;
    istr >> minor;
    if (istr.get() != '.')
      return NULL;
    istr >> micro;
    if (istr.peek() != ' ')
      patch = istr.get();

    if ((istr.get() != ' ')
        || (istr.get() != ' ')
        || (istr.get() != '('))
      return new Version(major, minor, micro, patch, Date(__DATE__), false);

    if (istr.fail())
      return new Version(major, minor, micro, patch, Date(__DATE__), false);

    Date* const date = Date::create(istr);
    if (date == NULL)
      return new Version(major, minor, micro, patch, Date(__DATE__), false);

    if (istr.get() != ')') {
      delete date;
      return NULL;
    }

    if ((istr.get() != ' ')
        || (istr.get() != '(')) {
      Version* const version
        = new Version(major, minor, micro, patch, *date, false);
      delete date;
      return version;
    }

    string release_str;
    while (   istr.good()
           && istr.peek() != ')')
      release_str += istr.get();

    if (   (release_str != "release")
        && (release_str != "developer version")) {
      Version* const version
        = new Version(major, minor, micro, patch, *date, false);
      delete date;
      return version;
    }

    bool const release = (release_str == "release");

    if ((istr.get() != ' ')
        || (istr.get() != '(')) {
      Version* const version
        = new Version(major, minor, micro, patch, *date, release);
      delete date;
      return version;
    }

    string description;
    while (   istr.good()
           && istr.peek() != ')')
      description += istr.get();

    Version* const version
      = new Version(major, minor, micro, patch, *date, release, description);
    delete date;

    return version;
  } // static Version* Version::create(string filename)

/**
 ** Constructor
 **
 ** @param	string	string to read the version von
 **
 ** @return	pointer to a new created version
 **		NULL in the case of an error
 **/
Version*
  Version::create(string const& s)
  {
    istringstream istr(s.c_str());

    return Version::create(istr);
  } // static Version* Version::create(string s)


/**
 ** Constructor
 ** the version number will be 'major'.'minor'.'micro''patch'
 **
 ** @param     major   major version number
 ** @param     minor   minor version number
 ** @param     micro   micro version number
 ** @param     patch   character to denote the patch version
 **                    default: none
 **/
Version::Version(unsigned const major,
                 unsigned const minor, unsigned const micro,
                 char const patch) :
  major_(major),
  minor_(minor),
  micro_(micro),
  patch_(patch),
  date_(Date(__DATE__)),
  release_(false),
  description_()
{ }

/**
 ** Constructor
 ** the version number will be 'major'.'minor'.'micro''patch'-'release'
 **
 ** @param	major         major version number
 ** @param	minor         minor version number
 ** @param	micro         micro version number
 ** @param	date          the date
 **/
Version::Version(unsigned const major,
                 unsigned const minor, unsigned const micro,
                 Date const& date) :
  major_(major),
  minor_(minor),
  micro_(micro),
  patch_('\0'),
  date_(date),
  release_(false),
  description_()
{ }

/**
 ** Constructor
 ** the version number will be 'major'.'minor'.'micro''patch'-'release'
 **
 ** @param	major         major version number
 ** @param	minor         minor version number
 ** @param	micro         micro version number
 ** @param	patch         character to denote the patch version
 ** @param	date          the date
 **/
Version::Version(unsigned const major,
                 unsigned const minor, unsigned const micro,
                 char const patch,
                 Date const& date) :
  major_(major),
  minor_(minor),
  micro_(micro),
  patch_(patch),
  date_(date),
  release_(false),
  description_()
{ }

/**
 ** Constructor
 ** the version number will be 'major'.'minor'.'micro''patch'-'release'
 **
 ** @param	major         major version number
 ** @param	minor         minor version number
 ** @param	micro         micro version number
 ** @param	date          the date
 ** @param	release	      whether this is a release
 ** @param	description   description
 **                       default: ""
 **/
Version::Version(unsigned const major,
                 unsigned const minor, unsigned const micro,
                 Date const& date,
                 bool const release,
                 std::string const& description) :
  major_(major),
  minor_(minor),
  micro_(micro),
  patch_('\0'),
  date_(date),
  release_(release),
  description_(description)
{ }

/**
 ** Constructor
 ** the version number will be 'major'.'minor'.'micro''patch'-'release'
 **
 ** @param	major         major version number
 ** @param	minor         minor version number
 ** @param	micro         micro version number
 ** @param	patch         character to denote the patch version
 ** @param	date          the date
 ** @param	release       whether this is a release
 ** @param	description   description
 **                       default: ""
 **/
Version::Version(unsigned const major,
                 unsigned const minor, unsigned const micro,
                 char const patch,
                 Date const& date,
                 bool const release,
                 std::string const& description) :
  major_(major),
  minor_(minor),
  micro_(micro),
  patch_(patch),
  date_(date),
  release_(release),
  description_(description)
{ }

/**
 ** Constructor
 ** the version number will be 'major'.'minor'.'micro'
 **
 ** @param     major         major version number
 ** @param     minor         minor version number
 ** @param     micro         micro version number
 ** @param     description   the description
 **/
Version::Version(unsigned const major,
                 unsigned const minor, unsigned const micro,
                 std::string const& description) :
  major_(major),
  minor_(minor),
  micro_(micro),
  patch_('\0'),
  date_(Date(__DATE__)),
  release_(false),
  description_(description)
{ }

/**
 ** copy constructor
 **
 ** @param		version		version to copy
 **/
Version::Version(Version const& version) :
  major_(version.major_),
  minor_(version.minor_),
  micro_(version.micro_),
  patch_(version.patch_),
  date_(version.date_),
  release_(version.release_),
  description_(version.description_)
{ }

/**
 ** Destructor
 **/
Version::~Version()
{
} // Version::~Version()

/**
 ** conversion to a string
 **
 ** @param	-
 **
 ** @return	the corresponding string
 **/
Version::operator string() const
{
  ostringstream ostr;

  ostr << *this;

  return ostr.str();
} // Version::operator string string() const

/**
 ** conversion of the number to a string
 **
 ** @param	-
 **
 ** @return	the corresponding string
 **/
string
Version::number_to_string() const
{
  ostringstream ostr;

  ostr << this->major_
    << "."
    << this->minor_
    << "."
    << this->micro_;

  if (this->patch_ != '\0')
    ostr << this->patch_;

  return ostr.str();
} // string Version::number_to_string() const

/**
 ** write the version to the stream
 **
 ** @param	ostr	output stream to write to
 **
 ** @return	the output stream
 **/
ostream&
Version::write(ostream& ostr) const
{
  ostr << this->number_to_string();

  ostr << "  (" << this->date_ << ')';

  if (this->release_)
    ostr << " (release)";
  else
    ostr << " (developer version)";

  if (!this->description_.empty())
    ostr << " (" << this->description_ << ")";

  return ostr;
} // ostream& Version::write(ostream& ostr) const

/**
 ** write 'version' to stream 'ostr'
 **
 ** @param	ostr	output stream to write to
 ** @param	version	the version
 **
 ** @return	the output stream
 **/
ostream&
operator<<(ostream& ostr, Version const& version)
{
  version.write(ostr);

  return ostr;
} // ostream& operator<<(ostream& ostr, Version const& version)

/**
 ** -> result
 **
 ** @param	v1	first version
 ** @param	v2	second version
 **
 ** @return	whether the versions are equal
 **/
bool
operator==(Version const& v1, Version const& v2)
{
  return ((v1.micro_ == v2.micro_)
          && (v1.minor_ == v2.minor_)
          && (v1.major_ == v2.major_)
          && (v1.patch_ == v2.patch_)
          && (v1.date_ == v2.date_)
          && (v1.release_ == v2.release_)
          && (v1.description_ == v2.description_));
} // bool operator==(Version const& v1, Version const& v2)

/**
 ** -> result
 **
 ** @param	v1	first version
 ** @param	v2	second version
 **
 ** @return	whether 'v1' is older than 'v2'
 **/
bool
operator<(Version const& v1, Version const& v2)
{

  if (v1.major_ < v2.major_)
    return true;
  if (v1.major_ > v2.major_)
    return false;

  // now we have 'v1.major == v2.major'
  if (v1.minor_ < v2.minor_)
    return true;
  if (v1.minor_ > v2.minor_)
    return false;

  // now we have 'v1.minor == v2.minor'
  if (v1.micro_ < v2.micro_)
    return true;
  if (v1.micro_ > v2.micro_)
    return false;

  // now we have 'v1.micro == v2.micro'
  if (v1.patch_ < v2.patch_)
    return true;
  if (v1.patch_ > v2.patch_)
    return false;

  // now the version numbers are equal
  return (v1.date_ < v2.date_);
} // bool operator<(Version const& v1, Version const& v2)

/**
 ** -> result
 **
 ** @param	v1	first version
 ** @param	v2	second version
 **
 ** @return	whether 'v1' is not newer than 'v2'
 **/
bool
operator<=(Version const& v1, Version const& v2)
{
  return ((v1 < v2) || (v1 == v2));
} // bool operator<=(Version const& v1, Version const& v2)

/**
 ** -> result
 **
 ** @param     v1   first version
 ** @param     v2   second version
 **
 ** @return    whether 'v1' is newer than 'v2'
 **/
bool
operator>(Version const& v1, Version const& v2)
{
  return (v2 < v1);
} // bool operator>(Version const& v1, Version const& v2)

/**
 ** -> result
 **
 ** @param     v1   first version
 ** @param     v2   second version
 **
 ** @return    whether 'v1' is not older than 'v2'
 **/
bool
operator>=(Version const& v1, Version const& v2)
{
  return (v2 <= v1);
} // bool operator>=(Version v1, Version v2)

} // namespace Utils
} // namespace DK
