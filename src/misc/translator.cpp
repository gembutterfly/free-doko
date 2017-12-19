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

#include "translator.h"
#include "translation.h"

#include "setting.h"
#include "../ui/ui.h"

#include "../utils/file.h"
#include "../utils/string.h"

// local defines
#define IGNORE_UNKNOWN_WORDS

#ifdef DKNOF
#undef IGNORE_UNKNOWN_WORDS
#define SHOW_DIFF
#endif

/**
 ** search the directory for translations
 **
 ** @param	-
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.4
 **/
void
Translator::dir_scan()
{
  // clear the old data
  Translator::directories.clear();
  Translator::names.clear();

  for (vector<string>::const_iterator
       d = ::setting.data_directories().begin();
       d != ::setting.data_directories().end();
       ++d) {
    Translator::dir_scan(*d + "/" + ::setting(Setting::LANGUAGES_DIRECTORY));
  }

  return ;
} // void Translator::dir_scan()

/**
 ** search the direcotyr for translations
 **
 ** @param	dirname		directory to search in
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.4
 **/
void
Translator::dir_scan(string const& dirname)
{
  DIR *translator_dir = opendir(dirname.c_str());
  if (translator_dir == NULL)
    return ;

  struct dirent *entry;

  Translator::directories.clear();
  Translator::names.clear();

  // search all entries in the directory
  while ((entry = readdir(translator_dir)) != NULL) {
    if (   (strcmp(entry->d_name, ".") == 0)
        || (strcmp(entry->d_name, "..") == 0) )
      continue;
    if (DK::Utils::File::isdirectory(dirname + "/" + entry->d_name)) {
      if (DK::Utils::File::isfile(dirname + "/" + entry->d_name + "/"
                                  + ::setting.value(Setting::LANGUAGE_FILE))) { 
        // add the translator to the list
        Translator translator;
        translator.load(dirname + "/" + entry->d_name + "/"
                        + ::setting.value(Setting::LANGUAGE_FILE));

        // only add the translator, when it is not in the list
        if (   (std::find(Translator::directories.begin(),
                          Translator::directories.end(),
                          entry->d_name) == Translator::directories.end())
            || (std::find(Translator::names.begin(), Translator::names.end(),
                          translator.name()) == Translator::names.end())) {

          Translator::directories.push_back(entry->d_name);
          Translator::names.push_back(translator.name());

        } // if (translator not in the list)
      } // if (is translatordirectory)
    } // if (entry->d_type == DT_DIR)
  } // while ((entry = readdir(translator_dir)) != NULL)

  closedir(translator_dir);

  return ;
} // void Translator::dir_scan(string const& dirname)

/**
 ** constructor
 **
 ** @param	-
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.4
 **/
Translator::Translator() :
  name_(),
  translation_()
{ }

/**
 ** destructor
 **
 ** @param	-
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.4
 **/
Translator::~Translator()
{ }

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	the token of the translator (also the directory)
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.4
 **/
string const&
Translator::token() const
{
  for (unsigned i = 0; i < Translator::names.size(); i++) {
    if (Translator::names[i] == this->name())
      return Translator::directories[i];
  }

  // token not found
  DEBUG_ASSERTION(false,
                  "Translator::token():\n"
                  "  token for " << this->name() << " not found.");

  return Translator::directories[0];
} // string const& Translator::token()

/**
 ** -> result
 **
 ** @param	text	text to search
 **
 ** @return	whether there exists a translation for 'text'
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.8
 **/
bool
Translator::contains(string const& text) const
{
  return (this->translation_.find(text) != this->translation_.end());
} // bool Translator::contains(string text) const

/**
 ** loads the translator given by the settings
 **
 ** @param	-
 **
 ** @return	Translator
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.4
 **/
