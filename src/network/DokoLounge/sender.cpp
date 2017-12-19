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

#include "sender.h"

#include "../server.h"
#include "../../misc/setting.h"
#include "../../misc/lounge.h"

#include "../../utils/string.h"

namespace Network {
  namespace DokoLounge {

    /**
     ** constructor
     **
     ** @param     interpreter   corresponding interpreter
     **
     ** @return    -
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.12
     **/
    Interpreter::Sender::Sender(Interpreter& interpreter) :
      Connection::Interpreter::Sender(interpreter)
    { }

    /**
     ** destructor
     **
     ** @param     -
     **
     ** @return    -
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.12
     **/
    Interpreter::Sender::~Sender()
    { }

    /**
     ** -> result
     **
     ** @param     -
     **
     ** @return    the account
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.12
     **/
    string const&
    Interpreter::Sender::account() const
    {
      return ::lounge->account();
    } // string Interpreter::Sender::account() const

    /**
     ** the connection is up
     **
     ** @param     -
     **
     ** @return    -
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.1
     **/
    void
      Interpreter::Sender::connection_up()
      {
#if 0
        if (this->connection().relation() == Connection::CHILD)
          this->connection().send("FreeDoko server\n");

        if (   (::game_status != GAMESTATUS::PROGRAMSTART)
            && (::game_status != GAMESTATUS::PARTY_NEW) ) {
          this->connection().send("connection rejected\n");
          // ToDo: reject the connection
          return ;
        }

        this->name();

        if (this->connection().relation() == Connection::CHILD)
          this->rules();

        if (this->connection().relation() == Connection::CHILD)
          this->connection().create_network_player();

        for (auto p : this->connection().server().party().players()) {
          this->player_type(*p);
          if (!this->connection().manages_player(*p))
            this->player_name(*p);
        }
#endif

        return ;
      } // void Interpreter::Sender::connection_up()

    /**
     ** send a ping
     **
     ** @param     -
     **
     ** @return    -
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.1
     **/
    void
      Interpreter::Sender::ping()
      {
#if 0
        this->send_with_time("ping");
#endif
        return ;
      } // void Interpreter::Sender::ping()

    /**
     ** send a ping with a number
     **
     ** @param     n   number of the ping
     **
     ** @return    -
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.1
     **/
    void
      Interpreter::Sender::ping(unsigned const n)
      {
#if 0
        this->send_special("ping: " + DK::Utils::String::to_string(n));
#endif
        return ;
      } // void Interpreter::Sender::ping(unsigned n)

    /**
     ** send a pong
     **
     ** @param     -
     **
     ** @return    -
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.1
     **/
    void
      Interpreter::Sender::pong()
      {
#if 0
        this->send_special("pong");
#endif
        return ;
      } // void Interpreter::Sender::pong()

    /**
     ** send a pong with a number
     **
     ** @param     n   number of the pong
     **
     ** @return    -
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.1
     **/
    void
      Interpreter::Sender::pong(unsigned const n)
      {
#if 0
        this->send_special("pong: " + DK::Utils::String::to_string(n));
#endif
        return ;
      } // void Interpreter::Sender::pong(unsigned n)

    /**
     ** send a chat text to the lounge
     **
     ** @param     text   chat text
     **
     ** @return    -
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.13
     **/
    void
      Interpreter::Sender::chat_lounge(string const& text)
      {
        this->chat("fuxbauchat", text);
        return ;
      } // void Interpreter::Sender::chat_lounge(string text)

    /**
     ** send a chat text to the table
     **
     ** @param     text   chat text
     **
     ** @return    -
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.13
     **/
    void
      Interpreter::Sender::chat_table(string const& text)
      {
        this->chat("TischChat", text);
        return ;
      } // void Interpreter::Sender::chat_table(string text)

