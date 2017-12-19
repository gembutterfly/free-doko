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

#include "receiver.h"
#include "sender.h"

#include "parser/parsers.h"

#include "../server.h"

#include "../../party/party.h"
#include "../../game/gameplay_action.h"

#include <cstring>

namespace Network {
  namespace FreeDoko {
    /**
     ** split the line in 'time', 'number' and 'message'
     **
     ** @param     line      the line to split
     ** @param     time      where to save the time
     ** @param     message   string to save the message part into
     **
     ** @return    message type
     **            'UNKNOWN' on error
     **
     ** @author    Diether Knof
     **
     ** @version   0.7.3
     **/
    Interpreter::MessageType
      Interpreter::Receiver::split_line(string const& line,
					Time& time,
					string& message)
      {
	/* Format:
	 *   1) simple text
	 *   2) *time: special text
	 *   3) time: normal text
	 */

	MessageType type = UNKNOWN;
	if (line.find(':') == string::npos)
	  type = SIMPLE;
	else if (   (line[0] == '*')
		 && (   (   (line[1] == '-')
			 && (line[2] == ':') )
		     || isdigit(line[1]) ) )
	  type = SPECIAL;
	else if (   (   (line[1] == '-')
		     && (line[2] == ':') )
		 || isdigit(line[1]) )
	  type = NORMAL;

	if (type == SIMPLE) {
	  message = line;
	  return type;
	}

	  size_t current_pos = 0;

	if (type == SPECIAL)
	  // skip the '*'
	  current_pos = 1;

	  time = Time();
	  { // read the time
	    size_t const time_colon = line.find(':', current_pos);

	    if (   (time_colon == 1)
		&& (line[current_pos] == '-') ) {
	      // all ok, we keep the initial values
	    } else {
	      // read <seconds>.<miliseconds>
	      string const time_text(line, current_pos, time_colon);
	      istringstream istr(time_text);
	      istr >> time.sec;
	      if (istr.peek() != '.')
		return UNKNOWN;
	      istr.get();
	      istr >> time.msec;
	      if (istr.fail())
		return UNKNOWN;
	    }

	    current_pos = time_colon + 1;
	  } // read the time

	  // skip blanks
	  while (   (current_pos < line.size())
		 && (line[current_pos] == ' '))
	    current_pos += 1;

	  if (current_pos == line.size())
	    return UNKNOWN;

	  message = string(line, current_pos);

	  return type;
	} // static Interpreter::MessageType Interpreter::Receiver::split_line(string line, Time& time, string& message)

	/**
	 ** constructor
	 **
	 ** @param	interpreter	corresponding interpreter
	 **
	 ** @return	-
	 **
	 ** @author	Diether Knof
	 **
	 ** @version	0.7.1
	 **/
	Interpreter::Receiver::Receiver(Interpreter& interpreter) :
	  Connection::Interpreter::Receiver(interpreter),
	  pending_line(),
	  parser(NULL)
	{ }

	/**
	 ** destructor
	 **
	 ** @param	-
	 **
	 ** @return	-
	 **
	 ** @author	Diether Knof
	 **
	 ** @version	0.7.1
	 **/
	Interpreter::Receiver::~Receiver()
	{ }

	/**
	 ** -> result
	 **
	 ** @param		-
	 **
	 ** @return		the FreeDoko interpreter
	 **
	 ** @author		Diether Knof
	 **
	 ** @version	0.7.1
	 **/
	Interpreter::Sender&
	  Interpreter::Receiver::sender()
	  {
	    return dynamic_cast<Interpreter::Sender&>(this->interpreter().sender());
	  } // Sender& Interpreter::Receiver::sender()

	/**
	 ** a text has been received
	 **
	 ** @param     text   received text
	 **
	 ** @return    whether the text was accepted (or saved for later)
	 **
	 ** @author    Diether Knof
	 **
	 ** @version   0.7.3
	 **/
	bool
	  Interpreter::Receiver::received(string const& text)
	  {
	    bool value = true;
	    size_t pos = 0;

	    string const text2 = pending_line + text;

	    // split the text in single line
	    while (   (pos < text2.size())
		   && (text2.find('\n', pos) != string::npos) ) {
	      value &= this->received_line(string(text2, pos,
						  text2.find('\n', pos) - pos));

	      pos = text2.find('\n', pos);
	      while (   (pos != string::npos)
		     && (pos < text2.size())
		     && (text2[pos] == '\n') )
		pos += 1;

	    } // while ('\n' remaining)
	    pending_line = ( (pos > text2.size())
			    ? ""
			    : string(text2, pos, string::npos) );

	    return value;
	  } // bool Interpreter::Receiver::received(string text)

