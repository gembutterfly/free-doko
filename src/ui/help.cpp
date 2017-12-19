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
 *********************************************************************/

#include "help.h"

#include "ui.h"
#include "../misc/translator.h"
#include "../utils/windows.h"
#include "../utils/string.h"
#include "../utils/file.h"

/**
 ** show the help page of the given location
 **
 ** @param     location	location of the page to show
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.6.3
 **
 **/
void
Help::show(string const& location)
{
#ifdef WINDOWS
  STARTUPINFO si = {sizeof(si)};
  PROCESS_INFORMATION pi;
  CreateProcess(NULL,
                const_cast<CHAR*>((::setting(Setting::BROWSER_COMMAND) + " " + location).c_str()),
                NULL, NULL, FALSE, 0, NULL, NULL,
                &si, &pi);
#else
  int const a =
    ::system((::setting(Setting::BROWSER_COMMAND) + " " + location
              + " &").c_str());
  (void) a;
#endif

  return ;
} // void Help::show(string const location)

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    the location of the homepage
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
string
Help::homepage_location()
{
  vector<string> languages;
  languages.push_back("en");
  languages.push_back("de");

  if (std::find(languages.begin(), languages.end(),
                ::setting(Setting::LANGUAGE))
      == languages.end())
    return ("http://free-doko.sourceforge.net/en/");

  return ("http://free-doko.sourceforge.net/"
          + ::setting(Setting::LANGUAGE) + "/");
} // string Help::homepage_location()

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    the location of the sourceforge page to download cardsets
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
string
Help::cardsets_download_location()
{
  return "http://sourceforge.net/project/showfiles.php?group_id=38051&package_id=30877";
} // string Help::cardsets_download_location()

/**
 ** -> result
 **
 ** @param     page   help page
 **                   default: index
 **
 ** @return    the location with the manual help in page 'page'
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.3
 **
 ** @bug       using the function 'realpath'
 **/
string
Help::manual_location(string const& page)
{
  // Search through a list of local directories, if that does not help,
  // return the internet address of the manual.

  // the directories to search
  vector<string> directories;

  { // fill the directories list
    if (getenv("FREEDOKO_MANUAL_DIRECTORY")) {
      directories.push_back(getenv("FREEDOKO_MANUAL_DIRECTORY"));
      directories.push_back(getenv("FREEDOKO_MANUAL_DIRECTORY")
                            + ("/" + ::translator.token()));
    }
#ifdef WINDOWS
    {
      string dir = DK::Utils::Windows::Reg_read(HKEY_LOCAL_MACHINE,
                                                "Software\\FreeDoko",
                                                "Help Directory");
      if (!dir.empty())
        directories.push_back(dir + "/" + ::translator.token());
    }
#endif
    vector<string> const datadir = ::setting.data_directories();
    for (vector<string>::const_iterator d = datadir.begin();
         d != datadir.end();
         d++)
      directories.push_back((::setting(Setting::MANUAL_DIRECTORY)[0] == '/'
                            ? "" : *d + "/")
                            + ::setting(Setting::MANUAL_DIRECTORY) + "/"
                            + ::translator.token());
    if (::translator.token() != ::setting(Setting::LANGUAGE))
      for (vector<string>::const_iterator d = datadir.begin();
           d != datadir.end();
           d++)
      directories.push_back((::setting(Setting::MANUAL_DIRECTORY)[0] == '/'
                            ? "" : *d + "/")
                              + ::setting(Setting::MANUAL_DIRECTORY) + "/"
                              + ::setting(Setting::LANGUAGE));
  } // fill the directories list


  { // search the manual page
    vector<string>::iterator d;
    for (d = directories.begin(); d != directories.end(); d++)
      if (DK::Utils::File::isfile(*d + "/" + page))
        return ("file://" + DK::Utils::File::absolute_path(*d + "/" + page));
  } // search the manual page

  return ("http://free-doko.sourceforge.net/doc/manual/"
          + ::translator.token() + "/" + page);
} // string Help::manual_location(string page)

/**
 ** -> result
 **
 ** @param     page   help page
 **
 ** @return    the location with the operation help in page 'page'
 **
 ** @author    Diether Knof
 **
 ** @version   0.6.3
 **/
string
Help::operation_location(string const& page)
{
  return manual_location("operation/" + page);
} // string Help::operation_location(string page)

/**
 ** -> result
 **
 ** @param     type   rule type to show
 **
 ** @return    the location of the help for the rule 'type'
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.3
 **/
string
Help::location(Rule::TypeBool const type)
{
  string name = ::name(type);
  return operation_location("rules_config.html"
                            + DK::Utils::String::replace_all(name,
                                                             " ", "_"));
} // string Help::location(Rule::TypeBool type)

/**
 ** -> result
 **
 ** @param     type   rule type to show
 **
 ** @return    the location of the help for the rule 'type'
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.3
 **/
string
Help::location(Rule::TypeUnsigned const type)
{
  string name = ::name(type);
  return operation_location("rules_config.html"
                            + DK::Utils::String::replace_all(name, " ", "_"));
} // string Help::location(Rule::TypeUnsigned type)

/**
 ** -> result
 **
 ** @param     type   setting type to show
 **
 ** @return    the location of the help for the setting 'type'
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.3
 **/
string
Help::location(Setting::TypeBool const type)
{
  string name = ::name(type);
  return operation_location("settings.html"
                            + DK::Utils::String::replace_all(name, " ", "_"));
} // string Help::location(Setting::TypeBool type)

/**
 ** -> result
 **
 ** @param     type   setting type to show
 **
 ** @return    the location of the help for the setting 'type'
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.3
 **/
string
Help::location(Setting::TypeUnsigned const type)
{
  string name = ::name(type);
  return operation_location("settings.html"
                            + DK::Utils::String::replace_all(name, " ", "_"));
} // string Help::location(Setting::TypeUnsigned type)

/**
 ** -> result
 **
 ** @param     type   setting type to show
 **
 ** @return    the location of the help for the setting 'type'
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.3
 **/
string
Help::location(Setting::TypeString const type)
{
  string name = ::name(type);
  return operation_location("settings.html"
                            + DK::Utils::String::replace_all(name, " ", "_"));
} // string Help::location(Setting::TypeString type)

/**
 ** -> result
 **
 ** @param     type   setting type to show
 **
 ** @return    the location of the cards order help
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.3
 **/
string
Help::location(Setting::TypeCardsOrder const type)
{
  string name = ::name(type);
  return operation_location("settings.html"
                            + DK::Utils::String::replace_all(name, " ", "_"));
} // string Help::location(Setting::TypeCardsOrder type)
