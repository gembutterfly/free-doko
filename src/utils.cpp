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


#include "constants.h"

#include <sys/stat.h>

// all things written into this stream are ignored
const Null_OStr null_ostr;

// values for the random function
#define PSEUDO_RAND_A 7200
#define PSEUDO_RAND_C  100
#define PSEUDO_RAND_M (PSEUDO_RAND_MAX + 1)

// variable with the last random value
static unsigned rand_value_p = 0;

/*****************************************************************************
 *
 ** unsigned pseudo_rand_set(unsigned const pseudo_seed)
 *
 ** Parameters:	pseudo_seed - new seed
 *
 ** Result:	random value (the new seed)
 ** 
 ** Version:	0.3.6
 *
 ** Description:	sets the random value to the seed (if it is < PSEUDO_RAND_MAX)
 *
 ****************************************************************************/
unsigned
pseudo_rand_set(unsigned const pseudo_seed)
{
  if (pseudo_seed < PSEUDO_RAND_MAX)
    rand_value_p = pseudo_seed;

  return rand_value_p;
} // unsigned pseudo_rand_set(pseudo_seed)

/*****************************************************************************
 *
 ** unsigned pseudo_rand_next()
 *
 ** Parameters:	None
 *
 ** Result:	next pseudo random value
 ** 
 ** Version:	0.3.6
 *
 ** Description:	Result
 *
 ****************************************************************************/
unsigned
pseudo_rand_next()
{
  rand_value_p = (  (PSEUDO_RAND_A * rand_value_p + PSEUDO_RAND_C)
                  % PSEUDO_RAND_M);

  return rand_value_p;
} // unsigned pseudo_rand_next()

/**
 ** -> result
 **
 ** @param	x	maximal value + 1
 **
 ** @return	random value in {0, ..., x - 1}
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 **
 ** @version 0.6.3
 **/
unsigned
RAND(unsigned const x)
{
  return (pseudo_rand_next() % x);
}

/**
 ** writes a mark with a counter
 **
 ** @param	string		text to write (default: NULL)
 ** @param	increase	whether to increase the counter (default: true)
 **
 ** @return	new counter
 **
 ** @author	Diether Knof
 **
 ** @version 0.6.9
 **/
unsigned MARK(char const* const string, bool increase)
{
  static unsigned int mark_nr = 0;
  if (increase)
    mark_nr += 1;

  if (string != NULL)
    cerr << mark_nr << ": " << string << endl;
  else
    cerr << mark_nr << endl;

  return mark_nr;
} // unsigned MARK(char const* const string = NULL, bool increase = true);

/**
 ** reads a line from the input stream
 ** empty lines are skipped and the DOS EOL is recognized
 **
 ** @param	istr	input stream
 **
 ** @return	read line
 **
 ** @author	Diether Knof
 **
 ** @version 0.6.9
 **/
string
read_line(std::istream& istr)
{
  string line;
  do {
    std::getline(istr, line);
    if ( !line.empty() && (*(line.end() - 1) == '\r') )
      line.erase(line.end() - 1);
  } while (line.empty() && istr.good()) ;
  return line;
} // string read_line(istream& istr)

/**
 ** generates an error
 **
 ** @param	error_message	error message
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version 0.6.3
 **/
void
generate_error(string const error_message)
{
  DEBUG_ASSERTION(false, error_message);

  return ;
}

/**
 ** constructor
 **
 ** @param	s	string with the unsigned
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version 0.6.9
 **/
Unsigned::Unsigned(string const& s) :
  u(UINT_MAX)
{
  istringstream istr(s);
  istr >> *this;
}

/**
 ** writes an 'Unsigned' into the stream
 ** 'UINT_MAX' is written as '-'
 **
 ** @param	ostr	stream to write into
 ** @param	u	unsigned to write
 **
 ** @return	output stream
 **
 ** @author	Diether Knof
 **
 ** @version 0.6.9
 **/
std::ostream&
operator<<(std::ostream& ostr, Unsigned const& u)
{
  if (u == UINT_MAX)
    ostr << '-';
  else
    ostr << static_cast<unsigned const&>(u);
  return ostr;
}

/**
 ** reads an 'Unsigned' from the stream
 ** a '-' is interpreted as 'UINT_MAX'
 **
 ** @param	istr	stream to read from
 ** @param	u	unsigned to read
 **
 ** @return	input stream
 **
 ** @author	Diether Knof
 **
 ** @version 0.6.9
 **/
std::istream&
operator>>(std::istream& istr, Unsigned& u)
{
  while (istr.good()
         && std::isspace(istr.peek()))
    istr.get();
  if (istr.peek() == '-') {
    u = UINT_MAX;
    istr.get();
  } else {
    istr >> static_cast<unsigned&>(u);
  }
  return istr;
}
