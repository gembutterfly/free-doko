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
#ifdef USE_NETWORK
#ifdef USE_NETWORK_DOKOLOUNGE

#include "lounge.h"

#include "help.h"
#include "blog.h"
#include "pin_board.h"
#include "messages.h"
#include "alert.h"

#include "../ui.h"
#include "../translations.h"
#include "../../../misc/setting.h"

#include "../../../misc/lounge.h"
#include "../../../network/server.h"
#include "../../../network/connection.h"
#include "../../../network/DokoLounge/sender.h"
#include "../../../utils/string.h"

#include <gtkmm/entry.h>

// todo: use dummy connection (text output) for debugging
//
// Umlaute konvertieren
// Eigenes Flüstern farbig
// Fett
// neuer Tisch
// Tisch löschen
// chat: Str-Return: an alle
// chat: Shift-Return: an letzten Empfänger flüstern
// Tastenkombinationen: an Hauptfenster durchreichen
// Größe vom Kartensatz abhängig machen

namespace UI_GTKMM_NS {
  namespace DokoLounge {

    /**
     ** create an account at DokoLounge
     **
     ** @param     -
     **
     ** @return    -
     ** 
     ** @todo      set the icon to tux.jpg -- perhaps in the settings, then check it after the login
     **/
    void
      Lounge::create_account_signal()
      {
        // already connected?
        // todo: also check sender for already 
        if (this->sender)
          return;
        this->connect_server();

        if (this->sender == nullptr)
          return ;
        this->sender->create_account();
        //this->sender->login();

        return ;
      } // void Lounge::create_account_signal()

    /**
     ** loging into DokoLounge
     **
     ** @param     -
     **
     ** @return    -
     **/
    void
      Lounge::login_signal()
      {
        // already connected?
        if (this->sender)
          return;

        this->connect_server();
        if (this->sender == nullptr)
          return ;
        this->sender->login();
        return ;
      } // void Lounge::login_signal()

    /**
     ** logout from DokoLounge
     **
     ** @param     -
     **
     ** @return    -
     **/
    void
      Lounge::logout_signal()
      {
        if (!this->sender)
          return;

        this->sender->logout();
        ::server->delete_connection(this->connection);
        this->sender = nullptr;
        this->connection = nullptr;
        return;
      } // void Lounge::logout_signal()

    /**
     ** refresh
     **
     ** @param     -
     **
     ** @return    -
     **/
    void
      Lounge::refresh_signal()
      {
        if (!this->sender)
          return;

        this->sender->refresh();
        return;
      } // void Lounge::refresh_signal()

    /**
     ** klingel from DokoLounge
     **
     ** @param     -
     **
     ** @return    -
     **/
    void
      Lounge::klingel_signal()
      {
        if (!this->sender)
          return;

        this->sender->klingel();
        return;
      } // void Lounge::klingel_signal()

    /**
     ** hilfe from DokoLounge
     **
     ** @param     -
     **
     ** @return    -
     **/
    void
      Lounge::hilfe_signal()
      {
        if (!this->sender)
          return;

        this->sender->hilfe();
        return;
      } // void Lounge::hilfe_signal()

    /**
     ** blog from DokoLounge
     **
     ** @param     -
     **
     ** @return    -
     **/
    void
      Lounge::blog_signal()
      {
        if (!this->sender)
          return;

        this->sender->blog();
        return;
      } // void Lounge::blog_signal()

    /**
     ** pinnwand from DokoLounge
     **
     ** @param     -
     **
     ** @return    -
     **/
    void
      Lounge::pinnwand_signal()
      {
        if (!this->sender)
          return;

        this->sender->pinnwand();
        return;
      } // void Lounge::pinnwand_signal()

    /**
     ** messages from DokoLounge
     **
     ** @param     -
     **
     ** @return    -
     **/
    void
      Lounge::messages_signal()
      {
        if (!this->sender)
          return;

        this->sender->messages();
        return;
      } // void Lounge::messages_signal()

    /**
     ** the player enters the table
     **
     ** @param     table   table number
     **
     ** @return    -
     **/
    void
      Lounge::enter_table_signal(int const table)
      {
        this->sender->enter_table(table);
        return ;
      } // void Lounge::enter_table_signal(int const table)

    /**
     ** the player follows another player
     **
     ** @param     name   player to follow
     **
     ** @return    -
     **/
    void
      Lounge::follow_player_signal(string const name)
      {
        this->sender->follow_player(name);
        return ;
      } // void Lounge::follow_player_signal(string name)

  } // namespace DokoLounge
} // namespace UI_GTKMM_NS

#endif // #ifdef USE_NETWORK_DOKOLOUNGE
#endif // #ifdef USE_NETWORK
#endif // #ifdef USE_UI_GTKMM
