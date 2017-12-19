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

/**
 ** copy constructor
 **
 ** @param	translation	Translation to copy
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.4
 **/
Translator::Translation::Translation(Translation const& translation) :
  translator(translation.translator),
  text(translation.text),
  type(translation.type),
  word(translation.word)
{
  for (unsigned i = 0; i < this->type.size(); ++i) {
    switch (this->type[i]) {
    case CHAR:
    case INT:
    case UNSIGNED:
    case DOUBLE:
      break;
    case STRING:
      this->word[i].s = new string(*(translation.word[i].s));
      break;
    case TRANSLATION:
      this->word[i].t = new Translation(*(translation.word[i].t));
      break;
    }; // switch (this->type[i])
  } // for (i < this->type.size())
} // Translator::Translation::Translation(Translation const& translation)

/**
 ** constructor: nothing
 **
 ** @param	translator	the corresponding translator
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.1
 **/
Translator::Translation::Translation(Translator const& translator) :
  translator(&translator),
  text(""),
  type(),
  word()
{ }

/**
 ** constructor: only text
 **
 ** @param	translator	the corresponding translator
 ** @param	text		the text to translate
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.4
 **/
Translator::Translation::Translation(Translator const& translator,
                                     string const& text) :
  translator(&translator),
  text(text),
  type(),
  word()
{ }

/**
 ** constructor: text with one int
 **
 ** @param	translator	the corresponding translator
 ** @param	text		the text to translate
 ** @param	i		integer to be inserted
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.4
 **/
Translator::Translation::Translation(Translator const& translator,
                                     string const& text,
                                     int const i) :
  translator(&translator),
  text(text),
  type(1),
  word(1)
{
  type[0] = INT;
  word[0].i = i;
}

/**
 ** constructor: text with one unsigned
 **
 ** @param	translator	the corresponding translator
 ** @param	text		the text to translate
 ** @param	u		unsigned to be inserted
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.4
 **/
Translator::Translation::Translation(Translator const& translator,
                                     string const& text,
                                     unsigned const u) :
  translator(&translator),
  text(text),
  type(1),
  word(1)
{
  type[0] = UNSIGNED;
  word[0].u = u;
}

/**
 ** constructor: text with one string
 **
 ** @param	translator	the corresponding translator
 ** @param	text		the text to translate
 ** @param	s		string to be inserted
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.4
 **/
Translator::Translation::Translation(Translator const& translator,
                                     string const& text,
                                     string const s) :
  translator(&translator),
  text(text),
  type(1),
  word(1)
{
  type[0] = STRING;
  word[0].s = new string(s);
}

/**
 ** constructor: text with one translation
 **
 ** @param	translator	the corresponding translator
 ** @param	text		the text to translate
 ** @param	t		translation to be inserted
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.4
 **/
Translator::Translation::Translation(Translator const& translator,
                                     string const& text,
                                     Translation const& t) :
  translator(&translator),
  text(text),
  type(1),
  word(1)
{
  type[0] = TRANSLATION;
  word[0].t = new Translation(t);
}

/**
 ** constructor: text with one string and one unsigned
 **
 ** @param	translator	the corresponding translator
 ** @param	text		the text to translate
 ** @param	s0		string to be inserted
 ** @param	i1		integer to be inserted
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.13
 **/
Translator::Translation::Translation(Translator const& translator,
                                     string const& text,
                                     string const& s0,
                                     int const i1) :
  translator(&translator),
  text(text),
  type(2),
  word(2)
{
  type[0] = STRING;
  word[0].s = new string(s0);
  type[1] = INT;
  word[1].i = i1;
}

/**
 ** constructor: text with one string and one unsigned
 **
 ** @param	translator	the corresponding translator
 ** @param	text		the text to translate
 ** @param	s0		string to be inserted
 ** @param	u1		unsigned to be inserted
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.4
 **/
Translator::Translation::Translation(Translator const& translator,
                                     string const& text,
                                     string const& s0,
                                     unsigned const u1) :
  translator(&translator),
  text(text),
  type(2),
  word(2)
{
  type[0] = STRING;
  word[0].s = new string(s0);
  type[1] = UNSIGNED;
  word[1].u = u1;
}

/**
 ** constructor: text with two strings
 **
 ** @param	translator	the corresponding translator
 ** @param	text		the text to translate
 ** @param	s0		1st string to be inserted
 ** @param	s1		2nd string to be inserted
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.4
 **/
Translator::Translation::Translation(Translator const& translator,
                                     string const& text,
                                     string const& s0,
                                     string const& s1) :
  translator(&translator),
  text(text),
  type(2),
  word(2)
{
  type[0] = STRING;
  word[0].s = new string(s0);
  type[1] = STRING;
  word[1].s = new string(s1);
}

