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

#include "getopt.h"

#include <iostream>
#include <cstdlib>
#include <climits>

using namespace GetOpt;

/**********************************************************************
 *
 * Option::Option()
 *
 * Parameters:	-
 *
 * Result:	-
 *
 * Description:	Constructor
 *
 **********************************************************************/
Option::Option() :
error_v(OK),
  name_v(),
  type_v(Syntax::BOOL),
  value_v(),
value_string_v()
{
  return ;
} // Option::Option()

/**********************************************************************
 *
 * Option::Option(Option const& option)
 *
 * Parameters:	option - the other option
 *
 * Result:	-
 *
 * Description:	Constructor
 *
 **********************************************************************/
Option::Option(Option const& option) :
error_v(option.error()),
  name_v(option.name()),
  type_v(option.type()),
  value_v(),
value_string_v(option.value_string())
{
  switch(option.type()) {
  case Syntax::BOOL:
    this->value_v.b = option.value(BOOL);
    break;
  case Syntax::INT:
    this->value_v.i = option.value(INT);
    break;
  case Syntax::UNSIGNED:
    this->value_v.u = option.value(UNSIGNED);
    break;
  case Syntax::DOUBLE:
    this->value_v.d = option.value(DOUBLE);
    break;
  case Syntax::CHAR:
    this->value_v.c = option.value(CHAR);
    break;
  case Syntax::BSTRING:
    break;
  case Syntax::END:
    break;
  } // switch(option.type())

  return ;
} // Option::Option()

/**********************************************************************
 *
 * Option& Option::operator=(Option const& option)
 *
 * Parameters:	option - the other option
 *
 * Result:	-
 *
 * Description:	Constructor
 *
 **********************************************************************/
Option&
Option::operator=(Option const& option)
{
  this->error_v = option.error();
  this->name_v = option.name();
  this->value_string_v = option.value_string();

  switch(option.type()) {
  case Syntax::BOOL:
    this->value_v.b = option.value(BOOL);
    break;
  case Syntax::INT:
    this->value_v.i = option.value(INT);
    break;
  case Syntax::UNSIGNED:
    this->value_v.u = option.value(UNSIGNED);
    break;
  case Syntax::DOUBLE:
    this->value_v.d = option.value(DOUBLE);
    break;
  case Syntax::CHAR:
    this->value_v.c = option.value(CHAR);
    break;
  case Syntax::BSTRING:
    break;
  case Syntax::END:
    break;
  } // switch(option.type())

  return *this;
} // Option& Option::operator=(Option const& option)

/**********************************************************************
 *
 * Option::~Option()
 *
 * Parameters:	-
 *
 * Result:	-
 *
 * Description:	Destructor -- nothing to do
 *
 **********************************************************************/
Option::~Option()
{
  return ;
} // Option::~Option()

/**********************************************************************
 *
 * Option::Error Option::error() const
 *
 * Parameters:	-
 *
 * Result:	the error code
 *
 * Description:	-> result
 *
 **********************************************************************/
Option::Error
Option::error() const
{
  return this->error_v;
} // Option::Error Option::error() const

/**********************************************************************
 *
 * bool Option::fail() const
 *
 * Parameters:	-
 *
 * Result:	wether the parsing is failed
 *
 * Description:	-> result
 *
 **********************************************************************/
bool
Option::fail() const
{
  return ((this->error() != OK)
	  && (this->error() != NO_OPTION));
} // bool Option::fail() const

/**********************************************************************
 *
 * Option::operator bool() const
 *
 * Parameters:	-
 *
 * Result:	whether another option could be parsed
 *		(with or without an error)
 *
 * Description:	-> result
 *
 **********************************************************************/
Option::operator bool() const
{
  return (this->error() != NO_OPTION);
} // Option::operator bool() const


/**********************************************************************
 *
 * std::string const& Option::name() const
 *
 * Parameters:	-
 *
 * Result:	the name of the option
 *
 * Description:	-> result
 *
 **********************************************************************/
std::string const&
Option::name() const
{
  return this->name_v;
} // std::string const& Option::name() const

/**********************************************************************
 *
 * Syntax::Type Option::type() const
 *
 * Parameters:	-
 *
 * Result:	the type of the option
 *
 * Description:	-> result
 *
 **********************************************************************/
Syntax::Type
Option::type() const
{
  return this->type_v;
} // Syntax::Type Option::type() const

/**********************************************************************
 *
 * bool Option::value(TypeBool) const
 *
 * Parameters:	-
 *
 * Result:	the bool value
 *
 * Description:	-> result
 *
 **********************************************************************/
bool
Option::value(TypeBool) const
{
  return this->value_v.b;
} // bool Option::value(TypeBool) const

