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
#include "trick.h"

#include "hand.h"
#include "../game/game.h"
#include "../player/player.h"
#include "../party/party.h"
#include "../party/rule.h"
#include "../ui/ui.h"
#include "../utils/string.h"

Trick const Trick::EMPTY;

#define GETLINE(istr, line) \
  std::getline(istr, line);  \
  if (*(line.end() - 1) == '\r') \
  line.erase(line.end() - 1)

/**
 ** constructor
 **
 ** @param	startplayer	the player, who has to play the first card
 **
 ** @return	-
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **
 **/
Trick::Trick(Player const& startplayer) :
cards_(),
  game_(&(startplayer.game())),
  no_(startplayer.game().trick_current_no()),
  startplayer_(startplayer.game().no(startplayer)),
  winnerplayer_(UINT_MAX),
intrickpile_(false)
{ }

/**
 ** constructor
 **
 ** @param     startplayer    the number of the startplayer
 ** @param     cards          the cards in the trick (in order of playing)
 ** @param     winnerplayer   the number of the winnerplayer
 **                            default: UINT_MAX
 **
 ** @return    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.3
 **/
Trick::Trick(unsigned const startplayer, vector<Card> const& cards,
	unsigned const winnerplayer) :
cards_(cards),
  game_(NULL),
  no_(UINT_MAX),
  startplayer_(startplayer),
  winnerplayer_(winnerplayer),
intrickpile_(false)
{ }

/**
 ** constructor
 **
 ** @param	istr	the stream to read the trick from
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.5.4
 **
 **/
Trick::Trick(istream& istr) :
cards_(),
  game_(NULL),
  no_(UINT_MAX),
  startplayer_(UINT_MAX),
  winnerplayer_(UINT_MAX),
intrickpile_(false)
{
  string line;
  GETLINE(istr, line);
#ifndef OUTDATED
  // 2006-09-14
  if (   (DK::Utils::String::word_first(line) != "startplayer:")
      && (DK::Utils::String::word_first(line) != "Startplayer:") )
#else
  if (DK::Utils::String::word_first(line) != "startplayer:")
#endif
    return ;

  DK::Utils::String::word_first_remove(line);
  this->startplayer_ = static_cast<unsigned>(atoi(line.c_str()));

  while (istr.good()) {
    GETLINE(istr, line);
    if (line.empty())
      break;
#ifndef OUTDATED
    // 2006-09-14
    if (   (DK::Utils::String::word_first(line) == "winner:")
	|| (DK::Utils::String::word_first(line) == "Winner:") )
#else
      if (DK::Utils::String::word_first(line) == "winner:")
#endif
	break;

    // the line is like: 1: club ace
    DK::Utils::String::word_first_remove(line);
    DK::Utils::String::remove_blanks(line);
    this->cards_.push_back(HandCard(Card(line)));
  } // while (istr.good())
  if (line.empty()
      || !istr.good())
    return ;

#ifndef OUTDATED
  // 2006-09-14
  if (   (DK::Utils::String::word_first(line) == "winner:")
      || (DK::Utils::String::word_first(line) == "Winner:") )
#else
    if (DK::Utils::String::word_first(line) == "winner:")
#endif
    {
      DK::Utils::String::word_first_remove(line);
      this->winnerplayer_ = static_cast<unsigned>(atoi(line.c_str()));
    } // if (DK::Utils::String::word_first(line) == "winner:")

  return ;
} // Trick::Trick(istream& istr)

/**
 ** constructor
 **
 ** @param	-
 **
 ** @return	-
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **
 **/
Trick::Trick() :
  cards_(),
  game_(NULL),
  no_(UINT_MAX),
  startplayer_(UINT_MAX),
  winnerplayer_(UINT_MAX),
  intrickpile_(false)
{ }

/**
 ** destructor
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 **
 ** @version	0.4.4
 **
 **/
Trick::~Trick()
{ }

/**
 ** copy constructor
 **
 ** @param	trick	the trick to copy
 **
 ** @return	-
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **
 **/
Trick::Trick(Trick const& trick) :
  cards_(trick.cards_),
  game_(trick.game_),
  no_(trick.no_),
  startplayer_(trick.startplayer_),
  winnerplayer_(trick.winnerplayer_),
  intrickpile_(trick.intrickpile_)
{ }

/**
 ** copy method
 **
 ** @param	trick	the trick to copy
 **
 ** @return	the trick
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **
 **/
Trick&
Trick::operator=(Trick const& trick)
{
  this->cards_ = trick.cards_;
  this->game_ = trick.game_;
  this->no_ = trick.no_;
  this->startplayer_ = trick.startplayer_;
  this->winnerplayer_ = trick.winnerplayer_;
  this->intrickpile_ = trick.intrickpile_;

  return *this;
} // Trick& Trick::operator=(Trick const& trick)

/**
 ** set the game
 **
 ** @param	game   new game of the trick
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.4
 **/
void
Trick::set_game(Game const& game)
{
  this->game_ = &game;
  for (HandCards::iterator c = this->cards_.begin();
       c != this->cards_.end();
       ++c)
    *c = HandCard(game.player(c->player().game().no(c->player())).hand(), *c);

  return ;
} // void Trick::set_game(Game game)

/**
 ** self check
 ** when an error is found, an ASSERTION is created
 **
 ** @param	-
 **
 ** @return	wether the self-check was successful (no error)
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **
 **/