/**
 ** constructor: text with one string and one translation
 **
 ** @param	translator	the corresponding translator
 ** @param	text		the text to translate
 ** @param	s0		string to be inserted
 ** @param	t1		translation to be inserted
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.4
 **/
Translator::Translation::Translation(Translator const& translator,
                                     string const& text,
                                     string const& s0,
                                     Translation const& t1) :
  translator(&translator),
  text(text),
  type(2),
  word(2)
{
  type[0] = STRING;
  word[0].s = new string(s0);
  type[1] = TRANSLATION;
  word[1].t = new Translation(t1);
}

/**
 ** constructor: text with one translation and one string
 **
 ** @param	translator	the corresponding translator
 ** @param	text		the text to translate
 ** @param	t0		translation to be inserted
 ** @param	u1		unsigned to be inserted
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.4
 **/
Translator::Translation::Translation(Translator const& translator,
                                     string const& text,
                                     Translation const& t0,
                                     unsigned const u1) :
  translator(&translator),
  text(text),
  type(2),
  word(2)
{
  type[0] = TRANSLATION;
  word[0].t = new Translation(t0);
  type[1] = UNSIGNED;
  word[1].u = u1;
}

/**
 ** constructor: text with one translation and one string
 **
 ** @param	translator	the corresponding translator
 ** @param	text		the text to translate
 ** @param	t0		translation to be inserted
 ** @param	s1		string to be inserted
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.4
 **/
Translator::Translation::Translation(Translator const& translator,
                                     string const& text,
                                     Translation const& t0,
                                     string const& s1) :
  translator(&translator),
  text(text),
  type(2),
  word(2)
{
  type[0] = TRANSLATION;
  word[0].t = new Translation(t0);
  type[1] = STRING;
  word[1].s = new string(s1);
}

/**
 ** constructor: text with two translations
 **
 ** @param	translator	the corresponding translator
 ** @param	text		the text to translate
 ** @param	t0		first translation to be inserted
 ** @param	t1		second translation to be inserted
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.4
 **/
Translator::Translation::Translation(Translator const& translator,
                                     string const& text,
                                     Translation const& t0,
                                     Translation const& t1) :
  translator(&translator),
  text(text),
  type(2),
  word(2)
{
  type[0] = TRANSLATION;
  word[0].t = new Translation(t0);
  type[1] = TRANSLATION;
  word[1].t = new Translation(t1);
}

/**
 ** constructor: text with one string and one int
 **
 ** @param	translator	the corresponding translator
 ** @param	text		the text to translate
 ** @param	s0		string to be inserted
 ** @param	i1		integer to be inserted
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.13
 **/
Translator::Translation::Translation(Translator const& translator,
                                     string const& text,
                                     int const i0,
                                     string const& s1) :
  translator(&translator),
  text(text),
  type(2),
  word(2)
{
  type[0] = INT;
  word[0].i = i0;
  type[1] = STRING;
  word[1].s = new string(s1);
}


/**
 ** constructor: text with two unsigned
 **
 ** @param	translator	the corresponding translator
 ** @param	text		the text to translate
 ** @param	u0		unsigned to be inserted
 ** @param	u1		unsigned to be inserted
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.14
 **/
Translator::Translation::Translation(Translator const& translator,
                                     string const& text,
                                     unsigned const u0,
                                     unsigned const u1) :
  translator(&translator),
  text(text),
  type(2),
  word(2)
{
  type[0] = UNSIGNED;
  word[0].u = u0;
  type[1] = UNSIGNED;
  word[1].u = u1;
}



/**
 ** constructor: text with three strings
 **
 ** @param	translator	the corresponding translator
 ** @param	text		the text to translate
 ** @param	s0		1st string to be inserted
 ** @param	s1		2nd string to be inserted
 ** @param	s2		3rd string to be inserted
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.4
 **/
Translator::Translation::Translation(Translator const& translator,
                                     string const& text,
                                     string const& s0,
                                     string const& s1,
                                     string const& s2) :
  translator(&translator),
  text(text),
  type(3),
  word(3)
{
  type[0] = STRING;
  word[0].s = new string(s0);
  type[1] = STRING;
  word[1].s = new string(s1);
  type[2] = STRING;
  word[2].s = new string(s2);
}

/**
 ** constructor: text with one string and two translations
 **
 ** @param	translator	the corresponding translator
 ** @param	text		the text to translate
 ** @param	s0		1st string to be inserted
 ** @param	t1		2nd translation to be inserted
 ** @param	t2		3rd translation to be inserted
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.5
 **/
