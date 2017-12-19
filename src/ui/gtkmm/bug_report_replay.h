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

#ifdef USE_UI_GTKMM

#ifndef GTKMM_BUG_REPORT_REPLAY_HEADER
#define GTKMM_BUG_REPORT_REPLAY_HEADER

#include "base.h"
#include "widgets/sticky_dialog.h"
#include "../../os/os.h"

#include "../../party/party.h"
#include "../../party/rule.h"

#include <stack>
using std::stack;

#include <gtkmm/treemodel.h>
namespace Gtk {
class Notebook;
class Label;
class Button;
class TextView;
class CellRenderer;
} // namespace Gtk

class Trick;

namespace UI_GTKMM_NS {
class GameplayActions;
class GameSummary;

/**
 ** @brief	the bug report that is replayed
 **
 ** Note, this class gets the information directly over the OS,
 ** the functions must not be called by UI_GTKMM.
 ** In 'Ui_Wrap' this class must be inserted behind '::bug_report_replay'.
 **
 ** @todo	save changes in the message box
 **/
class BugReportReplay : public Base, public Gtk::StickyDialog, public OS {
  public:
    // Constructor
    BugReportReplay(Base* parent);
    // Destructor
    ~BugReportReplay();

    // the party is opened
    void party_open(Party const& party);
    // get the party settings
    void party_get_settings();

    // a game is opened
    void game_open(Game const& game);
    // the cards are redistributed
    void game_redistribute();

    // a gameplay action
    void gameplay_action(GameplayAction const& action);

    // a bug report replay is opened
    void bug_report_replay_open(OS_NS::BugReportReplay const&
                                bug_report_replay);
    // the bug report replay is closed
    void bug_report_replay_close();

  private:
    // initialize this object
    void init();

    // update the bug report information
    void update_info();
    // update the showing of the actions already made
    void update_actions_past();
    // update the bug report information
    void update_auto_actions_button();

    // automatically execute the actions till the selected
    void set_auto_actions();
    // end the bug report
    void end_bug_report();

    // reset the ais
    void reset_ais();

  public:
    // the name of 'player' has changed
    void name_changed(Player const& player);

  private:
    OS_NS::BugReportReplay* bug_report_replay = nullptr;
    Gtk::Notebook* notebook = nullptr;

    Gtk::Label* version = nullptr;
    Gtk::Label* compiled = nullptr;
    Gtk::Label* system = nullptr;
    Gtk::Label* time = nullptr;
    Gtk::Label* language = nullptr;
    Gtk::Label* trick = nullptr;
    Gtk::Button* reset_ais_button = nullptr;
    Gtk::TextView* message = nullptr;

    Gtk::Label* seed = nullptr;
    Gtk::Label* startplayer = nullptr;
    Gtk::Label* soloplayer = nullptr;
    // rules
    // players
    // game_replay (tricks)
    // hands
    // gamepoints

    GameplayActions* actions = nullptr;
    Gtk::Button*     auto_actions_button = nullptr;
    GameplayActions* human_actions = nullptr;
    GameSummary*     game_summary = nullptr;

  private: // unused
    BugReportReplay() = delete;
    BugReportReplay(BugReportReplay const&) = delete;
    BugReportReplay& operator=(BugReportReplay const&) = delete;
}; // class BugReportReplay : public Base, public Gtk::StickyDialog, public OS

} // namespace UI_GTKMM_NS

#endif // #ifdef GTKMM_BUG_REPORT_REPLAY_HEADER

#endif // #ifdef USE_UI_GTKMM
