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

#include "constants.h"

#include "../utils/file.h"
#include "../utils/string.h"

#include "bug_report.h"

#include "../party/party.h"
#include "../party/rule.h"
#include "../game/game.h"
#include "../game/gameplay.h"
#include "../game/game_summary.h"
#include "../game/gameplay_action.h"
#include "../game/gameplay_actions/player1.h"
#include "../player/aiconfig.h"
#include "../card/trick.h"
#include "../misc/setting.h"
#include "../misc/translations.h"
#include "../ui/ui.h"

#ifdef USE_NETWORK
#include "../network/connection.h"
#endif

#ifdef WINDOWS
// for 'mkdir'
#include <io.h>
#else
// for 'mkdir'
#include <sys/stat.h>
#endif
#include <ctime>

// the bug report
OS_NS::BugReport* bug_report = NULL;

namespace OS_NS {

  string const BUG_REPORTS_DIRECTORY = "FreeDoko.BugReports";
  string const BUG_REPORT_FILE = "BugReport.FreeDoko";

  /**
   ** create a report
   **
   ** @param     message   message of the user
   **
   ** @return    the filename in which the report is written
   **
   ** @author    Diether Knof
   ** 
   ** @version   0.7.4
   **/
  string
    BugReport::report(string const& message)
    {
      return this->report(message, ".");
    } // string BugReport::report(string message = NULL)

