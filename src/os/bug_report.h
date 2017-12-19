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
 **********************************************************************/

#ifndef CLASS_OS_BUG_REPORT_HEADER
#define CLASS_OS_BUG_REPORT_HEADER

#include "os.h"

class GameplayAction;
namespace OS_NS {
  class BugReport;
} // namespace OS_NS 

extern OS_NS::BugReport* bug_report;

namespace OS_NS {

  extern string const BUG_REPORT_FILE;

  // The default BugReport
  class BugReport : public OS {
    public:
      // create a bug report
      string report(string const& message = NULL);
      // create a bug report
      string report(string const& message, string const& subdir);

    public:
      // constructor
      BugReport();
      // destructor
      ~BugReport();

      // add text to the bug report
      //void add(string const& text);

      // a gameplay action has happened
      void gameplay_action(GameplayAction const& action);

      // the parts of a game
      void game_open(Game const& game);
      void game_redistribute();
      void new_game();
      void game_start();
      void game_finish();

#ifdef USE_NETWORK
      // there was network traffic over the given connection
      void network_traffic(Network::Connection const& connection,
			   Network::TrafficType const traffic_type,
			   string const& text);
#endif

      string directory() const;
      string filename() const;
      string filename(string const directory) const;

    private:
      // clear the bug report data
      void clear();

    private:
      // the seed of the game
      unsigned seed;
      // the output stream (with the bug report)
      ostream* ostr;

      // the gameplay actions
      vector<GameplayAction*> actions;
      // the human gameplay actions
      vector<GameplayAction*> human_actions;
      // the game summary
      GameSummary* game_summary;
#ifdef USE_NETWORK
      // the network traffic type
      vector<Network::TrafficType> network_traffic_type;
      // the network traffic text
      vector<string> network_traffic_text;
#endif

      // number of written bug reports
      unsigned written_n;

    private: // not implemented
      BugReport(BugReport const&);
      BugReport& operator=(BugReport const&);
  }; // class BugReport

} // namespace OS_NS

#endif // #ifndef CLASS_OS_BUG_REPORT_HEADER
