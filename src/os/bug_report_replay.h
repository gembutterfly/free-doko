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

#ifndef CLASS_OS_BUG_REPORT_REPLAY_HEADER
#define CLASS_OS_BUG_REPORT_REPLAY_HEADER

#include "os.h"
#include "../game/gameplay.h"
#include "../game/gameplay_action.h"
#include "../card/hand.h"
#include "../card/trick.h"
#include "../party/party.h"
#include "../party/rule.h"
#include "../game/game.h"
#include "../game/game_summary.h"

#include "../basistypes.h"
#include "../utils/version.h"
#include "../utils/private.h"

namespace OS_NS {
  class BugReportReplay;
} // namespace OS_NS 
extern OS_NS::BugReportReplay* bug_report_replay;

namespace OS_NS {

  /**
   **
   ** A class to simplify the replay of the bug report.
   ** All info's from the bug report are used and differences to the
   ** gameplay are shown.
   **
   ** Parts of the bug report:
   ** - version		is printed
   ** - system		is printed
   ** - date		is printed
   ** - message		is printed
   ** - seed		is set
   ** - startplayer	is set
   ** - ruleset		is set, differences to the standard ruleset and
   **				the default are printed
   ** - players		are set, differences to the standard aiconfig
   **				 are printed
   ** - hands		are checked
   **			* marriage: bridge is checked
   ** - gametype		is checked
   **			* marriage: bridge is checked
   **			* poverty: shifted cards are checked
   **
   **
   **
   ** @author	Diether Knof
   **
   ** @version	0.5.4
   **
   ** @todo	poverty
   ** @todo	marriage
   ** @todo	automatic announcements for the player
   **
   **/
  class BugReportReplay : public OS {
    public:
      enum Verbose {
        VERBOSE_NONE = 0,
        VERBOSE_SEED = 1 << 1,
        VERBOSE_HEADER = 1 << 2,
        VERBOSE_MESSAGE = 1 << 3,
        VERBOSE_RULES = 1 << 4,
        VERBOSE_ACTION = 1 << 5,
        VERBOSE_HUMAN_ACTION = 1 << 6,
        VERBOSE_FINISH = 1 << 7,
        VERBOSE_ALL = ((1 << 8) - 1)
      }; // enum Verbose
    public:
      // Konstruktor with the filename of the bugreport and the verbose level
      BugReportReplay(string const& filename,
                      unsigned const verbose);
      ~BugReportReplay();

      // write the data of this bug report into 'ostr'
      void write(ostream& ostr) const;


      // prints the header and the differences
      void print_header() const;

      // the parts of a party
      void party_open(Party const& party);
      void party_get_settings();
      void party_loaded();
      void set_party_rule();
      void party_start();

      // a gameplay action
      void gameplay_action(GameplayAction const& action);

      // ask 'player' whether to accept the poverty
      void poverty_ask(Player const& player, unsigned const cardno);
      // a trick is opened
      void trick_open(Trick const& trick);

      // returns the next card played by the human player
      HandCard next_card(Player const& player) const;

      // the discrepancy for the given action
      GameplayAction::Discrepancy discrepancy(unsigned const action_no) const;
      // the discrepancy for the given human action
      GameplayAction::Discrepancy human_discrepancy(unsigned const action_no) const;

      // the current gameplay action
      GameplayAction const& current_action() const;
      // whether the current action should be executed automatically
      void set_auto_action_end(unsigned const auto_action_end);
      // whether the current action should be executed automatically
      bool auto_action() const;

    private:
      // initializes the bug report replay
      void init();

    private:
      // reads the whole file
      void read_file();

      // 'line' is expected -- test the current line
      bool expect_line(string const& read_line, string const& expected_line);
      // 'value' is expected -- test the current value
      bool expect_value(string const& read_value,
                        string const& expected_value);
      // 'keyword' is expected -- test the current keyword
      bool expect_keyword(string const& read_keyword,
                          string const& expected_keyword);


