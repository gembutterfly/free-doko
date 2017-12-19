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

#include "aiDb.h"

#include <sstream>

/**
 **
 ** -> result
 **
 ** @param	-
 **
 ** @return	a number for the group 'general'
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.6
 **
 **/
unsigned
AiDb::heuristics_group_general() const
{
  unsigned num = 0;

  for (int h = Aiconfig::HEURISTIC_FIRST; h <= Aiconfig::HEURISTIC_LAST; ++h)
    if (Aiconfig::is_general(static_cast<Aiconfig::Heuristic>(h)))
      num += this->heuristic(static_cast<Aiconfig::Heuristic>(h));

  return num;
} // unsigned AiDb::heuristics_group_general() const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	a number for the group 'poverty'
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.2
 **/
unsigned
AiDb::heuristics_group_poverty() const
{
  unsigned num = 0;

  for (int h = Aiconfig::HEURISTIC_FIRST; h <= Aiconfig::HEURISTIC_LAST; ++h)
    if (Aiconfig::is_poverty(static_cast<Aiconfig::Heuristic>(h)))
      num += this->heuristic(static_cast<Aiconfig::Heuristic>(h));

  return num;
} // unsigned AiDb::heuristics_group_poverty() const

/**
 **
 ** -> result
 **
 ** @param	-
 **
 ** @return	a number for the group 'solo'
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.6
 **
 **/
unsigned
AiDb::heuristics_group_solo() const
{
  unsigned num = 0;

  for (int h = Aiconfig::HEURISTIC_FIRST; h <= Aiconfig::HEURISTIC_LAST; ++h)
    if (Aiconfig::is_solo(static_cast<Aiconfig::Heuristic>(h)))
      num += this->heuristic(static_cast<Aiconfig::Heuristic>(h));

  return num;
} // unsigned AiDb::heuristics_group_solo() const

/**
 **
 ** -> result
 **
 ** @param	-
 **
 ** @return	a number for the group 'solo color'
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.6
 **
 **/
unsigned
AiDb::heuristics_group_solo_color() const
{
  unsigned num = 0;

  for (int h = Aiconfig::HEURISTIC_FIRST; h <= Aiconfig::HEURISTIC_LAST; ++h)
    if (Aiconfig::is_solo_color(static_cast<Aiconfig::Heuristic>(h)))
      num += this->heuristic(static_cast<Aiconfig::Heuristic>(h));

  return num;
} // unsigned AiDb::heuristics_group_solo_color() const

/**
 **
 ** -> result
 **
 ** @param	-
 **
 ** @return	a number for the group 'solo picture'
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.6
 **
 **/
unsigned
AiDb::heuristics_group_solo_picture() const
{
  unsigned num = 0;

  for (int h = Aiconfig::HEURISTIC_FIRST; h <= Aiconfig::HEURISTIC_LAST; ++h)
    if (Aiconfig::is_solo_picture(static_cast<Aiconfig::Heuristic>(h)))
      num += this->heuristic(static_cast<Aiconfig::Heuristic>(h));

  return num;
} // unsigned AiDb::heuristics_group_solo_picture() const

/**
 **
 ** -> result
 **
 ** @param	-
 **
 ** @return	a number for the group 'solo meatless'
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.6
 **
 **/
unsigned
AiDb::heuristics_group_solo_meatless() const
{
  unsigned num = 0;

  for (int h = Aiconfig::HEURISTIC_FIRST; h <= Aiconfig::HEURISTIC_LAST; ++h)
    if (Aiconfig::is_solo_meatless(static_cast<Aiconfig::Heuristic>(h)))
      num += this->heuristic(static_cast<Aiconfig::Heuristic>(h));

  return num;
} // unsigned AiDb::heuristics_group_solo_meatless() const

/**
 **
 ** writes the database to 'ostr'
 **
 ** @param	ostr	output stream
 **
 ** @return	output stream
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.6
 **
 **/
ostream&
AiDb::write(ostream& ostr) const
{
  this->PlayersDb::write(ostr);

  ostr << '\n';

  ostr << "heuristics\n"
    << "{\n";
  for (int h = Aiconfig::HEURISTIC_FIRST; h <= Aiconfig::HEURISTIC_LAST; ++h)
    ostr << setw(32) << ::name(static_cast<Aiconfig::Heuristic>(h)) << " =\t"
      << this->heuristic(static_cast<Aiconfig::Heuristic>(h)) << '\n';
  ostr <<"}\n";

  return ostr;
} // ostream& AiDb::write(ostream& ostr) const

/**
 **
 ** reads the group 'group' from 'istr'
 **
 ** @param	group	name of the group to read
 ** @param	istr	stream with the group info
 **
 ** @return	whether the group 'group' is known
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.6
 **
 **/
bool
AiDb::read_group(string const& group, istream& istr)
{
  if (group == "heuristics")
    this->read_group_heuristics(istr);
#ifndef OUTDATED
  // 0.7.4   2006-09-14
  else if (group == "Heuristics")
    this->read_group_heuristics(istr);
#endif
  else
    return this->PlayersDb::read_group(group, istr);

  return true;
} // bool AiDb::read_group(string const& group, istream& istr)

/**
 **
 ** reads the heuristics group from 'istr'
 **
 ** @param	istr	stream with the heuristics info
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.6
 **
 **/
void
AiDb::read_group_heuristics(istream& istr)
{
  unsigned depth = 0;

  // load the configuration
  while (istr.good()) {
    Config config;
    istr >> config;

    if (config.separator) {
      int h;
      for (h = Aiconfig::HEURISTIC_FIRST; h <= Aiconfig::HEURISTIC_LAST; ++h)
	if (config.name == ::name(static_cast<Aiconfig::Heuristic>(h)))
          break;
#ifdef WORKAROUND
      // name till 0.7.5
      if (config.name == "color: choose ace")
        h = Aiconfig::CHOOSE_ACE;
#endif
      if (h <= Aiconfig::HEURISTIC_LAST) {
        istringstream istr_value(config.value);
        istr_value >> this->heuristics_[h];
      } else {
        cerr << "ai database: group 'heuristics': "
          << "unknown type '" << config.name << "'."
          << endl;
      }
    } else { // if (config.separator)
      if(config.name == "{") {
        depth += 1;
      } else if(config.name == "}") {
        if (depth == 0) {
          cerr << "ai database: group 'heuristics': "
            << "found a '}' without a '{' before.\n"
            << "Finish reading the the file."
            << endl;
          break;
        } // if (depth == 0)
        depth -= 1;
        if (depth == 0)
          break;
      } else if(config.name == "") {
        cerr << "ai database: group 'heuristics': "
          << "Ignoring line \'" << config.value << "\'.\n";
      } else {
        cerr << "ai database: group 'heuristics': "
          << "type '" << config.name << "' unknown.\n"
          << "Ignoring it.\n";
      } // if (config.name == .)
    } // config.separator
  } // while (istr.good())

  return ;
} // void AiDb::read_group_heuristics(istream& istr)

/**
 **
 ** clears the database
 **
 ** @param	-
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.6
 **
 **/
void
AiDb::clear()
{
  this->heuristics_ = vector<unsigned>(Aiconfig::HEURISTIC_LAST + 1);
  this->PlayersDb::clear();

  return ;
} // void AiDb::clear()
