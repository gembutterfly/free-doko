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

#include <string.h>

#include <stdarg.h>

using namespace GetOpt;

// remove the argument 'n'
void remove_argument(int& argc, char* argv[], unsigned const n = 1);
// remove 'i' argument characters at position  '[n1][n2]'
// default: 'program -asdf' -> 'program -sdf'
void remove_argument_chars(int& argc, char* argv[], unsigned const i = 1,
			   unsigned const n1 = 1,  unsigned const n2 = 1);

/**********************************************************************
 *
 * Option getopt(int& argc, char* argv[],
 * 		  char const* const name, char short_name, Syntax::Type type,
 * 		 ...);
 *
 * Parameters:	argc - the number of arguments
 *		argv - the arguments
 *		name - the (long) name of the option
 *		short_name - the short name of the option
 *		type - the type of the option
 *		... - tripel 'name, short_name, type',
 *		      ended by 'type == Syntax::END'
 *
 * Result:	the next option
 *
 * Description:	gets the next option and removes it from the arguments
 *
 **********************************************************************/
Option
GetOpt::getopt(int& argc, char* argv[],
	       char const* const name, char short_name, Syntax::Type type,
	       ...)
{
  std::vector<Syntax> syntax;

  // put the arguments in a syntax list

  Syntax s;
  // reads the first syntax
  s.name = name;
  s.short_name = short_name;
  s.type = type;

  va_list ap;

  va_start(ap, type);

  // read all other syntax
  while(s.type != Syntax::END) {
    // adds the option-setting
    syntax.push_back(s);

    // reads the next syntax
    s.name = std::string(va_arg(ap, char const*)); // (memory leak?)
    s.short_name = char(va_arg(ap, int));
    s.type = Syntax::Type(va_arg(ap, int));
  } // while(true)

  va_end(ap);

  return getopt(argc, argv, syntax);
} // Option getopt(int& argc, char* argv[],
//               char const* const name, char short_name, Syntax::Type type,
//               ...)

/**********************************************************************
 *
 * Option getopt(int& argc, char* argv[], std::vector<Syntax> const& syntax)
 *
 * Parameters:	argc - the number of arguments
 *		argv - the arguments
 *		syntax - the syntax of the arguments
 *
 * Result:	the next option
 *
 * Description:	gets the next option and removes it from the arguments
 *
 **********************************************************************/