      // the current action has been processed
      void action_processed(GameplayAction const& action,
                            GameplayAction::Discrepancy const discrepancy);
      // check game action when requested
      void reference_check_game_action(GameplayAction const& action,
                                       GameplayAction::Discrepancy const
                                       discrepancy);

      // the current human action
      GameplayAction const& current_human_action() const;
      // handle the coming human action
      void handle_current_human_action();
      // print the coming human action
      void print_current_human_action() const;

      // this bug report is inconsistent
      void mismatch();
      // the end of the bug report is reached
      void end_reached();

    private:
      // the filename of the bugreport
      PRIV_VAR_R(string, filename);
      // the verbose level
      PRIV_VAR_R(unsigned, verbose);
      // whether this bug report was loaded successfully
      PRIV_VAR_R(bool, loaded);
      // whether this bug report is inconsistent
      PRIV_VAR_R(bool, inconsistent);
      // whether this bug report is finished
      PRIV_VAR_R(bool, finished);
      // whether this bug report is finished in the human parts
      PRIV_VAR_R(bool, human_finished);
      // whether the party should be started automatically
      PRIV_VAR_R(bool, auto_start_party);
      // till which action to process automatically
      PRIV_VAR_R(unsigned, auto_action_end);
      // whether to check the next action
      PRIV_VAR_R(bool, check_action);

      // the version
      PRIV_VAR_P_R(DK::Utils::Version, version);
      // the date of the compilation
      PRIV_VAR_R(string, compiled);
      // the system
      PRIV_VAR_R(string, system);
      // the time the bug report was created
      PRIV_VAR_R(string, time);
      // the language
      PRIV_VAR_R(string, language);
      // the trick of the bug report
      PRIV_VAR_R(unsigned, trickno);
      // the mesage
      PRIV_VAR_R(string, message);
      // the seed
      PRIV_VAR_R(unsigned, seed);
      // the number of the startplayer
      PRIV_VAR_R(unsigned, startplayer_no);
      // the gametype
      PRIV_VAR_R(::GameType, game_type);
      // the marriage selector
      PRIV_VAR_R(::MarriageSelector, marriage_selector);
      // the number of the soloplayer
      PRIV_VAR_R(unsigned, soloplayer_no);

      // the rule
      PRIV_VAR_R(::Rule, rule);
      // the players
      PRIV_VAR_R(vector< ::Player*>, players);
      // poverty cards
      PRIV_VAR_R(vector<Card>*, poverty_cards_shifted);
      PRIV_VAR_R(vector<Card>*, poverty_cards_returned);
      // hands
      PRIV_VAR_R(vector< ::Hand>, hands);
      // the number of the player with swines
      PRIV_VAR_R(unsigned, swines_player_no);
      // the number of the player with hyperswines
      PRIV_VAR_R(unsigned, hyperswines_player_no);
      // gameplay
      PRIV_VAR_R(vector< ::GameplayAction const*>, actions);
      // the number of the current action
      PRIV_VAR_R(unsigned, current_action_no);
      // whether the action is conform to the gameplay
      PRIV_VAR_R(vector<GameplayAction::Discrepancy>, actions_discrepancies);

      // full tricks
      PRIV_VAR_R(vector< ::Trick>, full_tricks);
      // game summary
      PRIV_VAR_R(::GameSummary*, game_summary);

      // current hands
      PRIV_VAR_R(vector< ::Hand>, current_hands);
      // current trick
      PRIV_VAR_P_R(::Trick, current_trick);
      // human actions
      PRIV_VAR_R(vector< ::GameplayAction const*>, human_actions);
      // the number of the current human action
      PRIV_VAR_R(unsigned, current_human_action_no);
      // whether the action is conform to the gameplay
      PRIV_VAR_R(vector<GameplayAction::Discrepancy>, human_actions_discrepancies);

    private: // not implemented
      BugReportReplay();
      BugReportReplay(BugReportReplay const&);
      BugReportReplay& operator=(BugReportReplay const&);
  }; // class BugReportReplay

} // namespace OS_NS

#endif // #ifndef CLASS_OS_BUG_REPORT_REPLAY_HEADER
