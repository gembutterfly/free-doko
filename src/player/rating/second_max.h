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

#ifndef CLASS_RATING_SECOND_MAX_HEADER
#define CLASS_RATING_SECOND_MAX_HEADER

#include "rating.h"

/**
 ** second maximum rater
 ** returns the second maximal value of the given values
 **/
class Rating::SecondMax : public Rating
{
  public:
    // constructor
    SecondMax() : Rating(SECOND_MAX) { }
    // destructor
    ~SecondMax() { }

    // returns the rated value
    int value() const
    { 
      if (this->values.empty())
        return INT_MIN;
      if (this->values.size() == 1)
        return *this->values.begin();
      this->sort();
      return *(this->values.rbegin() + 1);
    }

  private:
}; // class Rating::SecondMax : public Rating

#endif // #ifndef CLASS_RATING_SECOND_MAX_HEADER
