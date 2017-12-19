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

#include "auto_bug_report.h"
#include "bug_report.h"

#include "../party/party.h"
#include "../game/game_summary.h"
#include "../game/game.h"
#include "../card/trick.h"

#include "../utils/string.h"

namespace OS_NS {

  /**
   ** constructor
   **
   ** @param	-
   **
   ** @return	-
   **
   ** @author	Diether Knof
   ** 
   ** @version	0.7.4
   **/
  AutoBugReport::AutoBugReport() :
    OS(OS_TYPE::AUTO_BUG_REPORT)
  { }

  /**
   ** destructor
   **
   ** @param	-
   **
   ** @return	-
   **
   ** @author	Diether Knof
   ** 
   ** @version	0.7.4
   **/
  AutoBugReport::~AutoBugReport()
  { }

  /**
   ** the trick is full
   ** create automatic bug reports
   **
   ** @param	-
   **
   ** @return	-
   **
   ** @author	Diether Knof
   ** 
   ** @version	0.7.12
   **/
  void
    AutoBugReport::trick_full()
    {
      Game const& game = this->game();
      Trick const& trick = game.trick_current();
      for (unsigned c = 0; c < trick.actcardno(); ++c) {
        HandCard const& card = trick.card(c);
        Player const& player = trick.player_of_card(c);

        if (player != trick.winnerplayer()) {
          if (card.isdolle()) {
            this->create_report("dolle lost:\n"
                                + DK::Utils::String::to_string(trick)
                               );
          }
          if (   card.isfox()
              && (game.team(player) != game.team(trick.winnerplayer())) ) {
            this->create_report("fox lost:\n"
                                + DK::Utils::String::to_string(trick)
                               );
          }
          if (card.isswine()) {
            this->create_report("swine lost:\n"
                                + DK::Utils::String::to_string(trick)
                               );
          }
        } // if (player != trick.winnerplayer())
      } // for (c)


      return ;
    } // void AutoBugReport::trick_full()

  /**
   ** the game is finished
   ** create automatic bug reports
   **
   ** @param	-
   **
   ** @return	-
   **
   ** @author	Diether Knof
   ** 
   ** @version	0.7.4
   **/
  void
    AutoBugReport::game_finish()
    {
      GameSummary const& game_summary = this->party().last_game_summary();
      { // lost poverty
        if (   (game_summary.game_type() == GAMETYPE::POVERTY)
            && (game_summary.winnerteam() != TEAM::RE) )
          this->create_report("poverty lost by re"
                              " ("
                              + DK::Utils::String::to_string(game_summary.trick_points(TEAM::RE))
                              + ")");
      } // lost poverty
      { // lost solo
        if (   GAMETYPE::is_solo(game_summary.game_type())
            && (game_summary.winnerteam() != TEAM::RE)
            && (game_summary.trick_points(TEAM::RE) < 90) )
          this->create_report(::name(game_summary.game_type()) + " lost by re"
                              + " ("
                              + DK::Utils::String::to_string(game_summary.trick_points(TEAM::RE))
                              + ")");
      } // lost solo

      { // bad announcement
        if (   ANNOUNCEMENT::is_real(game_summary.highest_announcement_re())
            && (game_summary.trick_points(TEAM::RE)
                < (ANNOUNCEMENT::needed_points(game_summary.highest_announcement_re())
                   - 2 * 30) )
           )
          this->create_report("bad annoucement for re: "
                              + ::name(game_summary.highest_announcement_re())
                              + " ("
                              + DK::Utils::String::to_string(game_summary.trick_points(TEAM::CONTRA))
                              + ")");
        if (   (game_summary.highest_announcement_re()
                == ANNOUNCEMENT::NOANNOUNCEMENT)
            && (game_summary.trick_points(TEAM::RE)
                > (ANNOUNCEMENT::needed_points(ANNOUNCEMENT::NO120))
                + 2 * 30)
           )
          this->create_report("no annoucement for re: "
                              + DK::Utils::String::to_string(game_summary.trick_points(TEAM::CONTRA))
                              + " points for contra");

        if (   ANNOUNCEMENT::is_real(game_summary.highest_announcement_contra())
            && (game_summary.trick_points(TEAM::CONTRA)
                < (ANNOUNCEMENT::needed_points(game_summary.highest_announcement_contra())
                   - 2 * 30) )
           )
          this->create_report("bad annoucement for contra: "
                              + ::name(game_summary.highest_announcement_contra())
                              + " ("
                              + DK::Utils::String::to_string(game_summary.trick_points(TEAM::RE))
                              + ")");
        if (   (game_summary.highest_announcement_contra()
                == ANNOUNCEMENT::NOANNOUNCEMENT)
            && (game_summary.trick_points(TEAM::CONTRA)
                > (ANNOUNCEMENT::needed_points(ANNOUNCEMENT::NO120))
                + 2 * 30)
           )
          this->create_report("no annoucement for contra: "
                              + DK::Utils::String::to_string(game_summary.trick_points(TEAM::RE))
                              + " points for re");

        if (   (game_summary.game_type() == GAMETYPE::POVERTY)
            && (game_summary.winnerteam() != TEAM::RE) ) {
        } // if (bad announcement)
      } // bad announcement

      return ;
    } // void AutoBugReport::game_finish()

  /**
   ** create the report
   **
   ** @param	text   message of the bug report
   **
   ** @return	-
   **
   ** @author	Diether Knof
   ** 
   ** @version	0.7.4
   **/
  void
    AutoBugReport::create_report(string const& text) const
    {
      cout << "automatic bug report creation\n"
        << "  " << ::bug_report->filename() << '\n'
        << "  " << text << '\n'
        << endl;

      ::bug_report->report(text, "automatic");

      return ;
    } // void AutoBugReport::create_report(string text) const

} // namespace OS_NS
