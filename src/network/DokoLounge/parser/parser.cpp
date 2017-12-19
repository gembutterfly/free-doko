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
#include "../sender.h"

#include "../../../party/party.h"
#include "../../../game/gameplay_action.h"
#include "../../../misc/lounge.h"
#include "../../../ui/ui.wrap.h"

#include <cstring>

namespace Network {
  namespace DokoLounge {

    // static values

    // Ob die statischen Variablen initialisiert sind
    bool Parser::static_initialized = false;
    // Abbildung von einer Nummer (als Zeichenkette) zur entsprechenden Karte (wie "15" sowie "39" für Kreuz Dame)
    std::map<string, Card> Parser::cards_map;
    std::map<Card, string> Parser::cards_reverse_map1;
    std::map<Card, string> Parser::cards_reverse_map2;


    /*
<<neuesKonto>>Der Name "FreeDoko" ist leider schon vergeben.
Bitte ein Passwort mit mindestens 4 Buchsteben/Zahlen wählen.<</neuesKonto>>

Mit falscher Version angemeldet
<<deinName>><</deinName>><<fuxbauchat>>Bitte lade dir von unserer Homepage (www.doko-lounge.de) eine aktuelle Version herunter. Mit dieser Version kannst du nicht mehr spielen.<</fuxbauchat>><<offline>><</offline>>


Anzahl der Spieler hat sich geändert
<<LobbySpieler>><<anz>>3<</anz>><<User1>>MacHolstein<</User1>><<UserIcon1>>face4.jpg<</UserIcon1>><<User2>>Diether<</User2>><<UserIcon2>>tux.jpg<</UserIcon2>><<User3>>FreeDoko<</User3>><<UserIcon3>>fux1.jpg<</UserIcon3>><</LobbySpieler>>

Spieler ist einem Tisch beigetreten
<<Tischart>><<Tischanz>>5<</Tischanz>><<1>>Sonderregeln<</1>><<2>>Lerntisch<</2>><<3>>Sonderregeln<</3>><<4>>DDV<</4>><<5>>Sonderregeln<</5>><</Tischart>>
< <<alleTische>>(TischSpieler01)Diether,frei,frei,frei||(TischSpieler02)frei,frei,frei,frei||(TischSpieler03)frei,frei,frei,frei||(TischSpieler04)frei,frei,frei,frei||(TischSpieler05)frei,frei,frei,frei||<</alleTische>><<Tischart>><<Tischanz>>5<</Tischanz>><<1>>Sonderregeln<</1>><<2>>Lerntisch<</2>><<3>>Sonderregeln<</3>><<4>>DDV<</4>><<5>>Sonderregeln<</5>><</Tischart>><<alleTische>>(TischSpieler01)Diether,frei,frei,frei||(TischSpieler02)frei,frei,frei,frei||(TischSpieler03)frei,frei,frei,frei||(TischSpieler04)frei,frei,frei,frei||(TischSpieler05)frei,frei,frei,frei||<</alleTische>>


zuschauen

unknown keyword: TischErlaubt -- 3
unknown keyword: alleSpielerAmTisch -- <<spieler1>>FreeDoko<</spieler1>><<spieler2>>frei<</spieler2>><<spieler3>>frei<</spieler3>><<spieler4>>frei<</spieler4>>
unknown keyword: NeuePunkte -- <<FreeDoko>>0<</FreeDoko>><<frei>>0<</frei>><<frei>>0<</frei>><<frei>>0<</frei>>
unknown keyword: KartenAufTisch -- 00-00-00-00-
<<gastverlassen>>Diether<</gastverlassen>>

NeueFaces: neue Bilder -- ignorieren

unknown keyword: Karten -- 00-00-00-00-00-00-00-00-00-00-00-00-
unknown keyword: KartenAufTisch -- 00-00-00-00-
unknown keyword: ersteKarte -- 0
unknown keyword: SpielerIstDran -- frei
unknown keyword: NeuePunkte -- <<Diether>>6<</Diether>><<frei>>0<</frei>><<frei>>0<</frei>><<frei>>0<</frei>>
unknown keyword: zuschauer -- FreeDoko(0) schaut Diether zu
unknown keyword: Karten -- 35-14-42-17-47-32-20-26-34-01-09-28-
receiver.cpp#223  TischChat: Diether hat gegeben.
unknown keyword: Meldung -- Diether ist gesund.
unknown keyword: AnsageCount -- 12
unknown keyword: sonderregeln -- 
unknown keyword: regelinfo1 -- <<tischinfo>>Sonderregeln<</tischinfo>><<ohneSupersau>>1<</ohneSupersau>><<ersted>>1<</ersted>><<skir>>0<</skir>><<armut>>1<</armut>><<oNeunen>>0<</oNeunen>>
<<IchWillTischVerlassen>>Diether<</IchWillTischVerlassen>>

unknown keyword: deinIcon -- tux.jpg


*/

