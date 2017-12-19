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
 ********************************************************************/

#include "constants.h"

#include "setting.h"
#include "setting.cardsorder.h"
#include "setting.theme.h"

#ifdef LINUX
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>
#endif
#include <sys/stat.h>

#include "translator.h"
#include "translations.h"

#include "../party/party.h"
#include "../game/game.h"
#include "../player/human/human.h"

#include "../ui/ui.h"
#include "../ui/ui.wrap.h"

#include "../utils/file.h"
#include "../utils/string.h"
#include "../utils/windows.h"



// number of the Type
unsigned const Setting::BOOL_NUMBER = (1 + Setting::BOOL_LAST
                                       - Setting::BOOL_FIRST);
unsigned const Setting::UNSIGNED_NUMBER = (1 + Setting::UNSIGNED_LAST
                                           - Setting::UNSIGNED_FIRST);
unsigned const Setting::STRING_NUMBER = (1 + Setting::STRING_LAST
                                         - Setting::STRING_FIRST);
unsigned const Setting::STRING_CONST_NUMBER = (1 + Setting::STRING_CONST_LAST
                                               - Setting::STRING_CONST_FIRST);

/**
 ** constructor
 ** 
 ** @param     -
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
Setting::Setting() :
  bool_(BOOL_NUMBER),
  unsigned_(UNSIGNED_NUMBER),
  string_(STRING_NUMBER, ""),
  cardset_license_(""),
  cards_order_(new CardsOrder),
  data_directories_(),
  string_path_(STRING_NUMBER, ""),
  themes_()
{
  this->set_data_directories();
  this->set_to_hardcoded();
  this->create_themes_list();

  if (this == &::setting)
    Translator::dir_scan();
  if (this == &::setting)
    ::translator.load();

  return ;
} // Setting::Setting()

/**
 ** copy constructor
 ** 
 ** @param     setting   setting to copy
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
Setting::Setting(Setting const& setting):
  bool_(setting.bool_),
  unsigned_(setting.unsigned_),
  string_(setting.string_),
  cardset_license_(setting.cardset_license_),
  cards_order_(new CardsOrder(*setting.cards_order_)),
  data_directories_(setting.data_directories_),
  string_path_(setting.string_path_),
  themes_()
{
  for (vector<Theme*>::const_iterator t = setting.themes().begin();
       t != setting.themes().end();
       ++t)
    this->themes_.push_back(new Theme(**t));

  this->update_all();

  return ;
} // Setting::Setting(Setting setting)

/**
 ** copy operator
 ** 
 ** @param     setting   setting to copy
 **
 ** @return    this
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
Setting&
Setting::operator=(Setting const& setting)
{
  this->bool_ = setting.bool_;
  this->unsigned_ = setting.unsigned_;
  this->string_ = setting.string_;
  this->cardset_license_ = setting.cardset_license_;
  *this->cards_order_ = *setting.cards_order_;
  this->data_directories_ = setting.data_directories_;
  this->string_path_ = setting.string_path_;
  this->clear_themes_list();
  for (vector<Theme*>::const_iterator t = setting.themes().begin();
       t != setting.themes().end();
       ++t)
    this->themes_.push_back(new Theme(**t));

  return *this;
} // bool Setting::operator=(Setting setting) const

/**
 ** destructor
 ** 
 ** @param     -
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
Setting::~Setting()
{
  delete this->cards_order_;
  this->clear_themes_list();
  return ;
} // Setting::Setting()

/**
 ** set the values to the hardcoded
 ** 
 ** @param     -
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
void
Setting::set_to_hardcoded()
{
#if 0
  this->set(PUBLIC_DATA_DIRECTORY,	"");
  this->set(PRIVATE_DATA_DIRECTORY,	"");
  this->set(SETTINGS_FILE,		"FreeDokorc");
  this->set(LOGO_FILE,                  "logo.png");
  this->set(ICON_FILE,                  "icon.png");
  this->set(LANGUAGES_DIRECTORY,	"translations");
  this->set(LANGUAGE_FILE,		"text");
  this->set(GRAPHIC_EXTENSION,		"png");
  this->set(SOUND_EXTENSION,		"wav");
  this->set(CARDSETS_DIRECTORY,		"cardsets");
  this->set(CARDS_DIRECTORY,		"cards");
  this->set(CARDS_BACK_DIRECTORY,	"back");
  this->set(ICONS_DIRECTORY,		"icons");
  this->set(BACKGROUNDS_DIRECTORY,	"backgrounds");
  this->set(SOUNDS_DIRECTORY,		"sound");
  this->set(AI_DIRECTORY,		"ai");
  this->set(PARTIES_DIRECTORY,		"rules");
  this->set(RULES_DIRECTORY,		"rules");
  this->set(SETTINGS_DIRECTORY,		"settings");
  this->set(MANUAL_DIRECTORY,		"/usr/share/doc/manual");
#endif

  this->set(AUTOMATIC_SAVINGS,          true);
  this->set(SAVE_PARTY_CHANGES,		true);
  this->set(ADDITIONAL_PARTY_SETTINGS,	false);


  this->set(LANGUAGE,			"");

  this->set(NAME,			"");
#ifdef USE_NETWORK_DOKOLOUNGE
  this->set(DOKOLOUNGE_NAME,		"");
  this->set(DOKOLOUNGE_PASSWORD,	"");
  this->set(DOKOLOUNGE_AUTO_LOGIN,      false);
#endif

  this->set(SHOW_SPLASH_SCREEN,		true);
  this->set(SPLASH_SCREEN_TRANSPARENT,	false);

  this->set(SHOW_BUG_REPORT_BUTTON_IN_GAME_FINISHED_WINDOW,	true);
  this->set(SAVE_BUG_REPORTS_ON_DESKTOP,	true);

  this->set(SOUND,			true);

  this->set(AUTOMATIC_CARD_SUGGESTION,	false);
  this->set(ANNOUNCE_SWINES_AUTOMATICALLY,	true);
  this->set(SHOW_IF_VALID,		true);
  this->set(EMPHASIZE_VALID_CARDS,	false);
  this->set(ANNOUNCE_IN_TABLE,		true);
  this->set(SHOW_ALL_HANDS,		false);
  this->set(SHOW_AI_INFORMATION_HANDS,	false);
  this->set(SHOW_AI_INFORMATION_TEAMS,	false);
  this->set(SHOW_TRICKPILES_POINTS,	true);
  this->set(SHOW_KNOWN_TEAMS_IN_GAME,	true);
  this->set(SHOW_SOLOPLAYER_IN_GAME,	true);

  this->set(SHOW_FULL_TRICK_WINDOW,	true);
  this->set(SHOW_FULL_TRICK_WINDOW_IF_SPECIAL_POINTS,	true);
  this->set(CLOSE_FULL_TRICK_AUTOMATICALLY,	false);

  this->set(SHOW_GAMETYPE_WINDOW,			true);
  this->set(CLOSE_GAMETYPE_WINDOW_AUTOMATICALLY,	false);
  this->set(SHOW_MARRIAGE_WINDOW,		true);
  this->set(CLOSE_MARRIAGE_WINDOW_AUTOMATICALLY,	false);
  this->set(SHOW_GENSCHER_WINDOW,			true);
  this->set(CLOSE_GENSCHER_WINDOW_AUTOMATICALLY,	false);
  this->set(SHOW_ANNOUNCEMENT_WINDOW,			true);
  this->set(CLOSE_ANNOUNCEMENT_WINDOW_AUTOMATICALLY,	false);
  this->set(SHOW_SWINES_WINDOW,				true);
  this->set(CLOSE_SWINES_WINDOW_AUTOMATICALLY,		false);

  this->set(NETWORK_CONNECTIONS_URGENCY_HINT,		true);
  this->set(NETWORK_CHAT_URGENCY_HINT,			true);

  this->set(ROTATE_TRICK_CARDS,		false);
  this->set(CARDS_ORDER,		"trump up, club down, heart down, spade down, diamond down");


#ifdef USE_THREADS
  this->set(THREADS_MAX,	       		1);
#endif
  this->set(CARD_PLAY_DELAY,			500);
  this->set(FULL_TRICK_CLOSE_DELAY,		2000);

  this->set(GAMETYPE_WINDOW_CLOSE_DELAY,	2000);
  this->set(MARRIAGE_WINDOW_CLOSE_DELAY,20);
  this->set(GENSCHER_WINDOW_CLOSE_DELAY,	2000);
  this->set(ANNOUNCEMENT_WINDOW_CLOSE_DELAY,	2000);
  this->set(SWINES_WINDOW_CLOSE_DELAY,		2000);

  this->set(ORIGINAL_CARDS_SIZE,		true);
  this->set(CARDS_HEIGHT,			140);

  this->set(OWN_HAND_ON_TABLE_BOTTOM,		true);
  this->set(TABLE_ROTATION,			0);

  //this->set(CARDSET,		"xskat/french");
  //this->set(CARDS_BACK,			"xskat");
  { // set the cardset
    vector<string> cardsets;
    if ((*this)(LANGUAGE) == "de") {
      if (DK::Utils::Date() <= Setting::INNOCARD_EXPIRATION_DATE)
        cardsets.push_back("InnoCard/french/185");
      cardsets.push_back("xskat/french");
      cardsets.push_back("xskat/german");
    } else if ((*this)(LANGUAGE) == "de-alt") {
      cardsets.push_back("xskat/german");
    cardsets.push_back("freedoko-pingucards");
    } else { // english and so
      if (DK::Utils::Date() <= Setting::INNOCARD_EXPIRATION_DATE)
        cardsets.push_back("InnoCard/english/185");
      cardsets.push_back("dondorf");
    } // if (language == )
    cardsets.push_back("xskat/french");
    cardsets.push_back("dondorf");
    cardsets.push_back("kdecarddecks/spaced");
    cardsets.push_back("pysol/gpl");
    cardsets.push_back("pysol/a-m/gpl");
    cardsets.push_back("openclipart");

    vector<string>::const_iterator cardset;
    for (cardset = cardsets.begin();
         cardset != cardsets.end();
         ++cardset) {
      this->set(CARDSET, *cardset);
      if (!this->path(CARDSET).empty())
        break;
    } // for (cardset \in cardsets)

    if (cardset == cardsets.end()) {
      this->set(CARDSET, "");
      if (::ui)
        ::ui->error(::translation("Error::loading default cardset %scardset%",
                                  cardsets.front()
                                 ).translation());
    } else {
      this->set(CARDSET, *cardset);
    }
  } // set the cardset
  { // set the cards back
    vector<string> backs;

    backs.push_back("penguin");
    backs.push_back("back");

    vector<string>::const_iterator back;
    for (back = backs.begin();
         back != backs.end();
         ++back) {
      this->set(CARDS_BACK, *back);
      if (!this->path(CARDS_BACK).empty())
        break;
    } // for (back \in backs)

    if (back == backs.end()) {
      this->set(CARDS_BACK, "");
      if (::ui)
        ::ui->error(::translation("Error::loading default cards back %sback% (%scardset%)",
                                  backs.front(),
                                  this->value(CARDSET)
                                 ).translation());
    } else {
      this->set(CARDS_BACK, *back);
    }
  } // set the cards back
  { // set the icons
    this->set(ICONSET, "default");
  } // set the icons
  { // set the background
    vector<string> backgrounds;

    backgrounds.push_back("table");

    vector<string>::const_iterator background;
    for (background = backgrounds.begin();
         background != backgrounds.end();
         ++background) {
      this->set(BACKGROUND, *background);
      if (!this->path(BACKGROUND).empty())
        break;
    } // for (background \in backgrounds)

    if (background == backgrounds.end()) {
      this->set(BACKGROUND, "");
      if (::ui)
        ::ui->error(::translation("Error::loading default background %sbackground%",
                                  backgrounds.front()
                                 ).translation());
    } else {
      this->set(BACKGROUND, *background);
    }
  } // set the background


#ifdef WINDOWS
  this->set(NAME_FONT,			"Arial Bold Italic 16");
#else
  this->set(NAME_FONT,			"Sans Serif Bold Italic 16");
#endif
  this->set(NAME_FONT_COLOR,		"black");
  this->set(NAME_ACTIVE_FONT_COLOR,	"red");
  this->set(NAME_RESERVATION_FONT_COLOR,"blue");
#ifdef WINDOWS
  this->set(TRICKPILE_POINTS_FONT,	"Arial Bold 18");
#else
  this->set(TRICKPILE_POINTS_FONT,	"Sans Serif Bold 18");
#endif
  this->set(TRICKPILE_POINTS_FONT_COLOR,"black");

  this->set(POVERTY_SHIFT_ARROW_COLOR,"black");

#ifdef USE_SOUND_COMMAND
  this->set(PLAY_SOUND_COMMAND,		"");
#endif
  this->set(BROWSER_COMMAND,		"");

  return ;
} // void Setting::set_to_hardcoded()

/**
 ** sets the data diretories
 **
 ** @param	-
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.3
 **/
