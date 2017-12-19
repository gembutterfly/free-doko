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

#ifndef CLASS_SETTING_HEADER
#define CLASS_SETTING_HEADER

#include "../basistypes.h"

#include "translation.h"

#include "../utils/date.h"

/**
 ** the class for all preferences
 **
 ** @author   Diether Knof
 **/
class Setting {
  public:
    // cards order
    class CardsOrder;
    // theme (some settings)
    class Theme;

  public:
    // the date of the expiration of the Altenburg cardset
    static DK::Utils::Date const ALTENBURG_EXPIRATION_DATE;
    // the date of the expiration of the InnoCard cardset
    static DK::Utils::Date const INNOCARD_EXPIRATION_DATE;
    // the date of the expiration of the Elite cardset
    static DK::Utils::Date const ELITE_EXPIRATION_DATE;

  public:
    enum TypeBool {
      FIRST,
      BOOL_FIRST = FIRST,

      // save nothing (automatically)
      AUTOMATIC_SAVINGS = BOOL_FIRST,
      SAVE_PARTY_CHANGES,
      ADDITIONAL_PARTY_SETTINGS,
      SHOW_SPLASH_SCREEN,
      SPLASH_SCREEN_TRANSPARENT,
      SHOW_BUG_REPORT_BUTTON_IN_GAME_FINISHED_WINDOW,
      SAVE_BUG_REPORTS_ON_DESKTOP,
      SOUND,
#ifdef USE_NETWORK_DOKOLOUNGE
      DOKOLOUNGE_AUTO_LOGIN,
#endif

      AUTOMATIC_CARD_SUGGESTION,
      ANNOUNCE_SWINES_AUTOMATICALLY,

      SHOW_IF_VALID,
      EMPHASIZE_VALID_CARDS,
      ANNOUNCE_IN_TABLE,
      SHOW_ALL_HANDS,
      SHOW_AI_INFORMATION_HANDS,
      SHOW_AI_INFORMATION_TEAMS,
      SHOW_TRICKPILES_POINTS,
      SHOW_KNOWN_TEAMS_IN_GAME,
      SHOW_SOLOPLAYER_IN_GAME,

      SHOW_FULL_TRICK_WINDOW,
      SHOW_FULL_TRICK_WINDOW_IF_SPECIAL_POINTS,
      CLOSE_FULL_TRICK_AUTOMATICALLY,

      SHOW_GAMETYPE_WINDOW,
      CLOSE_GAMETYPE_WINDOW_AUTOMATICALLY,
      SHOW_MARRIAGE_WINDOW,
      CLOSE_MARRIAGE_WINDOW_AUTOMATICALLY,
      SHOW_GENSCHER_WINDOW,
      CLOSE_GENSCHER_WINDOW_AUTOMATICALLY,
      SHOW_ANNOUNCEMENT_WINDOW,
      CLOSE_ANNOUNCEMENT_WINDOW_AUTOMATICALLY,
      SHOW_SWINES_WINDOW,
      CLOSE_SWINES_WINDOW_AUTOMATICALLY,

      NETWORK_CONNECTIONS_URGENCY_HINT,
      NETWORK_CHAT_URGENCY_HINT,

      ROTATE_TRICK_CARDS,

      OWN_HAND_ON_TABLE_BOTTOM,

      ORIGINAL_CARDS_SIZE,

      BOOL_LAST = ORIGINAL_CARDS_SIZE
    }; // enum TypeBool
    static unsigned const BOOL_NUMBER;

    enum TypeUnsigned {
      UNSIGNED_FIRST = BOOL_LAST + 1,
#ifdef USE_THREADS
      THREADS_MAX = UNSIGNED_FIRST,
      CARD_PLAY_DELAY,
#else
      CARD_PLAY_DELAY = UNSIGNED_FIRST,
#endif

      FULL_TRICK_CLOSE_DELAY,

      GAMETYPE_WINDOW_CLOSE_DELAY,
      MARRIAGE_WINDOW_CLOSE_DELAY,
      GENSCHER_WINDOW_CLOSE_DELAY,
      ANNOUNCEMENT_WINDOW_CLOSE_DELAY,
      SWINES_WINDOW_CLOSE_DELAY,

      TABLE_ROTATION,

      CARDS_HEIGHT,