    /**
     ** constructor
     **
     ** @param	interpreter	corresponding interpreter
     **
     ** @return	-
     **
     ** @author	Diether Knof
     **
     ** @version	0.7.13
     **/
    Parser::Parser(Connection::Interpreter& interpreter) :
      OS(OS_TYPE::NETWORK_PARSER),
      interpreter(&interpreter),
      pending(),
      reservation_open(false),
      pending_reservation(),
      player_offset_(UINT_MAX),
      pending_gameplay_actions_()
    {
      Parser::init_static();
      dynamic_cast<UI_Wrap*>(::ui)->append(this);
      return;
    } // Parser::Parser(Connection::Interpreter& interpreter)

    /**
     ** destructor
     **
     ** @param     -
     **
     ** @return    -
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.13
     **/
    Parser::~Parser()
    {
      dynamic_cast<UI_Wrap*>(::ui)->remove(this);
    } // Parser::~Parser()

    /**
     ** -> result
     **
     ** @param     -
     **
     ** @return    the corresponding actor
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.13
     **/
    Connection::Actor&
      Parser::actor()
      {
        return this->interpreter->connection().actor();
      } // Connection::Actor& Parser::actor()

    /**
     ** -> result
     **
     ** @param     -
     **
     ** @return    the corresponding actor
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.13
     **/
    Connection::Actor const&
      Parser::actor() const
      {
        return this->interpreter->connection().actor();
      } // Connection::Actor const& Parser::actor() const

    /**
     ** -> result
     **
     ** @param     -
     **
     ** @return    the corresponding sender
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.13
     **/
    Interpreter::Sender&
      Parser::sender()
      {
        return dynamic_cast<Interpreter::Sender&>(this->interpreter->sender());
      } // Interpreter::Sender& Parser::sender()

    /**
     ** -> result
     **
     ** @param     -
     **
     ** @return    the corresponding party
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.13
     **/
    Party const&
      Parser::party() const
      {
        return this->actor().party();
      } // Party const& Parser::party() const

    /**
     ** -> result
     **
     ** @param     -
     **
     ** @return    the corresponding rule
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.13
     **/
    Rule const&
      Parser::rule() const
      {
        return this->party().rule();
      } // Rule const& Parser::rule() const

    /**
     ** -> result
     **
     ** @param     -
     **
     ** @return    the corresponding game
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.13
     **/
    Game const&
      Parser::game() const
      {
        return this->party().game();
      } // Game const& Parser::game() const

    /**
     ** Empfange ein Kommando
     ** Interpretier es direkt oder schiebe es in die Schlange
     **
     ** @param     keyword     keyword (command)
     ** @param     text        command text
     **
     ** @return    -
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.13
     **/
    void
      Parser::receive_command(string const& keyword,
                              string const& text)
      {
        if (   (keyword == "bell")
            || (keyword == "alert")
            || (keyword == "alerttext")
            || (keyword == "fuxbauchat")
            //|| (keyword == "LobbySpieler")
            //|| (keyword == "alleMehrspielertische")
            //|| (keyword == "TischChat")
           ) {
          this->interpret_command(keyword, text);
        } else if (   (keyword == "NeueFaces")
                   || (keyword == "NeueBilder") ) {
          // Ausnahme: neue Icons einfach wegwerfen
        } else {
          this->pending.push_back(pair<string, string>(keyword, text));
        }
        return ;
      } // void Parser::receive_command(string keyword, string text)

