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

#ifndef CLASS_RATING_LINEAR_REVERSE_HEADER
#define CLASS_RATING_LINEAR_REVERSE_HEADER

#include "rating.h"

/**
 ** reverse linear rater
 ** Returns the reverse linear value calculated by the given values.
 ** 2/(n*(n+1)) * \\Sum_i=1^n (n - i) * value[i]
 **/
class Rating::LinearReverse : public Rating
{
  public:
    // constructor
    LinearReverse() : Rating(LINEAR_REVERSE) { }
    // destructor
    ~LinearReverse() { }

    // returns the rated value
    int value() const
    {
      if (this->values.empty())
	return INT_MIN;
      this->sort();
      int v = 0;
      for (int i = 0; i < static_cast<int>(this->values.size()); ++i)
	v += (this->values.size() - i) * this->values[i];
      return v / static_cast<int>( (this->values.size()
				    * (this->values.size() + 1)
				   ) / 2);
    } // int value() const

  private:

}; // class Rating::LinearReverse : public Rating

#endif // #ifndef CLASS_RATING_LINEAR_REVERSE_HEADER
