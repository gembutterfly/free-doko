/*********************************************************************
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

#include "bug_report_replay.h"

#include "../player/ai/ai.h"
#include "../party/party.h"
#include "../game/game.h"
#include "../game/gameplay_actions.h"
#include "../game/gameplay_actions/check.h"
#include "../card/trick.h"

#include "../ui/ui.h"
#include "../ui/ui.wrap.h"

#include "../misc/setting.h"
#include "../misc/references_check.h"

#include "../utils/string.h"
#include "../utils/file.h"

#include <sstream>


// There can only be one 'bug report replay' instance.
// If a new is created, the old is removed.
OS_NS::BugReportReplay* bug_report_replay = NULL;

namespace OS_NS {
  // returns the value from the line 'line'
  static string get_value(string const& line);
  // returns the keyword from the line 'line'
  static string get_keyword(string const& line);


  /**
   ** -> result
   **
   ** @param     line   the line
   **
   ** @return    the value from the given line
   **
   ** @author    Diether Knof
   **
   ** @version   0.7.3
   **/
  string
    get_value(string const& line)
    {
      if (line.empty())
        return "";

      string value = line;
      DK::Utils::String::word_first_remove_with_blanks(value);

      return value;
    } // static string get_value(string line)

  /**
   ** -> result
   **
   ** @param     line   the line
   **
   ** @return    the keyword from the given line
   **
   ** @author    Diether Knof
   **
   ** @version   0.7.3
   **/
  string
    get_keyword(string const& line)
    {
      if (line.empty())
        return "";

      string const word
        = (  (std::find(line.begin(), line.end(), ':')
              != line.end())
           ? string(line.begin(),
                    std::find(line.begin(), line.end(), ':'))
           : line
          );

      return word;
    } // static string get_keyword(string line)


  /**
   ** Constructor
   ** (to be called from a child class)
   **
   ** @param	filename	the file with the bug report
   **
   ** @return	-
   **
   ** @author	Diether Knof
   **
   ** @version	0.6.7
   **/
  BugReportReplay::BugReportReplay(string const& filename,
                                   unsigned const verbose) :
    OS(OS_TYPE::OS_BUG_REPORT_REPLAY),
    filename_(filename),
    verbose_(verbose),
    loaded_(false),
    finished_(false),
    human_finished_(false),
    auto_start_party_(false),
    auto_action_end_(0),
    check_action_(false),
    version_(NULL),
    compiled_(),
    system_(),
    time_(),
    language_(),
    trickno_(),
    message_(),
    seed_(UINT_MAX),
    startplayer_no_(UINT_MAX),
    game_type_(GAMETYPE::NORMAL),
    marriage_selector_(MARRIAGE_SELECTOR::TEAM_SET),
    soloplayer_no_(UINT_MAX),
    rule_(),
    players_(),
    poverty_cards_shifted_(NULL),
    poverty_cards_returned_(NULL),
    hands_(),
    swines_player_no_(UINT_MAX),
    hyperswines_player_no_(UINT_MAX),
    actions_(),
    current_action_no_(0),
    actions_discrepancies_(),
    full_tricks_(),
    game_summary_(NULL),
    current_hands_(),
    current_trick_(NULL),
    human_actions_(),
    current_human_action_no_(0),
    human_actions_discrepancies_()
  {
    // remove an old bug report
    if (::bug_report_replay) {
      static_cast<UI_Wrap*>(::ui)->remove(::bug_report_replay);
      delete ::bug_report_replay;
      ::bug_report_replay = NULL;
    } // if (::bug_report_replay)

    if (!DK::Utils::File::isfile(filename)) {
      cerr << "Selected bug report '" << filename << "' is no file." << endl;
      return ;
    } // if (!DK::Utils::File::isfile(filename))

    try {
      this->read_file();
    } catch (ReadException const& read_exception) {
      return ;
    }

    this->init();

    this->print_header();

    return ;
  } // BugReportReplay::BugReportReplay(string const& filename)

  /**
   **
   ** Destructor
   **
   ** @param	-
   **
   ** @return	-
   **
   ** @author	Diether Knof
   **
   ** @version	0.6.7
   **
   **/
  BugReportReplay::~BugReportReplay()
  {
    ::ui->bug_report_replay_close();

    delete this->game_summary_;
    delete this->current_trick_;
    delete this->poverty_cards_shifted_;
    delete this->poverty_cards_returned_;

    if (this == ::bug_report_replay) {
      if (::ui)
        static_cast<UI_Wrap*>(::ui)->remove(::bug_report_replay);
      ::bug_report_replay = NULL;
    }

    return ;
  } // BugReportReplay::~BugReportReplay()

  /**
   ** initializes the bug report
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
    BugReportReplay::init()
    {
      ::bug_report_replay = this;
      ::setting.set(Setting::SAVE_PARTY_CHANGES, false);
      ::setting.set(Setting::ADDITIONAL_PARTY_SETTINGS, true);
      this->check_action_ = false;

      static_cast<UI_Wrap*>(::ui)->append(this);

      if (::game_status == GAMESTATUS::PARTY_NEW) {
        this->party_open(::party);
        this->party_get_settings();
      }

#ifdef POSTPONED
      // is called in 'party_open' so that this party is set
      // (-> UI_GTKMM::BugReportReplay::update_info)
      ::ui->bug_report_replay_open(*this);
#endif

      return ;
    } // void BugReportReplay::init()

  /**
   ** write the data of this bug report into 'ostr'
   **
   ** @param	ostr	output stream
   **
   ** @return	-
   **
   ** @author	Diether Knof
   **
   ** @version	0.7.1
   **/
  void
    BugReportReplay::write(ostream& ostr) const
    {
      ostr << "file: "      << this->filename() << '\n';
      if (this->version_)
        ostr << "version: " << this->version() << '\n';
      else
        ostr << "version: -\n";
      ostr << "compiled: "  << this->compiled() << '\n';
      ostr << "system: "    << this->system() << '\n';
      ostr << "time: "      << this->time() << '\n';
      ostr << "language: "  << this->language() << '\n';
      ostr << "trickno: "   << this->trickno() << '\n';

      ostr << '\n';

      ostr << "message\n"
        << "{\n"
        << this->message()
        << "}\n";

      ostr << '\n';

      ostr << "seed: "              << this->seed() << '\n';
      ostr << "startplayer_no: "    << this->startplayer_no() << '\n';
      ostr << "game_type: "         << this->game_type() << '\n';
      ostr << "marriage_selector: " << this->marriage_selector() << '\n';
      ostr << "soloplayer_no: "     << this->soloplayer_no() << '\n';

      ostr << '\n';

      ostr << "rule\n"
        << "{\n"
        << this->rule()
        << "}\n";

      ostr << '\n';

      ostr << "players\n"
        << "{\n";
      for (vector< ::Player*>::const_iterator p = this->players().begin();
           p != this->players().end();
           ++p)
        ostr << "{\n" << **p << "}\n";
      ostr << "}";

      ostr << '\n';

      if (this->poverty_cards_shifted_) {
        ostr << "poverty cards shifted\n"
          << "{\n";
        for (vector<Card>::const_iterator
             c = this->poverty_cards_shifted()->begin();
             c != this->poverty_cards_shifted()->end();
             ++c)
          ostr << *c << '\n';
        ostr << "}\n";
      } else {
        ostr << "poverty cards shifted: -\n";
      }

      if (this->poverty_cards_returned_) {
        ostr << "poverty cards returned\n"
          << "{\n";
        for (vector<Card>::const_iterator
             c = this->poverty_cards_shifted()->begin();
             c != this->poverty_cards_shifted()->end();
             ++c)
          ostr << *c << '\n';
        ostr << "}\n";
      } else {
        ostr << "poverty cards returned: -\n";
      }

      ostr << '\n';

      ostr << "hands\n"
        << "{\n";
      for (vector< ::Hand>::const_iterator h = this->hands().begin();
           h != this->hands().end();
           ++h)
        ostr << "{\n" << *h << "}\n";
      ostr << "}\n";

      ostr << '\n';

      ostr << "swines: ";
      if (this->swines_player_no() == UINT_MAX)
        ostr << '-';
      else
        ostr << this->swines_player_no();
      ostr << '\n';
      ostr << "hyperswines: ";
      if (this->hyperswines_player_no() == UINT_MAX)
        ostr << '-';
      else
        ostr << this->hyperswines_player_no();
      ostr << '\n';


      ostr << '\n';

      ostr << "game actions\n"
        << "{\n";
      for (vector<GameplayAction const*>::const_iterator
           a = this->actions().begin();
           a != this->actions().end();
           ++a)
        ostr << **a << '\n';
      ostr << "}\n";

      ostr << '\n';

      ostr << "full tricks\n"
        << "{\n";
      for (vector< ::Trick>::const_iterator t = this->full_tricks().begin();
           t != this->full_tricks().end();
           ++t)
        ostr << *t << '\n';
      ostr << "}\n";

      ostr << '\n';

      ostr << "game summary\n"
        << "{\n";
      if (this->game_summary_)
        ostr << this->game_summary();
      ostr << "}\n";

      ostr << '\n';

      ostr << "current hands\n"
        << "{\n";
      for (vector< ::Hand>::const_iterator h = this->current_hands().begin();
           h != this->current_hands().end();
           ++h)
        ostr << "{\n" << *h << "}\n";
      ostr << "}\n";

      ostr << '\n';

      if (this->current_trick_)
        ostr << "current trick\n"
          << this->current_trick();
      else
        ostr << "current trick: -\n";

      ostr << '\n';

      ostr << "human actions\n"
        << "{\n";
      for (vector<GameplayAction const*>::const_iterator
           a = this->human_actions().begin();
           a != this->human_actions().end();
           ++a)
        ostr << **a << '\n';
      ostr << "}\n";
      return ;
    } // void BugReportReplay::write(ostream& ostr) const

  /**
   ** test, whether the line is the expected
   **
   ** @param     read_line       line that was read
   ** @param     expected_line   line that is read
   **
   ** @return    whether the line is the expected
   **
   ** @author    Diether Knof
   **
   ** @version   0.7.3
   **/
  bool
    BugReportReplay::expect_line(string const& read_line,
                                 string const& expected_line)
    {
      if (read_line != expected_line) {
        cerr << "BugReportReplay:\n"
          << "  expected line '" << expected_line << "', "
          << "got: '" << read_line << "'"
          << "\n";

        this->mismatch();
        return false;
      } // if (read_line != expected_line)

      return true;
    } // bool BugReportReplay::expect_line(string read_line, string expected_line)

  /**
   ** test, whether the keyword is the expected
   **
   ** @param     read_keyword       keyword that was read
   ** @param     expected_keyword   keyword that is read
   **
   ** @return    whether the keyword is the expected
   **
   ** @author    Diether Knof
   **
   ** @version   0.7.3
   **/
  bool
    BugReportReplay::expect_keyword(string const& read_keyword,
                                    string const& expected_keyword)
    {
      if (read_keyword != expected_keyword) {
        cerr << "BugReportReplay:\n"
          << "  expected keyword '" << expected_keyword << "', "
          << "got: '" << read_keyword << "'"
          << "\n";

        this->mismatch();
        return false;
      } // if (read_keyword != expected_keyword)

      return true;
    } // bool BugReportReplay::expect_keyword(string read_keyword, string expected_line)


  /**
   ** reads the whole file
   **
   ** @param	-
   **
   ** @return	-
   **
   ** @author	Diether Knof
   **
   ** @version	0.7.3
   **
   ** @todo	poverty
   ** @todo	game summary
   **/
  void
    BugReportReplay::read_file()
    {
      cout << "Loading bug report " << this->filename() << endl;
      ifstream istr(this->filename().c_str());
      string line;    // the line read
      string keyword; // the keyword read
      string value;   // the value read

#define GETLINE \
      do { \
        if (istr.eof()) { \
          throw EOF; \
        } \
        std::getline(istr, line);  \
        DK::Utils::String::remove_blanks(line); \
        if ( (line.size() > 0) && (*(line.end() - 1) == '\r') ) \
        line.erase(line.end() - 1); \
      } while (line.empty() && istr.good()) ; \
      keyword = get_keyword(line); \
      value = get_value(line); \

      // reads the variable from 'value'
#define READ_FROM_VALUE(var)  \
      { \
        value = get_value(line); \
        istringstream istr(value); \
        istr >> var; \
      }

      // expect the keyword
      // if it is not valid, return from the function
#define EXPECT_LINE(expected_line) \
      if (true) { \
        if (!this->expect_line(line, expected_line)) \
        throw ReadException("expected line '" + string(line) + "'") ; \
      } else
#if 0
      ;
#endif

#define EXPECT_KEYWORD(expected_keyword) \
      if (true) { \
        if (!this->expect_keyword(keyword, expected_keyword)) \
        throw ReadException("expected keyword '" + string(keyword) + "'") ; \
      } else
#if 0
      ;
#endif

      try {
        this->human_finished_ = true;
        { // read the first lines (including message)
          // the first line
          GETLINE;
          // Byte Order Mark for UTF-8
          string BOM = "...";
          BOM[0] = 0xef;
          BOM[1] = 0xbb;
          BOM[2] = 0xbf;

          if (   (line != "# FreeDoko Bugreport")
              && (line != BOM + "# FreeDoko Bugreport")
              && (line != "# FreeDoko Reference")
              && (line != BOM + "# FreeDoko Reference") ) {
            cerr << "BugReportReplay::read_first_lines()\n"
              << "  The file is no bug report! First line does not match. Expecting\n"
              << "# FreeDoko Bugreport\n"
              << "  or\n"
              << "# FreeDoko Reference\n"
              << "  but got\n"
              << line
              << endl;
            this->mismatch();
            return ;
          } // if (line != "# FreeDoko Bugreport")

          // read and print the next informations
          // Example:
          // Version: 0.6.7b  (2004-12-19)
          // Compiled: Dec 19 2004, 19:36:18
          // System: Windows
          // Time: Fri Feb 25 22:55:22 2005
          //
          // Language: de
          // Trick: 9
          do { // while(!line.empty());
            GETLINE;

            if (line == "") {
            } else if (line == "/---------\\") {
              break;
            } else if (keyword == "version") {
              this->version_ = DK::Utils::Version::create(value);
              if (!this->version_) {
                cerr << "BugReportReplay: "
                  << "could not read version: '" << value << "'\n"
                  << "ignoring it." << endl;

              }
              if (   this->version_
                  && (this->version()
                      <= DK::Utils::Version(0, 6, 6)
                     ) ) {
                cerr << "BugReportReplay: Too old version "
                  << this->version()
                  << " < 0.6.6"
                  << endl;
#ifndef DKNOF
                exit(EXIT_SUCCESS);
#endif
              }
            } else if (keyword == "compiled") {
              this->compiled_  = value;
            } else if (keyword == "system") {
              this->system_    = value;
            } else if (keyword == "time") {
              this->time_      = value;
            } else if (keyword == "language") {
              this->language_  = value;
            } else if (keyword == "trick") {
              READ_FROM_VALUE(this->trickno_);
            } else { // if !(keyword == ...)
              cerr << "BugReportReplay: "
                << "unknown keyword '" << keyword << "'\n"
                << "ignoring it." << endl;
            } // if !(keyword == ...)

          } while(line != "/---------\\");

          { // read the message

            // the header exists of three lines -- ignore them
            GETLINE;
            GETLINE;

            this->message_.clear();
            do { // while (line != "/---------\\")
              GETLINE;
              if (line != "/--------\\")
                this->message_ += "  " + line + "\n";

              if (istr.fail()) {
                cerr << "BugReportReplay::read_first_lines()\n"
                  << "  (unexpected) Error reading the first lines.\n"
                  << "Aborting."
                  << endl;
                exit(EXIT_FAILURE);
              } // if (istr.fail())
            } while(line != "/--------\\") ;

            // the foot exists of three lines -- ignore them (one is already ignored)
            GETLINE;
            GETLINE;
          } // read the message
          GETLINE;
        } // read the first lines
        if (line == "finished") {
          throw EOF;
        } // if (line == "finished")
        { // seed, startplayer
          EXPECT_KEYWORD("seed");
          READ_FROM_VALUE(this->seed_);

          GETLINE;
          EXPECT_KEYWORD("startplayer");
          READ_FROM_VALUE(this->startplayer_no_);
        } // seed, startplayer
        { // rules, players
          GETLINE;

          if (line == "party") {
            EXPECT_LINE("party");
            // a '{'
            GETLINE;
            EXPECT_LINE("{");
            ::party.read(istr);
            for (unsigned n = 0; n < this->players().size(); ++n)
              delete this->players_[n];
            this->players_.clear();
            for (unsigned n = 0; n < ::party.players().size(); ++n)
              this->players_.push_back(::party.player(n).clone());
            GETLINE;
            this->rule_ = ::party.rule();
            EXPECT_LINE("}");
#ifdef WORKAROUND
            // ToDo: this shouldn't be necessary
            ::party.set_seed(this->seed());
#endif
          } else { // if !(line == "party")
            EXPECT_LINE("rules");
            this->rule_.read(istr);
            // set the rules here, so that the default ai types see the setting of WITH_NINES
            this->set_party_rule();

            GETLINE;
            EXPECT_LINE("players");

            // a '{'
            GETLINE;
            EXPECT_LINE("{");

            while (istr.peek() != '}') {
              this->players_.push_back(Player::create(istr));
              if (   !istr.good()
                  || !this->players_.back()) {
                cerr << "BugReportReplay::read_file()\n"
                  << "  could not load player.\n"
                  << "Aborting."
                  << endl;
                exit(EXIT_FAILURE);
              } // if (!this->players.back())
              while ((istr.peek() == '\n')
                     || (istr.peek() == '\r'))
                istr.get();
            } // while (istr.peek() != '}')

            GETLINE;
            EXPECT_LINE("}");
          } // if !(line == "party")
        } // players, players
        { // gametype
          GETLINE;
          if (keyword == "hands") {
            this->game_type_ = GAMETYPE::NORMAL;
          } else { // if (keyword == "hands")
            EXPECT_KEYWORD("gametype");
            DK::Utils::String::word_first_remove_with_blanks(line);
            for (int t = GAMETYPE::FIRST; t <= GAMETYPE::LAST; ++t)
              if (::name(static_cast<GameType>(t)) == line) {
                this->game_type_ = static_cast<GameType>(t);
                break;
              }
            { // special cases
              if (GAMETYPE::is_solo(this->game_type())
                  || (this->game_type() == GAMETYPE::THROWN_NINES)
                  || (this->game_type() == GAMETYPE::THROWN_KINGS)
                  || (this->game_type() == GAMETYPE::THROWN_NINES_AND_KINGS)
                  || (this->game_type() == GAMETYPE::THROWN_RICHNESS)
                  || (this->game_type() == GAMETYPE::FOX_HIGHEST_TRUMP) ) {
                GETLINE;
                EXPECT_KEYWORD("soloplayer");
                DK::Utils::String::word_first_remove_with_blanks(line);
                DK::Utils::String::word_first_remove_with_blanks(line);
                this->soloplayer_no_
                  = static_cast<unsigned>(atoi(line.c_str()));
              }
              if (this->game_type() == GAMETYPE::MARRIAGE) {
                // read the selector and the bride
                GETLINE;
                EXPECT_KEYWORD("selector");
                DK::Utils::String::word_first_remove_with_blanks(line);
                this->marriage_selector_
                  = MARRIAGE_SELECTOR::from_name(line);

                GETLINE;
                EXPECT_KEYWORD("bride");
                DK::Utils::String::word_first_remove_with_blanks(line);
                DK::Utils::String::word_first_remove_with_blanks(line);
                this->soloplayer_no_
                  = static_cast<unsigned>(atoi(line.c_str()));
              } // if (this->game_type() == GAMETYPE::MARRIAGE)
              if (this->game_type() == GAMETYPE::POVERTY) {
                // ToDo
              } // if (this->game_type == GAMETYPE::POVERTY)
            } // special cases
          } // if !(keyword == "hands")
        } // gametype
        { // hands
          if (keyword != "hands")
            GETLINE;
          EXPECT_LINE("hands");
          GETLINE;
          EXPECT_LINE("{");

          for (vector<Player*>::const_iterator
               player = this->players_.begin();
               player != this->players_.end();
               player++) {
            // the player number
            GETLINE;
            this->hands_.push_back(Hand(istr));
          } // for (player \in this->game().player)
          // a '}'
          GETLINE;
          EXPECT_LINE("}");
        } // hands
        { // actions, tricks
          GETLINE;
          EXPECT_LINE("gameplay actions");
          GETLINE;
          EXPECT_LINE("{");
          do { // while (line != "}")
            /* Format for a trick:
             *
             * Trick: 0
             * Played: Player 0: spade ace
             * Played: Player 1: spade nine
             * Played: Player 2: spade ace
             * Played: Player 3: spade nine
             * 
             * Trick full: 1
             * Startplayer: 0
             * 0: spade ace
             * 1: spade nine
             * 2: spade ace
             * 3: spade nine
             * Winner: 0
             */

            GETLINE;

            if (line == "}") {
              break;
            } else if (line[0] == '#') {
              continue;
            } else if (line == "stop") {
              this->auto_action_end_ = this->actions().size();
              this->auto_start_party_ = true;
              continue;
            }

            GameplayAction const* action
              = GameplayAction::new_(line, istr);
            if (!action) {
              cerr << "BugReportReplay:\n"
                << "  Unknown action '" << line << "'\n"
                << "  Ignoring it." << endl;
#ifdef DKNOF
              SEGFAULT;
#endif
              continue;
            }

#ifndef POSTPONED
            if (action->type == GameplayAction::TRICK_FULL)
              this->full_tricks_.push_back(static_cast<GameplayAction::TrickFull const*>(action)->trick);
#endif
#ifndef WORKAROUND
            if (action->type == GameplayAction::TRICK_FULL) {
              // Replace 'TrickFull' with 'TrickClosed',
              // since the trick in 'TrickFull' needs a corresponding game.
              delete action;
              action = new GameplayAction::TrickTaken;
            } // if (action->type == GameplayAction::TRICK_FULL)
#endif

            if (action->type == GameplayAction::CHECK) {
              this->auto_action_end_ = this->actions().size() + 1;
              this->auto_start_party_ = true;
            }

            this->actions_.push_back(action);
            { // special behaviour in a poverty: save the shifted cards
              if (action->type == GameplayAction::POVERTY_SHIFT) {
                this->poverty_cards_shifted_
                  = new vector<Card>(static_cast<GameplayAction::PovertyShift const*>(action)->cards);
              } else if (action->type == GameplayAction::POVERTY_RETURNED) {
                this->poverty_cards_returned_
                  = new vector<Card>(static_cast<GameplayAction::PovertyReturned const*>(action)->cards);
              } // if (action->type == GameplayAction::POVERTY_SHIFT)
            } // special behaviour in a poverty: save the shifted cards

          } while (line != "}");
        } // actions, tricks
        { // game summary
          GETLINE;
          if (line == "game summary") {
            EXPECT_LINE("game summary");
            this->game_summary_ = new GameSummary(this->rule(), istr);
            GETLINE;
            if (line == "special points") {
              // Just skip them -- they are contained in the game summary
              EXPECT_LINE("special points");
              GETLINE;
              EXPECT_LINE("{");

              do {
                GETLINE;
              } while (line != "}");
              GETLINE;
            } // if (line == "special points")
          } // if (line == "game summary")
        } // game summary
        if (istr.eof())
          throw EOF;
        { // current hands
          EXPECT_LINE("current hands");
          GETLINE;
          EXPECT_LINE("{");

          for (vector<Player*>::const_iterator
               player = this->players_.begin();
               player != this->players_.end();
               player++) {
            GETLINE;

            this->current_hands_.push_back(Hand(istr));
          } // for (player \in this->game().player)
          // a '}'
          GETLINE;
          EXPECT_LINE("}");
          GETLINE;
        } // current hands
        if (istr.eof())
          throw EOF;
        { // current trick
          if (line == "current trick")
          {
            this->current_trick_ = new Trick(istr);
            GETLINE;
          }
        } // current trick
        if (istr.eof())
          throw EOF;
        { // human actions
          EXPECT_LINE("human actions");
          GETLINE;
          EXPECT_LINE("{");
          do { // while (line != "}")

            GETLINE;
            if (line == "}")
              break;

            GameplayAction const* const action
              = GameplayAction::new_(line, istr);

            if (!action) {
              cerr << "BugReportReplay:\n"
                << "  Unknown human action '" << line << "'\n"
                << "  Ignoring it." << endl;
              continue;
            }

            this->human_actions_.push_back(action);

          } while (line != "}");
          this->human_finished_ = this->human_actions_.empty();
        } // human actions

      } catch (int const status) {
        if (status == EOF) {
          // finished
          // return ;
        } else {
          throw;
        }
      } catch (ReadException const& read_exception) {
        cerr << "BugReportReplay::read_file()\n"
          << "  read exception: " << read_exception.message()
          << endl;
        throw;
      } catch (...) {
        cerr << "BugReportReplay::read_file()\n"
          << "  unknown exception" << endl;
        throw;
      }

#undef GETLINE
#undef READ_FROM_VALUE
#undef EXPECT_LINE
#undef EXPECT_KEYWORD

      this->check_action_ = false;
      this->loaded_ = true;

      return ;
    } // void BugReportReplay::read_file()

  /**
   ** -> result
   **
   ** @param	-
   **
   ** @return	the current gameplay action
   **
   ** @author	Diether Knof
   **
   ** @version	0.7.3
   **/
  GameplayAction const&
    BugReportReplay::current_action() const
    {
      DEBUG_ASSERTION((this->current_action_no() < this->actions().size()),
                      "BugReportReplay::current_action()\n"
                      "  no further gameplay action");
      return *this->actions()[this->current_action_no()];
    } // GameplayAction BugReportReplay::current_action() const

  /**
   ** the current action has been processed
   **
   ** @param     action        processed action
   ** @param     discrepancy   discrepancy of the processed and the saved action
   **
   ** @return    -
   **
   ** @author    Diether Knof
   **
   ** @version   0.7.6
   **/
  void
    BugReportReplay::action_processed(GameplayAction const& action,
                                      GameplayAction::Discrepancy const
                                      discrepancy)
    {
      if (this->finished())
        return ;

      // check the action when requested
      if (   this->check_action()
          && (this->current_action().type != GameplayAction::CHECK) ) {
        DEBUG_ASSERTION(this->actions()[this->current_action_no() - 1]->type
                        == GameplayAction::CHECK,
                        "BugReportReplay::action_processed(discrepancy)\n"
                        "  last action is not 'check'");
        this->reference_check_game_action(action, discrepancy);
        this->check_action_ = false;
      } // if (this->check_action())

      // test for a human action
      if (!this->human_finished()
          && this->current_action() == this->current_human_action()) {
        this->human_actions_discrepancies_.push_back(discrepancy);
        this->current_human_action_no_ += 1;
        if (this->current_human_action_no() == this->human_actions().size()) {
          this->human_finished_ = true;
        }
      } // if (human action)

      // save the discrepancy and go to the next action
      this->actions_discrepancies_.push_back(discrepancy);
      this->current_action_no_ += 1;

      if (this->current_action_no() == this->actions().size()) {
        this->end_reached();
        return ;
      }

      // automatic actions for the human player
      if (::game_status >= GAMESTATUS::GAME_PLAY)
        this->handle_current_human_action();

      if (this->finished())
        return ;

      // test for printing to perform
      if (this->current_action().type == GameplayAction::PRINT) {
        if (   (::game_status >= GAMESTATUS::GAME_POVERTY_SHIFT)
            && (::game_status < GAMESTATUS::GAME_FINISHED) ) {
          // in a running game
          auto const& action = static_cast<GameplayAction::Print const&>(this->current_action());
          auto ai = dynamic_cast<Ai const*>(&::party.game().player_current());
          if (ai) {
            if (action.info() == "teams") {
              cout << '\n'
                << ai->name() << ": "
                << ai->team_information() << '\n';
            } else if (action.info() == "cards") {
              if (   (::game_status >= GAMESTATUS::GAME_POVERTY_SHIFT)
                  && (::game_status < GAMESTATUS::GAME_FINISHED) ) {
                cout << '\n'
                  << ai->name() << ": "
                  << ai->cards_information() << '\n';
              }
            } else {
              cout << "action print: unknown info " << action.info() << '\n';
            }
          } // if (ai)
        } // if (game_status ...)
        this->action_processed(this->current_action(), GameplayAction::NONE);
      } // if (this->current_action().type == GameplayAction::PRINT)

      // test for check to perform
      if (this->current_action().type == GameplayAction::CHECK) {
        this->check_action_ = true;
        this->action_processed(this->current_action(), GameplayAction::NONE);
      } // if (this->current_action().type == GameplayAction::CHECK)

      return ;
    } // void BugReportReplay::action_processed(GameplayAction action, GameplayAction::Discrepancy discrepancy)

  /**
   ** check the game action of a reference
   ** Print and save the result.
   ** End the game, if it is the last check
   **
   ** @param     action        processed action
   ** @param     discrepancy   discrepancy of the processed and the saved action
   **
   ** @return    -
   **
   ** @author    Diether Knof
   **
   ** @version   0.7.6
   **/
  void
    BugReportReplay::reference_check_game_action(GameplayAction const& action,
                                                 GameplayAction::Discrepancy
                                                 const discrepancy)
    {
      DEBUG_ASSERTION(this->current_action_no() > 0,
                      "BugreportReplay::reference_check_game_action(action, discrepancy)\n"
                      "  this is the first action in the bug report replay");

      GameplayAction::Check const* const check_action
        = dynamic_cast<GameplayAction::Check const*>(this->actions()[this->current_action_no() - 1]);
      DEBUG_ASSERTION(check_action != NULL,
                      "BugreportReplay::reference_check_game_action(action, discrepancy)\n"
                      "  last action is no check");

      if (::references_check)
        ::references_check->check(this->game(),
                                  action, this->current_action(),
                                  check_action->comment(),
                                  discrepancy,
                                  (this->auto_action_end()
                                   == this->current_action_no()) );

      return ;
    } // void BugReportReplay::reference_check_game_action(GameplayAction action, GameplayAction::Discrepancy discrepancy)

  /**
   ** -> result
   **
   ** @param	action_no   number of the action
   **
   ** @return	the discrepancy for the given action
   **
   ** @author	Diether Knof
   **
   ** @version	0.7.3
   **/
  GameplayAction::Discrepancy
    BugReportReplay::discrepancy(unsigned const action_no) const
    {
      if (action_no >= this->actions_discrepancies().size())
        return GameplayAction::FUTURE;

      return this->actions_discrepancies()[action_no];
    } // Discrepancy BugReportReplay::discrepancy(unsigned action_no) const

  /**
   ** -> result
   **
   ** @param	-
   **
   ** @return	the current human action
   **
   ** @author	Diether Knof
   **
   ** @version	0.7.3
   **/
  GameplayAction const&
    BugReportReplay::current_human_action() const
    {
      DEBUG_ASSERTION((this->current_human_action_no() < this->human_actions().size()),
                      "BugReportReplay::current_human_action()\n"
                      "  no further human action");
      return *this->human_actions()[this->current_human_action_no()];
    } // GameplayAction BugReportReplay::current_human_action() const

  /**
   ** -> result
   **
   ** @param	action_no   number of the human action
   **
   ** @return	the discrepancy for the given human action
   **
   ** @author	Diether Knof
   **
   ** @version	0.7.3
   **/
  GameplayAction::Discrepancy
    BugReportReplay::human_discrepancy(unsigned const action_no) const
    {
      if (action_no >= this->human_actions_discrepancies().size())
        return GameplayAction::FUTURE;

      return this->human_actions_discrepancies()[action_no];
    } // Discrepancy BugReportReplay::human_discrepancy(unsigned action_no) const

  /**
   ** handle the current human action
   **
   ** @param	-
   **
   ** @return	-
   **
   ** @author	Diether Knof
   **
   ** @version	0.7.3
   **/
  void
    BugReportReplay::handle_current_human_action()
    {
      if (   this->finished()
          || this->human_finished()
          || (this->current_action() != this->current_human_action()) )
        return ;

      // print the coming action
      this->print_current_human_action();

      // partly execute the action automatically
      switch (this->current_human_action().type) {
      case GameplayAction::ANNOUNCEMENT: {
        GameplayAction::Announcement const& announcement_action
          = static_cast<GameplayAction::Announcement const&>(this->current_human_action());
        if (this->verbose() & VERBOSE_ACTION)
          cout << "BugReportReplay:\n"
            << "  making announcement '" << announcement_action.announcement << "'"
            << " for player " << announcement_action.player
            << endl;

        // make the announcement in place of the human and tell it the user
        if (!ANNOUNCEMENT::is_reply(announcement_action.announcement))
          ::party.game().make_announcement(announcement_action.announcement,
                                           ::party.game().player(announcement_action.player));
      } break;

      case GameplayAction::SWINES: {
        GameplayAction::Swines const& swines_action
          = static_cast<GameplayAction::Swines const&>(this->current_human_action());
        if (this->verbose() & VERBOSE_ACTION)
          cout << "BugReportReplay:\n"
            << "  announce 'swines' for human player "
            << swines_action.player
            << endl;
        if (!::party.game().swines_announce(::party.game().player(swines_action.player)))
          cerr << "Error announcing 'swines' for human player"
            << swines_action.player
            << endl;
      } break;

      case GameplayAction::HYPERSWINES: {
        GameplayAction::Hyperswines const& hyperswines_action
          = static_cast<GameplayAction::Hyperswines const&>(this->current_human_action());
        if (this->verbose() & VERBOSE_ACTION)
          cout << "BugReportReplay:\n"
            << "  announce 'hyperswines' for human player "
            << hyperswines_action.player
            << endl;
        if (!::party.game().hyperswines_announce(::party.game().player(hyperswines_action.player)))
          cerr << "Error announcing 'hyperswines' for human player"
            << hyperswines_action.player
            << endl;
      } break;

      default:
        break;

      } // switch (this->current_human_action().type)
    } // void BugReportReplay::handle_current_human_action()

  /**
   ** print the next human action
   **
   ** @param	-
   **
   ** @return	-
   **
   ** @author	Diether Knof
   **
   ** @version	0.7.3
   **/
  void
    BugReportReplay::print_current_human_action() const
    {
      if (!(this->verbose() & VERBOSE_HUMAN_ACTION))
        return ;
      switch (this->current_human_action().type) {
      case GameplayAction::CARD_PLAYED: {
        GameplayAction::CardPlayed const& card_played_action
          = static_cast<GameplayAction::CardPlayed const&>(this->current_human_action());
        cout << "BugReportReplay: Human\n"
          << "  play " << card_played_action.card
          << endl;
      } break;

      case GameplayAction::RESERVATION: {
        GameplayAction::Reservation const& reservation_action
          = static_cast<GameplayAction::Reservation const&>(this->current_human_action());
        cout << "BugReportReplay: Human\n"
          << "  gametype " << reservation_action.reservation.game_type << '\n';
        if (reservation_action.reservation.game_type == GAMETYPE::MARRIAGE)
          cout << "  marriage selector " << reservation_action.reservation.marriage_selector <<'\n'; 
        if (reservation_action.reservation.swines)
          cout << "  swines\n";
        if (reservation_action.reservation.hyperswines)
          cout << "  hyperswines\n";
      } break;

      case GameplayAction::POVERTY_SHIFT: {
        GameplayAction::PovertyShift const& poverty_shift_action
          = static_cast<GameplayAction::PovertyShift const&>(this->current_human_action());
        cout << "BugReportReplay: Human\n"
          << "  poverty: shift\n";
        for (vector<Card>::const_iterator
             c = poverty_shift_action.cards.begin();
             c != poverty_shift_action.cards.end();
             ++c)
          cout << "    " << *c << '\n';
      } break;

      case GameplayAction::POVERTY_RETURNED: {
        GameplayAction::PovertyReturned const& poverty_returned_action
          = static_cast<GameplayAction::PovertyReturned const&>(this->current_human_action());
        cout << "BugReportReplay: Human\n"
          << "  poverty: return\n";
        for (vector<Card>::const_iterator
             c = poverty_returned_action.cards.begin();
             c != poverty_returned_action.cards.end();
             ++c)
          cout << "    " << *c << '\n';
      } break;

      case GameplayAction::POVERTY_ACCEPTED: {
        cout << "BugReportReplay: Human\n"
          << "  poverty: accept\n";
      } break;

      case GameplayAction::POVERTY_DENIED: {
        cout << "BugReportReplay: Human\n"
          << "  poverty: deny\n";
      } break;

      case GameplayAction::POVERTY_DENIED_BY_ALL:
      case GameplayAction::TRICK_OPEN:
      case GameplayAction::TRICK_FULL:
      case GameplayAction::TRICK_TAKEN:
      case GameplayAction::MARRIAGE:
      case GameplayAction::CHECK:
      case GameplayAction::PRINT:
        // nothing to do
        break;

      case GameplayAction::ANNOUNCEMENT:
      case GameplayAction::SWINES:
      case GameplayAction::HYPERSWINES:
        // -> action_processed

      case GameplayAction::GENSCHER:
        // ToDo
        break;
      } // switch (this->current_human_action().type)

      return ;
    } // void BugReportReplay::print_current_human_action() const

  /**
   ** the bug report is inconsistent:
   **
   ** @param	-
   **
   ** @return	-
   **
   ** @author	Diether Knof
   **
   ** @version	0.7.3
   **/
  void
    BugReportReplay::mismatch()
    {
      cerr << "BugReport mismatch!" << endl;

      return ;
    } // void BugReportReplay::mismatch()

  /**
   **
   ** the bug report is finished.
   ** remove it from the global ui
   **
   ** @param	-
   **
   ** @return	-
   **
   ** @author	Diether Knof
   **
   ** @version	0.6.7
   **
   **/
  void
    BugReportReplay::end_reached()
    {
      this->finished_ = true;
      this->human_finished_ = true;

      if (this->verbose() & VERBOSE_FINISH) {
        cout << "BugReport: finished" << endl;
        cout << "Message:\n"
          << "{\n"
          << this->message()
          << "}"
          <<endl;
      }

#ifndef OUTDATED
      // the bug report replay should be kept, so that another start of the
      // tournament restarts the bug report replay
      static_cast<UI_Wrap*>(::ui)->remove(this);
#endif

      return ;
    } // void BugReportReplay::end_reached()


  /**
   ** prints the header and the differences
   **
   ** @param	-
   **
   ** @return	-
   **
   ** @author	Diether Knof
   **
   ** @version	0.7.3
   **/
  void
    BugReportReplay::print_header() const
    {
      if (!(this->verbose() & VERBOSE_HEADER))
        return;
      // print the first lines
      cout << "BugReport\n"
        << "\n";

      if (this->version_)
        cout << "version:     " << this->version()  << '\n';
      else
        cout << "version:     -\n";
      cout
        << "compiled:    " << this->compiled()        << '\n'
        << "system:      " << this->system()          << '\n'
        << "time:        " << this->time()            << '\n'
        << "language:    " << this->language()        << '\n'
        << "seed:        " << this->seed()            << '\n'
        << "startplayer: " << this->startplayer_no()  << '\n'
        << "trick:       " << this->trickno()         << '\n'
        << '\n'
        << "message:\n"
        << "{\n"
        <<   this->message()
        << "}\n";

      return ;
    } // void BugReportReplay::print_header() const

  /**
   ** a party is opened
   **
   ** @param	party	the party that is opened
   **
   ** @return	-
   **
   ** @author	Diether Knof
   **
   ** @version	0.6.7
   **/
  void
    BugReportReplay::party_open(Party const& party)
    {
      this->OS::party_open(party);

      this->finished_ = false;
      this->current_action_no_ = 0;
      this->current_human_action_no_ = 0;
      this->actions_discrepancies_.clear();
      this->human_actions_discrepancies_.clear();

      this->human_finished_ = true;
      if (!this->human_actions_.empty()) {
        for (vector<Player*>::const_iterator
             player = this->players_.begin();
             player != this->players_.end();
             player++) {
          if ((*player)->type() == Player::HUMAN) {
            this->human_finished_ = false;
            break;
          }
        }
      } // if (!this->human_actions_.empty())

#ifndef OUTDATED
      ::ui->bug_report_replay_open(*this);
#endif

      if (this->current_action_no() == this->actions().size()) {
        this->end_reached();
        return ;
      }

      if (this->current_action().type == GameplayAction::CHECK) {
        this->check_action_ = true;
        this->action_processed(this->current_action(), GameplayAction::NONE);
      } // if (this->current_action().type == GameplayAction::CHECK)

      return ;
    } // void BugReportReplay::party_open(Party const& party)

  /**
   ** set the settings of the party
   **
   ** @param	-
   **
   ** @return	-
   **
   ** @author	Diether Knof
   **
   ** @version	0.7.3
   **/
  void
    BugReportReplay::party_get_settings()
    {
      ::party.set_seed(this->seed());
      ::party.set_startplayer(this->startplayer_no());

      // set the rules and print the rule differences
      this->set_party_rule();

      { // set the players
        for (unsigned n = 0; n < this->players().size(); ++n) {
          ::party.set_player(n, this->players()[n]->clone());
        }
      } // set the players

      if (this->verbose() & VERBOSE_MESSAGE)
        // repeat the message
        cout << '\n'
          << "Message:\n"
          << "{\n"
          <<   this->message()
          << "}\n";

      return ;
    } // void BugReportReplay::party_get_settings()

  /**
   ** the party is loaded
   **
   ** @param	-
   **
   ** @return	-
   **
   ** @author	Diether Knof
   **
   ** @version	0.7.9
   **/
  void
    BugReportReplay::party_loaded()
    {
      // (re)set the rules and print the rule differences
      this->set_party_rule();

      if (this->verbose() & VERBOSE_MESSAGE)
        // repeat the message
        cout << '\n'
          << "Message:\n"
          << "{\n"
          <<   this->message()
          << "}\n";

      return ;
    } // void BugReportReplay::party_loaded()

  /**
   ** set the rule of the party
   **
   ** @param	-
   **
   ** @return	-
   **
   ** @author	Diether Knof
   **
   ** @version	0.7.9
   **/
  void
    BugReportReplay::set_party_rule()
    {
      // the hardcoded ruleset (the harddcoded)
      Rule const rule_hardcoded;
      // set the party rules
      ::party.rule() = this->rule();

      if (this->verbose() & VERBOSE_RULES) {
        cout << '\n'
          << "rule differences:\n"
          << "{\n";
        cout << setw(38) << ""
          << setw(12) << "bug_report"
          << setw(12) << "hardcoded"
          << "\n";
        for (unsigned i = Rule::FIRST; i <= Rule::LAST; i++) {
          if (   (i >= (Rule::BOOL_FIRST))
              && (i <= Rule::BOOL_LAST)) {
            if (   (::party.rule()(Rule::TypeBool(i))
                    != rule_hardcoded(Rule::TypeBool(i))) ) {
              cout << setw(38) << name(Rule::TypeBool(i))
                << setw(12) << ::party.rule()(Rule::TypeBool(i))
                << setw(12) << rule_hardcoded(Rule::TypeBool(i))
                << "\n";
            } // if (rule differs)
          } else if (   (i >= Rule::UNSIGNED_FIRST)
                     && (i <= Rule::UNSIGNED_LAST)) {
            if (   (::party.rule()(Rule::TypeUnsigned(i))
                    != rule_hardcoded(Rule::TypeUnsigned(i))) ) {
              cout << setw(38) << name(Rule::TypeUnsigned(i))
                << setw(12) << ::party.rule()(Rule::TypeUnsigned(i))
                << setw(12) << rule_hardcoded(Rule::TypeUnsigned(i))
                << "\n";
            } // if (rule differs)
          } else if (   (i >= Rule::UNSIGNED_EXTRA_FIRST)
                     && (i <= Rule::UNSIGNED_EXTRA_LAST)) {
            if (   (::party.rule()(Rule::TypeUnsignedExtra(i))
                    != rule_hardcoded(Rule::TypeUnsignedExtra(i)))) {
              cout << setw(38) << name(Rule::TypeUnsignedExtra(i))
                << setw(12) << ::party.rule()(Rule::TypeUnsignedExtra(i))
                << setw(12) << rule_hardcoded(Rule::TypeUnsignedExtra(i))
                << "\n";
            } // if (rule differs)
          } else if (i == Rule::COUNTING) {
            if (   (::party.rule()(Rule::COUNTING)
                    != rule_hardcoded(Rule::COUNTING)) ) {
              cout << setw(38) << name(Rule::COUNTING)
                << setw(12) << ::party.rule()(Rule::COUNTING)
                << setw(12) << rule_hardcoded(Rule::COUNTING)
                << "\n";
            } // if (rule differs)
          } else { // if (i: type unknown)
            DEBUG_ASSERTION(false,
                            "BugReportReplay::party_open():\n"
                            "  unknown ruletype number " << i);
          } // if (i: type unknown)
        } // for (unsigned i = Rule::FIRST; i <= Rule::LAST; i++)
        cout << "}" << endl;
      } // if (this->verbose() & VERBOSE_RULES)

      { // set some specific rules
        vector<Rule::TypeBool> types;
        types.push_back(Rule::SHOW_IF_VALID);
        types.push_back(Rule::SHOW_ALL_HANDS);
        types.push_back(Rule::SHOW_TRICKS_IN_TRICKPILES);
        types.push_back(Rule::SHOW_TRICKPILES_POINTS);
        types.push_back(Rule::SHOW_KNOWN_TEAMS_IN_GAME);
        types.push_back(Rule::SHOW_SOLOPLAYER_IN_GAME);
        types.push_back(Rule::CARD_HINT);
        types.push_back(Rule::TAKE_BACK_TRICKS);
        for (vector<Rule::TypeBool>::const_iterator t = types.begin();
             t != types.end();
             ++t) {
          if (!::party.rule()(*t)) {
            if (this->verbose() & VERBOSE_RULES)
              cout << "setting rule '" << ::name(*t)
                << "' to true." << endl;
            ::party.rule().set(*t, true);
          } // if (!::party.rule()(*t))
        } // for t \in types

        if (::party.rule()(Rule::SHOW_TRICKS_NUMBER)
            < ::party.rule()(Rule::NUMBER_OF_TRICKS_IN_GAME)) {
          if (this->verbose() & VERBOSE_RULES)
            cout << "setting rule '" << ::name(Rule::SHOW_TRICKS_NUMBER)
              << "' to " << ::party.rule()(Rule::NUMBER_OF_TRICKS_IN_GAME)
              << "." << endl;
          ::party.rule().set(Rule::SHOW_TRICKS_NUMBER,
                             ::party.rule()(Rule::NUMBER_OF_TRICKS_IN_GAME));
        } // if (!::party.rule()(Rule::SHOW_TRICKS_NUMBER))
      } // set some specific rules

      return ;
    } // void BugReportReplay::set_party_rule()

  /**
   ** the party is started
   **
   ** @param	-
   **
   ** @return	-
   **
   ** @author	Diether Knof
   **
   ** @version	0.7.8
   **/
  void
    BugReportReplay::party_start()
    {
      this->auto_start_party_ = false;
      return ;
    } // void BugReportReplay::party_start()

  /**
   ** a gameplay action
   **
   ** @param     action   the action
   **
   ** @return    -
   **
   ** @author    Diether Knof
   **
   ** @version   0.7.3
   **/
  void
    BugReportReplay::gameplay_action(GameplayAction const& action)
    {
      if (this->finished())
        return ;

      // do handle all actions (see BugReport::gameplay_action(action) )
      switch (action.type) {
      case GameplayAction::POVERTY_DENIED_BY_ALL:
      case GameplayAction::TRICK_TAKEN:
        // do not handle
        return ;
      case GameplayAction::CHECK:
      case GameplayAction::PRINT:
        // shall not happen
        DEBUG_ASSERTION(false,
                        "Gameplay action 'check'/'print' should not be processed here");
        return ;

      case GameplayAction::TRICK_FULL: // replaced by TRICK_OPEN
        if (!::party.game().is_last_trick())
          return ;
      case GameplayAction::TRICK_OPEN: // replaced by TRICK_FULL
        if (::party.game().trick_current_no() == 0)
          return ;
      case GameplayAction::RESERVATION:
      case GameplayAction::POVERTY_SHIFT:
      case GameplayAction::POVERTY_RETURNED:
      case GameplayAction::POVERTY_ACCEPTED:
      case GameplayAction::POVERTY_DENIED:
      case GameplayAction::CARD_PLAYED:
      case GameplayAction::ANNOUNCEMENT:
      case GameplayAction::SWINES:
      case GameplayAction::HYPERSWINES:
      case GameplayAction::MARRIAGE:
      case GameplayAction::GENSCHER:
        // do handle
        break;
      } // switch (action.type)

      // check that this action conforms to the gameplay
      if (   (action == this->current_action())
          || (   (action.type == GameplayAction::TRICK_OPEN)
              && (this->current_action().type == GameplayAction::TRICK_FULL) )
         ) {
        this->action_processed(action, GameplayAction::NONE);
        return ;
      }
      // the action differs from the one of the bug report
      cerr << "BugReport different actions:\n"
        << "  game:       " << action << '\n'
        << "  bug report: " << this->current_action() << endl;

      // if the type is equal there is only some other gameplay
      if (action.type == this->current_action().type) {
        if (dynamic_cast<GameplayAction::Player1 const*>(&action)) {
          if (static_cast<GameplayAction::Player1 const&>(action).player
              != static_cast<GameplayAction::Player1 const&>(this->current_action()).player) {
            this->action_processed(action, GameplayAction::PLAYER);
          } else if (   (action.type == GameplayAction::CARD_PLAYED)
                     && (static_cast<GameplayAction::CardPlayed const&>(action).card
                         != static_cast<GameplayAction::CardPlayed const&>(this->current_action()).card) ) {
            this->action_processed(action, GameplayAction::CARD);
          } else {
            this->action_processed(action, GameplayAction::OTHER);
          }

        } else {
          this->action_processed(action, GameplayAction::OTHER);
        }

        return ;
      } // if (action.type == this->current_action().type)

      // skip announcements
      switch (this->current_action().type) {
      case GameplayAction::ANNOUNCEMENT:
      case GameplayAction::SWINES:
      case GameplayAction::HYPERSWINES:
      case GameplayAction::MARRIAGE:
      case GameplayAction::GENSCHER:
        if (action.type >= GameplayAction::CARD_PLAYED) {
          // just skip
          this->action_processed(action, GameplayAction::SKIPPED);
          this->gameplay_action(action);
          return ;
        }
        break;

      default:
        break;
      } // switch (this->current_action().type)

      switch (action.type) {
      case GameplayAction::CARD_PLAYED:
        if (this->current_action().type == GameplayAction::CARD_PLAYED) {
          this->action_processed(action, GameplayAction::SKIPPED);
          break;
        }
        // search the next card-played action
        while (!this->finished()) {
          this->action_processed(action, GameplayAction::SKIPPED);
          if (this->finished())
            break ;
          if (this->current_action().type == GameplayAction::CARD_PLAYED)
            break ;
        } // while (!this->finished())
        break;

      case GameplayAction::RESERVATION:
      case GameplayAction::POVERTY_SHIFT:
      case GameplayAction::POVERTY_ACCEPTED:
      case GameplayAction::POVERTY_RETURNED:
      case GameplayAction::POVERTY_DENIED:
      case GameplayAction::POVERTY_DENIED_BY_ALL:
      case GameplayAction::TRICK_OPEN:
      case GameplayAction::TRICK_FULL:
      case GameplayAction::TRICK_TAKEN:
        // ToDo
        break;

      case GameplayAction::ANNOUNCEMENT:
      case GameplayAction::MARRIAGE:
      case GameplayAction::GENSCHER:
      case GameplayAction::SWINES:
      case GameplayAction::HYPERSWINES:
      case GameplayAction::CHECK:
      case GameplayAction::PRINT:
        // ignore
        break;
      } // switch (action.type)

      return ;
    } // void BugReportReplay::gameplay_action(GameplayAction action)

  /**
   ** ask 'player' whether to accept the poverty
   **
   ** @param	player   player that is asked
   ** @param	cardno   number of shifted cards
   **
   ** @return	-
   **
   ** @author	Diether Knof
   **
   ** @version	0.7.3
   **/
  void
    BugReportReplay::poverty_ask(Player const& player,
                                 unsigned const cardno)
    {
      this->OS::poverty_ask(player, cardno);

      this->handle_current_human_action();

      return ;
    } // void BugReportReplay::poverty_ask(Player player, unsigned cardno);

  /**
   ** a new trick is opened
   **
   ** @param	trick	trick that is opened
   **
   ** @return	-
   **
   ** @author	Diether Knof
   **
   ** @version	0.7.3
   **/
  void
    BugReportReplay::trick_open(Trick const& trick)
    {
      this->OS::trick_open(trick);

      // automatic actions for the human player
      // De facto this is only needed for the first trick so that the player
      // can announce before any other action.
      this->handle_current_human_action();

      return ;
    } // void BugReportReplay::trick_open(Trick const& trick)

  /**
   ** -> result
   **
   ** @param     player   player
   **
   ** @return    the next card played by the player
   **
   ** @author    Diether Knof
   **
   ** @version   0.7.3
   **/
  HandCard
    BugReportReplay::next_card(Player const& player) const
    {
      if (this->human_finished())
        return HandCard();

      if (this->current_human_action().type == GameplayAction::GENSCHER)
        return HandCard(player, Card(Card::DIAMOND, Card::KING));

      if (this->current_human_action().type != GameplayAction::CARD_PLAYED) {
        cerr << "Bug report: human action should be 'card played' but is:\n"
          << "  " << this->current_human_action() << endl;
        return HandCard();
      } // if (this->current_human_action().type != GameplayAction::CARD_PLAYED)

      return HandCard(player, static_cast<GameplayAction::CardPlayed const&>(this->current_human_action()).card);
    } // HandCard BugReportReplay::next_card(Player const& player) const

  /**
   ** set the auto action end
   **
   ** @param     auto_action_end   the end of the auto actions
   **
   ** @return    -
   **
   ** @author    Diether Knof
   **
   ** @version   0.7.8
   **/
  // whether the current action should be executed automatically
  void
    BugReportReplay::set_auto_action_end(unsigned const auto_action_end)
    {
      this->auto_action_end_ = auto_action_end;
      this->auto_start_party_ = true;
      return ;
    } // void BugReportReplay::set_auto_action_end(unsigned auto_action_end)

  /**
   ** -> result
   **
   ** @param     -
   **
   ** @return    whether the current action should be executed automatically
   **
   ** @author    Diether Knof
   **
   ** @version   0.7.3
   **/
  bool
    BugReportReplay::auto_action() const
    {
      if (this->finished())
        return false;

      return (!this->check_action()
              && (this->current_action_no() < this->auto_action_end()));
    } // bool BugReportReplay::auto_action() const

} // namespace OS_NS