    /**
     ** send a chat text to the lounge
     **
     ** @param     text   chat text
     **
     ** @return    -
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.13
     **/
    void
      Interpreter::Sender::chat(string const& text)
      {
        this->chat_lounge(text);
        return ;
      } // void Interpreter::Sender::chat(string text)

    /**
     ** send a chat text
     **
     ** @param     location   where to chat
     ** @param     text       chat text
     **
     ** @return    -
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.13
     **/
    void
      Interpreter::Sender::chat(string const& location, string text)
      {
#if 0
        this->send("chat: " + text);
#endif
        /*
           'Ein Spieler will sein Passwort aendern
           If Left(dummyTxt, 12) = "/newpassword" Then
           'ein Admin will den Server stoppen
           If Left(dummyTxt, 11) = "/serverstop" Then
           such = "/anmeldungen"
           ShowAnmeldungen(userID, dummyTxt)
           such = "/mac"
           ShowMac(userID, dummyTxt)
           such = "/list"
           ShowInfoSpieler(userID, dummyTxt)
           such = "/lock "
           SpielerSperren(userID, dummyTxt)
           such = "/unlock "
           SpielerEntSperren(userID, dummyTxt)
           such = "/version"
           sende userID, "<<spielerinfow>>" + s + "<</spielerinfow>>"
           such = "/setoff "
           SetOffPlayer(dummyTxt)
           such = "/setoffline "
           SetOffPlayer(dummyTxt)
           such = "/update"
           s = "<<update>>" + AktuelleClientVersionDownload + "<</update>>"
           'ein Client will einen Listentisch erstellen
           such = "/startliste"
           ListenTischErstellen(userID, user, dummyTxt)
           'ein Client will eine Liste beenden
           such = "/endliste"
           ListenTischBeenden(userID, user, dummyTxt)
           'ein Client will die Listen Solis sehen
           such = "/listensoli"
           ListenSoliAnzeigen(userID, user, dummyTxt)
           '1ste Dulle sticht im letzten Stich bei Sinderregeln ein/ausstellen
           such = "/Dulle"
           'Supersau aus/anstellen
           such = "/Supersau"
           'ohnen Neunen spielen
           such = "/Neunen-"
           'ohnen Neunen spielen
           such = "/Neunen+"
           'regeln anzeigen
           such = "/regeln"
           'IDs der Clients anzeigen lassen
           such = "/online"
           'IDs der Clients anzeigen lassen
           such = "/SetOffId"
           'doppeltes Senden an/ausmachen
           such = "/SendeDoppelt"
           such = "/datenbank"
           DatenbankSenden(userID)
           such = "/gamedatenbank"
           DatenbankSenden(userID)
           'Faces sollen geupdatet werden
           such = "/faceUpdate"
           'Bilder (nicht Faces) sollen geupdatet werden
           such = "/BilderUpdate"
           'nur zum proggen spieler wechseln bei mehrspielertischen
           such = "/ww"
           'nur zum proggen spieler wechseln bei mehrspielertischen
           such = "/qq"
           */


        if (   (text.size() >= 2)
            && (text[0] == '/')
            && (text[1] == 'a') ) {
          // for all
          // /a: sende "<<anAlle><<name>>" + MeinName + "<</name>><<mess>>" + cs + "<</mess>><</anAlle>>"
          text.erase(0, 2);
          while (!text.empty() && (text[0] == ' ') )
            text.erase(0, 1);

          this->send_command("anAlle",
                             this->command("name", this->account())
                             + this->command("mess", text));
        } else if (   (text.size() >= 3)
                   && (text[0] == '/')
                   && (text[1] == 'f') ) {
          // whisper (flüsern)
          // /f: <<fluestern>><<sender>>Sender<</sender>><<empfaenger>>Empfänger<</empfaenger>><<message>>Text<</message>><</fluestern>>
          text.erase(0, 2);
          while (!text.empty() && (text[0] == ' ') )
            text.erase(0, 1);
          string const receiver = string(text, 0, text.find(" "));
          text.erase(0, text.find(" "));
          while (!text.empty() && (text[0] == ' ') )
            text.erase(0, 1);
          this->send_command("fluestern",
                             this->command("sender", this->account())
                             + this->command("empfaenger", receiver)
                             + this->command("message", text));
        } else {
          this->send_name_command("fuxbauchat", text);
        }
        return ;
      } // void Interpreter::Sender::chat(string location, string text)