  /**
   ** create a report
   **
   ** @param     message   message of the user
   ** @param     subdir    sub directory to save into
   **
   ** @return    the filename in which the report is written
   **
   ** @author    Diether Knof
   ** 
   ** @version   0.7.3
   **/
  string
    BugReport::report(string const& message, string const& subdir)
    {
      ostream* ofstr = NULL;
      string filename;
      { // open a bug report file

        // Remark: could use a stack here
        vector<string> directories;

        directories.push_back(this->directory());
        directories.push_back(::setting(Setting::PRIVATE_DATA_DIRECTORY)
                              + "/" + BUG_REPORTS_DIRECTORY);
        directories.push_back(::setting(Setting::PRIVATE_DATA_DIRECTORY));
        directories.push_back("./" + BUG_REPORTS_DIRECTORY);
        directories.push_back(".");

        for (vector<string>::const_iterator dir = directories.begin();
             dir != directories.end();
             dir++) {
          string const directory = ((subdir.empty() || (subdir == "."))
                                    ? *dir
                                    : *dir + "/" + subdir);
          bool created_dir = false;
          if (!DK::Utils::File::isdirectory(directory)) {
#ifdef WINDOWS
            mkdir(directory.c_str());
#else
            mkdir(directory.c_str(), 00777);
#endif
            created_dir = true;
          } // if (!DK::Utils::File::isdirectory(this->directory()))

          filename = directory + "/" + this->filename(directory);
          ofstr = new ofstream(filename.c_str(), std::ios::app);
          if (!ofstr->fail()) {
            if (created_dir)
              ::ui->information(::translation("BugReport::created directory '%sdirectory%'", directory),
                                INFORMATION::NORMAL);
          } else { // if !(!ofstr->fail())
            ::ui->information(::translation("BugReport::could not save at '%sfilename%', trying '%sfilename2%'.",
                                            filename.c_str(),
                                            (*(dir + 1) + "/" + subdir + "/" + this->filename(*(dir + 1) + "/" + subdir)
                                            )),
                              INFORMATION::NORMAL);
          } // if !(!ofstr->fail())
          if (!ofstr->fail())
            break;
        } // for (dir \in directories)
        if (ofstr->fail()) {
          cerr << ::translation("BugReport::could not save")
            << endl;
          cerr << ::translation("Press <Return> to display the bug report.")
            << endl;
          std::cin.get();
          ofstr = &cerr;
          filename = "standard error output";
        } // if (ofstr->fail())
      } // open a bug report file

      const time_t time_now = time(NULL);

      string const bug_report_output
        = static_cast<ostringstream*>(this->ostr)->str();

      { // write the version, system, time and message
        *ofstr << "# FreeDoko Bugreport\n"
          << '\n'
          << "version: " << *::version
          << '\n'
          << "compiled: " << __DATE__ << ", " << __TIME__ << '\n'
#ifdef WINDOWS
          << "system: Windows\n"
#endif
#ifdef LINUX
          << "system: Linux\n"
#endif
#ifdef HPUX
          << "system: HPUX\n"
#endif
          << "time: " << asctime(localtime(&time_now)) << "\n"
          << "language: " << ::setting(Setting::LANGUAGE) << "\n";

        if (   (::game_status & GAMESTATUS::GAME)
            && (::game_status != GAMESTATUS::GAME_NEW))
          *ofstr << "trick: " << this->game().trick_current_no() << '\n';

        *ofstr << '\n'
          << "/---------\\\n"
          << "| message |\n"
          << "\\---------/\n"
          << message << "\n\n"
          << "/--------\\\n"
          << "| output |\n"
          << "\\--------/\n";
      } // write the version, system, time and message
      if (bug_report_output.empty())
        *ofstr << "finished\n";
      if (!bug_report_output.empty()) {
        *ofstr << bug_report_output;
        if (   (::game_status == GAMESTATUS::GAME_RESERVATION)
            || (::game_status == GAMESTATUS::GAME_POVERTY_SHIFT) ) {
          *ofstr << '\n'
            << "hands\n"
            << "{\n";
          for (vector<Player*>::const_iterator player
               = this->game().players_begin();
               player != this->game().players_end();
               player++) {
            *ofstr << "Player " << this->game().no(**player)
              << " (" << (*player)->name() << ")\n"
              << (*player)->hand()
              << '\n';
          } // for (player)
          *ofstr << "}\n";
        } // if (::game_status == GAME_POVERTY_SHIFT or GAME_RESERVATION)
        { // write the gameplay actions
          *ofstr << '\n'
            << "gameplay actions\n"
            << "{\n";
          unsigned t = 0;
          for (vector<GameplayAction*>::const_iterator a = this->actions.begin();
               a != this->actions.end();
               ++a) {
            if ((*a)->type == GameplayAction::TRICK_OPEN) {
              t += 1;
              *ofstr << "  # trick " << t << '\n';
            } else {
              *ofstr << "  " << **a << '\n';
            }
          }
          *ofstr << "}\n";
        } // write the gameplay actions

        { // write the game summary
          if (this->game_summary) {

            *ofstr << '\n'
              << "game summary\n"
              << "{\n"
              << *this->game_summary
              << "}\n";

            { // special points
              *ofstr << "special points\n"
                << "{\n"
                << "\tteam\tvalue\tdescription:\n";
              for (Specialpointsvector::const_iterator
                   special_point = this->game_summary->specialpoints().begin();
                   special_point != this->game_summary->specialpoints().end();
                   special_point++) {
                *ofstr << '\t';
                *ofstr << special_point->team;
                // extra rule
                if ((special_point->type == SPECIALPOINT::NO120_SAID)
                    && (this->game().rule()(Rule::ANNOUNCEMENT_RE_DOUBLES)))
                  *ofstr << "\t" << "*2";
                else
                  *ofstr << "\t"
                    << Value_of_Specialpoint(special_point->type);
                *ofstr << "\t" << name(special_point->type)
                  << '\n';
              } // for (special_point)
              *ofstr << "\t= " << this->game_summary->points() << '\n';

              *ofstr << "}\n";

            } // special points
          } // if (this->game_summary)
        } // write the game summary

#ifdef OUTDATED
        // 2011-03-17 dknof
        { // write the current hands
          if (   (::game_status & GAMESTATUS::GAME)
              && (::game_status != GAMESTATUS::GAME_NEW)) {
            *ofstr << '\n'
              << "current hands\n"
              << "{\n";
            for (vector<Player*>::const_iterator player
                 = this->game().players_begin();
                 player != this->game().players_end();
                 player++) {
              *ofstr << "Player " << (*player)->no()
                << " (" << (*player)->name() << ")\n"
                << (*player)->hand()
                << '\n';
            } // for (player)
            *ofstr << "}\n";

            // write the current trick
            if (this->game().trick_current_no() != UINT_MAX) {
              *ofstr << '\n'
                << "current trick\n"
                << this->game().trick_current();
            } // if (this->game().trick_current_no() != UINT_MAX) 
          } // if (game_status & GAME)
        } // write the current hands
#endif

#ifdef OUTDATED
        // 2011-03-17 dknof
        { // write the human actions
          *ofstr << "\n\nhuman actions\n"
            << "{\n";
          for (vector<GameplayAction*>::const_iterator
               a = this->human_actions.begin();
               a != this->human_actions.end();
               ++a)
            *ofstr << "  " << **a << '\n';
          *ofstr << "}\n";
        } // write the human actions
#endif

#ifdef USE_NETWORK
        { // write the network log
          DEBUG_ASSERTION((this->network_traffic_type.size()
                           == this->network_traffic_text.size()),
                          "BugReport::write()\n"
                          "  size of network traffic type and network traffic text size do not match");

          if (!this->network_traffic_type.empty()) {
            vector<Network::TrafficType>::const_iterator type
              = this->network_traffic_type.begin();
            vector<string>::const_iterator text
              = this->network_traffic_text.begin();

            *ofstr << "\n"
              << "network log\n"
              << "{\n";
            for (; type != this->network_traffic_type.end(); ++type, ++text)
              *ofstr << "  " << *type << ": " << *text << '\n';
            *ofstr << "}\n";
          } // if (!this->network_traffic_type.empty())
        } // write the network log
#endif // #ifdef USE_NETWORK
      } // if (!bug_report_output.empty())

      // close the file stream
      *ofstr << flush;
      delete ofstr;
      ofstr = NULL;


      // I don't know, how to reinitialize 'ostrstream'
      delete(this->ostr);
      this->ostr = new ostringstream();

      // copy the old data
      *(this->ostr) << bug_report_output;

      // one more bug report written
      this->written_n += 1;

      return filename;
    } // virtual string BugReport::report(string message, string subdir)