bool
Trick::self_check() const
{
  DEBUG_ASSERTION(this->game_,
		  "Trick::self_check()\n"
		  "  no game");

  DEBUG_ASSERTION((this->no_ <= this->game_->trick_current_no()),
		  "Trick::self_check()\n"
		  "  number not valid: " << this->no_);

  DEBUG_ASSERTION((this == &this->game_->trick(this->no_)),
		  "Trick::self_check()\n"
		  "  Not the trick the number says to be");

  DEBUG_ASSERTION(this->startplayerno() != UINT_MAX,
		  "Trick::self_check()\n"
		  "  no startplayer");

  DEBUG_ASSERTION((!this->intrickpile_
		   || (this->winnerplayerno() != UINT_MAX)),
		  "Trick::self_check()\n"
		  "  in trickpile but no winnerplayer");

  return true;
} // bool Trick::self_check() const

/**
 **
 ** output of the trick
 **
 ** @param	ostr	output stream
 **
 ** @return	output stream
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 **
 ** @version	0.4.4
 **
 **/
ostream&
Trick::out(ostream& ostr) const
{
  ostr << "startplayer: " << this->startplayerno() << "\n";
  for(unsigned i = 0; i < this->actcardno(); i++) {
    ostr
      << (this->game_
	  ? this->playerno_of_card(i)
	  : (this->startplayerno() + i))
      << ": "
      << this->card(i) << "\n";
  }

  if (!this->game_
      || this->isfull())
    ostr << "winner: " << this->winnerplayerno() << "\n";

  return ostr;
} // ostream& Trick::out(ostream& ostr) const

/**
 **
 ** output of the trick
 **
 ** @param	ostr	output stream
 ** @param	trick	Trick to be put in the output stream
 **
 ** @return	output stream
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 **
 ** @version	0.4.4
 **
 **/
ostream&
operator<<(ostream& ostr, Trick const& trick)
{
  trick.out(ostr);

  return ostr;
} // ostream& operator<<(ostream& ostr, Trick const& trick)

/**
 **
 ** adds the card 'c' to the trick
 **
 ** @param	card	card, which should be added to this trick
 **
 ** @return	trick with c as last card
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 **
 ** @version	0.4.4
 **
 **/
Trick&
Trick::operator+=(HandCard const& card)
{
  DEBUG_ASSERTION(!card.is_empty(),
		  "Trick::operator+=(HandCard):\n"
		  "  shall add empty card to the trick");

  DEBUG_ASSERTION(   !this->game_
		  || !this->isfull(),
		  "Trick::operator+=(HandCard):\n"
		  "  trick is already full");

  DEBUG_ASSERTION((   !this->game_
		   || (&card.player() == &this->actplayer())),
		  "Trick::operator+=(HandCard):\n"
		  "  Card is from the wrong player "
		  << this->game().no(card.player())
		  << " (should be " << this->game().no(this->actplayer()) << ")\n"
		  "  Pointer: " << &card.player() << " != "
		  << &this->actplayer());

  if (this->game_) {
    // update the winnerplayer
    if (this->isstartcard()) {
      this->winnerplayer_ = this->startplayerno();
    } else {
      if (this->winnercard().less(card)) {
	this->winnerplayer_ = this->game().no(this->actplayer());
      }
    } // if !(this->isstartcard())
  } // if (this->game_)

  // add the card to the trick
  this->cards_.push_back(card);

  return *this;
} // Trick& Trick::operator+=(Card const& c)

/**
 ** -> result
 **
 ** @param	c	number of card
 **
 ** @return	the trick till the given card
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.10
 **/
Trick
Trick::till_card(unsigned const c) const
{
  DEBUG_ASSERTION((c <= this->actcardno()),
                  "Trick::till_card(" << c << ")\n"
                  "  the trick contains only " << this->actcardno() << " cards");

  Trick t(this->startplayer());
  for (unsigned i = 0; i <= c; ++i)
    t += this->card(i);

  return t;
} // Trick Trick::till_card(unsigned c) const

/**
 **
 ** -> result
 **
 ** @param	player   player
 **
 ** @return	the trick till the given player
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.10
 **/
Trick
Trick::till_player(Player const& player) const
{
  DEBUG_ASSERTION(this->has_played(player),
                  "Trick::till_player(" << this->game().no(player) << ")\n"
                  "  the player has not played so far");

  Trick t(this->startplayer());
  unsigned const n = this->cardno_of_player(player);
  for (unsigned i = 0; i <= n; ++i)
    t += this->card(i);

  return t;
} // Trick Trick::till_player(Player player) const

/**
 **
 ** -> result
 **
 ** @param	player   player
 **
 ** @return	the trick till before the given player
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.10
 **/
Trick
Trick::before_player(Player const& player) const
{
  DEBUG_ASSERTION(this->has_played(player),
                  "Trick::before_player(" << this->game().no(player) << ")\n"
                  "  the player has not played so far");

  Trick t(this->startplayer());
  unsigned const n = this->cardno_of_player(player);
  for (unsigned i = 0; i < n; ++i)
    t += this->card(i);

  return t;
} // Trick Trick::before_player(Player player) const

/**
 **
 ** -> result
 **
 ** @param	-
 **
 ** @return	the trick without the last played card
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.10
 **/
Trick
Trick::before_last_played_card() const
{
  DEBUG_ASSERTION(!this->isempty(),
                  "Trick::before_before_last_played_card()\n"
                  "  the trick is empty");

  Trick t(this->startplayer());
  for (unsigned i = 0; i + 1 < this->actcardno(); ++i)
    t += this->card(i);

  return t;
} // Trick Trick::before_last_played_card() const

