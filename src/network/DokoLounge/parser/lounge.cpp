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
 ********************************************************************/

#include "constants.h"

#ifdef USE_NETWORK
#ifdef USE_NETWORK_DOKOLOUNGE

#include "parser.h"

#include "../../../party/party.h"
#include "../../../game/gameplay_action.h"
#include "../../../misc/lounge.h"
#include "../../../utils/string.h"

#include <cstring>

namespace Network {
  namespace DokoLounge {

    /**
     ** interpret an alert
     **
     ** @param     text        alert text
     **
     ** @return    whether the command was accepted
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.12
     ** 
     ** @todo      interpret buttons
     **/
    bool
      Parser::alerttext(string text)
      {
        // example:
        // <<bu2>><</bu2>><<bu1>>Fein<</bu1>><<alerthead>>Willkommen <</alerthead>><<alerttext>>Hallo liebe Lounger,<</alerttext>>
        string keyword;
        string entry;
        EXPECT_KEYWORD("bu2");
        EXPECT_KEYWORD("bu1");
        EXPECT_KEYWORD("alerthead");
        string const alerthead = entry;
        EXPECT_KEYWORD("alerttext");
        string const alerttext = entry;
        EXPECT_EMPTY_TEXT;
        CLOG << alerthead << endl;
        ::lounge->set_alert(alerthead, alerttext);
        //if (alerthead == "Willkommen FreeDoko")
          //this->sender().send_icon();

        return true;
      } // bool Parser::alerttext(string text)

    /**
     ** interpret a chat message
     **
     ** @param     text        chat text
     **
     ** @return    whether the command was accepted
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.12
     ** 
     ** @todo      interpret player
     ** @todo      interpret type
     **/
    bool
      Parser::fuxbauchat(string text)
      {
        LoungeChatEntry::Color color;
        bool with_color = false;
        string player = "";
        LoungeChatEntry::Type type = LoungeChatEntry::SYSTEM;
        if (string(text, 0, 7) == "<<col>>") {
          // chat message with a color
          string keyword;
          string entry;
          EXPECT_KEYWORD("col");
          if (entry.size() == 3 + 1 + 3 + 1 + 3) {
            int i;
            color.r = 100 * DK::Utils::String::from_string(string(entry, 0, 1), i);
            color.r += 10 * DK::Utils::String::from_string(string(entry, 1, 1), i);
            color.r += 1 * DK::Utils::String::from_string(string(entry, 2, 1), i);
            color.r /= 256;
            color.g = 100 * DK::Utils::String::from_string(string(entry, 4, 1), i);
            color.g += 10 * DK::Utils::String::from_string(string(entry, 5, 1), i);
            color.g += 1 * DK::Utils::String::from_string(string(entry, 6, 1), i);
            color.g /= 256;
            color.b = 100 * DK::Utils::String::from_string(string(entry, 8, 1), i);
            color.b += 10 * DK::Utils::String::from_string(string(entry, 9, 1), i);
            color.b += 1 * DK::Utils::String::from_string(string(entry, 10, 1), i);
            color.b /= 256;
            with_color = true;
          }
        } // if (string(text, 7) == "<<col>>")
        // interpret the player and type
        if (text.find(":") == string::npos) {
          type = LoungeChatEntry::SYSTEM;
        } else if (text.find(":") < text.find(" ")) {
          type = LoungeChatEntry::PLAYER;
          player = string(text, 0, text.find(":"));
        } else if (text.find(" flüstert:") == text.find(" ")) {
          type = LoungeChatEntry::PLAYER_WHISPER;
          player = string(text, 0, text.find(" flüstert:"));
        } else if (text.find(" an alle:") == text.find(" ")) {
          type = LoungeChatEntry::PLAYER_FOR_ALL;
          player = string(text, 0, text.find(" an alle:"));
        } else if (text.substr(0, string("Du flüsterst an ").size())
                   == "Du flüsterst an ") {
          type = LoungeChatEntry::PLAYER_WHISPER;
          player = ::lounge->account();
        } else {
          type = LoungeChatEntry::SYSTEM;
        } // if (text...)
        if (with_color)
          ::lounge->add_lounge_chat_entry(type, text, player, color);
        else
          ::lounge->add_lounge_chat_entry(type, text, player);
        return true;
      } // bool Parser::fuxbauchat(string text)

    /**
     ** interpret the player list
     **
     ** @param     text     list to interpret
     **
     ** @return    whether the command was accepted
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.12
     **/
    bool
      Parser::LobbySpieler(string text)
      {
        // <<LobbySpieler>><<anz>>3<</anz>><<User1>>MacHolstein<</User1>><<UserIcon1>>face4.jpg<</UserIcon1>><<User2>>Diether<</User2>><<UserIcon2>>tux.jpg<</UserIcon2>><<User3>>FreeDoko<</User3>><<UserIcon3>>fux1.jpg<</UserIcon3>><</LobbySpieler>>
        string keyword;
        string entry;
        EXPECT_KEYWORD("anz");
        int anz = 0;
        DK::Utils::String::from_string(entry, anz);
        if (anz == 0) {
          cerr << "'interpret player list': anz = 0: " << entry << endl;
          return false;
        }
        vector<Lounge::Player> player_list;
        for (int i = 1; i <= anz; ++i) {
          EXPECT_KEYWORD("User" + DK::Utils::String::to_string(i));
          string const name = entry;
          EXPECT_KEYWORD("UserIcon" + DK::Utils::String::to_string(i));
          string const icon = entry;
          player_list.push_back(Lounge::Player(name, icon));
        }
        EXPECT_EMPTY_TEXT;

        ::lounge->set_player_list(player_list);

        return true;
      } // bool Parser::LobbySpieler(string text)