  /**
   ** Constructor
   **
   ** @param     -
   **
   ** @return    -
   **
   ** @author    Diether Knof
   ** 
   ** @version   0.6.7
   **/
  BugReport::BugReport() :
    OS(OS_TYPE::OS_BUG_REPORT),
    seed(0),
    ostr(NULL),
    actions(),
    human_actions(),
    game_summary(NULL),
#ifdef USE_NETWORK
    network_traffic_type(),
    network_traffic_text(),
#endif
    written_n(0)
    {
      this->ostr = new ostringstream();

      return ;
    } // BugReport::BugReport()

  /**
   **
   ** Destructor
   **
   ** @param     -
   **
   ** @return    -
   ** 
   ** @version   0.6.7
   **
   ** @author    Diether Knof
   **
   **/
  BugReport::~BugReport()
  {
    this->clear();
    delete this->ostr;

    return ;
  } // BugReport::~BugReport()

#if 0
  /**
   **
   ** adds the text to the bug report
   **
   ** @param     text	text to add
   **
   ** @return    -
   **
   ** @version   0.6.7
   **
   ** @author    Diether Knof
   **
   **/
  void
    BugReport::add(string const& text)
    {
      *(this->ostr) << text;

      return ;
    } // void BugReport::add(string const& text)
#endif

  /**
   ** the game is opened
   **
   ** @param     game	opened game
   **
   ** @return    -
   **
   ** @author    Diether Knof
   ** 
   ** @version   0.6.7
   **/
  void
    BugReport::game_open(Game const& game)
    {
      this->OS::game_open(game);

      this->new_game();
      return ;
    } // void BugReport::game_open(Game const& game)

  /**
   **
   ** the game is redistributed
   **
   ** @param     -
   **
   ** @return    -
   ** 
   ** @version   0.6.7b
   **
   ** @author    Diether Knof
   **
   **/
  void
    BugReport::game_redistribute()
    {
      this->new_game();
      return ;
    } // void BugReport::game_redistribute()

  /**
   ** the game is opened
   **
   ** @param     -
   **
   ** @return    -
   **
   ** @author    Diether Knof
   ** 
   ** @version   0.6.7
   **/
  void
    BugReport::new_game()
    {
      this->clear();

      this->seed = this->game().seed();

      *this->ostr << "seed: " << this->game().seed() << '\n'
        << "startplayer: " << this->game().no(this->game().startplayer()) << '\n'
        << '\n';
      {
        bool const write_database_bak = Player::write_database;
        Player::write_database = false;
        bool const write_standard_type_bak = Aiconfig::write_standard_type;
        Aiconfig::write_standard_type = false;

        *this->ostr << "party\n"
          << "{\n"
          << this->party()
          << "}\n";

        Aiconfig::write_standard_type = write_standard_type_bak;
        Player::write_database = write_database_bak;
      }
#ifdef OUTDATED
      // 0.7.8 2009-10-03
      // the whole party is now saved in the bug report
      *this->ostr << "rules\n"
        << "{\n"
        << this->game().rule()
        << "}\n";

      { // write the players
        *(this->ostr) <<'\n'
          <<"players\n"
          << "{\n";

        bool const write_standard_type_bak = Aiconfig::write_standard_type;
        Aiconfig::write_standard_type = false;

        for (vector<Player*>::const_iterator player
             = this->game().players_begin();
             player != this->game().players_end();
             player++) {
          bool const write_database_bak = Player::write_database;
          Player::write_database = false;
          *(this->ostr) << "{\n"
            << **player
            << "}\n"
            << '\n';
          Player::write_database = write_database_bak;
        } // for (player \in this->game().players())

        Aiconfig::write_standard_type = write_standard_type_bak;

        *(this->ostr) << "}\n";
      } // write the players
#endif

      return ;
    } // void BugReport::new_game()