void
Translator::load()
{
#ifdef SHOW_DIFF
  map<string, string> translation_bak(this->translation_);
  string const name_bak = this->name();
#endif

  // clear the translation
  this->translation_.clear();

  // the following translations are used for composing
  this->translation_[""] = "";
  this->translation_["%uunsigned%"] = "%uunsigned%";
  this->translation_["%iint%"] = "%iint%";
  this->translation_["%stext%"] = "%stext%";
  this->translation_["%ttranslation%: "] = "%ttranslation%: ";
  this->translation_["%ttranslation1%%ttranslation2%"] = "%ttranslation1%%ttranslation2%";
  this->translation_["%ttranslation%%stext%"] = "%ttranslation%%stext%";
  this->translation_["%stext%%ttranslation%"] = "%stext%%ttranslation%";

  for (vector<string>::const_iterator
       d = ::setting.data_directories().begin();
       d != ::setting.data_directories().end();
       ++d) {
    this->load(*d + "/" + ::setting(Setting::LANGUAGE_FILE));
  }

#ifdef SHOW_DIFF
  if (   !translation_bak.empty()
      && (this->name() != name_bak)
      && (::game_status > GAMESTATUS::PROGRAMSTART)  ) {
    cout << '\n';
    cout << "translation diff: " << name_bak << " - " << this->name() << '\n';
    cout << "---------------------------\n";
    for (map<string, string>::const_iterator t = translation_bak.begin();
         t != translation_bak.end();
         ++t)
      if (this->translation_.find(t->first) == this->translation_.end())
        cout << "> " << t->first << '\n';

    for (map<string, string>::const_iterator t = this->translation_.begin();
         t != this->translation_.end();
         ++t)
      if (translation_bak.find(t->first) == translation_bak.end())
        cout << "< " << t->first << '\n';
  } // if (!translation_bak.empty())
#endif

  this->update();

  return ;
} // Translator& Translator::load()

/**
 ** loads the translator given by the settings
 **
 ** @param	filename	file to load
 **
 ** @return	Translator
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.4
 **/
void
Translator::load(string filename)
{
  ifstream* istr = new ifstream(filename.c_str());

  if (istr->fail()) {
    // ToDo: If the file is not found, this can result in a long search
    if (std::find(Translator::names.begin(),
                  Translator::names.end(),
                  ::setting.value(Setting::LANGUAGE))
        != Translator::names.end()) {
      filename = (DK::Utils::File::dirname(filename, 2)
                  + "/"
                  + Translator::directories[std::find(Translator::names.begin(),
                                                      Translator::names.end(),
                                                      setting.value(Setting::LANGUAGE))
                  - Translator::names.begin()]
                  + "/text");

      delete istr;
      istr = new ifstream(filename.c_str());  
    }

    if (istr->fail()) {
      delete istr;
      return ;
    }
  } // if (istr->fail());

  if (DK::Utils::File::basename(DK::Utils::File::dirname(filename)).empty())
    // the filename is not in a subdirectory
    return ;
  this->name_ = DK::Utils::File::basename(DK::Utils::File::dirname(filename));

  while (istr->good()) {
    Config config;
    *istr >> config;

    // finished with the config file
    if ((config.name == "") && (config.value == ""))
      break;

    if (config.separator) {
      // a translator entry
      this->translation_[config.name] = config.value;
    } else { // if (config.separator)
      // a setting
      // if the value is in parentencies, remove both
      if ((config.value[0] == '\"')
          && (config.value[config.value.length() - 1] == '\"'))
        config.value = string(config.value, 1, config.value.length() - 2);
      if (config.name == "!name") {
        // set the name of the lanaguage
        this->name_ = config.value;
      } else if (config.name == "!include") {
        // include the given translator
        Translator translator_new;
        translator_new.load(DK::Utils::File::dirname(filename) + "/"
                            + ::setting(Setting::LANGUAGES_DIRECTORY) + "/"
                            + config.value + "/"
                            + ::setting.value(Setting::LANGUAGE_FILE));
        this->translation_.insert(translator_new.translation_.begin(),
                                  translator_new.translation_.end());
      } else if(config.name == "!end") {
        // ignore the rest of the file
        break;
      } else if (config.name == "!input") {
        // include the given file
        Translator translator_new;
        translator_new.load((DK::Utils::File::dirname(filename) + "/"
                             + config.value));
        this->translation_.insert(translator_new.translation_.begin(),
                                  translator_new.translation_.end());
      } else if(config.name == "") {
        cerr << "Translator file \'" << filename << "\': "
          << "Ignoring line \'" << config.value << "\'.\n";
      } else {
        cerr << "Translator file \'" << filename << "\': "
          << "Line " << config.name << " unknown.\n"
          << "Ignoring it.\n";
      } // if (config.name == .)
    } // config.separator

  } // while (istr->good())

  delete istr;

  return ;
} // void Translator::load(string filename)