/**
 **
 ** -> result
 **
 ** @param	c	number of card for the result
 **
 ** @return	card c of actual trick
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 **
 ** @version	0.4.4
 **
 **/
HandCard const&
Trick::card(unsigned const c) const
{
  DEBUG_ASSERTION((c < this->actcardno()),
                  "Trick::card(c):\n"
                  "  'c' is to great (" << c << ">=" << this->actcardno()
                  << ")");

  return this->cards_[c];
} // HandCard const& Trick::card(unsigned const c) const


/**
 **
 ** -> result
 **
 ** @param	player	player whose card is returned
 **
 ** @return	card of 'player' of actual trick
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 **
 ** @version	0.4.4
 **
 **/
HandCard const&
Trick::card_of_player(Player const& player) const
{
  return ((this->game().no(player) >= this->startplayerno())
          ? this->card(this->game().no(player)
                       - this->startplayerno())
          : this->card(this->game().no(player)
                       + this->game().number_of_players()
                       - this->startplayerno())
         );
} // HandCard const& Trick::card_of_player(Player const& player) const

/**
 **
 ** -> result
 **
 ** @param	c	number of card for the result
 **
 ** @return	the player, who has played the 'c'th card
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 **
 ** @version	0.4.4
 **
 **/
Player const&
Trick::player_of_card(unsigned const c) const
{
  return this->game().player(this->playerno_of_card(c));
} // Player const& Trick::player_of_card(unsigned const c) const

/**
 **
 ** -> result
 **
 ** @param	c	number of card for the result
 **
 ** @return	the nubmer of the player, who has played the 'c'th card
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.13
 **/
unsigned
Trick::playerno_of_card(unsigned const c) const
{
  return ((this->startplayerno() + c
           < this->game().number_of_players())
          ? (c + this->startplayerno())
          : (c + this->startplayerno()
             - this->game().number_of_players())
         );
} // unsigned Trick::playerno_of_card(unsigned const c) const

/**
 **
 ** -> result
 **
 ** @param	player	player, whose cardno is returnes
 **
 ** @return	the number of the card, the player has played
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 **
 ** @version	0.4.4
 **
 **/
unsigned
Trick::cardno_of_player(Player const& player) const
{
  return ((this->startplayerno()
           <= this->game().no(player))
          ? (this->game().no(player) - this->startplayerno())
          : (this->game().no(player)
             + this->game().number_of_players()
             - this->startplayerno())
         );
} // unsigned Trick::cardno_of_player(Player const& player) const

/**
 ** -> result
 **
 ** @param     player   player to check
 **
 ** @return    whether the player has already played a card
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
bool
Trick::has_played(Player const& player) const
{
  return (this->cardno_of_player(player) < this->actcardno());
} // bool Trick::has_played(Player player) const

/**
 ** -> result
 **
 ** @param     card   card to check
 **
 ** @return    whether the card is in the trick
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.10
 **/
bool
Trick::contains(Card const& card) const
{
  for (unsigned c = 0; c < this->actcardno(); c++)
    if (this->card(c) == card)
      return true;

  return false;
} // bool Trick::contains(Card card) const

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    whether the trick contains a fox
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.10
 **/
bool
Trick::contains_fox() const
{
  // first lets check if there is a fox
  for (unsigned c = 0; c < this->actcardno(); ++c)
    if (this->card(c).isfox() )
      return true;

  return false;
} // bool Trick::contains_fox() const

/**
 ** -> result
 **
 ** @param     game    corresponding game
 **
 ** @return    whether the trick contains a possible extrapoint
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.10
 **/
bool
Trick::contains_possible_extrapoint(Game const& game) const
{
  if (game.is_solo())
    return false;

  Rule const& rule = game.rule();

  if (   rule(Rule::EXTRAPOINT_CATCH_FOX)
      && this->contains(Card::FOX)
      && !this->game().swines_owner())
    return true;
  if (   rule(Rule::EXTRAPOINT_FOX_LAST_TRICK)
      && game.is_last_trick()
      && this->winnercard().isfox())
    return true;
  if (   rule(Rule::EXTRAPOINT_CATCH_FOX_LAST_TRICK)
      && game.is_last_trick()
      && this->contains(Card::FOX) )
    return true;
  if (   rule(Rule::EXTRAPOINT_CHARLIE)
      && game.is_last_trick()
      && (this->winnercard() == Card::CHARLIE) )
    return true;
  if (   rule(Rule::EXTRAPOINT_CATCH_CHARLIE)
      && game.is_last_trick()
      && this->contains(Card::CHARLIE) )
    return true;
  if (   rule(Rule::EXTRAPOINT_DOLLE_JABS_DOLLE)
      && (this->winnercard() == Card::DOLLE) )
    return true;
  if (rule(Rule::EXTRAPOINT_HEART_TRICK)) {
    unsigned c = 0;
    for (c = 0; c < this->actcardno(); c++)
      if (this->card(c).tcolor() != Card::HEART)
        break;
    if (c == this->actcardno())
      return true;
  }

  return false;
} // bool Trick::contains_possible_extrapoint(Game game) const

/**
 **
 ** -> result
 **
 ** @param	-
 **
 ** @return	the number of the startplayer
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.8
 **
 **/