    /**
     ** Interpretiere den Befehl
     ** Direkt in dieser Funktion werden nur einfache allgemeine Befehle interpretiert. 
     ** Befehle und Befehlsgruppen für den Tisch werden in check_pending_table() interpretiert.
     **
     ** @param     keyword     keyword (command)
     ** @param     text        command text
     **
     ** @return    Ob das Kommando akzeptiert wurde (es wird dann gelöscht)
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.13
     **/
    bool
      Parser::interpret_command(string const& keyword,
                                string const& text)
      {
        //CLOG << "< " << keyword << "   " << text << endl;
        //this->list_pending();
        if (keyword == "EOF") {
          // Ende der Datei (wird automatisch am Ende der eingelesenen Datei für Testzwecke aufgerufen)
          cout << "Ende der Datei erreicht. Beende Programm. " << endl;
          throw(GAMESTATUS::QUIT);
        } else if (keyword == "neuesKonto") {
          // Fehler beim neuen Konto
          CLOG << "neuesKonto: " << text << endl;
          return true;
        } else if (keyword == "offline") {
          // Anmeldung nicht erfolgreich
          return true;
        } else if (keyword == "deinName") {
          // Anmeldung
          ::lounge->logged_in(text);
          return true;
        } else if (keyword == "NeueFaces") {
          // Neue Bilder
          // <<NeueFaces>><<lanz>>4<</lanz>><<bild1>><<name>>knut.jpg<</name>><<pic>>255.....<</pic>><</bild1>><<bild2>>...<</bild4>><</NeueFaces>>
          // ignorieren
          CLOG << "NeueFaces: ignorieren" << endl;
          return true;
        } else if (keyword == "NeueBilder") {
          CLOG << "NeueBilder: ignorieren" << endl;
          return true;
        } else if (keyword == "deinIcon") {
          // Das Icon wurde zurückgesendet
          // ToDo: check against the setting
          CLOG << "Icon: " << text << endl;
          return true;
        } else if (keyword == "neuerEintrag") {
          // Neuer Eintrag in der Pinnwand
          CLOG << keyword << ": " << text << endl;
          return true;
        } else if (keyword == "bell") {
          // Die Klingel wurde geläutet
          CLOG << "bell: " << text << endl;
          return true;
        } else if (keyword == "alert") {
          // ignore it, see hack
          return true;
        } else if (keyword == "alert2") {
          // Erst einmal ignorieren
          return true;
        } else if (keyword == "alerttext") {
          // contains alert, alerthead, bu1, bu2, alerttext
          this->alerttext(text);
          return true;
        } else if (keyword == "hilfe") {
          // Hilfetext
          ::lounge->set_help(text);
          return true;
        } else if (keyword == "blogtxt") {
          // Blogtext
          ::lounge->set_blog(text);
          return true;
        } else if (keyword == "PinnwandData") {
          // Pinnwand
          ::lounge->set_pin_board(text);
          return true;
        } else if (keyword == "mail") {
          // Eigene Pinnwand / Nachrichten
          ::lounge->set_messages(text);
          return true;
        } else if (keyword == "fuxbauchat") {
          // Chat, mit Farbe/Spielername
          this->fuxbauchat(text);
          return true;
        } else if (keyword == "LobbySpieler") {
          this->LobbySpieler(text);
          return true;
        } else if (keyword == "alleMehrspielertische") {
          this->alleMehrspielertische(text);
          return true;
        } else if (keyword == "Gluckspilz") {
          this->Gluckspilz(text);
          return true;
        } else if (keyword == "KeinZuschauerMehr") {
          ::lounge->set_location(Lounge::LOUNGE);
          return true;
        } else if (keyword == "Tischart") {
          return true;

          // Tisch

        } else if (keyword == "TischChat") {
          // Chat am Tisch
          this->TischChat(text);
          return true;
        } else if (keyword == "DuBistZuschauer") {
          // Als Zuschauer an den Tisch setzen
          this->DuBistZuschauer(text);
          return true;
        } else if (keyword == "TischErlaubt") {
          // Als Spieler an den Tisch setzen
          this->TischErlaubt(text);
          return true;
        } else if (keyword == "zuschauer") {
          // Zuschauerliste
          this->zuschauer(text);
          return true;
        } else if (keyword == "alleSpielerAmTisch") {
          // Spielerliste
          this->alleSpielerAmTisch(text);
          return true;
        } else if (keyword == "regelinfo1") {
          // Regeln
          this->regelinfo1(text);
          return true;
        } // if (keyword == ...)

        return false;
      } // bool Parser::interpret_command(string keyword, string text)

