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
#ifdef DKNOF
#include "../../../player/ai/ai.h"
#endif

#include "../../../misc/lounge.h"
#include "../../../utils/string.h"

#include <cstring>


namespace Network {
  namespace DokoLounge {

    /**
     ** check the pending messages for gameplay
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
      Parser::check_pending_gameplay()
      {
#if 0
        COUT << endl;
        CLOG << ::lounge->location() << "  " << ::name(::game_status) << "  " << this->blocking() << endl;
        this->list_pending();
#endif
        if (this->actor().pending_action()) {
          return ;
        }

#ifdef CHECK_RUNTIME
      //::runtime["DokoLounge::check_pending_gameplay"].start();
#endif

        dynamic_cast<Network::DokoLounge::Interpreter::Receiver&>(this->interpreter->receiver()).get_next_command();

#ifdef DKNOF
        if (this->pending.size() >= 1000) {
          CLOG << this->game() << endl;
          this->list_pending();
          SEGFAULT;
        }
#endif

        if (this->pending.empty())
          return ;

        pair<string, string> const& command = this->pending.front();
        string const& keyword = command.first;
        string const& text = command.second;
#if 0
        CLOG << keyword << endl;
        CLOG << text << endl;
#endif

        { // TischChat -- wird erst einmal ignoriert
          if (keyword == "TischChatSystem") {
            if (   (text.find(" hat gegeben") != string::npos)
                || (text.find(" ist gesund.") != string::npos)
                || (text.find(" hat einen Vorbehalt.") != string::npos)
                || (text.find(" spielt:Hochzeit") != string::npos)
                || (text.find(" spielt mit der Hochzeit.") != string::npos)
                || (text.find(" spielt:Armut") != string::npos)
                || (text.find(" spielt:Buben-Solo") != string::npos)
                || (text.find(" spielt:Damen-Solo") != string::npos)
                || (text.find(" spielt:Fleischlos-Solo") != string::npos)
                || (text.find(" spielt:Kreuz-Solo") != string::npos)
                || (text.find(" spielt:Pik-Solo") != string::npos)
                || (text.find(" spielt:Herz-Solo") != string::npos)
                || (text.find(" spielt:Karo-Solo") != string::npos)
                || (text == "Niemand möchte ein Solo spielen.")
                || (text.find(" kommt raus.") != string::npos)
                || (   (text.find(" das Spiel mit") != string::npos)
                    && (text.find(" Augen gewonnen.") != string::npos) )
                || (   (text.find(" das Spiel mit") != string::npos)
                    && (text.find(" Augen verloren.") != string::npos) )
                || (text.find("Neue Spielerliste wurde erstellt. Es spielen an diesem Tisch: ") == 0)
                || (text.find(" hat den Tisch verlassen") != string::npos)
                ) {
                  // ignore message
                  this->pop_next_pending();
                  return ;
                } else {
                  CLOG << "Unknown TischChatSystem: " << text << endl;
                  SEGFAULT;
                }
          } // if (this->pending.count("TischChatSystem"))
        } // TischChat

        { // Ignorieren
          if ( // Information wird nicht benötigt
              (keyword == "InfoWClose")
              || (keyword == "sonderregeln") // ist leer
              || (keyword == "Hochzeit") // Steht schon in <<Meldung>> 
              || (keyword == "Hochzeitspartner") // Ergibt sich durch den Spielverlauf
              || (keyword == "ShowKreuzDame") // Ergibt sich durch den Spielverlauf
              || (keyword == "Meldungsfenster") // Vorbehalt anzeigen
             ) {
            this->pop_next_pending();
            return ;
          }

          if ( // ToDo
              (keyword == "AnsageCount")
              || (keyword == "EndabrechnungsInfo") // Mit GameSummary abgleichen
              || (keyword == "NeuePunkte") // Selber berechnete Turnierpunkte überschreiben
              || (keyword == "sticheprospieler") // Für das Springen in ein Spiel
              || (keyword == "NeuePunkte") // Turnierbeginn
             ) {
            this->pop_next_pending();
            return ;
          }
        } // Ignorieren

        //
        // Nun werden die verschiedenen Übermittlungen geprüft.
        //

        if (keyword == "alleSpielerAmTisch") {
          if (this->alleSpielerAmTisch(text)) {
            this->pop_next_pending();
            return ;
          }
        }
        if (keyword == "zuschauer") {
          if (this->zuschauer(text)) {
            this->pop_next_pending();
            return ;
          }
        }

        // * Turnier starten
        // * Vorbehalt
        // * Spiel gestartet
        // * Spiel neu geben
        //   CleanTisch, Karten, Meldungsfenster, SpielerIstDran, AnsageCount, TischChat
        // * Refresh
        //   Karten, KartenAufTisch, ersteKarte, trumpf, SpielerIstDran, Sau?, SuperSau?, SoloMeldung?, NeuePunkte?, Partei, AnsageCount, Hochzeit?, Hochzeitspartner?, sonderregeln?, ArmutIsons?, alleSpielerAmTisch, NeuePunkte
        // * Karte wurde gespielt
        //   Karten, KartenAufTisch, ersteKarte, SpielerIstDran
        // * Ansage getätigt
        // * Stich genommen
        // * Spiel beendet

        // Erst einmal "alle Spieler am Tisch" interpretieren, da dort alle Mitspieler aufgeführt sind.
        if (   (::game_status >= GAMESTATUS::GAME_PLAY)
            && !this->pending_gameplay_actions().empty()) {
          // Wenn in ein Spiel hineingesprungen wird, wird eine Liste von Spielaktionen (Karten ausspielen, Stiche nehmen) erstellt. Diese muss erst einmal abgearbeitet werden.
          this->forward_in_game();
        } else {
          unsigned const p = this->pending.size();
#define TRY(X)  if (p == this->pending.size()) this->try_##X()
          TRY(start_party);
          TRY(init_game);
          TRY(play_card);
          TRY(trick_taken);
          //this->try_start_party();
          //this->try_init_game();
          //this->try_trick_taken();
          //this->try_play_card();
        }

#if 0

        { // Turnier starten
          if (::game_status <= GAMESTATUS::PARTY_NEW) {
            // Um das Turnier zu starten müssen Spieler und Regeln bekannt sein.
            if (   (::lounge->location() != Lounge::LOUNGE)
                && this->pending.count("SpielerIstDran") // Startspieler
                && (   (::lounge->table().type == "DDV")
                    || (::lounge->table().type == "Lerntisch")
                    || true
                    || this->pending.count("regelinfo1") ) ) { // Regeln
              if (::lounge->table().type == "DDV") {
                this->actor().party().rule().set_to(Rule::DOKOLOUNGE_DDV);
              } else if (::lounge->table().type == "Lerntisch") {
                this->actor().party().rule().set_to(Rule::DOKOLOUNGE_LERNTISCH);
              } else {
                this->actor().party().rule().set_to(Rule::DOKOLOUNGE_SONDERREGELN);
                if (this->pending.count("regelinfo"))
                  if (this->regelinfo1(this->pending["regelinfo1"]))
                    this->pending.erase("regelinfo1");
              }
              // "SpielerIstDran" wird weiter unten berücksichtigt
              this->actor().party_start();
            } // if (Alle Informationen für ein Turnier sind da
          } // if (::game_status <= GAMESTATUS::PARTY_NEW)
        } // Turnier starten

        if (::game_status & GAMESTATUS::GAME) {
          if (   (::game_status < GAMESTATUS::GAME_FINISHED)
              && (::game_status > GAMESTATUS::GAME_INIT) ) {
            if (   this->pending.count("TischErlaubt")
                || this->pending.count("regelinfo1") ) { // rules
              CLOG << "TischErlaubt: " << this->pending["TischErlaubt"] << endl;
              CLOG << "regelinfo1: " << this->pending["regelinfo1"] << endl;
              CLOG << ::name(::game_status) << endl;
              exit(0);
            }
          }
          if (   (::game_status >= GAMESTATUS::GAME_PLAY)
              && !this->pending_gameplay_actions().empty()) {
            // Wenn in ein Spiel hineingesprungen wird, wird eine Liste von Spielaktionen (Karten ausspielen, Stiche nehmen) erstellt. Diese muss erst einmal abgearbeitet werden.
            this->forward_in_game();
          } else {

            if (::game_status == GAMESTATUS::GAME_INIT) {
              // Aktualisiere die Karten
              if (   !this->pending.empty()
                  //&& (this->pending.front().first == "Karten")
                  && this->pending.count("Karten")
                  && this->pending.count("KartenAufTisch")
                  && (this->pending["Karten"] == "00-00-00-00-00-00-00-00-00-00-00-00-")
                  && (this->pending["KartenAufTisch"] == "00-00-00-00-") ) {
                this->pending.erase("Karten");
                this->pending.erase("trumpf");
                this->pending.erase("AnsageCount");
              }
              // Aktualisiere den Startspieler
              if (   !this->pending.empty()
                  && (this->pending.front().first == "SpielerIstDran")
                  && this->pending.count("SpielerIstDran") ) {
                this->pending.erase("SpielerIstDran");
              }
              // Zuerst muss der Startspieler gesetzt und die Karten verteilt werden .
              // Außerdem muss geprüft werden, ob in ein laufendes Spiel hineingesprungen wird.
              if (   this->pending.count("SpielerIstDran")
                  && this->pending.count("Karten")
                  && this->pending.count("ersteKarte")
                  && this->pending.count("KartenAufTisch")
                  && !this->actor().game_players().empty()
                  //&& this->pending.count("AnsageCount")
                  //&& this->pending.count("TischChatSystem")
                 ) {
                // ToDo: den letzten Stich anschauen und mit bei den Informationen berücksichtigen
                if (this->pending.count("regelinfo1"))
                  if (this->regelinfo1(this->pending["regelinfo1"]))
                    this->pending.erase("regelinfo1");
                if (this->init_game()) {
                  this->pending.erase("SpielerIstDran");
                  this->pending.erase("Karten");
                  this->pending.erase("ersteKarte");
                  this->pending.erase("KartenAufTisch");
                  this->pending.erase("AnsageCount");
                  this->pending.erase("TischChatSystem");
                }
                this->pending.erase("trumpf");
              }
              // ToDo: Schule
            } // if (::game_status == GAMESTATUS::GAME_INIT)

            { // Vorbehalte
              if (::game_status == GAMESTATUS::GAME_RESERVATION) {
                // Der Vorbehalt geht solange, bis alle Personen gemeldet haben und anschließend die Karten verteilt werden. 
                // Eventuell Vorbehalt merken, so dass auf die Information gewartet werden kann.
                // Armut?
                if (this->pending.count("TischChatSystem")) {
                  string const& text = this->pending["TischChatSystem"];
                  if (   (text.find(" kommt raus.") != string::npos)
                     ) {
                    string const name = string(text, 0, text.find(" kommt raus."));

                    // ToDo: set the player number
                    //this->actor().game_set_startplayer(static_cast<unsigned>(::lounge->player_location(name)));
                    //SEGFAULT;

                    this->pending.erase("TischChatSystem");
                  }
                }
                // Hole die Vorbehalte
                if (this->pending.count("Meldung")) {
                  if (this->Meldung(this->pending["Meldung"]))
                    this->pending.erase("Meldung");
                }
                if (   this->pending.count("trumpf")
                    && this->pending.count("Karten")) {
                  if (this->pending.count("ArmutIsons"))
                    if (this->ArmutIsons(this->pending["ArmutIsons"]))
                      this->pending.erase("ArmutIsons");
                  if (this->trumpf(this->pending["trumpf"])) {
                    this->pending.erase("trumpf");
                    // Alle Informationen außer die Schweine sind bekannt,
                    // außerdem ist der Spieltyp entschieden.
                    this->list_pending();
                    //this->pending.clear();
                  }
                }
              } // if (::game_status == GAMESTATUS::GAME_RESERVATION)
            } // Vorbehalte

            if (::game_status == GAMESTATUS::GAME_PLAY) {
              if (this->actor().pending_action()) {
                CLOG << *this->actor().pending_action() << endl;
                SEGFAULT;
              }
              if (this->actor().game().played_cards_no() == 0) {
                // Kann bei mehrfacher Meldung auftreten
                if (this->pending.count("Meldung"))
                  this->pending.erase("Meldung");
                if (this->pending.count("trumpf"))
                  this->pending.erase("trumpf");
                if (this->pending.count("Sau"))
                  if (this->Sau(this->pending["Sau"]))
                    this->pending.erase("Sau");
                if (this->pending.count("Supersau"))
                  if (this->Supersau(this->pending["Supersau"]))
                    this->pending.erase("Supersau");
              }
              // Karten und Ansagen
              if (this->pending.count("Karten")) {
                // Wird nach dem Stich ('KartenAufTisch') betrachtet
              }
              if (this->pending.count("KartenAufTisch")) {
                this->KartenAufTisch(this->pending["KartenAufTisch"]);
                this->pending.erase("KartenAufTisch");
                if (this->pending.count("Karten")) {
                  this->Karten(this->pending["Karten"]);
                  this->pending.erase("Karten");
                }
              }
              if (this->pending.count("Ansage")) {
                this->Ansage(this->pending["Ansage"]);
                this->pending.erase("Ansage");
              }
              if (this->pending.count("ersteKarte")) {
                // Ignorieren
                this->pending.erase("ersteKarte");
              }
              if (this->pending.count("SpielerIstDran")) {
                //CLOG << this->pending["SpielerIstDran"] << endl;
                // Ignorieren
                this->pending.erase("SpielerIstDran");
              }
            } // if (::game_status == GAMESTATUS::GAME_PLAY)

            if (::game_status == GAMESTATUS::GAME_FULL_TRICK) {
              if (this->pending.count("StichVomTisch")) {
                this->StichVomTisch(this->pending["StichVomTisch"]);
                this->pending.erase("StichVomTisch");
              }
              if (this->pending.count("Ansage")) {
                this->Ansage(this->pending["Ansage"]);
                this->pending.erase("Ansage");
              }
              if (this->pending.count("ersteKarte")) {
                // Ignorieren
                this->pending.erase("ersteKarte");
              }
              if (this->pending.count("SpielerIstDran")) {
                // Ignorieren
                this->pending.erase("SpielerIstDran");
              }
            } // if (::game_status == GAMESTATUS::GAME_FULL_TRICK)

            if (::game_status == GAMESTATUS::GAME_FINISHED) {
              if (this->pending.count("SpielerIstDran")) {
                // Ignorieren
                this->pending.erase("SpielerIstDran");
              }
              if (this->pending.count("sticheprospieler")) {
                // Ignorieren
                this->pending.erase("sticheprospieler");
              }
              if (this->pending.count("EndabrechnungsInfo")) {
                this->EndabrechnungsInfo(this->pending["EndabrechnungsInfo"]);
                this->pending.erase("EndabrechnungsInfo");
              }
              if (this->pending.count("NeuePunkte")) {
                // ToDo: Aktualisiere die Turnierpunkte
                this->pending.erase("NeuePunkte");
              }
#ifdef POSTPONED
              // Lasse die Spielabrechnung offen, bis der Spieler sie selber schließt.
              // In der DokoLounge wird sie automatisch geschlossen, wenn ein Spieler die nächsten Karten gibt.
              if (this->pending.count("Karten")) {
                // new cards, so the game is closed
                this->list_pending();
                this->actor().game_close();
              }
#endif
            } // if (::game_status == GAMESTATUS::GAME_FINISHED)
          } // if !(forward in game)
        } // if (::game_status & GAMESTATUS::GAME)
#endif
#ifdef CHECK_RUNTIME
        //::runtime["DokoLounge::check_pending_gameplay"].stop();
#endif
        return ;
      } // void Parser::check_pending_gameplay()
    /**
     ** the game is opened
     **
     ** @param     game   opened game
     **
     ** @return    -
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.13
     **/
    void
      Parser::game_open(Game const& game)
      {
        this->reservation_open = false;
        this->pending_reservation = vector<Reservation>(4, Reservation());
        this->pending_gameplay_actions_.clear();
        if (::lounge->location() == Lounge::OBSERVER)
          this->sender().refresh_all_players();
        else
          this->sender().refresh();
        return ;
      } // void Parser::game_open(Game const& game)

