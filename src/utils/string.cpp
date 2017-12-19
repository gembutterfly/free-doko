/***************************************************************************
 *
 *   Copyright (C) 2002  by Diether Knof
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
 *
 **************************************************************************/

#include <ctype.h>

#include "string.h"

using std::string;

#include <istream>
#include <fstream>
using std::istream;

namespace DK {
  namespace Utils {
    namespace String {

      /**
       ** -> result
       **
       ** @param     s   the string
       **
       ** @return    the string without blanks at the start and end
       ** 
       ** @author    Diether Knof
       **
       ** @version   2006-07-23
       **/
      string
	remove_blanks(string const& s)
	{
	  string s2 = s;
	  return remove_blanks(s2);
	} // string remove_blanks(string s)

      /**
       **
       ** removes the blanks from the start and the end of the string
       **
       ** @param	s	the string which shall be changed
       **
       ** @return	the changed string
       ** 
       ** @author	Diether Knof
       **
       ** @version	2003-03-30
       **
       **/
      string&
	remove_blanks(string& s)
	{
	  if (s.empty())
	    return s;

	  // perhaps work with a substring
	  string::size_type pos_start;
	  string::size_type pos_end;

	  for (pos_start = 0; pos_start < s.length(); pos_start++)
	    if (!isspace(s[pos_start]))
	      break;
	  for (pos_end = s.length() - 1; pos_end > 0; --pos_end)
	    if (!isspace(s[pos_end]))
	      break;

	  return (s = string(s, pos_start, pos_end - pos_start + 1));
	} // string& remove_blanks(string& s)

      /**
       ** remove all trailing newlines
       **
       ** @param	s	the string which shall be changed
       **
       ** @return	the changed string
       ** 
       ** @author	Diether Knof
       **
       ** @version	2013-11-05
       **/
      string&
        remove_trailing_newlines(string& s)
        {
          while (   (s[s.size() - 1] == '\n')
                 || (s[s.size() - 1] == '\r') )
            s.resize(s.size() - 1);
          return s;
        } // string& remove_trailing_newlines(string& s)

      /**
       ** replace double newlines with one
       **
       ** @param	s	the string which shall be changed
       **
       ** @return	the changed string
       ** 
       ** @author	Diether Knof
       **
       ** @version	2013-11-05
       **/
      string&
        remove_double_newlines(string& s)
        {
          for (int i = s.size() - 2; i >= 0; --i) {
            if (   (s[i] == '\n')
                && (s[i+1] == '\n') ) {
              s.erase(i, 1);
            }
          } // for (i)
          for (int i = s.size() - 4; i >= 0; --i) {
            if (   (s[i] == '\n')
                && (s[i+1] == '\r')
                && (s[i+2] == '\n')
                && (s[i+3] == '\r') ) {
              s.erase(i, 2);
            }
          } // for (i)
          return s;
        } // string& remove_double_newlines(string& s)


      /**
       **
       ** returns the first word
       **
       ** @param	text	the text
       **
       ** @return	the first word
       ** 
       ** @author	Diether Knof
       **
       ** @version	2003-03-30
       **
       **/
      string
        word_first(string const& s)
        {
          // ignore first blanks
          string::const_iterator b; // start of the word
          for (b = s.begin(); b != s.end(); b++)
            if (!isspace(*b))
              break;

          // search the end of the word
          string::const_iterator e; // end of the word
          for (e = b; e != s.end(); e++)
            if (isspace(*e))
              break;

          return string(b, e);
        } // string word_first(string const& text)

      /**
       **
       ** removes the first word
       **
       ** @param	text	the text
       **
       ** @return	the remaining text
       ** 
       ** @author	Diether Knof
       **
       ** @version	2003-03-30
       **
       **/
      string&
        word_first_remove(string& s)
        {
          string::iterator c;

          // ignore first blanks
          for (c = s.begin(); c != s.end(); c++)
            if (!isspace(*c))
              break;

          // search the end of the word
          for ( ; c != s.end(); c++)
            if (isspace(*c))
              break;

          s.erase(s.begin(), c);

          return s;
        } // string& word_first_remove(string& text)

      /**
       **
       ** removes the first word with the following blanks
       **
       ** @param	text	the text
       **
       ** @return	the remaining text
       ** 
       ** @author	Diether Knof
       **
       ** @version	2003-09-10
       **
       **/
      string&
        word_first_remove_with_blanks(string& s)
        {
          word_first_remove(s);

          string::iterator c;

          // search the next non blank character
          for (c = s.begin(); c != s.end(); c++)
            if (!isspace(*c))
              break;

          s.erase(s.begin(), c);

          return s;
        } // string& word_first_remove_with_blanks(string& text)

      /**
       **
       ** replaces all 's' with 'replacement' in 'text'
       **
       ** @param	text		the string which shall be changed
       ** @param	s		the string which shall be replaced
       ** @param	replacement	the replacement for 's'
       **
       ** @return	the changed string
       ** 
       ** @author	Diether Knof
       **
       ** @version	2003-03-30
       **
       **/
      string&
        replace_all(string& text, string const& s, string const& replacement)
        {
          string::size_type pos = 0;

          while (pos != string::npos) {
            pos = text.find(s, pos);
            if (pos == string::npos)
              // no more 's' in '*this'
              break;

            text.replace(pos, s.length(), replacement);
            pos += replacement.length();
          } // while (pos < s.length())

          return text;
        } // string& replace_all(string& text, string const& s, string const& replacement)