    /**
     ** send the name
     **
     ** @param     -
     **
     ** @return    -
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.1
     **/
    void
      Interpreter::Sender::name()
      {
#if 0
        this->send("name: " + ::setting(Setting::NAME));
#endif
        return ;
      } // void Interpreter::Sender::name()

    /**
     ** sends a simple command
     ** example: <<klingel>><</klingel>>
     ** A simple message is send without further additions
     **
     ** @param     command   command to send
     **
     ** @return    -
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.12
     **/
    void
      Interpreter::Sender::send_simple_command(string const& command)
      {
        this->send(this->simple_command(command));

        return ;
      } // void Interpreter::Sender::send_simple_command(string command)

    /**
     ** sends a command with text command
     **
     ** @param     command   command to send
     ** @param     text      text of the command
     **
     ** @return    -
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.12
     **/
    void
      Interpreter::Sender::send_command(string const& command,
                                        string const& text)
      {
        this->send(this->command(command, text));

        return ;
      } // void Interpreter::Sender::send_simple_command(string command, string text)

    /**
     ** sends a command with the name
     **
     ** @param     command   command to send
     **
     ** @return    -
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.12
     **/
    void
      Interpreter::Sender::send_name_command(string const& command)
      {
        this->send(this->name_command(command));

        return ;
      } // void Interpreter::Sender::send_name_command(string command)

    /**
     ** sends a command with the name
     **
     ** @param     command   command to send
     ** @param     text      text of the command
     **
     ** @return    -
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.12
     **/
    void
      Interpreter::Sender::send_name_command(string const& command,
                                             string const& text)
      {
        this->send(this->name_command(command, text));

        return ;
      } // void Interpreter::Sender::send_name_command(string command, string text)

    /**
     ** sends the text over the connection
     **
     ** @param     text   text to send
     **
     ** @return    -
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.12
     **/
    void
      Interpreter::Sender::send(string const& text)
      {
        std::cout << "> " << text << '\n';
        this->connection().send(text);

        return ;
      } // void Interpreter::Sender::send(string text)

    /**
     ** -> result
     **
     ** @param     command     command
     **
     ** @return    a command string
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.12
     **/
    string
      Interpreter::Sender::simple_command(string const& command) const
      {
        return this->command(command, "");
      } // string Interpreter::Sender::simple_command(string command) const

    /**
     ** -> result
     **
     ** @param     command     command
     ** @param     text        text in the command
     **
     ** @return    a command string
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.12
     **/
    string
      Interpreter::Sender::command(string const& command, string const& text) const
      {
        return ("<<" + command + ">>" 
                + text
                + "<</" + command + ">>");
      } // string Interpreter::Sender::command(string command, string text) const

    /**
     ** -> result
     **
     ** @param     command     command
     **
     ** @return    a command string
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.12
     **/
    string
      Interpreter::Sender::name_command(string const& command) const
      {
        return this->command(command,
                             this->account());
      } // string Interpreter::Sender::command(string command) const

    /**
     ** -> result
     **
     ** @param     command     command
     ** @param     text        text in the command
     **
     ** @return    a command string
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.12
     **/
    string
      Interpreter::Sender::name_command(string const& command,
                                        string const& text) const
      {
        return this->command(command,
                             this->account() + ":" + text);
      } // string Interpreter::Sender::name_command(string command, string text) const

  } // namespace DokoLounge
} // namespace Network

#endif // #ifdef USE_NETWORK_DOKOLOUNGE
#endif // #ifdef USE_NETWORK