Option
GetOpt::getopt(int& argc, char* argv[], std::vector<Syntax> const& syntax)
{
  Option option;

  if (argc == 0) {
    // error -- there must be one argument
    option.error_v = Option::NO_ARGUMENT;
    return option;
  } else if ((argc == 1)
	     || ((argc == 2)
		 && (strncmp(argv[1], "--", 3) == 0)
		)
	    ) {
    // no remaining argument (or only '--')
    option.error_v = Option::NO_OPTION;
    return option;
  } else { // if (argc > 1)
    if ((argv[1][0] != '-')
	|| (argv[1][1] == '\0')) {
      // a sole string
      option.error_v = Option::OK;
      option.name_v = "";
      option.type_v = Syntax::BSTRING;
      option.value_string_v = argv[1];

      remove_argument(argc, argv);

      return option;
    } // if (argv[1][0] != '-')
    else if (strncmp(argv[1], "--", 3) == 0) {
      // a sole string on the second position
      option.error_v = Option::OK;
      option.name_v = "";
      option.type_v = Syntax::BSTRING;
      option.value_string_v = argv[2];

      remove_argument(argc, argv, 2);

      return option;
    } // if (strncmp(argv[1], "--", 3) == 0)
    else if (strncmp(argv[1], "--", 2) == 0) {
      // long option
      option.value_string_v = argv[1];
      option.error_v = Option::UNKNOWN_OPTION;
      // ***DK use Iterator
      for (unsigned n = 0; n < syntax.size(); n++) {
	// search the option
	if (strncmp(argv[1] + 2, syntax[n].name.c_str(),
		    syntax[n].name.length()) == 0) {

	  option.type_v = syntax[n].type;
	  option.name_v = syntax[n].name;

	  if (option.type() == Syntax::BOOL) {
	    if (argv[1][2 + syntax[n].name.size()] == '\0') {
	      option.error_v = Option::OK;
	      option.value_v.b = true;
	      remove_argument(argc, argv);

	      return option;
	    } // if (argument = '--name'

	  } else { // if !(option.type() == Syntax::BOOL)

	    if (argv[1][2 + syntax[n].name.size()] == '\0') {
	      // only the name of the option is in this argument,
	      // the value (if any) is in the next
	      remove_argument(argc, argv);

	      // test, whether there is a remaining argument
	      if (argc == 1) {
		option.error_v = Option::NO_ARGUMENT;

		return option;

	      } else { // if !(argc == 1)
		// the value of the option is now the argument number 1
		(option.value_string_v += " ") += argv[1];
		option.value_set(argv[1]);
		if (option.error() == Option::OK)
		  remove_argument(argc, argv);

		return option;

	      } // if !(argc == 1)
	    } // if (value in next argument)
	    else if (argv[1][2 + syntax[n].name.size()] == '=') {

	      option.value_set(argv[1] + 2 + syntax[n].name.length() + 1);
	      if (option.error() == Option::OK)
		remove_argument(argc, argv);

	      return option;

	    } // if (argument = '--name=value')

	  } // if !(option.type() == Syntax::BOOL)

	} // if (strncmp(argv[1] + 2, syntax, ) == 0)
      } // for (n < syntax.size())

    } else { // if !(long option)

      // long option
      option.value_string_v = argv[1];
      option.error_v = Option::UNKNOWN_OPTION;

      // short option
      // ***DK use Iterator
      for (unsigned n = 0; n < syntax.size(); n++) {
	// search the option
	if (argv[1][1] == syntax[n].short_name) {

	  option.type_v = syntax[n].type;
	  option.name_v = syntax[n].name;

	  if (option.type() == Syntax::BOOL) {
	    // BOOL
	    option.error_v = Option::OK;
	    option.value_v.b = true;
	    remove_argument_chars(argc, argv);
	    if (argv[1][1] == '\0') // argument is empty
	      remove_argument(argc, argv);

	    return option;

	  } else { // if !(option.type() == Syntax::BOOL)

	    if (argv[1][2] == '\0') {
	      // the value is in the next argument
	      remove_argument(argc, argv);
	      (option.value_string_v += " ") += argv[1];
	      option.value_set(argv[1]);
	      if (option.error() == Option::OK)
		remove_argument(argc, argv);

	      return option;

	    } else { // if !(argv[1][2] == '\0')

	      // the value is in this argument
	      if ((option.type() == Syntax::CHAR)
		  && !(argv[1][2] == '=')) {
		// CHAR
		option.error_v = Option::OK;
		option.value_v.c = argv[1][2];
		remove_argument_chars(argc, argv, 2);
		if (argv[1][1] == '\0') // argument is empty
		  remove_argument(argc, argv);

		return option;

	      } else { // if !(option.type() == Syntax::BOOL)
		// an '=' is ignored
		option.value_set(argv[1] + 2 +
				 (argv[1][2] == '=' ? 1 : 0));
		if (option.error() == Option::OK)
		  remove_argument(argc, argv);

		return option;

	      } // if !(option.type() == Syntax::BOOL)

	    } // if (value in this argument)

	  } // if !(option.type() == Syntax::BOOL)
	} // if (argv[1][1] == syntax[n].short_name)
      } // for (n < syntax.size())

    } // if (short option)

  } // if !(argc > 1)

  return option;
} // Option getopt(int& argc, char* argv[], std::vector<Syntax> const& syntax)

/**********************************************************************
 *
 * void remove_argument(int& argc, char* argv[], unsigned const n = 1)
 *
 * Parameters:	argc - the number of arguments
 *		argv - the arguments
 *		n - the argument to be removed
 *
 * Result:	-
 *
 * Description:	remove the 'n'th argument from argc and argv
 *
 **********************************************************************/
void
remove_argument(int& argc, char* argv[], unsigned const n)
{
  if (n >= unsigned(argc))
    return; // nothing to do (error)

  for (unsigned i = n; i < unsigned(argc) - 1; i++)
    argv[i] = argv[i + 1];

  argc -= 1;

  return;
} // void remove_argument(int& argc, char* argv[], unsigned const n = 1)

// remove 'i' argument characters at position  '[n1][n2]'
// default: 'program -asdf' -> 'program -sdf'
/**********************************************************************
 *
 * void remove_argument_chars(int& argc, char* argv[], unsigned const i = 1,
 *			      unsigned const n1 = 1,  unsigned const n2 = 1)
 *
 * Parameters:	argc - the number of arguments
 *		argv - the arguments
 *		i - the number of characters to remove
 *		n1 - the argument number of the character(s)
 *		n2 - the position in the argument of the character(s)
 *
 * Result:	-
 *
 * Description:	removes 'i' characters at the position argv[n1][n2]
 *
 **********************************************************************/
void
remove_argument_chars(int& argc, char* argv[], unsigned const i,
		      unsigned const n1,  unsigned const n2)
{
  if (n1 >= unsigned(argc))
    return; // nothing to do (error)

  if (n2 + i >= strlen(argv[n1])) {
    // the last argument
    argv[n1][n2] = '\0';
    return;
  }

  strcpy(argv[n1] + n2, argv[n1] + n2 + i);

  return;
} // void remove_argument_chars(int& argc, char* argv[], i = 1, n1 = 1, n2 = 1)
