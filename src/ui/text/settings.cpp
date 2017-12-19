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
 **********************************************************************/

#include "constants.h"

#ifdef USE_UI_TEXT

#include "settings.h"

#include "../../misc/translations.h"
#include "../../misc/setting.h"
#include "../../utils/string.h"

namespace UI_TEXT_NS {

  /**
   **
   ** constructor
   **
   ** @param	ui	pointer to the ui
   **
   ** @return	-
   **
   ** @version	0.5.4
   **
   ** @author	Diether Knof
   **
   **/
  UI_Text::Settings::Settings(UI_Text* const ui) :
    ui(ui)
  { }

  /**
   **
   ** destructor
   **
   ** @param	-
   **
   ** @return	-
   **
   ** @version	0.5.4
   **
   ** @author	Diether Knof
   **
   **/
  UI_Text::Settings::~Settings()
  { }

  /**
   **
   ** Enter the settings subsystem.
   ** The subsystem can be left with 'leave'
   **
   ** @param	-
   **
   ** @return	-
   **
   ** @version	0.5.4
   **
   ** @author	Diether Knof
   **
   ** @todo	all
   **
   **/
  void
    UI_Text::Settings::enter()
    { }

  /**
   **
   ** interprets the line read by the ui
   **
   ** @param	-
   **
   ** @return	whether the line was interpreted
   **
   ** @version	0.5.4
   **
   ** @author	Diether Knof
   **
   **/
  bool
    UI_Text::Settings::interpret_line()
    {
      // whether the line has been interpreted
      bool interpreted = false;

      if (this->ui->iskeyword("settings")) {
        interpreted = this->interpret_command("show");
      } else if (   (this->ui->first_iskeyword("setting"))
                 || (this->ui->first_iskeyword("settings"))) {
        string name = this->ui->line;
        DK::Utils::String::word_first_remove_with_blanks(name);
        interpreted = this->interpret_command(name);
        if (!interpreted) {
          this->ui->ostr() << "settings command not known.\n"
            << "Available commands: show, groups, ...\n";
          interpreted = true;
        }
      } // if (this->ui->iskeyword(...)

      return interpreted;
    } // bool UI_Text::Settings::interpret_line()

  /**
   **
   ** interprets the given command
   **
   ** @param	command	command that shall be interpreted
   **
   ** @return	whether the command was interpreted
   **
   ** @version	0.5.4
   **
   ** @author	Diether Knof
   **
   **/
  bool
    UI_Text::Settings::interpret_command(string const& command)
    {
      // whether the line has been interpreted
      bool interpreted = false;

      if (this->ui->iskeyword(command, "show")) {
        this->show();
        interpreted = true;
      } else if (this->ui->iskeyword(command, "groups")) {
        this->show_groups();
        interpreted = true;
      } else if (this->ui->iskeyword(command, "load")) {
        // load the settings
        ::setting.load();
        interpreted = true;
      } else if (this->ui->first_iskeyword(command, "load")) {
        // load the settings
        string name = this->ui->line;
        DK::Utils::String::word_first_remove_with_blanks(name);
        ::setting.load(name);
        interpreted = true;
      } else if (this->ui->iskeyword(command, "save")) {
        // save the settings
        if (::setting.save())
          this->ui->ostr() << ::translation("Settings saved.");
        interpreted = true;
      } else if (this->ui->first_iskeyword(command, "save")) {
        // load the settings
        string name = this->ui->line;
        DK::Utils::String::word_first_remove_with_blanks(name);
        if (::setting.save(name))
          this->ui->ostr() << ::translation("Settings saved.");
        interpreted = true;
      } else {
        // test for a type

        if (command.find('=') == string::npos) {
          int const type = ::setting.type_translated_or_normal(command);
          if (type != -1) {
            this->show(type);
            interpreted = true;
          } else if (this->is_group(command)) {
            this->show_group(command); 
            interpreted = true;
          } // if (type != -1)
        } else { // if !(command.find('=') == string::npos)
          // a setting shall be changed
          string name = command;
          string value = string(name, name.find('=') + 1);
          name.erase(name.find('='));
          DK::Utils::String::remove_blanks(name);
          DK::Utils::String::remove_blanks(value);
          int const type = ::setting.type_translated_or_normal(name);
          if (type != -1) {
            ::setting.set(name, value);
            interpreted = true;
          } // if (type != -1)
        } // if !(command.find('=') == string::npos)
      } // if (this->ui->iskeyword(...)

      return interpreted;
    } // bool UI_Text::Settings::interpret_command(string const& command)

  /**
   **
   ** show the settings
   **
   ** @param	-
   **
   ** @return	-
   **
   ** @version	0.5.4
   **
   ** @author	Diether Knof
   **
   **/
  void
    UI_Text::Settings::show() const
    {
      this->show_group("general");
      this->show_group("assistance");
      this->show_group("help");
      this->show_group("cards order");

      return ;
    } // void UI_Text::Settings::show() const

  /**
   **
   ** show the settings
   **
   ** @param	-
   **
   ** @return	-
   **
   ** @version	0.7.10
   **
   ** @author	Diether Knof
   **
   **/
  void
    UI_Text::Settings::show_groups() const
    {
      this->ui->ostr()
        << ::translation("settings groups") << ":\n"
        << "  " << ::translation("Setting::Group::general") << '\n'
        << "  " << ::translation("Setting::Group::assistance") << '\n'
        << "  " << ::translation("Setting::Group::help") << '\n'
        << "  " << ::translation("Setting::Group::cards order") << '\n'
        ;
      return ;
    } // void UI_Text::Settings::show_groups() const