unsigned
Trick::startplayerno() const
{
  return this->startplayer_;
} // unsigned Trick::startplayerno() const

/**
 **
 ** -> result
 **
 ** @param	-
 **
 ** @return	the startplayer
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.8
 **
 **/
Player const&
Trick::startplayer() const
{
  DEBUG_ASSERTION(this->startplayerno() != UINT_MAX,
                  "Trick::startplayer():\n"
                  "  startplayerno() == NULL != UINT_MAX");

  return this->game().player(this->startplayerno());
} // Player const& Trick::startplayer() const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	the last player
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.2
 **/
Player const&
Trick::lastplayer() const
{
  DEBUG_ASSERTION(this->startplayerno() != UINT_MAX,
                  "Trick::lastplayer():\n"
                  "  startplayerno() == N != UINT_MAX");

  return this->game().player((this->startplayerno()
                              + this->game().number_of_players() - 1)
                             % this->game().number_of_players());
} // Player const& Trick::lastplayer() const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	the last player
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.12
 **/
Player const&
Trick::secondlastplayer() const
{
  DEBUG_ASSERTION(this->startplayerno() != UINT_MAX,
                  "Trick::secondlastplayer():\n"
                  "  startplayerno() == N != UINT_MAX");

  return this->game().player((this->startplayerno()
                              + this->game().number_of_players() - 2)
                             % this->game().number_of_players());
} // Player const& Trick::secondlastplayer() const

/**
 ** set the startplayer
 **
 ** @param     startplayer   new startplayer
 **
 ** @return    the startplayer
 **
 ** @author    Diether Knof
 **
 ** @version   0.4.4
 **/
void
Trick::set_startplayer(Player const& startplayer)
{
  DEBUG_ASSERTION(this->startplayerno() == UINT_MAX,
                  "Trick::startplayer_set(startplayer):\n"
                  "  startplayer already set");

  this->startplayer_ = this->game().no(startplayer);

  return ;
} // void Trick::set_startplayer(Player startplayer)

/**
 ** set the startplayer
 **
 ** @param     startplayerno   number of new startplayer
 **
 ** @return    the startplayer
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.3
 **/
void
Trick::set_startplayer(unsigned const startplayerno)
{
  DEBUG_ASSERTION(this->startplayerno() == UINT_MAX,
                  "Trick::startplayer_set(startplayer):\n"
                  "  startplayer already set");

  this->startplayer_ = startplayerno;

  return ;
} // void Trick::set_startplayer(unsigned startplayerno)

/**
 **
 ** -> result
 **
 ** @param	-
 **
 ** @return	the startcard
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 **
 ** @version	0.4.4
 **
 **/
HandCard const&
Trick::startcard() const
{
  DEBUG_ASSERTION(!this->isstartcard(),
                  "Trick::startcard():\n"
                  "  trick is empty");

  return this->cards_[0];
} // HandCard Trick::startcard() const

/**
 **
 ** -> result
 **
 ** @param	-
 **
 ** @return	whether the trick is empty
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 **
 ** @version	0.4.4
 **
 **/
bool
Trick::isstartcard() const
{
  return (this->actcardno() == 0);
} // bool Trick::isstartcard() const

/**
 **
 ** -> result
 **
 ** @param	-
 **
 ** @return	whether the trick is empty
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 **
 ** @version	0.4.4
 **
 **/
bool
Trick::isempty() const
{
  return (this->actcardno() == 0);
} // bool Trick::isempty() const

/**
 **
 ** -> result
 **
 ** @param	-
 **
 ** @return	whether the trick is full
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 **
 ** @version	0.4.4
 **
 **/
bool
Trick::isfull() const
{
  return (this->remainingcardno() == 0);
} // bool Trick::isfull() const

/**
 **
 ** -> result
 **
 ** @param	-
 **
 ** @return	whether the trick lacks only one card
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 **
 ** @version	0.4.4
 **
 **/
bool
Trick::islastcard() const
{
  return (this->remainingcardno() == 1);
} // bool Trick::islastcard() const

/**
 **
 ** moves the trick in the trickpile
 **
 ** @param	-
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **
 ** @bug	const cast
 **
 **/
void
Trick::move_in_trickpile()
{
  // Bug: const cast
  const_cast<Player&>(this->winnerplayer()).move_in_trickpile(*this);

  this->intrickpile_ = true;

  if (!this->game().isvirtual())
    ::ui->trick_move_in_pile();

  return ;
} // void Trick::move_in_trickpile()

/**
 **
 ** -> result
 **
 ** @param	-
 **
 ** @return	the number of the acting card
 **		(that is, how many cards there are in the tricks)
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 **
 ** @version	0.4.4
 **
 **/
unsigned
Trick::actcardno() const
{
  return this->cards_.size();
} // unsigned Trick::actcardno() const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	the number of remaining cards to play in the trick
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.4
 **/
unsigned
Trick::remainingcardno() const
{
  return (this->game().number_of_players() - this->actcardno());
} // unsigned Trick::remainingcardno() const

/**
 **
 ** -> result
 **
 ** @param	-
 **
 ** @return	the acting player
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 **
 ** @version	0.4.4
 **
 **/
Player const&
Trick::actplayer() const
{
  return this->game().player(this->actplayerno());
} // Player const& Trick::actplayer() const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	the acting player number
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 **
 ** @version	0.7.13
 **/
unsigned
Trick::actplayerno() const
{
  return ((this->startplayerno() + this->actcardno())
          % this->game().number_of_players());
} // unsigned Trick::actplayerno() const