/**
 ** update the translation in the ui
 **
 ** @param     -
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.3
 **/
void
Translator::update()
{
  if (   (this == &::translator)
      && (ui != NULL))
    ::ui->language_changed();

  return ;
} // void Translator::update()

/**
 ** -> result
 **
 ** @param	translation	what shall be translated
 **
 ** @return	the translation of 'translation'
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.3
 **/
string
Translator::translate(Translation const& translation) const
{
  string text = translation.text;
  if (text.empty())
    return "";

  bool const first_upper = (text[0] == '~');
  if (text[0] == '~')
    text.erase(text.begin());

  if (text == "_GPL_") {
#include "../text/gpl.string"
    return GPL_string;
  }

  // translated text
  string s;
  map<string, string>::const_iterator word = this->translation_.find(text);
#ifdef WORKAROUND
  if (word == this->translation_.end()) {
    if (!this->translation_.empty()) {
      if (::isupper(text[0])) {
#ifndef IGNORE_UNKNOWN_WORDS
#if 0
        cerr << "Translation '" << this->name() << "': \"" << text << "\" not found.\n"
          << "  Testing for lower case version" << endl;
#endif
#endif
        text[0] = ::tolower(text[0]);
        word = this->translation_.find(text);
        text[0] = ::toupper(text[0]);
      }
    }
  }
#endif
  if (word == this->translation_.end()) {
    // translation not found (= translation not translated)
#ifndef IGNORE_UNKNOWN_WORDS
    if (!this->translation_.empty()) {
      cerr << "Translation '" << this->name() << "': \"" << text << "\" not found"
        << endl;
      // SEGFAULT;
    }
#endif
    if (!this->translation_.empty())
      this->translation_[text] = text;
    s = text;
  } else { // if !(word == this->translation_.end())
    s = word->second;
  } // if !(word == this->translation_.end())

  // start of a '%'-substring
  string::iterator pos = text.begin();
  // end of the '%'-substring
  string::iterator pos_end = text.end();
  string replacement; // the replacement for the '%'-substring
  for (unsigned i = 0; i < translation.type.size(); ++i) {
    // replace the %x...% entries in the text with the parameters

    if ((pos == text.begin()) && (pos_end == text.end()))
      pos = find(text.begin() , text.end(), '%');
    else
      pos = find(pos_end + 1, text.end(), '%');
    if (pos == text.end())
      // no more '%' in s
      break;

    pos_end = find(pos + 1, text.end(), '%');
    DEBUG_ASSERTION((pos_end != text.end()),
                    "Translation::translate(translation):\n"
                    "  uncorrect number of \%.\n"
                    "  Should be " << i << " but the text is: "
                    << text);
    if (pos_end == pos + 1) {
      // "%%" -- ignore
      i -= 1;
      continue;
    }

    // look, what of type the argument is
    switch(*(pos + 1)) {
    case 'c': // character
      DEBUG_ASSERTION((translation.type[i]
                       == Translator::Translation::CHAR),
                      "Translator::translate(translation):\n"
                      "  wrong type at position " << i << ":\n"
                      "  The text says 'char' but the translation says '"
                      << translation.type[i] << "'.\n"
                      "  Text: " << text);
      break;
    case 'd':
    case 'i': // integer
      DEBUG_ASSERTION((translation.type[i]
                       == Translator::Translation::INT),
                      "Translator::translate(translation):\n"
                      "  wrong type at position " << i << ":\n"
                      "  The text says 'int' but the translation says '"
                      << translation.type[i] << "'.\n"
                      "  Text: " << text);
      break;
    case 'u': // unsigned
      DEBUG_ASSERTION((translation.type[i]
                       == Translator::Translation::UNSIGNED),
                      "Translator::translate(translation):\n"
                      "  wrong type at position " << i << ":\n"
                      "  The text says 'unsigned' but the translation says '"
                      << translation.type[i] << "'.\n"
                      "  Text: " << text);
      break;
    case 'f':
    case 'g': // double
      DEBUG_ASSERTION((translation.type[i]
                       == Translator::Translation::DOUBLE),
                      "Translator::translate(translation):\n"
                      "  wrong type at position " << i << ":\n"
                      "  The text says 'double' but the translation says '"
                      << translation.type[i] << "'.\n"
                      "  Text: " << text);
      break;
    case 's': // string
#ifndef POSTPONED
      if (translation.type[i] == Translator::Translation::TRANSLATION) {
        cerr << "Translation type is 'translation' but the code says 'string'.\n"
          << "Text: " << text << endl;
        break;
      }
#endif
      DEBUG_ASSERTION((translation.type[i]
                       == Translator::Translation::STRING),
                      "Translator::translate(translation):\n"
                      "  wrong type at position " << i << ":\n"
                      "  The text says 'string' but the translation says '"
                      << translation.type[i] << "'.\n"
                      "  Text: " << text);
      break;
    case 't': // translation
      DEBUG_ASSERTION((translation.type[i]
                       == Translator::Translation::TRANSLATION),
                      "Translator::translate(translation):\n"
                      "  wrong type at position " << i << ":\n"
                      "  The text says 'translation' but the translation says '"
                      << translation.type[i] << "'.\n"
                      "  Text: " << text);
      break;
    default:
      DEBUG_ASSERTION(false,
                      "Translator::translate(translation):\n"
                      "  unknown replacement type '" << *(pos + 1) << "'.");
      break;
    } // switch (*(pos + 1))

    switch (translation.type[i]) {
    case Translator::Translation::CHAR:
      replacement = string(&(translation.word[i].c), 1);
      break;
    case Translator::Translation::INT:
      replacement = DK::Utils::String::to_string(translation.word[i].i);
      break;
    case Translator::Translation::UNSIGNED:
      replacement = DK::Utils::String::to_string(translation.word[i].u);
      break;
    case Translator::Translation::DOUBLE:
      replacement = DK::Utils::String::to_string(translation.word[i].d);
      break;
    case Translator::Translation::STRING:
      replacement = *(translation.word[i].s);
      break;
    case Translator::Translation::TRANSLATION:
      replacement = translation.word[i].t->translation();
      break;
    } // switch (translation.type[i])

    DK::Utils::String::replace_all(s, string(pos, pos_end + 1), replacement);
  } // for (i < translation.type.size())

  // replace "%%" with "%"
  DK::Utils::String::replace_all(s, "%%", "%");

  if (first_upper)
    s[0] = ::toupper(s[0]);

  return s;
} // string Translator::translate(Translation const& translation)