  /**
   ** the game is started
   **
   ** @param     -
   **
   ** @return    -
   **
   ** @author    Diether Knof
   ** 
   ** @version   0.6.7
   ** 
   ** @todo      poverty
   **/
  void
    BugReport::game_start()
    {
      { // write the game type
        *(this->ostr) << '\n'
          << "gametype: " << this->game().type()
          << '\n';

        switch (this->game().type()) {
        case GAMETYPE::NORMAL:
          break;

        case GAMETYPE::MARRIAGE_SOLO:
        case GAMETYPE::MARRIAGE: {
          *(this->ostr)
            << "selector: " << name(this->game().marriage_selector()) << '\n'
            << "bride: player "
            << this->game().no(this->game().soloplayer())
            << '\n';
        } break;

        case GAMETYPE::POVERTY: {
          // ToDo
        } break;

        case GAMETYPE::THROWN_NINES:
        case GAMETYPE::THROWN_KINGS:
        case GAMETYPE::THROWN_NINES_AND_KINGS:
        case GAMETYPE::THROWN_RICHNESS:
        case GAMETYPE::FOX_HIGHEST_TRUMP:
        case GAMETYPE::GENSCHER:
        case GAMETYPE::MARRIAGE_SILENT:
        case GAMETYPE::SOLO_MEATLESS:
        case GAMETYPE::SOLO_JACK:
        case GAMETYPE::SOLO_QUEEN:
        case GAMETYPE::SOLO_KING:
        case GAMETYPE::SOLO_QUEEN_JACK:
        case GAMETYPE::SOLO_KING_JACK:
        case GAMETYPE::SOLO_KING_QUEEN:
        case GAMETYPE::SOLO_KOEHLER:
        case GAMETYPE::SOLO_CLUB:
        case GAMETYPE::SOLO_HEART:
        case GAMETYPE::SOLO_SPADE:
        case GAMETYPE::SOLO_DIAMOND: {
          *(this->ostr) << "soloplayer: player "
            << this->game().no(this->game().soloplayer())
            << '\n';
          break;
        }
        } // switch (this->game().type())
      } // write the game type

      { // write the hands
        *(this->ostr) <<'\n'
          <<"hands\n"
          << "{\n";

        for (vector<Player*>::const_iterator player
             = this->game().players_begin();
             player != this->game().players_end();
             player++)
          *(this->ostr) << "player " << this->game().no(**player)
            << " (" << (*player)->name()
            << ")\n"
            << const_cast<Player const*>(*player)->hand()
            << '\n';

        *(this->ostr) << "}\n";
      } // write the hands

      return ;
    } // void BugReport::game_start()

  /**
   ** the game is finished
   **
   ** @param     -
   **
   ** @return    -
   **
   ** @author    Diether Knof
   ** 
   ** @version   0.7.3
   **/
  void
    BugReport::game_finish()
    { 
      this->OS::game_finish();

      this->game_summary = new GameSummary(this->party().last_game_summary());

      return ;
    } // void BugReport::game_finish()