/**
 **
 ** -> result
 **
 ** @param	-
 **
 ** @return	the points in the trick
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 **
 ** @version	0.4.4
 **
 **/
unsigned
Trick::points() const
{
  unsigned points = 0;

  for (unsigned c = 0; c < this->actcardno(); c++)
    points += (this->card(c).is_unknown() ? 0 : this->card(c).points());

  return points;
} // unsigned Trick::points() const

/**
 **
 ** -> result
 **
 ** @param	card	card which is checked
 **
 ** @return	whether it is valid to play 'card' with its hand
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.8
 **
 **/
bool
Trick::isvalid(HandCard const& card) const
{
  return card.isvalid(*this);
} // bool Trick::isvalid(HandCard card) const

/**
 **
 ** -> result
 **
 ** @param	card	card which is checked
 ** @param	hand	hand of the card
 **
 ** @return	whether it is valid to play 'card' with the hand 'hand'
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.8
 **
 **/
bool
Trick::isvalid(Card const& card, Hand const& hand) const
{
  return this->isvalid(HandCard(hand, card));
} // bool Trick::isvalid(Card const& card, Hand const& hand) const

/**
 **
 ** -> result
 **
 ** @param	-
 **
 ** @return	the number of the winnerplayer
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.8
 **
 **/
unsigned
Trick::winnerplayerno() const
{
  return this->winnerplayer_;
} // unsigned Trick::winnerplayerno() const

/**
 **
 ** -> result
 **
 ** @param	-
 **
 ** @return	the player which wins this trick
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 **
 ** @version	0.6.8
 **
 **/
Player const&
Trick::winnerplayer() const
{
  // for Game::numberoftricks_of_player()
  if (this->winnerplayerno() == UINT_MAX)
    return this->startplayer();

  DEBUG_ASSERTION(this->winnerplayerno() != UINT_MAX,
                  "Trick::winnerplayer():\n"
                  "  winnerplayerno() == UINT_MAX");

  return this->game().player(this->winnerplayerno());
} // Player const& Trick::winnerplayer() const

/**
 **
 ** -> result
 **
 ** @param	-
 **
 ** @return	the team which wins the trick
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 **
 ** @version	0.4.4
 **
 **/
Team
Trick::winnerteam() const
{
  return this->winnerplayer().team();
} // Team Trick::winnerteam() const

/**
 **
 ** -> result
 **
 ** @param	-
 **
 ** @return	card that `makes` the trick
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 **
 ** @version	0.4.4
 **
 **/
HandCard const&
Trick::winnercard() const
{
  if (this->isempty())
    return HandCard::EMPTY;

  return this->card_of_player(this->winnerplayer());
} // HandCard Trick::winnercard() const

/**
 **
 ** returns the number of the winnercard
 **
 ** @param	-
 **
 ** @return	cardno that `makes` the trick
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 **
 ** @version	0.4.4
 **
 **/
unsigned
Trick::winnercardno() const
{
  return this->cardno_of_player(this->winnerplayer());
} // unsigned Trick::winnercardno() const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	card which wins the trick without taking the last card played into account
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.9
 **/
HandCard const&
Trick::winnercard_before() const
{
  if (this->actcardno() <= 1)
    return HandCard::EMPTY;

  HandCard const* winnercard = &this->card(0);
  for (unsigned p = 1; p < this->actcardno() - 1; ++p)
    if (winnercard->less(this->card(p)))
      winnercard = &this->card(p);

  return *winnercard;
} // HandCard Trick::winnercard_before() const

/**
 ** -> result
 **
 ** @param	cardno   card to analyse
 **
 ** @return	whether the card jabs the cards before
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 **
 ** @version	0.7.5
 **/
bool
Trick::jabs_cards_before(unsigned const cardno) const
{
  DEBUG_ASSERTION(cardno < this->actcardno(),
                  "Trick::jabs_cards_before(cardno = " << cardno << ")\n"
                  "  Card is not played, yet.\n"
                  "  Trick:\n" << *this);

  HandCard const& card = this->card(cardno);
  for (unsigned c = 0; c < cardno; c++)
    if (!this->card(c).less(card))
      return false;

  return true;
} //  bool Trick::jabs_cards_before(unsigned const cardno) const


/**
 ** -> result
 **
 ** @param	player   player to analyse
 **
 ** @return	whether the player jabs the cards before
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 **
 ** @version	0.7.5
 **/
bool
Trick::jabs_cards_before(Player const& player) const
{
  DEBUG_ASSERTION(this->has_played(player),
                  "Trick::jabs_cards_before(player = " << this->game().no(player) << ")\n"
                  "  Player has not played, yet.\n"
                  "  Trick:\n" << *this);

  return -this->jabs_cards_before(this->cardno_of_player(player));
} //  bool Trick::jabs_cards_before(Player const& player) const

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    whether this is the last trick in the game
 **
 ** @author    Diether Knof
 **
 ** @version   0.6.7
 **/
bool
Trick::islast() const
{
  return (this->no() == this->game().trickno() - 1);
} // bool Trick::islast() const

/**
 ** -> result
 **
 ** @param     card   card to test
 **
 ** @return    whether the trick will be jabbed by this card
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
bool
Trick::isjabbed(Card const& card) const
{
  if (this->isempty())
    return true;
  else
    return this->winnercard().less(card);
} // bool Trick::isjabbed(Card card) const

/**
 ** -> result
 **
 ** @param     color   color of the card to test
 ** @param     value   value of the card to test
 **
 ** @return    whether the trick will be jabbed by this card
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.11
 **/
