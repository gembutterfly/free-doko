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

#include "rule.h"

#include "../utils/file.h"

#include "party.h"
#include "../ui/ui.h"
#include "../misc/setting.h"
#include "../misc/translations.h"
#include "../basistypes.h"


// number of the Type
unsigned const Rule::BOOL_NUMBER = (Rule::BOOL_LAST
				    - Rule::BOOL_FIRST + 1);
unsigned const Rule::UNSIGNED_NUMBER = (Rule::UNSIGNED_LAST
					- Rule::UNSIGNED_FIRST + 1);

/**
 ** constructor
 **
 ** @param	-
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.3
 **/
Rule::Rule() :
  bool_p(BOOL_NUMBER),
  unsigned_p(UNSIGNED_NUMBER),
  counting_(COUNTING_NS::PLUS)
{
  this->reset_to_hardcoded();
  return ;
} // Rule::Rule()

/**
 ** resets the rules to the default values
 **
 ** @param	-
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.3
 **/
void
Rule::reset_to_hardcoded()
{
  this->set_to(FREEDOKO);

  return ;
} // void Rule::reset_to_hardcoded()

/**
 ** resets the rules to a hardcoded ruleset
 **
 ** @param	ruleset   ruleset to set to
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.3
 **/
void
Rule::set_to(Rule::Ruleset const& ruleset)
{
  // first set to the ddv rules, then change according to the ruleset
  //
  this->set(UNSUPPORTED,			0);

  this->set(MUTATE,				false);
  this->set(SHOW_IF_VALID,			true);
  this->set(SHOW_ALL_HANDS,			false);
  this->set(SHOW_TRICKS_NUMBER,			1);
  this->set(SHOW_TRICKS_IN_TRICKPILES,		true);
  this->set(SHOW_TRICKPILES_POINTS,		false);
  this->set(SHOW_KNOWN_TEAMS_IN_GAME,		false);
  this->set(SHOW_SOLOPLAYER_IN_GAME,		true);

  this->set(CARD_HINT,				false);
  this->set(TAKE_BACK_TRICKS,			false);

  this->set(WITH_NINES,				true);

  this->set(BOCK,				false);
  this->set(BOCK_MULTIPLIER,			2);
  this->set(BOCK_APPEND,			false);
  this->set(BOCK_120,				false);
  this->set(BOCK_SOLO_LOST,			false);
  this->set(BOCK_RE_LOST,			false);
  this->set(BOCK_HEART_TRICK,			false);
  this->set(BOCK_BLACK,				false);

  this->set(THROW_WITH_NINES,			false);
  this->set(MIN_NUMBER_OF_THROWING_NINES,	5);
  this->set(THROW_WITH_KINGS,			false);
  this->set(MIN_NUMBER_OF_THROWING_KINGS,	5);
  this->set(THROW_WITH_NINES_AND_KINGS,		false);
  this->set(MIN_NUMBER_OF_THROWING_NINES_AND_KINGS,	7);
  this->set(THROW_WITH_RICHNESS,		false);
  this->set(MIN_RICHNESS_FOR_THROWING,	        70);

  this->set(POVERTY,				false);
  this->set(POVERTY_SHIFT,			false);

  this->set(POVERTY_SHIFT_ONLY_TRUMP,		false);
  this->set(POVERTY_FOX_DO_NOT_COUNT,		false);
  this->set(POVERTY_FOX_SHIFT_EXTRA,		false);
  this->set(POVERTY_FOX_SHIFT_OPEN,		false);
  this->set(THROW_WITH_ONE_TRUMP,		false);

  this->set(THROW_WHEN_FOX_HIGHEST_TRUMP,	false);

  this->set(MARRIAGE_DETERMINATION,		12);
  this->set(MARRIAGE_FIRST_FOREIGN,		true);
  this->set(MARRIAGE_FIRST_COLOR,		false);
  this->set(MARRIAGE_FIRST_SINGLE_COLOR,	false);
  this->set(MARRIAGE_FIRST_TRUMP,		false);
  this->set(MARRIAGE_FIRST_ONE_DECIDES,		false);
  this->set(MARRIAGE_BEFORE_POVERTY,		false);

  this->set(GENSCHER,				false);

  this->set(SOLO,				true);
  this->set(SOLO_JACK,				true);
  this->set(SOLO_QUEEN,				true);
  this->set(SOLO_KING,				true);
  this->set(SOLO_QUEEN_JACK,			true);
  this->set(SOLO_KING_JACK,			true);
  this->set(SOLO_KING_QUEEN,			true);
  this->set(SOLO_KOEHLER,			true);
  this->set(SOLO_COLOR,				true);
  this->set(SOLO_MEATLESS,			true);

  this->set(THROWING_BEFORE_SOLO,		false);

  this->set(DOLLEN,				true);
  this->set(DOLLEN_SECOND_OVER_FIRST,		false);
  this->set(DOLLEN_CONTRARY_IN_LAST_TRICK,	false);
  this->set(DOLLEN_FIRST_OVER_SECOND_WITH_SWINES,	false);

  this->set(SWINES,				false);
  this->set(SWINES_ANNOUNCEMENT_BEGIN,		true);
  this->set(SWINE_ONLY_SECOND,			false);
  this->set(SWINES_IN_SOLO,			true);
  this->set(SWINES_IN_POVERTY,			true);
  this->set(HYPERSWINES,			false);
  this->set(HYPERSWINES_ANNOUNCEMENT_BEGIN,	true);
  this->set(HYPERSWINES_IN_SOLO,		true);
  this->set(HYPERSWINES_IN_POVERTY,		true);
  this->set(SWINES_AND_HYPERSWINES_JOINT,	false);

  this->set(EXTRAPOINT_CATCH_FOX,		false);
  this->set(EXTRAPOINT_CATCH_FOX_LAST_TRICK,	false);
  this->set(EXTRAPOINT_FOX_LAST_TRICK,		false);
  this->set(EXTRAPOINT_DOUBLE_FOX_LAST_TRICK,	false);

  this->set(EXTRAPOINT_CHARLIE,			false);
  this->set(EXTRAPOINT_CATCH_CHARLIE,		false);
  this->set(EXTRAPOINT_DOUBLE_CHARLIE,		false);
  this->set(EXTRAPOINT_CATCH_DOUBLE_CHARLIE,	false);
  this->set(EXTRAPOINT_CATCH_CHARLIE_ONLY_WITH_DIAMOND_QUEEN, false);

  this->set(EXTRAPOINT_DOLLE_JABS_DOLLE,	false);
  this->set(EXTRAPOINT_HEART_TRICK,		false);

  this->set(KNOCKING,	                        false);
  this->set(ANNOUNCEMENT_LIMIT_ONLY_FOR_CURRENT,	false);
  this->set(ANNOUNCEMENT_TILL_FULL_TRICK,	false);
  this->set(ANNOUNCEMENT_FIRST_TRICK_THIRTY_POINTS,	false);
  this->set(ANNOUNCEMENT_FIRST_TRICK_THIRTY_POINTS_ONLY_FIRST,	false);
  this->set(ANNOUNCEMENT_FIRST_TRICK_THIRTY_POINTS_ONLY_RE_CONTRA,	true);
  this->set(ANNOUNCEMENT_FIRST_TRICK_THIRTY_POINTS_IN_MARRIAGE,	true);
  this->set(ANNOUNCEMENT_NO_120,		11);
  this->set(ANNOUNCEMENT_NO_90,			 9);
  this->set(ANNOUNCEMENT_NO_60,			 6);
  this->set(ANNOUNCEMENT_NO_30,			 3);
  this->set(ANNOUNCEMENT_NO_0,			 3);
  this->set(ANNOUNCEMENT_LAST,			 3);
  this->set(ANNOUNCEMENT_REPLY,			false);
  this->set(ANNOUNCEMENT_STRICT_LIMIT,	        false);
  this->set(ANNOUNCEMENT_RE_DOUBLES,		false);
  this->set(ANNOUNCEMENT_CONTRA_DOUBLES_AGAINST_RE,	false);

  this->set(NUMBER_OF_ROUNDS_LIMITED,		true);
  this->set(NUMBER_OF_ROUNDS,			20);
  this->set(POINTS_LIMITED,			false);
  this->set(POINTS,				40);
  this->set(NUMBER_OF_DUTY_SOLI,		0);
  this->set(NUMBER_OF_DUTY_COLOR_SOLI,		0);
  this->set(NUMBER_OF_DUTY_PICTURE_SOLI,	0);
  this->set(OFFER_DUTY_SOLO,	        	true);
  this->set(LUSTSOLO_PLAYER_LEADS,		false);
  this->set(SOLO_ALWAYS_COUNTS_TRIPLE,		false);

  // *** this->set(PARTY_POINTS_SUMMING,	"plus");

  this->set(NUMBER_OF_PLAYERS,			4);
  this->set(NUMBER_OF_PLAYERS_IN_GAME,		4);

  this->set(COUNTING,				COUNTING_NS::PLUS);

  switch (ruleset) {
  case DDV:
    break;
  case FREEDOKO:
    this->set(SHOW_IF_VALID,			true);
    this->set(SHOW_ALL_HANDS,			true);
    this->set(SHOW_TRICKS_NUMBER,			12);
    this->set(SHOW_TRICKPILES_POINTS,		false);
    this->set(SHOW_KNOWN_TEAMS_IN_GAME,		true);
    this->set(CARD_HINT,				true);
    this->set(BOCK,				true);
    this->set(BOCK_120,				true);
    this->set(THROW_WITH_NINES,			true);
    this->set(POVERTY,				true);
    this->set(POVERTY_SHIFT,			true);
    this->set(MARRIAGE_FIRST_COLOR,		true);
    this->set(MARRIAGE_FIRST_SINGLE_COLOR,	true);
    this->set(MARRIAGE_FIRST_TRUMP,		true);
    this->set(DOLLEN_SECOND_OVER_FIRST,		true);
    this->set(SWINES,				true);
    this->set(EXTRAPOINT_CATCH_FOX,		true);
    this->set(EXTRAPOINT_FOX_LAST_TRICK,		true);
    this->set(EXTRAPOINT_CHARLIE,			true);
    this->set(EXTRAPOINT_CATCH_CHARLIE,		true);
    this->set(ANNOUNCEMENT_FIRST_TRICK_THIRTY_POINTS,	true);
    this->set(ANNOUNCEMENT_RE_DOUBLES,		true);
    this->set(LUSTSOLO_PLAYER_LEADS,		true);
    break;
  case DOKOLOUNGE_DDV:
    // ToDo: ruleset DOKOLOUNGE_DDV
    this->set(NUMBER_OF_ROUNDS_LIMITED,		false);
    this->set(POINTS_LIMITED,			false);
    this->set(COUNTING,				COUNTING_NS::PLUSMINUS);
    this->set(ANNOUNCEMENT_TILL_FULL_TRICK,	true);
    this->set(EXTRAPOINT_CATCH_FOX,		true);
    this->set(EXTRAPOINT_CHARLIE,		true);
    break;
  case DOKOLOUNGE_SONDERREGELN:
    this->set_to(DOKOLOUNGE_DDV);
    this->set(SHOW_IF_VALID,			true);
    this->set(SHOW_TRICKPILES_POINTS,           true);
    this->set(SHOW_KNOWN_TEAMS_IN_GAME,         true);
    this->set(SHOW_SOLOPLAYER_IN_GAME,          true);
    this->set(EXTRAPOINT_CHARLIE,		true);
    this->set(EXTRAPOINT_CATCH_FOX,		true);
    this->set(ANNOUNCEMENT_LIMIT_ONLY_FOR_CURRENT,true);
    // changeable
    this->set(POVERTY,	                	true);
    this->set(SWINES,	                	true);
    this->set(SWINES_IN_SOLO,	               	true);
    this->set(HYPERSWINES,	                false);
    this->set(HYPERSWINES_IN_SOLO,             	true);
    this->set(DOLLEN_SECOND_OVER_FIRST,		true);
    this->set(DOLLEN_CONTRARY_IN_LAST_TRICK,	false);
    this->set(LUSTSOLO_PLAYER_LEADS,		true);
#ifdef DKNOF
    this->set(SHOW_ALL_HANDS,			true);
    this->set(SHOW_TRICKS_NUMBER,		12);
    this->set(CARD_HINT,                        true);
#endif
    break;
  case DOKOLOUNGE_LERNTISCH:
    // ToDo: ruleset DOKOLOUNGE_LERNTISCH
    this->set_to(DOKOLOUNGE_SONDERREGELN);
    this->set(SHOW_ALL_HANDS,			true);
    this->set(SHOW_TRICKS_NUMBER,		12);
    this->set(CARD_HINT,                        true);
    break;
  } // switch (ruleset)

  return ;
} // void Rule::set_to(Rule::Ruleset ruleset)


