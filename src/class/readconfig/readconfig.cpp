#include "readconfig.h"
#include <istream>
#include <iostream>
#include <sstream>
#include <ctype.h>
#include "../../utils/string.h"
using std::string;

// writes the next config of 'istr' in 'config'
std::istream&
operator>>(std::istream& istr, Config& config)
{
  string line;

  line = DK::Utils::String::getline_escape(istr);
  if (!istr.good() && (line.length() == 0)) {
    config.separator = false;
    config.name = "";
    config.value = "";
    return istr;
  }

  // look, if the line is empty or a remark
  DK::Utils::String::remove_blanks(line);
  if (   (line.size() == 0)
      || (static_cast<string const&>(line)[0] == '#')) // empty line or remark
    return (istr >> config);

  // look, if there is an '=' in the line
  string::size_type pos = line.find('=');
  if (pos == string::npos) {
    // no '='
    config.separator = false;
    if (isalpha(static_cast<string const&>(line)[0])) {
      // the whole line is the name
      config.name = line;
      config.value = "";
    } else { // if !(isalpha(static_cast<string const&>(line)[0]))
      // the first word is the name, the rest of the line the value
      for (pos = 0; pos < line.length(); pos++)
	if (isspace(static_cast<string const&>(line)[pos]))
	  break;
      config.name = string(line, 0, pos);
      if (pos < line.length()) {
	config.value = string(line, pos + 1, std::string::npos);
      } else {
	config.value = "";
      }
    } // if !(isalpha(static_cast<string const&>(line)[0]))
  } else { // if !('=' in 'line')
    config.separator = true;
    config.name = string(line, 0, pos);
    config.value = string(line, pos + 1, std::string::npos);
  } // if !('=' in 'line')

  DK::Utils::String::remove_blanks(config.value);
  DK::Utils::String::remove_blanks(config.name);

  return istr;
} // std::istream& operator>>(istream& istr, Config& config)

// read the configuration from the line
Config
Config_(std::string const& line)
{
  Config config;
  std::istringstream istr(line);

  istr >> config;

  return config;
} // Config Config_(string const& line)

// writes 'config' in 'ostr'
std::ostream&
operator<<(std::ostream& ostr, Config const& config)
{
  if (config.separator)
    ostr << config.name << " = " << config.value;
  else
    ostr << config.name << " " << config.value;

  return ostr;
} // ostream& operator<<(ostream& ostr, config)