bool
Trick::isjabbed(Card::Color const& color, Card::Value const& value) const
{
  return this->isjabbed(Card(color, value));
} // bool Trick::isjabbed(Card::Color color, Card::Value value) const

/**
 ** -> result
 ** take possible special cards into account (p.e. swines, that are not announced)
 **
 ** @param     card   card to test
 **
 ** @return    whether the trick will be jabbed by this card
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.11
 **/
bool
Trick::isjabbed(HandCard const& card) const
{
  if (this->isempty())
    return true;
  else {
    if (card.possible_hyperswine())
      return true;
    else if (card.possible_swine())
      return !this->winnercard().ishyperswine();
    else
      return this->winnercard().less(card);
  }
} // bool Trick::isjabbed(HandCard card) const

/**
 ** -> result
 ** take possible special cards into account (p.e. swines, that are not announced)
 **
 ** @param     card   card to test
 ** @param     hand   corresponding hand
 **
 ** @return    whether the trick will be jabbed by this card
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.11
 **/
bool
Trick::isjabbed(Card const& card, Hand const& hand) const
{
  return this->isjabbed(HandCard(hand, card));
} // bool Trick::isjabbed(Card card, Hand hand) const

/**
 ** -> result
 **
 ** @param     cardno_a   number of the first card
 ** @param     cardno_b   number of the second card
 **
 ** @return    whether the first card is less than the second card in this trick
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
bool
Trick::less(unsigned const cardno_a, unsigned const cardno_b) const
{
  DEBUG_ASSERTION(   (cardno_a < this->actcardno())
                  && (cardno_b < this->actcardno()),
                  "Trick::less(" << cardno_a << ", " << cardno_b << ")\n"
                  "  one cardno is less than the actcardno " << this->actcardno()
                 );
  if (cardno_a == cardno_b)
    return false;
  else if (cardno_a < cardno_b)
    return this->card(cardno_a).less(this->card(cardno_b));
  else // cardno_a > cardno_b
    return !this->card(cardno_b).less(this->card(cardno_a));
} // bool Trick::less(unsigned cardno_a, unsigned cardno_b) const

/**
 ** comparison of two tricks
 **
 ** @param	trick1	first trick to compare
 ** @param	trick2	second trick to compare
 **
 ** @return	whether the tricks are equal
 **
 ** @author	Diether Knof
 **
 ** @version	0.5.4
 **
 **/
bool
operator==(Trick const& trick1, Trick const& trick2)
{
  if ((trick1.startplayerno() != trick2.startplayerno())
      || (trick1.actcardno() != trick2.actcardno()))
    return false;

  for (unsigned c = 0; c < trick1.actcardno(); c++)
    if (trick1.card(c) != trick2.card(c))
      return false;

  if (trick1.winnerplayerno() != trick2.winnerplayerno())
    return false;

  return true;
} // bool operator==(Trick const& trick1, Trick const& trick2)

/**
 ** comparison of two tricks
 **
 ** @param	trick1	first trick to compare
 ** @param	trick2	second trick to compare
 **
 ** @return	whether the tricks are different
 **
 ** @author	Diether Knof
 **
 ** @version	0.5.4
 **
 **/
bool
operator!=(Trick const& trick1, Trick const& trick2)
{
  return !(trick1 == trick2);
} // bool operator!=(Trick const& trick1, Trick const& trick2)

/**
 **
 ** counts all points witch are turned on in rules and determines
 ** team which has won this point.
 ** takes all inforation from the game the trick belongs to.
 **
 ** @param	-
 **
 ** @return	a vector with names of specialpoints and type of them
 **		for solo games return value is an empty vector
 **
 ** @author	Diether Knof
 **
 ** @version	0.5.1
 **
 **/
Specialpointsvector
Trick::specialpoints() const
{
  vector<Team> teams;

  for (vector<Player*>::const_iterator p = this->game().players_begin();
       p != this->game().players_end();
       p++) {
    teams.push_back(this->game().teaminfo(**p));
  }

  return this->specialpoints(teams);
} // Specialpointsvector Trick::specialpoints() const

/**
 **
 ** counts all points witch are turned on in rules and determines
 ** team which has won this point
 **
 ** @param	teams	the teams of the players
 **
 ** @return	a vector with names of specialpoints and type of them
 **		for solo games return value is an empty vector
 **
 ** @author	Borg Enders
 **
 ** @version	0.4.4
 **
 **/