/**
 ** resets the rules to the values for the seed statistics
 **
 ** @param	-
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.3
 **/
void
Rule::reset_to_seed_statistics()
{
  this->set(THROW_WITH_NINES,			true);
  this->set(MIN_NUMBER_OF_THROWING_NINES,	5);
  this->set(THROW_WITH_KINGS,			true);
  this->set(MIN_NUMBER_OF_THROWING_KINGS,	5);
  this->set(THROW_WITH_NINES_AND_KINGS,		false);
  this->set(MIN_NUMBER_OF_THROWING_NINES_AND_KINGS,	7);
  this->set(THROW_WITH_RICHNESS,		false);
  this->set(MIN_RICHNESS_FOR_THROWING,	        70);

  this->set(POVERTY,				true);
  this->set(POVERTY_SHIFT,			true);
  this->set(POVERTY_FOX_DO_NOT_COUNT,		true);
  this->set(POVERTY_FOX_SHIFT_EXTRA,		true);
  this->set(THROW_WHEN_FOX_HIGHEST_TRUMP,	true);

  this->set(SOLO,				true);
  this->set(SOLO_JACK,				true);
  this->set(SOLO_QUEEN,				true);
  this->set(SOLO_KING,				true);
  this->set(SOLO_QUEEN_JACK,			true);
  this->set(SOLO_KING_JACK,			true);
  this->set(SOLO_KING_QUEEN,			true);
  this->set(SOLO_KOEHLER,			true);
  this->set(SOLO_COLOR,				true);
  this->set(SOLO_MEATLESS,			true);

  this->set(THROWING_BEFORE_SOLO,		false);

  this->set(SWINES,				true);
  this->set(HYPERSWINES,			true);

  return ;
} // void Rule::reset_to_seed_statistics()

/**
 ** copy constructor
 **
 ** @param	rule	rule to copy
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **
 **/
Rule::Rule(Rule const& rule):
  bool_p(rule.bool_p),
  unsigned_p(rule.unsigned_p),
  counting_(rule.counting_)
{
  return ;
} // Rule::Rule(Rule const& rule) const

/**
 ** copy operator
 **
 ** @param	rule	rule to copy
 **
 ** @return	rule
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **
 **/
Rule&
Rule::operator=(Rule const& rule)
{
  unsigned i;
  for (i = BOOL_FIRST; i <= BOOL_LAST; i++)
    this->set(TypeBool(i), rule.value(TypeBool(i)));
  for (i = UNSIGNED_FIRST; i <= UNSIGNED_LAST; i++)
    this->set(TypeUnsigned(i), rule.value(TypeUnsigned(i)));
  this->set(COUNTING, this->counting());

  return *this;
} // bool Rule::operator=(Rule const& rule) const

/**
 ** -> result
 **
 ** @param	type	rule type
 **
 ** @return	the value of 'type'
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **
 **/
bool
Rule::operator()(TypeBool const& type) const
{
  return (this->dependencies(type) && this->value(type));
} // bool Rule::operator()(TypeBool const& type) const

/**
 ** -> result
 **
 ** @param	type	rule type
 **
 ** @return	the value of 'type'
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **
 **/
unsigned
Rule::operator()(TypeUnsigned const& type) const
{
  switch(type) {
  case NUMBER_OF_DUTY_SOLI:
    if (this->dependencies(type))
      return this->value(type);
    else
      return 0;
  default:
    return this->value(type);
  } // switch(type)

  DEBUG_ASSERT(false);
  return 0;
} // unsigned Rule::operator()(TypeUnsigned const& type) const

/**
 ** -> result
 **
 ** @param	type	rule type
 **
 ** @return	the value of 'type'
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **
 **/
unsigned
Rule::operator()(TypeUnsignedExtra const& type) const
{
  switch(type) {
  case Rule::NUMBER_OF_SAME_CARDS:
    return ((*this)(NUMBER_OF_PLAYERS_IN_GAME) / 2);
  case Rule::NUMBER_OF_CARD_COLORS:
    return Card::NUMBER_OF_COLORS;
  case Rule::NUMBER_OF_CARD_TCOLORS:
    return Card::NUMBER_OF_TCOLORS;
  case Rule::NUMBER_OF_CARD_VALUES:
    return Card::NUMBER_OF_VALUES + ((*this)(WITH_NINES) ? 0 : -1);
  case Rule::NUMBER_OF_CARDS:
    return ((*this)(NUMBER_OF_SAME_CARDS)
            * (*this)(NUMBER_OF_CARD_COLORS) * (*this)(NUMBER_OF_CARD_VALUES));
  case Rule::NUMBER_OF_TEAMS:
    return 2;
  case Rule::NUMBER_OF_PLAYERS_PER_TEAM:
    return (*this)(NUMBER_OF_PLAYERS_IN_GAME) / (*this)(NUMBER_OF_TEAMS);
  case Rule::NUMBER_OF_GAMES_PER_ROUND:
    return (*this)(NUMBER_OF_PLAYERS);
  case Rule::MAX_NUMBER_OF_TRICKS_IN_GAME:
    return 2 * Card::NUMBER_OF_VALUES * Card::NUMBER_OF_COLORS / 4;
  case Rule::NUMBER_OF_TRICKS_IN_GAME:
    return (*this)(NUMBER_OF_CARDS) / (*this)(NUMBER_OF_PLAYERS_IN_GAME);
  case Rule::POINTS_FOR_DOPPELKOPF:
    return 40;
  case Rule::MAX_NUMBER_OF_POVERTY_TRUMPS:
    return 3;
  } // switch(type)

  DEBUG_ASSERTION(false,
                  "Rule::operator()(" << type << ")\n"
                  "  type = " << type << " not in switch");
  return 0;
} // unsigned Rule::operator()(TypeUnsignedExtra const& type) const

/**
 ** -> result
 **
 ** @param	type	rule type
 **
 ** @return	the value of 'type'
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **
 **/
Counting
Rule::operator()(TypeCounting const& type) const
{
  return this->counting();
} // Counting Rule::operator()(TypeCounting const& type) const

/**
 ** -> result
 **
 ** @param	game_type	gametype
 **
 ** @return	whether the rule 'game_typ' is valid
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **
 **/
bool
Rule::operator()(GameType const& game_type) const
{
  switch(game_type) {
  case GAMETYPE::NORMAL:
    return true;
  case GAMETYPE::THROWN_NINES:
    return (*this)(THROW_WITH_NINES);
  case GAMETYPE::THROWN_KINGS:
    return (*this)(THROW_WITH_KINGS);
  case GAMETYPE::THROWN_NINES_AND_KINGS:
    return (*this)(THROW_WITH_NINES_AND_KINGS);
  case GAMETYPE::THROWN_RICHNESS:
    return (*this)(THROW_WITH_RICHNESS);
  case GAMETYPE::FOX_HIGHEST_TRUMP:
    return (*this)(THROW_WHEN_FOX_HIGHEST_TRUMP);
  case GAMETYPE::POVERTY:
    return (*this)(POVERTY);
  case GAMETYPE::GENSCHER:
    return (*this)(GENSCHER);
  case GAMETYPE::MARRIAGE:
  case GAMETYPE::MARRIAGE_SOLO:
  case GAMETYPE::MARRIAGE_SILENT:
    return true;
  case GAMETYPE::SOLO_JACK:
    return (*this)(SOLO_JACK);
  case GAMETYPE::SOLO_QUEEN:
    return (*this)(SOLO_QUEEN);
  case GAMETYPE::SOLO_KING:
    return (*this)(SOLO_KING);
  case GAMETYPE::SOLO_QUEEN_JACK:
    return (*this)(SOLO_QUEEN_JACK);
  case GAMETYPE::SOLO_KING_JACK:
    return (*this)(SOLO_KING_JACK);
  case GAMETYPE::SOLO_KING_QUEEN:
    return (*this)(SOLO_KING_QUEEN);
  case GAMETYPE::SOLO_KOEHLER:
    return (*this)(SOLO_KOEHLER);
  case GAMETYPE::SOLO_CLUB:
  case GAMETYPE::SOLO_SPADE:
  case GAMETYPE::SOLO_HEART:
  case GAMETYPE::SOLO_DIAMOND:
    return (*this)(SOLO_COLOR);
  case GAMETYPE::SOLO_MEATLESS:
    return (*this)(SOLO_MEATLESS);
  } // switch(game_type)

  DEBUG_ASSERT(false);
  return false;
} // bool Rule::operator()(GameType const& game_type) const

/**
 ** -> result
 **
 ** @param	marriage_selector
 **
 ** @return	whether the 'marriage_selector' is valid
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **/
bool
Rule::operator()(MarriageSelector const& marriage_selector) const
{
  switch(marriage_selector) {
  case MARRIAGE_SELECTOR::SILENT:
  case MARRIAGE_SELECTOR::TEAM_SET:
    return false;
  case MARRIAGE_SELECTOR::FIRST_FOREIGN:
    return (*this)(MARRIAGE_FIRST_FOREIGN);
  case MARRIAGE_SELECTOR::FIRST_TRUMP:
    return (*this)(MARRIAGE_FIRST_TRUMP);
  case MARRIAGE_SELECTOR::FIRST_COLOR:
    return (*this)(MARRIAGE_FIRST_COLOR);
  case MARRIAGE_SELECTOR::FIRST_CLUB:
  case MARRIAGE_SELECTOR::FIRST_SPADE:
  case MARRIAGE_SELECTOR::FIRST_HEART:
    return (*this)(MARRIAGE_FIRST_SINGLE_COLOR);
  } // switch(marriage_selector)

  DEBUG_ASSERT(false);
  return false;
} // bool Rule::operator()(MarriageSelector const& marriage_selector) const

/**
 ** -> result
 **
 ** @param	special_point	specialpoint
 **
 ** @return	whether the rule 'special_point' is valid
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **
 **/
bool
Rule::operator()(Specialpoint const& special_point) const
{
  switch(special_point) {
  case SPECIALPOINT::NOSPECIALPOINT:
    return true;
  case SPECIALPOINT::CAUGHT_FOX:
    return (*this)(EXTRAPOINT_CATCH_FOX);
  case SPECIALPOINT::CAUGHT_FOX_LAST_TRICK:
    return (*this)(EXTRAPOINT_CATCH_FOX_LAST_TRICK);
  case SPECIALPOINT::FOX_LAST_TRICK:
    return (*this)(EXTRAPOINT_FOX_LAST_TRICK);
  case SPECIALPOINT::CHARLIE:
    return (*this)(EXTRAPOINT_CHARLIE);
  case SPECIALPOINT::CAUGHT_CHARLIE:
    return (*this)(EXTRAPOINT_CATCH_CHARLIE);
  case SPECIALPOINT::DOLLE_CAUGHT_DOLLE:
    return (*this)(EXTRAPOINT_DOLLE_JABS_DOLLE);
  case SPECIALPOINT::HEART_TRICK:
    return (*this)(EXTRAPOINT_HEART_TRICK);
  case SPECIALPOINT::DOPPELKOPF:
    return true;

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
  case SPECIALPOINT::CONTRA_WON:
  case SPECIALPOINT::SOLO:
  case SPECIALPOINT::NO120_REPLY:
  case SPECIALPOINT::NO90_REPLY:
  case SPECIALPOINT::NO60_REPLY:
  case SPECIALPOINT::NO30_REPLY:
  case SPECIALPOINT::NO0_REPLY:
    return true;

  case SPECIALPOINT::BOCK:
    return (*this)(BOCK);
  } // switch(special_point)

  DEBUG_ASSERT(false);
  return false;
} // bool Rule::operator()(Specialpoint const& special_point) const

/**
 ** -> result
 **
 ** @param	announcement
 **
 ** @return	how many remaining cards there must be to make the announcement
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **
 **/
