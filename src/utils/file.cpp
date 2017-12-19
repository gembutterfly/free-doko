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

#include "file.h"
#include "string.h"
#include "windows.h"

#include <sys/stat.h>
#if defined(LINUX) || defined(HPUX)
// for getpwent
#include <pwd.h>
#include <unistd.h>
// for MAXPATHLEN
#include <sys/param.h>
#endif
#ifdef WINDOWS
// for SHGetFolderPath
#include <shlobj.h>
#endif
#include <cstdlib>

#ifndef RELEASE
#include <iostream>
#endif

using std::string;

namespace DK {
  namespace Utils {
    namespace File {

      /**
       **
       ** returns the home directory of the user
       **
       ** @param	-
       **
       ** @return	the home directory
       ** 
       ** @author	Diether Knof
       **
       ** @version	2003-10-30
       **
       **/
      string
	home_directory()
	{
#ifndef WINDOWS
	  if (getenv("HOME") != nullptr)
	    return getenv("HOME");

	  struct passwd* pwd;
	  if ( (pwd = getpwuid(getuid())) != nullptr )
	    return pwd->pw_dir;


#else  // !#ifndef WINDOWS
          {
            // shlobj.h
            // see http://msdn.microsoft.com/en-us/library/bb762181(VS.85).aspx
            // depercated, but we use it since we also support Windows XP
            TCHAR szPath[MAX_PATH];

            if(SUCCEEDED(SHGetFolderPath(nullptr, 
                                         CSIDL_APPDATA|CSIDL_FLAG_CREATE, 
                                         nullptr, 
                                         0, 
                                         szPath))) 
            {
              return szPath;
            }
          }
#ifdef OUTDATED
          // DK 0.7.8 2010-01-16
          return DK::Utils::Windows::Reg_read(HKEY_CURRENT_USER,
                                              "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders",
                                              "AppData");
#endif
#endif // !#ifndef WINDOWS
          return "";
        } // string home_directory()

      /**
       **
       ** returns the desktop directory of the user
       **
       ** @param	-
       **
       ** @return	the desktop directory
       ** 		empty string, if the directory does not exists
       ** 
       ** @author	Diether Knof
       **
       ** @version	2004-03-03
       **
       **/
      string
        desktop_directory()
        {
#ifdef WINDOWS
          return DK::Utils::Windows::Reg_read(HKEY_CURRENT_USER,
                                              "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders",
                                              "Desktop");
#endif
#ifdef LINUX
          if (DK::Utils::File::isdirectory(home_directory() + "/Desktop"))
            return (home_directory() + "/Desktop");
#endif

          return "";
        } // string desktop_directory()

      /**
       **
       ** remove 'depth' times the basename of 'filename'
       **
       ** @param	filename	the filename to be checked
       **
       ** @return	the dirname of 'filename'
       ** 
       ** @author	Diether Knof
       **
       ** @version	2003-03-30
       **
       **/
      string
        dirname(std::string const& filename, unsigned const depth)
        {
          if (depth == 0)
            return filename;
          if (depth > 1)
            return dirname(dirname(filename), depth - 1);

          // search the last '/' and remove all from that position on
          unsigned pos;
          for (pos = filename.length(); pos > 0; pos--)
            if (filename[pos - 1] == '/')
              break;

          if (pos == 0){
            // the path does not contain a directory
            return ".";
          } else {
            return string(filename, 0, pos - 1);
          }
        } // string dirname(std::string const& filename, unsigned const depth = 1)

      /**
       **
       ** -> result
       **
       ** @param	filename	the filename to be checked
       ** @param	suffix		the suffix, that shall be removed
       **
       ** @return	the basename of 'filename' without 'suffix'
       ** 
       ** @author	Diether Knof
       **
       ** @version	2003-03-30
       **
       **/
      string
        basename(std::string const& filename, string const& suffix)
        {
          // search the last '/' and remove all til that position
          unsigned pos;
          for (pos = filename.length(); pos > 0; pos--)
            if (filename[pos - 1] == '/')
              break;

          if ((pos + suffix.length() + 1 <= filename.length())
              && (string(filename, filename.length() - suffix.length() - 1,
                         string::npos)
                  == "." + suffix))
            // the suffix exists
            return string(filename,
                          pos,
                          filename.length() - pos - suffix.length() - 1);
          else // if !(suffix exists)
            return string(filename, pos, string::npos);

        } // string basename(std::string const& filename, string const& suffix = "")

