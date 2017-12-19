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

#ifndef CLASS_RATING_MEDIAN_HEADER
#define CLASS_RATING_MEDIAN_HEADER

#include "rating.h"

/**
 ** median rater
 ** Returns the median value of the given values.
 **/
class Rating::Median : public Rating
{
  public:
    // constructor
    Median() : Rating(MEDIAN), values() { }
    // destructor
    ~Median() { }

    // returns the rated value
    int value() const
    {
      if (this->values.empty())
	return INT_MIN;
      this->sort();
      if (this->values.size() % 2 == 0)
	return (this->values[this->values.size() / 2]
		+ this->values[this->values.size() / 2 + 1]);
      else
	return this->values[this->values.size() / 2];
    } // int value() const

  private:
    // all given values
    mutable vector<int> values;

}; // class Rating::Median : public Rating

#endif // #ifndef CLASS_RATING_MEDIAN_HEADER