Translator::Translation::Translation(Translator const& translator,
                                     string const& text,
                                     string const& s0,
                                     Translation const& t1,
                                     Translation const& t2) :
  translator(&translator),
  text(text),
  type(3),
  word(3)
{
  type[0] = STRING;
  word[0].s = new string(s0);
  type[1] = TRANSLATION;
  word[1].t = new Translation(t1);
  type[2] = TRANSLATION;
  word[2].t = new Translation(t2);
}

/**
 ** constructor: text with four parameters
 **
 ** @param	translator	the corresponding translator
 ** @param	text		the text to translate
 ** @param	t0		1st translation to be inserted
 ** @param	u1		2dn unsigned to be inserted
 ** @param	t2		3rd translation to be inserted
 ** @param	u3		4th unsigned to be inserted
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.1
 **/
Translator::Translation::Translation(Translator const& translator,
                                     string const& text,
                                     Translation const& t0,
                                     unsigned const u1,
                                     Translation const& t2,
                                     unsigned const u3) :
  translator(&translator),
  text(text),
  type(4),
  word(4)
{
  type[0] = TRANSLATION;
  word[0].t = new Translation(t0);
  type[1] = UNSIGNED;
  word[1].u = u1;
  type[2] = TRANSLATION;
  word[2].t = new Translation(t2);
  type[3] = UNSIGNED;
  word[3].u = u3;
}

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
Translator::Translation::~Translation()
{
  for (unsigned i = 0; i < this->type.size(); ++i) {
    if (type[i] == STRING)
      delete word[i].s;
    if (type[i] == TRANSLATION)
      delete word[i].t;
  } // for (i < this->type.size())
} // Translator::Translation::~Translation()

/**
 ** setting operator
 **
 ** @param	translation	translation to set
 **
 ** @return	new translation
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.5
 **/
Translator::Translation&
Translator::Translation::operator=(Translator::Translation const& translation)
{
  DEBUG_ASSERTION((this != &translation),
                  "Translator::Translation::operator=(translation):\n"
                  "  same translation");

  DEBUG_ASSERTION((this->translator == translation.translator),
                  "Translator::Translation::operator=(translation):\n"
                  "  different Translator");

  for (unsigned i = 0; i < this->type.size(); ++i) {
    switch(this->type[i]) {
    case CHAR:
    case INT:
    case UNSIGNED:
    case DOUBLE:
      break;
    case STRING:
      delete this->word[i].s;
      break;
    case TRANSLATION:
      delete this->word[i].t;
      break;
    }; // switch(this->type[i])
  } // for (i < this->type.size())

  this->text = translation.text;
  this->type = translation.type;
  this->word = translation.word;

  for (unsigned i = 0; i < this->type.size(); ++i) {
    switch(this->type[i]) {
    case CHAR:
    case INT:
    case UNSIGNED:
    case DOUBLE:
      break;
    case STRING:
      this->word[i].s = new string(*(translation.word[i].s));
      break;
    case TRANSLATION:
      this->word[i].t = new Translation(*(translation.word[i].t));
      break;
    }; // switch(this->type[i])
  } // for (i < this->type.size())

  return *this;
} // Translator::Translation& Translator::Translation::operator=(Translator::Translation const& translation)

/**
 ** adds 'translation' to this translation
 **
 ** @param	translation	translation to add
 **
 ** @return	updated translation
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.5
 **/
Translator::Translation&
Translator::Translation::operator+=(Translator::Translation const& translation)
{
  DEBUG_ASSERTION((this->translator == translation.translator),
                  "Translator::Translation::operator+=(translation):\n"
                  "  different Translator");

  *this = Translator::Translation(*this->translator,
                                  "%ttranslation1%%ttranslation2%",
                                  *this, translation);

  return *this;
} // Translator::Translation& Translator::Translation::operator+=(Translator::Translation const& translation)

/**
 ** adds 'text' to this translation
 **
 ** @param	text	text to add
 **
 ** @return	updated translation
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.5
 **/
Translator::Translation&
Translator::Translation::operator+=(string const& text)
{
  *this = Translator::Translation(*this->translator,
                                  "%ttranslation%%stext%",
                                  *this, text);

  return *this;
} // Translator::Translation& Translator::Translation::operator+=(string const& text)

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	translated text
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.4
 **/
string
Translator::Translation::translation() const
{
  return this->translator->translate(*this);
} // string Translator::Translation::translation() const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	expanded text (with all replacements)
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.5
 **/
string
Translator::Translation::expanded_text() const
{
  return this->translator->expand(*this);
} // string Translator::Translation::expanded_text() const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	the original text
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.3
 **/