void
Setting::set_data_directories()
{
  // search the directory
  this->data_directories_.clear();
  if (getenv("FREEDOKO_DATA_DIRECTORY"))
    this->data_directories_.push_back(getenv("FREEDOKO_DATA_DIRECTORY"));
  if (getenv("FREEDOKO_MORE_DATA_DIRECTORY"))
    this->data_directories_.push_back(getenv("FREEDOKO_MORE_DATA_DIRECTORY"));
  this->data_directories_.push_back(this->value(PRIVATE_DATA_DIRECTORY));


#ifdef PUBLIC_DATA_DIRECTORY_VALUE
  this->data_directories_.push_back(PUBLIC_DATA_DIRECTORY_VALUE);
#else // !#ifdef PUBLIC_DATA_DIRECTORY_VALUE

#ifdef RELEASE

#ifdef WINDOWS
  this->data_directories_.push_back(DK::Utils::Windows::Reg_read(HKEY_CURRENT_USER,
                                                                 "Software\\FreeDoko",
                                                                 "Directory"));
  if (this->data_directories_.back().empty())
    this->data_directories_.pop_back();
  this->data_directories_.push_back(DK::Utils::Windows::Reg_read(HKEY_LOCAL_MACHINE,
                                                                 "Software\\FreeDoko",
                                                                 "Directory"));
  if (this->data_directories_.back().empty())
    this->data_directories_.pop_back();
#endif // #ifdef WINDOWS

#else // !#ifdef RELEASE
  this->data_directories_.push_back("../data");
  this->data_directories_.push_back("../../more_data");
#endif // !#ifdef RELEASE

#endif // !#ifdef PUBLIC_DATA_DIRECTORY_VALUE

  this->data_directories_.push_back(".");

  // make each name unique
  for (vector<string>::iterator d = this->data_directories_.begin();
       d != this->data_directories_.end();
       ++d)
    if (   (d->size() >= 2)
        && ((*d)[0] == '.')
        && ((*d)[1] == '/'))
      d->erase(0, 2);
  std::unique(this->data_directories_.begin(), this->data_directories_.end());

  return ;
} // Setting::set_data_directories()

/**
 ** adds the given directory to the data directories
 **
 ** @param	path   directory to add
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.12
 **/
void
Setting::add_data_directory(string const& path)
{
  string d = (   (path.length() >= 2)
              && (path[0] == '.')
              && (path[1] == '/')
              ? string(path, 2)
              : path);
  if (std::find(this->data_directories_.begin(),
                this->data_directories_.end(), d)
      == this->data_directories_.end()) {
    this->data_directories_.push_back(d);
    ::translator.dir_scan();
    ::translator.load();
  }
  return ;
} // void Setting::add_data_directory(string const& path)

/**
 ** write the data directories in the stream, separated by newlines
 **
 ** @param	ostr     stream to write into
 ** @param	indent   indention of each line, default: ""
 **
 ** @return	stream
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.12
 **/
ostream&
Setting::write_data_directories(ostream& ostr, string const& indent) const
{
  for (vector<string>::const_iterator d = this->data_directories_.begin();
       d != this->data_directories_.end();
       ++d)
    ostr << indent << *d << '\n';
  return ostr;
} // ostream& Setting::write_data_directories(ostream& ostr, string const& indent)

/**
 ** -> result
 **
 ** @param	indent   indention of each line, default: ""
 **
 ** @return	the public directories as string, separated by newlines
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.12
 **/
string
Setting::data_directories_string(string const& indent) const
{
  string l;
  for (vector<string>::const_iterator d = this->data_directories_.begin();
       d != this->data_directories_.end();
       ++d)
    l += indent + *d + "\n";
  return l;
} // string Setting::data_directories_string(string const& indent)

/**
 ** -> result
 **
 ** @param	type	setting type
 **
 ** @return	the setting 'type'
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.4
 **/
bool
Setting::operator()(TypeBool const type) const
{
  if ((FAST_PLAY & FAST_NS::SHOW_ALL_HANDS)
      && (type == Setting::SHOW_ALL_HANDS))
    return true;

  return (this->dependencies(type)
          && this->value(type));
} // bool Setting::operator()(TypeBool type) const

/**
 ** -> result
 **
 ** @param	type	setting type
 **
 ** @return	the setting 'type'
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.4
 **/
unsigned
Setting::operator()(TypeUnsigned const type) const
{
  return this->value(type);
} // unsigned Setting::operator()(TypeUnsigned type) const

/**
 ** -> result
 **
 ** @param	type	setting type
 **
 ** @return	the setting 'type'
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.3
 **/
string
Setting::operator()(TypeString const type) const
{
  string value = this->value(type);

  if (value.empty()) {
    switch(type) {
    case NAME:
#ifdef LINUX
      value = getpwuid(geteuid())->pw_gecos;
      if (value.empty())
        // no real name - take the login
        value = getpwuid(geteuid())->pw_name;
#endif
#ifdef WINDOWS
#ifdef USE_REGISTRY
      {
        TCHAR  infoBuf[1001];
        infoBuf[1000] = 0;
        DWORD  bufCharCount =1000;
        if( !GetUserName( infoBuf, &bufCharCount ))
          value = "Player";
        else
          value = infoBuf;
      }
#ifdef OUTDATED
      // 2010-01-24
      // *** DK value = string(getenv("USERNAME"));
      if (value.empty())
        // Microsoft Windows XP
        value = DK::Utils::Windows::Reg_read(HKEY_CURRENT_USER,
                                             "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer",
                                             "Logon User Name");
      if (value.empty())
        // Microsoft Windows XP
        value = DK::Utils::Windows::Reg_read(HKEY_LOCAL_MACHINE,
                                             "Software\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon",
                                             "DefaultUserName");
      if (value.empty())
        // Microsoft Windows XP
        value = DK::Utils::Windows::Reg_read(HKEY_LOCAL_MACHINE,
                                             "Software\\Microsoft\\Windows NT\\CurrentVersion",
                                             "RegisteredOwner");
      if (value.empty())
        // Microsoft Windows XP: -
        value = DK::Utils::Windows::Reg_read(HKEY_LOCAL_MACHINE,
                                             "Network\\Logon",
                                             "username");
      if (value.empty())
        // Microsoft Windows XP: USERNAME
        value = DK::Utils::Windows::Reg_read(HKEY_LOCAL_MACHINE,
                                             "System\\CurrentControlSet\\Control",
                                             "CurrentUser");
      if (value.empty())
        // Microsoft Windows XP: -
        value = DK::Utils::Windows::Reg_read(HKEY_LOCAL_MACHINE,
                                             "Software\\Microsoft\\Windows\\CurrentVersion",
                                             "CurrentUser");
      if (value.empty())
        value = DK::Utils::Windows::Reg_read(HKEY_LOCAL_MACHINE,
                                             "Software\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon",
                                             "DefaultUserName");
#endif
#else
      value = "Player";
#endif
#endif
      break;
#ifdef USE_NETWORK_DOKOLOUNGE
    case DOKOLOUNGE_NAME:
      value = (*this)(NAME);
      break;
    case DOKOLOUNGE_PASSWORD:
      value = "";
      break;
#endif
    case LANGUAGE:
      value = "en";
#ifdef LINUX
      if (getenv("LANG"))
        if (string(getenv("LANG"), 0, 2) == string("de"))
          value = "de";
#endif
#ifdef WINDOWS
      value = DK::Utils::Windows::Reg_read(HKEY_CURRENT_USER,
                                           "Software\\FreeDoko",
                                           "Language");
      if (value.empty())
        value = DK::Utils::Windows::Reg_read(HKEY_LOCAL_MACHINE,
                                             "Software\\FreeDoko",
                                             "Language");

      if (value.empty()) {
        // Ein bisschen kompliziert, aus der Registry die Sprache auszulesen:
        // zuerst wird der Laenderkode gelesen (Deutsch: 407),
        // anschliessend wird geschaut, welche Sprache zu dem Laendercode
        // gehoert.
        string const language_code
          = DK::Utils::Windows::Reg_read(HKEY_CURRENT_USER,
                                         "Control Panel\\International",
                                         "Locale");

        if (!language_code.empty()) {
          if (string(language_code, language_code.size() - 1)
              == "7") { // deutsch
            value = "de";
          } else if (string(language_code, language_code.size() - 1)
                     == "9") { // english
            value = "en";
          } else if (string(language_code, language_code.size() - 1)
                     == "A") { // spanish
            value = "en";
          } else if (string(language_code, language_code.size() - 1)
                     == "C") { // francais
            value = "en";
          } else { // unknown language code
            value = "en";
          } // unknown langugage code
        } // if (!language_code.empty())

      } // if (value.empty())

      if (value.empty()) {
        // *** DK
        // I didn't find the language in the registry, so I use this workaround
        if ((DK::Utils::Windows::Reg_read(HKEY_LOCAL_MACHINE,
                                          "Software\\Microsoft\\Windows\\CurrentVersion",
                                          "ProgramFilesDir") == string("C:\\Programme"))
            && (DK::Utils::Windows::Reg_read(HKEY_LOCAL_MACHINE,
                                             "Software\\Microsoft\\Windows\\CurrentVersion",
                                             "ProgramFilesPath") == string("C:\\Programme")
               )
           )
          value = "de";
      } // if (value.empty())
#endif
      break;
    case CARDSET:
      value = "xskat/french";
      break;
    case CARDS_BACK:
      value = "default";
      break;
    case ICONSET:
      value = "default";
      break;
    case BACKGROUND:
      value = "default";
      break;
    case NAME_FONT:
      value = "Serif Bold Italic 20";
      break;
    case NAME_FONT_COLOR:
      value = "black";
      break;
    case NAME_ACTIVE_FONT_COLOR:
      value = "red";
      break;
    case NAME_RESERVATION_FONT_COLOR:
      value = "blue";
      break;
    case TRICKPILE_POINTS_FONT:
      value = "Serif Bold 24";
      break;
    case TRICKPILE_POINTS_FONT_COLOR:
      value = "black";
      break;
    case POVERTY_SHIFT_ARROW_COLOR:
      value = "black";
      break;
#ifdef USE_SOUND_COMMAND
    case PLAY_SOUND_COMMAND:
#ifdef WINDOWS
#ifdef USE_REGISTRY
      { // Look in the registry for the default program for wav files
        string class_str
          = DK::Utils::Windows::Reg_read(HKEY_CURRENT_USER,
                                         "Software\\CLASSES\\.wav",
                                         "");
        if (class_str.empty())
          class_str = DK::Utils::Windows::Reg_read(HKEY_LOCAL_MACHINE,
                                                   "Software\\CLASSES\\.wav",
                                                   "");
        if (!class_str.empty()) {
          value = DK::Utils::Windows::Reg_read(HKEY_CURRENT_USER,
                                               "Software\\CLASSES\\" + class_str
                                               + "\\shell\\open\\command",
                                               "");
          if (value.empty())
            value = DK::Utils::Windows::Reg_read(HKEY_LOCAL_MACHINE,
                                                 "Software\\CLASSES\\" + class_str
                                                 + "\\shell\\open\\command",
                                                 "");
          if (   (value.size() >= 5)  
              && string(value, value.size() - 4, std::string::npos) == "\"\%1\"")
            value = string(value, 0, value.size() - 4);
        } // if (!class_str.empty())
      } // Look in the registry for the default program for wav files
#else
#endif
      if (value.empty())
        value = "explorer";
#else // #ifdef WINDOWS
      value = "aplay";
#endif // #ifdef WINDOWS
      break;
#endif // #ifdef USE_SOUND_COMMAND

    case BROWSER_COMMAND:
#ifdef LINUX
      value = "echo could not open a browser for"; // if nothing else works
      if (getenv("BROWSER") != NULL) {
        value = string(getenv("BROWSER"));
        break;
      }  // if (getenv("BROWSER") != NULL)

      { // test some browsers and take the first found
        auto const& browsers = {
        "firefox",
        "chromium",
        "google-chrome",
        "epiphany",
        "galeon",
        "mozilla",
        "opera",
        "konqueror",
        "rekonq",
        "midori",
        "arora",
        "web",
        "xterm -e w3m",
        "xterm -e links",
        "xterm -e lynx",
        };

        for (auto const& browser : browsers) {
          if (::system(("test -x /usr/bin/"s + browser
                        + " -o -x /usr/bin/X11/"s + browser
                        + " -o -x /usr/local/bin/"s + browser).c_str())
              == 0) {
            value = browser;
            break;
          }
        }
      } // test some browsers and take the first found
#endif
#ifdef WINDOWS
#ifdef USE_REGISTRY
      { // Look in the registry for the default browser for html
        string class_str
          = DK::Utils::Windows::Reg_read(HKEY_CURRENT_USER,
                                         "Software\\CLASSES\\.html",
                                         "");
        if (class_str.empty())
          class_str = DK::Utils::Windows::Reg_read(HKEY_LOCAL_MACHINE,
                                                   "Software\\CLASSES\\.html",
                                                   "");
        if (!class_str.empty()) {
          value = DK::Utils::Windows::Reg_read(HKEY_CURRENT_USER,
                                               "Software\\CLASSES\\" + class_str
                                               + "\\shell\\open\\command",
                                               "");
          if (value.empty())
            value = DK::Utils::Windows::Reg_read(HKEY_LOCAL_MACHINE,
                                                 "Software\\CLASSES\\" + class_str
                                                 + "\\shell\\open\\command",
                                                 "");
          if (   (value.size() >= 5)  
              && string(value, value.size() - 4, std::string::npos) == "\"\%1\"")
            value = string(value, 0, value.size() - 4);
        } // if (!class_str.empty())
      } // Look in the registry for the default browser for html
#else
#endif
      if (value.empty())
        value = "explorer";
#endif
      break;
    } // switch(type)
  } // if (value.empty())

  switch(type) {
  case CARDSET:
    value = ((*this)(CARDSETS_DIRECTORY) + "/"
             + value);
    break;
  case CARDS_BACK:
    value = ((*this)(CARDS_BACK_DIRECTORY) + "/"
             + value);
    break;
  case ICONSET:
    value = ((*this)(ICONSETS_DIRECTORY) + "/"
             + value);
    break;
  case BACKGROUND:
    value = ((*this)(BACKGROUNDS_DIRECTORY) + "/"
             + value);
    break;
  case NAME:
#ifdef USE_NETWORK_DOKOLOUNGE
  case DOKOLOUNGE_NAME:
  case DOKOLOUNGE_PASSWORD:
#endif
  case LANGUAGE:
  case NAME_FONT:
  case NAME_FONT_COLOR:
  case NAME_ACTIVE_FONT_COLOR:
  case NAME_RESERVATION_FONT_COLOR:
  case TRICKPILE_POINTS_FONT:
  case TRICKPILE_POINTS_FONT_COLOR:
  case POVERTY_SHIFT_ARROW_COLOR:
#ifdef USE_SOUND_COMMAND
  case PLAY_SOUND_COMMAND:
#endif
  case BROWSER_COMMAND:
    break;
  } // switch(type)

  return value;
} // string Setting::operator()(TypeString type) const

