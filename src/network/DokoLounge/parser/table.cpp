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
#include "../receiver.h"
#include "../sender.h"

#include "../../../party/party.h"
#include "../../../party/rule.h"
#include "../../../game/game.h"
#include "../../../game/gameplay_actions.h"
#include "../../../card/trick.h"

#include "../../../misc/lounge.h"
#include "../../../utils/string.h"

#include <cstring>


#define PLAYERNO_FROM_FREEDOKO(P) (((P) + this->player_offset()) % this->rule()(Rule::NUMBER_OF_PLAYERS_IN_GAME) + 1)
#define PLAYERNO_TO_FREEDOKO(P) ((((P) - 1) + this->rule()(Rule::NUMBER_OF_PLAYERS_IN_GAME) - this->player_offset()) % this->rule()(Rule::NUMBER_OF_PLAYERS_IN_GAME))

/* Bemerkungen
 *
 * Der menschliche Spieler/Zuschauer ist immer auf Position 0. Gegebenenfalls muss daher die Position beim Stich umgerechnet werden.
 * Regen noch offen (wie genau ausgestaltet
 * – Doppelkopf in Solospielen
 * – Ansagen dürfen Limit auch überspringen
 * – Abweichungen vom DDV
 * – Schule
 *
 * 
 */ 

/* Status (nur Beobachter)
 *
 *  Erledigt:
 *  – Turnier starten
 *    o Regeln setzen
 *    o Spieler setzen
 *
 *  Offen:
 *  – Spiel beginnen
 *    o Karten verteilen
 *    o Startspieler setzen
 *    o Vorbehalte setzen
 *    o Karten ausspielen
 *    o Stiche einsammlen
 *    o Ansagen
 *  – Im Spiel einsteigen
 *  – selber spielen
 *  – Chat
 *  – Übersicht Zuschauer
 *  – Anzeige Regeln
 *  – Regeländerungen
 *
 *  Mehr Spieler: Einfach dem Turnier hinzufügen. Beim Start eines Spiels die Spieler entsprechend auswählen
 *
 *  In Spiel springen: Solange das Blatt von Spieler 0 noch unbekannte Karten hat, das Spiel nachstellen.
 *  - Vorbehalt: erst mal keine
 *  - Unbekannte Karten spielen, bis maximal eine unbekannte Karte im Blatt verbleibt
 *  - Noch für einen Stich unbekannte Karten spielen, falls der Spieler im Stich noch nicht gespielt hat
 *  - Den aktuellen Stich nachspielen, dabei die ausgespielte Karte vom Spieler noch der Hand zuführen
 *  - Anzahl der ausgespielten Karten merken
 *  - Startspieler ist Startspieler/Gewinner des Stichs
 *  
 * Tisch betreten: Refresh durchführen
 * Neues Spiel: Refresh für alle Spieler durchführen
 */ 

namespace Network {
  namespace DokoLounge {

    /**
     ** table chat message
     **
     ** @param     text        chat text
     **
     ** @return    whether the command was accepted
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.13
     **/
    bool
      Parser::TischChat(string text)
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
        if (text.find("(Gast) ") == 0)
          text.erase(0, 7);
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

        if (type == LoungeChatEntry::SYSTEM) {
          this->pending.push_back(pair<string, string>("TischChatSystem", text));
        }

        if (with_color)
          ::lounge->add_table_chat_entry(type, text, player, color);
        else
          ::lounge->add_table_chat_entry(type, text, player);
        return true;
      } // bool Parser::TischChat(string text)

    /**
     ** a table has been entered
     **
     ** @param     text        table number
     **
     ** @return    whether the command was accepted
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.13
     **/
    bool
      Parser::DeinTisch(string text)
      {
        // example:
        // <<DeinTisch>>1<</DeinTisch>>
        unsigned table = 0;
        DK::Utils::String::from_string(text, table);

        // todo: check the status

        return true;
      } // bool Parser::DeinTisch(string text)

