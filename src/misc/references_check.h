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

#ifndef CLASS_REFERENCES_CHECK_HEADER
#define CLASS_REFERENCES_CHECK_HEADER

#include "constants.h"

#include "../game/gameplay_actions/check.h"
class Game;

#ifdef BOOST
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#endif

/**
 ** a check for reference files over a directory
 **
 ** @author	Diether Knof
 **
 ** @todo       add statistics
 ** @todo       add support for subdirectories
 **/
class ReferencesCheck {
  public:
    // the statistics for the reference checks
    class Statistics {
      public:
        // constructor
        Statistics() : no_files(0), no_checks(0), no_success(0) { }
        // reset the statistics
        void reset()
        { this->no_files = this->no_checks = this->no_success = 0; }

        // number of files checked
        int no_files;
        // number of checks made
        int no_checks;
        // number of successfully checks
        int no_success;
    }; // class Statistics

  public:
    // constructor
    ReferencesCheck();
    // constructor
    ReferencesCheck(string const& references_dir);
    // destructor
    ~ReferencesCheck();

    // set the main directory
    void set_main_directory(string const& references_main_dir);
    // set the directory
    void set_directory(string const& references_dir);

    // a check has been made
    void check(Game const& game,
               GameplayAction const& game_action,
               GameplayAction const& check_action,
               string const& comment,
               GameplayAction::Discrepancy const& discrepancy,
               bool const last_check);

    // open the next file
    bool next_reference();
    // load the reference
    bool load_reference();

    // write the statistics
    void write_statistics() const;


  private:
    // the main directory with the references
    PRIV_VAR_R(string, references_main_dir);
    // the directory with the references
    PRIV_VAR_R(string, references_dir);
    // the (remaining) subdirs with the references
    PRIV_VAR_R(vector<string>, references_subdir);
    // the file for the report
    PRIV_VAR_R(string, report_file);
#ifdef BOOST
    // iterator over the directory
    boost::filesystem::directory_iterator dir_itr;
#endif
    // the statistics
    PRIV_VAR_R(Statistics, statistics);

  private: // not to be used
    ReferencesCheck(ReferencesCheck const&);
    ReferencesCheck& operator=(ReferencesCheck const&);
}; // class ReferencesCheck

// the references check
extern ReferencesCheck* references_check;

#endif // #ifndef CLASS_REFERENCES_CHECK_HEADER