/**
 ** -> result
 **
 ** @param     type   setting type
 **
 ** @return    value of setting 'type'
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
string
Setting::operator()(TypeStringConst const type) const
{
  switch(type) {
  case CARDSET_LICENSE:
    return this->value(CARDSET_LICENSE);
  case ICONSET_LICENSE:
    return this->value(ICONSET_LICENSE);
  case PUBLIC_DATA_DIRECTORY:
    return DK::Utils::File::filename_expand(this->value(PUBLIC_DATA_DIRECTORY));
  case PRIVATE_DATA_DIRECTORY:
    return DK::Utils::File::filename_expand(this->value(PRIVATE_DATA_DIRECTORY));
  case SETTINGS_FILE:
    return ((*this)(PRIVATE_DATA_DIRECTORY)
            + "/" + this->value(type));
  case LOGO_FILE:
    return ((*this)(PUBLIC_DATA_DIRECTORY)
            + this->value(type));
  case ICON_FILE:
    return ((*this)(PUBLIC_DATA_DIRECTORY)
            + this->value(type));
  case LANGUAGES_DIRECTORY:
    return this->value(type);
  case LANGUAGE_FILE:
    return ((*this)(LANGUAGES_DIRECTORY) + "/"
            + (*this)(LANGUAGE) + "/"
            + this->value(type));
  case GRAPHIC_EXTENSION:
    return this->value(type);
  case SOUND_EXTENSION:
    return this->value(type);
  case CARDSETS_DIRECTORY:
    return this->value(type);
  case CARDS_DIRECTORY:
    return ((*this)(CARDSET) + "/"
            + this->value(type));
  case CARDS_BACK_DIRECTORY:
    return ((*this)(CARDSET) + "/"
            + this->value(type));
  case ICONS_DIRECTORY:
    return (this->value(ICONSETS_DIRECTORY)
            + "/" + this->value(ICONSET));
  case ICONSETS_DIRECTORY:
    return this->value(type);
  case ICONS_FROM_CARDSET_DIRECTORY:
    return ((*this)(CARDSET) + "/"
            + this->value(type));
  case BACKGROUNDS_DIRECTORY:
    return this->value(type);
#ifdef USE_NETWORK_DOKOLOUNGE
  case DOKOLOUNGE_DIRECTORY:
    return this->value(type);
#endif
  case SOUNDS_DIRECTORY:
    return (this->value(type) + "/" + ::translator.token());
  case AI_DIRECTORY:
    return this->value(type);
  case PARTIES_DIRECTORY:
    return this->value(type);
  case RULES_DIRECTORY:
    return this->value(type);
  case SETTINGS_DIRECTORY:
    return this->value(type);
  case MANUAL_DIRECTORY:
    return this->value(type);
  } // switch(type)

  return "";
} // string Setting::operator()(TypeStringConst type) const

/**
 ** -> result
 **
 ** @param     type   cards order
 **
 ** @return    the cards order
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
Setting::CardsOrder const&
Setting::operator()(TypeCardsOrder type) const
{
  return this->value(type);
} // CardsOrder Setting::operator()(TypeCardsOrder type) const

/**
 ** -> result
 **
 ** @param     type   cards order
 **
 ** @return    the cards order
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
Setting::CardsOrder&
Setting::operator()(TypeCardsOrder const type)
{
  return this->value(type);
} // CardsOrder& Setting::operator()(TypeCardsOrder type)

/**
 ** -> result
 **
 ** @param	name	name of the type
 **
 ** @return	the type with the name 'name';
 **		-1, if the name is not recognized
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.0
 **/
int
Setting::type(string const& name) const
{
  for (int t = BOOL_FIRST; t <= BOOL_LAST; ++t)
    if (name == ::name(TypeBool(t)))
      return t;

  for (int t = UNSIGNED_FIRST; t <= UNSIGNED_LAST; ++t)
    if (name == ::name(TypeUnsigned(t)))
      return t;

  for (int t = STRING_FIRST; t <= STRING_LAST; ++t)
    if (name == ::name(TypeString(t)))
      return t;

  if (name == ::name(CARDS_ORDER))
    return CARDS_ORDER;

  return -1;
} // int Setting::type(string const& name) const

/**
 ** -> result
 **
 ** @param	name	name of the type
 **
 ** @return	the type with the translated name 'name';
 **		-1, if the name is not recognized
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.0
 **/
int
Setting::type_translated(string const& name) const
{
  for (int t = BOOL_FIRST; t <= BOOL_LAST; t++)
    if (name == ::translation(TypeBool(t)))
      return t;

  for (int t = UNSIGNED_FIRST; t <= UNSIGNED_LAST; t++)
    if (name == ::translation(TypeUnsigned(t)))
      return t;

  for (int t = STRING_FIRST; t <= STRING_LAST; t++)
    if (name == ::translation(TypeString(t)))
      return t;

  if (name == ::translation(CARDS_ORDER))
    return CARDS_ORDER;

  return -1;
} // int Setting::type_translated(string const& name) const


/**
 ** -> result
 **
 ** @param	name	name of the type
 **
 ** @return	the type with the translated or normal name 'name';
 **		-1, if the name is not recognized
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.0
 **/
int
Setting::type_translated_or_normal(string const& name) const
{
  int type = this->type_translated(name);
  if (type == -1)
    type = this->type(name);

  return type;
} // int Setting::type_translated_or_normal(string const& name) const

/**
 ** -> result
 **
 ** @param	type   the type (bool)
 **
 ** @return	whether the dependencies of the setting 'type' are met
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.1
 **/
bool
Setting::dependencies(TypeBool const& type) const
{
  switch(type) {
  case AUTOMATIC_SAVINGS:
    return true;
  case SAVE_PARTY_CHANGES:
    return (*this)(AUTOMATIC_SAVINGS);
  case ADDITIONAL_PARTY_SETTINGS:
    return true;
  case SHOW_SPLASH_SCREEN:
    return true;
  case SPLASH_SCREEN_TRANSPARENT:
    return (*this)(SHOW_SPLASH_SCREEN);
  case SHOW_BUG_REPORT_BUTTON_IN_GAME_FINISHED_WINDOW:
  case SAVE_BUG_REPORTS_ON_DESKTOP:
  case SOUND:
#ifdef USE_NETWORK_DOKOLOUNGE
  case DOKOLOUNGE_AUTO_LOGIN:
#endif
  case AUTOMATIC_CARD_SUGGESTION:
  case ANNOUNCE_SWINES_AUTOMATICALLY:
  case SHOW_IF_VALID:
  case EMPHASIZE_VALID_CARDS:
  case ANNOUNCE_IN_TABLE:
  case SHOW_ALL_HANDS:
  case SHOW_AI_INFORMATION_HANDS:
  case SHOW_AI_INFORMATION_TEAMS:
  case SHOW_TRICKPILES_POINTS:
  case SHOW_KNOWN_TEAMS_IN_GAME:
  case SHOW_SOLOPLAYER_IN_GAME:
    // the showing is also dependent of the rule
    return true;
  case SHOW_FULL_TRICK_WINDOW_IF_SPECIAL_POINTS:
    return !(*this)(SHOW_FULL_TRICK_WINDOW);
  case CLOSE_GAMETYPE_WINDOW_AUTOMATICALLY:
    return (*this)(SHOW_GAMETYPE_WINDOW);
  case CLOSE_MARRIAGE_WINDOW_AUTOMATICALLY:
    return (*this)(SHOW_MARRIAGE_WINDOW);
  case CLOSE_GENSCHER_WINDOW_AUTOMATICALLY:
    return (*this)(SHOW_GENSCHER_WINDOW);
  case CLOSE_ANNOUNCEMENT_WINDOW_AUTOMATICALLY:
    return (*this)(SHOW_ANNOUNCEMENT_WINDOW);
  case CLOSE_SWINES_WINDOW_AUTOMATICALLY:
    return (*this)(SHOW_SWINES_WINDOW);
  case SHOW_FULL_TRICK_WINDOW:
  case CLOSE_FULL_TRICK_AUTOMATICALLY:
  case SHOW_GAMETYPE_WINDOW:
  case SHOW_MARRIAGE_WINDOW:
  case SHOW_GENSCHER_WINDOW:
  case SHOW_ANNOUNCEMENT_WINDOW:
  case SHOW_SWINES_WINDOW:
  case NETWORK_CONNECTIONS_URGENCY_HINT:
  case NETWORK_CHAT_URGENCY_HINT:
  case ROTATE_TRICK_CARDS:
  case OWN_HAND_ON_TABLE_BOTTOM:
  case ORIGINAL_CARDS_SIZE:
    return true;
  } // switch(type)

  return true;
} // bool Setting::dependencies(TypeBool const& type) const

/**
 ** -> result
 **
 ** @param	type   the type (unsigned)
 **
 ** @return	whether the dependencies of the setting 'type' are met
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.0
 **/
bool
Setting::dependencies(TypeUnsigned const& type) const
{
  switch(type) {
  case FULL_TRICK_CLOSE_DELAY:
    return (*this)(CLOSE_FULL_TRICK_AUTOMATICALLY);
  case GAMETYPE_WINDOW_CLOSE_DELAY:
    return (*this)(CLOSE_GAMETYPE_WINDOW_AUTOMATICALLY);
  case MARRIAGE_WINDOW_CLOSE_DELAY:
    return (*this)(CLOSE_MARRIAGE_WINDOW_AUTOMATICALLY);
  case GENSCHER_WINDOW_CLOSE_DELAY:
    return (*this)(CLOSE_GENSCHER_WINDOW_AUTOMATICALLY);
  case ANNOUNCEMENT_WINDOW_CLOSE_DELAY:
    return (*this)(CLOSE_ANNOUNCEMENT_WINDOW_AUTOMATICALLY);
  case SWINES_WINDOW_CLOSE_DELAY:
    return (*this)(CLOSE_SWINES_WINDOW_AUTOMATICALLY);
  case CARDS_HEIGHT:
    return !(*this)(ORIGINAL_CARDS_SIZE);
  default:
    return true;
  } // switch(type)

  return true;
} // bool Setting::dependencies(TypeUnsigned const& type) const

/**
 ** -> result
 **
 ** @param	type   the type (string)
 **
 ** @return	whether the dependencies of the setting 'type' are met
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.0
 **/
bool
Setting::dependencies(TypeString const& type) const
{
  (void)type;
  return true;
} // bool Setting::dependencies(TypeString const& type) const

/**
 ** -> result
 **
 ** @param	type	setting type
 **
 ** @return	the value of the setting 'type'
 ** 
 ** @author	Diether Knof
 **
 ** @version	0.7.0
 **/
bool const&
Setting::value(const TypeBool type) const
{
#ifndef POSTPONED
  // *** DK
  // else there is a compiler warning: returning reference to temporary
  static bool value = false;
  value = this->bool_[type - BOOL_FIRST];
  return value;
#else // #ifndef POSTPONED
  return this->bool_[type - BOOL_FIRST];
#endif // #ifndef POSTPONED
} // bool const& Setting::value(const TypeBool type) const

/**
 ** -> result
 **
 ** @param	type	setting type
 **
 ** @return	the value of the (bool) setting 'type'
 ** 
 ** @author	Diether Knof
 **
 ** @version	0.7.0
 **/
unsigned const&
Setting::value(const TypeUnsigned type) const
{
  return this->unsigned_[type - UNSIGNED_FIRST];
} // unsigned const& Setting::value(const TypeUnsigned type)

/**
 ** -> result
 **
 ** @param	type	setting type
 **
 ** @return	the value of the (string) setting 'type'
 ** 
 ** @author	Diether Knof
 **
 ** @version	0.7.0
 **/