    /**
     ** Starte das Turnier
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
      Parser::try_start_party()
      {
        if (!(this->check_next_pending({"Karten",
                                       "KartenAufTisch",
                                       "ersteKarte",
                                       "SpielerIstDran"})
              && (::game_status < GAMESTATUS::GAME)) 
           ) {
          return;
        }

        if (::lounge->table().type == "DDV") {
          this->actor().party().rule().set_to(Rule::DOKOLOUNGE_DDV);
        } else if (::lounge->table().type == "Lerntisch") {
          this->actor().party().rule().set_to(Rule::DOKOLOUNGE_LERNTISCH);
        } else {
          this->actor().party().rule().set_to(Rule::DOKOLOUNGE_SONDERREGELN);
        }
        this->actor().party_start();
        return;
      } // void Parser::try_start_party()

    /**
     ** Initializiere das Spiel
     ** Setze den Startspiele und verteile die Karten.
     ** Prüfe außerdem, ob in ein Spiel hineingesprungen wurde.
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
      Parser::try_init_game()
      {
        if (!(this->check_next_pending({"Karten",
                                       "KartenAufTisch",
                                       "ersteKarte",
                                       "SpielerIstDran"})
              && (::game_status == GAMESTATUS::GAME_INIT))
           ) {
          return ;
        }

        CLOG << ((::lounge->location() == Lounge::OBSERVER)
                 ? "observer: observing " + ::lounge->observed_player()
                 : "player: " + ::lounge->account())
          << endl;
        this->player_offset_ = ::lounge->position_of_player((::lounge->location() == Lounge::OBSERVER)
                                                            ? ::lounge->observed_player()
                                                            : ::lounge->account());

#if 0
        if (this->pending.count("TischChatSystem")) {
          CLOG << this->pending["TischChatSystem"] << endl;
          if (this->pending["TischChatSystem"]
              != this->pending["SpielerIstDran"] + " kommt raus.") {
            SEGFAULT;
            return ;
          }
        }
#endif

        // ToDo: Setze die Spieler
        vector<Player*> players;
        // zeige die Spieler des Turniers
        CLOG << endl; this->party().write_players_short(COUT);
        for (unsigned p = 0; p < this->game().number_of_players(); ++p) {
          if (::lounge->table().players[PLAYERNO_FROM_FREEDOKO(p) - 1] == "")
            return ;
          CLOG << "players.push_back(" << ::lounge->table().players[PLAYERNO_FROM_FREEDOKO(p) - 1] << ")" << endl;
          string const& name = ::lounge->table().players[PLAYERNO_FROM_FREEDOKO(p) - 1];
          Player* const player = ::party.player_by_name(name);
          DEBUG_ASSERTION(player != nullptr,
                          "Table::init_game()\n"
                          "  player '" << name << "' not found in players\n");
          players.push_back(player);
        }
        this->actor().game_set_players(players);

        string keyword;
        string entry;
        // Blatt
        EXPECT_PENDING("Karten");
        Hand hand(Parser::cards(entry));
        this->pop_next_pending(); // "Karten"
        Hand unknown(vector<Card>(hand.cardsnumber_all(), Card::UNKNOWN));

        vector<Hand*> hands(this->rule()(Rule::NUMBER_OF_PLAYERS_IN_GAME),
                            &unknown);
        hands[0] = &hand;

        EXPECT_PENDING("KartenAufTisch");
        if (   (hand.numberof(Card::UNKNOWN) == 0)
            && (entry == "00-00-00-00-")) {
          // Einfacher Fall (Spielanfang): Es wurden noch keine Karten ausgespielt.

          this->pop_next_pending(); // "KartenAufTisch"
          EXPECT_PENDING("ersteKarte");
          this->pop_next_pending();
          EXPECT_PENDING("SpielerIstDran");
          this->actor().game_set_startplayer(this->playerno(entry));
          this->pop_next_pending(); // "SpielerIstDran"
          this->actor().game_distribute_cards(hands);

          return ;
        } // if (at start of game)

        // Sprung mitten ins Spiel: es wurden schon einige Karten ausgespielt.
        // Das Spiel wird mit leeren Karten nachgespielt.

        this->pending_gameplay_actions_.clear();

        // Stich
        vector<Card> const cards = Parser::cards(entry);
        this->pop_next_pending(); // "KartenAufTisch"
        Card const& card_of_human = cards[PLAYERNO_FROM_FREEDOKO(0) - 1];

        if (   (hand.numberof(Card::UNKNOWN) == hand.cardsnumber())
            && (card_of_human.is_unknown())) {
          // Leeres Blatt: bisher sind noch keine Karten verteilt.
          return ;
        }

        // Suche den Startspieler
        unsigned startplayer = UINT_MAX;
        EXPECT_PENDING("ersteKarte");
        Card const first_card = this->card(entry);
        this->pop_next_pending(); // "ersteKarte"

        EXPECT_PENDING("SpielerIstDran");
        startplayer = static_cast<unsigned>(::lounge->position_of_player(entry));
        this->pop_next_pending(); // "SpielerIstDran"
        if (   (cards[0] == first_card)
            && (cards[cards.size() - 1].is_unknown()))
          startplayer = 1;
        else
          for (unsigned c = 0; c + 1 < cards.size(); ++c)
            if (   cards[c].is_unknown()
                && (cards[c+1] == first_card))
              startplayer = c + 2;

        // Spiele alle bisherigen Stiche mit unbekannten Karten
        if (hand.numberof(Card::UNKNOWN) > 0) {
          for (unsigned t = 0;
               t < (  card_of_human.is_unknown()
                    ? hand.numberof(Card::UNKNOWN)
                    : hand.numberof(Card::UNKNOWN) - 1);
               ++t) {
            for (unsigned p = 0; p < this->game().number_of_players(); ++p) {
              this->pending_gameplay_actions_.push_back(new ::GameplayAction::CardPlayed( (p + PLAYERNO_TO_FREEDOKO(startplayer)) % this->game().number_of_players(), Card::UNKNOWN));
            } // for (p)
            this->pending_gameplay_actions_.push_back(new ::GameplayAction::TrickTaken());
          } // for (t)

          // Füge den letzten Stich hinzu
          if (!card_of_human.is_unknown())
            hand.set_unknown(card_of_human);
          for (unsigned p = 0;
               (   (p < this->game().number_of_players())
                && !cards[(p + startplayer - 1) % this->game().number_of_players()].is_unknown());
               ++p) {
            this->pending_gameplay_actions_.push_back(new ::GameplayAction::CardPlayed( (p + PLAYERNO_TO_FREEDOKO(startplayer)) % this->game().number_of_players(), cards[(p + startplayer - 1) % this->game().number_of_players()]));
            CLOG << p << cards[(p + startplayer - 1) % this->game().number_of_players()] << endl;
          } // for (p)
        } // if (hand.numberof(Card::UNKNOWN) > 0)

        // Setze die Parameter des Spiels
        this->actor().game_set_startplayer(PLAYERNO_TO_FREEDOKO(startplayer));
        for (unsigned p = 0; p < this->game().number_of_players(); ++p) {
          CLOG << p << ": ";
          this->game().player(p).write_short(COUT);
          COUT << endl;
          //dynamic_cast<Ai const&>(this->game().player(p)).cards_information().write(COUT);
          CLOG << &this->game().player(p) << endl;
        }
        this->actor().game_distribute_cards(hands);
#if 1
        // Setze die Vorbehalte
        // ToDo: Solo, Armut, ...
        CLOG << "set reservations" << endl;
        for (unsigned p = 0; p < this->game().number_of_players(); ++p)
          this->actor().gameplay_action(::GameplayAction::Reservation(p, this->pending_reservation[p]));
#endif

#ifdef DKNOF
#if 0
        for (auto a : this->pending_gameplay_actions())
          CLOG << *a << endl;
        CLOG << hand;
#endif
#endif

        CLOG << "player offset: " << this->player_offset() << endl;
        CLOG << "0: " << this->game().player(0).name() << endl;
        CLOG << "0: " << ::lounge->table(0).players[0] << endl;
        CLOG << "observed: " << ::lounge->observed_player() << endl;
        DEBUG_ASSERTION((this->game().player(0).name() ==
                         ((::lounge->location() == Lounge::OBSERVER)
                          ? ::lounge->observed_player()
                          : ::lounge->account())),
                        "Parser::init_game()\n"
                        "  Player at position 0 = " << this->game().player(0).name() << " is not the (observed) player "
                        << ((::lounge->location() == Lounge::OBSERVER)
                            ? ::lounge->observed_player()
                            : ::lounge->account())
                        << ".");

        return ;
      } // void Parser::try_init_game()

    /**
     ** forward in the game
     **
     ** @param     game   opened game
     **
     ** @return    whether an action was emitted
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.13
     **/
    void
      Parser::forward_in_game()
      {
        if (this->actor().pending_action())
          return ;
        // Vorbehalte: bereits erledigt
        if (this->pending_gameplay_actions().empty())
          return ;

        this->actor().gameplay_action(*this->pending_gameplay_actions().front());
        delete this->pending_gameplay_actions_.front();
        this->pending_gameplay_actions_.pop_front();
        return ;
      } // void Parser::forward_in_game()