    /**
     ** interpret the table list
     **
     ** @param     text     list to interpret
     **
     ** @return    whether the command was accepted
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.13
     **/
    bool
      Parser::alleMehrspielertische(string text)
      {
        // <<alleMehrspielertische>><<tischanz>>1<</tischanz>><<tisch1>><<tischart>>Sonderregeln<</tischart>><<spieleranz>>4<</spieleranz>><<spieler1>>frei<</spieler1>><<spieler2>>frei<</spieler2>><<spieler3>>frei<</spieler3>><<spieler4>>frei<</spieler4>><</tisch1>><</alleMehrspielertische>>

        string keyword;
        string entry;
        CLOG << text << endl;

        EXPECT_KEYWORD("tischanz");
        unsigned anz = 0;
        DK::Utils::String::from_string(entry, anz);
        if (anz == 0) {
          cerr << "'interpret table list': Tischanz = 0: " << entry << endl;
          return false;
        }
        ::lounge->set_table_number(anz);

        for (unsigned t = 0; t < anz; ++t) {
          EXPECT_KEYWORD("tisch" + DK::Utils::String::to_string(t + 1));
          string text = entry;
          EXPECT_KEYWORD("tischart");
          ::lounge->set_table_type(t, entry);
          EXPECT_KEYWORD("spieleranz");
          unsigned playerno = 0;
          DK::Utils::String::from_string(entry, playerno);
          vector<string> name(playerno);
          for (unsigned p = 0; p < playerno; ++p) {
            EXPECT_KEYWORD("spieler" + DK::Utils::String::to_string(p + 1));
            if (entry == "frei")
              name[p] = "";
            else
              name[p] = entry;
          }
          ::lounge->set_table_players(t, name);
        } // for (t < anz)

        return true;
      } // bool Parser::alleMehrspielertische(string text)

#ifdef OUTDATED
    // Version 58
    /**
     ** interpret the table list
     **
     ** @param     text     list to interpret
     **
     ** @return    whether the command was accepted
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.13
     **/
    bool
      Parser::alleTische(string text)
      {
        // <<alleTische>>(TischSpieler01)Diether,frei,frei,frei||(TischSpieler02)frei,frei,frei,frei||(TischSpieler03)frei,frei,frei,frei||(TischSpieler04)frei,frei,frei,frei||(TischSpieler05)frei,frei,frei,frei||<</alleTische>>

        vector<string> name(4);
        for (unsigned t = 0; t < ::lounge->tables().size(); ++t) {
          if (t < 9)
            EXPECT_TEXT("(TischSpieler0" + DK::Utils::String::to_string(t+1) + ")");
          else
            EXPECT_TEXT("(TischSpieler" + DK::Utils::String::to_string(t+1) + ")");
          // 1. name
          for (unsigned i = 0; i < 3; ++i) {
            name[i] = string(text, 0, text.find(','));
            if (name[i].size() == text.size()) {
              CLOG << endl;
              return false;
            }
            text = string(text, name[i].size() + 1);
          }
          name[3] = string(text, 0, text.find("||"));
          text = string(text, name[3].size() + 2);

          for (unsigned i = 0; i < 4; ++i)
            if (name[i] == "frei")
              name[i] = "";
          ::lounge->set_table_players(t, name);
        } // (for t)

        return true;
      } // bool Parser::alleTische(string text)
#endif

    /**
     ** interpret the table types
     **
     ** @param     text     types to interpret
     **
     ** @return    whether the command was accepted
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.12
     **/
    bool
      Parser::Tischart(string text)
      {
        // <<Tischart>><<Tischanz>>5<</Tischanz>><<1>>Sonderregeln<</1>><<2>>Lerntisch<</2>><<3>>Sonderregeln<</3>><<4>>DDV<</4>><<5>>Sonderregeln<</5>><</Tischart>>
        string keyword;
        string entry;
        EXPECT_KEYWORD("Tischanz");
        unsigned anz = 0;
        DK::Utils::String::from_string(entry, anz);
        if (anz == 0) {
          cerr << "'interpret table list': Tischanz = 0: " << entry << endl;
          return false;
        }
        ::lounge->set_table_number(anz);
#ifdef OUTDATED
        // 0.7.13, 2013-12-14 DK
        vector<Lounge::Table> table_list;
        for (unsigned i = 1; i <= anz; ++i) {
          EXPECT_KEYWORD(DK::Utils::String::to_string(i));
          string const type = entry;
          table_list.push_back(Lounge::Table(type));
        }

        ::lounge->set_table_list(table_list);
#endif
        for (unsigned t = 0; t < anz; ++t) {
          EXPECT_KEYWORD(DK::Utils::String::to_string(t + 1));
          string const type = entry;
          ::lounge->set_table_type(t, type);
        }
        EXPECT_EMPTY_TEXT;

        return true;
      } // bool Parser::Tischart(string text)

    /**
     ** The winner of the last month
     **
     ** @param     text     name and points
     **
     ** @return    whether the command was accepted
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.13
     **/
    bool
      Parser::Gluckspilz(string text)
      {
        // <<Gluckspilz>><<name>>Petra<</name>><<taler>>174<</taler>><</Gluckspilz>><<
        string keyword;
        string entry;

        EXPECT_KEYWORD("name");
        string const name = entry;

        EXPECT_KEYWORD("taler");
        int points = 0;
        DK::Utils::String::from_string(entry, points);

        // ToDo
        ::lounge->set_winner_last_month(name, points);

        return true;
      } // bool Parser::Gluckspilz(string text)
  } // namespace DokoLounge
} // namespace Network

#endif // #ifdef USE_NETWORK_DOKOLOUNGE
#endif // #ifdef USE_NETWORK