    /**
     ** players at the table
     **
     ** @param     text        players list
     **
     ** @return    whether the command was accepted
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.13
     ** 
     ** @todo      setze die Spieler bei Spielstart
     **/
    bool
      Parser::alleSpielerAmTisch(string text)
      {
        // <<spieler1>>Diether<</spieler1>><<spieler2>>ELF-K<</spieler2>><<spieler3>>Redwitch<</spieler3>><<spieler4>>MacHolstein<</spieler4>>
        CLOG << text << endl;
        vector<string> player;

        { // Lese die Spieler aus
          string entry, keyword;
          while (!text.empty()) {
            this->strip_next_part(text, keyword, entry);
            player.push_back(entry);
            // Ergänze gegebenenfalls die Spieler in den Turnierspielern
            if (this->party().player_by_name(entry) == nullptr) {
              this->actor().add_party_player(entry);
              CLOG << entry << endl;
            }
          } // while (!text.empty())
        } // Lese die Spieler aus

        // Setze die Spieler des Spiels
        if (player.size() >= this->actor().party().rule()(Rule::NUMBER_OF_PLAYERS_IN_GAME)) {
          vector<Player*> game_players(this->actor().party().rule()(Rule::NUMBER_OF_PLAYERS_IN_GAME), nullptr);
          unsigned p;
          for (p = 0; p < this->actor().party().rule()(Rule::NUMBER_OF_PLAYERS_IN_GAME); ++p) {
            game_players[p] = this->actor().party().player_by_name(::lounge->table().players[PLAYERNO_FROM_FREEDOKO(p) - 1]);
            if (game_players[p] == nullptr)
              break;
          }
          if (p == this->actor().party().rule()(Rule::NUMBER_OF_PLAYERS_IN_GAME))
            this->actor().game_set_players(game_players);
        } // if (player.size() >= 4)

        return true;
      } // bool Parser::alleSpielerAmTisch(string text)

    /**
     ** new cards 
     **
     ** @param     text        hand
     **
     ** @return    whether the command was accepted
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.13
     ** 
     ** @todo      all
     **/
    bool
      Parser::CleanTisch(string text)
      {
        CLOG << text << endl;
        exit(0);
        return false;
      } // bool Parser::CleanTisch(string text)

    /**
     ** the hand
     **
     ** @param     text        hand
     **
     ** @return    whether the command was accepted
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.13
     ** 
     ** @todo      in a game: check the hand against the one of the current game
     **/
    bool
      Parser::Karten(string text)
      {
        Hand hand(Parser::cards(text));
        if (::game_status == GAMESTATUS::GAME_INIT) {
          // see Parser::init_game()
          return false;
        } // if (::game_status == GAMESTATUS::GAME_INIT)
        if (::game_status == GAMESTATUS::GAME_PLAY) {
          // ToDo: check the hand against the one of the current game
        } // if (::game_status == GAMESTATUS::GAME_PLAY)

        return false;
      } // bool Parser::Karten(string text)

    /**
     ** the trick
     **
     ** @param     text        trick
     **
     ** @return    whether the command was accepted
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.13
     **/
    bool
      Parser::KartenAufTisch(string text)
      {
        if (text == "00-00-00-00-")
          return true;
        vector<Card> const cards = Parser::cards(text);
        if (this->game().trick_current().isfull()) {
          CLOG << this->actor().pending_action() << endl;
          CLOG << *this->actor().pending_action() << endl;
          SEGFAULT;
        }
        //if (std::count(cards.begin(), cards.end(), Card::UNKNOWN) == 0)
        //exit(0);
        if (std::count(cards.begin(), cards.end(), Card::UNKNOWN)
            == this->game().trick_current().remainingcardno())
          return false;

        DEBUG_ASSERTION(std::count(cards.begin(), cards.end(), Card::UNKNOWN) + 1
                        == this->game().trick_current().remainingcardno(),
                        "Parser::KartenAufTisch(" << text << ")\n"
                        "   current trick does not match:\n"
                        << this->game().trick_current());
        unsigned const playerno = this->game().trick_current().actplayerno();
        Card const& card = cards[PLAYERNO_FROM_FREEDOKO(playerno) - 1];
        DEBUG_ASSERTION(card != Card::UNKNOWN,
                        "Parser::KartenAufTisch(" << text << ")\n"
                        "   got empty card for current player "
                        << this->game().player(PLAYERNO_FROM_FREEDOKO(playerno)).name() << " = "
                        << PLAYERNO_FROM_FREEDOKO(playerno) << "(= DokoLounge), "
                        << playerno << "(= FreeDoko)\n"
                        << "  player offset = " << this->player_offset() << '\n'
                        << "  player 0 = " << this->game().player(PLAYERNO_TO_FREEDOKO(1)).name());
        this->actor().gameplay_action(::GameplayAction::CardPlayed(playerno, card));
        return true;
      } // bool Parser::KartenAufTisch(string text)