unsigned
Rule::remaining_cards(Announcement const& announcement) const
{
  switch(announcement) {
  case ANNOUNCEMENT::NOANNOUNCEMENT:
  case ANNOUNCEMENT::REPLY:
  case ANNOUNCEMENT::NO120_REPLY:
  case ANNOUNCEMENT::NO90_REPLY:
  case ANNOUNCEMENT::NO60_REPLY:
  case ANNOUNCEMENT::NO30_REPLY:
  case ANNOUNCEMENT::NO0_REPLY:
    return UINT_MAX;
  case ANNOUNCEMENT::NO120:
    return (*this)(Rule::ANNOUNCEMENT_NO_120);
  case ANNOUNCEMENT::NO90:
    return (*this)(Rule::ANNOUNCEMENT_NO_90);
  case ANNOUNCEMENT::NO60:
    return (*this)(Rule::ANNOUNCEMENT_NO_60);
  case ANNOUNCEMENT::NO30:
    return (*this)(Rule::ANNOUNCEMENT_NO_30);
  case ANNOUNCEMENT::NO0:
    return (*this)(Rule::ANNOUNCEMENT_NO_0);
  } // switch(announcement)

  DEBUG_ASSERT(false);
  return 0;
} // unsigned Rule::remaining_cards(Announcement const& announcement) const

/**
 ** -> result
 **
 ** @param	type	type
 **
 ** @return	whether the dependencies of 'type' are met
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.0
 **/
bool
Rule::dependencies(TypeBool const& type) const
{
  switch(type) {
  case MUTATE:
#ifdef RELEASE
    return false;
#else
    return true;
#endif
  case SHOW_IF_VALID:
  case SHOW_ALL_HANDS:
  case SHOW_TRICKS_IN_TRICKPILES:
  case SHOW_TRICKPILES_POINTS:
  case SHOW_KNOWN_TEAMS_IN_GAME:
  case SHOW_SOLOPLAYER_IN_GAME:
    return true;

  case CARD_HINT:
    return true;
  case TAKE_BACK_TRICKS:
    return true;

  case WITH_NINES:
    return true;

  case BOCK:
    return true;
  case BOCK_APPEND:
    return (*this)(BOCK);
  case BOCK_120:
    return (*this)(BOCK);
  case BOCK_SOLO_LOST:
    return (*this)(BOCK);
  case BOCK_RE_LOST:
    return (   (*this)(BOCK)
            && !(*this)(KNOCKING));
  case BOCK_HEART_TRICK:
    return (*this)(BOCK);
  case BOCK_BLACK:
    return (*this)(BOCK);

  case THROW_WITH_NINES:
    return (*this)(WITH_NINES);
  case THROW_WITH_KINGS:
    return true;
  case THROW_WITH_NINES_AND_KINGS:
    return (*this)(WITH_NINES);
  case THROW_WITH_RICHNESS:
    return true;

  case POVERTY:
    return true;
  case POVERTY_SHIFT:
    return (*this)(POVERTY);
  case POVERTY_SHIFT_ONLY_TRUMP:
  case POVERTY_FOX_SHIFT_OPEN:
  case THROW_WITH_ONE_TRUMP:
    return (*this)(POVERTY_SHIFT);

  case POVERTY_FOX_DO_NOT_COUNT:
    return (*this)(POVERTY);

  case POVERTY_FOX_SHIFT_EXTRA:
    return (   (*this)(POVERTY_FOX_DO_NOT_COUNT)
            && !(*this)(POVERTY_SHIFT_ONLY_TRUMP) );

  case THROW_WHEN_FOX_HIGHEST_TRUMP:
    return true;

  case MARRIAGE_FIRST_FOREIGN:
  case MARRIAGE_FIRST_COLOR:
  case MARRIAGE_FIRST_SINGLE_COLOR:
  case MARRIAGE_FIRST_TRUMP:
    return true;
  case MARRIAGE_FIRST_ONE_DECIDES:
    return true;
  case MARRIAGE_BEFORE_POVERTY:
    return (*this)(POVERTY);

  case GENSCHER:
    return !(!(*this)(WITH_NINES)
             && (*this)(HYPERSWINES));

  case SOLO:
    return true;
  case SOLO_JACK:
  case SOLO_QUEEN:
  case SOLO_KING:
  case SOLO_QUEEN_JACK:
  case SOLO_KING_JACK:
  case SOLO_KING_QUEEN:
  case SOLO_KOEHLER:
  case SOLO_COLOR:
  case SOLO_MEATLESS:
    return (*this)(SOLO);

  case THROWING_BEFORE_SOLO:
    return (   (*this)(SOLO)
            && (   (*this)(THROW_WITH_NINES)
                || (*this)(THROW_WITH_KINGS)
                || (*this)(THROW_WITH_NINES_AND_KINGS)
                || (*this)(THROW_WITH_RICHNESS)
                || (*this)(THROW_WITH_ONE_TRUMP)
                || (*this)(THROW_WHEN_FOX_HIGHEST_TRUMP)
                || (    (*this)(POVERTY)
                    && !(*this)(POVERTY_SHIFT) )
               ) );

  case DOLLEN:
    return true;
  case DOLLEN_SECOND_OVER_FIRST:
    return (*this)(DOLLEN);
  case DOLLEN_CONTRARY_IN_LAST_TRICK:
    return (*this)(DOLLEN);
  case DOLLEN_FIRST_OVER_SECOND_WITH_SWINES:
    return ((*this)(DOLLEN)
            && (*this)(SWINES)
            && ( (*this)(DOLLEN_SECOND_OVER_FIRST)
                || (*this)(DOLLEN_CONTRARY_IN_LAST_TRICK)));

  case SWINES:
    return true;
  case SWINES_ANNOUNCEMENT_BEGIN:
    return ((*this)(SWINES)
            && !(this->value(SWINE_ONLY_SECOND)));
  case SWINE_ONLY_SECOND:
    return ((*this)(SWINES)
            && !(this->value(SWINES_ANNOUNCEMENT_BEGIN))
            && !(this->value(HYPERSWINES)));
  case SWINES_IN_SOLO:
    return ((*this)(SWINES)
            && (*this)(SOLO_COLOR));
  case SWINES_IN_POVERTY:
    return ((*this)(SWINES)
            && (*this)(POVERTY_SHIFT));
  case HYPERSWINES:
    return ((*this)(SWINES)
            && !(*this)(SWINE_ONLY_SECOND));
#ifdef WORKAROUND
    // this activated leads to an infinte recursion
    // && !(!(*this)(WITH_NINES)
    // && (*this)(GENSCHER)));
#endif
  case HYPERSWINES_ANNOUNCEMENT_BEGIN:
    return ((*this)(HYPERSWINES)
            && (*this)(SWINES_ANNOUNCEMENT_BEGIN));
  case HYPERSWINES_IN_SOLO:
    return ((*this)(HYPERSWINES)
            && (*this)(SWINES_IN_SOLO));
  case HYPERSWINES_IN_POVERTY:
    return ((*this)(HYPERSWINES)
            && (*this)(SWINES_IN_POVERTY));
  case SWINES_AND_HYPERSWINES_JOINT:
    return (*this)(HYPERSWINES);

  case EXTRAPOINT_CATCH_FOX:
    return true;

  case EXTRAPOINT_CATCH_FOX_LAST_TRICK:
    return (*this)(EXTRAPOINT_CATCH_FOX);

  case EXTRAPOINT_FOX_LAST_TRICK:
    return true;

  case EXTRAPOINT_DOUBLE_FOX_LAST_TRICK:
    return (*this)(EXTRAPOINT_FOX_LAST_TRICK);

  case EXTRAPOINT_CHARLIE:
    return true;
  case EXTRAPOINT_CATCH_CHARLIE:
  case EXTRAPOINT_DOUBLE_CHARLIE:
    return (*this)(EXTRAPOINT_CHARLIE);
  case EXTRAPOINT_CATCH_DOUBLE_CHARLIE:
    return (*this)(EXTRAPOINT_DOUBLE_CHARLIE);
  case EXTRAPOINT_CATCH_CHARLIE_ONLY_WITH_DIAMOND_QUEEN:
    return (*this)(EXTRAPOINT_CATCH_CHARLIE);

  case EXTRAPOINT_DOLLE_JABS_DOLLE:
    return ((*this)(DOLLEN)
            && ( (*this)(DOLLEN_SECOND_OVER_FIRST)
                || (*this)(DOLLEN_CONTRARY_IN_LAST_TRICK) ));
  case EXTRAPOINT_HEART_TRICK:
    return ((*this)(DOLLEN)
            && !(*this)(WITH_NINES));

  case KNOCKING:
  case ANNOUNCEMENT_LIMIT_ONLY_FOR_CURRENT:
  case ANNOUNCEMENT_TILL_FULL_TRICK:
  case ANNOUNCEMENT_FIRST_TRICK_THIRTY_POINTS:
    return true;
  case ANNOUNCEMENT_REPLY:
    return !(*this)(KNOCKING);
  case ANNOUNCEMENT_STRICT_LIMIT:
    return true;

  case ANNOUNCEMENT_FIRST_TRICK_THIRTY_POINTS_ONLY_FIRST:
    return (*this)(ANNOUNCEMENT_FIRST_TRICK_THIRTY_POINTS);

  case ANNOUNCEMENT_FIRST_TRICK_THIRTY_POINTS_ONLY_RE_CONTRA:
    return (*this)(ANNOUNCEMENT_FIRST_TRICK_THIRTY_POINTS);

  case ANNOUNCEMENT_FIRST_TRICK_THIRTY_POINTS_IN_MARRIAGE:
    return (*this)(ANNOUNCEMENT_FIRST_TRICK_THIRTY_POINTS);

  case ANNOUNCEMENT_RE_DOUBLES:
    return true;

  case ANNOUNCEMENT_CONTRA_DOUBLES_AGAINST_RE:
    return (*this)(ANNOUNCEMENT_RE_DOUBLES);

  case NUMBER_OF_ROUNDS_LIMITED:
    return true;

  case POINTS_LIMITED:
    return ((*this)(COUNTING) != COUNTING_NS::PLUSMINUS);

  case OFFER_DUTY_SOLO:
    return (*this)(NUMBER_OF_DUTY_SOLI);

  case LUSTSOLO_PLAYER_LEADS:
    return (*this)(SOLO);

  case SOLO_ALWAYS_COUNTS_TRIPLE:
    return (   (*this)(SOLO)
            && ((*this)(COUNTING) != COUNTING_NS::PLUSMINUS) );
  } // switch(type)

  DEBUG_ASSERT(false);
  return false;
} // bool Rule::dependencies(TypeBool) const;

/**
 ** -> result
 **
 ** @param	type	type
 **
 ** @return	whether the dependencies of 'type' are met
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **
 **/
bool
Rule::dependencies(TypeUnsigned const& type) const
{
  switch(type) {
  case UNSUPPORTED:
    return true;

  case MIN_NUMBER_OF_THROWING_NINES:
    return (*this)(THROW_WITH_NINES);
  case MIN_NUMBER_OF_THROWING_KINGS:
    return (*this)(THROW_WITH_KINGS);
  case MIN_NUMBER_OF_THROWING_NINES_AND_KINGS:
    return (*this)(THROW_WITH_NINES_AND_KINGS);
  case MIN_RICHNESS_FOR_THROWING:
    return (*this)(THROW_WITH_RICHNESS);

  case BOCK_MULTIPLIER:
    return (*this)(BOCK);

  case SHOW_TRICKS_NUMBER:
  case MARRIAGE_DETERMINATION:
  case ANNOUNCEMENT_NO_120:
  case ANNOUNCEMENT_NO_90:
  case ANNOUNCEMENT_NO_60:
  case ANNOUNCEMENT_NO_30:
  case ANNOUNCEMENT_NO_0:
  case ANNOUNCEMENT_LAST:
    return true;

  case NUMBER_OF_ROUNDS:
    return (*this)(NUMBER_OF_ROUNDS_LIMITED);

  case POINTS:
    return (*this)(POINTS_LIMITED);

  case NUMBER_OF_DUTY_SOLI:
    return (   (*this)(SOLO)
            && (   (*this)(SOLO_JACK)
                || (*this)(SOLO_QUEEN)
                || (*this)(SOLO_KING)
                || (*this)(SOLO_QUEEN_JACK)
                || (*this)(SOLO_KING_JACK)
                || (*this)(SOLO_KING_QUEEN)
                || (*this)(SOLO_KOEHLER)
                || (*this)(SOLO_COLOR)
                || (*this)(SOLO_MEATLESS))
            && (   (*this)(NUMBER_OF_ROUNDS_LIMITED)
                || (*this)(POINTS_LIMITED)));

  case NUMBER_OF_DUTY_COLOR_SOLI:
    return (   ((*this)(NUMBER_OF_DUTY_SOLI) > 0)
            && (*this)(SOLO_COLOR));

  case NUMBER_OF_DUTY_PICTURE_SOLI:
    return (   ((*this)(NUMBER_OF_DUTY_SOLI) > 0)
            && (   (*this)(SOLO_JACK)
                || (*this)(SOLO_QUEEN)
                || (*this)(SOLO_KING)
                || (*this)(SOLO_QUEEN_JACK)
                || (*this)(SOLO_KING_JACK)
                || (*this)(SOLO_KING_QUEEN)
                || (*this)(SOLO_KOEHLER) ) );

  case NUMBER_OF_PLAYERS:
  case NUMBER_OF_PLAYERS_IN_GAME:
    return true;
  } // switch(type)

  DEBUG_ASSERT(false);
  return false;
} // bool Rule::dependencies(TypeUnsigned const& type) const

