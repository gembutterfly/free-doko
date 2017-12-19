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

#ifndef CLASS_TRANSLATOR_HEADER
#define CLASS_TRANSLATOR_HEADER

#include "constants.h"

/**
 **
 ** The translator
 ** * supports recursive translations
 ** * can be used independent, just remove all FreeDoko-specific conversions
 **   (should only be in this file)
 **
 ** @author	Diether Knof
 **
 **/
class Translator {
  public:
    class Translation;
    friend class Translation;

  public:
    static vector<string> directories;
    static vector<string> names;

    static void dir_scan();
    static void dir_scan(string const& dirname);

  public:

    Translator();

    ~Translator();

    string const& token() const;

    bool contains(string const& text) const;

    void load();
    void load(string filename);
    void update();

  protected:
    string translate(Translation const& translation) const;
    string expand(Translation const& translation) const;

  private:
    PRIV_VAR_R(string, name);
    mutable map<string, string> translation_;

  private: // not to be used
    Translator(Translator const&);
    Translator& operator=(Translator const&);
}; // class Translator

Translator::Translation operator+(Translator::Translation const& translation1,
				  Translator::Translation const& translation2);
Translator::Translation operator+(Translator::Translation const& translation,
				  string const& text);
Translator::Translation operator+(string const& text,
				  Translator::Translation const& translation);

bool operator==(Translator::Translation const& translation1,
		Translator::Translation const& translation2);
bool operator==(Translator::Translation const& translation,
		string const& text);
bool operator==(string const& text,
		Translator::Translation const& translation);

ostream& operator<<(ostream& ostr, Translator::Translation const& translation);


extern Translator translator;

#endif // #ifndef CLASS_TRANSLATOR_HEADER
