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

#ifdef USE_UI_GTKMM

#include "ui.h"

#ifdef USE_NETWORK
#ifdef USE_NETWORK_DOKOLOUNGE
#include "../../misc/lounge.h"
#include "dokolounge/lounge.h"

namespace UI_GTKMM_NS {

/**
 ** the user has logged in
 **
 ** @param     name   name of the player
 **
 ** @return    -
 **/
void
  UI_GTKMM::lounge_logged_in(string const& name)
  {
    auto const context = this->thrower.context();
    if (this->lounge)
      this->lounge->logged_in(name);
    return ;
  } // void UI_GTKMM::lounge_logged_in(string name)

/**
 ** the user has logged out
 **
 ** @param     -
 **
 ** @return    -
 **/
void
  UI_GTKMM::lounge_logged_out()
  {
    auto const context = this->thrower.context();
    if (this->lounge)
      this->lounge->logged_out();
    return ;
  } // void UI_GTKMM::lounge_logged_out()

/**
 ** the location has changed
 **
 ** @param     -
 **
 ** @return    -
 **/
void
  UI_GTKMM::lounge_location_changed(::Lounge::Location const& location)
  {
    auto const context = this->thrower.context();
    if (this->lounge)
      this->lounge->location_changed(location);
    return ;
  } // void UI_GTKMM::lounge_logged_out(::Lounge::Location location)

/**
 ** a chat entry has been added in the lounge
 **
 ** @param     entry   new chat entry
 **
 ** @return    -
 **/
void
  UI_GTKMM::lounge_chat_entry_added(::LoungeChatEntry const& entry)
  {
    auto const context = this->thrower.context();
    if (this->lounge)
      this->lounge->lounge_chat_entry_added(entry);
    return ;
  } // void UI_GTKMM::lounge_chat_entry_added(::LoungeChatEntry entry)

/**
 ** a chat entry has been added at a table
 **
 ** @param     entry   new chat entry
 **
 ** @return    -
 **/
void
  UI_GTKMM::table_chat_entry_added(::LoungeChatEntry const& entry)
  {
    auto const context = this->thrower.context();
    if (this->lounge)
      this->lounge->table_chat_entry_added(entry);
    return ;
  } // void UI_GTKMM::table_chat_entry_added(::LoungeChatEntry entry)

/**
 ** the help of the lounge has changed
 **
 ** @param     text   text of the help
 **
 ** @return    -
 **/
void
  UI_GTKMM::lounge_help_changed(string const& text)
  {
    auto const context = this->thrower.context();
    if (this->lounge)
      this->lounge->help_changed(text);
    return ;
  } // void UI_GTKMM::lounge_help_changed(string text)

/**
 ** the blog of the lounge has changed
 **
 ** @param     text   text of the blog
 **
 ** @return    -
 **/
void
  UI_GTKMM::lounge_blog_changed(string const& text)
  {
    auto const context = this->thrower.context();
    if (this->lounge)
      this->lounge->blog_changed(text);
    return ;
  } // void UI_GTKMM::lounge_blog_changed(string text)

/**
 ** the pin_board of the lounge has changed
 **
 ** @param     text   text of the pin_board
 **
 ** @return    -
 **/
void
  UI_GTKMM::lounge_pin_board_changed(string const& text)
  {
    auto const context = this->thrower.context();
    if (this->lounge)
      this->lounge->pin_board_changed(text);
    return ;
  } // void UI_GTKMM::lounge_pin_board_changed(string text)

/**
 ** the messages of the lounge has changed
 **
 ** @param     text   text of the messages
 **
 ** @return    -
 **/
void
  UI_GTKMM::lounge_messages_changed(string const& text)
  {
    auto const context = this->thrower.context();
    if (this->lounge)
      this->lounge->messages_changed(text);
    return ;
  } // void UI_GTKMM::lounge_messages_changed(string text)

/**
 ** an alert from the lounge
 **
 ** @param     title   alert title
 ** @param     text    text of the alert
 **
 ** @return    -
 **/
void
  UI_GTKMM::lounge_alert(string const& title, string const& text)
  {
    auto const context = this->thrower.context();
    if (this->lounge)
      this->lounge->show_alert(title, text);
    return ;
  } // void UI_GTKMM::lounge_alert(string title, string text)

/**
 ** the players have changed
 **
 ** @param     players    players list
 **
 ** @return    -
 **/
void
  UI_GTKMM::lounge_players_changed(vector< ::Lounge::Player> const& players)
  {
    auto const context = this->thrower.context();
    if (this->lounge)
      this->lounge->players_changed(players);
    return ;
  } // void UI_GTKMM::lounge_players_changed(vector< ::Lounge::Player> players)

/**
 ** the tables have changed
 **
 ** @param     tables    tables list
 **
 ** @return    -
 **/
void
  UI_GTKMM::lounge_tables_changed(vector< ::Lounge::Table> const& tables)
  {
    auto const context = this->thrower.context();
    if (this->lounge)
      this->lounge->tables_changed(tables);
    return ;
  } // void UI_GTKMM::lounge_tables_changed(vector< ::Lounge::Table> tables)

/**
 ** the audience has changed
 **
 ** @param     audience    new audience
 **
 ** @return    -
 **/
void
  UI_GTKMM::table_audience_changed(string const& audience)
  {
    auto const context = this->thrower.context();
    if (this->lounge)
      this->lounge->table_audience_changed(audience);
    return ;
  } // void UI_GTKMM::table_audience_changed(string audience)

} // namespace UI_GTKMM_NS

#endif // #ifdef USE_NETWORK_DOKOLOUNGE
#endif // #ifdef USE_NETWORK
#endif // #ifdef USE_UI_GTKMM
