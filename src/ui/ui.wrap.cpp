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

#include "ui.wrap.h"

#include "../card/hand.h"

#ifdef USE_NETWORK
#include "../network/listener.h"
#include "../network/connection.h"
#ifdef USE_NETWORK_DOKOLOUNGE
#include "../misc/lounge.h"
#endif
#endif


/**
 ** Constructor
 ** a ui of the type 'DUMMY' and no os
 **
 ** @param	-
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **/
UI_Wrap::UI_Wrap() :
UI(UI_TYPE::WRAP),
  ui(UI::new_(UI_TYPE::DUMMY)),
os()
{
  return ;
} // UI_Wrap::UI_Wrap()

/**
 ** Destructor
 **
 ** @param	-
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **/
UI_Wrap::~UI_Wrap()
{
  return ;
} // UI_Wrap::~UI_Wrap()

  /**
   ** sets the ui that is wrapped
   **
   ** @param	ui	the wrapped ui
   **
   ** @return	-
   **
   ** @author	Diether Knof
   **
   ** @version	0.6.7
   **/
void
UI_Wrap::ui_set(UI* const ui)
{ 
  delete this->ui;
  this->ui = ui;

  return ;
} // void UI_Wrap::ui_set(UI* const ui)

/**
 ** add the os to the wrapped os's
 **
 ** @param	os	os to be prepended
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.8
 **/
void
UI_Wrap::prepend(OS* const os)
{ 
  this->os.insert(this->os.begin(), os);

  return ;
} // void UI_Wrap::prepend(OS* const os)

/**
 ** add the os to the wrapped os's
 **
 ** @param	os	os to be appended
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **/
void
UI_Wrap::append(OS* const os)
{ 
  this->os.push_back(os);
  
  if (::game_status >= GAMESTATUS::PARTY_NEW)
    os->party_open(this->party());
  if (::game_status >= GAMESTATUS::GAME_NEW)
    os->game_open(this->game());

  return ;
} // void UI_Wrap::append(OS* const os)

/**
 ** remove 'os' from the wrapped 'os's
 **
 ** @param	os	os to be removeed
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **/
void
UI_Wrap::remove(OS* const os)
{ 
  for (vector<OS*>::iterator os_it = this->os.begin();
       os_it != this->os.end();
       os_it++)
    if (*os_it == os) {
      this->os.erase(os_it);
      break;
    } // if (*os_it == os) 

  return ;
} // void UI_Wrap::remove(OS* const os)