/**
 ** -> result
 **
 ** @param	type	type
 **
 ** @return	the value of 'type'
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **
 **/
bool
Rule::value(TypeBool const& type) const
{
  return this->bool_p[type - BOOL_FIRST];
} // bool Rule::value(TypeBool) const;

/**
 ** -> result
 **
 ** @param	type	type
 **
 ** @return	the value of 'type'
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **
 **/
unsigned
Rule::value(TypeUnsigned const& type) const
{
  return this->unsigned_p[type - UNSIGNED_FIRST];
} // unsigned Rule::value(TypeUnsigned const& type) const

/**
 ** -> result
 **
 ** @param	type	type
 **
 ** @return	the value of 'type'
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **
 **/
Counting
Rule::value(TypeCounting const& type) const
{
  return this->counting_;
} // Counting Rule::value(TypeCounting const& type) const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	the counting
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **
 **/
Counting const&
Rule::counting() const
{
  return this->counting_;
} // Counting const& Rule::counting() const

/**
 ** -> result
 **
 ** @param	type	type
 **
 ** @return	the minimal value for 'type'
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **/
unsigned
Rule::min(TypeUnsigned const& type) const
{
  switch(type) {
  case UNSUPPORTED:
    return 0;

  case BOCK_MULTIPLIER:
    return 2;

  case SHOW_TRICKS_NUMBER:
    return 1;

  case MIN_NUMBER_OF_THROWING_NINES:
  case MIN_NUMBER_OF_THROWING_KINGS:
    return ( ( ((*this)(NUMBER_OF_SAME_CARDS) * (*this)(NUMBER_OF_CARD_COLORS))
              / (*this)(NUMBER_OF_PLAYERS_IN_GAME))
            + 1);

  case MIN_NUMBER_OF_THROWING_NINES_AND_KINGS:
    return ( ( 2 * ((*this)(NUMBER_OF_SAME_CARDS)
                    * (*this)(NUMBER_OF_CARD_COLORS))
              / (*this)(NUMBER_OF_PLAYERS_IN_GAME))
            + 1);

  case MIN_RICHNESS_FOR_THROWING:
    return ( (  (*this)(NUMBER_OF_SAME_CARDS)
              * (*this)(NUMBER_OF_CARD_COLORS)
              * (  Card::ACE
                 + Card::TEN
                 + Card::KING
                 + Card::QUEEN
                 + Card::JACK
                 + Card::NINE)
             ) / (*this)(NUMBER_OF_PLAYERS_IN_GAME)
            + 1);


  case Rule::MARRIAGE_DETERMINATION:
    return 1;

  case Rule::ANNOUNCEMENT_NO_120:
  case Rule::ANNOUNCEMENT_NO_90:
  case Rule::ANNOUNCEMENT_NO_60:
  case Rule::ANNOUNCEMENT_NO_30:
  case Rule::ANNOUNCEMENT_NO_0:
  case Rule::ANNOUNCEMENT_LAST:
    return 1;

  case Rule::NUMBER_OF_ROUNDS:
    return 1;

  case Rule::POINTS:
    return 1;

  case Rule::NUMBER_OF_DUTY_SOLI:
    return ((*this)(NUMBER_OF_DUTY_COLOR_SOLI)
            + (*this)(NUMBER_OF_DUTY_PICTURE_SOLI));

  case Rule::NUMBER_OF_DUTY_COLOR_SOLI:
    return 0;

  case Rule::NUMBER_OF_DUTY_PICTURE_SOLI:
    return 0;

  case Rule::NUMBER_OF_PLAYERS:
    return 4;

  case Rule::NUMBER_OF_PLAYERS_IN_GAME:
    return 4;
  } // switch(type)

  DEBUG_ASSERT(false);
  return 0;
} // unsigned Rule::min(TypeUnsigned const& type) const

/**
 ** -> result
 **
 ** @param	type	type
 **
 ** @return	the maximal value for 'type'
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **
 **/
unsigned
Rule::max(TypeUnsigned const& type) const
{
  switch(type) {
  case UNSUPPORTED:
    return UINT_MAX - 1;

  case BOCK_MULTIPLIER:
    return 256;

  case SHOW_TRICKS_NUMBER:
    return (*this)(MAX_NUMBER_OF_TRICKS_IN_GAME);

  case MIN_NUMBER_OF_THROWING_NINES:
  case MIN_NUMBER_OF_THROWING_KINGS:
    return ((*this)(NUMBER_OF_SAME_CARDS) * (*this)(NUMBER_OF_CARD_COLORS));

  case MIN_NUMBER_OF_THROWING_NINES_AND_KINGS:
    return 2 * ((*this)(NUMBER_OF_SAME_CARDS) * (*this)(NUMBER_OF_CARD_COLORS));

  case MIN_RICHNESS_FOR_THROWING:
    return (8 * Card::ACE + 4 * Card::TEN);

  case Rule::MARRIAGE_DETERMINATION:
    return (*this)(MAX_NUMBER_OF_TRICKS_IN_GAME);

  case Rule::ANNOUNCEMENT_NO_120:
  case Rule::ANNOUNCEMENT_NO_90:
  case Rule::ANNOUNCEMENT_NO_60:
  case Rule::ANNOUNCEMENT_NO_30:
  case Rule::ANNOUNCEMENT_NO_0:
  case Rule::ANNOUNCEMENT_LAST:
    return (*this)(MAX_NUMBER_OF_TRICKS_IN_GAME);

  case Rule::NUMBER_OF_ROUNDS:
    return UINT_MAX - 1;

  case Rule::POINTS:
    return UINT_MAX - 1;

  case Rule::NUMBER_OF_DUTY_SOLI:
    return ((*this)(NUMBER_OF_ROUNDS)
            * (*this)(NUMBER_OF_PLAYERS));

  case Rule::NUMBER_OF_DUTY_COLOR_SOLI:
    return ((*this)(NUMBER_OF_DUTY_SOLI)
            - (*this)(NUMBER_OF_DUTY_PICTURE_SOLI));

  case Rule::NUMBER_OF_DUTY_PICTURE_SOLI:
    return ((*this)(NUMBER_OF_DUTY_SOLI)
            - (*this)(NUMBER_OF_DUTY_COLOR_SOLI));

  case Rule::NUMBER_OF_PLAYERS:
    return 4;

  case Rule::NUMBER_OF_PLAYERS_IN_GAME:
    return (((*this)(NUMBER_OF_PLAYERS) / 2) * 2);
  } // switch(type)

  DEBUG_ASSERT(false);
  return 0;
} // unsigned Rule::max(TypeUnsigned const& type) const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	all card colors
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.6
 **
 **/
vector<Card::Color> const&
Rule::card_colors() const
{
  static vector<Card::Color> colors;
  if (colors.empty()) {
    colors.push_back(Card::CLUB);
    colors.push_back(Card::SPADE);
    colors.push_back(Card::HEART);
    colors.push_back(Card::DIAMOND);
  } // if (colors.empty())

  DEBUG_ASSERTION((colors.size() == (*this)(NUMBER_OF_CARD_COLORS)),
                  "Rule::card_colors():\n"
                  "  Number of card colors is not valid");

  return colors;
} // vector<Card::Color> const& Rule::card_colors() const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	all card values (depending on rule 'with nines')
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.6
 **
 **/
vector<Card::Value> const&
Rule::card_values() const
{
  static vector<Card::Value> values_without_nines;
  static vector<Card::Value> values_with_nines;

  if (values_without_nines.empty()) {
    values_without_nines.push_back(Card::ACE);
    values_without_nines.push_back(Card::KING);
    values_without_nines.push_back(Card::QUEEN);
    values_without_nines.push_back(Card::JACK);
    values_without_nines.push_back(Card::TEN);
  } // if (values_without_nines.empty())
  if (values_with_nines.empty()) {
    values_with_nines = values_without_nines;
    values_with_nines.push_back(Card::NINE);
  } // if (values_with_nines.empty())

  vector<Card::Value> const& values
    = ( (*this)(WITH_NINES)
       ? values_with_nines
       : values_without_nines );

  DEBUG_ASSERTION((values.size() == (*this)(NUMBER_OF_CARD_VALUES)),
                  "Rule::card_values():\n"
                  "  Number of card values is not valid");

  return values;
} // vector<Card::Value> const& Rule::card_values() const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	all cards (depending on rule 'with nines')
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.6
 **
 **/
vector<Card> const&
Rule::cards() const
{
  static vector<Card> cards_without_nines;
  static vector<Card> cards_with_nines;

  if (cards_without_nines.empty()) {
    vector<Card::Color> colors;
    colors.push_back(Card::CLUB);
    colors.push_back(Card::SPADE);
    colors.push_back(Card::HEART);
    colors.push_back(Card::DIAMOND);

    vector<Card::Value> values;
    values.push_back(Card::ACE);
    values.push_back(Card::KING);
    values.push_back(Card::QUEEN);
    values.push_back(Card::JACK);
    values.push_back(Card::TEN);

    for (vector<Card::Color>::const_iterator c = colors.begin();
         c != colors.end();
         ++c)
      for (vector<Card::Value>::const_iterator v = values.begin();
           v != values.end();
           ++v)
        cards_without_nines.push_back(Card(*c, *v));
  } // if (cards_without_nines.empty())
  if (cards_with_nines.empty()) {
    cards_with_nines = cards_without_nines;

    vector<Card::Color> colors;
    colors.push_back(Card::CLUB);
    colors.push_back(Card::SPADE);
    colors.push_back(Card::HEART);
    colors.push_back(Card::DIAMOND);

    for (vector<Card::Color>::const_iterator c = colors.begin();
         c != colors.end();
         ++c)
      cards_with_nines.push_back(Card(*c, Card::NINE));
  } // if (cards_with_nines.empty())

  vector<Card> const& cards
    = ( (*this)(WITH_NINES)
       ? cards_with_nines
       : cards_without_nines );

  DEBUG_ASSERTION((cards.size()
                   == ( (*this)(NUMBER_OF_CARD_COLORS)
                       * (*this)(NUMBER_OF_CARD_VALUES) ) ),
                  "Rule::cards():\n"
                  "  Number of cards is not valid:\n"
                  "  'cards.size() = " << cards.size()
                  << " != "
                  <<  (*this)(NUMBER_OF_CARD_COLORS)
                  << " * " << (*this)(NUMBER_OF_CARD_VALUES) );

  return cards;
} // vector<Card> const& Rule::cards() const

/**
 **
 ** sets the rule 'type' to the value 'value'
 **
 ** @param	type	type
 ** @param	value	value
 **
 ** @return	-
 **
 ** @version	0.6.7
 **
 ** @author	Diether Knof
 **
 **/
