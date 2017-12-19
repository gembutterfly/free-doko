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

#ifndef DK_UTILS_FILE_HEADER
#define DK_UTILS_FILE_HEADER

#include <string>

namespace DK {
  namespace Utils {
    namespace File {
      // returns the home directory of the current user
      std::string home_directory();
      // returns the desktop directory of the current user
      std::string desktop_directory();

      // returns the dirname of the file
      std::string dirname(std::string const& filename,
			  unsigned const depth = 1);
      // returns the basename of the file
      std::string basename(std::string const& filename,
			   std::string const& suffix = "");
      // returns the extension of the file
      std::string extension(std::string const& filename);

      // returns whether the given name is a file
      bool isfile(std::string const& name);
      // returns whether the given name is a directory
      bool isdirectory(std::string const& name);

      // expands the filename: replaces '~/' by '$HOME/'
      std::string filename_expand(std::string filename);

      // returns the absolute path of the given path
      std::string absolute_path(std::string path);
    } // namespace File
  } // namespace Utils
} // namespace DK

#endif // #ifndef DK_UTIL_FILES_HEADER
