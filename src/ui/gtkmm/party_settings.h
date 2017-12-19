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

#ifndef GTKMM_PARTY_SETTINGS_HEADER
#define GTKMM_PARTY_SETTINGS_HEADER

#include "base.h"
#include "widgets/sticky_dialog.h"
namespace Gtk {
class Notebook;
class StockButton;
class Label;
class SpinButton;
class CheckButton;
class RadioButton;
class Frame;
} // namespace Gtk

class Setting;
class Player;
class Aiconfig;

namespace UI_GTKMM_NS {
class Players;
class Rules;

/**
 ** @brief	the party settings dialog
 **
 ** @author	Diether Knof
 **/
class PartySettings : public Base, public Gtk::StickyDialog {
  public:
    // constructor
    PartySettings(Base* parent);
    // destructor
    ~PartySettings();

    // get the party settings
    void get();
    // the shown signal
    void on_show();

    // load a bug report
    void load_bug_report();

    // update the sensitivity
    void sensitivity_update();
    // update the seed value sensitivity
    void seed_value_sensitivity_update();

    // update all
    void update();

    // update the rules (value, sensitivity)
    void rules_update();
    // the rules have changed
    void rule_change(int type);

    // switch the players
    void players_switched(::Player const& player_a,
                          ::Player const& player_b);
    // update the player
    void player_update(::Player const& player);
    // update the name of the player
    void name_update(::Player const& player);
  private:
    // update the name and font of the player
    void name_update_local(::Player const& player);
  public:
    // update the font of the player
    void voice_update(::Player const& player);
    // update the aiconfig
    void aiconfig_update(::Aiconfig const& aiconfig);

  private:
    // initialize the window
    void init();

    // event: start the party
    void start_party_event();

    // event: the seed has changed
    void seed_change_event();
    // event: the startplayer has changed
    void startplayer_change_event();

    // event: players have changed
    void switch_players_event(unsigned p);

    // event: a key press
    bool key_press(GdkEventKey* key);

#ifndef OUTDATED
    // remove, when the network is (mostly) stable
    void show_network_unstable_warning();
#endif

  public:
    Players* players = nullptr;
    Rules* rules = nullptr;

  private:
    Gtk::StockButton* load_bug_report_button = nullptr;
    Gtk::StockButton* start_party_button = nullptr;
    Gtk::StockButton* close_button = nullptr;

    Gtk::Frame* seed_frame = nullptr;
    Gtk::SpinButton* seed_value = nullptr;
    Gtk::CheckButton* seed_random = nullptr;

    Gtk::CheckButton* rule_number_of_rounds_limited = nullptr;
    Gtk::SpinButton* rule_number_of_rounds = nullptr;
    Gtk::CheckButton* rule_points_limited = nullptr;
    Gtk::SpinButton* rule_points = nullptr;

    Gtk::Frame* startplayer_frame = nullptr;
    vector<Gtk::RadioButton*> startplayer;
    Gtk::RadioButton* startplayer_random = nullptr;
    vector<Gtk::Button*> switch_players_buttons;

    Gtk::StockButton* configure_players = nullptr;
    Gtk::StockButton* configure_rules = nullptr;
#ifdef USE_NETWORK
    Gtk::StockButton* configure_network = nullptr;
#ifdef USE_NETWORK_DOKOLOUNGE
    Gtk::StockButton* dokolounge = nullptr;
#endif
#endif

  private: // unused
    PartySettings() = delete;
    PartySettings(PartySettings const&) = delete;
    PartySettings& operator=(PartySettings const&) = delete;
}; // class PartySettings : public Base, public Gtk::StickyDialog

} // namespace UI_GTKMM_NS

#endif // #ifdef GTKMM_PARTY_SETTINGS_HEADER

#endif // #ifdef USE_UI_GTKMM