void
Rule::set(TypeBool type, bool value)
{
  if (this->value(type) == value)
    return ;

#ifdef OUTDATED
  // loading in a game leads now to problems
#ifdef RELEASE
  DEBUG_ASSERTION((::game_status < GAMESTATUS::PARTY_PLAY),
                  "Rule::set(TypeBool, value):\n"
                  "  'game_status' is not valid: " << ::game_status);
#endif
#endif

  bool const old_value = this->bool_p[type - BOOL_FIRST];
  this->bool_p[type - BOOL_FIRST] = value;

  switch (type) {
  case WITH_NINES:
#ifdef OUTDATED
    // DK 2005-02-13
    // I think this is a bad idea:
    // when changing 'with nines' -> 'without nines' -> 'with nines'
    // it should keep '12'

    // if all tricks could be shown set the number to the highest value
    // (normal game: 10 -> 12)
    if ((*this)(NUMBER_OF_PLAYERS_IN_GAME))
      if (this->value(SHOW_TRICKS_NUMBER)
          == ((*this)(NUMBER_OF_TRICKS_IN_GAME)
              - (((*this)(NUMBER_OF_CARD_COLORS)
                  * (*this)(NUMBER_OF_SAME_CARDS))
                 / (*this)(NUMBER_OF_PLAYERS_IN_GAME)))) {
        type = SHOW_TRICKS_NUMBER;
        value = (*this)(NUMBER_OF_TRICKS_IN_GAME);
        this->set(type, value);
      }
#endif
    break;
  case MARRIAGE_FIRST_FOREIGN:
  case MARRIAGE_FIRST_COLOR:
  case MARRIAGE_FIRST_SINGLE_COLOR:
  case MARRIAGE_FIRST_TRUMP:
    // if none is set, set 'first color'
    if (   !this->value(MARRIAGE_FIRST_FOREIGN)
        && !this->value(MARRIAGE_FIRST_COLOR)
        && !this->value(MARRIAGE_FIRST_SINGLE_COLOR)
        && !this->value(MARRIAGE_FIRST_TRUMP)) {
        type = MARRIAGE_FIRST_COLOR;
        value = true;
        this->set(type, value);
    }
    break;
  default:
    break;
  } // switch(type)
  if (this == &::party.rule()) {
    ::party.rule_changed(type, &old_value);
    if (::ui != NULL) {
      ::ui->rule_changed(type, &old_value);
      // the set-function has to be called again with the same values, 
      // because else the ui could change the values
      this->set(type, value);
    } // if (::ui != NULL)
  } // if (this == &::party.rule())

  return ;
} // void Rule::set(TypeBool, bool);

/**
 **
 ** sets the rule 'type' to the value 'value'
 **
 ** @param	type	type
 ** @param	value	value in text form
 **
 ** @return	-
 **
 ** @version	0.6.7
 **
 ** @author	Diether Knof
 **
 **/
void
Rule::set(TypeBool const& type, string const& value)
{
  if (   (value != "true")
      && (value != "false")
      && (value != "yes")
      && (value != "no")
      && (value != "0")
      && (value != "1"))
    cerr << "illegal value '" << value << "' for '" << type << "', "
      << "must be a boolean ('true' or 'false' or 'yes' or 'no' or '1' or '0'. "
      << "Ignoring"
      << endl;
  else
    this->set(type, (((value == "false")
                      || (value == "no")
                      || (value == "0"))
                     ? false
                     : true));

  return ;
} // void Rule::set(TypeBool const& type, string const& value)

/**
 **
 ** sets the rule 'type' to the value 'value'
 **
 ** @param	type	type
 ** @param	value	value
 **
 ** @return	-
 **
 ** @version	0.6.7
 **
 ** @author	Diether Knof
 **
 **/
void
Rule::set(TypeUnsigned const& type, unsigned const& value)
{
  if (this->value(type) == value)
    return ;

#ifdef OUTDATED
  // loading in a game leads now to problems
#ifdef RELEASE
  DEBUG_ASSERTION((::game_status < GAMESTATUS::PARTY_PLAY),
                  "Rule::set(TypeUnsigned, value):\n"
                  "  'game_status' is not valid: " << ::game_status);
#endif
#endif

  unsigned const old_value = this->unsigned_p[type - UNSIGNED_FIRST];
  this->unsigned_p[type - UNSIGNED_FIRST] = value;

  switch(type) {
  case ANNOUNCEMENT_NO_120:
    if (value < this->value(ANNOUNCEMENT_NO_90))
      this->set(ANNOUNCEMENT_NO_90, value);
    break;
  case ANNOUNCEMENT_NO_90:
    if (value > this->value(ANNOUNCEMENT_NO_120))
      this->set(ANNOUNCEMENT_NO_120, value);
    if (value < this->value(ANNOUNCEMENT_NO_60))
      this->set(ANNOUNCEMENT_NO_60, value);
    break;
  case ANNOUNCEMENT_NO_60:
    if (value > this->value(ANNOUNCEMENT_NO_90))
      this->set(ANNOUNCEMENT_NO_90, value);
    if (value < this->value(ANNOUNCEMENT_NO_30))
      this->set(ANNOUNCEMENT_NO_30, value);
    break;
  case ANNOUNCEMENT_NO_30:
    if (value > this->value(ANNOUNCEMENT_NO_60))
      this->set(ANNOUNCEMENT_NO_60, value);
    if (value < this->value(ANNOUNCEMENT_NO_0))
      this->set(ANNOUNCEMENT_NO_0, value);
    break;
  case ANNOUNCEMENT_NO_0:
    if (value > this->value(ANNOUNCEMENT_NO_30))
      this->set(ANNOUNCEMENT_NO_30, value);
    if (value < this->value(ANNOUNCEMENT_LAST))
      this->set(ANNOUNCEMENT_LAST, value);
    break;
  case ANNOUNCEMENT_LAST:
    if (value > this->value(ANNOUNCEMENT_NO_0))
      this->set(ANNOUNCEMENT_NO_0, value);
    break;
  default:
    break;
  } // switch(type)

  if (this == &::party.rule()) {
    ::party.rule_changed(type, &old_value);
    if (ui != NULL) {
      ::ui->rule_changed(type, &old_value);
      // the set-function has to be called again with the same values, 
      // because else the ui could change the values
      this->set(type, value);
    } // if (ui != NULL)
  } // if (this == &::party.rule())

  return ;
} // void Rule::set(TypeUnsigned const& type, unsigned const& value)

/**
 **
 ** sets the rule 'type' to the value 'value'
 **
 ** @param	type	type
 ** @param	value	value in text form
 **
 ** @return	-
 **
 ** @version	0.6.7
 **
 ** @author	Diether Knof
 **
 **/
void
Rule::set(TypeUnsigned const& type, string const& value)
{
  char* end_ptr;
  unsigned number = strtoul(value.c_str(), &end_ptr, 0);
  if (*end_ptr != '\0')
    cerr << "illegal value '" << value << "' for '" << type << "', "
      << "must be a digit.\n"
      << "Ignoring"
      << endl;
  else
    this->set(type, number);

  return ;
} // void Rule::set(const TypeUnsigned type, string const& value)

/**
 **
 ** sets the rule 'type' to the value 'value'
 **
 ** @param	type	type
 ** @param	value	counting
 **
 ** @return	-
 **
 ** @version	0.6.7
 **
 ** @author	Diether Knof
 **
 **/
void
Rule::set(TypeCounting const& type, Counting const& value)
{
  if (this->counting_ == value)
    return ;

  Counting const old_value = this->counting();

  this->counting_ = value;

  if (this == &::party.rule()) {
    ::party.rule_changed(type, &old_value);
    if (::ui != NULL) {
      ::ui->rule_changed(type, &old_value);
      // the set-function has to be called again with the same values, 
      // because else the ui could change the values
      this->set(type, value);
    } // if (::ui != NULL)
  } // if (this == &::party.rule())

  return ;
} // void Rule::set(const TypeCounting type, Counting const& value)

/**
 **
 ** sets the rule 'type' to the value 'value'
 **
 ** @param	type	type
 ** @param	value	value in text form
 **
 ** @return	-
 **
 ** @version	0.6.7
 **
 ** @author	Diether Knof
 **
 **/
void
Rule::set(TypeCounting const& type, string const& value)
{
  if (value == ::name(COUNTING_NS::PLUS))
    this->set(type, COUNTING_NS::PLUS);
  else if (value == ::name(COUNTING_NS::MINUS))
    this->set(type, COUNTING_NS::MINUS);
  else if (value == ::name(COUNTING_NS::PLUSMINUS))
    this->set(type, COUNTING_NS::PLUSMINUS);
#ifdef WORKAROUND
  // 0.7.0 name changed
  else if (value == ::name_old(COUNTING_NS::PLUS))
    this->set(type, COUNTING_NS::PLUS);
  else if (value == ::name_old(COUNTING_NS::MINUS))
    this->set(type, COUNTING_NS::MINUS);
  else if (value == ::name_old(COUNTING_NS::PLUSMINUS))
    this->set(type, COUNTING_NS::PLUSMINUS);
#endif
  else
    cerr << "illegal value '" << value << "' for '" << type << "', "
      << "must be "
      << "'" << ::name(COUNTING_NS::PLUS) << "', "
      << "'" << ::name(COUNTING_NS::MINUS) << "' or "
      << "'" << ::name(COUNTING_NS::PLUSMINUS) << ".\n"
      << "Ignoring"
      << endl;

  return ;
} // void Rule::set(const TypeCounting type, string const& value)

/**
 ** sets the rule 'type' to the value 'value'
 **
 ** @param	type	name of the type
 ** @param	value	value in text form
 **
 ** @return	whether the type was found
 **
 ** @version	0.7.3
 **
 ** @author	Diether Knof
 **/
bool
Rule::set(string const& type, string const& value)
{
  unsigned i;

  for (i = Rule::BOOL_FIRST; i <= Rule::BOOL_LAST; i++) {
    if (type == name(Rule::TypeBool(i))) {
      this->set(Rule::TypeBool(i), value);
      return true;
    }
  }
  for (i = Rule::UNSIGNED_FIRST; i <= Rule::UNSIGNED_LAST; i++) {
    if (type == name(Rule::TypeUnsigned(i))) {
      this->set(Rule::TypeUnsigned(i), value);
      return true;
    }
  }

  if (type == name(COUNTING)) {
    this->set(Rule::COUNTING, value);
    return true;
  }

#ifndef OUTDATED
  // 0.7.2b -> 0.7.3
  if (type == "throw with five nines") {
    this->set(Rule::THROW_WITH_NINES, value);
    this->set(Rule::MIN_NUMBER_OF_THROWING_NINES, 5);
    return true;
  }
  if (type == "throw with six kings") {
    this->set(Rule::THROW_WITH_KINGS, value);
    this->set(Rule::MIN_NUMBER_OF_THROWING_KINGS, 6);
    return true;
  }
#endif

  return false;
} // bool Rule::set(string const& type, string const& value)

/**
 ** mutate the rule
 ** change one rule (bool: flip; unsigned: increase/decrease)
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
Rule::mutate()
{
  int const r = rand() % (UNSIGNED_LAST + 1);

  // do not mutate all rules
  if (   (r == NUMBER_OF_PLAYERS)
      || (r == NUMBER_OF_PLAYERS_IN_GAME)
      || (r == NUMBER_OF_ROUNDS_LIMITED)
      || (r == LUSTSOLO_PLAYER_LEADS)
      || (r == POINTS_LIMITED)
      || (r == MUTATE)
      || (   (r >= SOLO)
          && (r <= SOLO_MEATLESS))
     )
    return ;

  if (   (r >= BOOL_FIRST)
      && (r <= BOOL_LAST) ) {
    TypeBool const type = static_cast<TypeBool>(r);
    cout << "mutate " << type
      << ": " << this->value(type)
      << " (" << (*this)(type) << ")";
    this->set(type, !this->value(type));
    cout << " → " << this->value(type)
      << " (" << (*this)(type) << ")"
      << endl;
  } else if (   (r >= UNSIGNED_FIRST)
             && (r <= UNSIGNED_LAST) ) {
    TypeUnsigned const type = static_cast<TypeUnsigned>(r);
    cout << "mutate " << type
      << ": " << this->value(type)
      << " (" << (*this)(type) << ")";
    unsigned value = this->value(type);
    if (value != UINT_MAX) {
      if (rand() % 2 == 0) {
        if (value > this->min(type))
          value -= 1;
      } else {
        if (value < this->max(type))
          value += 1;
      }
    }
    this->set(type, value);
    cout << " → " << this->value(type)
      << " (" << (*this)(type) << ")"
      << endl;
  } else {
    cout << "mutate " << r << endl;
  }

  return ;
} // void Rule::mutate()

/**
 **
 ** load the rules from the given file
 ** (or the file in the rules directory)
 **
 ** @param	file	file to load the rules from
 **
 ** @return	whether the rules could be loaded
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
bool
Rule::load(string const& filename)
{
  {
    ifstream istr(filename.c_str());
    if (istr.good())
      return this->read(istr);
  }
  {
    ifstream istr((::setting(Setting::PRIVATE_DATA_DIRECTORY)
                   + "/" + ::setting(Setting::RULES_DIRECTORY)
                   + "/" + filename).c_str());
    if (istr.good())
      return this->read(istr);
  }
  {
    ifstream istr((::setting(Setting::PUBLIC_DATA_DIRECTORY)
                   + "/" + ::setting(Setting::RULES_DIRECTORY)
                   + "/" + filename).c_str());
    if (istr.good())
      return this->read(istr);
  }

  return false;
} // bool Rule::load(string filename)

/**
 ** load the rules from the given stream
 **
 ** @param	istr	stream to load the rules from
 **
 ** @return	whether the rules could be loaded
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **/
bool
Rule::read(istream& istr)
{
  this->reset_to_hardcoded();

  unsigned depth = 0;

  // load the rules
  while (istr.good()) {
    Config config;
    istr >> config;

    // finished with the config file
    if ((config.name == "") && (config.value == ""))
      break;

    if (config.separator) {
      // a setting
      if (!this->set(config.name, config.value)) {
        cerr << "rule file:	"
          << "ignoring unknown rule '" << config.name << "'."
          << endl;
      }
    } else { // if (config.separator)
      // a setting
      // if the value is in parentencies, remove both
      if(config.name == "!end") {
        // ignore the rest of the file
        break;
      } else if(config.name == "!stdout") {
        // output of the data to 'stdout'
        cout << config.value << endl;
      } else if(config.name == "!stderr") {
        // output of the data to 'stderr'
        cerr << config.value << endl;
      } else if(config.name == "{") {
        depth += 1;
      } else if(config.name == "}") {
        if (depth == 0) {
          cerr << "Rulefile: found a '}' without a '{' before.\n"
            << "Finish reading the the file."
            << endl;
          break;
        } // if (depth == 0)
        depth -= 1;
        if (depth == 0)
          break;
      } else if(config.name == "") {
        cerr << "Rule file: "
          << "Ignoring line \'" << config.value << "\'.\n";
      } else {
        cerr << "Rule file: "
          << "Rule '" << config.name << "' unknown.\n"
          << "Ignoring it.\n";
      } // if (config.name == .)
    } // config.separator
  } // while (istr.good())

  return true;
} // bool Rule::read(istream& istr)