/**
 ** print all elements
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
UI_Wrap::print() const
{ 
  cout << "UI_Wrap:\n";
  for (vector<OS*>::const_iterator os_it = this->os.begin();
       os_it != this->os.end();
       os_it++)
    cout << "  os: " << *os_it << " (" << (*os_it)->type() << ")\n";
  cout << "  ui: " << this->ui << " (" << this->ui->type() << ")\n";

  return ;
} // void UI_Wrap::print() const

#ifndef DOXYGEN_SHOULD_SKIP_THIS

// macros for shorter code
// return_type UI_GTKMM::function()
#define WRAP_FUNCTION_0(return_type, function) \
  return_type \
UI_Wrap::function() \
{ \
  vector<OS*> oss = this->os; \
  for (vector<OS*>::iterator os = oss.begin(); os != oss.end(); os++) \
    (*os)->function(); \
  return this->ui->function(); \
}
// return_type UI_GTKMM::function(type var)
#define WRAP_FUNCTION_1(return_type, function, type) \
  return_type \
UI_Wrap::function(type var) \
{ \
  vector<OS*> oss = this->os; \
  for (vector<OS*>::iterator os = oss.begin(); os != oss.end(); os++) \
    (*os)->function(var); \
  return this->ui->function(var); \
}
// return_type UI_GTKMM::function(type1 var1, type2 var2)
#define WRAP_FUNCTION_2(return_type, function, type1, type2) \
  return_type \
UI_Wrap::function(type1 var1, type2 var2) \
{ \
  vector<OS*> oss = this->os; \
  for (vector<OS*>::iterator os = oss.begin(); os != oss.end(); os++) \
    (*os)->function(var1, var2); \
  return this->ui->function(var1, var2); \
}
// void UI_GTKMM::function()
#define WRAP_FUNCTION_VOID_0(function) \
  void \
UI_Wrap::function() \
{ \
  this->UI::function(); \
  vector<OS*> oss = this->os; \
  for (vector<OS*>::iterator os = oss.begin(); os != oss.end(); os++) \
    (*os)->function(); \
  this->ui->function(); \
  return ; \
}
// void UI_GTKMM::function(type var)
#define WRAP_FUNCTION_VOID_1(function, type) \
  void \
UI_Wrap::function(type var) \
{ \
  this->UI::function(var); \
  vector<OS*> oss = this->os; \
  for (vector<OS*>::iterator os = oss.begin(); os != oss.end(); os++) \
    (*os)->function(var); \
  this->ui->function(var); \
  return ; \
}
// void UI_GTKMM::function(type1 var1, type2 var2)
#define WRAP_FUNCTION_VOID_2(function, type1, type2) \
  void \
UI_Wrap::function(type1 var1, type2 var2) \
{ \
  this->UI::function(var1, var2); \
  vector<OS*> oss = this->os; \
  for (vector<OS*>::iterator os = oss.begin(); os != oss.end(); os++) \
    (*os)->function(var1, var2); \
  this->ui->function(var1, var2); \
  return ; \
}
// void UI_GTKMM::function(type1 var1, type2 var2, type3 var3)
#define WRAP_FUNCTION_VOID_3(function, type1, type2, type3) \
  void \
UI_Wrap::function(type1 var1, type2 var2, type3 var3) \
{ \
  this->UI::function(var1, var2, var3); \
  vector<OS*> oss = this->os; \
  for (vector<OS*>::iterator os = oss.begin(); os != oss.end(); os++) \
    (*os)->function(var1, var2, var3); \
  this->ui->function(var1, var2, var3); \
  return ; \
}

//WRAP_FUNCTION_VOID_2(init, int&, char**&)
void
UI_Wrap::init(int& argc, char**& argv)
{
  vector<OS*> oss = this->os;
  for (vector<OS*>::iterator os = oss.begin(); os != oss.end(); os++)
    (*os)->main_init(argc, argv);
  this->ui->init(argc, argv);
  return ;
}
WRAP_FUNCTION_VOID_0(quit)
WRAP_FUNCTION_VOID_0(update)
WRAP_FUNCTION_VOID_1(sleep, unsigned const)
WRAP_FUNCTION_VOID_0(language_changed)

#ifdef USE_NETWORK
#ifdef USE_NETWORK_DOKOLOUNGE
    // the lounge
WRAP_FUNCTION_VOID_1(lounge_logged_in, string const&)
WRAP_FUNCTION_VOID_0(lounge_logged_out)
WRAP_FUNCTION_VOID_1(lounge_location_changed, ::Lounge::Location const&)
WRAP_FUNCTION_VOID_1(lounge_chat_entry_added, ::LoungeChatEntry const&)
WRAP_FUNCTION_VOID_1(table_chat_entry_added, ::LoungeChatEntry const&)
WRAP_FUNCTION_VOID_1(lounge_help_changed, string const&)
WRAP_FUNCTION_VOID_1(lounge_blog_changed, string const&)
WRAP_FUNCTION_VOID_1(lounge_pin_board_changed, string const&)
WRAP_FUNCTION_VOID_1(lounge_messages_changed, string const&)
WRAP_FUNCTION_VOID_2(lounge_alert, string const&, string const&)
WRAP_FUNCTION_VOID_1(lounge_players_changed, vector<Lounge::Player> const&)
WRAP_FUNCTION_VOID_1(lounge_tables_changed, vector<Lounge::Table> const&)
WRAP_FUNCTION_VOID_2(lounge_winner_last_month_changed, string const&, int const)
WRAP_FUNCTION_VOID_1(table_audience_changed, string const&)
#endif
#endif

WRAP_FUNCTION_VOID_1(party_open, Party&)
WRAP_FUNCTION_VOID_0(party_get_settings)
WRAP_FUNCTION_VOID_0(party_loaded)
WRAP_FUNCTION_VOID_0(party_start)
WRAP_FUNCTION_VOID_1(party_start_round, unsigned const)
WRAP_FUNCTION_VOID_0(party_finish)
WRAP_FUNCTION_VOID_0(party_close)

WRAP_FUNCTION_VOID_1(gameplay_action, GameplayAction const&)
WRAP_FUNCTION_VOID_1(game_open, Game&)
WRAP_FUNCTION_VOID_0(game_cards_distributed)
WRAP_FUNCTION_VOID_0(game_redistribute)
WRAP_FUNCTION_VOID_1(reservation_ask, Player const&)
WRAP_FUNCTION_1(Reservation, reservation_get, Player const&)
WRAP_FUNCTION_VOID_2(reservation_got, Reservation const&, Player const&)
WRAP_FUNCTION_VOID_0(game_start)
WRAP_FUNCTION_VOID_0(game_finish)
WRAP_FUNCTION_VOID_0(game_close)

WRAP_FUNCTION_VOID_1(trick_open, Trick const&)
WRAP_FUNCTION_1(HandCard, card_get, Player const&)
WRAP_FUNCTION_VOID_1(card_played, HandCard const&)
WRAP_FUNCTION_VOID_2(announcement_made, Announcement const&, Player const&)
WRAP_FUNCTION_VOID_1(swines_announced, Player const&)
WRAP_FUNCTION_VOID_1(hyperswines_announced, Player const&)
WRAP_FUNCTION_VOID_2(ai_test_card, Card const&, unsigned const)
WRAP_FUNCTION_VOID_1(ai_card_weighting, int const)
WRAP_FUNCTION_VOID_1(virtual_card_played, HandCard const&)
WRAP_FUNCTION_VOID_0(virtual_trick_full)

WRAP_FUNCTION_VOID_1(poverty_shifting, Player const&)
WRAP_FUNCTION_VOID_2(poverty_shift, Player const&, unsigned const)
WRAP_FUNCTION_VOID_2(poverty_ask, Player const&, unsigned const)
WRAP_FUNCTION_VOID_1(poverty_take_denied, Player const&)
WRAP_FUNCTION_VOID_0(poverty_take_denied_by_all)
WRAP_FUNCTION_VOID_3(poverty_take_accepted,
                     Player const&, unsigned const, unsigned const)
WRAP_FUNCTION_1(HandCards, poverty_shift, Player&)
WRAP_FUNCTION_2(bool, poverty_take_accept, Player&, unsigned const)
WRAP_FUNCTION_2(HandCards, poverty_cards_change, Player&, HandCards const&)
WRAP_FUNCTION_VOID_2(poverty_cards_get_back, Player&, HandCards const&)

WRAP_FUNCTION_VOID_2(marriage, Player const&, Player const&)
WRAP_FUNCTION_0(Player const*, genscher_partner)
WRAP_FUNCTION_VOID_2(genscher, Player const&, Player const&)

WRAP_FUNCTION_VOID_0(trick_full)
WRAP_FUNCTION_VOID_0(trick_move_in_pile)
WRAP_FUNCTION_VOID_0(trick_close)

WRAP_FUNCTION_VOID_0(redraw_all)

WRAP_FUNCTION_VOID_2(players_switched, Player const&, Player const&)
WRAP_FUNCTION_VOID_1(player_changed, Player const&)
WRAP_FUNCTION_VOID_1(player_added, Player const&)
WRAP_FUNCTION_VOID_0(sparse_players_removed)
WRAP_FUNCTION_VOID_1(name_changed, Player const&)
WRAP_FUNCTION_VOID_1(voice_changed, Player const&)
WRAP_FUNCTION_VOID_1(hand_changed, Player const&)
WRAP_FUNCTION_VOID_1(teaminfo_changed, Player const&)
WRAP_FUNCTION_VOID_1(aiconfig_changed, Aiconfig const&)

WRAP_FUNCTION_VOID_0(gametype_changed)

WRAP_FUNCTION_VOID_0(party_seed_changed)
WRAP_FUNCTION_VOID_0(party_startplayer_changed)
WRAP_FUNCTION_VOID_2(rule_changed, int const, void const* const)

WRAP_FUNCTION_VOID_2(setting_changed, int const, void const* const)
WRAP_FUNCTION_VOID_1(bug_report_replay_open, OS_NS::BugReportReplay const&)

WRAP_FUNCTION_VOID_0(bug_report_replay_close)
WRAP_FUNCTION_VOID_1(help_load, string const&)
WRAP_FUNCTION_VOID_3(chat, string const&, string const&, bool const)

#ifdef USE_NETWORK
WRAP_FUNCTION_VOID_1(network_listener_new, Network::Listener const&)
WRAP_FUNCTION_VOID_1(network_listener_deleted, Network::Listener const&)
WRAP_FUNCTION_VOID_1(network_connection_new, Network::Connection const&)
WRAP_FUNCTION_VOID_1(network_connection_changed, Network::Connection const&)
WRAP_FUNCTION_VOID_1(network_connection_deleted, Network::Connection const&)
WRAP_FUNCTION_VOID_3(network_traffic,
                     Network::Connection const&,
                     Network::TrafficType const,
                     string const&)
#endif

WRAP_FUNCTION_VOID_1(status_message_changed, Translator::Translation const&)
WRAP_FUNCTION_VOID_0(set_busy)
WRAP_FUNCTION_VOID_0(set_not_busy)
WRAP_FUNCTION_VOID_1(progress_changed, double const)
WRAP_FUNCTION_VOID_0(progress_finished)
WRAP_FUNCTION_VOID_1(first_run, Translator::Translation const&)
WRAP_FUNCTION_VOID_1(program_updated, DK::Utils::Version const&)
WRAP_FUNCTION_VOID_3(information,
                     Translator::Translation const&, INFORMATION::Type const,
                     bool const)
WRAP_FUNCTION_VOID_1(error, string const&)

#undef WRAP_FUNCTION_0
#undef WRAP_FUNCTION_1
#undef WRAP_FUNCTION_2
#undef WRAP_FUNCTION_VOID_0
#undef WRAP_FUNCTION_VOID_1
#undef WRAP_FUNCTION_VOID_2
#undef WRAP_FUNCTION_VOID_3

#endif // #ifndef DOXYGEN_SHOULD_SKIP_THIS
