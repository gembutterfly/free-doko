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

#ifndef CLASS_OS_HEADER
#define CLASS_OS_HEADER

#include "../card/card.h"
#include "../party/party.h"
#include "../misc/translation.h"

class Trick;
class Player;
class Aiconfig;
class Hand;
class Reservation;
class GameplayAction;
namespace OS_NS {
  class BugReportReplay;
}
#ifdef USE_NETWORK
#ifdef USE_NETWORK_DOKOLOUNGE
#include "../misc/lounge.h"
#endif
namespace Network {
  class Listener;
  class Connection;
} // namespace Network
#include "../network/types.h"
#endif
#include "../utils/version.h"

namespace OS_TYPE {
  enum OSType {
    UNNAMED,
    GAMEPLAY,
    PARTY_POINTS,
    SEED,
    OS_BUG_REPORT,
    OS_BUG_REPORT_REPLAY,
    AUTO_BUG_REPORT,
    SOUND,
#ifdef USE_NETWORK
    SERVER,
    NETWORK_LOG,
    NETWORK_PARSER,
#endif
    CHATTER
  };
} // namespace OS_TYPE

/**
 **
 ** the base os (output system) class
 **
 ** @author	Diether Knof
 **
 **/
class OS {
  friend OS* new_OS(OS_TYPE::OSType const type);

  public:
  OS(OS_TYPE::OSType const type = OS_TYPE::UNNAMED);
  OS(OS const&);
  OS& operator=(OS const&);

  virtual ~OS();

  OS_TYPE::OSType type() const;

  // initialize the OS
  virtual void main_init(int& argc, char**& argv)		{ }
    // quit the program
    virtual void quit() { }

  // updates the OS
  virtual void update()					{ }
  // sleeps 'sleep_usec' thousands of seconds
  // (< 0 for infinity)
  virtual void sleep(unsigned const sleep_usec)		{ this->update(); }
  // updates the language
  virtual void language_changed()			{ }

#ifdef USE_NETWORK
#ifdef USE_NETWORK_DOKOLOUNGE
    // the lounge
    virtual void lounge_logged_in(string const& name) { }
    virtual void lounge_logged_out() { }
    virtual void lounge_location_changed(::Lounge::Location const& location) { }
    virtual void lounge_chat_entry_added(::LoungeChatEntry const& entry) { }
    virtual void table_chat_entry_added(::LoungeChatEntry const& entry) { }
    virtual void lounge_help_changed(string const& text) { }
    virtual void lounge_blog_changed(string const& text) { }
    virtual void lounge_pin_board_changed(string const& text) { }
    virtual void lounge_messages_changed(string const& text) { }
    virtual void lounge_alert(string const& title, string const& text) { }
    virtual void lounge_players_changed(vector<Lounge::Player> const& players) { }
    virtual void lounge_tables_changed(vector<Lounge::Table> const& tables) { }
    virtual void lounge_winner_last_month_changed(string const& winner, int const points) { }
    virtual void table_audience_changed(string const& audience) { }
#endif
#endif

  // the parts of a party
  virtual void party_open(Party const& party); // call from childs
  virtual void party_get_settings()			{ }
  virtual void party_loaded()				{ }
  virtual void party_start()				{ }
  virtual void party_start_round(unsigned const round)	{ }
  virtual void party_finish()				{ }
  virtual void party_close(); // call from childs

  virtual void gameplay_action(GameplayAction const& action) { }

  // the parts of a game
  virtual void game_open(Game const& game); // call from childs
  virtual void game_cards_distributed()			{ }
  virtual void game_redistribute()			{ }
  virtual void reservation_ask(Player const& player)    { }
  virtual void reservation_get(Player const& player)	{ }
  virtual void reservation_got(Reservation const& reservation,
			       Player const& player)    { }
  virtual void game_start()				{ }
  virtual void game_finish()				{ }
  virtual void game_close(); // call from childs

  // the parts of a trick
  virtual void trick_open(Trick const& trick); // call from childs
  virtual void trick_full()				{ }
  virtual void trick_move_in_pile()			{ }
  virtual void trick_close(); // call from childs

  // get a card
  virtual void card_get(Player const& player)		{ }
  // the card is played
  virtual void card_played(HandCard const& card) { }
  // an announcement is made
  virtual void announcement_made(Announcement const& announcement,
				 Player const& player)	{ }
  // the player has swines
  virtual void swines_announced(Player const& player)	{ }
  // the player has hyperswines
  virtual void hyperswines_announced(Player const& player) { }

  // ai actions
  virtual void ai_test_card(Card const& card,
			    unsigned const playerno)	{ }
  virtual void ai_card_weighting(int const weighting)	{ }
  virtual void virtual_card_played(HandCard const& card){ }
  virtual void virtual_trick_full() { }