string const&
Translator::Translation::original_text() const
{
  return this->text;
} // string Translator::Translation::original_text() const

/**
 ** writes the translation in the stream 'ostr'
 **
 ** @param	ostr	output stream
 **
 ** @return	output stream
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.4
 **/
ostream&
Translator::Translation::write(ostream& ostr) const
{
  ostr << this->translation();

  return ostr;
} // ostream& Translator::Translation::write(ostream& ostr) const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	whether the translation is empty
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.5
 **/
bool
Translator::Translation::empty() const
{
  return this->text.empty();
} // bool Translator::Translation::empty() const

/**
 ** compose the two translations
 **
 ** @param	translation1	first translation
 ** @param	translation2	second translation
 **
 ** @return	composed translation
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.4
 **/
Translator::Translation
operator+(Translator::Translation const& translation1,
          Translator::Translation const& translation2)
{
  return Translator::Translation(*translation1.translator,
                                 "%ttranslation1%%ttranslation2%",
                                 translation1,
                                 translation2);
} // Translator::Translation operator+(Translator::Translation const& translation1, Translator::Translation const& translation2)

/**
 ** compose the translation with the text
 **
 ** @param	translation	translation
 ** @param	text		text
 **
 ** @return	composed translation
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.4
 **/
Translator::Translation
operator+(Translator::Translation const& translation,
          string const& text)
{
  return Translator::Translation(*translation.translator,
                                 "%ttranslation%%stext%",
                                 translation,
                                 text);
} // Translator::Translation operator+(Translator::Translation const& translation, string const& text)

/**
 ** compose the translation with the text
 **
 ** @param	text		text
 ** @param	translation	translation
 **
 ** @return	composed translation
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.4
 **/
Translator::Translation
operator+(string const& text,
          Translator::Translation const& translation)
{
  return Translator::Translation(*translation.translator,
                                 "%stext%%ttranslation%",
                                 text,
                                 translation);
} // Translator::Translation operator+(string const& text, Translator::Translation const& translation)

/**
 ** compare the two translations
 **
 ** @param	translation1	first translation
 ** @param	translation2	second translation
 **
 ** @return	whether the translations of the two are the same
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.8
 **/
bool
operator==(Translator::Translation const& translation1,
           Translator::Translation const& translation2)
{
  return (translation1.translation() == translation2.translation());
} // bool operator==(Translator::Translation translation1, Translator::Translation translation2)

/**
 ** compare the translation with the text
 **
 ** @param	translation	translation
 ** @param	text		text
 **
 ** @return	whether the translation of 'translation' is 'text'
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.4
 **/
bool
operator==(Translator::Translation const& translation,
           string const& text)
{
  return (translation.translation() == text);
} // bool operator==(Translator::Translation const& translation, string const& text)

/**
 ** compare the translation with the text
 **
 ** @param	text		text
 ** @param	translation	translation
 **
 ** @return	whether the translation of 'translation' is 'text'
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.4
 **/
bool
operator==(string const& text,
           Translator::Translation const& translation)
{
  return (text == translation.translation());
} // bool operator==(string const& text, Translator::Translation const& translation)

/**
 ** compare the two translations
 **
 ** @param	translation1	first translation
 ** @param	translation2	second translation
 **
 ** @return	whether the first translation is less in alphabetic order then the second
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.8
 **/
bool
operator<(Translator::Translation const& translation1,
           Translator::Translation const& translation2)
{
  return (translation1.translation() < translation2.translation());
} // bool operator<(Translator::Translation translation1, Translator::Translation translation2)

/**
 ** write the translation to ostr
 **
 ** @param	ostr		stream to write to
 ** @param	translation	translation
 **
 ** @return	output stream
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.4
 **/
ostream&
operator<<(ostream& ostr,
           Translator::Translation const& translation)
{
  return translation.write(ostr);
} // ostream& operator==(ostream& ostr, Translator::Translation const& translation)

/**
 ** -> result
 ** 
 ** @param     type   parameter type
 ** 
 ** @return    name of the parameter type
 ** 
 ** @author    Diether Knof
 ** 
 ** @version   0.7.4
 **/ 
// returns the name of the translation
string
name(Translator::Translation::Type const& type)
{
  switch (type) {
  case Translator::Translation::CHAR:
    return "char";
  case Translator::Translation::INT:
    return "int";
  case Translator::Translation::UNSIGNED:
    return "unsigned";
  case Translator::Translation::DOUBLE:
    return "double";
  case Translator::Translation::STRING:
    return "string";
  case Translator::Translation::TRANSLATION:
    return "translation";
  } // switch (type)
  return "";
} // string name(Translator::Translation::Type type)