    /**
     ** check pending commands
     **
     ** @param     -
     **
     ** @return    -
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.13
     **/
    void
      Parser::check_pending()
      {
        if (this->pending.empty())
          return ;
        if (this->blocking())
          return ;
        { // Einen einzelnen Befehl interpretieren
          pair<string, string> const& command = this->pending.front();
          if (this->interpret_command(command.first, command.second)) {
            this->pending.pop_front();
            this->check_pending();
          }
        }
        return ;
      } // void Parser::check_pending()

    /**
     ** -> result
     **
     ** @param     -
     **
     ** @return    Ob ein blockierendes Element vorliegt (zum Beispiel gameplay action) das vor weiterer Interpretation des Netzwerkverkehrs abgearbeitet werden muss
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.13
     **/
    bool
      Parser::blocking()
      {
        bool const debug_info = false;
        // a gameplay action has not passed the game
        if (this->actor().pending_action()) {
          if (debug_info)
            CLOG << *this->actor().pending_action() << endl;
          return true;
        }
        if (!this->pending_gameplay_actions_.empty()) {
          if (debug_info)
            CLOG << *this->pending_gameplay_actions_.front() << endl;
          return true;
        }

        return false;
      } // bool Parser::blocking()

    /**
     ** -> result
     **
     ** @param     text    text
     ** @param     part    part to search
     **
     ** @return    the text of <<part>>text<</part>>
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.13
     **/
    string
      Parser::text_of_part(string const& text, string const& part)
      {
        if (   (text.find("<<" + part + ">>") == string::npos)
            || (text.find("<</" + part + ">>") == string::npos)
            || (text.find("<<" + part + ">>")
                > text.find("<</" + part + ">>")) ) {
          return "";
        }
        return string(text,
                      text.find("<<" + part + ">>") + string("<<" + part + ">>").size(),
                      text.find("<</" + part + ">>") - text.find("<<" + part + ">>") - string("<<" + part + ">>").size());
      } // static string Parser::text_of_part(string text, string part)


    /**
     ** gets the next part of 'text' and remove it from text
     ** <<part>>entry<</part>>moretext
     **
     ** @param     text      text (will be changed)
     ** @param     keyword   keyword of the part (will be overwritten)
     ** @param     entry     entry of the part (will be overwritten)
     **
     ** @return    whether a part was found
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.13
     **/
    bool
      Parser::strip_next_part(string& text,
                              string& keyword, string& entry)
      {
        if (text.size() < 2 + 1 + 2 + 3 + 1 + 2) // minimum: <<a>><</a>>
          return false;

        if (!(   (text[0] == '<')
              && (text[1] == '<') ) )
          return false;

        if (text.find(">>") == string::npos) {
          return false;
        }
        string const keyword_t = string(text, 2, text.find(">>") - 2);

        string::size_type const n = text.find("<</" + keyword_t + ">>");
        if (n == string::npos) {
          return false;
        }

        keyword = keyword_t;
        entry = string(text,
                       ("<<" + keyword + ">>").size(),
                       n - ("<<" + keyword + ">>").size());
        text.erase(0, n + ("<</" + keyword + ">>").size());

        return true;
      } // bool Parser::strip_next_part(string& text, string& keyword, string& entry)

    /**
     ** gets the next part of 'text' and remove it from text
     ** Name: moretext
     **
     ** @param     text      text (will be changed)
     ** @param     name      name of the part (will be overwritten)
     ** @param     entry     entry of the part (will be overwritten)
     **
     ** @return    whether a name was found
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.13
     **/
    bool
      Parser::strip_next_name(string& text,
                              string& name, string& entry)
      {
        if (text.size() < 1 + 2 + 1) // minimum: N: x
          return false;

        if (text.find(": ") == string::npos) {
          return false;
        }

        name = string(text, 2, text.find(": ") - 2);
        entry = string(text, (name + ": ").size());
        text.erase(0, (name + ": ").size());

        return true;
      } // bool Parser::strip_next_name(string& text, string& name, string& entry)