    /**
     ** First card of the trick
     **
     ** @param     text        first card of the trick
     **
     ** @return    whether the command was accepted
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.13
     ** 
     ** @todo      all
     **/
    bool
      Parser::ersteKarte(string text)
      {
        Card const card = Parser::card(text);

        CLOG << card << endl;
        exit(0);
        return false;
      } // bool Parser::ersteKarte(string text)

    /**
     ** Which player has to play
     **
     ** @param     text        player whose turn it is
     **
     ** @return    whether the command was accepted
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.13
     ** 
     ** @todo      all
     **/
    bool
      Parser::SpielerIstDran(string text)
      {

        // 'frei' ist ein unbesetzter Platz
        if (text == "frei")
          return false;
        if (::game_status != GAMESTATUS::GAME_INIT)
          return false;
        this->actor().game_set_startplayer(this->playerno(text));
        return true;
      } // bool Parser::SpielerIstDran(string text)

    /**
     ** The player is watching anothor one
     **
     ** @param     text        spectators
     **
     ** @return    whether the command was accepted
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.13
     **/
    bool
      Parser::zuschauer(string text)
      {
        // <<zuschauer>>Diether (26) schaut Petra zu [1]
        ::lounge->set_audience(text);
        // prüfe, ob der Spieler noch einem anderen Spieler zuschaut
        if (*text.rbegin() != '\r')
          text += "\r";
        while (!text.empty()) {
          CLOG << text << endl;
          string observer = string(text, 0, text.find(" ("));
          text.erase(0, text.find(")"));
          EXPECT_TEXT(") schaut ");
          string player = string(text, 0, text.find(" zu"));
          text.erase(0, text.find(" zu") + 4); // with additional ' ' or '\r'
          CLOG << player << endl;
          // check for " [0]"
          if (text.size() >= 3) {
            if (   (text[0] == '[')
                && (text.find("]") != string::npos) )
              text.erase(0, text.find("]") + 2);
          }
          CLOG << text << endl;
          if (observer == ::lounge->account()) {
            if (::lounge->location() != Lounge::OBSERVER) {
              ::lounge->observe_player(player);
              for (unsigned p = 0; p < this->party().playerno(); ++p)
                this->actor().set_player_type(this->party().player(p), true);
            }
          }
          while (!text.empty()
                 && (   (text[0] == '\r')
                     || (text[0] == '\n')
                     || (text[0] == ' ')))
            text.erase(0, 1);
        } // while (!text.empty())
        return true;
      } // bool Parser::zuschauer(string text)

    /**
     ** the audience (who is only watching)
     **
     ** @param     text        observed player
     **
     ** @return    whether the command was accepted
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.13
     **/
    bool
      Parser::DuBistZuschauer(string text)
      {
        // <<DuBistZuschauer>>frei<</DuBistZuschauer>>
        DEBUG_ASSERTION(::game_status <= GAMESTATUS::PARTY_NEW,
                        "DuBistZuschauer(" << text << ")\n"
                        "  game status = " << ::name(::game_status) << " > PARTY_NEW");
        DEBUG_ASSERTION(::lounge->location() == Lounge::LOUNGE,
                        "DuBistZuschauer(" << text << ")\n"
                        "  location = " << ::lounge->location() << " != LOUNGE");
        ::lounge->observe_player(text);

        // Die Spieler an dem Tisch festsetzen, die Information ist bereits vorhanden.
        for (auto p : ::lounge->table().players)
          this->actor().add_party_player(p);

        return true;
      } // bool Parser::DuBistZuschauer(string text)

    /**
     ** Ein Tisch wurde betreten
     **
     ** @param     text        Tisch
     **
     ** @return    Ob das Kommando akzeptiert wurde
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.13
     **/
    bool
      Parser::TischErlaubt(string text)
      {
        // <<TischErlaubt>>1<</TischErlaubt>>
        DEBUG_ASSERTION (::game_status <= GAMESTATUS::PARTY_NEW,
                         "TischErlaubt(" << text << ")\n"
                         "  game status = " << ::name(::game_status) << " > PARTY_NEW");
        DEBUG_ASSERTION (::lounge->location() == Lounge::LOUNGE,
                         "TischErlaubt(" << text << ")\n"
                         "  location = " << ::lounge->location() << " != LOUNGE");
        int table = -1;
        DK::Utils::String::from_string(text, table);
        ::lounge->set_location(Lounge::TABLE, table - 1);
        return true;
      } // bool Parser::TischErlaubt(string text)

  } // namespace DokoLounge
} // namespace Network

#endif // #ifdef USE_NETWORK_DOKOLOUNGE
#endif // #ifdef USE_NETWORK