	/**
	 ** a line has been received
	 **
	 ** @param     line	received line
	 **
	 ** @return    whether the line was accepted (or saved for later)
	 **
	 ** @author    Diether Knof
	 **
	 ** @version   0.7.3
	 **/
	bool
	  Interpreter::Receiver::received_line(string const& line)
	  {
	    // ignore empty lines
	    if (line.empty())
	      return true;

	    Time time;
	    string message;
	    switch (Receiver::split_line(line, time, message)) {
	    case UNKNOWN:
	      cerr << "error splitting the line '" << line << "'" << endl;
	      return false;
	    case SIMPLE:
	      return this->act_simple(message);
	    case SPECIAL:
	      return this->act_special(message);
	    case NORMAL:
	      return this->act_normal(message);
	    } // switch (Receiver::split_line(line, time, message))

	    return false;
	  } // bool Interpreter::Receiver::received_line(string line)

	/**
	 ** act according to a simple message
	 **
	 ** @param     message   simple message
	 **
	 ** @return    whether the message could be interpreted
	 **
	 ** @author    Diether Knof
	 **
	 ** @version   0.7.2
	 **
	 ** @todo      bye
	 **/
	bool
	  Interpreter::Receiver::act_simple(string const& message)
	  {
	    /* simple messages:
	     * 'FreeDoko server'
	     *     the connection is to a FreeDoko server
	     * 'bye'
	     *     the server has dropped the connection
	     */

	    // The connection is to a FreeDoko server
	    if (message == "FreeDoko server") {
	      // send the name
	      this->sender().name();
	      return true;
	    } else if (message == "bye") {
	      // ToDo: end the connection
	      return true;
	    } else if (message == "ping") {
	      return this->act_special(message);
	    } else {
	      return false;
	    }
	  } // bool Interpreter::Receiver::act_simple(string message)

	/**
	 ** act according to a special message
	 **
	 ** @para      message   special message
	 **
	 ** @return    whether the message could be interpreted
	 **
	 ** @author    Diether Knof
	 **
	 ** @version   0.7.1
	 **
	 ** @todo      bye
	 **/
	bool
	  Interpreter::Receiver::act_special(string const& message)
	  {
	    /* Special words
	     *   ping			reply with 'pong'
	     *   pong			'ping' reply
	     *   ping: %dnumber%	reply with 'pong: %dnumber'
	     *   pong: %dnumber%	'ping' reply
	     *   bye
	     */

	    { // without parameter
	      if (message == "ping") {
		this->sender().pong();
		return true;
	      } else if (message == "pong") {
		// ToDo: check, whether this is a reply of a ping
		return true;
	      } else if (message == "bye") {
		// ToDo: close the connection
		return true;
	      } // if (message == ...)
	    } // without parameter

	    { // with a parameter
	      size_t const colon_pos = message.find(':');
	      if (colon_pos != string::npos) {
		// one parameter
		string const word(message, 0, colon_pos);
		string const param(message,
				   ((message.size() > colon_pos)
				    && (message[colon_pos + 1] == ' '))
				   ? colon_pos + 2
				   : colon_pos + 1);

		// Network
		if (word == "ping") {
		  // reply with 'pong: %n+1%'
		  istringstream istr(param);
		  unsigned n;
		  istr >> n;
		  if (istr.fail())
		    return false;

		  this->sender().pong(n);

		  return true;

		} else if (word == "pong") {
		  // a reply of a ping
		  istringstream istr(param);
		  unsigned n;
		  istr >> n;
		  if (istr.fail())
		    return false;

		  // ToDo: check, whether this is a reply of a ping

		  return true;

		} // if (word == "...")

	      } // if (colon_pos != string::npos)
	    } // with a parameter

	    cerr << "FreeDoko::Interpreter::Receiver::act_special(message)\n"
	      << "  I do not know the special message '" << message << "' :-("
	      << endl;
	    return false;
	  } // bool Interpreter::Receiver::act_special(string message)