string const&
Setting::value(TypeString const type) const
{
  return this->string_[type - STRING_FIRST];
} // string const& Setting::value(TypeString type)

/**
 ** searches the file for 'type' in the directories
 **
 ** @param     type	setting type
 **
 ** @return    the file/directory with 'type'
 **            empty string if not found
 ** 
 ** @author    Diether Knof
 **
 ** @version   0.7.5
 **/
string const&
Setting::path(TypeString const type) const
{
  return this->string_path_[type - STRING_FIRST];
} // string Setting::path(TypeString type) const

/**
 ** searches the file for 'type' in the directories
 **
 ** @param     type	setting type
 **
 ** @return    the file/directory with 'type'
 **            empty string if not found
 ** 
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
string const
Setting::path(TypeStringConst const type) const
{
  switch (type) {
  case CARDSET_LICENSE:
    return "";
  case ICONSET_LICENSE:
    return "";

  case PUBLIC_DATA_DIRECTORY:
  case PRIVATE_DATA_DIRECTORY:
    return this->value(type);

  case SETTINGS_FILE:
    return (this->value(PRIVATE_DATA_DIRECTORY) + "/"
            + this->value(type));

  case LOGO_FILE:
  case ICON_FILE: {
    // search the directory

    vector<string> const datadir = this->data_directories();

    for (vector<string>::const_iterator dd = datadir.begin();
         dd != datadir.end();
         dd++) {
      string const file = (*dd + "/" + this->value(type));
      // check for the file
      ifstream istr(file.c_str());
      if (istr.good()) {
        return file;
      }
    } // for (dd \in datadir)

    return "";
  } // case LOGO_FILE, ICON_FILE

    // the following directories are subdirectories of the
    // public/private data directory
  case LANGUAGE_FILE:
  case LANGUAGES_DIRECTORY:
  case CARDSETS_DIRECTORY:
  case CARDS_DIRECTORY:
  case CARDS_BACK_DIRECTORY:
  case ICONS_FROM_CARDSET_DIRECTORY:
  case ICONSETS_DIRECTORY:
  case ICONS_DIRECTORY:
  case BACKGROUNDS_DIRECTORY:
#ifdef USE_NETWORK_DOKOLOUNGE
  case DOKOLOUNGE_DIRECTORY:
#endif
  case SOUNDS_DIRECTORY:
  case AI_DIRECTORY:
  case PARTIES_DIRECTORY:
  case RULES_DIRECTORY:
  case SETTINGS_DIRECTORY:
  case MANUAL_DIRECTORY:
    return "";

  case GRAPHIC_EXTENSION:
  case SOUND_EXTENSION:
    return "";
  } // switch(type)

  return "";
} // string const Setting::path(TypeStringConst type) const

/**
 ** search a cards back
 **
 ** @param     -
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
void
Setting::search_cards_back()
{
  if (!this->path(CARDS_BACK).empty())
    return ;

  string new_back;

  { // search the file from the settings
    string const file = (this->path(CARDSET) + "/"
                         + this->value(CARDS_BACK_DIRECTORY) + "/"
                         + this->value(CARDS_BACK)
                         + "." + (*this)(GRAPHIC_EXTENSION));
    // check for the file
    ifstream istr(file.c_str());
    if (istr.good()) {
      new_back = file;
    }

    if (new_back.empty()) {

      vector<string> const datadir = this->data_directories();

      for (vector<string>::const_iterator dd = datadir.begin();
           dd != datadir.end();
           dd++) {
        string const file = (*dd + "/" + (*this)(CARDSET) + "/"
                             + this->value(CARDS_BACK_DIRECTORY) + "/"
                             + this->value(CARDS_BACK)
                             + "." + (*this)(GRAPHIC_EXTENSION));
        // check for the file
        ifstream istr(file.c_str());
        if (istr.good()) {
          new_back = file;
          break;
        }
      } // for (dd \in datadir)
    } // if (new_back.empty())

  } // search the file from the settings


  vector<string> const datadir = this->data_directories();

  // search any back
  for (vector<string>::const_iterator dd = datadir.begin();
       dd != datadir.end();
       ++dd) {
    string const directory = (*dd + "/" + (*this)(CARDS_BACK_DIRECTORY));

    DIR *dir = opendir(directory.c_str());

    if (dir == NULL)
      continue;

    if (dir != NULL) {
      struct dirent *entry;

      // search all entries in the directory
      while ((entry = readdir(dir)) != NULL) {
        if ((strcmp(entry->d_name, ".") == 0)
            || (strcmp(entry->d_name, "..") == 0))
          continue;
        if (DK::Utils::File::isfile(directory + "/" + entry->d_name)) {
          string const basename = DK::Utils::File::basename(entry->d_name);
          if ((basename.size()
               > 1 + (*this)(Setting::GRAPHIC_EXTENSION).size())
              && (string(basename,
                         basename.size()
                         - 1
                         - (*this)(Setting::GRAPHIC_EXTENSION).size(),
                         1 + (*this)(Setting::GRAPHIC_EXTENSION).size())
                  == "." + (*this)(Setting::GRAPHIC_EXTENSION))
             ) {
            new_back = string(basename,
                              0,
                              basename.size()
                              - 1 - (*this)(GRAPHIC_EXTENSION).size());
            break;
          } // if (extension is right)
        } // if (DK::Utils::File::isfile(entry->d_name))
      } // while (entry != NULL) ;
      closedir(dir);
    } // if (dir != NULL)
  } // for (dd \in datadir)

  if (new_back.empty()) {
    if (::ui)
      ::ui->error(::translation("Error::could not find any back for cardset %scardset%", this->value(CARDSET)).translation());
  } else {
    this->set(Setting::CARDS_BACK, new_back);
  }

  return ;
} // void Setting::search_cards_back()

/**
 ** search a background
 **
 ** @param     -
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
void
Setting::search_background()
{
  if (!this->path(BACKGROUND).empty())
    return ;

  string new_background;

  vector<string> const datadir = this->data_directories();

  // search any background
  for (vector<string>::const_iterator dd = datadir.begin();
       dd != datadir.end();
       ++dd) {
    string const directory = (*dd + "/" + (*this)(BACKGROUNDS_DIRECTORY));

    DIR *dir = opendir(directory.c_str());

    if (dir == NULL)
      continue;

    if (dir != NULL) {
      struct dirent *entry;

      // search all entries in the directory
      while ((entry = readdir(dir)) != NULL) {
        if ((strcmp(entry->d_name, ".") == 0)
            || (strcmp(entry->d_name, "..") == 0))
          continue;
        if (DK::Utils::File::isfile(directory + "/" + entry->d_name)) {
          string const basename = DK::Utils::File::basename(entry->d_name);
          if ((basename.size()
               > 1 + (*this)(Setting::GRAPHIC_EXTENSION).size())
              && (string(basename,
                         basename.size()
                         - 1
                         - (*this)(Setting::GRAPHIC_EXTENSION).size(),
                         1 + (*this)(Setting::GRAPHIC_EXTENSION).size())
                  == "." + (*this)(Setting::GRAPHIC_EXTENSION))
             ) {
            new_background = string(basename,
                                    0,
                                    basename.size()
                                    - 1 - (*this)(GRAPHIC_EXTENSION).size());
            break;
          } // if (extension is right)
        } // if (DK::Utils::File::isfile(entry->d_name))
      } // while (entry != NULL) ;
      closedir(dir);
    } // if (dir != NULL)
  } // for (dd \in datadir)

  if (new_background.empty()) {
    if (::ui)
      ::ui->error(::translation("Error::could not find any background"
                               ).translation());
  } else {
    this->set(Setting::BACKGROUND, new_background);
  }

  return ;
} // void Setting::search_background()

#ifdef USE_NETWORK_DOKOLOUNGE
/**
 ** -> result
 **
 ** @param     subdir   subdirectory with the corresponding icon
 ** @param     icon     icon name
 **
 ** @return    path to the dokolounge icon
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.12
 **/
string 
Setting::dokolounge_icon(string const& subdir, string const& icon) const
{
  string icon_path;

  vector<string> const datadir = this->data_directories();

  for (vector<string>::const_iterator dd = datadir.begin();
       dd != datadir.end();
       ++dd) {
    string const directory = (*dd + "/" + (*this)(DOKOLOUNGE_DIRECTORY)
                              + "/" + subdir);

    DIR *dir = opendir(directory.c_str());

    if (dir == NULL)
      continue;

    if (dir != NULL) {
      struct dirent *entry;

      // search all entries in the directory
      while ((entry = readdir(dir)) != NULL) {
        if ((strcmp(entry->d_name, ".") == 0)
            || (strcmp(entry->d_name, "..") == 0))
          continue;
        if (DK::Utils::File::isfile(directory + "/" + entry->d_name)) {
          string const basename = DK::Utils::File::basename(entry->d_name);
          if (basename == icon)
            return directory + "/" + basename;
        } // if (DK::Utils::File::isfile(entry->d_name))
      } // while (entry != NULL) ;
      closedir(dir);
    } // if (dir != NULL)
  } // for (dd \in datadir)

  return "";
} // string Setting::dokolounge_icon(string subdir, string icon) const

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    path to the dokolounge logo
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.13
 **/
string 
Setting::dokolounge_logo() const
{
  return this->dokolounge_icon(".", "gesichter3.gif");
} // string Setting::dokolounge_logo() const
/**
 ** -> result
 **
 ** @param     icon   icon name
 **
 ** @return    path to the dokolounge player icon
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.13
 **/
string 
Setting::dokolounge_player_icon(string const& icon) const
{
  return this->dokolounge_icon("faces", icon);
} // string Setting::dokolounge_player_icon(string const& icon) const

/**
 ** -> result
 **
 ** @param     icon   icon name
 **
 ** @return    path to the dokolounge table icon
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.13
 **/
string 
Setting::dokolounge_table_icon(string const& icon) const
{
  return this->dokolounge_icon(".", icon);
} // string Setting::dokolounge_table_icon(string const& icon) const
#endif

/**
 ** -> result
 **
 ** @param	type	setting type
 **
 ** @return	the value of the (string) setting 'type'
 ** 
 ** @author	Diether Knof
 **
 ** @version	0.7.0
 **/
string const
Setting::value(TypeStringConst const type) const
{
  switch(type) {
  case CARDSET_LICENSE:
    return this->cardset_license_;
  case ICONSET_LICENSE:
    return this->iconset_license_;
  case PUBLIC_DATA_DIRECTORY:
    {
      string value;

#ifdef PUBLIC_DATA_DIRECTORY_VALUE
      value = PUBLIC_DATA_DIRECTORY_VALUE;
#else // !#ifdef PUBLIC_DATA_DIRECTORY_VALUE

#ifdef RELEASE

#ifndef WINDOWS
      value = "/usr/share/games/FreeDoko";
#else // !#ifndef WINDOWS
      value = DK::Utils::Windows::Reg_read(HKEY_LOCAL_MACHINE,
                                           "Software\\FreeDoko",
                                           "Directory");
#endif // #ifndef WINDOWS

#else // !#ifdef RELEASE
      value = "../data";
#endif // !#ifdef RELEASE

      { // test that the directory exists
        struct stat status;
        stat(value.c_str(), &status);
        if (!S_ISDIR(status.st_mode))
          value = ".";
      } // test that the directory exists

#endif // !#ifdef PUBLIC_DATA_DIRECTORY_VALUE
      if (value.empty())
        value = ".";
      return value;
    }
    break;
  case PRIVATE_DATA_DIRECTORY:
    {
      string value;
#if defined(LINUX) || defined(HPUX)
      value = "~/.FreeDoko";
#endif
#ifdef WINDOWS
      value = "~/FreeDoko";
#endif
      return DK::Utils::File::filename_expand(value);
    }
  case SETTINGS_FILE:
    return "FreeDokorc";
  case LOGO_FILE:
    return "logo.png";
  case ICON_FILE:
    return "icon.png";
  case LANGUAGES_DIRECTORY:
    return "translations";
  case LANGUAGE_FILE:
    return "text";
  case GRAPHIC_EXTENSION:
    return "png";
#ifdef POSTPONED
    // When the program is started (and 'Setting::default_to_hardcoded()' is
    // called) the ui type is not yet known.
    if (::ui) {
      UI* ui = ::ui;
      while (ui->type() == UI_TYPE::WRAP) {
        ui = ((UI_Wrap*)(ui))->ui;
      } // while (ui->type == UI_TYPE::WRAP)

      switch(ui->type()) {
      case UI_TYPE::DUMMY:
#ifdef POSTPONED
        DEBUG_ASSERTION(false,
                        "Call of 'setting.value(GRAPHIC_EXTENSION)'"
                        " with 'ui.type() == DUMMY'");
        break;
#endif
        return "";
      case UI_TYPE::WRAP:
        return "";
#ifdef USE_UI_TEXT
      case UI_TYPE::TEXT:
        return "txt";
#endif
#ifdef USE_UI_AATEXT
      case UI_TYPE::AATEXT:
#endif 
        return "txt";
#ifdef USE_UI_GTKMM
      case UI_TYPE::GTKMM_DOKO:
        return "png";
#endif // #ifdef UISE_UI_GTKMM
      } // switch(::ui->type())
    } // if (::ui)
#endif // #ifdef POSTPONED
    return "";
  case SOUND_EXTENSION:
#ifdef DKNOF
    //return "ogg";
#endif
    return "wav";
  case CARDSETS_DIRECTORY:
    return "cardsets";
  case CARDS_DIRECTORY:
    return "cards";
  case CARDS_BACK_DIRECTORY:
    return "backs";
  case ICONS_FROM_CARDSET_DIRECTORY:
    return "icons";
  case ICONSETS_DIRECTORY:
    return "iconsets";
  case ICONS_DIRECTORY:
    return "default";
  case BACKGROUNDS_DIRECTORY:
    return "backgrounds";
#ifdef USE_NETWORK_DOKOLOUNGE
  case DOKOLOUNGE_DIRECTORY:
    return "Doko-Lounge";
#endif
  case SOUNDS_DIRECTORY:
    return "sounds";
  case AI_DIRECTORY:
    return "ai";
  case PARTIES_DIRECTORY:
    return "parties";
  case RULES_DIRECTORY:
    return "rules";
  case SETTINGS_DIRECTORY:
    return "settings";
  case MANUAL_DIRECTORY:
#ifdef MANUAL_DIRECTORY_VALUE
    return MANUAL_DIRECTORY_VALUE;
#endif // !#ifdef MANUAL_DIRECTORY_VALUE

    if (DK::Utils::File::isdirectory("../manual"))
      return "../manual";
    else if (   DK::Utils::File::isdirectory("../trunk")
             && DK::Utils::File::isdirectory("../../manual"))
      return "../../manual";
    else
      return "../manual";
  } // switch(type)

  return "";
} // string const Setting::value(TypeStringConst const type)

