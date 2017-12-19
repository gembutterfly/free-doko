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

#ifndef CLASS_TRANSLATION_HEADER
#define CLASS_TRANSLATION_HEADER

#include "translator.h"

/**
 **
 ** one translation (that is the original text with the place marker and the 
 **		     objects to insert)
 **
 ** @author	Diether Knof
 **
 **/
class Translator::Translation {
  public:
    friend class Translator;

    enum Type {
      CHAR,
      INT,
      UNSIGNED,
      DOUBLE,
      STRING,
      TRANSLATION
    }; // enum Type
    union Word {
      char c;
      int i;
      unsigned u;
      double d;
      string* s;
      Translation* t;
    }; // union Word

  public:
    Translation(Translation const& translation);
    Translation(Translator const& translator);
    Translation(Translator const& translator, string const& text);

    // one argument
    Translation(Translator const& translator, string const& text,
		int const i);
    Translation(Translator const& translator, string const& text,
		unsigned const u);
    Translation(Translator const& translator, string const& text,
		string const s);
    Translation(Translator const& translator, string const& text,
		Translation const& t);

    // two arguments
    Translation(Translator const& translator, string const& text,
		string const& s0,
		int const i1);
    Translation(Translator const& translator, string const& text,
		string const& s0,
		unsigned const u1);
    Translation(Translator const& translator, string const& text,
		string const& s0,
		string const& s1);
    Translation(Translator const& translator, string const& text,
		string const& s0,
		Translation const& t1);
    Translation(Translator const& translator, string const& text,
		Translation const& t0,
		int const i1);
    Translation(Translator const& translator, string const& text,
		Translation const& t0,
		unsigned const u1);
    Translation(Translator const& translator, string const& text,
		Translation const& t0,
		string const& s1);
    Translation(Translator const& translator, string const& text,
		Translation const& t0,
		Translation const& t1);
    Translation(Translator const& translator, string const& text,
		int const i0,
		string const& s1);
    Translation(Translator const& translator, string const& text,
		unsigned const u0,
		unsigned const u1);

    // three arguments
    Translation(Translator const& translator, string const& text,
		string const& s0,
		string const& s1,
		string const& s2);
    Translation(Translator const& translator, string const& text,
		string const& s0,
		Translation const& t1,
		Translation const& t2);
    
    // four arguments
    Translation(Translator const& translator, string const& text,
		Translation const& t0,
		unsigned const u1,
		Translation const& t2,
		unsigned const u3);


    ~Translation();

    //operator string() const;

    Translation& operator=(Translation const&);
    Translation& operator+=(Translation const& translation);
    Translation& operator+=(string const& text);

    string translation() const;
    string expanded_text() const;
    string const& original_text() const;

    ostream& write(ostream& ostr) const;

    bool empty() const;

  public:
    Translator const* const translator;
  protected:
    string text;
    vector<Type> type;
    vector<Word> word;
  private: // not to be used
    Translation();
}; // class Translator::Translation

// compare of two translations (t.i. the text)
bool operator==(Translator::Translation const& translation1,
                Translator::Translation const& translation2);
bool operator==(Translator::Translation const& translation,
                string const& text);
bool operator==(string const& text,
                Translator::Translation const& translation);
bool operator<(Translator::Translation const& translation1,
               Translator::Translation const& translation2);

// returns the name of the translation
string name(Translator::Translation::Type const& type);
WRITE_NAME(Translator::Translation::Type)

#endif // #ifndef CLASS_TRANSLATION_HEADER