/**
 ** saves the rules in the given file
 **
 ** @param	filename   file to save the rules into
 **
 ** @return	whether the rules could be saved
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
bool
Rule::save(string const& filename) const
{
  string filename_tmp = filename + ".tmp";
  ofstream ostr(filename_tmp.c_str());
  if (!ostr.good()) {
    ::ui->information(::translation("Error::Rule::save: Error opening temporary file %stmpfile%. Aborting saving.", filename_tmp), INFORMATION::PROBLEM);
    return false;
  }

  ostr << "# FreeDoko rules (version " << *::version << ")\n"
    << '\n';
  this->write(ostr);

  if (!ostr.good()) {
    ::ui->information(::translation("Error::Rule::save: Error saving in temporary file %stmpfile%. Keeping temporary file (for bug tracking).", filename_tmp), INFORMATION::PROBLEM);
    return false;
  }
  ostr.close();

#ifdef WINDOWS
  unlink(filename.c_str());
#endif
  if (rename(filename_tmp.c_str(), filename.c_str())) {
    ::ui->information(::translation("Error::Rule::save: Could not rename temporary file %stmpfile% to requested file %sfile%. Keeping temporary file.", filename_tmp, filename), INFORMATION::PROBLEM);
    return false;
  }

  return true;
} // bool Rule::save(string const& file) const

/**
 ** puts the rules in the ouput stream
 **
 ** @param	ostr	output stream
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **/
void
Rule::write(ostream& ostr) const
{
  std::ios_base::fmtflags const flags = ostr.flags();
  ostr << std::boolalpha;

  if (this->value(MUTATE))
    ostr << MUTATE << " = "
      << this->value(MUTATE) << '\n'
      << '\n';

  ostr << NUMBER_OF_ROUNDS_LIMITED << " = "
    << this->value(NUMBER_OF_ROUNDS_LIMITED) << '\n'
    << "  "
    << NUMBER_OF_ROUNDS << " = "
    << this->value(NUMBER_OF_ROUNDS) << '\n'

    << POINTS_LIMITED << " = "
    << this->value(POINTS_LIMITED) << '\n'
    << "  "
    << POINTS << " = "
    << this->value(POINTS) << '\n'

    << NUMBER_OF_DUTY_SOLI << " = "
    << this->value(NUMBER_OF_DUTY_SOLI) << '\n'
    << "  "
    << NUMBER_OF_DUTY_COLOR_SOLI << " = "
    << this->value(NUMBER_OF_DUTY_COLOR_SOLI) << '\n'
    << "  "
    << NUMBER_OF_DUTY_PICTURE_SOLI << " = "
    << this->value(NUMBER_OF_DUTY_PICTURE_SOLI) << '\n'
    << OFFER_DUTY_SOLO << " = "
    << this->value(OFFER_DUTY_SOLO) << '\n'
    << LUSTSOLO_PLAYER_LEADS << " = "
    << this->value(LUSTSOLO_PLAYER_LEADS) << '\n'
    << SOLO_ALWAYS_COUNTS_TRIPLE << " = "
    << this->value(SOLO_ALWAYS_COUNTS_TRIPLE) << '\n'

    // *** << PARTY_POINTS_SUMMING << " = "
    // *** << this->value(PARTY_POINTS_SUMMING) << '\n'

    << NUMBER_OF_PLAYERS << " = "
    << this->value(NUMBER_OF_PLAYERS) << '\n'

    << NUMBER_OF_PLAYERS_IN_GAME << " = "
    << this->value(NUMBER_OF_PLAYERS_IN_GAME) << '\n'

    << '\n'

    << SHOW_IF_VALID << " = "
    << this->value(SHOW_IF_VALID) << '\n'

    << SHOW_ALL_HANDS << " = "
    << this->value(SHOW_ALL_HANDS) << '\n'

    << SHOW_TRICKS_NUMBER << " = "
    << this->value(SHOW_TRICKS_NUMBER) << '\n'

    << SHOW_TRICKS_IN_TRICKPILES << " = "
    << this->value(SHOW_TRICKS_IN_TRICKPILES) << '\n'

    << SHOW_TRICKPILES_POINTS << " = "
    << this->value(SHOW_TRICKPILES_POINTS) << '\n'

    << SHOW_KNOWN_TEAMS_IN_GAME << " = "
    << this->value(SHOW_KNOWN_TEAMS_IN_GAME) << '\n'

    << SHOW_SOLOPLAYER_IN_GAME << " = "
    << this->value(SHOW_SOLOPLAYER_IN_GAME) << '\n'

    << '\n'

    << CARD_HINT << " = "
    << this->value(CARD_HINT) << '\n'
    << TAKE_BACK_TRICKS << " = "
    << this->value(TAKE_BACK_TRICKS) << '\n'

    << '\n'

    << WITH_NINES << " = "
    << this->value(WITH_NINES) << '\n'

    << '\n'

    << BOCK << " = "
    << this->value(BOCK) << '\n'
    << "  "
    << BOCK_MULTIPLIER << " = "
    << this->value(BOCK_MULTIPLIER) << '\n'
    << "  "
    << BOCK_APPEND << " = "
    << this->value(BOCK_APPEND) << '\n'
    << "  "
    << BOCK_120 << " = "
    << this->value(BOCK_120) << '\n'
    << "  "
    << BOCK_SOLO_LOST << " = "
    << this->value(BOCK_SOLO_LOST) << '\n'
    << "  "
    << BOCK_RE_LOST << " = "
    << this->value(BOCK_RE_LOST) << '\n'
    << "  "
    << BOCK_HEART_TRICK << " = "
    << this->value(BOCK_HEART_TRICK) << '\n'
    << "  "
    << BOCK_BLACK << " = "
    << this->value(BOCK_BLACK) << '\n'

    << '\n'


    << THROW_WITH_NINES << " = "
    << this->value(THROW_WITH_NINES) << '\n'

    << MIN_NUMBER_OF_THROWING_NINES << " = "
    << this->value(MIN_NUMBER_OF_THROWING_NINES) << '\n'

    << THROW_WITH_KINGS << " = "
    << this->value(THROW_WITH_KINGS) << '\n'

    << MIN_NUMBER_OF_THROWING_KINGS << " = "
    << this->value(MIN_NUMBER_OF_THROWING_KINGS) << '\n'

    << THROW_WITH_NINES_AND_KINGS << " = "
    << this->value(THROW_WITH_NINES_AND_KINGS) << '\n'

    << MIN_NUMBER_OF_THROWING_NINES_AND_KINGS << " = "
    << this->value(MIN_NUMBER_OF_THROWING_NINES_AND_KINGS) << '\n'

    << THROW_WITH_RICHNESS << " = "
    << this->value(THROW_WITH_RICHNESS) << '\n'

    << MIN_RICHNESS_FOR_THROWING << " = "
    << this->value(MIN_RICHNESS_FOR_THROWING) << '\n'

    << '\n'

    << POVERTY << " = "
    << this->value(POVERTY) << '\n'
    << "  "
    << POVERTY_SHIFT << " = "
    << this->value(POVERTY_SHIFT) << '\n'
    << "  "
    << POVERTY_SHIFT_ONLY_TRUMP << " = "
    << this->value(POVERTY_SHIFT_ONLY_TRUMP) << '\n'
    << "  "
    << POVERTY_FOX_DO_NOT_COUNT << " = "
    << this->value(POVERTY_FOX_DO_NOT_COUNT) << '\n'
    << "  "
    << POVERTY_FOX_SHIFT_EXTRA << " = "
    << this->value(POVERTY_FOX_SHIFT_EXTRA) << '\n'
    << "  "
    << POVERTY_FOX_SHIFT_OPEN << " = "
    << this->value(POVERTY_FOX_SHIFT_OPEN) << '\n'
    << "  "
    << THROW_WITH_ONE_TRUMP << " = "
    << this->value(THROW_WITH_ONE_TRUMP) << '\n'
    << "  "
    << THROW_WHEN_FOX_HIGHEST_TRUMP << " = "
    << this->value(THROW_WHEN_FOX_HIGHEST_TRUMP) << '\n'

    << '\n'

    << MARRIAGE_DETERMINATION << " = "
    << this->value(MARRIAGE_DETERMINATION) << '\n'
    << "  "
    << MARRIAGE_FIRST_FOREIGN << " = "
    << this->value(MARRIAGE_FIRST_FOREIGN) << '\n'
    << "  "
    << MARRIAGE_FIRST_COLOR << " = "
    << this->value(MARRIAGE_FIRST_COLOR) << '\n'
    << "  "
    << MARRIAGE_FIRST_SINGLE_COLOR << " = "
    << this->value(MARRIAGE_FIRST_SINGLE_COLOR) << '\n'
    << "  "
    << MARRIAGE_FIRST_TRUMP << " = "
    << this->value(MARRIAGE_FIRST_TRUMP) << '\n'
    << "  "
    << MARRIAGE_FIRST_ONE_DECIDES << " = "
    << this->value(MARRIAGE_FIRST_ONE_DECIDES) << '\n'
    << "  "
    << MARRIAGE_BEFORE_POVERTY << " = "
    << this->value(MARRIAGE_BEFORE_POVERTY) << '\n'

    << '\n'

    << GENSCHER << " = "
    << this->value(GENSCHER) << '\n'

    << '\n'

    << SOLO << " = "
    << this->value(SOLO) << '\n'
    << "  "
    << SOLO_JACK << " = "
    << this->value(SOLO_JACK) << '\n'
    << "  "
    << SOLO_QUEEN << " = "
    << this->value(SOLO_QUEEN) << '\n'
    << "  "
    << SOLO_KING << " = "
    << this->value(SOLO_KING) << '\n'
    << "  "
    << SOLO_QUEEN_JACK << " = "
    << this->value(SOLO_QUEEN_JACK) << '\n'
    << "  "
    << SOLO_KING_JACK << " = "
    << this->value(SOLO_KING_JACK) << '\n'
    << "  "
    << SOLO_KING_QUEEN << " = "
    << this->value(SOLO_KING_QUEEN) << '\n'
    << "  "
    << SOLO_KOEHLER << " = "
    << this->value(SOLO_KOEHLER) << '\n'
    << "  "
    << SOLO_COLOR << " = "
    << this->value(SOLO_COLOR) << '\n'
    << "  "
    << SOLO_MEATLESS << " = "
    << this->value(SOLO_MEATLESS) << '\n'
    << "  "
    << THROWING_BEFORE_SOLO << " = "
    << this->value(THROWING_BEFORE_SOLO) << '\n'

    << '\n'

    << DOLLEN << " = "
    << this->value(DOLLEN) << '\n'
    << "  "
    << DOLLEN_SECOND_OVER_FIRST << " = "
    << this->value(DOLLEN_SECOND_OVER_FIRST) << '\n'
    << "  "
    << DOLLEN_CONTRARY_IN_LAST_TRICK << " = "
    << this->value(DOLLEN_CONTRARY_IN_LAST_TRICK) << '\n'
    << "  "
    << DOLLEN_FIRST_OVER_SECOND_WITH_SWINES << " = "
    << this->value(DOLLEN_FIRST_OVER_SECOND_WITH_SWINES) << '\n'

    << '\n'

    << SWINES << " = "
    << this->value(SWINES) << '\n'
    << "  "
    << SWINES_IN_SOLO << " = "
    << this->value(SWINES_IN_SOLO) << '\n'
    << "  "
    << SWINES_IN_POVERTY << " = "
    << this->value(SWINES_IN_POVERTY) << '\n'
    << "  "
    << SWINES_ANNOUNCEMENT_BEGIN << " = "
    << this->value(SWINES_ANNOUNCEMENT_BEGIN) << '\n'
    << "  "
    << SWINE_ONLY_SECOND << " = "
    << this->value(SWINE_ONLY_SECOND) << '\n'

    << HYPERSWINES << " = "
    << this->value(HYPERSWINES) << '\n'
    << "  "
    << HYPERSWINES_IN_SOLO << " = "
    << this->value(HYPERSWINES_IN_SOLO) << '\n'
    << "  "
    << HYPERSWINES_IN_POVERTY << " = "
    << this->value(HYPERSWINES_IN_POVERTY) << '\n'
    << "  "
    << HYPERSWINES_ANNOUNCEMENT_BEGIN << " = "
    << this->value(HYPERSWINES_ANNOUNCEMENT_BEGIN) << '\n'
    << "  "
    << SWINES_AND_HYPERSWINES_JOINT << " = "
    << this->value(SWINES_AND_HYPERSWINES_JOINT) << '\n'

    << '\n'

    << "# Extrapoints:\n"

    << EXTRAPOINT_CATCH_FOX << " = "
    << this->value(EXTRAPOINT_CATCH_FOX) << '\n'
    << EXTRAPOINT_CATCH_FOX_LAST_TRICK << " = "
    << this->value(EXTRAPOINT_CATCH_FOX_LAST_TRICK) << '\n'
    << EXTRAPOINT_FOX_LAST_TRICK << " = "
    << this->value(EXTRAPOINT_FOX_LAST_TRICK) << '\n'
    << EXTRAPOINT_DOUBLE_FOX_LAST_TRICK << " = "
    << this->value(EXTRAPOINT_DOUBLE_FOX_LAST_TRICK) << '\n'

    << EXTRAPOINT_DOLLE_JABS_DOLLE << " = "
    << this->value(EXTRAPOINT_DOLLE_JABS_DOLLE) << '\n'
    << EXTRAPOINT_HEART_TRICK << " = "
    << this->value(EXTRAPOINT_HEART_TRICK) << '\n'

    << EXTRAPOINT_CHARLIE << " = "
    << this->value(EXTRAPOINT_CHARLIE) << '\n'
    << EXTRAPOINT_CATCH_CHARLIE << " = "
    << this->value(EXTRAPOINT_CATCH_CHARLIE) << '\n'
    << EXTRAPOINT_DOUBLE_CHARLIE << " = "
    << this->value(EXTRAPOINT_DOUBLE_CHARLIE) << '\n'
    << EXTRAPOINT_CATCH_DOUBLE_CHARLIE << " = "
    << this->value(EXTRAPOINT_CATCH_DOUBLE_CHARLIE) << '\n'
    << EXTRAPOINT_CATCH_CHARLIE_ONLY_WITH_DIAMOND_QUEEN << " = "
    << this->value(EXTRAPOINT_CATCH_CHARLIE_ONLY_WITH_DIAMOND_QUEEN) << '\n'


    << '\n'

    << ANNOUNCEMENT_FIRST_TRICK_THIRTY_POINTS << " = "
    << this->value(ANNOUNCEMENT_FIRST_TRICK_THIRTY_POINTS) << '\n'

    << ANNOUNCEMENT_FIRST_TRICK_THIRTY_POINTS_ONLY_FIRST << " = "
    << this->value(ANNOUNCEMENT_FIRST_TRICK_THIRTY_POINTS_ONLY_FIRST) << '\n'

    << ANNOUNCEMENT_FIRST_TRICK_THIRTY_POINTS_ONLY_RE_CONTRA << " = "
    << this->value(ANNOUNCEMENT_FIRST_TRICK_THIRTY_POINTS_ONLY_RE_CONTRA) << '\n'

    << ANNOUNCEMENT_FIRST_TRICK_THIRTY_POINTS_IN_MARRIAGE << " = "
    << this->value(ANNOUNCEMENT_FIRST_TRICK_THIRTY_POINTS_IN_MARRIAGE) << '\n'

    << ANNOUNCEMENT_RE_DOUBLES << " = "
    << this->value(ANNOUNCEMENT_RE_DOUBLES) << '\n'

    << ANNOUNCEMENT_CONTRA_DOUBLES_AGAINST_RE << " = "
    << this->value(ANNOUNCEMENT_CONTRA_DOUBLES_AGAINST_RE) << '\n'

    << '\n'

    << "# till when to make the announcements:\n"

    << KNOCKING << " = "
    << this->value(KNOCKING) << '\n'

    << ANNOUNCEMENT_NO_120 << " = "
    << this->value(ANNOUNCEMENT_NO_120) << '\n'
    << ANNOUNCEMENT_NO_90 << " = "
    << this->value(ANNOUNCEMENT_NO_90) << '\n'
    << ANNOUNCEMENT_NO_60 << " = "
    << this->value(ANNOUNCEMENT_NO_60) << '\n'
    << ANNOUNCEMENT_NO_30 << " = "
    << this->value(ANNOUNCEMENT_NO_30) << '\n'
    << ANNOUNCEMENT_NO_0 << " = "
    << this->value(ANNOUNCEMENT_NO_0) << '\n'
    << ANNOUNCEMENT_LAST << " = "
    << this->value(ANNOUNCEMENT_LAST) << '\n'

    << ANNOUNCEMENT_LIMIT_ONLY_FOR_CURRENT << " = "
    << this->value(ANNOUNCEMENT_LIMIT_ONLY_FOR_CURRENT) << '\n'
    << ANNOUNCEMENT_TILL_FULL_TRICK << " = "
    << this->value(ANNOUNCEMENT_TILL_FULL_TRICK) << '\n'

    << ANNOUNCEMENT_REPLY << " = "
    << this->value(ANNOUNCEMENT_REPLY) << '\n'

    << ANNOUNCEMENT_STRICT_LIMIT << " = "
    << this->value(ANNOUNCEMENT_STRICT_LIMIT) << '\n'

    << '\n'

    << "# "
    << NUMBER_OF_SAME_CARDS << " = "
    << (*this)(NUMBER_OF_SAME_CARDS) << '\n'
    << "# "
    << NUMBER_OF_CARD_COLORS << " = "
    << (*this)(NUMBER_OF_CARD_COLORS) << '\n'
    << "# "
    << NUMBER_OF_CARD_TCOLORS << " = "
    << (*this)(NUMBER_OF_CARD_TCOLORS) << '\n'
    << "# "
    << NUMBER_OF_CARD_VALUES << " = "
    << (*this)(NUMBER_OF_CARD_VALUES) << '\n'
    << "# "
    << NUMBER_OF_CARDS << " = "
    << (*this)(NUMBER_OF_CARDS) << '\n'
    << "# "
    << NUMBER_OF_TEAMS << " = "
    << (*this)(NUMBER_OF_TEAMS) << '\n'
    << "# "
    << NUMBER_OF_PLAYERS_PER_TEAM << " = "
    << (*this)(NUMBER_OF_PLAYERS_PER_TEAM) << '\n'
    << "# "
    << NUMBER_OF_TRICKS_IN_GAME << " = "
    << (*this)(NUMBER_OF_TRICKS_IN_GAME) << '\n'

    << '\n'

    << COUNTING << " = "
    << (*this)(COUNTING) << '\n'
    ;

  ostr.flags(flags);
  return ;
} // void Rule::write(ostream& ostr) const