/**
 ** -> result
 **
 ** @param	translation	what shall be expanded
 **
 ** @return	the expanded 'translation' (that is all text replaced)
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.4
 **/
string
Translator::expand(Translation const& translation) const
{
  string text = translation.text;
  string s = text; // keeps the string

  // start of a '%'-substring
  string::iterator pos = text.begin();
  // end of the '%'-substring
  string::iterator pos_end = text.end();
  string replacement; // the replacement for the '%'-substring
  for (unsigned i = 0; i < translation.type.size(); ++i) {
    // replace the %x...% entries in the text with the parameters

    if ((pos == text.begin()) && (pos_end == text.end()))
      pos = find(text.begin(), text.end(), '%');
    else
      pos = find(pos_end + 1, text.end(), '%');
    if (pos == text.end())
      // no more '%' in s
      break;

    pos_end = find(pos + 1, text.end(), '%');
    DEBUG_ASSERTION((pos_end != text.end()),
                    "Translation::translate(translation):\n"
                    "  uncorrect number of \%.\n"
                    "  Should be " << i << " but the text is: "
                    << text);
    if (pos_end == pos + 1) {
      // "%%" -- ignore
      i -= 1;
      continue;
    }

    // look, what of type the argument is
    switch(*(pos + 1)) {
    case 'c': // character
      DEBUG_ASSERTION((translation.type[i]
                       == Translator::Translation::CHAR),
                      "Translator::translate(translation):\n"
                      "  wrong type at position " << i << ":\n"
                      "  The text says 'char' but the translation says '"
                      << translation.type[i] << "'.\n"
                      "  Text: " << text);
      break;
    case 'd':
    case 'i': // integer
      DEBUG_ASSERTION((translation.type[i]
                       == Translator::Translation::INT),
                      "Translator::translate(translation):\n"
                      "  wrong type at position " << i << ":\n"
                      "  The text says 'int' but the translation says '"
                      << translation.type[i] << "'.\n"
                      "  Text: " << text);
      break;
    case 'u': // unsigned
      DEBUG_ASSERTION((translation.type[i]
                       == Translator::Translation::UNSIGNED),
                      "Translator::translate(translation):\n"
                      "  wrong type at position " << i << ":\n"
                      "  The text says 'unsigned' but the translation says '"
                      << translation.type[i] << "'.\n"
                      "  Text: " << text);
      break;
    case 'f':
    case 'g': // double
      DEBUG_ASSERTION((translation.type[i]
                       == Translator::Translation::DOUBLE),
                      "Translator::translate(translation):\n"
                      "  wrong type at position " << i << ":\n"
                      "  The text says 'double' but the translation says '"
                      << translation.type[i] << "'.\n"
                      "  Text: " << text);
      break;
    case 's': // string
#ifndef POSTPONED
      if (translation.type[i] == Translator::Translation::TRANSLATION) {
        cerr << "Translation type is 'translation' but the code says 'string'.\n"
          << "Text: " << text << endl;
        break;
      }
#endif
      DEBUG_ASSERTION((translation.type[i]
                       == Translator::Translation::STRING),
                      "Translator::translate(translation):\n"
                      "  wrong type at position " << i << ":\n"
                      "  The text says 'string' but the translation says '"
                      << translation.type[i] << "'.\n"
                      "  Text: " << text);
      break;
    case 't': // translation
      DEBUG_ASSERTION((translation.type[i]
                       == Translator::Translation::TRANSLATION),
                      "Translator::translate(translation):\n"
                      "  wrong type at position " << i << ":\n"
                      "  The text says 'translation' but the translation says '"
                      << translation.type[i] << "'.\n"
                      "  Text: " << text);
      break;
    default:
      DEBUG_ASSERTION(false,
                      "Translator::translate(translation):\n"
                      "  unknown replacement type '" << *(pos + 1) << "'.");
      break;
    } // switch (*(pos + 1))

    switch (translation.type[i]) {
    case Translator::Translation::CHAR:
      replacement = string(&(translation.word[i].c), 1);
      break;
    case Translator::Translation::INT:
      replacement = DK::Utils::String::to_string(translation.word[i].i);
      break;
    case Translator::Translation::UNSIGNED:
      replacement = DK::Utils::String::to_string(translation.word[i].u);
      break;
    case Translator::Translation::DOUBLE:
      replacement = DK::Utils::String::to_string(translation.word[i].d);
      break;
    case Translator::Translation::STRING:
      replacement = *(translation.word[i].s);
      break;
    case Translator::Translation::TRANSLATION:
      replacement = translation.word[i].t->expanded_text();
      break;
    } // switch (translation.type[i])

    DK::Utils::String::replace_all(s, string(pos, pos_end + 1), replacement);
  } // for (i < translation.type.size())

  // replace "%%" with "%"
  DK::Utils::String::replace_all(s, "%%", "%");

  return s;
} // string Translator::expand(Translation const& translation)

#ifdef IGNORE_UNKNOWN_WORDS
#undef IGNORE_UNKNOWN_WORDS
#endif
#ifdef SHOW_DIFF
#undef SHOW_DIFF
#endif
