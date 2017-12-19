/**********************************************************************
 *
 *   FreeDoko a Doppelkopf-Game
 *    
 *   Copyright (C) 2001-2017 by Diether Knof and Borg Enders
 *
 *   This program is free software; you can redistribute it and/or 
 *   modify it under the terms of the GNU General Public Rules as 
 *   published by the Free Software Foundation; either version 2 of 
 *   the Rules, or (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public Rules for more details. 
 *   You can find this license in the file 'gpl.txt'.
 *
 *   You should have received a copy of the GNU General Public Rules
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

#ifdef USE_UI_GTKMM

#include "filemenu_file.h"

namespace Gtk {

  /**
   ** constructor
   **
   ** @param	signal_slot    signal slot
   ** @param	file      file to filter for
   **
   ** @return	-
   **/
  FileMenuFilterFile::FileMenuFilterFile(sigc::slot1<void, string> signal_slot,
                                         string const& file) :
    FileMenu(signal_slot),
    file(file)
  {  }

  /**
   ** destructor
   **
   ** @param	-
   **
   ** @return	-
   **/
  FileMenuFilterFile::~FileMenuFilterFile()
  { }

  /**
   ** create a new file menu
   **
   ** @param	directory    new directory
   **
   ** @return	new file menu
   **/
  FileMenu* 
    FileMenuFilterFile::create_new(string const& directory) const
    {
      auto file_menu = new FileMenuFilterFile(this->signal_slot_,
                                              this->file);
      file_menu->set_directory(directory);
      return file_menu;
    }

  /**
   ** -> result
   **
   ** @param	filename   filename to test for
   **
   ** @return	whether the filename is accepted
   **/
  bool
    FileMenuFilterFile::accept(string const& filename) const
    {
      if (this->file == ".")
        return (Glib::file_test(filename, Glib::FILE_TEST_IS_DIR));
      else
        return (Glib::file_test(filename, Glib::FILE_TEST_IS_DIR)
                && Glib::file_test(filename + "/" + this->file,
                                   Glib::FILE_TEST_IS_REGULAR));
    } // bool FileMenuFilterFile::accept(string filename) const

} // namespace Gtk

#endif // #ifdef USE_UI_GTKMM