    /**
     ** -> result
     **
     ** @param     text        two digits
     **
     ** @return    the corresponding card
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.13
     **/
    Card const&
      Parser::card(string const& text)
      {
        return Parser::cards_map[text];
      } // Card Parser::card(string text)

    /**
     ** -> result
     **
     ** @param     card   a card
     **
     ** @return    the corresponding two digits (first match)
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.13
     **/
    string const&
      Parser::number1(Card const& card)
      {
        return Parser::cards_reverse_map1[card];
      } // static string Parser::number1(Card card)

    /**
     ** -> result
     **
     ** @param     card   a card
     **
     ** @return    the corresponding two digits (second match)
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.13
     **/
    string const&
      Parser::number2(Card const& card)
      {
        return Parser::cards_reverse_map2[card];
      } // static string Parser::number2(Card card)

    /**
     ** -> result
     **
     ** @param     text        text with the cards
     **
     ** @return    the cards, given by the text
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.13
     **/
    vector<Card>
      Parser::cards(string text)
      {
        vector<Card> cards;
        while (text.size() >= 2) {
          cards.push_back(Parser::card(string(text, 0, 2)));
          text.erase(0, 3);
        } // while (!text.isempty())
        return cards;
      } // static vector<Card> Parser::cards(string text)

    /**
     ** initialize the static values
     **
     ** @param     -
     **
     ** @return    -
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.13
     **/
    void
      Parser::init_static()
      {
        if (Parser::static_initialized)
          return ;

        Parser::cards_map["00"] = Card::UNKNOWN;
        Parser::cards_map["01"] = Card::HEART_ACE;
        Parser::cards_map["02"] = Card::HEART_KING;
        Parser::cards_map["03"] = Card::HEART_QUEEN;
        Parser::cards_map["04"] = Card::HEART_JACK;
        Parser::cards_map["05"] = Card::HEART_TEN;
        Parser::cards_map["06"] = Card::HEART_NINE;
        Parser::cards_map["07"] = Card::DIAMOND_ACE;
        Parser::cards_map["08"] = Card::DIAMOND_KING;
        Parser::cards_map["09"] = Card::DIAMOND_QUEEN;
        Parser::cards_map["10"] = Card::DIAMOND_JACK;
        Parser::cards_map["11"] = Card::DIAMOND_TEN;
        Parser::cards_map["12"] = Card::DIAMOND_NINE;
        Parser::cards_map["13"] = Card::CLUB_ACE;
        Parser::cards_map["14"] = Card::CLUB_KING;
        Parser::cards_map["15"] = Card::CLUB_QUEEN;
        Parser::cards_map["16"] = Card::CLUB_JACK;
        Parser::cards_map["17"] = Card::CLUB_TEN;
        Parser::cards_map["18"] = Card::CLUB_NINE;
        Parser::cards_map["19"] = Card::SPADE_ACE;
        Parser::cards_map["20"] = Card::SPADE_KING;
        Parser::cards_map["21"] = Card::SPADE_QUEEN;
        Parser::cards_map["22"] = Card::SPADE_JACK;
        Parser::cards_map["23"] = Card::SPADE_TEN;
        Parser::cards_map["24"] = Card::SPADE_NINE;
        Parser::cards_map["25"] = Card::HEART_ACE;
        Parser::cards_map["26"] = Card::HEART_KING;
        Parser::cards_map["27"] = Card::HEART_QUEEN;
        Parser::cards_map["28"] = Card::HEART_JACK;
        Parser::cards_map["29"] = Card::HEART_TEN;
        Parser::cards_map["30"] = Card::HEART_NINE;
        Parser::cards_map["31"] = Card::DIAMOND_ACE;
        Parser::cards_map["32"] = Card::DIAMOND_KING;
        Parser::cards_map["33"] = Card::DIAMOND_QUEEN;
        Parser::cards_map["34"] = Card::DIAMOND_JACK;
        Parser::cards_map["35"] = Card::DIAMOND_TEN;
        Parser::cards_map["36"] = Card::DIAMOND_NINE;
        Parser::cards_map["37"] = Card::CLUB_ACE;
        Parser::cards_map["38"] = Card::CLUB_KING;
        Parser::cards_map["39"] = Card::CLUB_QUEEN;
        Parser::cards_map["40"] = Card::CLUB_JACK;
        Parser::cards_map["41"] = Card::CLUB_TEN;
        Parser::cards_map["42"] = Card::CLUB_NINE;
        Parser::cards_map["43"] = Card::SPADE_ACE;
        Parser::cards_map["44"] = Card::SPADE_KING;
        Parser::cards_map["45"] = Card::SPADE_QUEEN;
        Parser::cards_map["46"] = Card::SPADE_JACK;
        Parser::cards_map["47"] = Card::SPADE_TEN;
        Parser::cards_map["48"] = Card::SPADE_NINE;

        for (auto c : Parser::cards_map) {
          if (  (c.first[0] == '0')
              || (c.first[0] == '1')
              || ((c.first[0] == '2') && (c.first[1] == '0'))
              || ((c.first[0] == '2') && (c.first[1] == '1'))
              || ((c.first[0] == '2') && (c.first[1] == '2'))
              || ((c.first[0] == '2') && (c.first[1] == '3'))
              || ((c.first[0] == '2') && (c.first[1] == '4')) )
            Parser::cards_reverse_map1[c.second] = c.first;
          else 
            Parser::cards_reverse_map2[c.second] = c.first;
        } // for (auto c : Parser::cards_map)

        Parser::static_initialized = true;
        return ;
      } // static void Parser::init_static()

