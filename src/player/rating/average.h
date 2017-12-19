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

#ifndef CLASS_RATING_AVERAGE_HEADER
#define CLASS_RATING_AVERAGE_HEADER

#include "rating.h"

/**
 ** average rater
 ** Returns the average value of the given values.
 **/
class Rating::Average : public Rating
{
  public:
    // constructor
    Average() : Rating(AVERAGE) { }
    // destructor
    ~Average() { }

    // returns the rated value
    int value() const
    {
      if (this->values.empty())
        return INT_MIN;

      int sum = 0;
      for (vector<int>::const_iterator v = this->values.begin();
           v != this->values.end();
           ++v)
        sum += *v;

      return sum / this->values.size();
    } // int value() const

  private:
}; // class Rating::Average : public Rating

#endif // #ifndef CLASS_RATING_AVERAGE_HEADER
