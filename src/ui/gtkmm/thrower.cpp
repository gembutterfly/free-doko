/********************************************************************** *
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

#ifdef USE_UI_GTKMM

#include "thrower.h"

#include "../../card/card.h"

#ifdef DKNOF
//#define THROWER_INFO
#endif

namespace UI_GTKMM_NS {

/**
 ** constructor
 **
 ** @param     -
 **/
Thrower::Thrower() :
  exception_(),
  exception_type_(NONE),
  depth_(0)
{ }

/**
 ** destructor
 **
 ** @param     -
 **/
Thrower::~Thrower()
{ }

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    whether an exception will be thrown
 **/
Thrower::operator bool() const
{
  return this->will_throw();
} // Thrower::operator bool() const

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    whether an exception will be thrown
 **/
bool
Thrower::will_throw() const
{
  return (this->exception_type() != NONE);
} // bool Thrower::will_throw() const

/**
 ** throws the delayed exception
 **
 ** @param     -
 **
 ** @return    - (of course)
 **/
void
Thrower::throw_it()
throw(GameStatus, Card)
{
  if (this->exception_type() == NONE)
    return ;

#ifdef THROWER_INFO
  cout << "thrower: throw it" << endl;
#endif

  // Since this function is left by throwing an exception,
  // I have to delete the type before the 'switch' (or in each 'case')
  // and I cannot use 'this->clear()' because then I do not have the
  // information afterwards.
  auto const type = this->exception_type();

  this->exception_type_ = NONE;

  switch (type) {
  case NONE:
    break;
  case GAMESTATUS:
    throw (this->exception().game_status);
    break;
  case CARD:
    {
      auto const card = *this->exception().card;
      delete this->exception().card;
      this->exception_.card = nullptr;

      throw (card);
    }
    break;
  }; // switch(type)

  return ;
} // void Thrower::throw_it()

/**
 ** removes the exception to throw
 **
 ** @param     -
 **
 ** @return    -
 **/
void
Thrower::clear()
{
#ifdef THROWER_INFO
  cout << "thrower: clear" << endl;
#endif
  switch (this->exception_type()) {
  case NONE:
  case GAMESTATUS:
    break;
  case CARD:
    delete this->exception().card;
    break;
  } // switch (this->exception_type())

  this->exception_type_ = NONE;

  return ;
} // void Thrower::clear()

/**
 ** increment the depth
 **
 ** @param     -
 **
 ** @return    -
 **/
void
Thrower::inc_depth()
{
  this->depth_ += 1;
  return ;
} // void Thrower::inc_depth()

/**
 ** decrement the depth
 ** if depth '0' is reached, throw
 **
 ** @param     -
 **
 ** @return    -
 **/
void
Thrower::dec_depth()
{
  DEBUG_ASSERTION((this->depth() > 0),
                  "Thrower::dec_depth()\n"
                  "  depth is '0'");
  DEBUG_ASSERTION((this->depth() < 10000),
                  "Thrower::dec_depth()\n"
                  "  depth is too big: " << this->depth());

  this->depth_ -= 1;

  if (this->depth() == 0) {
    this->throw_it();
  }

  return ;
} // void Thrower::dec_depth()

/**
 ** set the depth
 ** only use if you know exactly why you need it!
 **
 ** @param     depth   new depth
 **
 ** @return    -
 **/
void
Thrower::set_depth(unsigned const depth)
{
  this->depth_ = depth;
  return ;
} // void Thrower::set_depth(unsigned depth)

/**
 ** stores 'game_status' for throwing
 **
 ** @param     game_status   object to throw
 ** @param     file          the file of the throw
 ** @param     line          the line number of the throw
 **
 ** @return    -
 **/
void
Thrower::operator()(GameStatus const game_status,
                    string const& file,
                    unsigned const line)
{
#ifdef THROWER_INFO
  cout << "thrower: throw game status '" << game_status << "'" << endl;
#endif
#ifdef DKNOF
  if (this->will_throw()
      && (   (this->exception_type() != GAMESTATUS)
          || (this->exception_.game_status != game_status)) )
    DEBUG_ASSERTION(!this->will_throw(),
                    "Thrower::operator()(GameStatus):\n"
                    "  will already throw '"
                    << this->exception_type() << "'\n"
                    "  first throw: " << this->file << "::" << this->line
                    << '\n'
                    << "  new throw: " << file << "::" << line << '\n');
#endif

  this->exception_type_ = GAMESTATUS;
  this->exception_.game_status = game_status;

  this->file = file;
  this->line = line;

  return ;
} // bool Thrower::operator()(GameStatus game_status, string file, unsigned line)

/**
 ** stores 'card' for throwing
 **
 ** @param     card   object to throw
 ** @param     file   the file of the throw
 ** @param     line   the line number of the throw
 **
 ** @return    -
 **/
void
Thrower::operator()(Card const& card,
                    string const& file,
                    unsigned const line)
{
  // double throw (by clicking to fast)
  if (this->exception_type_ == CARD)
    return ;

#ifdef THROWER_INFO
  cout << "thrower: throw card '" << card << "'" << endl;
#endif
  DEBUG_ASSERTION(!this->will_throw(),
                  "Thrower::operator()(Card):\n"
                  "  will already throw '"
                  << this->exception_type() << "'\n"
                  "  first throw: " << this->file << "::" << this->line
                  << '\n'
                  << "  new throw: " << file << "::" << line << '\n');

  this->exception_type_ = CARD;
  this->exception_.card = new Card(card);

  this->file = file;
  this->line = line;

  return ;
} // bool Thrower::operator()(Card card, string file, unsigned line)

} // namespace UI_GTKMM_NS

#endif // #ifdef USE_UI_GTKMM