	/**
	 ** act according to a normal message
	 **
	 ** @param     message   normal message
	 **
	 ** @return    whether the message could be interpreted
	 **
	 ** @author    Diether Knof
	 **
	 ** @version   0.7.2
	 **/
	bool Interpreter::Receiver::act_normal(string const& message)
	{
	  // if there is a parser, use it
	  if (this->parser) {
	    if (!this->parser->interpret(message))
	      return false;

	    if (this->parser->finished()) {
	      delete this->parser;
	      this->parser = NULL;
	    }
	    return true;
	  } // if (this->parser)


	  string word;
	  string param;
	  unsigned param_u = UINT_MAX;
	  { // check for a parameter
	    size_t const colon_pos = message.find(':');
	    if (colon_pos != string::npos) {
	      // one parameter
	      word = string(message, 0, colon_pos);
	      param = string(message,
			     ((message.size() > colon_pos)
			      && (message[colon_pos + 1] == ' '))
			     ? colon_pos + 2
			     : colon_pos + 1);
	      param_u = atoi(param.c_str());
	    }
	  } // check for a parameter

	  if (word == "name") {
	    // change the name of this connection
	    this->connection().set_name(param);
	    return true;

	  } else if (string(word, 0, strlen("player ")) == "player ") {
	    // player name or type
	    unsigned playerno = UINT_MAX;
	    sscanf(word.c_str(), "player %u:", &playerno);
	    if (playerno >= this->connection().server().party().playerno())
	      return false;
	    Player const&
	      player = this->connection().server().party().player(playerno);

	    Config const config = Config_(param);
	    if (config.name == "name") {
	      return this->actor().set_player_name(player, config.value);
	    } else if (config.name == "type") {
	      return this->actor().set_player_type(player,
						   (config.value == "network"));
	    } else {
	      return false;
	    } // if (config.name == "...")

	  } else if (word == "players switched") {
	    // ToDo: error checking
	    unsigned const player_a
	      = atoi(param.c_str());
	    unsigned const player_b
	      = atoi(param.c_str() + strlen(" 1"));
	    this->actor().players_switched(player_a, player_b);
	    return true;

	  } else if (word == "chat") {
	    // the player has chattet
	    this->actor().chat(param);
	    return true;

	  } else if (message == "rules") {
	    DEBUG_ASSERTION((parser == NULL),
			    "Network::FreeDoko::Interpreter::Receiver::act_normal(\"rules\")\n"
			    "  there is already a parser");
	    this->parser = new Network::FreeDoko::Parser::Rules(this->interpreter());
	    return true;

	  } else if (word == "rule") {
	    // a rule was send
	    return this->actor().set_rule(param);

	  } else if (word == "ask") {
#ifdef DKNOF
	    COUT << "ask for '" << param << "'" << endl;
#endif

	    if (param == "rules") {
	      this->sender().rules();
	      return true;

	    } else if (param == "name") {
	      this->sender().name();
	      return true;

	    } // if (param == "...")

	    //
	    // gameplay
	    //
	  } else if (   (word == "sync")
                     && (param == "gameplay actions") ) {
	    DEBUG_ASSERTION((parser == NULL),
			    "Network::FreeDoko::Interpreter::Receiver::act_normal(\"sync: gameplay actions\")\n"
			    "  there is already a parser");
	    this->parser = new Network::FreeDoko::Parser::GameplayActions(this->interpreter());
	    return true;

	  } else if (word == "gameplay action") {
	    GameplayAction* action = GameplayAction::new_(param);
	    if (action) {
	      this->actor().gameplay_action(*action);
	      delete action;

	      return true;
	    } // if (action)

	  } else if (word == "party") {

	    if (param == "start") {
	      this->actor().party_start();
	      return true;

	    } else if (string(param, 0, strlen("startplayer:"))
		       == "startplayer:") {
	      param_u = atoi(param.c_str() + strlen("startplayer:"));
	      this->actor().party_set_startplayer(param_u);
	      return true;

	    } else if (string(param, 0, strlen("seed:"))
		       == "seed:") {
	      param_u = atoi(param.c_str() + strlen("seed:"));
	      this->actor().party_set_seed(param_u);
	      return true;

	    } else if (string(param, 0, strlen("start round:"))
		       == "start round:") {
	      param_u = atoi(param.c_str() + strlen("start round:"));
	      this->actor().party_start_round(param_u);
	      return true;

	    } else if (param == "finish") {
	      this->actor().party_finish();
	      return true;
	    } // if (param == ...)

	  } else if (word == "game") {

	    if (param == "open") {
	      this->actor().game_open();
	      return true;
            } else if (param == "start") {
	      this->actor().game_start();
	      return true;
	    } else if (param == "finish") {
	      this->actor().game_finish();
	      return true;
	    } else if (string(param, 0, strlen("startplayer:"))
		       == "startplayer:") {
	      // ToDo
	      return true;
	    } else if (string(param, 0, strlen("seed:"))
		       == "seed:") {
	      // ToDo
	      return true;
	    } else if (string(param, 0, strlen("hand "))
		       == "hand ") {
	      // ToDo
	      return true;
	    }

	  } // if (word == ...)

	  cerr << '\n'
	    << "FreeDoko::Interpreter::Receiver::act_normal(message)\n"
	    << "  I do not know the message '" << message << "' :-("
	    << endl;
	  cerr << "word = '" << word << "'" << endl;
	  cerr << "param = '" << param << "'" << endl;
	  return false;
	} // bool Interpreter::Receiver::act_normal(string message)

  } // namespace FreeDoko
} // namespace Network

#endif // #ifdef USE_NETWORK