      UNSIGNED_LAST = CARDS_HEIGHT
    }; // enum TypeUnsigned
    static unsigned const UNSIGNED_NUMBER;

    enum TypeString {
      STRING_FIRST = UNSIGNED_LAST + 1,

      NAME = STRING_FIRST,
      LANGUAGE,

#ifdef USE_NETWORK_DOKOLOUNGE
      DOKOLOUNGE_NAME,
      DOKOLOUNGE_PASSWORD,
#endif

      CARDSET,
      CARDS_BACK,
      ICONSET,
      BACKGROUND,
      NAME_FONT,
      NAME_FONT_COLOR,
      NAME_ACTIVE_FONT_COLOR,
      NAME_RESERVATION_FONT_COLOR,
      TRICKPILE_POINTS_FONT,
      TRICKPILE_POINTS_FONT_COLOR,
      POVERTY_SHIFT_ARROW_COLOR,

#ifdef USE_SOUND_COMMAND
      PLAY_SOUND_COMMAND,
#endif
      BROWSER_COMMAND,

      STRING_LAST = BROWSER_COMMAND
    }; // enum TypeString
    static unsigned const STRING_NUMBER;

    enum TypeStringConst {
      STRING_CONST_FIRST = STRING_LAST + 1,

      CARDSET_LICENSE = STRING_CONST_FIRST,
      ICONSET_LICENSE,

      PUBLIC_DATA_DIRECTORY,
      PRIVATE_DATA_DIRECTORY,
      SETTINGS_FILE,
      LOGO_FILE,
      ICON_FILE,
      LANGUAGES_DIRECTORY,
      LANGUAGE_FILE,
      GRAPHIC_EXTENSION,
      SOUND_EXTENSION,
      CARDSETS_DIRECTORY,
      CARDS_DIRECTORY,
      CARDS_BACK_DIRECTORY,
      ICONS_FROM_CARDSET_DIRECTORY,
      ICONSETS_DIRECTORY,
      ICONS_DIRECTORY,
      BACKGROUNDS_DIRECTORY,
      SOUNDS_DIRECTORY,
#ifdef USE_NETWORK_DOKOLOUNGE
      DOKOLOUNGE_DIRECTORY,
#endif
      AI_DIRECTORY,
      PARTIES_DIRECTORY,
      RULES_DIRECTORY,
      SETTINGS_DIRECTORY,
      MANUAL_DIRECTORY,

      STRING_CONST_LAST = MANUAL_DIRECTORY
    }; // enum TypeStringConst
    static unsigned const STRING_CONST_NUMBER;

    enum TypeCardsOrder {
      CARDS_ORDER = STRING_CONST_LAST + 1,

      LAST = CARDS_ORDER
    }; // enum TypeCardsOrder

  public:
    Setting();
    Setting(Setting const& setting);
    Setting& operator=(Setting const& setting);
    ~Setting();

    // set the directories to search for the data
    void set_data_directories();
    // add a path to the data diretories
    void add_data_directory(string const& path);
    // write the data diretories in the stream
    ostream& write_data_directories(ostream& ostr,
                                    string const& indent = "") const;
    // a string containing the data directories
    string data_directories_string(string const& indent = "") const;


    // returns, whether the setting is activ or not (or the value)
    bool operator()(const TypeBool type) const;
    unsigned operator()(const TypeUnsigned type) const;
    string operator()(const TypeString type) const;
    string operator()(const TypeStringConst type) const;
    CardsOrder const& operator()(const TypeCardsOrder type) const;
    CardsOrder& operator()(const TypeCardsOrder type);

    // returns the type with the name
    int type(string const& name) const;
    // returns the type with the translated name
    int type_translated(string const& name) const;
    // returns the type with the translated or normal name
    int type_translated_or_normal(string const& name) const;

    // returns, whether the dependencies of the settings are met
    bool dependencies(TypeBool const& type) const;
    bool dependencies(TypeUnsigned const& type) const;
    bool dependencies(TypeString const& type) const;

    // returns the setting
    bool const& value(TypeBool const type) const;
    unsigned const& value(TypeUnsigned const type) const;
    string const& value(TypeString const type) const;
    string const& path(TypeString const type) const;
    string const value(TypeStringConst const type) const;
    string const path(TypeStringConst const type) const;
    CardsOrder const& value(TypeCardsOrder const type) const;
    CardsOrder& value(TypeCardsOrder const type);

