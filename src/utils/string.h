/**********************************************************************
 *
 *   Copyright (C) 2002  by Diether Knof
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
 *   Contact:
 *     Diether Knof dknof@gmx.de
 *
 **********************************************************************/

#ifndef DK_UTILS_STRING_HEADER
#define DK_UTILS_STRING_HEADER

#include <string>
#include <vector>
#include <iosfwd>
#include <iomanip>
#include <sstream>
#include <cstring>

namespace DK {
  namespace Utils {
    namespace String {


      // remove all leading and trailing blanks
      std::string& remove_blanks(std::string& s);
      // remove all leading and trailing blanks
      std::string remove_blanks(std::string const& s);

      // remove all trailing newlines
      std::string& remove_trailing_newlines(std::string& s);
      // replace double newlines with one
      std::string& remove_double_newlines(std::string& s);

      // return the first word
      std::string word_first(std::string const& text);
      // remove the first word
      std::string& word_first_remove(std::string& text);
      // remove the first word and following blanks
      std::string& word_first_remove_with_blanks(std::string& text);

      // replace all 's' with 'replacement' in 'text'
      std::string& replace_all(std::string& text,
                               std::string const& s, std::string const& replacement);

      // gets a line and replaces the escape-sequences
      std::string getline_escape(std::istream& istr);

      // reads till an eof
      std::string get_till_eof(std::istream& istr);
      // reads the whole file in the string
      std::string getfile(std::string const& filename);

      // split 'line' according to 'separator'
      std::vector<std::string> split(std::string const& line,
                                     char const separator);

      // convert from latin1 to utf8
      std::string latin1_to_utf8(std::string text);

      // converts the argument to a string, using 'strstream'
      template<typename T>
        std::string to_string(T const& t)
        {
          std::ostringstream entry;

          entry << t;

          return entry.str();
        } // template<typename T> std::string to_string(T const& t)

      // converts the argument to a string, using 'strstream'
      template<typename T>
        std::string to_string(T const& t, int const width,
                              char const fill = ' ')
        {
          std::ostringstream entry;

          entry << std::setfill(fill) << std::setw(width) << t;

          return entry.str();
        } // template<typename T> std::string to_string(T const& t, int width, char fill = ' ')

      // reads the argument from a string, using 'strstream'
      template<typename T>
        T const& from_string(std::string const& str, T& t)
        {
          std::istringstream istr(str.c_str());

          istr >> t;

          return t;
        } // template<typename T> T from_string(T& t)

    } // namespace String
  } // namespace Utils
} // namespace DK

#endif // #ifndef UTIL_DK_STRINGS_HEADER
