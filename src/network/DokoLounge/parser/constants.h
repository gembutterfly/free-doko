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

#ifndef HEADER_NETWORK_DOKOLOUNGE_PARSER_CONSTANTS
#define HEADER_NETWORK_DOKOLOUNGE_PARSER_CONSTANTS

#include "../constants.h"

#define EXPECT_PENDING(KW) \
        if (this->pending.empty()) { \
          cerr << __FILE__ << '#' << __LINE__ << ' ' \
               << "no pending command, but expected '" << KW << "'\n"; \
          exit(0); \
        } \
        keyword = this->pending.front().first; \
        entry = this->pending.front().second; \
        if (keyword != (KW)) { \
          cerr << __FILE__ << '#' << __LINE__ << ' ' \
               << "'interpret ...': unknown keyword '" << keyword << "' expected '" << (KW) << "'\n" \
               << "entry: " << entry << "\n"; \
          exit(0); \
        } else (void) 0

#define EXPECT_TEXT(TEXT) \
        if (   (text.size() < string(TEXT).size()) \
            || (string(text, 0, string(TEXT).size()) != (TEXT)) ) { \
          cerr << __FILE__ << '#' << __LINE__ << ' ' \
               << "'interpret ...': unknown text '" << text << "' expected '" << (TEXT) << "'\n"; \
          exit(0); \
        } else \
          text.erase(0, string(TEXT).size())

#define EXPECT_KEYWORD(KW) \
        this->strip_next_part(text, keyword, entry); \
        if (keyword != (KW)) { \
          cerr << __FILE__ << '#' << __LINE__ << ' ' \
               << "'interpret ...': unknown keyword '" << keyword << "' expected '" << (KW) << "'\n" \
               << "entry: " << entry << "\n" \
               << "remaining text: " << text << "\n"; \
          exit(0); \
        } else (void) 0

#define EXPECT_EMPTY_TEXT \
        if (!text.empty()) { \
          cerr << __FILE__ << '#' << __LINE__ << ' ' \
               << "'interpret ...': expected empty text\n" \
               << "remaining text: " << text << "\n"; \
          exit(0); \
        } else (void) 0

#define PLAYERNO_FROM_FREEDOKO(P) (((P) + this->player_offset()) % this->rule()(Rule::NUMBER_OF_PLAYERS_IN_GAME) + 1)
#define PLAYERNO_TO_FREEDOKO(P) ((((P) - 1) + this->rule()(Rule::NUMBER_OF_PLAYERS_IN_GAME) - this->player_offset()) % this->rule()(Rule::NUMBER_OF_PLAYERS_IN_GAME))


#endif // #ifndef HEADER_NETWORK_DOKOLOUNGE_PARSER_CONSTANTS