      /**
       **
       ** -> result
       **
       ** @param	filename	the filename to be checked
       **
       ** @return	the extension of 'filename'
       ** 
       ** @author	Diether Knof
       **
       ** @version	2003-03-30
       **
       **/
      string
        extension(std::string const& filename)
        {
          // search the last '/' and remove all til that position
          unsigned pos;
          for (pos = filename.length(); pos > 0; pos--)
            if ((filename[pos - 1] == '/')
                || (filename[pos - 1] == '.'))
              break;

          if ((pos == 0) || (filename[pos - 1] == '/')){
            // the file does not contain an extension
            return "";
          } else {
            return string(filename, pos, string::npos);
          }
        } // string extension(std::string const& filename)

      /**
       **
       ** -> result
       **
       ** @param	name	the name to check
       **
       ** @return	whether 'name' is a file that exists
       ** 
       ** @author	Diether Knof
       **
       ** @version	2003-03-30
       **
       **/
      bool
        isfile(string const& name)
        {
          struct stat status;

          // look, whether the file exists
#ifdef LINUX
          if (lstat(name.c_str(), &status) == -1)
#else
            ;
          if (stat(name.c_str(), &status) == -1)
#endif
            return false;

          return (S_ISREG(status.st_mode) ? true : false);
        } // bool isfile(string const& name)

      /**
       **
       ** -> result
       **
       ** @param	name	the name to check
       **
       ** @return	whether 'name' is a directory that exists
       ** 
       ** @author	Diether Knof
       **
       ** @version	2003-03-30
       **
       **/
      bool
        isdirectory(string const& name)
        {
          struct stat status;

          // look, whether the file exists
#ifdef LINUX
          if (lstat(name.c_str(), &status) == -1)
#else
            ;
          if (stat(name.c_str(), &status) == -1)
#endif
            return false;

          return (S_ISDIR(status.st_mode) ? true : false);
        } // bool isdirectory(String const& name)


      /**
       **
       ** Linux: replayce '~/' with $HOME
       ** Windows: replace '~/' with HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVerson\Explorer\Shell Folders\AppData
       **
       ** @param	filename	filename to be expanded
       **
       ** @return	expanded filename
       ** 
       ** @author	Diether Knof
       **
       ** @version	2003-04-02
       **
       **/
      string
        filename_expand(string filename)
        {
          if (filename.size() >= 2) {
            if ((filename[0] == '~')
                && (filename[1] == '/')) {
              filename = home_directory() + string(filename, 1, string::npos);
#ifdef WINDOWS
              DK::Utils::String::replace_all(filename, "\\", "/");
#endif // #ifdef WINDOWS
            } // if ((filename[0] == '~') && (filename[1] == '/'))
          } // if (filename.size() >= 2)

          return filename;
        } // string filename_expand(string filename)

      /**
       **
       ** Linux: replayce '~/' with $HOME
       ** Windows: replace '~/' with SHGetFolderPath(CSIDL_APPDATA)
       **
       ** @param     path   path (relative or absolute
       **
       ** @return    absolute path, 'path' if it could not be detected
       ** 
       ** @author    Dirk Przybylla
       ** @author    Diether Knof
       **
       ** @version   2010-06-06
       **
       ** @bug       using the function 'realpath'
       **/
      string
        absolute_path(string path)
        {
#ifdef WINDOWS
          { // Windows
            // check the size of the buffer needed
            DWORD const bufsize
              = GetFullPathName(path.c_str(), 0, nullptr, nullptr);
            if (bufsize == 0) {
#ifndef RELEASE
              std::cerr << "GetFullPathName failed. Errorcode ["
                << GetLastError() << "]" << std::endl;
#endif
              return path;
            }
            // get the full path by a windows specific function
            TCHAR* const buffer = new char[bufsize + 1];
            if (buffer == 0)
              return path;
            DWORD const dwRetVal
              = GetFullPathName(path.c_str(), bufsize, buffer, nullptr);
            if (dwRetVal == 0) {
#ifndef RELEASE
              std::cerr << "GetFullPathName failed. Errorcode ["
                << GetLastError() << "]" << std::endl;
#endif
              delete[] buffer;
              return path;
            }
            buffer[bufsize] = 0;
            path = string(buffer);
            delete[] buffer;
            return path;
          } // Windows
#else
          { // not Windows
            if (!path.empty()
                && (path[0] == '/') )
              return path;
            char path_p[MAXPATHLEN];
            // Bug: the manual page from 'realpath' says not to use the function
            if (realpath(path.c_str(), path_p) != nullptr)
              return string(path_p);
          } // not Windows
#endif
          return path;
        } // string absolute_expand(string path)

    } // namespace File
  } // namespace Utils
} // namespace DK
