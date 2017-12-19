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

#ifndef CLASS_UI_WRAP_HEADER
#define CLASS_UI_WRAP_HEADER

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "ui.h"
#include "../os/os.h"

/**
 **
 ** @brief	contains an ui and some os
 ** executes the functions first for the os, then for the ui
 **
 ** @author	Diether Knof
 **
 **/
class UI_Wrap : public UI {
  public:
    UI_Wrap();
    ~UI_Wrap();

    // initialize the ui
    void init(int& argc, char**& argv);
    // quit the program
    void quit();

    // updates the UI
    void update();
    // sleeps 'sleep_msec' thousands of seconds
    // (< 0 for infinity)
    void sleep(unsigned const sleep_msec);
    // the language has changed
    void language_changed();

#ifdef USE_NETWORK
#ifdef USE_NETWORK_DOKOLOUNGE
    // the lounge
    void lounge_logged_in(string const& name);
    void lounge_logged_out();
    void lounge_location_changed(::Lounge::Location const& location);
    void lounge_chat_entry_added(::LoungeChatEntry const& entry);
    void table_chat_entry_added(::LoungeChatEntry const& entry);
    void lounge_help_changed(string const& text);
    void lounge_blog_changed(string const& text);
    void lounge_pin_board_changed(string const& text);
    void lounge_messages_changed(string const& text);
    void lounge_alert(string const& title, string const& text);
    void lounge_players_changed(vector<Lounge::Player> const& players);
    void lounge_tables_changed(vector<Lounge::Table> const& tables);
    void lounge_winner_last_month_changed(string const& winner, int const points);
    void table_audience_changed(string const& audience);
#endif
#endif

    // the parts of a party
    void party_open(Party& party);
    void party_get_settings();
    void party_loaded();
    void party_start();
    void party_start_round(unsigned const round);
    void party_finish();
    void party_close();

    // the parts of a game
    void gameplay_action(GameplayAction const& action);
    void game_open(Game& game);
    void game_cards_distributed();
    void game_redistribute();
    void reservation_ask(Player const& player);
    Reservation reservation_get(Player const& player);
    void reservation_got(Reservation const& reservation, Player const& player);
    void game_start();
    void game_finish();
    void game_close();

    // the parts of a trick
    void trick_open(Trick const& trick);
    void trick_full();
    void trick_move_in_pile();
    void trick_close();
    // get a card
    HandCard card_get(Player const& player);
    // the card is played
    void card_played(HandCard const& card);
    // an announcement is made
    void announcement_made(Announcement const& announcement,
			   Player const& player);
    // the player has swines
    void swines_announced(Player const& player);
    // the player has hyperswines
    void hyperswines_announced(Player const& player);

    // ai actions
    void ai_test_card(Card const& card, unsigned const playerno);
    void ai_card_weighting(int const weighting);
    void virtual_card_played(HandCard const& card);
    void virtual_trick_full();

    // 'player' has a poverty and shifts
    void poverty_shifting(Player const& player);
    // 'player' shifts 'cardno' cards
    void poverty_shift(Player const& player, unsigned const cardno);
    // ask 'player' whether to accept the poverty
    void poverty_ask(Player const& player, unsigned const cardno);
    // the player 'player' has denied the poverty trumps
    void poverty_take_denied(Player const& player);
    // all players have denied to take the cards
    void poverty_take_denied_by_all();
    // the player 'player' has accepted the poverty trumps
    // and has returned 'cardno' cards with 'trumpno' trumps
    void poverty_take_accepted(Player const& player,
			       unsigned const cardno,
			       unsigned const trumpno);
    // 'player' shifts cards
    HandCards poverty_shift(Player& player);
    // returns whether 'player' accepts the poverty
    bool poverty_take_accept(Player& player,
			     unsigned const cardno);
    // the player changes the cards of the poverty
    HandCards poverty_cards_change(Player& player,
				   HandCards const& cards);
    // the poverty player 'player' gets 'cards'
    void poverty_cards_get_back(Player& player,
				HandCards const& cards);

    // the marriage partner has found a bride
    void marriage(Player const& bridegroom, Player const& bride);
    // return a partner for a genscher
    Player const* genscher_partner();
    // a player has announced a genscher
    void genscher(Player const& genscher, Player const& partner);

    // redrawing
    void redraw_all();
    void gametype_changed();
    void players_switched(Player const& player_a, Player const& player_b);
    void player_changed(Player const& player);
    void player_added(Player const& player);
    void sparse_players_removed();
    void name_changed(Player const& player);
    void voice_changed(Player const& player);
    void hand_changed(Player const& player);
    void teaminfo_changed(Player const& player);
    void aiconfig_changed(Aiconfig const& aiconfig);

    void bug_report_replay_open(OS_NS::BugReportReplay const&
				bug_report_replay);
    void bug_report_replay_close();

    // change of the party seed
    void party_seed_changed();
    // change of the party startplayer
    void party_startplayer_changed();
    // change of the rules
    void rule_changed(int const type, void const* const old_value);
    // changes of the settings
    void setting_changed(int const type, void const* const old_value);
    // load the help page
    void help_load(string const& page);

    // 'player' has chatted 'message'
    void chat(string const& player, string const& message,
	      bool const self = true);
#ifdef USE_NETWORK
    // a new listener was created
    void network_listener_new(Network::Listener const& listener);
    // a listener was deleted
    void network_listener_deleted(Network::Listener const& listener);
    // a new connection was established
    void network_connection_new(Network::Connection const& connection);
    // a connection has changed
    void network_connection_changed(Network::Connection const& connection);
    // a connection is deleted
    void network_connection_deleted(Network::Connection const& connection);
    // there was network traffic over the given connection
    void network_traffic(Network::Connection const& connection,
			 Network::TrafficType const traffic_type,
			 string const& text);
#endif

    // the status message has changed
    void status_message_changed(Translator::Translation const& status_message);
    // the progress has changed
    void progress_changed(double const progress_max);
    // the progress is finished
    void progress_finished();

    void set_busy();
    void set_not_busy();

    // the first run of the program
    void first_run(Translator::Translation const& message);
    // an update of the version
    void program_updated(DK::Utils::Version const& old_version);
    // information for the user
    void information(Translator::Translation const& message,
		     INFORMATION::Type const type,
                     bool const force_show = false);
    // an error has occured
    void error(string const& message);

  public:
    void ui_set(UI* const ui);
    void prepend(OS* const os);
    void append(OS* const os);
    void remove(OS* const os);
    void print() const;

    UI* ui;
    vector<OS*> os;

  private: // not implemented
    UI_Wrap(UI_Wrap const&);
    UI_Wrap& operator=(UI_Wrap const&);
}; // class UI_Wrap

#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS

#endif // #ifndef CLASS_UI_WRAP_HEADER