/**
 ** -> result
 **
 ** @param	type	setting type (only 'CARDS_ORDER')
 **
 ** @return	the value of the cards order
 ** 
 ** @author	Diether Knof
 **
 ** @version	0.7.0
 **/
Setting::CardsOrder const&
Setting::value(TypeCardsOrder const type) const
{
  return *this->cards_order_;
} // Setting::CardsOrder Setting::value(TypeCardsOrder type) const

/**
 ** -> result
 **
 ** @param	type	setting type (only 'CARDS_ORDER')
 **
 ** @return	the value of the cards order
 ** 
 ** @author	Diether Knof
 **
 ** @version	0.7.0
 **/
Setting::CardsOrder&
Setting::value(TypeCardsOrder const type)
{
  return *this->cards_order_;
} // Setting::CardsOrder& Setting::value(TypeCardsOrder type)

/**
 ** -> result
 **
 ** @param	type	setting type (Type Unsigned)
 **
 ** @return	the minimal value of the setting 'type'
 ** 
 ** @author	Diether Knof
 **
 ** @version	0.7.0
 **/
unsigned
Setting::min(TypeUnsigned const& type) const
{
  switch(type) {
#ifdef USE_THREADS
  case THREADS_MAX:
    return 1;
#endif
  case TABLE_ROTATION:
    return 0;
  case CARDS_HEIGHT:
    return 2;
  default:
    return 0;
  } // switch(type)
} // unsigned Setting::min(TypeUnsigned const& type) const

/**
 ** -> result
 **
 ** @param	type	setting type (Type Unsigned)
 **
 ** @return	the maximal value of the setting 'type'
 ** 
 ** @author	Diether Knof
 **
 ** @version	0.7.0
 **/
unsigned
Setting::max(TypeUnsigned const& type) const
{
  switch(type) {
#ifdef USE_THREADS
  case THREADS_MAX:
    return 12;
#endif
  case CARD_PLAY_DELAY:
    return 60000; // 60 seconds
  case FULL_TRICK_CLOSE_DELAY:
  case GAMETYPE_WINDOW_CLOSE_DELAY:
  case MARRIAGE_WINDOW_CLOSE_DELAY:
  case GENSCHER_WINDOW_CLOSE_DELAY:
  case ANNOUNCEMENT_WINDOW_CLOSE_DELAY:
  case SWINES_WINDOW_CLOSE_DELAY:
    return 60000; // 60 seconds
  case TABLE_ROTATION:
    return 3;
  case CARDS_HEIGHT:
    return 1000;
  } // switch(type)

  return 0;
} // unsigned Setting::max(TypeUnsigned const& type) const


/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    the themes list
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
vector<Setting::Theme*> const&
Setting::themes() const
{
  return this->themes_;
} // vector<Setting::Theme*> const& Setting::themes() const

/**
 ** -> result
 **
 ** @param     name   name of the theme
 **
 ** @return    the theme with the name 'name'
 **            'NULL' if none is found
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
Setting::Theme const*
Setting::theme(string const& name) const
{
  for (vector<Theme*>::const_iterator t = this->themes().begin();
       t != this->themes().end();
       ++t)
    if ((*t)->name() == name)
      return *t;

  return NULL;
} // Setting::Theme const* Setting::theme(string const& name) const

/**
 ** sets the 'type' to 'value'
 **
 ** @param	type	setting type (Type Bool)
 ** @param	value	new value
 **
 ** @return	-
 ** 
 ** @author	Diether Knof
 **
 ** @version	0.7.0
 **/
void
Setting::set(TypeBool const type, bool const value)
{
  if (this->value(type) == value)
    return ;

  bool const old_value = this->bool_[type - BOOL_FIRST];
  this->bool_[type - BOOL_FIRST] = value;

  if (   ::ui
      && (this == &::setting))
    ::ui->setting_changed(type, &old_value);

  return ;
} // void Setting::set(TypeBool type, bool value)

/**
 ** sets the 'type' to 'value'
 **
 ** @param	type	setting type (Type Bool)
 ** @param	value	new value (as string)
 **
 ** @return	-
 ** 
 ** @author	Diether Knof
 **
 ** @version	0.7.0
 **/
void
Setting::set(TypeBool const type, string const& value)
{
  if ((value != "true")
      && (value != "false")
      && (value != "yes")
      && (value != "no")
      && (value != "0")
      && (value != "1")) {
    cerr << "illegal value '" << value << "' for '" << type << "', "
      << "must be a boolean ('true' or 'false' or 'yes' or 'no' or '1' or '0'. "
      << "Taking 'false'."
      << endl;
  }

  this->set(type, ((   (value == "false")
                    || (value == "no")
                    || (value == "0"))
                   ? false : true));
  return ;
} // void Setting::set(TypeBool type, string value)

/**
 ** sets the 'type' to 'value'
 **
 ** @param	type	setting type (Type Unsigned)
 ** @param	value	new value
 **
 ** @return	-
 ** 
 ** @author	Diether Knof
 **
 ** @version	0.7.0
 **/
void
Setting::set(TypeUnsigned const type, unsigned const value)
{
  if (this->value(type) == value)
    return ;

  unsigned const old_value = this->unsigned_[type - UNSIGNED_FIRST];
  this->unsigned_[type - UNSIGNED_FIRST] = value;

  if (   ui
      && (this == &::setting))
    ::ui->setting_changed(type, &old_value);

  return ;
} // void Setting::set(TypeUnsigned type, unsigned value)

/**
 ** sets the 'type' to 'value'
 **
 ** @param	type	setting type (Type Unsigned)
 ** @param	value	new value (as string)
 **
 ** @return	-
 ** 
 ** @author	Diether Knof
 **
 ** @version	0.7.0
 **/
void
Setting::set(TypeUnsigned const type, string const& value)
{
  char* end_ptr;
  unsigned number = strtoul(value.c_str(), &end_ptr, 0);
  if (*end_ptr != '\0') {
    cerr << "illegal value '" << value << "' for '" << type << "', "
      << "must be a digit. "
      << "Taking " << number << "."
      << endl;
  }

  this->set(type, number);
  return ;
} // void Setting::set(TypeUnsigned type, string value)

/**
 ** sets the 'type' to 'value'
 **
 ** @param	type	setting type (Type String)
 ** @param	value	new value
 **
 ** @return	-
 ** 
 ** @author	Diether Knof
 **
 ** @version	0.7.0
 **/
void
Setting::set(TypeString const type, string const& value)
{
  if (this->value(type) == value)
    return ;

  string const old_value = this->string_[type - STRING_FIRST];

  // First test, whether this is the default value.
  // If it is, then leave the value empty.
#ifdef POSTPONED
  this->string_[type - STRING_FIRST] = "";
#endif // #ifdef POSTPONED 

  this->string_[type - STRING_FIRST] = value;

  this->update_path(type);

  { // check for path values
    switch(type) {
    case BACKGROUND:
    case CARDS_BACK:
    case CARDSET:
    case ICONSET:
    case LANGUAGE:
      if (this->path(type).empty()) {
        this->string_[type - STRING_FIRST] = old_value;
        return ;
      }
      break;
    default:
      break;
    } // switch (type)
  } // check for path values

  if (this == &::setting) {
    switch(type) {
    case NAME:
      break;
#ifdef USE_NETWORK_DOKOLOUNGE
    case DOKOLOUNGE_NAME:
    case DOKOLOUNGE_PASSWORD:
#endif
    case LANGUAGE:
      ::translator.load();
      break;
    case CARDSET:
    case ICONSET:
      { // read the license
        vector<string> license_files;
        license_files.push_back("copyright");
        license_files.push_back("Copyright");
        license_files.push_back("COPYRIGHT");
        license_files.push_back("license");
        license_files.push_back("License");
        license_files.push_back("LICENSE");
        license_files.push_back("copyright.txt");
        license_files.push_back("Copyright.txt");
        license_files.push_back("COPYRIGHT.txt");
        license_files.push_back("license.txt");
        license_files.push_back("License.txt");
        license_files.push_back("LICENSE.txt");

        string directory = this->path(type);

        string license_text;

        for (vector<string>::const_iterator license
             = license_files.begin();
             ((license != license_files.end())
              && license_text.empty());
             license++) {

          if (DK::Utils::File::isfile(directory + "/"
                                      + *license)) {
            license_text = DK::Utils::String::getfile(directory + "/"
                                                      + *license);
          } else if (DK::Utils::File::isfile(directory + "/../"
                                             + *license)) {
            license_text = DK::Utils::String::getfile(directory + "/../"
                                                      + *license);
          } // if (license found)
        } // for (license \in license_files)

        if (license_text.empty())
          // no license found
          license_text = "license not found :-(";

        if (type == CARDSET) {
          this->cardset_license_ = license_text;
        } else {
          this->iconset_license_ = license_text;
        }
      } // read the license
      if (type == CARDSET) {
        this->search_cards_back();
      }
      break;

    default:
      break;
    } // switch(type)

    if (ui)
      ::ui->setting_changed(type, &old_value);
  } // if (this == &::setting)

  return ;
} // void Setting::set(const TypeString type, string const& value)

/**
 ** sets the 'type' to 'value'
 **
 ** @param	type	setting type (Type CardsOrder)
 ** @param	value	new value
 **
 ** @return	-
 ** 
 ** @author	Diether Knof
 **
 ** @version	0.7.0
 **/
void
Setting::set(TypeCardsOrder const type, CardsOrder const& value)
{
  if (this->value(type) == value) {
    return ;
  }

  CardsOrder const old_value = *this->cards_order_;

  // the following code keeps the sorting order when the cards are mixed,
  if (!value.sorted()) {
    this->cards_order_->sorted_set(value.sorted());
  } else { // if !(!value.sorted())
    *this->cards_order_ = value;

    if (this == &::setting) {
      if (game_status & GAMESTATUS::GAME)
        ::party.game().players_hands_sort();

      if (ui)
        ::ui->setting_changed(type, &old_value);
    } // if (this == &::setting)
  } // if !(!value.sorted())

  return ;
} // void Setting::set(TypeCardsOrder type, CardsOrder value)

/**
 ** sets the 'type' to 'value'
 **
 ** @param	type	setting type (Type CardsOrder)
 ** @param	value	new value (as string)
 **
 ** @return	-
 ** 
 ** @author	Diether Knof
 **
 ** @version	0.7.0
 **/
void
Setting::set(TypeCardsOrder const type, string const& value)
{
  this->set(type, CardsOrder(value));
  return ;
} // void Setting::set(TypeCardsOrder type, string value)

/**
 ** sets the 'type' to 'value'
 ** the parameters are strings
 **
 ** @param	type	name of the setting type
 ** @param	value	new value
 **
 ** @return	whether the setting was successful
 ** 
 ** @author	Diether Knof
 **
 ** @version	0.7.0
 **/
bool
Setting::set(string const& type, string const& value)
{
  for (int i = Setting::BOOL_FIRST; i <= Setting::BOOL_LAST; i++) {
    if ((type == ::name(Setting::TypeBool(i)))
        || (type == ::translation(Setting::TypeBool(i)))) {
      this->set(Setting::TypeBool(i), value);
      return true;
    }
  }
  for (int i = Setting::UNSIGNED_FIRST; i <= Setting::UNSIGNED_LAST; i++) {
    if ((type == ::name(Setting::TypeUnsigned(i)))
        || (type == ::translation(Setting::TypeUnsigned(i)))) {
      this->set(Setting::TypeUnsigned(i), value);
      return true;
    }
  }
  for (int i = Setting::STRING_FIRST; i <= Setting::STRING_LAST; i++) {
    if ((type == ::name(Setting::TypeString(i)))
        || (type == ::translation(Setting::TypeString(i)))) {
      this->set(Setting::TypeString(i), value);
      return true;
    }
  }

  if ((type == ::name(Setting::CARDS_ORDER))
      || (type == ::translation(Setting::CARDS_ORDER))) {
    this->set(CARDS_ORDER, value);
    return true;
  }

  return false;
} // bool Setting::set(string const& type, string const& value)

