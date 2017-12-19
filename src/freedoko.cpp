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
 ********************************************************************/


/**
 **
 ** Alpha-Version of FreeDoko 
 **
 **   developed since fall 2001
 **
 **/

#include "constants.h"
#include <ctime>
#include <unistd.h>
#ifdef WINDOWS
// for 'mkdir'
#include <io.h>
#else
// for 'mkdir'
#include <sys/stat.h>
#endif

#include "class/getopt/getopt.h"

#include "utils/file.h"
#include "utils/string.h"

#include "utils.h"
#include "basistypes.h"

#include "misc/setting.h"
#include "misc/translations.h"
#include "misc/bug_report.h"
#include "misc/references_check.h"
#ifdef USE_NETWORK
#ifdef USE_NETWORK_DOKOLOUNGE
#include "misc/lounge.h"
#endif
#endif

#include "party/party.h"
#include "player/aiconfig.h"

#include "ui/ui.wrap.h"
#include "ui/ui.dummy.h"

#ifdef USE_SOUND
#include "sound/sound.h"
#endif

#ifdef USE_NETWORK
#include "network/server.h"
#include "network/FreeDoko/interpreter.h"
#include "os/network_log.h"
#endif

#include "os/seed.h"
#include "os/party_points.h"
#include "os/bug_report.h"
#include "os/bug_report_replay.h"
#include "os/auto_bug_report.h"
#include "os/gameplay.h"

#ifdef CHECK_RUNTIME
#include "runtime.h"
#endif

DK::Utils::Version const* version = nullptr;
vector<DK::Utils::Version const*> all_versions;
// the user interface
UI* ui = nullptr;

#include "text/gpl.string"

// the translator (ToDo: convert in pointer)
vector<string> Translator::directories;
vector<string> Translator::names;
Translator translator;

// the date of the exipration of the InnoCard cardset
DK::Utils::Date const Setting::INNOCARD_EXPIRATION_DATE(2017, 12, 31);
// the date of the exipration of the Altenburg cardset
DK::Utils::Date const Setting::ALTENBURG_EXPIRATION_DATE(2013, 12, 31);
// the date of the exipration of the Elite cardset
DK::Utils::Date const Setting::ELITE_EXPIRATION_DATE(2009, 12, 31);
// the settings (ToDo: convert in pointer, move Translator::* above to translator.cpp)
Setting setting;

// the party (ToDo: convert in pointer)
Party party;

#include "utils.h"


#ifndef WINDOWS
int
main(int argc, char* argv[])
#else
  // ToDo: use 'WinMain'