/**
 ** -> result
 **
 ** @param	type	rule type
 **
 ** @return	the name of 'type'
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.3
 **/
string
name(Rule::TypeBool const& type)
{
  switch(type) {
  case Rule::MUTATE:
    return "mutate";
  case Rule::SHOW_IF_VALID:
    return "show if valid";
  case Rule::SHOW_ALL_HANDS:
    return "show all hands";
  case Rule::SHOW_TRICKS_IN_TRICKPILES:
    return "show tricks in trickpiles";
  case Rule::SHOW_TRICKPILES_POINTS:
    return "show trickpiles points";
  case Rule::SHOW_KNOWN_TEAMS_IN_GAME:
    return "show known teams in game";
  case Rule::SHOW_SOLOPLAYER_IN_GAME:
    return "show soloplayer in game";
  case Rule::CARD_HINT:
    return "card hint";
  case Rule::TAKE_BACK_TRICKS:
    return "take back tricks";
  case Rule::WITH_NINES:
    return "with nines";
  case Rule::BOCK:
    return "bock";
  case Rule::BOCK_APPEND:
    return "append bock";
  case Rule::BOCK_120:
    return "bock after 120-120";
  case Rule::BOCK_SOLO_LOST:
    return "bock after lost solo";
  case Rule::BOCK_RE_LOST:
    return "bock after lost re/contra";
  case Rule::BOCK_HEART_TRICK:
    return "bock after heart trick";
  case Rule::BOCK_BLACK:
    return "bock after black game";
  case Rule::THROW_WITH_NINES:
    return "throw with nines";
  case Rule::THROW_WITH_KINGS:
    return "throw with kings";
  case Rule::THROW_WITH_NINES_AND_KINGS:
    return "throw with nines and kings";
  case Rule::THROW_WITH_RICHNESS:
    return "throw with richness";
  case Rule::POVERTY:
    return "poverty";
  case Rule::POVERTY_SHIFT:
    return "poverty shift";
  case Rule::POVERTY_SHIFT_ONLY_TRUMP:
    return "poverty shift only trump";
  case Rule::POVERTY_FOX_DO_NOT_COUNT:
    return "poverty fox do not count";
  case Rule::POVERTY_FOX_SHIFT_EXTRA:
    return "poverty fox shift extra";
  case Rule::POVERTY_FOX_SHIFT_OPEN:
    return "poverty fox shift open";
  case Rule::THROW_WITH_ONE_TRUMP:
    return "throw with one trump";
  case Rule::THROW_WHEN_FOX_HIGHEST_TRUMP:
    return "throw when fox highest trump";
  case Rule::MARRIAGE_FIRST_FOREIGN:
    return "marriage first foreign";
  case Rule::MARRIAGE_FIRST_COLOR:
    return "marriage first color";
  case Rule::MARRIAGE_FIRST_SINGLE_COLOR:
    return "marriage first single color";
  case Rule::MARRIAGE_FIRST_TRUMP:
    return "marriage first trump";
  case Rule::MARRIAGE_FIRST_ONE_DECIDES:
    return "marriage first one decides";
  case Rule::MARRIAGE_BEFORE_POVERTY:
    return "marriage before poverty";
  case Rule::GENSCHER:
    return "genscher";
  case Rule::SOLO:
    return "solo";
  case Rule::SOLO_JACK:
    return "solo jack";
  case Rule::SOLO_QUEEN:
    return "solo queen";
  case Rule::SOLO_KING:
    return "solo king";
  case Rule::SOLO_QUEEN_JACK:
    return "solo queen jack";
  case Rule::SOLO_KING_JACK:
    return "solo king jack";
  case Rule::SOLO_KING_QUEEN:
    return "solo king queen";
  case Rule::SOLO_KOEHLER:
    return "solo koehler";
  case Rule::SOLO_COLOR:
    return "solo color";
  case Rule::SOLO_MEATLESS:
    return "solo meatless";
  case Rule::THROWING_BEFORE_SOLO:
    return "throwing before solo";
  case Rule::DOLLEN:
    return "dollen";
  case Rule::DOLLEN_SECOND_OVER_FIRST:
    return "dollen second over first";
  case Rule::DOLLEN_CONTRARY_IN_LAST_TRICK:
    return "dollen contrary in last trick";
  case Rule::DOLLEN_FIRST_OVER_SECOND_WITH_SWINES:
    return "dollen first over second with swines";
  case Rule::SWINES:
    return "swines";
  case Rule::SWINES_ANNOUNCEMENT_BEGIN:
    return "swines announcement begin";
  case Rule::SWINE_ONLY_SECOND:
    return "swine only second";
  case Rule::SWINES_IN_SOLO:
    return "swines in solo";
  case Rule::SWINES_IN_POVERTY:
    return "swines in poverty";
  case Rule::HYPERSWINES:
    return "hyperswines";
  case Rule::HYPERSWINES_ANNOUNCEMENT_BEGIN:
    return "hyperswines announcement begin";
  case Rule::HYPERSWINES_IN_SOLO:
    return "hyperswines in solo";
  case Rule::HYPERSWINES_IN_POVERTY:
    return "hyperswines in poverty";
  case Rule::SWINES_AND_HYPERSWINES_JOINT:
    return "swines and hyperswines joint";
  case Rule::EXTRAPOINT_CATCH_FOX:
    return "extrapoint catch fox";
  case Rule::EXTRAPOINT_CATCH_FOX_LAST_TRICK:
    return "extrapoint catch fox last trick";
  case Rule::EXTRAPOINT_FOX_LAST_TRICK:
    return "extrapoint fox last trick";
  case Rule::EXTRAPOINT_DOUBLE_FOX_LAST_TRICK:
    return "extrapoint double fox last trick";
  case Rule::EXTRAPOINT_DOLLE_JABS_DOLLE:
    return "extrapoint dolle jabs dolle";
  case Rule::EXTRAPOINT_HEART_TRICK:
    return "extrapoint heart trick";
  case Rule::EXTRAPOINT_CHARLIE:
    return "extrapoint charlie";
  case Rule::EXTRAPOINT_CATCH_CHARLIE:
    return "extrapoint catch charlie";
  case Rule::EXTRAPOINT_DOUBLE_CHARLIE:
    return "extrapoint double charlie";
  case Rule::EXTRAPOINT_CATCH_DOUBLE_CHARLIE:
    return "extrapoint catch double charlie";
  case Rule::EXTRAPOINT_CATCH_CHARLIE_ONLY_WITH_DIAMOND_QUEEN:
    return "extrapoint catch charlie only with diamond queen";
  case Rule::KNOCKING:
    return "knocking";
  case Rule::ANNOUNCEMENT_LIMIT_ONLY_FOR_CURRENT:
    return "announcement limit only for current";
  case Rule::ANNOUNCEMENT_TILL_FULL_TRICK:
    return "announcement till full trick";
  case Rule::ANNOUNCEMENT_FIRST_TRICK_THIRTY_POINTS:
    return "announcement first trick thirty points";
  case Rule::ANNOUNCEMENT_FIRST_TRICK_THIRTY_POINTS_ONLY_FIRST:
    return "announcement first trick thirty points only first";
  case Rule::ANNOUNCEMENT_FIRST_TRICK_THIRTY_POINTS_ONLY_RE_CONTRA:
    return "announcement first trick thirty points only re/contra";
  case Rule::ANNOUNCEMENT_FIRST_TRICK_THIRTY_POINTS_IN_MARRIAGE:
    return "announcement first trick thirty points in marriage";
  case Rule::ANNOUNCEMENT_REPLY:
    return "announcement reply";
  case Rule::ANNOUNCEMENT_STRICT_LIMIT:
    return "announcement strict limit";
  case Rule::ANNOUNCEMENT_RE_DOUBLES:
    return "announcement re/contra doubles";
  case Rule::ANNOUNCEMENT_CONTRA_DOUBLES_AGAINST_RE:
    return "announcement contra doubles against re";
  case Rule::NUMBER_OF_ROUNDS_LIMITED:
    return "number of rounds limited";
  case Rule::POINTS_LIMITED:
    return "points limited";
  case Rule::OFFER_DUTY_SOLO:
    return "offer duty solo";
  case Rule::LUSTSOLO_PLAYER_LEADS:
    return "lustsolo player leads";
  case Rule::SOLO_ALWAYS_COUNTS_TRIPLE:
    return "solo always counts triple";
  } // switch(type)

  DEBUG_ASSERT(false);
  return "";
} // string name(Rule::TypeBool const& type)

