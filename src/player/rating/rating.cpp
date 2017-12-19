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
#include "rating.h"
#include "ratings.h"

/**
 ** constructor
 **
 ** @param     type   rating type
 **
 ** @return    -
 **/
Rating::Rating(Type const type) :
  type_(type)
{ }

/**
 ** -> result
 **
 ** @param     type   the rating type to return
 **
 ** @return    new rating of type 'type' (to be freed)
 **/
Rating*
Rating::new_(Type const type)
{
  switch(type) {
  case MAX:
    return new Max();
  case MIN:
    return new Min();
  case SECOND_MAX:
    return new SecondMax();
  case SECOND_MIN:
    return new SecondMin();
  case MEDIAN:
    return new Median();
  case AVERAGE:
    return new Average();
  case AVERAGE_PHYSICAL:
    return new AveragePhysical();
  case LINEAR:
    return new Linear();
  case LINEAR_REVERSE:
    return new LinearReverse();
  } // switch(type)
  return NULL;
} // Rating* Rating::new_(Type const type)

/**
 ** -> result
 **
 ** @param     type   the rating type to return
 **
 ** @return    new rating of type 'type' (to be freed)
 **/
unique_ptr<Rating>
Rating::create(Type const type)
{
  switch(type) {
  case MAX:
    return make_unique<Max>();
  case MIN:
    return make_unique<Min>();
  case SECOND_MAX:
    return make_unique<SecondMax>();
  case SECOND_MIN:
    return make_unique<SecondMin>();
  case MEDIAN:
    return make_unique<Median>();
  case AVERAGE:
    return make_unique<Average>();
  case AVERAGE_PHYSICAL:
    return make_unique<AveragePhysical>();
  case LINEAR:
    return make_unique<Linear>();
  case LINEAR_REVERSE:
    return make_unique<LinearReverse>();
  } // switch(type)
  return {};
} // unique_ptr<Rating> Rating::create(Type type)

/**
 ** write the rating in the stream
 **
 ** @param     ostr     stream to write into
 ** @param     rating   rating to write
 **
 ** @return    -
 **/
ostream&
operator<<(ostream& ostr, Rating const& rating)
{
  rating.write(ostr);
  return ostr;
} // ostream& operator<<(ostream& ostr, Rating rating)

/**
 ** write the rating in the stream
 **
 ** @param     ostr  stream to write into
 **
 ** @return    -
 **/
void
Rating::write(ostream& ostr) const
{
  ostr << ::name(this->type()) << '\n';
  for (auto const& i : this->values)
    ostr << "  " << i << '\n';
} // void Rating::write(ostream& ostr) const

/**
 ** add the value
 **
 ** @param     value   value to add
 **
 ** @return    -
 **/
void
Rating::add(int const value)
{
  this->values.push_back(value);
} // void Rating::add(int value)

/**
 ** deletes the worst 'n' arguments
 **
 ** @param     n   number of arguments to ignore
 **
 ** @return    -
 **/
void
Rating::delete_worst(size_t const n)
{
  if (this->values.size() <= n) {
    this->values.clear();
    return ;
  }
  this->sort();
  this->values.erase(this->values.begin(), this->values.begin() + n);
} // void Rating::delete_worst(size_t n)

/**
 ** sorts the values
 **
 ** @param     -
 **
 ** @return    -
 **/
void
Rating::sort() const
{
  std::sort(this->values.begin(), this->values.end());
} // void Rating::sort() const

/**
 ** -> result
 **
 ** @param     type   rating type
 **
 ** @return    name of the rating type
 **/
string
name(Rating::Type const type)
{
  switch(type) {
  case Rating::MAX:
    return "maximum";
  case Rating::MIN:
    return "minimum";
  case Rating::SECOND_MAX:
    return "second maximum";
  case Rating::SECOND_MIN:
    return "second minimum";
  case Rating::MEDIAN:
    return "median";
  case Rating::AVERAGE:
    return "average";
  case Rating::AVERAGE_PHYSICAL:
    return "physical average";
  case Rating::LINEAR:
    return "linear";
  case Rating::LINEAR_REVERSE:
    return "linear reverse";
  } // switch(type)
  return {};
} // string name(Rating::Type type)

/**
 ** write the type in the output stream
 **
 ** @param    ostr   output stream
 ** @param    type   rating type
 **
 ** @return   output stream
 **/
ostream&
operator<<(ostream& ostr, Rating::Type const type)
{ return (ostr << ::name(type)); }

/**
 ** -> result
 **
 ** @param	name   name of the type
 **
 ** @return	the rating type
 **/
Rating::Type
Rating::type_from_name(string const& name)
{
  for (auto r : RatingTypeALL)
    if (::name(r) == name)
      return r;
  throw name;
  return Rating::MAX;
} // Rating::Type type_from_name(string name)

