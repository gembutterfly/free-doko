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

// functions for getting the options

#ifndef GETOPT_HEADER
#define GETOPT_HEADER

#include <string>
#include <vector>

namespace GetOpt {

  struct Syntax {
    enum Type {
      BOOL,
      INT,
      UNSIGNED,
      DOUBLE,
      CHAR,
      BSTRING,
      END
    }; // enum Type

    std::string name; // the (long) name of the option
    char short_name; // the short form of the option
    Type type; // the type of the option
  }; // struct Syntax

  class Option {
    friend Option getopt(int& argc, char* argv[],
			 std::vector<Syntax> const& syntax);
    public:
    // the error code
    enum Error {
      OK,
      NO_OPTION,
      UNKNOWN_OPTION,
      FALSE_ARGUMENT,
      NO_ARGUMENT,
      UNKNOWN_ERROR
    }; // enum Error
    // different option types
    enum TypeBool { BOOL };
    enum TypeInt { INT = BOOL + 1 };
    enum TypeUnsigned { UNSIGNED = INT + 1 };
    enum TypeDouble { DOUBLE = UNSIGNED + 1 };
    enum TypeChar { CHAR = DOUBLE + 1 };
    // could not use 'STRING', 'B' stands for 'basic'
    enum Typestring { BSTRING = CHAR + 1 };
    public:
    // constructor
    Option();
    // constructor
    Option(Option const& option);
    // setting equal
    Option& operator=(Option const& option);

    // desctuctor
    ~Option();

    // the error code
    Error error() const;
    // wether the parsing has failed
    bool fail() const;
    // whether an option could be parsed (with or without an error)
    operator bool() const;

    // the name of the option
    std::string const& name() const;
    // the Type of the option
    Syntax::Type type() const;

    // the boolean value
    bool value(TypeBool) const;
    // the int value
    int value(TypeInt) const;
    // the unsigned value
    unsigned value(TypeUnsigned) const;
    // the double value
    double value(TypeDouble) const;
    // the char value
    char value(TypeChar) const;
    // the string value
    std::string const& value(Typestring) const;
    // the string value
    std::string const& value_string() const;

    // set the value
    Error value_set(char const* argv);

    // ***  protected:
    public:
    Error error_v; // the errorcode
    std::string name_v; // the name of the option
    Syntax::Type type_v; // the type
    union {
      bool b;
      int i;
      unsigned u;
      double d;
      char c;
    } value_v; // the value of the option
    std::string value_string_v; // could not put this in the union
  }; // class Option

  // get the next option
  Option getopt(int& argc, char* argv[],
		char const* const name, char short_name, Syntax::Type type,
		...);
  // get the next option
  Option getopt(int& argc, char* argv[], std::vector<Syntax> const& syntax);

} // namepsace GetOpt

// output of the error
std::ostream& operator<<(std::ostream& ostr, GetOpt::Option::Error const error);


#endif
