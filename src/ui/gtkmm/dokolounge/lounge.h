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
#ifdef USE_NETWORK
#ifdef USE_NETWORK_DOKOLOUNGE

#ifndef GTKMM_DOKOLOUNGE_HEADER
#define GTKMM_DOKOLOUNGE_HEADER

#include "../base.h"
#include "../widgets/sticky_dialog.h"
#include "../widgets/scaled_pixbuf.h"

#include <map>
using std::map;

namespace UI_GTKMM_NS {
  namespace DokoLounge {
    class Help;
    class Blog;
    class PinBoard;
    class Messages;
    class Alert;
    class Chat;
  }; // namespace DokoLounge 
}; // namespace UI_GTKMM_NS 

#include "../../../misc/lounge.h"

namespace Gtk {
  class Label;
  class StockButton;
  class Table;
  class TextView;
  class Entry;
  class Paned;
}; // namespace Gtk
#include "../../../network/DokoLounge/sender.h"

namespace UI_GTKMM_NS {
  namespace DokoLounge {
    class Lounge : public Base, public Gtk::StickyDialog {
      friend class Chat;
      public:
      // constructor
      Lounge(Base* parent);
      // destructor
      ~Lounge();

      // the lounge is shown
      void on_show();

      // the game is logged in
      void logged_in(string const& name);
      // the game is logged out
      void logged_out();

      // the location has changed
      void location_changed(::Lounge::Location const& location);

      // the color of a player
      Gdk::Color const& player_color(string const& name);

      // a chat entry has been added in the loung
      void lounge_chat_entry_added(::LoungeChatEntry const& entry);
      // a chat entry has been added at a table
      void table_chat_entry_added(::LoungeChatEntry const& entry);
      // the help has been changed
      void help_changed(string const& text);
      // the blog has been changed
      void blog_changed(string const& text);
      // the pin board has been changed
      void pin_board_changed(string const& text);
      // the messages has been changed
      void messages_changed(string const& text);
      // an alert
      void show_alert(string const& title, string const& text);
      // the players have been changed
      void players_changed(vector< ::Lounge::Player> const& players);
      // the tables have been changed
      void tables_changed(vector< ::Lounge::Table> const& tables);
      // the audience has changed
      void table_audience_changed(string const& audience);


      private:
      // initialize the Lounge
      void init();
      // update the sensitivity
      void sensitivity_update();

      // close the window
      void close();

      // connect to the server
      void connect_server();
      // connect to the server
      void create_account_signal();
      // connect to the server
      void login_signal();

      // signals

      // logout
      void logout_signal();
      // refresh
      void refresh_signal();
      // klingel
      void klingel_signal();
      // hilfe
      void hilfe_signal();
      // blog
      void blog_signal();
      // pinnwand
      void pinnwand_signal();
      // messages
      void messages_signal();

      // enter a table
      void enter_table_signal(int table);
      // enter a table
      void enter_table_signalw(Gtk::Widget* widget, int table);
      // follow a player
      void follow_player_signal(string name);


      // update the player colors
      void update_player_colors();
      // recreate the players list
      void recreate_players_list();
      // recreate the tables list
      void recreate_tables_list();

      // the icon of player 'player'
      Gdk::ScaledPixbuf& player_icon(string const& player);
      // the icon of player 'player' for the table
      Gdk::ScaledPixbuf& player_table_icon(string const& player);
      // the icon of table type 'type'
      Gdk::ScaledPixbuf& table_icon(string const& type);

      private:
      ::Network::Connection* connection = nullptr;
      ::Network::DokoLounge::Interpreter::Sender* sender = nullptr;

      Help* help = nullptr;
      Blog* blog = nullptr;
      PinBoard* pin_board = nullptr;
      Messages* messages = nullptr;
      Alert* alert = nullptr;
      Chat* lounge_chat = nullptr;
      public: // for MainWindow in order to get the key signals
      Chat* table_chat = nullptr;
      private:

      Gtk::Label* name = nullptr;
      Gtk::StockButton* create_account_button = nullptr;
      Gtk::StockButton* login_button = nullptr;
      Gtk::StockButton* logout_button = nullptr;
      Gtk::StockButton* refresh_button = nullptr;
      Gtk::StockButton* klingel_button = nullptr;
      Gtk::StockButton* hilfe_button = nullptr;
      Gtk::StockButton* blog_button = nullptr;
      Gtk::StockButton* pinnwand_button = nullptr;
      Gtk::StockButton* messages_button = nullptr;
      Gtk::Table* players_list = nullptr;
      Gtk::Table* tables_list = nullptr;
      Gtk::TextView* audience_text = nullptr;
      Gtk::Paned* table_paned = nullptr;

      /*
         Rangliste
         */

      // close button
      Gtk::StockButton* close_button = nullptr;

      queue<Gdk::Color> player_colors_reserve;
      std::map<string, Gdk::Color> player_colors;
      std::map<string, Gdk::ScaledPixbuf> player_icons;
      std::map<string, Gdk::ScaledPixbuf> table_icons;

      private: // unused
      Lounge();
      Lounge(Lounge const&);
      Lounge& operator=(Lounge const&);
    }; // class Lounge
  } // namespace DokoLounge
} // namespace UI_GTKMM_NS

#endif // #ifndef GTKMM_DOKOLOUNGE_HEADER
#endif // #ifdef USE_NETWORK_DOKOLOUNGE
#endif // #ifdef USE_NETWORK
#endif // #ifdef USE_UI_GTKMM