  // 'player' has a poverty and shifts
  virtual void poverty_shifting(Player const& player)	{ }
  // 'player' shifts 'cardno' cards
  virtual void poverty_shift(Player const& player,
			     unsigned const cardno)	{ }
  // ask 'player' whether to accept the poverty
  virtual void poverty_ask(Player const& player,
			   unsigned const cardno)	{ }
  // the player 'player' has denied the poverty trumps
  virtual void poverty_take_denied(Player const& player) { }
  // all players have denied to take the cards
  virtual void poverty_take_denied_by_all()		{ }
  // the player 'player' has accepted the poverty trumps
  // and has returned 'cardno' cards with 'trumpno' trumps
  virtual void poverty_take_accepted(Player const& player,
				     unsigned const cardno,
				     unsigned const trumpno) { }
  // 'player' shifts cards
  virtual void poverty_shift(Player const& player)	{ }
  // returns whether 'player' accepts the poverty
  virtual void poverty_take_accept(Player const& player,
				   unsigned const cardno) { }
  // the player changes the cards of the poverty
  virtual void poverty_cards_change(Player const& player,
				    HandCards const& cards)	{ }
  // the poverty player 'player' gets 'cards'
  virtual void poverty_cards_get_back(Player const& player,
				      HandCards const& cards) { }

  // the marriage partner has found a bride
  virtual void marriage(Player const& bridegroom, Player const& bride) { }
  // a player searchs a partner for a genscher
  virtual void genscher_partner()			{ }
  // a player has announced a genscher
  virtual void genscher(Player const& genscher, Player const& partner) { }

  // redrawing
  virtual void redraw_all()				{ }
  virtual void gametype_changed()			{ }
  virtual void players_switched(Player const& player_a,
				Player const& player_b) { }
  virtual void player_changed(Player const& player)
  { this->name_changed(player); this->voice_changed(player); }
  virtual void player_added(Player const& player) { }
  virtual void sparse_players_removed() { }
  virtual void name_changed(Player const& player)	{ }
  virtual void voice_changed(Player const& player)	{ }
  virtual void hand_changed(Player const& player)	{ }
  virtual void teaminfo_changed(Player const& player)	{ }
  virtual void aiconfig_changed(Aiconfig const& aiconfig)	{ }

  // change of the party seed
  virtual void party_seed_changed() { }
  // change of the party startplayer
  virtual void party_startplayer_changed() { }
  // change of the rules
  virtual void rule_changed(int const type, void const* const old_value) { }
  // changes of the settings
  virtual void setting_changed(int const type, void const* const old_value) { }

  virtual void bug_report_replay_open(OS_NS::BugReportReplay const&
                                      bug_report_replay);
  virtual void bug_report_replay_close();

  // load the help page
  virtual void help_load(string const& page)		{ }

  // 'player' has chatted 'message'
  virtual void chat(string const& player, string const& message,
                    bool const self = true) { }
#ifdef USE_NETWORK
  // a new listener was created
  virtual void network_listener_new(Network::Listener const&
                                    listener) { }
  // a listener was deleted
  virtual void network_listener_deleted(Network::Listener const&
                                        listener) { }
  // a new connection was established
  virtual void network_connection_new(Network::Connection const&
                                      connection) { }
  // a connection has changed
  virtual void network_connection_changed(Network::Connection const&
                                          connection) { }
  // a connection is deleted
  virtual void network_connection_deleted(Network::Connection const&
                                          connection) { }
  // there was network traffic over the given connection
  virtual void network_traffic(Network::Connection const& connection,
                               Network::TrafficType const traffic_type,
                               string const& text) { }
#endif

  // for status messages
  Translator::Translation status_message() const;
  void add_status_message(Translator::Translation const& status_message);
  void remove_status_message(Translator::Translation const& status_message);
  virtual void status_message_changed(Translator::Translation const&
                                      status_message)	{ }

  // whether the ui is busy
  virtual void set_busy();
  virtual void set_not_busy();

  // for progress
  double progress() const;
  void set_max_progress(double const progress_max);
  void add_progress(double const progress);
  void finish_progress();
  virtual void progress_changed(double const progress_max) { }
  virtual void progress_finished()			{ }

  // the first run of the program
  virtual void first_run(Translator::Translation const& message) { }
  // an update of the version
  virtual void program_updated(DK::Utils::Version const& old_version) { }
  // information for the user
  virtual void information(Translator::Translation const& message,
                           INFORMATION::Type const type,
                           bool const force_show = false)
  { }

  // an error has occured
  virtual void error(string const& message)		{ }

  // add the message to the shown messages set
  bool add_to_messages_shown(Translator::Translation const& message);

  protected:
  OS_TYPE::OSType type_;

  private:
  PRIV_VAR_CP_R(Party, party);
  PRIV_VAR_CP_R(Game, game);
  PRIV_VAR_CP_R(Trick, trick);

  PRIV_VAR_CP_R(OS_NS::BugReportReplay, bug_report_replay);

  vector<Translator::Translation> status_message_;
  PRIV_VAR_R(bool, busy);
  PRIV_VAR(double, progress);
  PRIV_VAR(double, progress_max);

  // set of shown messages
  set<Translator::Translation> messages_shown_;

  private: // not to be used
}; // class OS

OS* new_OS(const OS_TYPE::OSType type_a);

#endif // #ifndef CLASS_OS_HEADER
