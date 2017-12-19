// a function to read entries out of a configuration file

#ifndef READCONFIG_HEADER
#define READCONFIG_HEADER

#include <string>
#include <iosfwd>

struct Config {
  bool separator;	// whether there was a separator
  std::string name;	// the name of the entry
  std::string value;	// the value of the entry
}; // struct Config

// writes the next config of 'istr' in 'config'
// * blank lines and lines beginning with '#' are ignored
// * the separator is '=':
//     the left is the name, the right is the value
// * if no '=' is in the line the first nonblank character is looked at:
//   - if it is a character, the whole line is the value
//   - if it is not a character, the first word is the name, the rest the value
std::istream& operator>>(std::istream& istr, Config& config);
Config Config_(std::string const& line);

std::ostream& operator<<(std::ostream& ostr, Config const& config);

#endif // #ifndef READCONFIG_HEADER