    /**
     ** Eine Karte wurde gespielt
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
      Parser::try_play_card()
      {
        if (!(this->check_next_pending({"?Karten",
                                       "KartenAufTisch",
                                       "ersteKarte",
                                       "SpielerIstDran"})
              && (::game_status >= GAMESTATUS::GAME_PLAY)
              && (::game_status <= GAMESTATUS::GAME_TRICK_TAKEN)
             ) )
          return ;

        string keyword;
        string entry;
        Hand const hand = ((this->next_keyword() == "Karten")
                           ? Parser::cards(entry)
                           : Hand());
        if (this->next_keyword() == "Karten") {
          this->pop_next_pending(); // "Karten"
        }

        EXPECT_PENDING("KartenAufTisch");
        vector<Card> const cards = Parser::cards(entry);
        this->pop_next_pending(); // "Karten"

        EXPECT_PENDING("ersteKarte");
        Card const startcard = Parser::card(entry);
        (void) startcard;
        this->pop_next_pending(); // "Karten"

        EXPECT_PENDING("SpielerIstDran");
        unsigned const nextplayer = this->playerno(entry);
        (void) nextplayer;
        this->pop_next_pending(); // "Karten"

        // Teste, ob eine weitere Karte gespielt wurde
        if (std::count(cards.begin(), cards.end(), Card::UNKNOWN)
            == this->game().trick_current().remainingcardno())
          return ;

        DEBUG_ASSERTION(std::count(cards.begin(), cards.end(), Card::UNKNOWN) + 1
                        == this->game().trick_current().remainingcardno(),
                        "Parser::play_card()\n"
                        "   current trick does not match:\n"
                        << this->game().trick_current());

        unsigned const playerno = this->game().trick_current().actplayerno();
        Card const& card = cards[PLAYERNO_FROM_FREEDOKO(playerno) - 1];
        DEBUG_ASSERTION(card != Card::UNKNOWN,
                        "Parser::play_card()\n"
                        "   got empty card for current player "
                        << this->game().player(PLAYERNO_FROM_FREEDOKO(playerno)).name() << " = "
                        << PLAYERNO_FROM_FREEDOKO(playerno) << "(= DokoLounge), "
                        << playerno << "(= FreeDoko)\n"
                        << "  player offset = " << this->player_offset() << '\n'
                        << "  player 0 = " << this->game().player(PLAYERNO_TO_FREEDOKO(1)).name());
        this->actor().gameplay_action(::GameplayAction::CardPlayed(playerno, card));

        // todo: Prüfe, ob die Werte passen
        (void) hand;
        (void) startcard;
        (void) nextplayer;
        return ;
      } // void Parser::try_play_card()

    /**
     ** a gameplay action has happened
     **
     ** @param     action   gameplay action
     **
     ** @return    -
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.13
     **/
    void
      Parser::gameplay_action(GameplayAction const& action)
      {
        this->actor().check_gameplay_action(action);
        return ;
      } // void Parser::gameplay_action(GameplayAction action)