    /**
     ** Überprüft, ob die nächsten ausstehenden Befehle passen
     ** Beginnt das Schlüsselwort mit '?', ist es optional
     **
     ** @param     keys   Schlüsselworte zum testen
     **
     ** @return    0, wenn nicht alle Schlüsselworte passen, sonst Anzahl der passenden Schlüsselworte
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.13
     **/
    unsigned
      Parser::check_next_pending(std::initializer_list<std::string> keys) const
      {
        unsigned n = 0;
        auto p = this->pending.begin();
        for (auto k : keys) {
          bool const optional = ((k.size() > 1) && (k[0] == '?'));
          string const key = (optional ? string(k.begin() + 1, k.end()) : k);

          if (   (p == this->pending.end())
              || (key != p->first) ) {
            if (optional)
              continue;
            return 0;
          }

          ++p;
          ++n;
        } // for (auto& k : keys)

        return n;
      } // unsigned Parser::check_next_pending(initializer_list<std::string> keys) const

    /**
     ** -> Rückgabe
     **
     ** @param     -
     **
     ** @return    der nächste Befehl
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.13
     **/
    string
      Parser::next_keyword() const
      {
        if (this->pending.empty())
          return "";
        return this->pending.front().first;
      } // string Parser::next_keyword() const

    /**
     ** -> Rückgabe
     **
     ** @param     -
     **
     ** @return    der Eintrag zum nächsten Befehl
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.13
     **/
    string
      Parser::next_entry() const
      {
        if (this->pending.empty())
          return "";
        return this->pending.front().second;
      } // string Parser::next_entry() const

    /**
     ** Entfernt den nächsten ausstehenden Befehl
     **
     ** @param     n   Anzahl der ausstehenden Befehle zum Entfernen
     **
     ** @return    -
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.13
     **/
    void
      Parser::pop_next_pending(unsigned const n)
      {
        if (this->pending.empty())
          return ;
        cout << "<< " << this->pending.front().first << ": " << this->pending.front().second << endl;
        this->pending.pop_front();
        if (n > 1)
          this->pop_next_pending(n-1);
        return ;
      } // void Parser::pop_next_pending(unsigned const n = 1)

    /**
     ** Gibt die ausstehenden Befehle aus stdout aus
     **
     ** @param     -
     **
     ** @return    -
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.13
     **/
    void
      Parser::list_pending() const
      {
        cout << "pending: " << this->pending.size() << "\n";
        int i = 0;
        for (auto p : this->pending) {
          cout << i << ": " << p.first << ": " << p.second << endl;
          if (i >= 10) {
            cout << "...\n";
            break;
          }
          ++i;
        } // for (p)
        return ;
      } // void Parser::list_pending() const

  } // namespace DokoLounge
} // namespace Network

#endif // #ifdef USE_NETWORK_DOKOLOUNGE
#endif // #ifdef USE_NETWORK