/**
 ** set the values from the theme
 **
 ** @param     theme   theme to set
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
void
Setting::set(Theme const& theme)
{
  for (map<TypeBool, bool>::const_iterator t = theme.bool_.begin();
       t != theme.bool_.end();
       ++t)
    this->set(t->first, t->second);
  for (map<TypeUnsigned, unsigned>::const_iterator t = theme.unsigned_.begin();
       t != theme.unsigned_.end();
       ++t)
    this->set(t->first, t->second);
  for (map<TypeString, string>::const_iterator t = theme.string_.begin();
       t != theme.string_.end();
       ++t)
    this->set(t->first, t->second);

  return ;
} // void Setting::set(Theme theme)

/**
 ** set the values from the theme 'name'
 **
 ** @param     name   name of the theme
 **
 ** @return    whether the theme was found
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
bool
Setting::set_theme(string const& name)
{
  Theme const* theme = this->theme(name);
  if (!theme)
    return false;

  this->set(*theme);
  return true;
} // bool Setting::set_theme(string const& name)

/**
 ** -> result
 **
 ** @param     playerno   the number of the player
 **
 ** @return    the position of the player playerno
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.13
 **/
Position
Setting::position(unsigned const playerno) const
{
  int const rotation
    = ( (   ::setting(Setting::OWN_HAND_ON_TABLE_BOTTOM)
         && (::party.humanno() == 1)
         && ::game_status & GAMESTATUS::GAME
         && (::party.game().no(*::party.human_player()) != UINT_MAX))
       ? ::party.game().no(*::party.human_player())
       : ::setting(Setting::TABLE_ROTATION) );

  switch ( (playerno + 4 - rotation) % 4) {
  case 0:
    return Position::SOUTH;
  case 1:
    return Position::WEST;
  case 2:
    return Position::NORTH;
  case 3:
    return Position::EAST;
  default:
    DEBUG_ASSERTION(false,
                    "Table::position(playerno):\n"
                    "  player number " << playerno
                    << " invalid.");
    break;
  } // switch(playerno)

  return Position::NORTH;
} // Position Table::position(unsigned playerno) const

/**
 ** load the settings from the default file
 **
 ** @param	-
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.0
 **/
void
Setting::load()
{
  this->load((*this)(PUBLIC_DATA_DIRECTORY) + "/" + this->value(SETTINGS_FILE),
             false);
  this->load((*this)(SETTINGS_FILE), false);

  return ;
} // void Setting::load()

/**
 ** load the settings from the given file
 **
 ** @param	filename	file to load the settings from
 ** @param	filename_not_exists_output	whether to show an information
 **						if the file could not be opened
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.0
 **/
void
Setting::load(string const& filename, bool filename_not_exists_output)
{
  ifstream istr(filename.c_str());

  if (istr.fail()) {
    if (filename_not_exists_output)
      cerr << "Setting::load(" << filename << "):\n"
        << "  Error while opening the file"
        << endl;
    return ;
  }

  // load the settings
  while (istr.good()) {
    Config config;
    istr >> config;

    // finished with the config file
    if ((config.name == "") && (config.value == ""))
      break;

    if (config.separator) {
      // a setting
      if (!set(config.name, config.value)) {
        cerr << "setting file:	"
          << "ignoring unknown setting '" << config.name << "'."
          << endl;
      }
    } else { // if (config.separator)
      // a setting
      // if the value is in parentencies, remove both
      if (config.name == "!input") {
        // include the given file
        this->load((DK::Utils::File::dirname(filename) + "/"
                    + config.value));
      } else if (config.name == "!end") {
        // ignore the rest of the file
        break;
      } else if (config.name == "!stdout") {
        // output of the data to 'stdout'
        cout << config.value << endl;
      } else if (config.name == "!stderr") {
        // output of the data to 'stderr'
        cerr << config.value << endl;
      } else if (config.name == "") {
        cerr << "Setting file \'" << filename << "\':	"
          << "Ignoring line \'" << config.value << "\'.\n";
      } else {
        cerr << "Setting file \'" << filename << "\':	"
          << "Setting '" << config.name << "' unknown.\n"
          << "Ignoring it.\n";
      } // if (config.name == .)
    } // config.separator

  } // while (istr.good())

  return ;
} // void Setting::load(string const& filename, bool filename_not_exists_output)

/**
 ** check whether a cardset is outdated and replaces it
 **
 ** @param	-
 **
 ** @return	-
 **/
void
Setting::check_for_outdated_cardset()
{
  // since version 0.7.12 (from 2014)
  // replace Altenburg cardset with InnoCard
  auto const cardset_old = this->value(CARDSET);
  bool changed = false;
  if (this->value(CARDSET) == "InnoCard/french") {
    this->set(CARDSET, "InnoCard/french/185");
    changed = true;
  }
  //if (string(this->value(CARDSET), 0, 8) == "InnoCard") {
    //this->set(CARDSET, "xskat/french");
    //changed = true;
  //}
  if (this->value(CARDSET) == "Altenburg/english") {
    this->set(CARDSET, "InnoCard/english/185");
    //this->set(CARDSET, "dondorf");
    changed = true;
  }
  if (this->value(CARDSET) == "Altenburg/german") {
    this->set(CARDSET, "xskat/german");
    changed = true;
  }
  if (string(this->value(CARDSET), 0, 10) == "Altenburg") {
    this->set(CARDSET, "InnoCard/french/185");
    //this->set(CARDSET, "xskat/french");
    changed = true;
  }
  if (changed && (this->value(CARDSET) == cardset_old)) {
    this->set(CARDSET, "xskat/french");
  }
  if (changed) {
    ::ui->information(::translation("cardset replaced: %sorig%, %sreplacement%", cardset_old, this->value(CARDSET)), INFORMATION::PROBLEM, true);
  }
} // void Setting::check_for_outdated_cardset()

/**
 ** save the settings in the default file
 **
 ** @param	-
 **
 ** @return	whether the saving was successful
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
bool
Setting::save() const
{
  return this->save((*this)(SETTINGS_FILE));
} // bool Setting::save() const

/**
 ** save the settings in the given file
 **
 ** @param	filename	file into which to save the settings
 **
 ** @return	whether the saving was successful
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
bool
Setting::save(string const& filename) const
{
  string filename_tmp = filename + ".tmp";
  ofstream ostr(filename_tmp.c_str());
  if (!ostr.good()) {
    ::ui->information(::translation("Error::Setting::save: Error opening temporary file %stmpfile%. Aborting saving.", filename_tmp), INFORMATION::PROBLEM);
    return false;
  }

  ostr << "#!FreeDoko -s\n"
    << "# FreeDoko settings file (version " << *::version << ")\n"
    << '\n';
  this->write(ostr);

  if (!ostr.good()) {
    ::ui->information(::translation("Error::Setting::save: Error saving in temporary file %stmpfile%. Keeping temporary file (for bug tracking).", filename_tmp), INFORMATION::PROBLEM);
    return false;
  }
  ostr.close();

#ifdef WINDOWS
  unlink(filename.c_str());
#endif
  if (rename(filename_tmp.c_str(), filename.c_str())) {
    ::ui->information(::translation("Error::Setting::save: Could not rename temporary file %stmpfile% to requested file %sfile%. Keeping temporary file.", filename_tmp, filename), INFORMATION::PROBLEM);
    return false;
  }

  return true;
} // bool Setting::save(string const& filename)

/**
 ** updates all settings in the ui
 **
 ** @param	-
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.0
 **/
void
Setting::update_all()
{
  if (this == &::setting) {
    ::translator.load();

    for (int i = BOOL_FIRST; i <= BOOL_LAST; i++)
      ::ui->setting_changed(i, &(this->value(TypeBool(i))));
    for (int i = UNSIGNED_FIRST; i <= UNSIGNED_LAST; i++)
      ::ui->setting_changed(i, &(this->value(TypeUnsigned(i))));
    for (int i = STRING_FIRST; i <= STRING_LAST; i++)
      ::ui->setting_changed(i, &(this->value(TypeString(i))));
    ::ui->setting_changed(CARDS_ORDER, &(this->value(CARDS_ORDER)));
  } // if (this == &::setting)

  return ;
} // void Setting::update_all()

/**
 ** create the themes list
 **
 ** @param     -
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
void
Setting::create_themes_list()
{
  this->clear_themes_list();

  Theme* theme;
  { // InnoCard french
    theme = new Theme("französisch (InnoCard)");
    theme->set(CARDSET, "InnoCard/french/185");
    theme->set(CARDS_BACK, "penguin");
    theme->set(LANGUAGE, "de");
    theme->set(BACKGROUND, "table");
    this->themes_.push_back(theme);
  } // InnoCard french
  { // xskat german
    theme = new Theme("deutsch (xskat)");
    theme->set(CARDSET, "xskat/german");
    theme->set(CARDS_BACK, "penguin");
    theme->set(LANGUAGE, "de-alt");
    theme->set(BACKGROUND, "table");
    this->themes_.push_back(theme);
  } // xskat german
  { // InnoCard english
    theme = new Theme("english (InnoCard)");
    theme->set(CARDSET, "InnoCard/english/185");
    theme->set(CARDS_BACK, "penguin");
    theme->set(LANGUAGE, "en");
    theme->set(BACKGROUND, "table");
    this->themes_.push_back(theme);
  } // InnoCard english
#if 0
  { // dondorf english
    theme = new Theme("english (dondorf)");
    theme->set(CARDSET, "dondorf");
    theme->set(CARDS_BACK, "penguin");
    theme->set(LANGUAGE, "en");
    this->themes_.push_back(theme);
  } // dondorf english
#endif
#if 0
  { // autumn
    theme = new Theme(::translation("Setting::Theme::autumn"));
    theme->set(CARDS_BACK, "leaves");
    theme->set(BACKGROUND, "grass-with-leaves");
    this->themes_.push_back(theme);
  } // autumn
#endif
  { // christmas
    theme = new Theme(::translation("Setting::Theme::christmas"));
    theme->set(CARDS_BACK, "christmas/candle");
    theme->set(BACKGROUND, "christmas/sky");
    this->themes_.push_back(theme);
  } // christmas

  return ;
} // void Setting::create_themes_list()

/**
 ** clear the themes list
 **
 ** @param     -
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
void
Setting::clear_themes_list()
{
  for (vector<Theme*>::iterator t = this->themes_.begin();
       t != this->themes_.end();
       ++t)
    delete *t;
  this->themes_.clear();

  return ;
} // void Setting::clear_themes_list()

/**
 ** update the path of the string setting 'type'
 **
 ** @param     type	setting type
 **
 ** @return    -
 ** 
 ** @author    Diether Knof
 **
 ** @version   0.7.5
 **/