      /**
       **
       ** gets a line and replaces the escape-sequences
       **
       ** @param	istr	the input stream
       **
       ** @return	the line
       ** 
       ** @author	Diether Knof
       **
       ** @version	2003-03-30
       **
       **/
      string
        get_till_eof(istream& istr)
        {
          string text;
          std::getline(istr, text, '\0');

          return text;
        } // string get_till_eof(istream& istr)

      /**
       **
       ** gets a line and replaces the escape-sequences
       **
       ** @param	istr	the input stream
       **
       ** @return	the line
       ** 
       ** @author	Diether Knof
       **
       ** @version	2003-03-30
       **
       **/
      string
        getfile(string const& filename)
        {
          std::ifstream istr(filename.c_str());

          if (istr.fail())
            return "";

          return get_till_eof(istr);
        } // string getfile(string const& filename)

      /**
       **
       ** gets a line and replaces the escape-sequences
       **
       ** @param	istr	the input stream
       **
       ** @return	the line
       ** 
       ** @author	Diether Knof
       **
       ** @version	2003-03-30
       **
       **/
      string
        getline_escape(istream& istr)
        {
          string line;

          // load the line(s)
          // (they can expand over more than one line, if the last character is a '\')
          do {
            string new_line;
            std::getline(istr, new_line);
            if (istr.fail() || new_line.empty())
              return line;

            if (!line.empty()) {
              // this is a following line, remove the '\\' from the last line ...
              line.erase(line.end() - 1);
              // ... and the first spaces
              while (!new_line.empty() && isspace(*(new_line.begin())))
                new_line.erase(new_line.begin());
            } // if (!line.empty())

            line += new_line;
            if (*(line.end() - 1) == '\r')
              line.erase(line.end() - 1);
          } while (!line.empty()
                   && (line[line.size() - 1] == '\\')
                   && !istr.eof()) ;

          // replace the escape-sequences
          string::size_type pos = 0;
          char c;
          while ((pos = line.find('\\', pos)) != string::npos) {
            c = line[pos + 1];
            switch (c) {
            case '\n': // newline (is ignored)
              // ignore all following blanks
              while (isspace(c)) {
                c = istr.get();
                if ((c == '\n') || istr.eof() || istr.fail())
                  break;
              }
              break;
            case '\r': { // end of line in DOS-files
#if 0
              c = istr.get();
              if (c == '\n') {
#endif
                // ignore all following blanks
                while (isspace(c)) {
                  c = istr.get();
                  if ((c == '\n') || istr.eof() || istr.fail())
                    break;
                }
                break;
#if 0
              } else {
                istr.putback(char(c));
                c = '\b';
              }
#endif
            }
            case 'a':
              c = '\a';
              break;
            case 'b':
              c = '\b';
              break;
            case 'f':
              c = '\f';
              break;
            case 'n':
              c = '\n';
              break;
            case 'r':
              c = '\r';
              break;
            case 't':
              c = '\t';
              break;
            case 'v':
              c = '\v';
              break;
            default: // the character is taken
              break;
            } // switch(c)
            line.replace(pos, 2, string(1, c));
          }

          return line;
        } // string getline_escape(istream& istr)

      /**
       ** split 'line' according to 'delemiter'
       **
       ** @param     line        line to split
       ** @param     separator   separator of the parts
       **
       ** @return    list with the separated parts,
       **            without leading or trailing blanks
       ** 
       ** @author    Diether Knof
       **
       ** @version   2006-03-30
       **/
      std::vector<std::string> split(std::string const& line,
                                   char const separator)
      {
        std::vector<std::string> parts;

        size_t pos1 = 0;
        for (size_t pos2 = line.find(separator, pos1);
             (pos1 != string::npos) && (pos1 < line.size() - 1);
             pos1 = pos2, pos2 = line.find(separator, pos1 + 1)) {
          parts.push_back(string(line, pos1 + 1,
                                 (pos2 == string::npos)
                                 ? pos2
                                 : pos2 - pos1 - 1));
          remove_blanks(parts.back());
        } // for (pos1, pos2)

        return parts;
      } // std::vector<std::string> split(std::string line, char seperator)

      /**
       ** converts from latin1 to utf8
       **
       ** @param     text   text in latin1 encoding
       **
       ** @return    text in utf8 encoding
       **
       ** @author    Diether Knof
       **
       ** @version   0.7.12
       **/
      std::string
        latin1_to_utf8(std::string text)
        {
          for (std::string::iterator c = text.begin();
               c != text.end();
               ++c) {
            if(static_cast<unsigned char>(*c) >= 0x80) {
              // thanks to http://stackoverflow.com/questions/5586214/how-to-convert-char-from-iso-8859-1-to-utf-8-in-c-multiplatformly for the code
              c = text.insert(c, 0xc0 | (*c & 0xc0) >> 6);// first byte, simplified since our range is only 8-bits
              ++c;
              *c = (0x80 | (*c & 0x3f));
            }

          } // for (c)
          return text;
        } // std::string latin1_to_utf8(std::string text)


    } // namespace String
  } // namespace Utils
} // namespace DK