    /**
     ** the game is closed
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
      Parser::game_close()
      {
        this->OS::game_close();
        return ;
      } // void Parser::game_close()


    /**
     ** (global) points of the player
     **
     ** @param     text        points of the players on the table
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
      Parser::NeuePunkte(string text)
      {
        // <<Diether>>6<</Diether>><<frei>>0<</frei>><<frei>>0<</frei>><<frei>>0<</frei>>
        return true;
        // get the player names
        string entry, keyword;
        vector<string> player;
        //this->player_offset_ = UINT_MAX;
        unsigned p = 0;
        while (!text.empty()) {
          int points;
          this->strip_next_part(text, keyword, entry);
          player.push_back(keyword);
#if 0
          if (keyword == ((::lounge->location() == Lounge::OBSERVER)
                          ? ::lounge->observed_player()
                          : ::lounge->account()) )
            this->player_offset_ = p;
#endif
          DK::Utils::String::from_string(entry, points);
          p += 1;
        }
#if 0
        DEBUG_ASSERTION(this->player_offset() != UINT_MAX,
                        "Parser::NeuePunkte(text)\n"
                        "  player not found");
#endif
        for (unsigned p = 0; p < this->party().playerno(); ++p)
          this->actor().set_player_name(this->party().player((p + this->player_offset()) % this->party().playerno()),
                                        player[p]);
        return true;
      } // bool Parser::NeuePunkte(string text)

    /**
     ** rules
     **
     ** @param     text        rules
     **
     ** @return    whether the command was accepted
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.13
     **/
    bool
      Parser::regelinfo1(string text)
      {
        // <<tischinfo>>Sonderregeln<</tischinfo>><<ohneSupersau>>1<</ohneSupersau>><<ersted>>1<</ersted>><<skir>>1<</skir>><<armut>>1<</armut>><<oNeunen>>0<</oNeunen>>
        string keyword, entry;
        EXPECT_KEYWORD("tischinfo");
        if (entry == "Sonderregeln") {
          Rule& rule = this->actor().party().rule();
          rule.set_to(Rule::DOKOLOUNGE_SONDERREGELN);
          // Armut (mit Schieben, 3 Karten)
          // Schweine (immer an)
          // Hyperschweine
          // Zweite Herz Zehn immer über die erste / nicht im letzten Stich
          // Solospieler beginnt immer
          // Mit/ohne Neunen
          EXPECT_KEYWORD("ohneSupersau");
          rule.set(Rule::HYPERSWINES, (entry == "1"));
          EXPECT_KEYWORD("ersted");
          rule.set(Rule::DOLLEN_SECOND_OVER_FIRST, (entry != "1"));
          EXPECT_KEYWORD("skir");
          //rule.set(Rule::DOLLEN_SECOND_OVER_FIRST, (entry == "1"));
          EXPECT_KEYWORD("armut");
          rule.set(Rule::POVERTY, (entry == "1"));
          EXPECT_KEYWORD("oNeunen");
          rule.set(Rule::WITH_NINES, (entry != "1"));
          EXPECT_EMPTY_TEXT;
        } else {
          CLOG << entry << endl;
          CLOG << text << endl;
          //exit(0);
        } // if (entry == "Sonderregeln")
        return true;
      } // bool Parser::regelinfo1(string text)