void
Setting::update_path(TypeString const type)
{
  string const path_bak = this->string_path_[type - STRING_FIRST];

  switch(type) {
  case NAME:
#ifdef USE_NETWORK_DOKOLOUNGE
  case DOKOLOUNGE_NAME:
  case DOKOLOUNGE_PASSWORD:
#endif
  case NAME_FONT:
  case NAME_FONT_COLOR:
  case NAME_ACTIVE_FONT_COLOR:
  case NAME_RESERVATION_FONT_COLOR:
  case TRICKPILE_POINTS_FONT:
  case TRICKPILE_POINTS_FONT_COLOR:
  case POVERTY_SHIFT_ARROW_COLOR:
    break;
  case LANGUAGE: {
    // @todo   all (use the vectors in 'Translator')
    this->string_path_[type - STRING_FIRST] = (*this)(LANGUAGE_FILE);
    break;
  } // case LANGUAGE:

  case CARDSET: {
    // search the directory
    vector<string> subdir;
    subdir.push_back((*this)(CARDSET));
    vector<string> const datadir = this->data_directories();

    for (vector<string>::const_iterator sd = subdir.begin();
         sd != subdir.end();
         sd++) {
#ifdef WORKAROUND
      // check for Altenburg cardset
      if (   (ALTENBURG_EXPIRATION_DATE < DK::Utils::Date() )
          && (sd->find("Altenburg") != string::npos) ) {
        ::ui->information(::translation("License: Altenburg expired"),
                          INFORMATION::WARNING);
        continue;
      } // if (Altenburg license expired)
      // check for InnoCard cardset
      if (   (INNOCARD_EXPIRATION_DATE < DK::Utils::Date() )
          && (sd->find("InnoCard") != string::npos) ) {
        ::ui->information(::translation("License: InnoCard expired"),
                          INFORMATION::WARNING);
        continue;
      } // if (InnoCard license expired)
      // check for Elite cardset
      if (   (ELITE_EXPIRATION_DATE < DK::Utils::Date() )
          && (sd->find("Elite") != string::npos) ) {
        ::ui->information(::translation("License: Elite expired"),
                          INFORMATION::WARNING);
        continue;
      } // if (Elite license expired)
#endif
      for (vector<string>::const_iterator dd = datadir.begin();
           dd != datadir.end();
           dd++) {
        string const directory = *dd + "/" + *sd;
        // check for the file
        if (DK::Utils::File::isfile(directory + "/"
                                    + this->value(CARDS_DIRECTORY) + "/"
                                    + ::name(Card::CLUB) + "/"
                                    + ::name(Card::ACE)
                                    + "." + (*this)(GRAPHIC_EXTENSION)
                                   )) {
          this->string_path_[type - STRING_FIRST] = directory;
          break;
        }
      } // for (dd \in datadir)
      if (!this->string_path_[type - STRING_FIRST].empty())
        break;
    } // for (sd \in subdir)

    this->update_path(CARDS_BACK);
    break;
  } // case CARDSET: 

  case CARDS_BACK: {

    vector<string> backs; // backs to search
    backs.push_back(this->value(CARDS_BACK));
    backs.push_back("penguin");
    backs.push_back("back");
    backs.push_back("back01");
    backs.push_back("deck1");
    for (vector<string>::const_iterator back = backs.begin();
         back != backs.end();
         ++back) {
      string file = (this->path(CARDSET) + "/"
                     + this->value(CARDS_BACK_DIRECTORY) + "/"
                     + *back + "." + (*this)(GRAPHIC_EXTENSION));
      // check for the file
      if (!DK::Utils::File::isfile(file)) {
        vector<string> const datadir = this->data_directories();

        for (vector<string>::const_iterator dd = datadir.begin();
             dd != datadir.end();
             dd++) {
          file = (*dd + "/" + (*this)(CARDSET) + "/"
                  + this->value(CARDS_BACK_DIRECTORY) + "/"
                  + *back + "." + (*this)(GRAPHIC_EXTENSION));
          if (DK::Utils::File::isfile(file)) {
            break;
          }
        } // for (dd \in datadir)
      }

      if (DK::Utils::File::isfile(file)) {
        if (*back == this->value(CARDS_BACK)) {
          this->string_path_[type - STRING_FIRST] = file;
          break;
        } else {
          this->set(CARDS_BACK, *back);
          break;
        }
      }

    } // for (back \in backs)
    break;
  } // case CARDS_BACK:

  case ICONSET: {
    // search a directory
    vector<string> const datadir = this->data_directories();

    for (vector<string>::const_iterator dd = datadir.begin();
         dd != datadir.end();
         dd++) {
      string const dir = (*dd + "/" + (*this)(ICONSET));
      if (DK::Utils::File::isdirectory(dir)) {
        this->string_path_[type - STRING_FIRST] = dir;
        break;
      }
    } // for (dd \in datadir)
    break;
  } // case ICONSET:

  case BACKGROUND: {
    // search the directory

    vector<string> const datadir = this->data_directories();

    for (vector<string>::const_iterator dd = datadir.begin();
         dd != datadir.end();
         dd++) {
      string const file = (*dd + "/" + (*this)(BACKGROUNDS_DIRECTORY) + "/"
                           + this->value(BACKGROUND)
                           + "." + (*this)(GRAPHIC_EXTENSION)
                          );
      // check for the file
      if (DK::Utils::File::isfile(file)) {
        this->string_path_[type - STRING_FIRST] = file;
        break;
      }
    } // for (dd \in datadir)

    break;
  } // case BACKGROUND:

#ifdef USE_SOUND_COMMAND
  case PLAY_SOUND_COMMAND:
    this->string_path_[type - STRING_FIRST] = (*this)(type);
    break ;
#endif
  case BROWSER_COMMAND:
    this->string_path_[type - STRING_FIRST] = (*this)(type);
    break ;
  } // switch(type)

  if (this->string_path_[type - STRING_FIRST].empty())
    this->string_path_[type - STRING_FIRST] = path_bak;

  return ;
} // void Setting::update_path(TypeString type)

/**
 ** prints the settings in the output stream
 **
 ** @param	ostr	ouput stream
 **
 ** @return	output stream
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.0
 **/
ostream&
Setting::write(ostream& ostr) const
{
  std::ios_base::fmtflags const flags = ostr.flags();
  ostr << std::boolalpha;

  ostr << ::name(NAME) << " = "
    << this->value(NAME) << "\n"

    << ::name(LANGUAGE) << " = "
    << this->value(LANGUAGE) << "\n"

#ifdef USE_NETWORK_DOKOLOUNGE
    << ::name(DOKOLOUNGE_NAME) << " = "
    << this->value(DOKOLOUNGE_NAME) << "\n"
    << ::name(DOKOLOUNGE_PASSWORD) << " = "
    << this->value(DOKOLOUNGE_PASSWORD) << "\n"
    << ::name(DOKOLOUNGE_AUTO_LOGIN) << " = "
    << this->value(DOKOLOUNGE_AUTO_LOGIN) << "\n"
#endif

    << ::name(SOUND) << " = "
    << this->value(SOUND) << "\n"

#ifdef USE_SOUND_COMMAND
    << ::name(PLAY_SOUND_COMMAND) << " = "
    << this->value(PLAY_SOUND_COMMAND) << "\n"
#endif

    << ::name(BROWSER_COMMAND) << " = "
    << this->value(BROWSER_COMMAND) << "\n"

    << ::name(SHOW_BUG_REPORT_BUTTON_IN_GAME_FINISHED_WINDOW) << " = "
    << this->value(SHOW_BUG_REPORT_BUTTON_IN_GAME_FINISHED_WINDOW) << "\n"

    << ::name(SAVE_BUG_REPORTS_ON_DESKTOP) << " = "
    << this->value(SAVE_BUG_REPORTS_ON_DESKTOP) << "\n"

#ifdef USE_THREADS
    << ::name(THREADS_MAX) << " = "
    << this->value(THREADS_MAX) << "\n"
#endif

    << "\n"
    << "# party settings\n"
    << ::name(ADDITIONAL_PARTY_SETTINGS) << " = "
    << this->value(ADDITIONAL_PARTY_SETTINGS) << "\n"


    << "\n"
    << "# information and delay\n"

    << ::name(CARD_PLAY_DELAY) << " = "
    << ((this->value(CARD_PLAY_DELAY) == UINT_MAX)
        ? "-1"
        : DK::Utils::String::to_string(this->value(CARD_PLAY_DELAY)).c_str())
    << "\n"

    << ::name(AUTOMATIC_CARD_SUGGESTION) << " = "
    << this->value(AUTOMATIC_CARD_SUGGESTION) << "\n"

    << ::name(ANNOUNCE_SWINES_AUTOMATICALLY) << " = "
    << this->value(ANNOUNCE_SWINES_AUTOMATICALLY) << "\n"

    << ::name(SHOW_IF_VALID) << " = "
    << this->value(SHOW_IF_VALID) << "\n"

    << ::name(EMPHASIZE_VALID_CARDS) << " = "
    << this->value(EMPHASIZE_VALID_CARDS) << "\n"

    << ::name(ANNOUNCE_IN_TABLE) << " = "
    << this->value(ANNOUNCE_IN_TABLE) << "\n"

    << ::name(SHOW_FULL_TRICK_WINDOW) << " = "
    << this->value(SHOW_FULL_TRICK_WINDOW) << "\n"

    << ::name(SHOW_FULL_TRICK_WINDOW_IF_SPECIAL_POINTS) << " = "
    << this->value(SHOW_FULL_TRICK_WINDOW_IF_SPECIAL_POINTS) << "\n"

    << ::name(CLOSE_FULL_TRICK_AUTOMATICALLY) << " = "
    << this->value(CLOSE_FULL_TRICK_AUTOMATICALLY) << "\n"

    << ::name(FULL_TRICK_CLOSE_DELAY) << " = "
    << this->value(FULL_TRICK_CLOSE_DELAY) << "\n"


    << ::name(SHOW_GAMETYPE_WINDOW) << " = "
    << this->value(SHOW_GAMETYPE_WINDOW) << "\n"

    << ::name(CLOSE_GAMETYPE_WINDOW_AUTOMATICALLY) << " = "
    << this->value(CLOSE_GAMETYPE_WINDOW_AUTOMATICALLY) << "\n"

    << ::name(GAMETYPE_WINDOW_CLOSE_DELAY) << " = "
    << this->value(GAMETYPE_WINDOW_CLOSE_DELAY) << "\n"


    << ::name(SHOW_MARRIAGE_WINDOW) << " = "
    << this->value(SHOW_MARRIAGE_WINDOW) << "\n"

    << ::name(CLOSE_MARRIAGE_WINDOW_AUTOMATICALLY) << " = "
    << this->value(CLOSE_MARRIAGE_WINDOW_AUTOMATICALLY) << "\n"

    << ::name(MARRIAGE_WINDOW_CLOSE_DELAY) << " = "
    << this->value(MARRIAGE_WINDOW_CLOSE_DELAY) << "\n"


    << ::name(SHOW_GENSCHER_WINDOW) << " = "
    << this->value(SHOW_GENSCHER_WINDOW) << "\n"

    << ::name(CLOSE_GENSCHER_WINDOW_AUTOMATICALLY) << " = "
    << this->value(CLOSE_GENSCHER_WINDOW_AUTOMATICALLY) << "\n"

    << ::name(GENSCHER_WINDOW_CLOSE_DELAY) << " = "
    << this->value(GENSCHER_WINDOW_CLOSE_DELAY) << "\n"


    << ::name(SHOW_ANNOUNCEMENT_WINDOW) << " = "
    << this->value(SHOW_ANNOUNCEMENT_WINDOW) << "\n"

    << ::name(CLOSE_ANNOUNCEMENT_WINDOW_AUTOMATICALLY) << " = "
    << this->value(CLOSE_ANNOUNCEMENT_WINDOW_AUTOMATICALLY) << "\n"

    << ::name(ANNOUNCEMENT_WINDOW_CLOSE_DELAY) << " = "
    << this->value(ANNOUNCEMENT_WINDOW_CLOSE_DELAY) << "\n"


    << ::name(SHOW_SWINES_WINDOW) << " = "
    << this->value(SHOW_SWINES_WINDOW) << "\n"

    << ::name(CLOSE_SWINES_WINDOW_AUTOMATICALLY) << " = "
    << this->value(CLOSE_SWINES_WINDOW_AUTOMATICALLY) << "\n"

    << ::name(SWINES_WINDOW_CLOSE_DELAY) << " = "
    << this->value(SWINES_WINDOW_CLOSE_DELAY) << "\n"


    << ::name(NETWORK_CONNECTIONS_URGENCY_HINT) << " = "
    << this->value(NETWORK_CONNECTIONS_URGENCY_HINT) << "\n"

    << ::name(NETWORK_CHAT_URGENCY_HINT) << " = "
    << this->value(NETWORK_CHAT_URGENCY_HINT) << "\n"


    << ::name(SHOW_TRICKPILES_POINTS) << " = "
    << this->value(SHOW_TRICKPILES_POINTS) << "\n"

    << ::name(SHOW_KNOWN_TEAMS_IN_GAME) << " = "
    << this->value(SHOW_KNOWN_TEAMS_IN_GAME) << "\n"

    << ::name(SHOW_SOLOPLAYER_IN_GAME) << " = "
    << this->value(SHOW_SOLOPLAYER_IN_GAME) << "\n"

    << ::name(SHOW_ALL_HANDS) << " = "
    << this->value(SHOW_ALL_HANDS) << "\n"

    << ::name(SHOW_AI_INFORMATION_HANDS) << " = "
    << this->value(SHOW_AI_INFORMATION_HANDS) << "\n"

    << ::name(SHOW_AI_INFORMATION_TEAMS) << " = "
    << this->value(SHOW_AI_INFORMATION_TEAMS) << "\n"

    << "\n";

  // so the sorting is saved, even if the mixed modus is aktivated

  if (!this->value(CARDS_ORDER).sorted()) {
    CardsOrder cards_order_tmp(this->value(CARDS_ORDER));

    cards_order_tmp.sorted_set(true);

    ostr << ::name(CARDS_ORDER) << " = "
      << cards_order_tmp << "\n";
  } // if (this->value(CARDS_ORDER).mixed())

  ostr << ::name(CARDS_ORDER) << " = "
    << this->value(CARDS_ORDER) << "\n";

  ostr << "# appearance\n"

    << "#   table\n"

    << ::name(OWN_HAND_ON_TABLE_BOTTOM) << " = "
    << this->value(OWN_HAND_ON_TABLE_BOTTOM) << "\n"

    << ::name(TABLE_ROTATION) << " = "
    << this->value(TABLE_ROTATION) << "\n"

    << ::name(ROTATE_TRICK_CARDS) << " = "
    << this->value(ROTATE_TRICK_CARDS) << "\n"

    << "#   cards\n"

    << ::name(ORIGINAL_CARDS_SIZE) << " = "
    << this->value(ORIGINAL_CARDS_SIZE) << "\n"

    << ::name(CARDS_HEIGHT) << " = "
    << this->value(CARDS_HEIGHT) << "\n"

    << "#   theme\n"

    << ::name(CARDSET) << " = "
    << this->value(CARDSET) << "\n"

    << ::name(CARDS_BACK) << " = "
    << this->value(CARDS_BACK) << "\n"

    << ::name(ICONSET) << " = "
    << this->value(ICONSET) << "\n"

    << ::name(BACKGROUND) << " = "
    << this->value(BACKGROUND) << "\n"

    << "#   fonts and colors\n"

    << ::name(NAME_FONT) << " = "
    << this->value(NAME_FONT) << "\n"

    << ::name(NAME_FONT_COLOR) << " = "
    << this->value(NAME_FONT_COLOR) << "\n"

    << ::name(NAME_ACTIVE_FONT_COLOR) << " = "
    << this->value(NAME_ACTIVE_FONT_COLOR) << "\n"

    << ::name(NAME_RESERVATION_FONT_COLOR) << " = "
    << this->value(NAME_RESERVATION_FONT_COLOR) << "\n"

    << ::name(TRICKPILE_POINTS_FONT) << " = "
    << this->value(TRICKPILE_POINTS_FONT) << "\n"

    << ::name(TRICKPILE_POINTS_FONT_COLOR) << " = "
    << this->value(TRICKPILE_POINTS_FONT_COLOR) << "\n"

    << ::name(POVERTY_SHIFT_ARROW_COLOR) << " = "
    << this->value(POVERTY_SHIFT_ARROW_COLOR) << "\n"

    << "#   splash screen\n"

    << ::name(SHOW_SPLASH_SCREEN) << " = "
    << this->value(SHOW_SPLASH_SCREEN) << "\n"

    << ::name(SPLASH_SCREEN_TRANSPARENT) << " = "
    << this->value(SPLASH_SCREEN_TRANSPARENT) << "\n";

  ostr << "\n";

  ostr.flags(flags);
  return ostr;
} // ostream& Setting::write(ostream& ostr) const

