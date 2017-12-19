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

#ifndef TRANSLATIONS_HEADER
#define TRANSLATIONS_HEADER

/*
 * This header has functions 'translation(...)' which returns a 'Translation'
 * for the given arguments.
 */

#include "translator.h"
#include "translation.h"

#include "../basistypes.h"

#include "../card/card.h"

// no argument
inline
Translator::Translation translation(string const& text)
{ return Translator::Translation(::translator, text); }

// one argument
inline
Translator::Translation translation(char const* const text,
				    int const i)
{ return Translator::Translation(::translator, text, i); }

inline
Translator::Translation translation(char const* const text,
				    unsigned const u)
{ return Translator::Translation(::translator, text, u); }

inline
Translator::Translation translation(char const* const text,
				    string const s)
{ return Translator::Translation(::translator, text, s); }

inline
Translator::Translation translation(char const* const text,
				    Translator::Translation const t)
{ return Translator::Translation(::translator, text, t); }

// two arguments

// first: string
inline
Translator::Translation translation(char const* const text,
				    string const& s0,
				    int const i1)
{ return Translator::Translation(::translator, text, s0, i1); }

inline
Translator::Translation translation(char const* const text,
				    string const& s0,
				    unsigned const u1)
{ return Translator::Translation(::translator, text, s0, u1); }

inline
Translator::Translation translation(char const* const text,
				    string const& s0,
				    string const& s1)
{ return Translator::Translation(::translator, text, s0, s1); }

inline
Translator::Translation translation(char const* const text,
				    string const& s0,
				    Translator::Translation const& t1)
{ return Translator::Translation(::translator, text, s0, t1); }

// first: Translation
inline
Translator::Translation translation(char const* const text,
				    Translator::Translation const& t0,
				    int const i1)
{ return Translator::Translation(::translator, text, t0, i1); }

inline
Translator::Translation translation(char const* const text,
				    Translator::Translation const& t0,
				    unsigned const u1)
{ return Translator::Translation(::translator, text, t0, u1); }

inline
Translator::Translation translation(char const* const text,
				    Translator::Translation const& t0,
				    string const& s1)
{ return Translator::Translation(::translator, text, t0, s1); }

inline
Translator::Translation translation(char const* const text,
				    Translator::Translation const& t0,
				    Translator::Translation const& t1)
{ return Translator::Translation(::translator, text, t0, t1); }


// first: integer
inline
Translator::Translation translation(char const* const text,
				    int const i0,
				    string const& s1)
{ return Translator::Translation(::translator, text, i0, s1); }


// first: unsigned
inline
Translator::Translation translation(char const* const text,
				    unsigned const u0,
				    unsigned const u1)
{ return Translator::Translation(::translator, text, u0, u1); }


// three arguments

// first: string

inline
Translator::Translation translation(char const* const text,
				    string const& s0,
				    string const& s1,
				    string const& s2)
{ return Translator::Translation(::translator, text, s0, s1, s2); }

inline
Translator::Translation translation(char const* const text,
				    string const& s0,
				    Translator::Translation const& t1,
				    Translator::Translation const& t2)
{ return Translator::Translation(::translator, text, s0, t1, t2); }

// four arguments

// first: translation
inline
Translator::Translation translation(char const* const text,
				    Translator::Translation const& t0,
				    unsigned const u1,
				    Translator::Translation const& t2,
				    unsigned const u3
				   )
{ return Translator::Translation(::translator, text, t0, u1, t2, u3); }




// other conversions

inline
Translator::Translation translation(Card::Value const& value)
{ return Translator::Translation(::translator,
				 "Card::Value::"
				 + ::name(value)); }
inline
Translator::Translation translation(Card::TColor const& tcolor)
{ return Translator::Translation(::translator,
				 "Card::Color::"
				 + ::name(tcolor)); }
inline
Translator::Translation translation(Card const& card)
{ return Translator::Translation(::translator,
				 "Card::%tcolor% %tvalue%",
				 ::translation(card.color()),
				 ::translation(card.value())); }

inline
Translator::Translation translation(Team const& team)
{ return Translator::Translation(::translator,
				 "Team::"
				 + ::name(team)); }
inline
Translator::Translation translation(GameType const& game_type)
{ return Translator::Translation(::translator,
				 "GameType::"
				 + ::name(game_type)); }
inline
Translator::Translation translation(Announcement const& announcement)
{ return Translator::Translation(::translator,
				 "Announcement::"
				 + ::name(announcement)); }
inline
Translator::Translation translation(MarriageSelector const& marriage_selector)
{ return Translator::Translation(::translator,
				 "MarriageSelector::"
				 + ::name(marriage_selector)); }
inline
Translator::Translation translation(BockTrigger const& bock_trigger)
{ return Translator::Translation(::translator,
				 "BockTrigger::"
				 + ::name(bock_trigger)); }


#endif // #ifndef TRANSLATIONS_HEADER
