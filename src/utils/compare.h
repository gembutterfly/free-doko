//////////////////////////////////////////////////////////////////////////////
//
//   Copyright (C) 2002  by Diether Knof
//
//   This program is free software; you can redistribute it and/or 
//   modify it under the terms of the GNU General Public License as 
//   published by the Free Software Foundation; either version 2 of 
//   the License, or (at your option) any later version.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details. 
//   You can find this license in the file 'gpl.txt'.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 59 Temple Place, Suite 330, Boston, 
//   MA  02111-1307  USA
//
//  Contact:
//    Diether Knof dknof@gmx.de
//
//////////////////////////////////////////////////////////////////////////////

#ifndef DK_UTILS_COMPARE_HEADER
#define DK_UTILS_COMPARE_HEADER

// A macro that generates the comparisons '!=', '>', '<=' and '>=' from
// the defined comparisons '==' and '<'.
// The order must be strict (so either '<', '==' or '>')
//
// call: CREATE_COMPARE_FUNCTIONS(Type)

#define CREATE_COMPARE_FUNCTIONS(Type) \
  inline bool operator!=(Type const& t1, Type const& t2) \
    { return !(t1 == t2); } \
  inline bool operator> (Type const& t1, Type const& t2) \
    { return (t2 < t1); } \
  inline bool operator<=(Type const& t1, Type const& t2) \
    { return !(t1 > t2); } \
  inline bool operator>=(Type const& t1, Type const& t2) \
    { return !(t1 < t2); }

#endif // #ifndef DK_UTILS_COMPARE_HEADER