Specialpointsvector
Trick::specialpoints(vector<Team> const& teams) const
{
  Specialpointsvector sp;

  Team const local_winnerteam
    = ( (  (teams[this->winnerplayerno()] != TEAM::UNKNOWN)
         ? teams[this->winnerplayerno()]
         : TEAM::NOTEAM) );

  Rule const& rule = this->game().rule();

  // if this game is a solo there are no specialpoints
  if (   GAMETYPE::is_solo(this->game().type())
      && !(   (this->game().type() == GAMETYPE::MARRIAGE_SOLO)
           && (this->no() < this->game().marriage_determination_trickno()) ) )
    return sp;

  bool dolle_caught = false;

  // doppelkopf
  if (this->points() >= rule(Rule::POINTS_FOR_DOPPELKOPF)) {
    Specialpoints s(SPECIALPOINT::DOPPELKOPF, local_winnerteam);
    s.player_get_no = this->winnerplayerno();

    sp.push_back(s);
  } // if (this->points() >= rule(Rule::POINTS_FOR_DOPPELKOPF))


  bool fl_caught_charlie=false;
  for (unsigned i = 0; i < this->actcardno(); i++) {
    // the card belongs to the winner team
    if (   ::is_real(local_winnerteam)
        && (local_winnerteam == teams[this->playerno_of_card(i)]) )
      continue;

    // fox caught
    if (   this->card(i).isfox()
        && rule(Rule::EXTRAPOINT_CATCH_FOX)
        && (this->winnerplayer() != this->player_of_card(i))) {
      Specialpoints s(SPECIALPOINT::CAUGHT_FOX, local_winnerteam);
      s.player_get_no = this->winnerplayerno();
      s.player_of_no = this->playerno_of_card(i);

      sp.push_back(s);

      if (this->islast()
          && rule(Rule::EXTRAPOINT_CATCH_FOX_LAST_TRICK)) {
        Specialpoints s(SPECIALPOINT::CAUGHT_FOX_LAST_TRICK, local_winnerteam);
        s.player_get_no = this->winnerplayerno();
        s.player_of_no = this->playerno_of_card(i);

        sp.push_back(s);
      }
    }

    // Charlie caught
    if (this->islast()
        && (card(i) == Card::CHARLIE)
        && !fl_caught_charlie
        && rule(Rule::EXTRAPOINT_CATCH_CHARLIE)
        && (!rule(Rule::EXTRAPOINT_CATCH_CHARLIE_ONLY_WITH_DIAMOND_QUEEN)
            || (this->winnercard() == Card(Card::DIAMOND, Card::QUEEN)) )
        && (local_winnerteam != teams[this->playerno_of_card(i)])) {

      fl_caught_charlie=true;

      Specialpoints s(SPECIALPOINT::CAUGHT_CHARLIE, local_winnerteam);
      s.player_get_no = this->winnerplayerno();
      s.player_of_no = this->playerno_of_card(i);
      sp.push_back(s);
      // check for double charlies caught
      if (rule(Rule::EXTRAPOINT_CATCH_DOUBLE_CHARLIE))
        for (unsigned n = i + 1; n < actcardno(); ++n) {
          if (   (card(n) == Card::CHARLIE)
              && (teams[this->playerno_of_card(n)] != s.team) ) {
            Specialpoints s(SPECIALPOINT::CAUGHT_CHARLIE,
                            local_winnerteam);
            s.player_get_no = this->winnerplayerno();
            s.player_of_no = this->playerno_of_card(n);
            sp.push_back(s);
          }
        }
    } // if (charlie caught)

    // dolle caught
    dolle_caught |= this->card(i).isdolle();

  } // for (i < this->actcardno())

  if (rule(Rule::EXTRAPOINT_DOLLE_JABS_DOLLE)
      && dolle_caught
      && (winnercard() == Card::DOLLE)) {
    for (unsigned c = 0; c < this->actcardno(); c++) {
      if (   (c != this->winnercardno())
          && (this->card(c) == Card::DOLLE)
          && (local_winnerteam != teams[this->playerno_of_card(c)]) ) {
        Specialpoints s(SPECIALPOINT::DOLLE_CAUGHT_DOLLE,
                        local_winnerteam);
        s.player_get_no = this->winnerplayerno();
        s.player_of_no = this->playerno_of_card(c);

        sp.push_back(s);
      } // if (dolle caught of opposite team)
    } // for (c)
  } // if (dolle caught)

  if (rule(Rule::EXTRAPOINT_HEART_TRICK)
      && (GAMETYPE::is_normal(this->game().type()))
      && (this->winnercard().tcolor() == Card::HEART)) {
    unsigned c;
    for (c = 0; c < this->actcardno(); ++c)
      if (this->card(c).tcolor() != Card::HEART)
        break;
    if (c == this->actcardno()) {
      Specialpoints s(SPECIALPOINT::HEART_TRICK,
                      local_winnerteam);
      s.player_get_no = this->winnerplayerno();

      sp.push_back(s);
    }
  } // if (heart trick)

  // check for foxes who are winning the last trick
  if (this->islast()
      && (winnercard().isfox())
      && rule(Rule::EXTRAPOINT_FOX_LAST_TRICK)) {

    Specialpoints s(SPECIALPOINT::FOX_LAST_TRICK,
                    local_winnerteam);
    s.player_get_no = this->winnerplayerno();
    //s.player_of_no = this->winnerplayerno();
    sp.push_back(s);

    // check for double fox winning
    if (rule(Rule::EXTRAPOINT_DOUBLE_FOX_LAST_TRICK))
      for (unsigned n=winnercardno()+1; n<actcardno(); n++)
      {
        if(card(n).isfox() &&
           teams[this->playerno_of_card(n)]==s.team) {
          Specialpoints s(SPECIALPOINT::FOX_LAST_TRICK,
                          local_winnerteam);
          s.player_get_no = this->winnerplayerno();
          //s.player_of_no = this->playerno_of_card(n);
          sp.push_back(s);
        }
      }
  }


  // check for charlies in lasttrick
  if (this->islast()) {
    if ((winnercard()==Card::CHARLIE)
        && rule(Rule::EXTRAPOINT_CHARLIE))
    {

      Specialpoints s(SPECIALPOINT::CHARLIE,
                      local_winnerteam);
      s.player_get_no = this->winnerplayerno();
      //s.player_of_no = this->winnerplayerno();
      sp.push_back(s);

      // check for double charlies
      if (rule(Rule::EXTRAPOINT_DOUBLE_CHARLIE))
        for (unsigned n=winnercardno()+1; n<actcardno(); n++)
        {
          if(card(n)==Card::CHARLIE &&
             teams[this->playerno_of_card(n)]==s.team)
          {
            Specialpoints s(SPECIALPOINT::CHARLIE,
                            local_winnerteam);
            s.player_get_no = this->winnerplayerno();
            //s.player_of_no = this->playerno_of_card(n);

            sp.push_back(s);
          }
        }
    } // if (charlie)
  }

  return sp;
} // Specialpointsvector Trick::specialpoints(vector<Team> const& teams) const