    /**
     ** rules
     **
     ** @param     text        trick
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
      Parser::sonderregeln(string text)
      {
        CLOG << text << endl;
        exit(0);
        return false;
      } // bool Parser::sonderregeln(string text)

    /**
     ** reservation
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
      Parser::Meldung(string text)
      {
        CLOG << text << endl;
        if (text.find(" ist gesund") != string::npos) {
          // Standard-Vorbehalt, nichts zu tun.
        } else if (   (text.find("Niemand m") == 0)
                   && (text.find("chte ein Solo spielen") != string::npos) ) {
          // Niemand möchte ein Solo spielen.
          // Hochzeit und Armut werden später angesagt.
        } else if (text.find(" hat einen Vorbehalt") != string::npos) {
          // Irgend ein Vorbehalt (Solo, Hochzeit, Armut)
          string const player = string(text, 0, text.find(" hat einen Vorbehalt"));
          this->reservation_open = true;
        } else if (text.find(" spielt:") != string::npos) {
          string const player = string(text, 0, text.find(" spielt:"));
          string const gametype_str = string(text,
                                             text.find(" spielt:")
                                             + string(" spielt:").size());
          // ToDo: Armut

          GameType& gametype
            = this->pending_reservation[this->playerno(player)].game_type;
          if (gametype_str == "Karo-Solo")
            gametype = GAMETYPE::SOLO_DIAMOND;
          else if (gametype_str == "Hochzeit") {
            gametype = GAMETYPE::MARRIAGE;
            this->pending_reservation[this->playerno(player)].marriage_selector
              = MARRIAGE_SELECTOR::FIRST_FOREIGN;
            this->actor().set_known_card(this->playerno(player),
                                         Card::CLUB_QUEEN, 2);
          } else if (gametype_str == "Herz-Solo")
            gametype = GAMETYPE::SOLO_HEART;
          else if (gametype_str == "Pik-Solo")
            gametype = GAMETYPE::SOLO_SPADE;
          else if (gametype_str == "Kreuz-Solo")
            gametype = GAMETYPE::SOLO_CLUB;
          else if (gametype_str == "Buben-Solo")
            gametype = GAMETYPE::SOLO_JACK;
          else if (gametype_str == "Damen-Solo")
            gametype = GAMETYPE::SOLO_QUEEN;
          else if (gametype_str == "Fleischlos-Solo")
            gametype = GAMETYPE::SOLO_MEATLESS;
          else {
            CLOG << "Unknown reservation: " << gametype_str << endl;
            SEGFAULT;
          }
          this->reservation_open = false;
          CLOG << ::name(gametype) << endl;
        }
        return true;
      } // bool Parser::Meldung(string text)

    /**
     ** the trump color is given
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
      Parser::trumpf(string text)
      {
        if (this->reservation_open) {
          return false;
        }

        // announce the reservations
        for (unsigned p = 0; p < this->game().number_of_players(); ++p) {
          this->actor().gameplay_action(::GameplayAction::Reservation(p, this->pending_reservation[p]));
          CLOG << p << ": " << this->pending_reservation[p] << endl;
        }
        return true;
      } // bool Parser::trumpf(string text)

    /**
     ** announcement ?
     **
     ** @param     text        trick
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
      Parser::AnsageCount(string text)
      {
        CLOG << text << endl;
        exit(0);
        return false;
      } // bool Parser::AnsageCount(string text)

    /**
     ** announcement
     **
     ** @param     text        player, team and announcement
     **
     ** @return    whether the command was accepted
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.13
     **/
    bool
      Parser::Ansage(string text)
      {
        DEBUG_ASSERTION(text.find(": ") != string::npos,
                        "Parser::Ansage(" << text << ")\n"
                        "  format unknown");
        string const player = string(text, 0, text.find(": "));
        text.erase(0, player.size() + 2);

        DEBUG_ASSERTION(text.find(" ") != string::npos,
                        "Parser::Ansage(" << text << ")\n"
                        "  format unknown");
        string const team_str = string(text, 0, text.find(" "));
        Team team = TEAM::UNKNOWN;
        if (team_str == "RE")
          team = TEAM::RE;
        else if (team_str == "KONTRA")
          team = TEAM::CONTRA;
        DEBUG_ASSERTION(TEAM::is_real(team),
                        "Parser::Ansage(" << text << ")\n"
                        "  unknown team");
        text.erase(0, team_str.size() + 1);

        Announcement announcement = ANNOUNCEMENT::NOANNOUNCEMENT;
        if (text == "keine 120")
          announcement = ANNOUNCEMENT::NO120;
        else if (text == "keine 90")
          announcement = ANNOUNCEMENT::NO90;
        else if (text == "keine 60")
          announcement = ANNOUNCEMENT::NO60;
        else if (text == "keine 30")
          announcement = ANNOUNCEMENT::NO30;
        else if (text == "SCHWARZ")
          announcement = ANNOUNCEMENT::NO0;

        DEBUG_ASSERTION(announcement != ANNOUNCEMENT::NOANNOUNCEMENT,
                        "Parser::Ansage(" << text << ")\n"
                        "  unknown announcement");

        this->actor().set_team(this->playerno(player), team);
        if (   (this->game().type() == GAMETYPE::NORMAL)
            && (team == TEAM::RE))
          this->actor().set_known_card(this->playerno(player), Card::CLUB_QUEEN);
        this->actor().gameplay_action(GameplayAction::Announcement(this->playerno(player), announcement));
        return true;
      } // bool Parser::Ansage(string text)