/**********************************************************************
 *
 * int Option::value(TypeInt) const
 *
 * Parameters:	-
 *
 * Result:	the int value
 *
 * Description:	-> result
 *
 **********************************************************************/
int
Option::value(TypeInt) const
{
  return this->value_v.i;
} // int Option::value(TypeInt) const

/**********************************************************************
 *
 * unsigned Option::value(TypeUnsigned) const
 *
 * Parameters:	-
 *
 * Result:	the unsigned value
 *
 * Description:	-> result
 *
 **********************************************************************/
unsigned
Option::value(TypeUnsigned) const
{
  return this->value_v.u;
} // unsigned Option::value(TypeUnsigned) const

/**********************************************************************
 *
 * double Option::value(TypeDouble) const
 *
 * Parameters:	-
 *
 * Result:	the double value
 *
 * Description:	-> result
 *
 **********************************************************************/
double
Option::value(TypeDouble) const
{
  return this->value_v.d;
} // double Option::value(TypeDouble) const

/**********************************************************************
 *
 * char Option::value(TypeChar) const
 *
 * Parameters:	-
 *
 * Result:	the char value
 *
 * Description:	-> result
 *
 **********************************************************************/
char
Option::value(TypeChar) const
{
  return this->value_v.b;
} // char Option::value(TypeChar) const

/**********************************************************************
 *
 * std::string const& Option::value(Typestring) const
 *
 * Parameters:	-
 *
 * Result:	the string value
 *
 * Description:	-> result
 *
 **********************************************************************/
std::string const&
Option::value(Typestring) const
{
  return this->value_string_v;
} // std::string const& Option::value(Typestring) const

/**********************************************************************
 *
 * std::string const& Option::value_string() const
 *
 * Parameters:	-
 *
 * Result:	the string value
 *
 * Description:	-> result
 *
 **********************************************************************/
std::string const&
Option::value_string() const
{
  return this->value_string_v;
} // std::string const& Option::value_string() const

/**********************************************************************
 *
 * Option::Error Option::value_set(char const* argv)
 *
 * Parameters:	argv - the argument with the valule
 *
 * Result:	the errorcode
 *
 * Description:	sets the value of the option
 *
 **********************************************************************/
Option::Error
Option::value_set(char const* argv)
{
  this->error_v = OK;

  switch(this->type()) {
  case Syntax::BOOL:
    // cannot be
    this->error_v = UNKNOWN_ERROR;
    break;
  case Syntax::INT:
    {
      char* endptr;
      this->value_v.i = strtol(argv, &endptr, 0);
      if ((endptr == argv)
	  || (*endptr != '\0'))
	this->error_v = FALSE_ARGUMENT;
      break;
    }
  case Syntax::UNSIGNED:
    {
      char* endptr;
      this->value_v.u = strtoul(argv, &endptr, 0);
      if ((endptr == argv)
	  || (*endptr != '\0')
	  || (this->value_v.u == UINT_MAX)
	  || (argv[0] == '-'))
	this->error_v = FALSE_ARGUMENT;
      break;
    }
  case Syntax::DOUBLE:
    {
      char* endptr;
      this->value_v.d = strtod(argv, &endptr);
      if ((endptr == argv)
	  || (*endptr != '\0'))
	this->error_v = FALSE_ARGUMENT;
      break;
    }
  case Syntax::CHAR:
    this->value_v.c = *argv;
    if (argv[1] != '\0')
      this->error_v = FALSE_ARGUMENT;
    break;
  case Syntax::BSTRING:
    this->value_string_v = argv;
    this->error_v = OK;
    break;
  case Syntax::END:
    // cannot be
    this->error_v = UNKNOWN_ERROR;
    break;
  }; // switch(this->type())

  return this->error();
} // Option::Error Option::value_set(char const* argv)

/**********************************************************************
 *
 * std::ostream& operator<<(std::ostream& ostr, const Option::Error error)
 *
 * Parameters:	ostr - the output stream
 *		error - the error
 *
 * Result:	the output stream
 *
 * Description:	output of the error to the stream
 *
 **********************************************************************/
std::ostream&
operator<<(std::ostream& ostr, Option::Error const error)
{
  switch(error) {
  case Option::OK:
    ostr << "ok";
    break;
  case Option::NO_OPTION:
    ostr << "no option";
    break;
  case Option::UNKNOWN_OPTION:
    ostr << "unknown option";
    break;
  case Option::FALSE_ARGUMENT:
    ostr << "false argument";
    break;
  case Option::NO_ARGUMENT:
    ostr << "no argument";
    break;
  case Option::UNKNOWN_ERROR:
    ostr << "unknown error";
    break;
  } // switch(error)

  return ostr;
} // std::ostream& operator<<(std::ostream& ostr, Option::Error const error)

