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

#ifndef CLASS_NETWORK_CONNECTION_INTERPRETER_DOKOLOUNGE_PARSER_HEADER
#define CLASS_NETWORK_CONNECTION_INTERPRETER_DOKOLOUNGE_PARSER_HEADER

#include "../../../os/os.h"

#include "../interpreter.h"
#include "../../actor.h"
#include "../../../game/reservation.h"

#include <map>
using std::map;
#include <list>
using std::list;

namespace Network {
  namespace DokoLounge {

    /**
     ** parser of the DokoLounge protocol parts
     **
     ** @author	Diether Knof
     **/
    class Parser : public OS {
      typedef list<pair<string, string> > PendingCommands;
      public:
        // general parts

        // returns the text part of <<part>>text<</part>>
        static string text_of_part(string const& text, string const& part);
        // gets the next part of 'text' and remove it from text
        static bool strip_next_part(string& text, string& keyword, string& entry);
        // gets the name of 'text' and remove it from text
        static bool strip_next_name(string& text, string& name, string& entry);

        // the card, given by two numbers
        static Card const& card(string const& text);
        // the cards, given by the text
        static vector<Card> cards(string text);
        // the (two digit) number of the card
        static string const& number1(Card const& card);
        static string const& number2(Card const& card);

      private:
        // initialize the static values
        static void init_static();

        // whether the static values are initialized
        static bool static_initialized;
        // map from number (like 22) to the corresponding card
        static std::map<string, Card> cards_map;
        static std::map<Card, string> cards_reverse_map1;
        static std::map<Card, string> cards_reverse_map2;

      public:
	// constructor
	Parser(Connection::Interpreter& interpreter);
	// destructor
	~Parser();

	// the corresponding actor
	Connection::Actor& actor();
	// the corresponding actor
	Connection::Actor const& actor() const;
	// the corresponding sender
	Interpreter::Sender& sender();
	// the corresponding party
	Party const& party() const;
	// the corresponding rule
	Rule const& rule() const;
	// the corresponding game
	Game const& game() const;

        // receive a command
        void receive_command(string const& keyword, string const& text);
        // interpret the command
        bool interpret_command(string const& keyword, string const& text);

        // whether there is a blocking element
        bool blocking();

        // Prüfe die ausstehenden allgemeine Befehle
        void check_pending();
        // Prüfe die ausstehenden Befehle für das Spiel
        void check_pending_gameplay();
        // Prüft die ausstehenden Befehle
        void update() { this->check_pending(); this->check_pending_gameplay(); }

        // the game is opened
        void game_open(Game const& game);
        // from OS: a gameplay action
        void gameplay_action(GameplayAction const& action);
        // the game is closed
        void game_close();

      private:
        // Spielaktionen, diese können aus mehreren Befehlen bestehen
        // Sie sind erfolgreich ausgeführt, wenn 'pending' kleiner geworden ist.

        // Starte das Turnier (setze Regeln)
        void try_start_party();
        // Initialisiere das Spiel (setze Startspieler, Blätter)
        void try_init_game();
        // Wenn mitten in ein Spiel gesprungen wurde: den nächsten Befehl abarbeiten
        void forward_in_game();
        // Spiele die nächste Karte
        void try_play_card();
        // Der Stich wurde aufgenommen
        void try_trick_taken();

      private:

        // Lounge

        // alert
        bool alerttext(string text);
        // chat message in the lobby
        bool fuxbauchat(string text);
        // player list
        bool LobbySpieler(string text);
        // table types
        bool alleMehrspielertische(string text);
#ifdef OUTDATED
          // Client Version 54
        // table types
        bool alleTische(string text);
#endif
        // table list (Besetzung)
        bool Tischart(string text);
        // Glückspilz
        bool Gluckspilz(string text);

        // Table

        // interpret table chat
        bool TischChat(string text);

        // table entered
        bool DeinTisch(string text);
        // players at the table
        bool alleSpielerAmTisch(string text);
        // new cards 
        bool CleanTisch(string text);

        // Refresh
        // the hand
        bool Karten(string text);
        // trick on the table
        bool KartenAufTisch(string text);
        // First card of the trick
        bool ersteKarte(string text);
        // Which player has to play
        bool SpielerIstDran(string text);
        // swines
        bool Sau(string text);
        // hyperswines
        bool Supersau(string text);
        // Armut
        bool ArmutIsons(string text);
        // (global) points of the player
        bool NeuePunkte(string text);
        // team
        bool Partei(string text);
        // announcement ?
        bool AnsageCount(string text);
        // marriage
        bool Hochzeit(string text);
        // marriage brigde
        bool Hochzeitspartner(string text);
        // Spielersymbole
        bool SpielerIcons(string text);
        // rules
        bool sonderregeln(string text);
        // Who is only watching
        bool zuschauer(string text);
        // Who the player is watching
        bool DuBistZuschauer(string text);
        // Einen Tisch betreten
        bool TischErlaubt(string text);
        // rules
        bool regelinfo1(string text);
        // reservation
        bool Meldung(string text);
        // trump
        bool trumpf(string text);
        // announcement
        bool Ansage(string text);

        // how many tricks one player has
        bool sticheprospieler(string text);
        // game finished: game summary
        bool EndabrechnungsInfo(string text);

      private:
        // the playerno of the player
        unsigned playerno(string const& player) const;

        // Überprüft, ob die nächsten ausstehenden Befehle passen
        unsigned check_next_pending(std::initializer_list<std::string> keys) const;
        // Der nächste Befehl
        string next_keyword() const;
        // Der Eintrag zum nächsten Befehl
        string next_entry() const;
        // Entfernt den nächsten ausstehenden Befehl
        void pop_next_pending(unsigned const n = 1);

        // list the pending entries on cout
        void list_pending() const;

      protected:
	// the corresponding interpreter
	Connection::Interpreter* interpreter;

        // pending commands (the order is relevant)
        PendingCommands pending;
        // whether there is a reservation
        bool reservation_open;
        // pending reservations
        vector< ::Reservation> pending_reservation;
        // the player offset
        PRIV_VAR_R(unsigned, player_offset);
        // for jumping into a game: the cards already played
        PRIV_VAR_R(list<GameplayAction*>, pending_gameplay_actions);

      private: // unused
	Parser();
	Parser(Parser const&);
	Parser& operator=(Parser const&);
    }; // class Network::Interpreter::DokoLounge::Parser : public OS

  } // namespace DokoLounge
} // namespace Network

#endif // #ifndef CLASS_NETWORK_CONNECTION_INTERPRETER_DOKOLOUNGE_PARSER_HEADER

#endif // #ifdef USE_NETWORK_DOKOLOUNGE
#endif // #ifdef USE_NETWORK