    /**
     ** Schweine
     **
     ** @param     text        the player
     **
     ** @return    whether the command was accepted
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.13
     **/
    bool
      Parser::Sau(string text)
      {
        // ToDo: check whether the trump color is known?
        unsigned const p = this->playerno(text);
        CLOG << p << endl;
        CLOG << this->actor().game().player(p).name() << endl;
        CLOG << ::name(::game_status) << endl;
        this->actor().gameplay_action(GameplayAction::Swines(p));
        return true;
      } // bool Parser::Sau(string text)

    /**
     ** Hyperschweine
     **
     ** @param     text        the player
     **
     ** @return    whether the command was accepted
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.13
     **/
    bool
      Parser::Supersau(string text)
      {
        // ToDo: check whether the trump color is known?
        unsigned const p = this->playerno(text);
        this->actor().gameplay_action(GameplayAction::Hyperswines(p));
        return true;
      } // bool Parser::Supersau(string text)

    /**
     ** Armut
     **
     ** @param     text        the player
     **
     ** @return    whether the command was accepted
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.13
     **/
    bool
      Parser::ArmutIsons(string text)
      {
        string keyword, entry;
        EXPECT_KEYWORD("a");
        Player const* const pa = this->actor().party().player_by_name(entry);
        unsigned const pan = this->actor().game().no(*pa);
        EXPECT_KEYWORD("b");
        Player const* const pb = this->actor().party().player_by_name(entry);
        unsigned const pbn = this->actor().game().no(*pb);
        vector<Card> cards(3, Card::UNKNOWN);

        Game const& game = this->actor().game();
        this->pending_reservation[pan].game_type = GAMETYPE::POVERTY;
        this->actor().gameplay_action(GameplayAction::PovertyShift(pan, cards));
        for (unsigned i = 0; i < game.playerno(); ++i) {
          if (   (i != pan)
              && (i != pbn) )
            this->actor().gameplay_action(GameplayAction::PovertyDenied(i));
        }
        this->actor().gameplay_action(GameplayAction::PovertyAccepted(pbn));
        this->actor().gameplay_action(GameplayAction::PovertyReturned(pbn, cards));
        return true;
      } // bool Parser::ArmutIsons(string text)

