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
 **********************************************************************/

#pragma once

// some utils
// D.K.

#include "class/readconfig/readconfig.h"
#include "utils/private.h"

#define QUAD(x) ((x) * (x))

#undef VOID
#define VOID

#define PSEUDO_RAND_MAX 279840
// sets the seed of the pseudo random function
unsigned pseudo_rand_set(unsigned const pseudo_seed);
// gets the next pseudo random number
// the function is: r = (a * r + c) % m (see definition)
unsigned pseudo_rand_next();

unsigned
RAND(unsigned const x);

#ifdef MS_VISUAL_C
template<typename T>
inline T const& max(T const& a, T const& b)
{
  return ((a > b) ? a : b);
}
template<typename T>
inline T const& min(T const& a, T const& b)
{
  return ((a < b) ? a : b);
}
#endif

// writes a mark with a counter
unsigned MARK(char const* const string = NULL, bool increase = true);


// file-utils

// all things written into this stream are ignored
class Null_OStr : private std::ostream {
  public:
    Null_OStr() : std::ostream(NULL) {}

    template <class T>
      Null_OStr const& operator<<(T const& t) const
      { return *this; }
  private:
};
extern const Null_OStr null_ostr;


// reads a line from the input stream
string read_line(std::istream& istr);

// generate an error
void generate_error(string const error_message = "generated error");



// this class is an unsigned
// the difference is, that 'UINT_MAX' written in /read from a stream is a '-'
class Unsigned {
  public:
    Unsigned()				: u(0)	{ }
    Unsigned(unsigned const& u)		: u(u)	{ }
    explicit Unsigned(string const& s);
    operator unsigned&()			{ return u; }
    operator unsigned const&() const		{ return u; }
  private:
    unsigned u;
}; // class Unsigned
std::ostream& operator<<(std::ostream& ostr, Unsigned const& u);
std::istream& operator>>(std::istream& istr, Unsigned& u);


// output of the name
#define WRITE_NAME(Type) \
  inline ostream& operator<<(ostream& ostr, Type const t) \
{ return (ostr << ::name(t)); }


// class for temporary setting a variable to a different value
template<typename T>
class LocalValueProxy {
  public:
    LocalValueProxy(T& t, T value) :
      variable(t), old_value(t)
  { t = value; }

    ~LocalValueProxy()
    { this->variable = this->old_value; }

    LocalValueProxy() = delete;
    LocalValueProxy(LocalValueProxy const&) = default;
    LocalValueProxy& operator=(LocalValueProxy const&) = delete;
  private:
    T& variable;
    T const old_value;
}; // class LocalValueProxy
template<typename T>
LocalValueProxy<T> set_local_value(T& t, T value)
{ return LocalValueProxy<T>(t, value); }