  /**
   ** a gameplay action has happened
   **
   ** @param     action    gameplay action
   **
   ** @return    -
   **
   ** @author    Diether Knof
   ** 
   ** @version   0.7.3
   **/
  void
    BugReport::gameplay_action(GameplayAction const& action)
    {
      this->OS::gameplay_action(action);

      // do not save all actions
      switch (action.type) {
      case GameplayAction::POVERTY_DENIED_BY_ALL:
      case GameplayAction::TRICK_TAKEN:
      case GameplayAction::CHECK:
      case GameplayAction::PRINT:
        // do not save
        return ;

      case GameplayAction::RESERVATION:
      case GameplayAction::POVERTY_SHIFT:
      case GameplayAction::POVERTY_ACCEPTED:
      case GameplayAction::POVERTY_RETURNED:
      case GameplayAction::POVERTY_DENIED:
      case GameplayAction::TRICK_OPEN:
      case GameplayAction::CARD_PLAYED:
      case GameplayAction::ANNOUNCEMENT:
      case GameplayAction::SWINES:
      case GameplayAction::HYPERSWINES:
      case GameplayAction::MARRIAGE:
      case GameplayAction::GENSCHER:
      case GameplayAction::TRICK_FULL:
        // do save
        break;
      } // switch (action.type)

      this->actions.push_back(action.clone());

      if (dynamic_cast<GameplayAction::Player1 const*>(&action))
        if (this->game().player(static_cast<GameplayAction::Player1 const&>(action).player).type()
            == Player::HUMAN)
          this->human_actions.push_back(action.clone());

      return ;
    } // void BugReport::gameplay_action(GameplayAction action)

#ifdef USE_NETWORK
  /**
   ** there was traffic over the network
   **
   ** @param     -
   **
   ** @return    -
   ** 
   ** @author    Diether Knof
   **
   ** @version   0.6.7
   **/
  void
    BugReport::network_traffic(Network::Connection const& connection,
                               Network::TrafficType const traffic_type,
                               string const& text)
    {
      this->network_traffic_type.push_back(traffic_type);
      this->network_traffic_text.push_back(text);
      return ;
    } // void BugReport::network_traffic(Network::Connection connection, Network::TrafficType traffic_type, string text)
#endif // #ifdef USE_NETWORK

  /**
   ** clear the bug report stream and make a backup before
   **
   ** @param     -
   **
   ** @return    -
   ** 
   ** @author    Diether Knof
   **
   ** @version   0.6.7
   **/
  void
    BugReport::clear()
    {
      this->seed = UINT_MAX;
      delete this->ostr;
      this->ostr = new ostringstream();

      for (vector<GameplayAction*>::iterator a = this->actions.begin();
           a != this->actions.end();
           ++a)
        delete *a;
      this->actions.clear();

      for (vector<GameplayAction*>::iterator a = this->human_actions.begin();
           a != this->human_actions.end();
           ++a)
        delete *a;
      this->human_actions.clear();

      delete this->game_summary;
      this->game_summary = NULL;

#ifdef USE_NETWORK
      this->network_traffic_type.clear();
      this->network_traffic_text.clear();
#endif

      return ;
    } // virtual void BugReport::clear()

  /**
   ** -> result
   **
   ** @param     -
   **
   ** @return    the directory to save the bug report in
   **
   ** @author    Diether Knof
   ** 
   ** @version   0.6.7
   **/
  string
    BugReport::directory() const
    {
      if (::setting(Setting::SAVE_BUG_REPORTS_ON_DESKTOP)) {
        return (DK::Utils::File::desktop_directory()
                + "/" + BUG_REPORTS_DIRECTORY);
      } // if (::setting(Setting::SAVE_BUG_REPORTS_ON_DESKTOP))

      return (::setting(Setting::PRIVATE_DATA_DIRECTORY)
              + "/" + BUG_REPORTS_DIRECTORY);
    } // virtual void BugReport::directory()

  /**
   ** -> result
   **
   ** @param     -
   **
   ** @return    the filename (BUG_REPORT_FILE.`seed`)
   ** 
   ** @author    Diether Knof
   **
   ** @version   0.6.7
   **/
  string
    BugReport::filename() const
    {
      return this->filename(this->directory());
    } // virtual void BugReport::filename()

  /**
   ** -> result
   **
   ** @param     directory   the directory to save the bug report in
   **
   ** @return    the filename (`seed`.BUG_REPORT_FILE)
   **
   ** @author    Diether Knof
   ** 
   ** @version   0.7.5
   **/
  string
    BugReport::filename(string const directory) const
    {
      // The counter 'written_n' is used so that multiple creating of
      // a bug report with moving the previous one does lead to different names.
      string filename = (((::game_status >= GAMESTATUS::GAME_INIT)
                          ? DK::Utils::String::to_string(this->game().seed(), 6, '0')
                          : string("party")
                         )
                         + ( (this->written_n > 0)
                            ? "_" + DK::Utils::String::to_string(this->written_n)
                            : "")
                         + "." + BUG_REPORT_FILE);

      // Check whether the file does not exists, yet.
      // If the file does already exists,
      // append a number and search the first which does not exists.
      for (unsigned n = this->written_n;
           DK::Utils::File::isfile(directory + "/" + filename);
           n += 1,
           filename = (((::game_status >= GAMESTATUS::GAME_INIT)
                        ? DK::Utils::String::to_string(this->game().seed(), 6, '0')
                        : string("party")
                       ) + "_" + DK::Utils::String::to_string(n)
                       + "." + BUG_REPORT_FILE))
        ;

      return filename;
    } // void BugReport::filename(string directory)

} // namespace OS_NS