  /**
   ** show the settings of the group 'group'
   **
   ** @param	group   group to show
   **
   ** @return	-
   **
   ** @author	Diether Knof
   **
   ** @version	0.7.10
   **/
  void
    UI_Text::Settings::show_group(string const& group) const
    {
      if (group == "general") {
        this->ui->ostr() << "** "
          << ::translation("Setting::Group::general")
          << " **\n";
        this->show(Setting::NAME);
        this->show(Setting::LANGUAGE);
        this->show(Setting::SOUND);
#ifdef USE_THREADS
        this->show(Setting::THREADS_MAX);
#endif
        this->show(Setting::SAVE_BUG_REPORTS_ON_DESKTOP);
        this->show(Setting::SAVE_PARTY_CHANGES);
#ifdef USE_SOUND_COMMAND
        this->show(Setting::PLAY_SOUND_COMMAND);
#endif
        this->show(Setting::BROWSER_COMMAND);
      } else if (group == "assistance") {
        this->ui->ostr() << "** "
          << ::translation("Setting::Group::assistance")
          << " **\n";
        this->show(Setting::SHOW_SOLOPLAYER_IN_GAME);
        this->show(Setting::ANNOUNCE_SWINES_AUTOMATICALLY);
        this->show(Setting::EMPHASIZE_VALID_CARDS);
      } else if (group == "help") {
        this->ui->ostr() << "** "
          << ::translation("Setting::Group::help")
          << " **\n";
        this->show(Setting::SHOW_ALL_HANDS);
        this->show(Setting::SHOW_AI_INFORMATION_HANDS);
        this->show(Setting::SHOW_KNOWN_TEAMS_IN_GAME);
        this->show(Setting::SHOW_AI_INFORMATION_TEAMS);
        this->show(Setting::SHOW_TRICKPILES_POINTS);
        this->show(Setting::AUTOMATIC_CARD_SUGGESTION);
      } else if (group == "cards order") {
        this->ui->ostr() << "** "
          << ::translation("Setting::Group::cards order")
          << " **\n";
        this->show(Setting::CARDS_ORDER);
      } else {
        cerr << "settings group '" << group << "' unknown\n";
      }
    } // void UI_Text::Settings::show_group(string group) const

  /**
   ** -> result
   **
   ** @param	name   group name to check
   **
   ** @return	-
   **
   ** @author	Diether Knof
   **
   ** @version	0.7.10
   **/
  bool
    UI_Text::Settings::is_group(string const& name) const
    {
      return (   (name == "general")
              || (name == "behaviour")
              || (name == "assistance")
              || (name == "help")
              || (name == "cards order") );
    } // bool UI_Text::Settings::is_group(string const& name) const

  /**
   **
   ** show the given setting
   **
   ** @param	type	type to show
   **
   ** @return	-
   **
   ** @version	0.5.4
   **
   ** @author	Diether Knof
   **
   **/
  void
    UI_Text::Settings::show(int const type) const
    {
      if ((type >= Setting::BOOL_FIRST)
          && (type <= Setting::BOOL_LAST))
        this->ui->ostr() << "  "
          << setw(32) << ::translation(static_cast<Setting::TypeBool>(type))
          << " = " << ::setting(static_cast<Setting::TypeBool>(type))
          << '\n';
      else if ((type >= Setting::UNSIGNED_FIRST)
               && (type <= Setting::UNSIGNED_LAST))
        this->ui->ostr() << "  "
          << setw(32) << ::translation(static_cast<Setting::TypeUnsigned>(type))
          << " = " << ::setting(static_cast<Setting::TypeUnsigned>(type))
          << '\n';
      else if ((type >= Setting::STRING_FIRST)
               && (type <= Setting::STRING_LAST))
        this->ui->ostr() << "  "
          << setw(32) << ::translation(static_cast<Setting::TypeString>(type))
          << " = " << ::setting(static_cast<Setting::TypeString>(type))
          << '\n';
      else if (type == Setting::CARDS_ORDER)
        this->ui->ostr() << "  "
          << setw(32)
          << ::translation(static_cast<Setting::TypeCardsOrder>(type))
          << " = " << ::setting(static_cast<Setting::TypeCardsOrder>(type))
          << '\n';
      else
        this->ui->ostr() << ::translation("Setting")
          << ": "
          << "type '%itype%' not found."
          << '\n';

      return ;
    } // void UI_Text::Settings::show(int const type) const

  /**
   **
   ** show the given setting
   **
   ** @param	name	name of the type
   **
   ** @return	-
   **
   ** @version	0.5.4
   **
   ** @author	Diether Knof
   **
   **/
  void
    UI_Text::Settings::show(string const& name) const
    {
      int const type = ::setting.type_translated_or_normal(name);

      if (type != -1)
        this->show(type);
      else
        this->ui->ostr() << ::translation("Setting '%sname%' not found.",
                                          name.c_str())
          << '\n';

      return ;
    } // void UI_Text::Settings::show(string const& name) const

} // namespace UI_TEXT_NS

#endif // #ifdef USE_UI_TEXT
