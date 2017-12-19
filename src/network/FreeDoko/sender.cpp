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

#ifdef USE_NETWORK

#include "constants.h"

#include "sender.h"

#include "../server.h"
#include "../../misc/setting.h"

#include "../../utils/string.h"

namespace Network {
  namespace FreeDoko {

  /**
   ** constructor
   **
   ** @param     interpreter   corresponding interpreter
   **
   ** @return    -
   **
   ** @author    Diether Knof
   **
   ** @version   0.7.1
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
   ** @version   0.7.1
   **/
   Interpreter::Sender::~Sender()
  { }

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

       for (vector<Player*>::const_iterator
	    p = this->connection().server().party().players().begin();
	    p != this->connection().server().party().players().end();
	    ++p) {
	   this->player_type(**p);
	   if (!this->connection().manages_player(**p))
	     this->player_name(**p);
       }

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
       this->send_with_time("ping");
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
       this->send_special("ping: " + DK::Utils::String::to_string(n));
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
       this->send_special("pong");
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
       this->send_special("pong: " + DK::Utils::String::to_string(n));
       return ;
     } // void Interpreter::Sender::pong(unsigned n)

   /**
    ** send a chat text
    **
    ** @param     text   chat text
    **
    ** @return    -
    **
    ** @author    Diether Knof
    **
    ** @version   0.7.1
    **/
   void
     Interpreter::Sender::chat(string const& text)
     {
       this->send("chat: " + text);
       return ;
     } // void Interpreter::Sender::chat(string text)

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
       this->send("name: " + ::setting(Setting::NAME));
       return ;
     } // void Interpreter::Sender::name()

   /**
    ** sends a simple message
    ** A simple message is send without further additions
    **
    ** @param     text   text to send
    **
    ** @return    -
    **
    ** @author    Diether Knof
    **
    ** @version   0.7.3
    **/
   void
     Interpreter::Sender::send_simple(string const& text)
     {
       this->connection().send(text + "\n");

       return ;
     } // void Interpreter::Sender::send_simple(string text)

   /**
    ** sends a special message
    ** A special message is prefixed by the time.
    **
    ** @param     text   text to send
    **
    ** @return    -
    **
    ** @author    Diether Knof
    **
    ** @version   0.7.3
    **/
   void
     Interpreter::Sender::send_special(string const& text)
     {
       COUT << "Send special " << text << endl;
       this->send_with_time(text, true);

       return ;
     } // void Interpreter::Sender::send_special(string text)

   /**
    ** sends a normal message
    ** A normal message is prefixed by the time.
    **
    ** @param     text   text to send
    **
    ** @return    -
    **
    ** @author    Diether Knof
    **
    ** @version   0.7.3
    **/
   void
     Interpreter::Sender::send(string const& text)
     {
       this->send_with_time(text);

       return ;
     } // void Interpreter::Sender::send(string text)

   /**
    ** sends the lines over the connection (each line with time and counter)
    **
    ** @param     lines   lines to send
    **
    ** @return    -
    **
    ** @author    Diether Knof
    **
    ** @version   0.7.1
    **/
   void
     Interpreter::Sender::send_lines(string const& lines)
     {
       string::size_type pos = 0;
       while (pos != string::npos) {
	 string::size_type const pos_eol = lines.find('\n', pos);
	 if (pos_eol == string::npos)
	   break;

	 if (pos_eol != pos)
	   this->send(string(lines, pos, pos_eol - pos));
	 pos = pos_eol + 1;
       } // while (pos != string::npos)

       return ;
     } // void Interpreter::Sender::send_lines(string lines)

   /**
    ** prepends the time to the text and send it over the connection
    **
    ** @param     text      text to send
    ** @param     special   whether the message is special
    **                      default: false
    **
    ** @return    -
    **
    ** @author    Diether Knof
    **
    ** @version   0.7.3
    **/
   Time
     Interpreter::Sender::send_with_time(string const& text, bool const special)
     {
       Time const time = Time::current();
       string const timestring
	 = ( (time.sec == 0)
	    ? string("-")
	    : (DK::Utils::String::to_string(time.sec)
	       + "."
	       + DK::Utils::String::to_string(time.msec)) );

       if (special)
	 this->connection().send("*" + timestring + ": " + text + "\n");
       else
	 this->connection().send(timestring + ": " + text + "\n");

       return time;
     } // Time Interpreter::Sender::send_with_time(string text, bool special = false)

  } // namespace FreeDoko
} // namespace Network

#endif // #ifdef USE_NETWORK