/**
 **
 ** -> result
 **
 ** @param	-
 **
 ** @return	a vector with the cards that made a specialpoint
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **
 **/
HandCards
Trick::specialpoints_cards() const
{
  Specialpointsvector const spv = this->specialpoints();

  HandCards cards;

  for (Specialpointsvector::const_iterator sp = spv.begin();
       sp != spv.end();
       ++sp) {
    switch (sp->type) {
    case SPECIALPOINT::NOSPECIALPOINT:
      break;
    case SPECIALPOINT::CAUGHT_FOX:
    case SPECIALPOINT::CAUGHT_FOX_LAST_TRICK:
      for (HandCards::const_iterator c = this->cards_.begin();
           c != this->cards_.end();
           ++c)
        if (c->istrumpace()) {
          cards.push_back(*c);
          break;
        }
      break;
    case SPECIALPOINT::FOX_LAST_TRICK:
      cards.push_back(this->winnercard());
      break;
    case SPECIALPOINT::CHARLIE:
      cards.push_back(this->winnercard());
      break;
    case SPECIALPOINT::CAUGHT_CHARLIE:
      for (HandCards::const_iterator c = this->cards_.begin();
           c != this->cards_.end();
           ++c)
        if (*c == Card::CHARLIE) {
          cards.push_back(*c);
          break;
        }
      break;
    case SPECIALPOINT::DOLLE_CAUGHT_DOLLE:
      for (HandCards::const_reverse_iterator c = this->cards_.rbegin();
           c != this->cards_.rend();
           ++c)
        if (*c == Card::DOLLE) {
          cards.push_back(*c);
          break;
        }
      break;
    case SPECIALPOINT::HEART_TRICK:
      cards.push_back(this->winnercard());
      break;
    case SPECIALPOINT::DOPPELKOPF:
      // check for confusing winnercard
      if (   !this->winnercard().istrumpace()
          && (this->winnercard() != Card::DOLLE)) {
        cards.push_back(this->winnercard());
        break;
      }
      { // search better card
        HandCards::const_iterator c;
        for (c = this->cards_.begin();
             c != this->cards_.end();
             ++c) {
          if (   !c->istrumpace()
              && (*c != Card::DOLLE)) {
            cards.push_back(*c);
            break;
          }
        } // for (c \in this->cards_)
        if (c != this->cards_.end())
          break;
      } // search better card

      // found no better card -- taking winnercard
      cards.push_back(this->winnercard());
      break;
    case SPECIALPOINT::WON:
    case SPECIALPOINT::NO90:
    case SPECIALPOINT::NO60:
    case SPECIALPOINT::NO30:
    case SPECIALPOINT::NO0:
    case SPECIALPOINT::NO120_SAID:
    case SPECIALPOINT::NO90_SAID:
    case SPECIALPOINT::NO60_SAID:
    case SPECIALPOINT::NO30_SAID:
    case SPECIALPOINT::NO0_SAID:
    case SPECIALPOINT::NO90_SAID_120_GOT:
    case SPECIALPOINT::NO60_SAID_90_GOT:
    case SPECIALPOINT::NO30_SAID_60_GOT:
    case SPECIALPOINT::NO0_SAID_30_GOT:
    case SPECIALPOINT::NO120_REPLY:
    case SPECIALPOINT::NO90_REPLY:
    case SPECIALPOINT::NO60_REPLY:
    case SPECIALPOINT::NO30_REPLY:
    case SPECIALPOINT::NO0_REPLY:
    case SPECIALPOINT::CONTRA_WON:
    case SPECIALPOINT::SOLO:
    case SPECIALPOINT::BOCK:
      break;
    } // switch (sp->type)
  } // for (sp \in spv)

  return cards;
} // HandCards Trick::specialpoints_cards() const

/**
 ** -> result
 **
 ** @return	c   cardnumber of actual trick
 **
 ** @return	whether this is a specialpoints card
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.5
 **/
bool
Trick::is_specialpoints_card(unsigned const c) const
{
  DEBUG_ASSERTION((c < this->actcardno()),
                  "Trick::is_specialpoints_card(c):\n"
                  "  'c' is to great (" << c << ">=" << this->actcardno()
                  << ")");

  unsigned const playerno = this->playerno_of_card(c);
  HandCards const sp_cards = this->specialpoints_cards();
  for (HandCards::const_iterator sp_card = sp_cards.begin();
       sp_card != sp_cards.end();
       ++sp_card) {
    if (this->game().no(sp_card->player()) == playerno)
      return true;
  } // for (sp_card)

  return false;
} // bool Trick::is_specialpoints_card(unsigned c) const