    // the minimum and maximum value
    unsigned min(TypeUnsigned const& type) const;
    unsigned max(TypeUnsigned const& type) const;

    // return the themes list
    vector<Theme*> const& themes() const;
    // return the theme with the given name
    Theme const* theme(string const& name) const;

    // sets the setting
    bool set(string const& type, string const& value);
    void set(const TypeBool type, bool const value);
    void set(const TypeBool type, string const& value);
    void set(const TypeString type, string const& value);
    void set(const TypeUnsigned type, unsigned const value);
    void set(const TypeUnsigned type, string const& value);
    void set(const TypeCardsOrder type,
             CardsOrder const& value);
    void set(const TypeCardsOrder type,
             string const& value);
    // set the values from the theme
    void set(Theme const& theme);
    // set the values from the theme 'name'
    bool set_theme(string const& name);

    // returns the position of 'playerno'
    Position position(unsigned const playerno) const;

    void load();
    void load(string const& filename,
              bool filename_not_exists_output = true);

    bool save() const;
    bool save(string const& filename) const;

    // check whether a cardset is outdated and replaces it
    void check_for_outdated_cardset();

    // write of the settings
    ostream& write(ostream& ostr) const;

#ifdef USE_NETWORK_DOKOLOUNGE
    // the path for the logo
    string dokolounge_logo() const;
    // the path for the player icon
    string dokolounge_player_icon(string const& icon) const;
    // the path for the table icon
    string dokolounge_table_icon(string const& icon) const;
  private:
    // the path for the icon
    string dokolounge_icon(string const& subdir, string const& icon) const;
  public:
#endif

  private:
    // set to the hardcoded values
    void set_to_hardcoded();

    // search a cards back
    void search_cards_back();
    // search a background
    void search_background();

    // update all
    void update_all();
    // create the themes list
    void create_themes_list();
    // clear the themes list
    void clear_themes_list();
    // update the path
    void update_path(TypeString const type);

  private:
    // boolean settings
    vector<bool> bool_;
    // unsigned settings
    vector<unsigned> unsigned_;
    // string settings
    vector<string> string_;
    // the cardset license (string const setting)
    string cardset_license_;
    // the iconset license (string const setting)
    string iconset_license_;
    // cards order setting
    CardsOrder* cards_order_;

    // directories for the data
    PRIV_VAR_R(vector<string>, data_directories);
    // string settings (paths)
    vector<string> string_path_;
    // themes
    vector<Theme*> themes_;

}; // class Setting

// conversion in a string
string name(Setting::TypeBool const& type);
string name(Setting::TypeUnsigned const& type);
string name(Setting::TypeString const& type);
string name(Setting::TypeStringConst const& type);
string name(Setting::TypeCardsOrder const& type);

// output of the names
WRITE_NAME(Setting::TypeBool)
WRITE_NAME(Setting::TypeUnsigned)
WRITE_NAME(Setting::TypeString)
WRITE_NAME(Setting::TypeStringConst)
WRITE_NAME(Setting::TypeCardsOrder)

  // output of the Settings
  ostream& operator<<(ostream& ostr, Setting const& setting);

  // comparison of two settings (value)
  bool operator==(Setting const& setting1, Setting const& setting2);
  bool operator!=(Setting const& setting1, Setting const& setting2);

  //extern string GPL;
  //extern string cardset_license;

inline
Translator::Translation translation(Setting::TypeBool const& type)
{ return Translator::Translation(::translator,
                                 "Setting::"
                                 + ::name(type)); }
inline
Translator::Translation translation(Setting::TypeUnsigned const& type)
{ return Translator::Translation(::translator,
                                 "Setting::"
                                 + ::name(type)); }
inline
Translator::Translation translation(Setting::TypeString const& type)
{ return Translator::Translation(::translator,
                                 "Setting::"
                                 + ::name(type)); }
inline
Translator::Translation translation(Setting::TypeStringConst const& type)
{ return Translator::Translation(::translator,
                                 "Setting::"
                                 + ::name(type)); }
inline
Translator::Translation translation(Setting::TypeCardsOrder const& type)
{ return Translator::Translation(::translator,
                                 "Setting::"
                                 + ::name(type)); }

extern Setting setting;

#include "setting.cardsorder.h"
#include "setting.theme.h"

#endif // #ifndef CLASS_SETTING_HEADER