    /**
     ** Stich genommen
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
      Parser::try_trick_taken()
      {
        unsigned const n = this->check_next_pending({"StichVomTisch",
                                                    "?Hochzeit",
                                                    "SpielerIstDran",
                                                    "sticheprospieler"});
        if (!(n
              && (::game_status == GAMESTATUS::GAME_FULL_TRICK))
           )
          return ;

        this->actor().gameplay_action(::GameplayAction::TrickTaken());
        this->pop_next_pending(n);
        return ;
      } // void Parser::try_trick_taken()

    /**
     ** how many tricks one player has
     **
     ** @param     text        trick
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
      Parser::sticheprospieler(string text)
      {
        CLOG << text << endl;
        exit(0);
        return false;
      } // bool Parser::sticheprospieler(string text)

    /**
     ** game finished: game summary
     **
     ** @param     text        trick
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
      Parser::EndabrechnungsInfo(string text)
      {
        // ToDo: check against game summary
        CLOG << text << endl;
        //exit(0);
        return false;
      } // bool Parser::EndabrechnungsInfo(string text)

    /**
     ** -> result
     **
     ** @param     player  name of the player
     **
     ** @return    number of the player
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.13
     **/
    unsigned
      Parser::playerno(string const& player) const
      {
        DEBUG_ASSERTION(::game_status & GAMESTATUS::GAME,
                        "Parser::playerno(" + player + ")\n"
                        "game_status not game: " << ::name(::game_status));
        for (unsigned p = 0; p < ::party.game().number_of_players(); ++p)
          if (this->game().player(p).name() == player)
            return p;
        DEBUG_ASSERTION(false,
                        "Parser::playerno(" + player + ")\n"
                        "Player not found");
        return UINT_MAX;
      } // unsigned Parser::playerno(string player) const

  } // namespace DokoLounge
} // namespace Network

#endif // #ifdef USE_NETWORK_DOKOLOUNGE
#endif // #ifdef USE_NETWORK
