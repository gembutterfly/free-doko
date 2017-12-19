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

#ifdef USE_NETWORK
#ifdef USE_NETWORK_DOKOLOUNGE

#ifndef CLASS_LOUNGE_HEADER
#define CLASS_LOUNGE_HEADER

class Lounge;
extern Lounge* lounge;

#include <ctime>

/**
 ** A lounge
 ** Till now: for DokoLounge (http://www.doko-lounge.de/)
 **
 ** @author   Diether Knof
 **/
struct LoungeChatEntry {
  struct Color {
    public:
      Color() :
        r(0), g(0), b(0)
    { }
      Color(double const r, double const g, double const b) :
        r(r), g(g), b(b)
    { }
    public:
      double r, g, b;
  };
  enum Type {
    SYSTEM,
    PLAYER,
    PLAYER_WHISPER,
    PLAYER_FOR_ALL
  }; // enum Type
  public:
  LoungeChatEntry(Type const& type, string const& text) :
    type(type), time(::time(NULL)), text(text), player(""), color(), with_color(false)
  { }
  LoungeChatEntry(Type const& type, string const& text, string const& player) :
    type(type), time(::time(NULL)), text(text), player(player), color(), with_color(false)
  { }
  LoungeChatEntry(Type const& type, string const& text, string const& player,
                  Color const& color) :
    type(type), time(::time(NULL)), text(text), player(player), color(color), with_color(true)
  { }

  public:
  Type type;
  time_t time;
  string text;
  string player;
  Color color;
  bool with_color;
}; // class LoungeChatEntry

class Lounge {
  public:
    class Player {
      public:
        Player(string const& name, string const& icon) :
          name(name), icon(icon)
      { }
      public:
        string name;
        string icon;
    }; // class Player
    enum Location {
      LOUNGE,
      TABLE,
      OBSERVER
    }; // enum Location

    class Table {
      public:
        Table(string const& type) :
          type(type), players()
      { }
        void set_players(vector<string> const& players)
        { this->players = players; }
      public:
        string type;
        vector<string> players;
    }; // class Table

  public:
    // constructor
    Lounge();
    // destructor
    ~Lounge();

    // clear the lounge
    void clear();

    // whether the player is logged in
    bool is_logged_in() const;
    // the player
    Player const& player(string const& name) const;
    // the location of the player (table number or -1)
    int table_of_player(string const& name) const;
    // the position of the player at a table
    int position_of_player(string const& name) const;

    // set the location
    void set_location(Location const& location, int const table = -1);
    // observe the player
    void observe_player(string const& name);

    // the table, the player is at
    Table const& table() const;
    // the table i
    Table const& table(unsigned const t) const;

    // logged in
    void logged_in(string const& name);
    // logged out
    void logged_out();

    // add a chat entry
    void add_lounge_chat_entry(LoungeChatEntry::Type const& type,
                        string const& text, string const& player,
                        LoungeChatEntry::Color const& color);
    void add_lounge_chat_entry(LoungeChatEntry::Type const& type,
                        string const& text, string const& player);
    // add a chat entry
    void add_table_chat_entry(LoungeChatEntry::Type const& type,
                        string const& text, string const& player,
                        LoungeChatEntry::Color const& color);
    void add_table_chat_entry(LoungeChatEntry::Type const& type,
                        string const& text, string const& player);
    // set the help
    void set_help(string const& text);
    // set the help
    void set_blog(string const& text);
    // set the pinnwand
    void set_pin_board(string const& text);
    // set the messages
    void set_messages(string const& text);
    // set the alert
    void set_alert(string const& title, string const& text);
    // set the player list
    void set_player_list(vector<Player> const& players);
    // set the table list
    void set_table_list(vector<Table> const& tables);
    // set the table number
    void set_table_number(unsigned const table_number);
    // set the table type
    void set_table_type(unsigned const table,
                        string const& type);
    // set the table players
    void set_table_players(unsigned const table,
                           vector<string> const& players);
    // set the audience
    void set_winner_last_month(string const& player, int const points);
    // set the audience
    void set_audience(string const& audience);

  private:
    // set the party players to one human and 3 dummy players
    void reset_party_players();

  private:
    PRIV_VAR_R(string, account); // the account of the player, if logged in
    // Tische
    // Spieler
    PRIV_VAR_R(vector<LoungeChatEntry>, lounge_chat); // the chat entries in the lounge
    PRIV_VAR_R(vector<LoungeChatEntry>, table_chat); // the chat entries at a table
    PRIV_VAR_R(string, help); // the help text
    PRIV_VAR_R(string, blog); // the blog text
    PRIV_VAR_R(string, pin_board); // the pin board text
    PRIV_VAR_R(string, messages); // the messages (individual pin board)
    PRIV_VAR_R(string, alert_title); // the alert title
    PRIV_VAR_R(string, alert); // the alert text
    PRIV_VAR_R(vector<Player>, players); // the players list
    PRIV_VAR_R(vector<Table>, tables); // the tables list
    PRIV_VAR_R(Location, location); // the players location
    PRIV_VAR_R(int, location_table); // the players location (table number)
    PRIV_VAR_R(string, observed_player); // the observed player
    PRIV_VAR_R(string, winner_last_month); // the winner of the last month
    PRIV_VAR_R(int, winner_last_month_points); // the points of the winner of the last month
    PRIV_VAR_R(string, audience); // the audience text

  private: // unused
    Lounge(Lounge const&);
    Lounge& operator=(Lounge const&);
}; // class Lounge

string name(LoungeChatEntry::Type const& type);
string name(Lounge::Location const& location);
WRITE_NAME(Lounge::Location)

#endif // #ifndef CLASS_LOUNGE_HEADER

#endif // #ifdef USE_NETWORK_DOKOLOUNGE
#endif // #ifdef USE_NETWORK