/**
 ** -> result
 **
 ** @param	type	rule type
 **
 ** @return	the name of 'type'
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.3
 **/
string
name(Rule::TypeUnsigned const& type)
{
  switch(type) {
  case Rule::UNSUPPORTED:
    return "unsupported";
  case Rule::BOCK_MULTIPLIER:
    return "bock multiplier";
  case Rule::SHOW_TRICKS_NUMBER:
    return "show tricks number";
  case Rule::MIN_NUMBER_OF_THROWING_NINES:
    return "min number of throwing nines";
  case Rule::MIN_NUMBER_OF_THROWING_KINGS:
    return "min number of throwing kings";
  case Rule::MIN_NUMBER_OF_THROWING_NINES_AND_KINGS:
    return "min number of throwing nines and kings";
  case Rule::MIN_RICHNESS_FOR_THROWING:
    return "min richness for throwing";
  case Rule::MARRIAGE_DETERMINATION:
    return "marriage determination";
  case Rule::ANNOUNCEMENT_NO_120:
    return "announcement no 120";
  case Rule::ANNOUNCEMENT_NO_90:
    return "announcement no 90";
  case Rule::ANNOUNCEMENT_NO_60:
    return "announcement no 60";
  case Rule::ANNOUNCEMENT_NO_30:
    return "announcement no 30";
  case Rule::ANNOUNCEMENT_NO_0:
    return "announcement no 0";
  case Rule::ANNOUNCEMENT_LAST:
    return "last announcement";
  case Rule::NUMBER_OF_ROUNDS:
    return "number of rounds";
  case Rule::POINTS:
    return "points";
  case Rule::NUMBER_OF_DUTY_SOLI:
    return "number of duty soli";
  case Rule::NUMBER_OF_DUTY_COLOR_SOLI:
    return "number of duty color soli";
  case Rule::NUMBER_OF_DUTY_PICTURE_SOLI:
    return "number of duty picture soli";
  case Rule::NUMBER_OF_PLAYERS:
    return "number of players";
  case Rule::NUMBER_OF_PLAYERS_IN_GAME:
    return "number of players in game";
  } // switch(type)

  DEBUG_ASSERT(false);
  return "";
} // string name(Rule::TypeUnsigned const& type)

/**
 ** -> result
 **
 ** @param	type	rule type
 **
 ** @return	the name of 'type'
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.3
 **/
string
name(Rule::TypeUnsignedExtra const& type)
{
  switch(type) {
  case Rule::NUMBER_OF_SAME_CARDS:
    return "number of same cards";
  case Rule::NUMBER_OF_CARD_COLORS:
    return "number of card colors";
  case Rule::NUMBER_OF_CARD_TCOLORS:
    return "number of card tcolors";
  case Rule::NUMBER_OF_CARD_VALUES:
    return "number of card values";
  case Rule::NUMBER_OF_CARDS:
    return "number of cards";
  case Rule::NUMBER_OF_TEAMS:
    return "number of teams";
  case Rule::NUMBER_OF_PLAYERS_PER_TEAM:
    return "number of players per team";
  case Rule::NUMBER_OF_GAMES_PER_ROUND:
    return "number of games per round";
  case Rule::MAX_NUMBER_OF_TRICKS_IN_GAME:
    return "maximal number of tricks in game";
  case Rule::NUMBER_OF_TRICKS_IN_GAME:
    return "number of tricks in game";
  case Rule::POINTS_FOR_DOPPELKOPF:
    return "points for doppelkopf";
  case Rule::MAX_NUMBER_OF_POVERTY_TRUMPS:
    return "max number of poverty trumps";
  } // switch(type)

  DEBUG_ASSERTION(false,
                  "Rule::name(" << static_cast<int>(type) << ")\n"
                  "  type = " << static_cast<int>(type) << " not in switch");
  return "";
} // string name(Rule::TypeUnsignedExtra const& type)

/**
 ** -> result
 **
 ** @param	type	rule type
 **
 ** @return	the name of 'type'
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **/
string
name(Rule::TypeCounting const& type)
{
  return "counting";
} // string name(Rule::TypeUnsignedExtra const& type)

/**
 ** -> result
 **
 ** @param	counting	the counting
 **
 ** @return	the name of 'counting'
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **/
string
name(Counting const& counting)
{
  using namespace COUNTING_NS;
  switch(counting) {
  case PLUS:
    return "plus";
  case MINUS:
    return "minus";
  case PLUSMINUS:
    return "plus-minus";
  } // switch(counting)

  DEBUG_ASSERT(false);
  return "";
} // string name(Counting const& counting)

#ifdef WORKAROUND
// 0.7.0 name changed

/**
 ** -> result
 **
 ** @param	counting	the counting
 **
 ** @return	the name of 'counting'
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **/
string
name_old(Counting const& counting)
{
  using namespace COUNTING_NS;
  switch(counting) {
  case PLUS:
    return "counting: plus";
  case MINUS:
    return "counting: minus";
  case PLUSMINUS:
    return "counting: plus-minus";
  } // switch(counting)

  DEBUG_ASSERT(false);
  return "";
} // string name(Counting const& counting)
#endif // #ifdef WORKAROUND

/**
 ** prints the name of 'type'
 **
 ** @param	ostr	output stream
 ** @param	type	type to print the name of
 **
 ** @return	output stream
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **/
ostream&
operator<<(ostream& ostr, Rule::TypeBool const& type)
{
  ostr << name(type);

  return ostr;
} // ostream& operator<<(ostream& ostr, Rule::TypeBool const& type)

/**
 **
 ** prints the name of 'type'
 **
 ** @param	ostr	output stream
 ** @param	type	type to print the name of
 **
 ** @return	output stream
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **
 **/
ostream&
operator<<(ostream& ostr, Rule::TypeUnsigned const& type)
{
  ostr << name(type);

  return ostr;
} // ostream& operator<<(ostream& ostr, Rule::TypeUnsigned const& type)

/**
 **
 ** prints the name of 'type'
 **
 ** @param	ostr	output stream
 ** @param	type	type to print the name of
 **
 ** @return	output stream
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **
 **/
ostream&
operator<<(ostream& ostr, Rule::TypeUnsignedExtra const& type)
{
  ostr << name(type);

  return ostr;
} // ostream& operator<<(ostream& ostr, Rule::TypeUnsignedExtra const& type)

/**
 **
 ** prints the name of 'type'
 **
 ** @param	ostr	output stream
 ** @param	type	type to print the name of
 **
 ** @return	output stream
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **
 **/
ostream&
operator<<(ostream& ostr, Rule::TypeCounting const& type)
{
  ostr << name(type);

  return ostr;
} // ostream& operator<<(ostream& ostr, Rule::TypeCounting const& type)

/**
 **
 ** prints the name of 'counting'
 **
 ** @param	ostr		output stream
 ** @param	counting	counting to print
 **
 ** @return	output stream
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **
 **/
ostream&
operator<<(ostream& ostr, Counting const& counting)
{
  ostr << name(counting);

  return ostr;
} // ostream& operator<<(ostream& ostr, Counting const& counting)

/**
 **
 ** prints the rule in 'ostr'
 **
 ** @param	ostr	output stream
 ** @param	rule	rule to print
 **
 ** @return	output stream
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **
 **/
ostream&
operator<<(ostream& ostr, Rule const& rule)
{
  rule.write(ostr);

  return ostr;
} // ostream& operator<<(ostream& ostr, Rule const& rule)

/**
 **
 ** -> result
 **
 ** @param	rule1	first rule that is compared
 ** @param	rule2	second rule that is compared
 **
 ** @return	whether the two rules are equal (according to value)
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.6
 **
 **/
bool
operator==(Rule const& rule1, Rule const& rule2)
{
  for (int i = Rule::BOOL_FIRST; i <= Rule::BOOL_LAST; ++i)
    if (rule1.value(static_cast<Rule::TypeBool>(i))
        != rule2.value(static_cast<Rule::TypeBool>(i)))
      return false;

  for (int i = Rule::UNSIGNED_FIRST; i <= Rule::UNSIGNED_LAST; ++i)
    if (rule1.value(static_cast<Rule::TypeUnsigned>(i))
        != rule2.value(static_cast<Rule::TypeUnsigned>(i)))
      return false;

  return true;
} // bool operator==(Rule const& rule1, Rule const& rule2)

/**
 **
 ** -> result
 **
 ** @param	rule1	first rule that is compared
 ** @param	rule2	second rule that is compared
 **
 ** @return	whether the two rules are different (according to value)
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.6
 **
 **/
bool
operator!=(Rule const& rule1, Rule const& rule2)
{
  return !(rule1 == rule2);
} // bool operator!=(Rule const& rule1, Rule const& rule2)
