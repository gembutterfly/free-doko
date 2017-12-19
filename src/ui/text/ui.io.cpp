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

#include "constants.h"

#ifdef USE_UI_TEXT

#include "ui.h"
#include "general.h"
#include "party.h"
#include "help.h"
#include "bug_report.h"
#include "settings.h"

#include "../../misc/translations.h"
#include "../../utils/string.h"

namespace UI_TEXT_NS {

  /**
   ** reads a line from the input
   **
   ** @param	default_command    default command
   **
   ** @return	-
   **
   ** @author	Diether Knof
   **
   ** @version	0.5.4
   **/
  void
    UI_Text::getline(string const& default_command)
    {
      if ((this->istr_ == &cin)
          && (this->ostr_ == &cout)) {
        this->ostr() << this->prompt;
        if (!default_command.empty())
          this->ostr() << " [" << default_command << "]";
        this->ostr() << ": " << flush;
      }
      std::getline(this->istr(), this->line);
      if (this->istr().eof()) {
        ::game_status = GAMESTATUS::QUIT;
      } // if (this->istr().eof())
      if (!this->istr().good() && !this->istr().eof()) {
        cerr << "UI Text: Problems with reading the line.\n"
          << "Aborting." << endl;
        exit(0);
      } // if (!this->istr().good())
      if (this->line.empty())
        this->line = default_command;

        if (this->line.find('=') == string::npos) {
          this->line_name = "";
          this->line_value = "";
        } else {
          this->line_name = this->line;
          this->line_value = string(this->line_name,
                                    this->line_name.find('=') + 1);
          this->line_name.erase(this->line_name.find('='));
          DK::Utils::String::remove_blanks(this->line_name);
          DK::Utils::String::remove_blanks(this->line_value);
        }

      return ;
    } // void UI_Text::getline(string const& default_command = "")

  /**
   ** interprets the line (non special commands)
   **
   ** @param	-
   **
   ** @return	-
   **
   ** @author	Diether Knof
   **
   ** @version	0.7.10
   **/
  bool
    UI_Text::interpret_line()
    {
        // test general, help, settings
        if (   (this->general->interpret_line())
            || (this->party->interpret_line())
            || (this->help->interpret_line())
            || (this->bug_report->interpret_line())
            || (this->settings->interpret_line()) ) {
          return true;
        } else { // if !(line interpreted)
          this->ostr() << ::translation("Unknown command '%scommand%'.",
                                        this->line.c_str())
            << '\n';
          return false;
        } // if !(line interpreted)
    } // bool UI_Text::interpret_line()

  /**
   ** test the line for the keyword:
   ** -> result
   **
   ** @param	keyword	keyword to test
   **
   ** @return	whether the read line is the keyword (with translation)
   **
   ** @author	Diether Knof
   **
   ** @version	0.5.4
   **/
  bool
    UI_Text::iskeyword(string const& keyword) const
    {
      return this->iskeyword(this->line, keyword);
    } // bool UI_Text::iskeyword(string const& keyword) const

  /**
   ** test 'text' for the keyword:
   ** -> result
   **
   ** @param	text	text to test
   ** @param	keyword	keyword to test
   **
   ** @return	whether the text is the keyword (with translation)
   **
   ** @author	Diether Knof
   **
   ** @version	0.5.4
   **/
  bool
    UI_Text::iskeyword(string const& text, string const& keyword) const
    {
#ifdef POSTPONED
      return (   (text == keyword)
              || (text == ::translation("UI::Text::Command::" + keyword)));
#else
      return (text == keyword);
#endif
    } // bool UI_Text::iskeyword(string const& text, string const& keyword) const

  /**
   ** test the first word of the line for the keyword:
   ** -> result
   **
   ** @param	keyword	keyword to test
   **
   ** @return	whether the first word of the read line is the keyword
   **		(with translation)
   **
   ** @author	Diether Knof
   **
   ** @version	0.5.4
   **/
  bool
    UI_Text::first_iskeyword(string const& keyword) const
    {
      return this->first_iskeyword(this->line, keyword);
    } // bool UI_Text::first_iskeyword(string const& keyword) const

  /**
   ** test the first word of 'text' for the keyword:
   ** -> result
   **
   ** @param	text	text to test
   ** @param	keyword	keyword to test
   **
   ** @return	whether the first word of 'text' is the keyword
   **
   ** @author	Diether Knof
   **
   ** @version	0.5.4
   **/
  bool
    UI_Text::first_iskeyword(string const& text, string const& keyword) const
    {
      return this->iskeyword(DK::Utils::String::word_first(text), keyword);
    } // bool UI_Text::first_iskeyword(string const& text, string const& keyword) const

} // namespace UI_TEXT_NS

#endif // #ifdef USE_UI_TEXT
