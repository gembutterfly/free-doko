#include <iostream>

#include "getopt.h"

using std::cout;
using std::cerr;
using std::endl;

#define bool int
#define true 1
#define false 0

char const* const usage = ("usage: PROGRAM -[h]\n"
			   "\t--bool\t-b\tsets the bool-variable to true\n"
			   "\t--char\t-c\tsets the char-variable\n"
			   "\t--double\t-d\tsets the double-variable\n"
			   "\t--float\t-f\tsets the float-variable\n"
			   "\t--int\t-i\tsets the int-variable\n"
			   "\t--strn\t-s\tsets the string-variable\n");

int
main(int argc, char* argv[])
{

  cout << argc << " arguments:\n";
  for(int j = 0; j < argc; j++)
    cout << '\t' << j << ". " << argv[j] << '\n';
  cout << endl;

  GetOpt::Option option;
  while (option = getopt(argc, argv,
			 "help", 'h', GetOpt::Syntax::BOOL,
			 "version", 'v', GetOpt::Syntax::BOOL,
			 "usage", 'U', GetOpt::Syntax::BOOL,
			 "bool", 'b', GetOpt::Syntax::BOOL,
			 "char", 'c', GetOpt::Syntax::CHAR,
			 "double", 'd', GetOpt::Syntax::DOUBLE,
			 "int", 'i', GetOpt::Syntax::INT,
			 "strn", 's', GetOpt::Syntax::BSTRING,
			 "unsigned", 'u', GetOpt::Syntax::UNSIGNED,
			 "", '\0', GetOpt::Syntax::END)) {

    if (option.fail()) {
      // wrong usage
      cerr << "\nwrong usage: "
	<< option.error() << " " << option.value_string() << "\n\n"
	<< usage
	<< endl;
      return 1;
    } // if (option.fail())

    if (option.name() == "help") {
      cout << "This is the help." << endl;
    } else if (option.name() == "version") {
      cout << "This is the version." << endl;
    } else if (option.name() == "usage") {
      cout << usage << endl;
    } else if (option.name() == "bool") {
      cout << "bool:\t" << option.value(GetOpt::Option::BOOL) << endl;
    } else if (option.name() == "char") {
      cout << "char:\t" << option.value(GetOpt::Option::CHAR) << endl;
    } else if (option.name() == "double") {
      cout << "double:\t" << option.value(GetOpt::Option::DOUBLE) << endl;
    } else if (option.name() == "int") {
      cout << "int:\t" << option.value(GetOpt::Option::INT) << endl;
    } else if (option.name() == "strn") {
      cout << "string:\t" << option.value(GetOpt::Option::BSTRING) << endl;
    } else if (option.name() == "unsigned") {
      cout << "unsigned:\t" << option.value(GetOpt::Option::UNSIGNED) << endl;
    } else if (option.name() == "") {
      cout << "extra:\t" << option.value(GetOpt::Option::BSTRING) << endl;
    } // if (option.name() == ...)
  } // while (getopt())

  return 0;
} // main()