int
main(int argc, char* argv[])
#endif
{
#ifdef USE_THREADS
  cerr
    << "FreeDoko\n"
    << "  Thread support is experimental still.\n"
    << "  Only use multiple threads if you want to enjoy a crash now and then.\n";
#endif

  ::ui = new UI_Wrap;
  ::bug_report = new OS_NS::BugReport;
#ifdef USE_NETWORK
#ifdef USE_NETWORK_DOKOLOUNGE
  ::lounge = new Lounge;
#endif
#endif

  try {
    // game status
    ::game_status = GAMESTATUS::PROGRAMSTART;

    using DK::Utils::Version;
    using DK::Utils::Date;


    // make an assertion create a bug report
    ::debug_function = &create_assertion_bug_report;

    // list of all Versions
    ::all_versions.push_back(new Version(0,6,3,     Date(2004,  5,  5), true));
    ::all_versions.push_back(new Version(0,6,4,     Date(2004,  6,  1), true));
    ::all_versions.push_back(new Version(0,6,5,     Date(2004,  6, 24), true));
    ::all_versions.push_back(new Version(0,6,5,'b', Date(2004,  6, 28), true));
    ::all_versions.push_back(new Version(0,6,6,     Date(2004,  9, 25), true));
    ::all_versions.push_back(new Version(0,6,7,     Date(2004, 12, 16), true));
    ::all_versions.push_back(new Version(0,6,7,'b', Date(2004, 12, 20), true));
    ::all_versions.push_back(new Version(0,6,8,     Date(2005,  4, 11), true));
    ::all_versions.push_back(new Version(0,6,9,     Date(2005,  6, 30), true));
    ::all_versions.push_back(new Version(0,7,0,     Date(2005,  8, 22), true));
    ::all_versions.push_back(new Version(0,7,1,     Date(2005, 11, 12), true));
    ::all_versions.push_back(new Version(0,7,2,     Date(2006,  1, 23), true));
    ::all_versions.push_back(new Version(0,7,2,'b', Date(2006,  2,  9), true));
    ::all_versions.push_back(new Version(0,7,3,     Date(2006,  8, 10), true));
    ::all_versions.push_back(new Version(0,7,4,     Date(2007,  1, 18), true));
    ::all_versions.push_back(new Version(0,7,5,     Date(2008, 10, 26), true));
    ::all_versions.push_back(new Version(0,7,6,     Date(2009,  8,  1), true));
    ::all_versions.push_back(new Version(0,7,6,'b', Date(2009,  8, 16), true));
    ::all_versions.push_back(new Version(0,7,7,     Date(2009,  9, 27), true));
    ::all_versions.push_back(new Version(0,7,8,     Date(2010,  3, 13), true));
    ::all_versions.push_back(new Version(0,7,9,     Date(2011,  3,  6), true));
    ::all_versions.push_back(new Version(0,7,10,    Date(2011,  8, 14), true));
    ::all_versions.push_back(new Version(0,7,11,    Date(2011, 12, 18), true));
    ::all_versions.push_back(new Version(0,7,12,    Date(2013, 12,  7), true));
    ::all_versions.push_back(new Version(0,7,13,    Date(2014, 12, 29), true));
    ::all_versions.push_back(new Version(0,7,14,    Date(2016,  1, 16), true));
    ::all_versions.push_back(new Version(0,7,15,    Date(2016, 12, 30), true));
    ::all_versions.push_back(new Version(0,7,16,    Date(2017,  1, 10), true
#ifdef VERSION_DESCRIPTION
                                         , VERSION_DESCRIPTION
#endif
                                        ));
    ::version = ::all_versions.back();

    // initialisation for random functions
    ::srand(static_cast<unsigned>(::time(nullptr) % RAND_MAX));

#ifdef USE_NETWORK
    // open the network
    ::server = new Network::Server();
    static_cast<UI_Wrap*>(::ui)->append(::server);
    // if not empty, a connection for the program start
    string network_connection;

    // log the network traffic into files
#ifdef DKNOF
    static_cast<UI_Wrap*>(::ui)->append(new_OS(OS_TYPE::NETWORK_LOG));
#endif
#endif // #ifdef USE_NETWORK

    // the ui selected by command line option
    UI* selected_ui = nullptr;

    { // parse options
      // This file contains the help you get calling the program with '-?'.
      // It includes what arguments you can use.

      // load the settings
      ::setting.add_data_directory(DK::Utils::File::dirname(argv[0]));
#ifndef RELEASE
      ::setting.add_data_directory(DK::Utils::File::dirname(argv[0]) + string("/../data"));
#endif // #ifndef RELEASE
      ::setting.load();

      ::party.set_seed(SEED_START);
      ::party.set_startplayer(STARTPLAYER);

      // parse the options
      GetOpt::Option option;
      while (option =
             GetOpt::getopt(argc, argv,
                            "help",             '?',  GetOpt::Syntax::BOOL,
                            "hilfe",            'h',  GetOpt::Syntax::BOOL,
                            "version",          'v',  GetOpt::Syntax::BOOL,
                            "debug",           '\0',  GetOpt::Syntax::BOOL,
                            "license",          'L',  GetOpt::Syntax::BOOL,
                            "defines",          '\0', GetOpt::Syntax::BOOL,
                            "directories",      '\0', GetOpt::Syntax::BOOL,
                            "ui",               'u',  GetOpt::Syntax::BSTRING,
                            "settings",         '\0', GetOpt::Syntax::BSTRING,
                            "name",             'n',  GetOpt::Syntax::BSTRING,
                            "language",         'l',  GetOpt::Syntax::BSTRING,
                            "cardset",          'C',  GetOpt::Syntax::BSTRING,
                            "cards-height",     'H',  GetOpt::Syntax::UNSIGNED,
                            "seed",             's',  GetOpt::Syntax::UNSIGNED,
                            "bug-report",       'b',  GetOpt::Syntax::BSTRING,
                            "references",       'r',  GetOpt::Syntax::BSTRING,
                            "auto-bug-reports", '\0', GetOpt::Syntax::BOOL,
                            "no-automatic-savings", '\0', GetOpt::Syntax::BOOL,
                            "fast-play",        'F',  GetOpt::Syntax::INT,
                            "seed-info",         '\0',  GetOpt::Syntax::BOOL,
                            "save-standard-aiconfig",  '\0',  GetOpt::Syntax::BOOL,
#ifdef USE_THREADS
                            "threads",          'T',  GetOpt::Syntax::UNSIGNED,
#endif
#ifdef USE_NETWORK
                            "start-server",     '\0', GetOpt::Syntax::BOOL,
                            "connect",          '\0', GetOpt::Syntax::BSTRING,
#ifdef USE_NETWORK_DOKOLOUNGE
                            "dokolounge",          '\0', GetOpt::Syntax::BOOL,
#endif
#endif
                            // end of arguments
                            "", 0, GetOpt::Syntax::END
                              ) // { }
    ) {

      if (option.fail()) {
        // wrong usage
        cerr << argv[0] << "\n"
          << "wrong usage: "
          << option.error() << " " << option.value_string() << "\n"
          << "For the syntax type '" << argv[0] << " -?'"
          << endl;

        return EXIT_FAILURE;
      } // if (option.fail())

      if (   (option.name() == "help") 
          || (option.name() == "hilfe")) {
        // output of the help
#include "text/help.string"

        cout << help_string << endl;
        return EXIT_SUCCESS;
      } else if (option.name() == "version") {
        // output of the version
        cout << "FreeDoko " << *::version << "\n"
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
          << "compiler: "
#ifdef __GNUG__
          << "g++"
#endif
          << '\n'
#ifdef __VERSION__
          << "  version: " << __VERSION__ << '\n'
#endif
#ifdef __cplusplus
          << "  C++ version: " << __cplusplus << '\n'
#endif
          ;
        return EXIT_SUCCESS;
      } else if (option.name() == "license") {
        // output of the license (GPL)
        cout << "FreeDoko -- License:\n\n"
          << GPL_string
          << '\n'
          << '\n'
          << "----------\n"
          << '\n'
          << "Cardset '" << ::setting(Setting::CARDSET) << "' license:\n"
          << ::setting(Setting::CARDSET_LICENSE)
          << endl;
        return EXIT_SUCCESS;
      } else if (option.name() == "defines") {
#ifdef VERSION_DESCRIPTION
        cout << "VERSION_DESCRIPTION = " << VERSION_DESCRIPTION << '\n';
#endif
#ifdef RELEASE
        cout << "RELEASE\n";
#endif
#ifdef ASSERTION_GENERATES_SEGFAULT
        cout << "ASSERTION_GENERATES_SEGFAULT\n";
#endif
#ifdef DKNOF
        cout << "DKNOF\n";
#endif
#ifdef BENDERS
        cout << "BENDERS\n";
#endif
        cout << '\n';
        cout << "directories:\n";
#ifdef PUBLIC_DATA_DIRECTORY_VALUE
        cout << "  PUBLIC_DATA_DIRECTORY_VALUE = " << PUBLIC_DATA_DIRECTORY_VALUE << '\n';
#endif
#ifdef MANUAL_DIRECTORY_VALUE
        cout << "  MANUAL_DIRECTORY_VALUE      = " << MANUAL_DIRECTORY_VALUE << '\n';
#endif
        cout << '\n';
        cout << "modules:\n";
#ifdef USE_UI_TEXT
        cout << "  USE_UI_TEXT  = " << USE_UI_TEXT << '\n';
#endif
#ifdef USE_UI_GTKMM
        cout << "  USE_UI_GTKMM = " << USE_UI_GTKMM << '\n';
#endif
#ifdef USE_SOUND
        cout << "  USE_SOUND    = " << USE_SOUND << '\n';
#ifdef USE_SOUND_ALUT
        cout << "    USE_SOUND_ALUT      = " << USE_SOUND_ALUT << '\n';
#endif
#ifdef USE_SOUND_COMMAND
        cout << "    USE_SOUND_COMMAND   = " << USE_SOUND_COMMAND << '\n';
#endif
#ifdef USE_SOUND_PLAYSOUND
        cout << "    USE_SOUND_PLAYSOUND = " << USE_SOUND_PLAYSOUND << '\n';
#endif
#endif
#ifdef USE_NETWORK
        cout << "  USE_NETWORK  = " << USE_NETWORK << '\n';
#endif
#ifdef USE_THREADS
        cout << "  USE_THREADS  = " << USE_THREADS << "\n";
#endif
#ifdef NO_UPDATE_ON_DEMAND
        cout << "  NO_UPDATE_ON_DEMAND  = " << NO_UPDATE_ON_DEMAND << "\n";
#endif
        return EXIT_SUCCESS;
      } else if (option.name() == "directories") {
        { // current directory
          char current_dir[PATH_MAX + 1];
          current_dir[PATH_MAX] = '\0';
          if (getcwd(current_dir, PATH_MAX) != nullptr)
            cout << "current directory\n"
              << "  " << current_dir << '\n';
        } // current directory
        { // data directory
          cout << "data directories\n";
          vector<string> const directories
            = ::setting.data_directories();
          for (vector<string>::const_iterator
               d = directories.begin();
               d != directories.end();
               ++d)
            cout << "  " << *d << '\n';
        } // data directory
        return EXIT_SUCCESS;
      } else if (option.name() == "debug") {
        static_cast<UI_Wrap*>(ui)->append(new_OS(OS_TYPE::GAMEPLAY));
      } else if (option.name() == "ui") {
        delete selected_ui;
        selected_ui = nullptr;
        if (option.value_string() == "none")
          selected_ui = UI::new_(UI_TYPE::DUMMY);
#ifdef USE_UI_TEXT
        else if (option.value_string() == "text")
          selected_ui = UI::new_(UI_TYPE::TEXT);
#endif
#ifdef USE_UI_GTKMM
        else if (option.value_string() == "gtkmm")
          selected_ui = UI::new_(UI_TYPE::GTKMM_DOKO);
#endif
        else {
          cerr << "ui '" << option.value_string() << "' not implemented.\n"
            << "exiting";
          return EXIT_FAILURE;
        } // if (option.value_string() == "...")

      } else if (option.name() == "settings") {
        setting.load(option.value_string());

      } else if (option.name() == "name") {
        setting.set(Setting::NAME, option.value_string());

      } else if (option.name() == "language") {
        setting.set(Setting::LANGUAGE, option.value_string());

      } else if (option.name() == "cards-height") {
        setting.set(Setting::CARDS_HEIGHT,
                    option.value(GetOpt::Option::UNSIGNED));

      } else if (option.name() == "cardset") {
        setting.set(Setting::CARDSET, option.value_string());

      } else if (option.name() == "seed") {
        ::party.set_seed(option.value(GetOpt::Option::UNSIGNED));

      } else if (option.name() == "bug-report") {
        OS_NS::BugReportReplay* bug_report_replay
          = new OS_NS::BugReportReplay(option.value_string(),
                                       OS_NS::BugReportReplay::VERBOSE_ALL);
        if (!bug_report_replay->loaded()) {
          cerr << "Error loading the bug report '" << option.value_string()
            << "'" << endl;
          delete bug_report_replay;
        } else {
          ::game_status = GAMESTATUS::PARTY_INITIAL_LOADED;
        }
      } else if (option.name() == "references") {
        if (::references_check)
          delete ::references_check;
        ::references_check = new ReferencesCheck(option.value_string());
        ::game_status = GAMESTATUS::PARTY_INITIAL_LOADED;
        ::FAST_PLAY = FAST_NS::ALL;
      } else if (option.name() == "auto-bug-reports") {
        // automatic bug reports
        static_cast<UI_Wrap*>(ui)->append(new OS_NS::AutoBugReport);
      } else if (option.name() == "no-automatic-savings") {
        setting.set(Setting::AUTOMATIC_SAVINGS,
                    !option.value(GetOpt::Option::BOOL));
      } else if (option.name() == "fast-play") {
        // PAUSE          = 1,                 // skip pauses
        // PLAYER         = PAUSE         << 1 // change human players to ais
        // RANDOM_AI      = PLAYER        << 1, // set the players to the random ai
        // FULL_TRICK     = RANDOM_AI     << 1 // skip full trick window
        // PARTY_START    = FULL_TRICK    << 1 // automatical start party
        // GAME_FINISHED  = PARTY_START   << 1 // skip game finished dialog
        // SHOW_ALL_HANDS = GAME_FINISHED << 1 // show all hands

        FAST_PLAY |= option.value(GetOpt::Option::INT);

      } else if (option.name() == "seed-info") {
        if (option.value(GetOpt::Option::BOOL))
          FAST_PLAY |= FAST_NS::SEED_INFO;
        else
          FAST_PLAY &= ~FAST_NS::SEED_INFO;
      } else if (option.name() == "save-standard-aiconfig") {
        Aiconfig aiconfig;
        aiconfig.set_to_difficulty(Aiconfig::STANDARD_DEFENSIVE);
        string path = (::setting(Setting::PRIVATE_DATA_DIRECTORY)
                       + "/"
                       + ::setting(Setting::AI_DIRECTORY)
                       + "/standard.defensive.saved"
                      );
#ifdef WINDOWS
        DK::Utils::String::replace_all(path, "/", "\\");
#endif
        if (aiconfig.save(path))
          cout << "Aiconfig 'standard defensive' saved in " << path << endl;
        return EXIT_SUCCESS;

#ifdef USE_THREADS
      } else if (option.name() == "threads") {
        setting.set(Setting::THREADS_MAX,
                    option.value(GetOpt::Option::UNSIGNED));
#endif // #ifdef USE_THREADS
#ifdef USE_NETWORK
      } else if (option.name() == "start-server") {
        ::server->create_listener(Network::Server::FREEDOKO_STANDARD_PORT);

      } else if (option.name() == "connect") {
        network_connection = option.value_string();
#ifdef USE_NETWORK_DOKOLOUNGE
      } else if (option.name() == "dokolounge") {
        // ToDo
#endif
#endif // #ifdef USE_NETWORK

      } else if (option.name().empty()) {
        if (option.value_string() == "")
          continue;
        if (option.value_string() == "-") {
          // this ensures, that the default party is not loaded
          ::game_status = GAMESTATUS::PARTY_INITIAL_LOADED;
          continue;
        }

#ifdef USE_NETWORK
        if (!server->empty()) {
          cerr << "Cannot load party when the server is not empty.\n"
            << "ignoring party file.\n";
#ifndef RELEASE
          exit(EXIT_FAILURE);
#endif
          continue;
        } // if (!server->empty())
#endif // #ifdef USE_NETWORK
        string const party_filename = option.value_string();

        { // check for a bug report
          if (   (  (party_filename.length() > strlen(".BugReport.FreeDoko"))
                  && (string(party_filename,
                             party_filename.length()
                             - strlen(".BugReport.FreeDoko")) )
                  == ".BugReport.FreeDoko")
              || (  (party_filename.length() > strlen(".Reference.FreeDoko"))
                  && (string(party_filename,
                             party_filename.length()
                             - strlen(".Reference.FreeDoko")) )
                  == ".Reference.FreeDoko") ) {
            OS_NS::BugReportReplay* bug_report_replay
              = new OS_NS::BugReportReplay(party_filename,
                                           OS_NS::BugReportReplay::VERBOSE_ALL);
            if (!bug_report_replay->loaded()) {
              cerr << "Error loading the bug report '" << option.value_string()
                << "'" << endl;
              delete bug_report_replay;
            } else {
              ::game_status = GAMESTATUS::PARTY_INITIAL_LOADED;
            }
            continue;
          } // if (bug report)
        } // check for a bug report
        { // check for network traffic file
#ifdef USE_NETWORK_DOKOLOUNGE
          if (  (party_filename.length() > strlen(".in"))
              && (string(party_filename,
                         party_filename.length()
                         - strlen(".in")) )
              == ".in") {
            ::server->create_connection(party_filename, 0,
                                        ::Network::DOKOLOUNGE);
            continue;
          }
#endif
        } // check for network traffic file

        try {
          if (::party.load(party_filename))
            ::game_status = GAMESTATUS::PARTY_INITIAL_LOADED;
        } catch (ReadException const& exception) {
          ::ui->information(::translation("Party load error: '%sfilename%'",
                                          party_filename)
                            + "\n"
                            + exception.message(),
                            INFORMATION::PROBLEM);
        }

      } else { // if (option.nam() == ...)
        cerr << "Option '" << option.name() << "' unknown!" << endl;
        exit(EXIT_FAILURE);
      } // if (option.name() == ...)
    } // while (getopt())

    // do not load anything, if a party is already loaded
    if (::game_status != GAMESTATUS::PARTY_INITIAL_LOADED) {
      // load the rules
      ::party.rule().load(::setting(Setting::PRIVATE_DATA_DIRECTORY)
                          + "/"
                          + ::setting(Setting::RULES_DIRECTORY)
                          + "/last");
      ::party.load_players();
      { // load the default party
        string const party_filename
          = (::setting(Setting::PRIVATE_DATA_DIRECTORY)
             + "/"
             + ::setting(Setting::PARTIES_DIRECTORY)
             + "/"
             + "current");
        if (DK::Utils::File::isfile(party_filename))
          if (::party.load(party_filename))
            ::game_status = GAMESTATUS::PARTY_INITIAL_LOADED;
      } // load the default party
    } // if (::game_status != GAMESTATUS::PARTY_INITIAL_LOADED)

    } // parse options


#ifdef POSTPONED
    static_cast<UI_Wrap*>(ui)->append(new_OS(OS_TYPE::CHATTER));
#endif

#ifdef USE_SOUND
    static_cast<UI_Wrap*>(::ui)->append(Sound::new_());
#endif

    { // set the ui
      if (selected_ui) {
      } else if (FAST_PLAY & FAST_NS::SEED_INFO) {
        selected_ui = UI::new_(UI_TYPE::DUMMY);
      } else { // if !(selected_ui) && (FAST_PLAY & FAST_NS::SEED_INFO)
        selected_ui
#if defined(USE_UI_GTKMM)
          = UI::new_(UI_TYPE::GTKMM_DOKO);
#elif defined(USE_UI_AATEXT)
        = UI::new_(UI_TYPE::AATEXT);
#elif defined(USE_UI_TEXT)
        = UI::new_(UI_TYPE::TEXT);
#else
        = UI::new_(UI_TYPE::DUMMY);
#endif
      } // if !(selected_ui) && (FAST_PLAY & FAST_NS::SEED_INFO)
      if (selected_ui) {
        delete static_cast<UI_Wrap*>(::ui)->ui;
        static_cast<UI_Wrap*>(::ui)->ui = selected_ui;

#ifdef USE_NETWORK
        ::server->ui_changed(*selected_ui);
#endif
      } // if (selected_ui)
    } // set the ui

    {
      auto const game_status_bak = ::game_status;
      ::game_status = GAMESTATUS::PROGRAMSTART;
      // init the ui
      ::ui->init(argc = 1, argv);
      // show the messages in the new ui
      if (selected_ui) {
        // show information messages from the initializing
        for (auto const& m : ::ui->messages_shown())
          ::ui->information(m.first, m.second, true);
      }
      ::game_status = game_status_bak;
    }

    if (::setting(Setting::AUTOMATIC_SAVINGS)) {
      // if this is the first start, create some directories
      INFORMATION::Type information_type = INFORMATION::NORMAL;
      vector<string> dirnames;
      dirnames.push_back(::setting(Setting::PRIVATE_DATA_DIRECTORY));
      dirnames.push_back(::setting(Setting::PRIVATE_DATA_DIRECTORY)
                         + "/" + ::setting(Setting::PARTIES_DIRECTORY));
      dirnames.push_back(::setting(Setting::PRIVATE_DATA_DIRECTORY)
                         + "/" + ::setting(Setting::RULES_DIRECTORY));
      dirnames.push_back(::setting(Setting::PRIVATE_DATA_DIRECTORY)
                         + "/" + ::setting(Setting::AI_DIRECTORY));
      Translator::Translation information = ::translation("");

      for (auto const& dir : dirnames) {
        if (!DK::Utils::File::isdirectory(dir)) {
          information += (::translation("Message::creating directory \'%sdir%\'",
                                        dir));
#ifdef WINDOWS
          if (mkdir(dir.c_str()) != 0)
#else
            if (mkdir(dir.c_str(), 00700) != 0)
#endif
            {
              information += (" - " + ::translation("Message::failed") + "!");
              information_type = INFORMATION::WARNING;
            }
          information += "\n";
        } // if (!isdirectory(dir))
      } // for (dir : dirnames)
      if (!information.empty()) {
        if (information_type == INFORMATION::NORMAL)
          cout << information;
        else
          ::ui->information(information, information_type);
      }
    } // if (::setting(Setting::AUTOMATIC_SAVINGS))

    { // read the version of the last usage of FreeDoko and write the new version

      // read the old version
      auto const version_old
        = DK::Utils::Version::create_from_file(::setting(Setting::PRIVATE_DATA_DIRECTORY)
                                               + "/Version");

      if (::setting(Setting::AUTOMATIC_SAVINGS)) {
        { // write the new version
          ofstream ostr((::setting(Setting::PRIVATE_DATA_DIRECTORY)
                         + "/Version").c_str());
          if (!ostr.fail())
            ostr << *::version;
        } // write the new version
      } // if (::setting(Setting::AUTOMATIC_SAVINGS))

      if (version_old == nullptr) {
        // This seems to be the first start -- give some introduction.
        ::ui->first_run(::translation("Greeting::first run"));
      } else { // if !(version_old == nullptr)
#ifdef RELEASE
        // give information if the version is updated
        if (*version_old < *::version) {
          ::ui->program_updated(*version_old);
        }
#endif
        delete version_old;
      } // if (version_old != nullptr)

    } // read the version of the last usage of FreeDoko and write the new version

    ::setting.check_for_outdated_cardset();

    // add the bug report in the ui.wrap,
    // so it gets the information of the gameplay
    static_cast<UI_Wrap*>(::ui)->append(::bug_report);

    // output of the seed each game
    if (::setting(Setting::AUTOMATIC_SAVINGS))
      static_cast<UI_Wrap*>(::ui)->append(new OS_NS::Seed(new ofstream((::setting(Setting::PRIVATE_DATA_DIRECTORY) + "/seed").c_str(),
                                                                       std::ios::app)));


#ifndef RELEASE
#if 0
    // output of the party points
    if (::setting(Setting::AUTOMATIC_SAVINGS))
      static_cast<UI_Wrap*>(ui)->append(new OS_NS::PartyPoints(new ofstream("FreeDoko.party_points")));
#endif
#endif

    // If no game has been saved in the party, the ui gets a problem with
    // a random startplayer.
    // So we check here, whether a game was already played.
    // Here, because so the rules and players are taken from the saved party
    // and not from the other files.
    if (::party.gameno() == 0)
      ::game_status = GAMESTATUS::PARTY_NEW;
    if (::game_status == GAMESTATUS::PARTY_LOADED)
      ::game_status = GAMESTATUS::PARTY_INITIAL_LOADED;

    if (FAST_PLAY & FAST_NS::SEED_INFO) {
      ::party.rule().reset_to_seed_statistics();
      cout << "seed;no;name;trumpno;club queens;spade queens;dollen;queens;jacks;diamond aces;diamond nines;fehl colors;color aces;reservation;hand value\n";
    }

    if (FAST_PLAY & FAST_NS::PLAYER) {
      // change all humans to ai's
      for (unsigned p = 0; p < ::party.playerno(); p++) {
        if (::party.player(p).type() == Player::HUMAN)
          ::party.set_playertype(p, Player::AI);
      }
    } // if (FAST_PLAY | FAST_NS::PLAYER)

    if (FAST_PLAY & FAST_NS::RANDOM_AI) {
      // set all ai's to random ai's
      for (unsigned p = 0; p < ::party.playerno(); p++) {
        if (::party.player(p).type() == Player::AI)
          ::party.set_playertype(p, Player::AI_RANDOM);
      }
    } // if (FAST_PLAY & FAST_NS::RANDOM_AI)

    // now the preparations have finished

    if (::game_status == GAMESTATUS::PROGRAMSTART)
      ::game_status = GAMESTATUS::PARTY_NEW;

    while (::game_status != GAMESTATUS::QUIT) {
      try {
        try {
          ::party.open();
          ::ui->party_open(::party);

#ifdef USE_NETWORK
          if (!network_connection.empty()) {
            ::server->create_connection(network_connection,
                                        Network::Server::FREEDOKO_STANDARD_PORT,
                                        Network::FREEDOKO);
            network_connection.clear();
            ::game_status = GAMESTATUS::PARTY_NEW;
          }
#endif

          if (   (::game_status == GAMESTATUS::PARTY_INITIAL_LOADED)
              || (::game_status == GAMESTATUS::PARTY_LOADED) ) {
            ::ui->party_loaded();
          } else {
            ::party.remove_auto_save();
            ::ui->party_get_settings();
          }
#ifdef USE_NETWORK
          if (   ::server
              && ::server->has_parent_connection()) {
            while (   (::game_status >= GAMESTATUS::PARTY_NEW)
                   && (::game_status < GAMESTATUS::PARTY_PLAY) ) {
              ::ui->wait();
            }
          }
#endif
          if (::game_status == GAMESTATUS::QUIT)
            throw ::game_status;

#ifdef WORKAROUND
          // in a network game, the gamestatus is set to PARTY_PLAY
          if (::game_status == GAMESTATUS::PARTY_PLAY)
            ::game_status = GAMESTATUS::PARTY_NEW;
#endif
          if (::game_status == GAMESTATUS::PARTY_NEW)
            ::party.init();

          // start playing
          ::game_status = GAMESTATUS::PARTY_PLAY;
          ::party.play();
        } catch (GameStatus const& gs) {
          if (gs == GAMESTATUS::PARTY_FINISHED)
            ::game_status = GAMESTATUS::PARTY_FINISHED;
          else
            throw;
        } // try
        if (FAST_PLAY & FAST_NS::QUIT_WHEN_FINISHED)
          throw(GAMESTATUS::QUIT);

        ::ui->party_finish();

        ::game_status = GAMESTATUS::PARTY_NEW;
      } catch (GameStatus const& gs) {
        DEBUG_ASSERTION((   (gs == GAMESTATUS::PARTY_NEW)
                         || (gs == GAMESTATUS::PARTY_LOADED)
                         || (gs == GAMESTATUS::QUIT)),
                        "main():\n"
                        "  caught gamestatus '" << gs << "'");
        ::game_status = gs;
      } catch (...) {
        ::ui->party_close();
        ::party.close();
        throw;
      }
      // last chance to save the party and the players
      if (::game_status == GAMESTATUS::QUIT)
        if (::setting(Setting::SAVE_PARTY_CHANGES))
          ::party.save_changes();

#ifdef CHECK_RUNTIME
      cout << "games: " << ::party.finished_games() << '\n';
#endif
      ::ui->party_close();
      if (::game_status != GAMESTATUS::PARTY_LOADED)
        ::party.close();

      if (::references_check) {
        try {
          ::game_status = GAMESTATUS::PARTY_FINISHED;
          ::ui->party_close();
          ::party.close();
          ::references_check->next_reference();
        } catch (GameStatus const& gs) {
          DEBUG_ASSERTION((   (gs == GAMESTATUS::PARTY_NEW)
                           || (gs == GAMESTATUS::QUIT)),
                          "main():\n"
                          "  caught gamestatus '" << gs << "'");
          ::game_status = gs;
        } catch (...) {
          throw;
        }
      } // if (::reference_check)
    } // while (game_status != QUIT)

    // last chance to save the changes
    if (::setting(Setting::AUTOMATIC_SAVINGS))
      ::setting.save();

  } catch (std::exception const& e) {
    cerr << "main()\n"
      << "  uncaught standard exception: " << e.what()
      << "  (" << &e << ")" << endl;
#ifdef WORKAROUND
    // with 'throw' the ui is not destructed :-(
    //return EXIT_FAILURE;
#endif
    throw;
  } catch (Card const& card) {
    cerr << "main()\n"
      << "  caught card exception: " << card << endl;
    return EXIT_FAILURE;
  } catch (...) {
    cerr << "main()\n"
      << "  uncaught unknown exception" << endl;
#ifdef WORKAROUND
    // with 'throw' the ui is not destructed :-(
    return EXIT_FAILURE;
#endif
    throw;
  } // try

  ::ui->quit();

  // clean up
#ifdef USE_NETWORK
  static_cast<UI_Wrap*>(::ui)->remove(::server);
  delete ::server;
#ifdef USE_NETWORK_DOKOLOUNGE
  delete ::lounge;
#endif
#endif
  delete ::references_check;
  delete ::ui;

#ifdef CHECK_RUNTIME
  // In the destructor
  //cout << ::runtime << endl;
#endif

  return EXIT_SUCCESS;
} // int main(argc, argv)