/**
 ** -> result
 **
 ** @param	type	the type (bool)
 **
 ** @return	name of the type
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.0
 **/
string
name(Setting::TypeBool const& type)
{
  switch(type) {
  case Setting::AUTOMATIC_SAVINGS:
    return "automatic savings";
  case Setting::SAVE_PARTY_CHANGES:
    return "save party changes";
  case Setting::ADDITIONAL_PARTY_SETTINGS:
    return "additional party settings";
  case Setting::SHOW_SPLASH_SCREEN:
    return "show splash screen";
  case Setting::SPLASH_SCREEN_TRANSPARENT:
    return "splash screen transparent";
  case Setting::SHOW_BUG_REPORT_BUTTON_IN_GAME_FINISHED_WINDOW:
    return "show bug report button in game finished window";
  case Setting::SAVE_BUG_REPORTS_ON_DESKTOP:
    return "save bug reports on desktop";
  case Setting::SOUND:
    return "sound";
#ifdef USE_NETWORK_DOKOLOUNGE
  case Setting::DOKOLOUNGE_AUTO_LOGIN:
    return "DokoLounge auto login";
#endif
  case Setting::AUTOMATIC_CARD_SUGGESTION:
    return "automatic card suggestion";
  case Setting::ANNOUNCE_SWINES_AUTOMATICALLY:
    return "announce swines automatically";
  case Setting::SHOW_IF_VALID:
    return "show if valid";
  case Setting::EMPHASIZE_VALID_CARDS:
    return "emphasize valid cards";
  case Setting::ANNOUNCE_IN_TABLE:
    return "announce in table";
  case Setting::SHOW_ALL_HANDS:
    return "show all hands";
  case Setting::SHOW_AI_INFORMATION_HANDS:
    return "show ai information hands";
  case Setting::SHOW_AI_INFORMATION_TEAMS:
    return "show ai information teams";
  case Setting::SHOW_TRICKPILES_POINTS:
    return "show trickpiles points";
  case Setting::SHOW_KNOWN_TEAMS_IN_GAME:
    return "show known teams in game";
  case Setting::SHOW_SOLOPLAYER_IN_GAME:
    return "show soloplayer in game";
  case Setting::SHOW_FULL_TRICK_WINDOW:
    return "show full trick window";
  case Setting::SHOW_FULL_TRICK_WINDOW_IF_SPECIAL_POINTS:
    return "show full trick window if special points";
  case Setting::CLOSE_FULL_TRICK_AUTOMATICALLY:
    return "close full trick automatically";
  case Setting::SHOW_GAMETYPE_WINDOW:
    return "show gametype window";
  case Setting::CLOSE_GAMETYPE_WINDOW_AUTOMATICALLY:
    return "close gametype window automatically";
  case Setting::SHOW_MARRIAGE_WINDOW:
    return "show marriage window";
  case Setting::CLOSE_MARRIAGE_WINDOW_AUTOMATICALLY:
    return "close marriage window automatically";
  case Setting::SHOW_GENSCHER_WINDOW:
    return "show genscher window";
  case Setting::CLOSE_GENSCHER_WINDOW_AUTOMATICALLY:
    return "close genscher window automatically";
  case Setting::SHOW_ANNOUNCEMENT_WINDOW:
    return "show announcement window";
  case Setting::CLOSE_ANNOUNCEMENT_WINDOW_AUTOMATICALLY:
    return "close announcement window automatically";
  case Setting::SHOW_SWINES_WINDOW:
    return "show swines window";
  case Setting::CLOSE_SWINES_WINDOW_AUTOMATICALLY:
    return "close swines window automatically";
  case Setting::NETWORK_CONNECTIONS_URGENCY_HINT:
    return "network connections urgency hint";
  case Setting::NETWORK_CHAT_URGENCY_HINT:
    return "network chat urgency hint";
  case Setting::ROTATE_TRICK_CARDS:
    return "rotate trick cards";
  case Setting::OWN_HAND_ON_TABLE_BOTTOM:
    return "own hand on table bottom";
  case Setting::ORIGINAL_CARDS_SIZE:
    return "original cards size";
  } // switch(type)

  return "";
} // string name(Setting::TypeBool const& type)

/**
 ** -> result
 **
 ** @param	type	the type (unsigned)
 **
 ** @return	name of the type
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.0
 **/
string
name(Setting::TypeUnsigned const& type)
{
  switch(type) {
#ifdef USE_THREADS
  case Setting::THREADS_MAX:
    return "max threads";
#endif
  case Setting::CARD_PLAY_DELAY:
    return "card play delay";
  case Setting::FULL_TRICK_CLOSE_DELAY:
    return "full trick close delay";
  case Setting::GAMETYPE_WINDOW_CLOSE_DELAY:
    return "gametype window close delay";
  case Setting::MARRIAGE_WINDOW_CLOSE_DELAY:
    return "marriage window close delay";
  case Setting::GENSCHER_WINDOW_CLOSE_DELAY:
    return "genscher window close delay";
  case Setting::ANNOUNCEMENT_WINDOW_CLOSE_DELAY:
    return "announcement window close delay";
  case Setting::SWINES_WINDOW_CLOSE_DELAY:
    return "swines window close delay";
  case Setting::TABLE_ROTATION:
    return "table rotation";
  case Setting::CARDS_HEIGHT:
    return "cards height";
  } // switch(type)

  return "";
} // string name(Setting::TypeUnsigned const& type)

/**
 ** -> result
 **
 ** @param	type	the type (string)
 **
 ** @return	name of the type
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.0
 **/
string
name(Setting::TypeString const& type)
{
  switch(type) {
  case Setting::NAME:
    return "name";
  case Setting::LANGUAGE:
    return "language";
#ifdef USE_NETWORK_DOKOLOUNGE
  case Setting::DOKOLOUNGE_NAME:
    return "DokoLounge name";
  case Setting::DOKOLOUNGE_PASSWORD:
    return "DokoLounge password";
#endif
  case Setting::CARDSET:
    return "cardset";
  case Setting::CARDS_BACK:
    return "cards back";
  case Setting::ICONSET:
    return "iconset";
  case Setting::BACKGROUND:
    return "background";
  case Setting::NAME_FONT:
    return "name font";
  case Setting::NAME_FONT_COLOR:
    return "name font color";
  case Setting::NAME_ACTIVE_FONT_COLOR:
    return "name active font color";
  case Setting::NAME_RESERVATION_FONT_COLOR:
    return "name reservation font color";
  case Setting::TRICKPILE_POINTS_FONT:
    return "trickpile points font";
  case Setting::TRICKPILE_POINTS_FONT_COLOR:
    return "trickpile points font color";
  case Setting::POVERTY_SHIFT_ARROW_COLOR:
    return "poverty shift arrow color";
#ifdef USE_SOUND_COMMAND
  case Setting::PLAY_SOUND_COMMAND:
    return "play sound command";
#endif
  case Setting::BROWSER_COMMAND:
    return "browser command";
  } // switch(type)

  return "";
} // string name(Setting::TypeString const& type)

/**
 ** -> result
 **
 ** @param	type	the type (string const)
 **
 ** @return	name of the type
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.0
 **/
string
name(Setting::TypeStringConst const& type)
{
  switch(type) {
  case Setting::CARDSET_LICENSE:
    return "cardset license";
  case Setting::ICONSET_LICENSE:
    return "iconset license";
  case Setting::PUBLIC_DATA_DIRECTORY:
    return "public data directory";
  case Setting::PRIVATE_DATA_DIRECTORY:
    return "private data directory";
  case Setting::SETTINGS_FILE:
    return "settings file";
  case Setting::LOGO_FILE:
    return "logo file";
  case Setting::ICON_FILE:
    return "icon file";
  case Setting::LANGUAGES_DIRECTORY:
    return "languages directory";
  case Setting::LANGUAGE_FILE:
    return "language file";
  case Setting::GRAPHIC_EXTENSION:
    return "graphic extension";
  case Setting::SOUND_EXTENSION:
    return "sound extension";
  case Setting::CARDSETS_DIRECTORY:
    return "cardsets directory";
  case Setting::CARDS_DIRECTORY:
    return "cards directory";
  case Setting::CARDS_BACK_DIRECTORY:
    return "cards back directory";
  case Setting::ICONS_FROM_CARDSET_DIRECTORY:
    return "icons from cardsets directory";
  case Setting::ICONSETS_DIRECTORY:
    return "iconsets directory";
  case Setting::ICONS_DIRECTORY:
    return "icons directory";
  case Setting::BACKGROUNDS_DIRECTORY:
    return "backgrounds directory";
#ifdef USE_NETWORK_DOKOLOUNGE
  case Setting::DOKOLOUNGE_DIRECTORY:
    return "dokolounge directory";
#endif
  case Setting::SOUNDS_DIRECTORY:
    return "sounds directory";
  case Setting::AI_DIRECTORY:
    return "ai directory";
  case Setting::PARTIES_DIRECTORY:
    return "parties directory";
  case Setting::RULES_DIRECTORY:
    return "rules directory";
  case Setting::SETTINGS_DIRECTORY:
    return "settings directory";
  case Setting::MANUAL_DIRECTORY:
    return "manual directory";
  } // switch(type)

  return "";
} // string name(Setting::TypeStringConst const& type)

/**
 ** -> result
 **
 ** @param	type	the type (cards order)
 **
 ** @return	name of the type
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.0
 **/
string
name(Setting::TypeCardsOrder const& type)
{
  switch(type) {
  case Setting::CARDS_ORDER:
    return "cards order";
  } // switch(type)

  return "";
} // string name(Setting::TypeCardsOrder const& type)

/**
 ** write the setting in the output stream
 **
 ** @param	ostr	output stream
 ** @param	setting	setting to write
 **
 ** @return	output stream
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.0
 **/
ostream&
operator<<(ostream& ostr, Setting const& setting)
{
  setting.write(ostr);

  return ostr;
} // ostream& operator<<(ostream& ostr, Setting const& setting)

/**
 ** -> result
 **
 ** @param	setting1	first setting that is compared
 ** @param	setting2	second setting that is compared
 **
 ** @return	whether the two settings are equal (according to value)
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.6
 **/
bool
operator==(Setting const& setting1, Setting const& setting2)
{
  for (int i = Setting::BOOL_FIRST; i <= Setting::BOOL_LAST; ++i)
    if (setting1.value(static_cast<Setting::TypeBool>(i))
        != setting2.value(static_cast<Setting::TypeBool>(i)))
      return false;

  for (int i = Setting::UNSIGNED_FIRST; i <= Setting::UNSIGNED_LAST; ++i)
    if (setting1.value(static_cast<Setting::TypeUnsigned>(i))
        != setting2.value(static_cast<Setting::TypeUnsigned>(i)))
      return false;

  for (int i = Setting::STRING_FIRST; i <= Setting::STRING_LAST; ++i)
    if (setting1.value(static_cast<Setting::TypeString>(i))
        != setting2.value(static_cast<Setting::TypeString>(i)))
      return false;

  if (setting1.value(Setting::CARDS_ORDER)
      != setting2.value(Setting::CARDS_ORDER))
    return false;

  return true;
} // bool operator==(Setting const& setting1, Setting const& setting2)

/**
 ** -> result
 **
 ** @param	setting1	first setting that is compared
 ** @param	setting2	second setting that is compared
 **
 ** @return	whether the two settings are different (according to value)
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.6
 **/
bool
operator!=(Setting const& setting1, Setting const& setting2)
{
  return !(setting1 == setting2);
} // bool operator!=(Setting const& setting1, Setting const& setting2)
