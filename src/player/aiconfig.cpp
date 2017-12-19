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

#include "aiconfig.h"

#include "../party/party.h"
#include "../party/rule.h"
#include "../misc/setting.h"
#include "../misc/translations.h"
#include "../ui/ui.h"


unsigned const Aiconfig::HEURISTIC_NUMBER = (Aiconfig::HEURISTIC_LAST
                                             - Aiconfig::HEURISTIC_FIRST + 1);
unsigned const Aiconfig::BOOL_NUMBER = (Aiconfig::BOOL_LAST
                                        - Aiconfig::BOOL_FIRST + 1);
unsigned const Aiconfig::INT_NUMBER = (Aiconfig::INT_LAST
                                       - Aiconfig::INT_FIRST + 1);
unsigned const Aiconfig::CARD_NUMBER = (Aiconfig::CARD_LAST
                                        - Aiconfig::CARD_FIRST + 1);

bool Aiconfig::write_standard_type = true;

/**
 ** -> result
 **
 ** @param     difficulty   difficulty of the returned ai
 **
 ** @return    the preset aiconfig of the given difficulty
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.2
 **/
Aiconfig const&
Aiconfig::preset(Difficulty const difficulty)
{
  DEBUG_ASSERTION( (difficulty != CUSTOM),
                  "Aiconfig::preset(difficulty)\n"
                  "  difficulty is 'CUSTOM'");

  // just to get a warning when a new difficulty has been added
  switch (difficulty) {
  case CUSTOM:
  case NOVICE:
  case STANDARD_DEFENSIVE:
  case STANDARD_OFFENSIVE:
  case PROFI:
  case PROFI_AGGRESSIVE:
  case PROFI_UNFAIR:
    break;
  } // switch (difficulty)

  static vector<Aiconfig const*> aiconfigs_with_nines;
  static vector<Aiconfig const*> aiconfigs_without_nines;
  if (aiconfigs_with_nines.empty()) {
    { // CUSTOM
      aiconfigs_with_nines.push_back(NULL);
      aiconfigs_without_nines.push_back(NULL);
    } // CUSTOM
    { // NOVICE
#include "aiconfigs.preset/novice"
      istringstream istr(novice);
      Aiconfig* const aiconfig_with_nines
        = new Aiconfig(istr, NOVICE);
      Aiconfig* const aiconfig_without_nines
        = new Aiconfig(*aiconfig_with_nines);
      aiconfigs_with_nines.push_back(aiconfig_with_nines);
      aiconfigs_without_nines.push_back(aiconfig_without_nines);
    } // NOVICE
    { // STANDARD_DEFENSIVE
      // this is the hardcoded configuration
      Aiconfig* const aiconfig_with_nines
        = new Aiconfig();
      Aiconfig* const aiconfig_without_nines
        = new Aiconfig(*aiconfig_with_nines);
      for (unsigned i = aiconfig_without_nines->aitype_p.size() / 2 + 1;
           i < aiconfig_without_nines->aitype_p.size(); ++i)
        aiconfig_without_nines->aitype_p[i-2]
          = aiconfig_without_nines->aitype_p[i];
      for (unsigned i = aiconfig_without_nines->rating_p.size() / 2 + 1;
           i < aiconfig_without_nines->rating_p.size(); ++i)
        aiconfig_without_nines->rating_p[i-2]
          = aiconfig_without_nines->rating_p[i];
      for (unsigned i = aiconfig_without_nines->future_limit_p.size() / 2 + 1;
           i < aiconfig_without_nines->future_limit_p.size(); ++i)
        aiconfig_without_nines->future_limit_p[i-2]
          = aiconfig_without_nines->future_limit_p[i];
      aiconfigs_with_nines.push_back(aiconfig_with_nines);
      aiconfigs_without_nines.push_back(aiconfig_without_nines);
    } // STANDARD_DEFENSIVE
    { // STANDARD_OFFENSIVE
#include "aiconfigs.preset/standard.offensive"
      istringstream istr(standard_offensive);
      Aiconfig* const aiconfig_with_nines
        = new Aiconfig(istr, STANDARD_OFFENSIVE);
      Aiconfig* const aiconfig_without_nines
        = new Aiconfig(*aiconfig_with_nines);
      for (unsigned i = aiconfig_without_nines->aitype_p.size() / 2 + 1;
           i < aiconfig_without_nines->aitype_p.size(); ++i)
        aiconfig_without_nines->aitype_p[i-2]
          = aiconfig_without_nines->aitype_p[i];
      for (unsigned i = aiconfig_without_nines->rating_p.size() / 2 + 1;
           i < aiconfig_without_nines->rating_p.size(); ++i)
        aiconfig_without_nines->rating_p[i-2]
          = aiconfig_without_nines->rating_p[i];
      for (unsigned i = aiconfig_without_nines->future_limit_p.size() / 2 + 1;
           i < aiconfig_without_nines->future_limit_p.size(); ++i)
        aiconfig_without_nines->future_limit_p[i-2]
          = aiconfig_without_nines->future_limit_p[i];
      aiconfigs_with_nines.push_back(aiconfig_with_nines);
      aiconfigs_without_nines.push_back(aiconfig_without_nines);
    } // STANDARD_OFFENSIVE
    { // PROFI
#include "aiconfigs.preset/profi"
      istringstream istr(profi);
      Aiconfig* const aiconfig_with_nines
        = new Aiconfig(istr, PROFI);
      Aiconfig* const aiconfig_without_nines
        = new Aiconfig(*aiconfig_with_nines);
      for (unsigned i = aiconfig_without_nines->aitype_p.size() / 2 + 1;
           i < aiconfig_without_nines->aitype_p.size(); ++i)
        aiconfig_without_nines->aitype_p[i-2]
          = aiconfig_without_nines->aitype_p[i];
      for (unsigned i = aiconfig_without_nines->rating_p.size() / 2 + 1;
           i < aiconfig_without_nines->rating_p.size(); ++i)
        aiconfig_without_nines->rating_p[i-2]
          = aiconfig_without_nines->rating_p[i];
      for (unsigned i = aiconfig_without_nines->future_limit_p.size() / 2 + 1;
           i < aiconfig_without_nines->future_limit_p.size(); ++i)
        aiconfig_without_nines->future_limit_p[i-2]
          = aiconfig_without_nines->future_limit_p[i];
      aiconfigs_with_nines.push_back(aiconfig_with_nines);
      aiconfigs_without_nines.push_back(aiconfig_without_nines);
    } // PROFI
    { // PROFI_AGGRESSIVE
#include "aiconfigs.preset/profi.aggressive"
      istringstream istr(profi_aggressive);
      Aiconfig* const aiconfig_with_nines
        = new Aiconfig(istr, PROFI_AGGRESSIVE);
      Aiconfig* const aiconfig_without_nines
        = new Aiconfig(*aiconfig_with_nines);
      for (unsigned i = aiconfig_without_nines->aitype_p.size() / 2 + 1;
           i < aiconfig_without_nines->aitype_p.size(); ++i)
        aiconfig_without_nines->aitype_p[i-2]
          = aiconfig_without_nines->aitype_p[i];
      for (unsigned i = aiconfig_without_nines->rating_p.size() / 2 + 1;
           i < aiconfig_without_nines->rating_p.size(); ++i)
        aiconfig_without_nines->rating_p[i-2]
          = aiconfig_without_nines->rating_p[i];
      for (unsigned i = aiconfig_without_nines->future_limit_p.size() / 2 + 1;
           i < aiconfig_without_nines->future_limit_p.size(); ++i)
        aiconfig_without_nines->future_limit_p[i-2]
          = aiconfig_without_nines->future_limit_p[i];
      aiconfigs_with_nines.push_back(aiconfig_with_nines);
      aiconfigs_without_nines.push_back(aiconfig_without_nines);
    } // PROFI_AGGRESSIVE
    { // PROFI_UNFAIR
#include "aiconfigs.preset/profi.unfair"
      istringstream istr(profi_unfair);
      Aiconfig* const aiconfig_with_nines
        = new Aiconfig(istr, PROFI_UNFAIR);
      Aiconfig* const aiconfig_without_nines
        = new Aiconfig(*aiconfig_with_nines);
      for (unsigned i = aiconfig_without_nines->aitype_p.size() / 2 + 1;
           i < aiconfig_without_nines->aitype_p.size(); ++i)
        aiconfig_without_nines->aitype_p[i-2]
          = aiconfig_without_nines->aitype_p[i];
      for (unsigned i = aiconfig_without_nines->rating_p.size() / 2 + 1;
           i < aiconfig_without_nines->rating_p.size(); ++i)
        aiconfig_without_nines->rating_p[i-2]
          = aiconfig_without_nines->rating_p[i];
      for (unsigned i = aiconfig_without_nines->future_limit_p.size() / 2 + 1;
           i < aiconfig_without_nines->future_limit_p.size(); ++i)
        aiconfig_without_nines->future_limit_p[i-2]
          = aiconfig_without_nines->future_limit_p[i];
      aiconfigs_with_nines.push_back(aiconfig_with_nines);
      aiconfigs_without_nines.push_back(aiconfig_without_nines);
    } // PROFI_UNFAIR
  } // if (aiconfigs.empty())

  if (::party.rule()(Rule::WITH_NINES))
    return *aiconfigs_with_nines[difficulty];
  else
    return *aiconfigs_without_nines[difficulty];
} // static Aiconfig const& Aiconfig::preset(Difficulty difficulty);

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    the (interesting) keys for the heuristic configuration
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.4
 **/
vector<HeuristicsMap::Key> const&
Aiconfig::keys()
{
  static vector<HeuristicsMap::Key> keys;
  if (keys.empty()) {
    keys.push_back(HeuristicsMap::Key(HeuristicsMap::DEFAULT,
                                      HeuristicsMap::RE));
    keys.push_back(HeuristicsMap::Key(HeuristicsMap::DEFAULT,
                                      HeuristicsMap::CONTRA));
    keys.push_back(HeuristicsMap::Key(HeuristicsMap::MARRIAGE_UNDETERMINED,
                                      HeuristicsMap::RE));
    keys.push_back(HeuristicsMap::Key(HeuristicsMap::MARRIAGE_UNDETERMINED,
                                      HeuristicsMap::CONTRA));
    keys.push_back(HeuristicsMap::Key(HeuristicsMap::MARRIAGE_SILENT,
                                      HeuristicsMap::RE));
    keys.push_back(HeuristicsMap::Key(HeuristicsMap::MARRIAGE_SILENT,
                                      HeuristicsMap::CONTRA));
    keys.push_back(HeuristicsMap::Key(HeuristicsMap::POVERTY,
                                      HeuristicsMap::SPECIAL));
    keys.push_back(HeuristicsMap::Key(HeuristicsMap::POVERTY,
                                      HeuristicsMap::RE));
    keys.push_back(HeuristicsMap::Key(HeuristicsMap::POVERTY,
                                      HeuristicsMap::CONTRA));
    keys.push_back(HeuristicsMap::Key(HeuristicsMap::SOLI_COLOR,
                                      HeuristicsMap::RE));
    keys.push_back(HeuristicsMap::Key(HeuristicsMap::SOLI_COLOR,
                                      HeuristicsMap::CONTRA));
    keys.push_back(HeuristicsMap::Key(HeuristicsMap::SOLI_SINGLE_PICTURE,
                                      HeuristicsMap::RE));
    keys.push_back(HeuristicsMap::Key(HeuristicsMap::SOLI_SINGLE_PICTURE,
                                      HeuristicsMap::CONTRA));
    keys.push_back(HeuristicsMap::Key(HeuristicsMap::SOLI_DOUBLE_PICTURE,
                                      HeuristicsMap::RE));
    keys.push_back(HeuristicsMap::Key(HeuristicsMap::SOLI_DOUBLE_PICTURE,
                                      HeuristicsMap::CONTRA));
    keys.push_back(HeuristicsMap::Key(HeuristicsMap::SOLO_KOEHLER,
                                      HeuristicsMap::RE));
    keys.push_back(HeuristicsMap::Key(HeuristicsMap::SOLO_KOEHLER,
                                      HeuristicsMap::CONTRA));
    keys.push_back(HeuristicsMap::Key(HeuristicsMap::SOLO_MEATLESS,
                                      HeuristicsMap::RE));
    keys.push_back(HeuristicsMap::Key(HeuristicsMap::SOLO_MEATLESS,
                                      HeuristicsMap::CONTRA));
  } // if (keys.empty())

  return keys;
} // static vector<HeuristicsMap::Key> const& Aiconfig::keys()

/**
 ** -> result
 **
 ** @param	type   aitype
 **
 ** @return	whether the aitype has support for the rating
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.3
 **/
bool
supports_rating(AiType const type)
{
  switch(type) {
  case AiType::NO_CHOOSEBESTCARD:
  case AiType::RANDOM:
  case AiType::VIRTUAL_GAMES:
    return false;
  case AiType::GAMETREE:
  case AiType::GAMETREE_WITH_HEURISTICS:
  case AiType::GAMETREE_FOR_TEAM:
  case AiType::MONTE_CARLO:
  case AiType::MONTE_CARLO_JAB_OR_SERVE:
    return true;
  }; // switch (type)

  DEBUG_ASSERTION(false,
                  "::support_rating(type)\n"
                  "  unsupported type '" << type << "'");
  return false;
} // bool supports_rating(AiType const type)

/**
 ** -> return
 **
 ** @param	heuristic   the heuristic type
 **
 ** @return	whether 'heuristic' is a real heuristic
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.3
 **/
bool
Aiconfig::is_real(Aiconfig::Heuristic const heuristic)
{
  return (   (heuristic != Aiconfig::NO_HEURISTIC)
          && (heuristic != Aiconfig::MANUAL)
          && (heuristic != Aiconfig::BUG_REPORT)
          && (heuristic != Aiconfig::NETWORK)
          && (heuristic != Aiconfig::ONLY_VALID_CARD)
          && (heuristic != Aiconfig::CHOOSE_BEST_CARD));
} // static bool Aiconfig::is_real(Aiconfig::Heuristic heuristic)

/**
 ** -> return
 **
 ** @param	h	the heuristic type
 **
 ** @return	whether 'h' is a general heuristic
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.6
 **/
bool
Aiconfig::is_general(Aiconfig::Heuristic const h)
{
  return (    Aiconfig::is_real(h)
          && !Aiconfig::is_solo(h)
          && !Aiconfig::is_poverty(h) );
} // static bool Aiconfig::is_general(Aiconfig::Heuristic const h)

/**
 ** -> return
 **
 ** @param	h	the heuristic type
 **
 ** @return	whether 'h' was made for a poverty
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.3
 **/
bool
Aiconfig::is_poverty(Aiconfig::Heuristic const h)
{
  return (   (h == Aiconfig::CHOOSE_PFUND_POVERTY)
          || (   (h >= Aiconfig::HEURISTIC_POVERTY_FIRST)
              && (h <= Aiconfig::HEURISTIC_POVERTY_LAST) )
         );
} // static bool Aiconfig::is_poverty(Aiconfig::Heuristic const h)

/**
 ** -> return
 **
 ** @param	h	the heuristic type
 **
 ** @return	whether 'h' was made for a solo
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.6
 **/
bool
Aiconfig::is_solo(Aiconfig::Heuristic const h)
{
  return (   Aiconfig::is_solo_color(h)
          || Aiconfig::is_solo_picture(h)
          || Aiconfig::is_solo_meatless(h));
} // static bool Aiconfig::is_solo(Aiconfig::Heuristic const h)

/**
 ** -> return
 **
 ** @param	h	the heuristic type
 **
 ** @return	whether 'h' was made for a color solo
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.6
 **/
bool
Aiconfig::is_solo_color(Aiconfig::Heuristic const h)
{
  switch (h) {
  case BEST_WINNING:
  case COLOR_JAB_FOR_ACE:
  case COLOR_LOW_HIGH:
    return true;
  default:
    return false;
  }; // switch(h)
#ifdef WORKAROUND
  // g++ 4.0 -O3
  return false;
#endif
} // static bool Aiconfig::is_solo_color(Aiconfig::Heuristic const h)

/**
 ** -> return
 **
 ** @param	h	the heuristic type
 **
 ** @return	whether 'h' was made for a picture solo
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.6
 **/
bool
Aiconfig::is_solo_picture(Aiconfig::Heuristic const h)
{
  switch (h) {
  case PICTURE_SECONDBESTTRUMP:
    return true;
  default:
    return false;
  }; // switch(h)
#ifdef WORKAROUND
  // g++ 4.0 -O3
  return false;
#endif
} // static bool Aiconfig::is_solo_picture(Aiconfig::Heuristic const h)

/**
 ** -> return
 **
 ** @param	h	the heuristic type
 **
 ** @return	whether 'h' was made for a meatless solo
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.6
 **/
bool
Aiconfig::is_solo_meatless(Aiconfig::Heuristic const h)
{
  switch (h) {
  case MEATLESS_PLAYHIGHESTCOLOR:
    return true;
  default:
    return false;
  }; // switch(h)
#ifdef WORKAROUND
  // g++ 4.0 -O3
  return false;
#endif
} // static bool Aiconfig::is_solo_meatless(Aiconfig::Heuristic const h)


/**
 ** constructor
 **
 ** @param     -
 **
 ** @result    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.9
 **/
Aiconfig::Aiconfig() :
  difficulty_(STANDARD_DEFENSIVE),
  aitype_p(::party.rule()(Rule::MAX_NUMBER_OF_TRICKS_IN_GAME)),
  rating_p(::party.rule()(Rule::MAX_NUMBER_OF_TRICKS_IN_GAME)),
  future_limit_p(::party.rule()(Rule::MAX_NUMBER_OF_TRICKS_IN_GAME)),
  heuristic_states_(),
  bool_p(BOOL_NUMBER),
  int_p(INT_NUMBER),
  card_p(CARD_NUMBER),
  update_level_(0)
{
  this->reset_to_hardcoded();
  return ;
} // Aiconfig::Aiconfig()

/**
 ** constructor
 **
 ** @param	difficulty   difficulty to set
 **
 ** @result    -
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.13
 **/
Aiconfig::Aiconfig(Difficulty const difficulty) :
  difficulty_(STANDARD_DEFENSIVE),
  aitype_p(::party.rule()(Rule::MAX_NUMBER_OF_TRICKS_IN_GAME)),
  rating_p(::party.rule()(Rule::MAX_NUMBER_OF_TRICKS_IN_GAME)),
  future_limit_p(::party.rule()(Rule::MAX_NUMBER_OF_TRICKS_IN_GAME)),
  heuristic_states_(),
  bool_p(BOOL_NUMBER),
  int_p(INT_NUMBER),
  card_p(CARD_NUMBER),
  update_level_(0)
{
  this->reset_to_hardcoded();
  this->set_to_difficulty(difficulty);
  return ;
} // Aiconfig::Aiconfig(Difficulty difficulty)

/**
 ** constructor
 **
 ** @param	istr	stream with the infos
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.5.2
 **/
Aiconfig::Aiconfig(istream& istr) :
  difficulty_(STANDARD_DEFENSIVE),
  aitype_p(::party.rule()(Rule::MAX_NUMBER_OF_TRICKS_IN_GAME)),
  rating_p(::party.rule()(Rule::MAX_NUMBER_OF_TRICKS_IN_GAME)),
  future_limit_p(::party.rule()(Rule::MAX_NUMBER_OF_TRICKS_IN_GAME)),
  heuristic_states_(),
  bool_p(BOOL_NUMBER),
  int_p(INT_NUMBER),
  card_p(CARD_NUMBER),
  update_level_(0)
{
  (void)this->heuristic_states_[HeuristicsMap::Key(HeuristicsMap::DEFAULT,
                                                   HeuristicsMap::RE)];
#ifdef WORKAROUND
  this->init_heuristic_states();
#endif
  this->read(istr);

  return ;
} // Aiconfig::Aiconfig(istream& istr)

/**
 ** constructor
 **
 ** @param	istr         stream with the infos
 ** @param	difficulty   difficulty to set
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.5.2
 **/
Aiconfig::Aiconfig(istream& istr, Difficulty const difficulty) :
  difficulty_(difficulty),
  aitype_p(::party.rule()(Rule::MAX_NUMBER_OF_TRICKS_IN_GAME)),
  rating_p(::party.rule()(Rule::MAX_NUMBER_OF_TRICKS_IN_GAME)),
  future_limit_p(::party.rule()(Rule::MAX_NUMBER_OF_TRICKS_IN_GAME)),
  heuristic_states_(),
  bool_p(BOOL_NUMBER),
  int_p(INT_NUMBER),
  card_p(CARD_NUMBER),
  update_level_(1)
{
  (void)this->heuristic_states_[HeuristicsMap::Key(HeuristicsMap::DEFAULT,
                                                   HeuristicsMap::RE)];
#ifdef WORKAROUND
  this->init_heuristic_states();
#endif
  this->read(istr);
  DEBUG_ASSERTION(this->update_level_ == 1,
                  "Aiconfig::Aiconfig(istr, difficulty)\n"
                  "  this->update_level_ = " << this->update_level_ << " is not 1");
  this->update_level_ = 0;

  this->difficulty_ = difficulty;

  return ;
} // Aiconfig::Aiconfig(istream& istr, Difficulty difficulty)


/**
 ** copy constructor
 **
 ** @param	aiconfig   aiconfig to copy
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.4.2
 **/
Aiconfig::Aiconfig(Aiconfig const& aiconfig) :
  difficulty_(aiconfig.difficulty_),
  aitype_p(aiconfig.aitype_p),
  rating_p(aiconfig.rating_p),
  future_limit_p(aiconfig.future_limit_p),
  heuristic_states_(aiconfig.heuristic_states_),
  bool_p(aiconfig.bool_p),
  int_p(aiconfig.int_p),
  card_p(aiconfig.card_p),
  update_level_(0)
{ }

/**
 ** copy operator
 **
 ** @param	aiconfig   aiconfig to copy
 **
 ** @return	aiconfig
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.3
 **/
Aiconfig&
Aiconfig::operator=(Aiconfig const& aiconfig)
{
  if (this == &aiconfig)
    return *this;

  this->enter_update();
  this->difficulty_ = aiconfig.difficulty_;
  for (unsigned t = 0; t < ::party.rule()(Rule::MAX_NUMBER_OF_TRICKS_IN_GAME);
       ++t) {
    this->set_aitype(t, aiconfig.aitype(t));
    this->set_rating(t, aiconfig.rating(t));
    this->set_future_limit(t, aiconfig.future_limit(t));
  }
  this->heuristic_states_ = aiconfig.heuristic_states_;
  for (unsigned i = BOOL_FIRST; i <= BOOL_LAST; i++)
    this->set(TypeBool(i), aiconfig.value(TypeBool(i)));
  for (unsigned i = INT_FIRST; i <= INT_LAST; i++)
    this->set(TypeInt(i), aiconfig.value(TypeInt(i)));
  for (unsigned i = CARD_FIRST; i <= CARD_LAST; i++)
    this->set(TypeCard(i), aiconfig.value(TypeCard(i)));
  this->difficulty_ = aiconfig.difficulty();

  this->leave_update(this->difficulty());

  return *this;
} // Aiconfig& Aiconfig::operator=(Aiconfig const& aiconfig) const

/**
 ** destructor
 **
 ** @param	-
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.4.2
 **/
Aiconfig::~Aiconfig()
{ }

/**
 ** resets the aiconfig to the default values
 **
 ** @param	no   number of the player
 **                  default: 'UINT_MAX'
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.3
 **/
void
Aiconfig::reset_to_hardcoded(unsigned const no)
{

  switch (no) {
  case UINT_MAX:
    break;
  default:
    this->set_to_difficulty(PROFI);
    return ;
  } // switch (no)

  this->enter_update();

  this->set(TRUSTING,                              false);
  this->set(AGGRESSIVE,                             false);
  this->set(HANDS_KNOWN,                            false);
  this->set(TEAMS_KNOWN,                            false);
  this->set(ESTIMATE_HANDS,                         true);
  this->set(FEHLCREATIONONFIRSTCARD,                true);

  this->set(REMEMBERTRICKS,                            9);

  this->set(LIMIT_THROW_FEHL,                          8);
  this->set(LIMITQUEEN,                               12);
  this->set(LIMITDOLLE,                               20);

  this->set(LASTFEHLCREATION,                          3);
  this->set(LAST_TRICKS_WITHOUT_HEURISTICS,            3);
  this->set(FIRST_TRICK_FOR_TRUMP_POINTS_OPTIMIZATION, 4);

  this->set(ANNOUNCELIMIT,                            13);
  this->set(ANNOUNCELIMITDEC,                          1);
  this->set(ANNOUNCECONFIG,                            1);
  this->set(ANNOUNCELIMITREPLY,                       11);
  this->set(ANNOUNCECONFIGREPLY,                       1);

  this->set(TAKEPOVERTY,                              15);

  this->set(SINGLESOLO,                               20);
  this->set(DOUBLESOLO,                               54);
  this->set(TRIPLESOLO,                              108);
  this->set(COLORSOLO,                                30);
  this->set(MEATLESS,                                 28);

  this->set(LIMITTHROWING,                   Card(Card::DIAMOND, Card::QUEEN));

  this->set(TRUMPLIMIT_SOLOCOLOR,            Card(Card::DIAMOND, Card::QUEEN));
  this->set(TRUMPLIMIT_SOLOJACK,             Card(Card::DIAMOND, Card::JACK));
  this->set(TRUMPLIMIT_SOLOQUEEN,            Card(Card::DIAMOND, Card::QUEEN));
  this->set(TRUMPLIMIT_SOLOKING,             Card(Card::DIAMOND, Card::KING));
  this->set(TRUMPLIMIT_SOLOJACKKING,         Card(Card::CLUB, Card::JACK));
  this->set(TRUMPLIMIT_SOLOJACKQUEEN,        Card(Card::CLUB, Card::JACK));
  this->set(TRUMPLIMIT_SOLOQUEENKING,        Card(Card::CLUB, Card::QUEEN));
  this->set(TRUMPLIMIT_SOLOKOEHLER,          Card(Card::CLUB, Card::QUEEN));
  this->set(TRUMPLIMIT_MEATLESS,             Card(Card::DIAMOND, Card::TEN));
  this->set(TRUMPLIMIT_NORMAL,               Card(Card::HEART, Card::QUEEN));

  this->set(LOWEST_TRUMPLIMIT_SOLOCOLOR,     Card(Card::HEART, Card::JACK));
  this->set(LOWEST_TRUMPLIMIT_SOLOJACK,      Card(Card::HEART, Card::JACK));
  this->set(LOWEST_TRUMPLIMIT_SOLOQUEEN,     Card(Card::HEART, Card::QUEEN));
  this->set(LOWEST_TRUMPLIMIT_SOLOKING,      Card(Card::HEART, Card::KING));
  this->set(LOWEST_TRUMPLIMIT_SOLOJACKKING,  Card(Card::SPADE, Card::JACK));
  this->set(LOWEST_TRUMPLIMIT_SOLOJACKQUEEN, Card(Card::SPADE, Card::JACK));
  this->set(LOWEST_TRUMPLIMIT_SOLOQUEENKING, Card(Card::SPADE, Card::QUEEN));
  this->set(LOWEST_TRUMPLIMIT_SOLOKOEHLER,   Card(Card::SPADE, Card::QUEEN));
  this->set(LOWEST_TRUMPLIMIT_MEATLESS,      Card(Card::DIAMOND, Card::QUEEN));
  this->set(LOWEST_TRUMPLIMIT_NORMAL,        Card(Card::DIAMOND, Card::JACK));

  this->set(LIMITHIGH,                       Card(Card::HEART, Card::QUEEN));

  this->aitype_p[ 0] = AiType::MONTE_CARLO_JAB_OR_SERVE;
  this->aitype_p[ 1] = AiType::MONTE_CARLO_JAB_OR_SERVE;
  this->aitype_p[ 2] = AiType::VIRTUAL_GAMES;
  this->aitype_p[ 3] = AiType::VIRTUAL_GAMES;
  this->aitype_p[ 4] = AiType::VIRTUAL_GAMES;
  this->aitype_p[ 5] = AiType::VIRTUAL_GAMES;
  this->aitype_p[ 6] = AiType::VIRTUAL_GAMES;
  this->aitype_p[ 7] = AiType::VIRTUAL_GAMES;
  this->aitype_p[ 8] = AiType::GAMETREE_FOR_TEAM;
  this->aitype_p[ 9] = AiType::GAMETREE_FOR_TEAM;
  this->aitype_p[10] = AiType::GAMETREE_FOR_TEAM;
  this->aitype_p[11] = AiType::GAMETREE_FOR_TEAM;
  this->rating_p[ 0] = Rating::AVERAGE;
  this->rating_p[ 1] = Rating::AVERAGE;
  this->rating_p[ 2] = Rating::AVERAGE;
  this->rating_p[ 3] = Rating::AVERAGE;
  this->rating_p[ 4] = Rating::AVERAGE;
  this->rating_p[ 5] = Rating::AVERAGE;
  this->rating_p[ 6] = Rating::AVERAGE;
  this->rating_p[ 7] = Rating::AVERAGE;
  this->rating_p[ 8] = Rating::MAX;
  this->rating_p[ 9] = Rating::MAX;
  this->rating_p[10] = Rating::MAX;
  this->rating_p[11] = Rating::MAX;
  this->future_limit_p[ 0] = 2000;
  this->future_limit_p[ 1] = 2000;
  this->future_limit_p[ 2] = 10000;
  this->future_limit_p[ 3] = 10000;
  this->future_limit_p[ 4] = 10000;
  this->future_limit_p[ 5] = 10000;
  this->future_limit_p[ 6] = 10000;
  this->future_limit_p[ 7] = 10000;
  this->future_limit_p[ 8] = 10000;
  this->future_limit_p[ 9] = 10000;
  this->future_limit_p[10] = 10000;
  this->future_limit_p[11] = 10000;

  this->init_heuristic_states();

  this->difficulty_ = STANDARD_DEFENSIVE;
  this->leave_update(STANDARD_DEFENSIVE);

  return ;
} // void Aiconfig::reset_to_hardcoded(unsigned no = UINT_MAX)

/**
 ** sets the aiconfig to the preset difficulty
 **
 ** @param	difficulty   difficulty to set to
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.2
 **/
void
Aiconfig::set_to_difficulty(Difficulty const difficulty)
{
  if (difficulty == CUSTOM)
    return ;

  if (this->difficulty() == difficulty) {
    return ;
  }

  *this = Aiconfig::preset(difficulty);
  if (::ui != NULL)
    ::ui->aiconfig_changed(*this);
  return ;
} // void Aiconfig::set_to_difficulty(Difficulty const difficulty)

/**
 ** the rule has changed
 **
 ** @param       type        rule type to have changed
 ** @param       old_value   old value of the rule
 **
 ** @return      -
 **
 ** @author      Diether Knof
 **
 ** @version     0.7.8
 **/
void
Aiconfig::rule_changed(int const type, void const* const old_value)
{
  if (type == Rule::WITH_NINES) {
    if (this->difficulty() != CUSTOM) {
      Difficulty const difficulty = this->difficulty();
      this->difficulty_ = CUSTOM;
      this->set_to_difficulty(difficulty);
    }
  } // if (type == Rule::WITH_NINES)

  return ;
} // void Aiconfig::rule_changed(int const type, void const* const old_value)

/**
 ** updates the difficulty
 **
 ** @param	-
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.8
 **/
void
Aiconfig::update_difficulty()
{
  if (this->in_update())
    return ;
  int d;
  for (d = DIFFICULTY_FIRST; d <= DIFFICULTY_LAST; ++d)
    if (   (d != CUSTOM)
        && this->equal(Aiconfig::preset(static_cast<Difficulty>(d))))
      break;

  if (d > DIFFICULTY_LAST)
    d = CUSTOM;

  if (d == this->difficulty())
    return ;

  this->difficulty_ = static_cast<Difficulty>(d);
  if (::ui)
    ::ui->aiconfig_changed(*this);

  return ;
} // void Aiconfig::update_difficulty()

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	whether the aiconfig is in an update
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.8
 **/
bool
Aiconfig::in_update() const
{
  return (this->update_level_ > 0);
} // bool Aiconfig::in_update() const

/**
 ** begin a (nested) update
 **
 ** @param	-
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.8
 **/
void
Aiconfig::enter_update()
{
  this->update_level_ += 1;
  return ;
} // void Aiconfig::enter_update()

/**
 ** end a (nested) update
 ** if the last update level is ended, update the difficulty
 **
 ** @param	-
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.8
 **/
void
Aiconfig::leave_update()
{
  DEBUG_ASSERTION((this->update_level_ > 0) && (this->update_level_ < 9999),
                  "Aiconfig::leave_update()\n"
                  "  update_level is not > 0 / < 9999: " << this->update_level_);
  this->update_level_ -= 1;
  if (this->update_level_ == 0)
    this->update_difficulty();

  return ;
} // void Aiconfig::leave_update()

/**
 ** end a (nested) update
 ** if the last update level is ended, set the difficulty manual
 **
 ** @param	difficulty   difficulty to set to
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.8
 **/
void
Aiconfig::leave_update(Difficulty const difficulty)
{
  DEBUG_ASSERTION(this->update_level_ > 0,
                  "Aiconfig::leave_update()\n"
                  "  update_level is not > 0: " << this->update_level_);
  this->update_level_ -= 1;
  if (this->update_level_ == 0)
    this->difficulty_ = difficulty;

  return ;
} // void Aiconfig::leave_update(Difficulty const difficulty)

/**
 ** comparison with the aiconfig
 **
 ** @param	aiconfig   aiconfig to compare with
 **
 ** @return	whether aiconfig has the same values as the argument
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.3
 **/
bool
Aiconfig::equal(Aiconfig const& aiconfig) const
{
  return (   (this->aitype_p          == aiconfig.aitype_p)
          && (this->future_limit_p    == aiconfig.future_limit_p)
          && (this->rating_p          == aiconfig.rating_p)
          && (this->heuristic_states_ == aiconfig.heuristic_states_)
          && (this->bool_p            == aiconfig.bool_p)
          && (this->int_p        == aiconfig.int_p)
          && (this->card_p            == aiconfig.card_p) );
} // bool Aiconfig::equal(Aiconfig aiconfig) const

/**
 ** -> result
 **
 ** @param	trickno		trick number
 **
 ** @return	the aitype for trick 'trickno'
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **/
AiType const&
Aiconfig::aitype(unsigned const trickno) const
{
  return this->aitype_p[trickno];
} // AiType const& Aiconfig::aitye(unsigned const trickno) const;

/**
 ** sets the aitype for trick 'trickno'
 **
 ** @param	trickno		trick number
 ** @param	aitype		new aitype
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **/
void
Aiconfig::set_aitype(unsigned const trickno, AiType const& aitype)
{
#ifdef WORKAROUND
  if (trickno >= this->future_limit_p.size()) {
    return ;
  }
#endif
  this->enter_update();
  this->aitype_p[trickno] = aitype;
  this->leave_update();

  return ;
} // void Aiconfig::set_aitype(unsigned const trickno, AiType const& aitype) const

/**
 ** sets the aitype for all tricks
 **
 ** @param	aitype		new aitype
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **/
void
Aiconfig::set_aitype_for_all_tricks(AiType const& aitype)
{
  this->enter_update();
  for (unsigned i = 0; i < this->aitype_p.size(); ++i)
    this->set_aitype(i, aitype);
  this->leave_update();

  return ;
} // void Aiconfig::set_aitype_for_all_tricks(AiType const& aitype) const

/**
 ** -> result
 **
 ** @param	trickno		trick number
 **
 ** @return	the future limit for trick 'trickno'
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.3
 **/
unsigned const 
Aiconfig::future_limit(unsigned const trickno) const
{
  return this->future_limit_p[trickno];
} // unsigned const Aiconfig::future_limit(unsigned trickno) const

/**
 ** sets the future limit for trick 'trickno'
 **
 ** @param	trickno		trick number
 ** @param	future_limit	new future limit
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.3
 **/
void
Aiconfig::set_future_limit(unsigned const trickno, unsigned const future_limit)
{
#ifdef WORKAROUND
  if (trickno >= this->future_limit_p.size()) {
    return ;
  }
#endif
  this->enter_update();
  this->future_limit_p[trickno] = future_limit;
  this->leave_update();

  return ;
} // void Aiconfig::set_future_limit(unsigned trickno, unsigned future_limit)

/**
 ** sets the future limit for all tricks
 **
 ** @param	future_limit		new future limit
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.3
 **/
void
Aiconfig::set_future_limit_for_all_tricks(unsigned const future_limit)
{
  this->enter_update();
  for (unsigned i = 0; i < this->future_limit_p.size(); ++i)
    this->set_future_limit(i, future_limit);
  this->leave_update();

  return ;
} // void Aiconfig::set_future_limit_for_all_tricks(unsigned future_limit)

/**
 ** -> result
 **
 ** @param	trickno   trick number
 **
 ** @return	the rating for trick 'trickno'
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.3
 **/
Rating::Type const&
Aiconfig::rating(unsigned const trickno) const
{
  return this->rating_p[trickno];
} // Rating::Type const Aiconfig::rating(unsigned trickno) const

/**
 ** sets the rating for trick 'trickno'
 **
 ** @param	trickno   trick number
 ** @param	rating    new rating 
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.3
 **/
void
Aiconfig::set_rating(unsigned const trickno, Rating::Type const& rating)
{
#ifdef WORKAROUND
  if (trickno >= this->rating_p.size()) {
    return ;
  }
#endif

  this->enter_update();
  this->rating_p[trickno] = rating;
  this->leave_update();

  return ;
} // void Aiconfig::set_future_limit(unsigned trickno, Rating::Type rating)

/**
 ** sets the rating for all tricks
 **
 ** @param	rating   new rating for all tricks
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.3
 **/
void
Aiconfig::set_rating_for_all_tricks(Rating::Type const& rating)
{
  this->enter_update();
  for (unsigned i = 0; i < this->rating_p.size(); ++i)
    this->set_rating(i, rating);
  this->leave_update();

  return ;
} // void Aiconfig::set_rating_for_all_tricks(Rating::Type rating)

/**
 ** -> result
 **
 ** @param	key     the key
 **
 ** @return	the heuristic states for the given key
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.3
 **/
vector<Aiconfig::HeuristicState> const&
Aiconfig::heuristic_states(HeuristicsMap::Key const& key) const
{
  map<HeuristicsMap::Key, vector<HeuristicState> >::const_iterator
    states = this->heuristic_states_.find(key);

  // if there is no special take a default one
  if (   (states == this->heuristic_states_.end())
      || states->second.empty()) {
    if (   (key.gametype_group == HeuristicsMap::SOLO_MEATLESS)
        && (key.playertype_group == HeuristicsMap::CONTRA) )
      return this->heuristic_states(HeuristicsMap::SOLO_MEATLESS,
                                    HeuristicsMap::RE);

    if (key.gametype_group == HeuristicsMap::DEFAULT)
      return this->heuristic_states(HeuristicsMap::DEFAULT, HeuristicsMap::RE);

    return this->heuristic_states(HeuristicsMap::DEFAULT, key.playertype_group);
  } // if (states.empty())

  return states->second;
} // vector<Aiconfig::HeuristicState> Aiconfig::heuristic_states(HeuristicsMap::Key key) const

/**
 ** -> result
 **
 ** @param	gametype_group     the game type group
 ** @param	playertype_group   the player type group
 **
 ** @return	the heuristic states for the given groups
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.3
 **/
vector<Aiconfig::HeuristicState> const&
Aiconfig::heuristic_states(HeuristicsMap::GametypeGroup const gametype_group,
                           HeuristicsMap::PlayerTypeGroup const playertype_group
                          ) const
{
  return this->heuristic_states(HeuristicsMap::Key(gametype_group,
                                                   playertype_group));
} // vector<Aiconfig::HeuristicState> Aiconfig::heuristic_states(HeuristicsMap::GametypeGroup gametype_group, HeuristicsMap::PlayerTypeGroup playertype_group) const

/**
 ** -> result
 **
 ** @param	key     the key
 **
 ** @return	the heuristic states for the given key
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.3
 **/
vector<Aiconfig::HeuristicState>&
Aiconfig::heuristic_states(HeuristicsMap::Key const& key)
{
  map<HeuristicsMap::Key, vector<HeuristicState> >::iterator
    states = this->heuristic_states_.find(key);

  // if there is no special take a default one
  if (states == this->heuristic_states_.end()) {
    if (   (key.gametype_group == HeuristicsMap::SOLO_MEATLESS)
        && (key.playertype_group == HeuristicsMap::CONTRA) )
      return this->heuristic_states(HeuristicsMap::SOLO_MEATLESS,
                                    HeuristicsMap::RE);

    if (key.gametype_group == HeuristicsMap::DEFAULT)
      return this->heuristic_states(HeuristicsMap::DEFAULT, HeuristicsMap::RE);

    return this->heuristic_states(HeuristicsMap::DEFAULT, key.playertype_group);
  } // if (states.empty())

  return states->second;
} // vector<Aiconfig::HeuristicState> Aiconfig::heuristic_states(HeuristicsMap::Key key) const

/**
 ** -> result
 **
 ** @param	gametype_group     the game type group
 ** @param	playertype_group   the player type group
 **
 ** @return	the heuristic states for the given groups
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.3
 **/
vector<Aiconfig::HeuristicState>&
Aiconfig::heuristic_states(HeuristicsMap::GametypeGroup const gametype_group,
                           HeuristicsMap::PlayerTypeGroup const playertype_group
                          )
{
  return this->heuristic_states(HeuristicsMap::Key(gametype_group,
                                                   playertype_group));
} // vector<Aiconfig::HeuristicState>& Aiconfig::heuristic_states(HeuristicsMap::GametypeGroup gametype_group, HeuristicsMap::PlayerTypeGroup playertype_group)

/**
 ** -> result
 **
 ** @param	gametype_group     the game type group
 ** @param	playertype_group   the player type group
 **
 ** @return	the heuristics for the given groups
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.3
 **/
vector<Aiconfig::Heuristic>
Aiconfig::heuristics(HeuristicsMap::GametypeGroup const gametype_group,
                     HeuristicsMap::PlayerTypeGroup const playertype_group
                    ) const
{
  vector<HeuristicState> const& states
    = this->heuristic_states(gametype_group, playertype_group);

  vector<Heuristic> heuristics;
  heuristics.push_back(ONLY_VALID_CARD);
  for (vector<HeuristicState>::const_iterator s = states.begin();
       s != states.end();
       ++s)
    if (s->active)
      heuristics.push_back(s->heuristic);

  heuristics.push_back(CHOOSE_BEST_CARD);
  return heuristics;
} // vector<Aiconfig::Heuristic> Aiconfig::heuristics(HeuristicsMap::GametypeGroup gametype_group, HeuristicsMap::PlayerTypeGroup playertype_group) const

/**
 ** -> result
 **
 ** @param	player   player
 **
 ** @return	the heuristics for the player
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.3
 **/
vector<Aiconfig::Heuristic>
Aiconfig::heuristics(Player const& player) const
{
  return this->heuristics(HeuristicsMap::group(player.game()),
                          HeuristicsMap::group(player));
} // vector<Aiconfig::Heuristic> Aiconfig::heuristics(Player player) const

/**
 ** initializes the heuristic states
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
Aiconfig::init_heuristic_states()
{
  this->enter_update();
  this->heuristic_states_.clear();
  (void)this->heuristic_states_[HeuristicsMap::Key(HeuristicsMap::DEFAULT,
                                                   HeuristicsMap::RE)];


  { // normal
    vector<HeuristicState>& states
      = this->heuristic_states_[HeuristicsMap::Key(HeuristicsMap::DEFAULT,
                                                   HeuristicsMap::RE)];
    states.push_back(HeuristicState(CHOOSE_ACE,                     true));
    states.push_back(HeuristicState(PLAY_COLOR_FOR_PARTNER_ACE,     true));
    states.push_back(HeuristicState(CHOOSE_TEN,                     true));
    states.push_back(HeuristicState(TRY_FOR_DOPPELKOPF,             true));
    states.push_back(HeuristicState(CHOOSE_FOR_COLOR_TRICK,         true));
    states.push_back(HeuristicState(SERVE_COLOR_TRICK,              true));
    states.push_back(HeuristicState(JAB_FOX,                        true));
    states.push_back(HeuristicState(CHOOSE_PFUND,                   true));
    states.push_back(HeuristicState(CHOOSE_PFUND_BEFORE_PARTNER,    true));
    states.push_back(HeuristicState(JAB_FOR_ACE,                    true));
    states.push_back(HeuristicState(PLAY_HIGHEST_COLOR_CARD_IN_GAME,true));
    states.push_back(HeuristicState(PLAY_FOR_TEAM,                  true));
    states.push_back(HeuristicState(PLAY_COLOR_FOR_PARTNER,         true));
    states.push_back(HeuristicState(TRY_COLOR_FOR_PARTNER,          true));
    states.push_back(HeuristicState(START_WITH_COLOR,               false));
    states.push_back(HeuristicState(RETRY_COLOR,                    true));
    states.push_back(HeuristicState(PARTNER_BACKHAND_DRAW_TRUMP,    true));
    states.push_back(HeuristicState(CREATE_FEHL,                    true));
    states.push_back(HeuristicState(GET_TRICK_FOR_ANNOUNCEMENT,     true));
    states.push_back(HeuristicState(LET_TRICK_PASS,                 true));
    states.push_back(HeuristicState(BEST_WINNING,                   true));
    states.push_back(HeuristicState(DRAW_TRUMP,                     true));
    states.push_back(HeuristicState(PLAY_TO_JAB_LATER,              true));
    states.push_back(HeuristicState(START_WITH_LOW_COLOR,           false));
    states.push_back(HeuristicState(START_WITH_LOW_TRUMP,           true));
    states.push_back(HeuristicState(LOW_HIGH,                       true));
    states.push_back(HeuristicState(PLAY_FOR_PARTNER_WORRIES,       true));
    states.push_back(HeuristicState(SERVE_TRUMP_TRICK,              true));
    states.push_back(HeuristicState(PLAY_TRUMP,                     true));
    states.push_back(HeuristicState(PLAY_BAD_COLOR,                 true));
    states.push_back(HeuristicState(JAB_COLOR_OVER_FOX,             true));

    { // contra
      vector<HeuristicState>& states
        = this->heuristic_states_[HeuristicsMap::Key(HeuristicsMap::DEFAULT,
                                                     HeuristicsMap::CONTRA)]
        = this->heuristic_states_[HeuristicsMap::Key(HeuristicsMap::DEFAULT,
                                                     HeuristicsMap::RE)];
      for (vector<HeuristicState>::iterator s = states.begin();
           s != states.end();
           ++s) {
        switch (s->heuristic) {
        case START_WITH_COLOR:
          s->active = true;
          break;
        case START_WITH_LOW_COLOR:
          s->active = true;
          break;
        case START_WITH_LOW_TRUMP:
          s->active = false;
          break;
        default:
          break;
        } // switch (s->heuristic)
      } // for (s)
    } // contra
  } // normal
  { // poverty
    { // soloplayer
      vector<HeuristicState>& states
        = this->heuristic_states_[HeuristicsMap::Key(HeuristicsMap::POVERTY,
                                                     HeuristicsMap::SPECIAL)];
      states.push_back(HeuristicState(CHOOSE_ACE,                   true));
      states.push_back(HeuristicState(PLAY_COLOR_FOR_PARTNER_ACE,   true));
      states.push_back(HeuristicState(CHOOSE_TEN,                   true));
      states.push_back(HeuristicState(PLAY_COLOR_FOR_PARTNER,       true));
      states.push_back(HeuristicState(TRY_COLOR_FOR_PARTNER,        true));
      states.push_back(HeuristicState(CHOOSE_PFUND_POVERTY,         true));
      states.push_back(HeuristicState(POVERTY_SPECIAL_PLAY_PFUND,   true));
      states.push_back(HeuristicState(POVERTY_SPECIAL_GIVE_NO_POINTS, true));
      states.push_back(HeuristicState(POVERTY_SPECIAL_OFFER_PFUND, true));
      states.push_back(HeuristicState(SERVE_COLOR_TRICK,          true));
      states.push_back(HeuristicState(SERVE_TRUMP_TRICK,          true));
      states.push_back(HeuristicState(PLAY_BAD_COLOR,   true));
    } // soloplayer
    { // re
      vector<HeuristicState>& states
        = this->heuristic_states_[HeuristicsMap::Key(HeuristicsMap::POVERTY,
                                                     HeuristicsMap::RE)];
      states.push_back(HeuristicState(CHOOSE_ACE,                 true));
      states.push_back(HeuristicState(CHOOSE_TEN,                 true));
      states.push_back(HeuristicState(CREATE_FEHL,                true));
      states.push_back(HeuristicState(SERVE_COLOR_TRICK,          true));
      states.push_back(HeuristicState(CHOOSE_FOR_COLOR_TRICK,     true));
      states.push_back(HeuristicState(POVERTY_RE_TRUMP_COLOR_TRICK_HIGH, true));
      states.push_back(HeuristicState(GET_TRICK_FOR_ANNOUNCEMENT, true));
      states.push_back(HeuristicState(SERVE_TRUMP_TRICK,          true));
      states.push_back(HeuristicState(DRAW_TRUMP,                 true));
      states.push_back(HeuristicState(PLAY_TO_JAB_LATER,              true));
      states.push_back(HeuristicState(PLAY_HIGHEST_COLOR_CARD_IN_GAME,true));
      states.push_back(HeuristicState(POVERTY_RE_PLAY_TRUMP,      true));
      states.push_back(HeuristicState(POVERTY_BEST_WINNING_CARD,  true));
      states.push_back(HeuristicState(PLAY_TRUMP,                 true));
      states.push_back(HeuristicState(JAB_COLOR_OVER_FOX,         true));
    } // re

    { // contra
      vector<HeuristicState>& states
        = this->heuristic_states_[HeuristicsMap::Key(HeuristicsMap::POVERTY,
                                                     HeuristicsMap::CONTRA)];
      states.push_back(HeuristicState(PLAY_COLOR_FOR_PARTNER,         true));
      states.push_back(HeuristicState(CHOOSE_ACE,                   true));
      states.push_back(HeuristicState(CHOOSE_TEN,                   true));
      states.push_back(HeuristicState(POVERTY_CONTRA_PLAY_COLOR,    true));
      states.push_back(HeuristicState(POVERTY_CONTRA_TRUMP_COLOR_TRICK_HIGH, true));
      states.push_back(HeuristicState(CHOOSE_PFUND,                 true));
      states.push_back(HeuristicState(SERVE_COLOR_TRICK,            true));
      states.push_back(HeuristicState(CHOOSE_FOR_COLOR_TRICK,     true));
      states.push_back(HeuristicState(POVERTY_LEAVE_TO_PARTNER,     true));
      states.push_back(HeuristicState(POVERTY_OVERJAB_RE,           true));
      states.push_back(HeuristicState(PLAY_HIGHEST_COLOR_CARD_IN_GAME,true));
      states.push_back(HeuristicState(PARTNER_BACKHAND_DRAW_TRUMP,  true));
      states.push_back(HeuristicState(JAB_FOX,                        true));
      states.push_back(HeuristicState(PLAY_FOR_PARTNER_WORRIES,       true));
      states.push_back(HeuristicState(CHOOSE_PFUND_BEFORE_PARTNER,        true));
      states.push_back(HeuristicState(SERVE_TRUMP_TRICK,            true));
      states.push_back(HeuristicState(BEST_WINNING,           true));
      states.push_back(HeuristicState(JAB_COLOR_OVER_FOX,     true));
    } // contra
  } // poverty

  { // solo
    { // color
      { // soloplayer
        vector<HeuristicState>& states
          = this->heuristic_states_[HeuristicsMap::Key(HeuristicsMap::SOLI_COLOR,
                                                       HeuristicsMap::RE)];
        states.push_back(HeuristicState(CHOOSE_ACE,           true));
        states.push_back(HeuristicState(CHOOSE_TEN,           true));
        states.push_back(HeuristicState(COLOR_JAB_FOR_ACE,          true));
        states.push_back(HeuristicState(SERVE_COLOR_TRICK,          true));
        states.push_back(HeuristicState(PLAY_HIGHEST_COLOR_CARD_IN_GAME,true));
        states.push_back(HeuristicState(BEST_WINNING,         true));
        states.push_back(HeuristicState(COLOR_LOW_HIGH,             true));
        states.push_back(HeuristicState(DRAW_TRUMP,                 true));
        states.push_back(HeuristicState(PLAY_TO_JAB_LATER,              true));
        states.push_back(HeuristicState(GET_TRICK_FOR_ANNOUNCEMENT, true));
        states.push_back(HeuristicState(SERVE_TRUMP_TRICK,          true));
        states.push_back(HeuristicState(START_WITH_LOW_COLOR,       true));
      } // soloplayer
      { // contra
        vector<HeuristicState>& states
          = this->heuristic_states_[HeuristicsMap::Key(HeuristicsMap::SOLI_COLOR,
                                                       HeuristicsMap::CONTRA)]
          = this->heuristic_states_[HeuristicsMap::Key(HeuristicsMap::DEFAULT,
                                                       HeuristicsMap::RE)];
        for (vector<HeuristicState>::iterator s = states.begin();
             s != states.end();
             ++s) {
          switch (s->heuristic) {
          case START_WITH_LOW_COLOR:
            s->active = true;
            break;
          case JAB_FOR_ACE:
            s->heuristic = COLOR_JAB_FOR_ACE;
            break;
          case LOW_HIGH:
            s->heuristic = COLOR_LOW_HIGH;
            break;
          case START_WITH_COLOR:
            s->heuristic = PLAY_COLOR_IN_SOLO;
            s->active = true;
            break;
          case PARTNER_BACKHAND_DRAW_TRUMP:
            s->active = false;
            break;
          default:
            break;
          } // switch (s->heuristic)
        } // for (s \in states)

        // ToDo: add PARTNER_BACKHAND_DRAW_TRUMP
      } // contra
    } // color
    { // single picture
      { // soloplayer
        vector<HeuristicState>& states
          = this->heuristic_states_[HeuristicsMap::Key(HeuristicsMap::SOLI_SINGLE_PICTURE,
                                                       HeuristicsMap::RE)];
        states.push_back(HeuristicState(PLAY_HIGHEST_COLOR_CARD_IN_GAME,true));
        states.push_back(HeuristicState(DRAW_TRUMP,                 true));
        states.push_back(HeuristicState(CHOOSE_SINGLE_ACE,          true));
        states.push_back(HeuristicState(CHOOSE_ACE,                 true));
        states.push_back(HeuristicState(CHOOSE_TEN,                 true));
        states.push_back(HeuristicState(PICTURE_SECONDBESTTRUMP,    true));
        states.push_back(HeuristicState(COLOR_JAB_FOR_ACE,          true));
        states.push_back(HeuristicState(SERVE_COLOR_TRICK,          true));
        states.push_back(HeuristicState(BEST_WINNING,         true));
        states.push_back(HeuristicState(COLOR_LOW_HIGH,             true));
        states.push_back(HeuristicState(GET_TRICK_FOR_ANNOUNCEMENT, true));
        states.push_back(HeuristicState(SERVE_TRUMP_TRICK,          true));
        states.push_back(HeuristicState(PLAY_TRUMP,                 true));
        states.push_back(HeuristicState(CREATE_FEHL,                true));
        states.push_back(HeuristicState(START_WITH_LOW_COLOR,       true));
      } // soloplayer
      { // contra
        vector<HeuristicState>& states
          = this->heuristic_states_[HeuristicsMap::Key(HeuristicsMap::SOLI_SINGLE_PICTURE,
                                                       HeuristicsMap::CONTRA)]
          = this->heuristic_states_[HeuristicsMap::Key(HeuristicsMap::DEFAULT,
                                                       HeuristicsMap::RE)];
        for (vector<HeuristicState>::iterator s = states.begin();
             s != states.end();
             ++s) {
          switch (s->heuristic) {
          case CHOOSE_TEN:
            s->heuristic = PLAY_COLOR_FOR_PARTNER_ACE;
            break;
          case PLAY_COLOR_FOR_PARTNER_ACE:
            s->heuristic = CHOOSE_TEN;
            break;
          case START_WITH_LOW_COLOR:
            s->active = true;
            break;
          case START_WITH_COLOR:
            s->heuristic = PLAY_COLOR_IN_SOLO;
            s->active = true;
            break;
          case PARTNER_BACKHAND_DRAW_TRUMP:
            s->active = false;
            break;
          default:
            break;
          } // switch (s->heuristic)
        } // for (s \in states)
      } // contra
    } // single picture
    { // double picture
      { // soloplayer
        vector<HeuristicState>& states
          = this->heuristic_states_[HeuristicsMap::Key(HeuristicsMap::SOLI_DOUBLE_PICTURE,
                                                       HeuristicsMap::RE)];
        states.push_back(HeuristicState(PLAY_HIGHEST_COLOR_CARD_IN_GAME,true));
        states.push_back(HeuristicState(DRAW_TRUMP,                 true));
        states.push_back(HeuristicState(CHOOSE_SINGLE_ACE,          true));
        states.push_back(HeuristicState(CHOOSE_ACE,                 true));
        states.push_back(HeuristicState(CHOOSE_TEN,                 true));
        states.push_back(HeuristicState(PICTURE_SECONDBESTTRUMP,    true));
        states.push_back(HeuristicState(COLOR_JAB_FOR_ACE,          true));
        states.push_back(HeuristicState(SERVE_COLOR_TRICK,          true));
        states.push_back(HeuristicState(BEST_WINNING,         true));
        states.push_back(HeuristicState(PLAY_TO_JAB_LATER,          true));
        states.push_back(HeuristicState(COLOR_LOW_HIGH,             true));
        states.push_back(HeuristicState(GET_TRICK_FOR_ANNOUNCEMENT, true));
        states.push_back(HeuristicState(SERVE_TRUMP_TRICK,          true));
        states.push_back(HeuristicState(PLAY_TRUMP,                 true));
        states.push_back(HeuristicState(CREATE_FEHL,                true));
        states.push_back(HeuristicState(START_WITH_LOW_COLOR,       true));
      } // soloplayer
      { // contra
        vector<HeuristicState>& states
          = this->heuristic_states_[HeuristicsMap::Key(HeuristicsMap::SOLI_DOUBLE_PICTURE,
                                                       HeuristicsMap::CONTRA)]
          = this->heuristic_states_[HeuristicsMap::Key(HeuristicsMap::DEFAULT,
                                                       HeuristicsMap::RE)];
        for (vector<HeuristicState>::iterator s = states.begin();
             s != states.end();
             ++s) {
          switch (s->heuristic) {
          case CHOOSE_TEN:
            s->heuristic = PLAY_COLOR_FOR_PARTNER_ACE;
            break;
          case PLAY_COLOR_FOR_PARTNER_ACE:
            s->heuristic = CHOOSE_TEN;
            break;
          case START_WITH_LOW_COLOR:
            s->active = true;
            break;
          case START_WITH_COLOR:
            s->heuristic = PLAY_COLOR_IN_SOLO;
            s->active = true;
            break;
          case PARTNER_BACKHAND_DRAW_TRUMP:
            s->active = false;
            break;
          default:
            break;
          } // switch (s->heuristic)
        } // for (s \in states)
      } // contra
    } // double picture
    { // koehler
      { // soloplayer
        vector<HeuristicState>& states
          = this->heuristic_states_[HeuristicsMap::Key(HeuristicsMap::SOLO_KOEHLER,
                                                       HeuristicsMap::RE)];
        states.push_back(HeuristicState(PLAY_HIGHEST_COLOR_CARD_IN_GAME,true));
        states.push_back(HeuristicState(DRAW_TRUMP,                 true));
        states.push_back(HeuristicState(CHOOSE_SINGLE_ACE,          true));
        states.push_back(HeuristicState(CHOOSE_ACE,                 true));
        states.push_back(HeuristicState(CHOOSE_TEN,                 true));
        states.push_back(HeuristicState(PICTURE_SECONDBESTTRUMP,    true));
        states.push_back(HeuristicState(COLOR_JAB_FOR_ACE,          true));
        states.push_back(HeuristicState(SERVE_COLOR_TRICK,          true));
        states.push_back(HeuristicState(GET_TRICK_FOR_ANNOUNCEMENT, true));
        states.push_back(HeuristicState(SERVE_TRUMP_TRICK,          true));
        states.push_back(HeuristicState(BEST_WINNING,         true));
        states.push_back(HeuristicState(PLAY_TRUMP,                 true));
        states.push_back(HeuristicState(PLAY_TO_JAB_LATER,          true));
        states.push_back(HeuristicState(COLOR_LOW_HIGH,             true));
      } // soloplayer
      { // contra
        vector<HeuristicState>& states
          = this->heuristic_states_[HeuristicsMap::Key(HeuristicsMap::SOLO_KOEHLER,
                                                       HeuristicsMap::CONTRA)]
          = this->heuristic_states_[HeuristicsMap::Key(HeuristicsMap::DEFAULT,
                                                       HeuristicsMap::RE)];
        for (vector<HeuristicState>::iterator s = states.begin();
             s != states.end();
             ++s) {
          switch (s->heuristic) {
          case START_WITH_COLOR:
            s->heuristic = PLAY_COLOR_IN_SOLO;
            s->active = true;
            break;
          case PARTNER_BACKHAND_DRAW_TRUMP:
            s->active = false;
            break;
          default:
            break;
          } // switch (s->heuristic)
        } // for (s \in states)
      } // contra
    } // koehler
    { // meatless
      { // soloplayer
        vector<HeuristicState>& states
          = this->heuristic_states_[HeuristicsMap::Key(HeuristicsMap::SOLO_MEATLESS,
                                                       HeuristicsMap::RE)];
        states.push_back(HeuristicState(PLAY_HIGHEST_COLOR_CARD_IN_GAME,true));
        states.push_back(HeuristicState(MEATLESS_PLAYHIGHESTCOLOR,  true));
        states.push_back(HeuristicState(SERVE_COLOR_TRICK,          true));
        states.push_back(HeuristicState(CHOOSE_PFUND,               true));
        states.push_back(HeuristicState(CREATE_FEHL,                true));
      } // soloplayer
      { // contra
        this->heuristic_states_[HeuristicsMap::Key(HeuristicsMap::SOLO_MEATLESS,
                                                   HeuristicsMap::CONTRA)]
          = this->heuristic_states_[HeuristicsMap::Key(HeuristicsMap::SOLO_MEATLESS,
                                                       HeuristicsMap::RE)];
      } // contra
    } // meatless
  } // solo
  { // marriage
    { // undetermined marriage
      this->heuristic_states_[HeuristicsMap::Key(HeuristicsMap::MARRIAGE_UNDETERMINED,
                                                 HeuristicsMap::RE)]
        = this->heuristic_states_[HeuristicsMap::Key(HeuristicsMap::DEFAULT,
                                                     HeuristicsMap::RE)];

      vector<HeuristicState>& states
        = this->heuristic_states_[HeuristicsMap::Key(HeuristicsMap::MARRIAGE_UNDETERMINED,
                                                     HeuristicsMap::RE)];

      states.insert(states.begin(),
                    HeuristicState(PLAY_TO_GET_MARRIED,               true));
      states.insert(states.begin(),
                    HeuristicState(CHOOSE_SINGLE_ACE,                 true));

      {
        vector<HeuristicState>& states = this->heuristic_states_[HeuristicsMap::Key(HeuristicsMap::MARRIAGE_UNDETERMINED,
                                                                                    HeuristicsMap::CONTRA)];

        states.push_back(HeuristicState(PLAY_TO_MARRY,                  true));
        states.push_back(HeuristicState(CHOOSE_ACE,                     true));
        states.push_back(HeuristicState(PLAY_COLOR_FOR_PARTNER_ACE,     true));
        states.push_back(HeuristicState(CHOOSE_TEN,                     true));
        states.push_back(HeuristicState(TRY_FOR_DOPPELKOPF,             true));
        states.push_back(HeuristicState(CHOOSE_FOR_COLOR_TRICK,         true));
        states.push_back(HeuristicState(SERVE_COLOR_TRICK,              true));
        states.push_back(HeuristicState(JAB_FOX,                        true));
        states.push_back(HeuristicState(CHOOSE_PFUND,                   true));
        states.push_back(HeuristicState(CHOOSE_PFUND_BEFORE_PARTNER,    true));
        states.push_back(HeuristicState(JAB_FOR_ACE,                    true));
        states.push_back(HeuristicState(PLAY_FOR_TEAM,                  true));
        states.push_back(HeuristicState(PLAY_COLOR_FOR_PARTNER,         true));
        states.push_back(HeuristicState(TRY_COLOR_FOR_PARTNER,          true));
        states.push_back(HeuristicState(RETRY_COLOR,                    true));
        states.push_back(HeuristicState(PARTNER_BACKHAND_DRAW_TRUMP,    true));
        states.push_back(HeuristicState(CREATE_FEHL,                    true));
        states.push_back(HeuristicState(BEST_WINNING,                   true));
        states.push_back(HeuristicState(DRAW_TRUMP,                     true));
        states.push_back(HeuristicState(PLAY_TO_JAB_LATER,              true));
        states.push_back(HeuristicState(LOW_HIGH,                       true));
        states.push_back(HeuristicState(GET_TRICK_FOR_ANNOUNCEMENT,     true));
        states.push_back(HeuristicState(SERVE_TRUMP_TRICK,              true));
        states.push_back(HeuristicState(PLAY_FOR_PARTNER_WORRIES,       true));
        states.push_back(HeuristicState(PLAY_BAD_COLOR,                 true));
        states.push_back(HeuristicState(JAB_COLOR_OVER_FOX,             true));
      }
    } // undetermined marriage
    { // silent marriage
      vector<HeuristicState>& states
        = this->heuristic_states_[HeuristicsMap::Key(HeuristicsMap::MARRIAGE_SILENT,
                                                     HeuristicsMap::RE)];
      states.push_back(HeuristicState(CHOOSE_ACE,                     true));
      states.push_back(HeuristicState(CHOOSE_TEN,                     true));
      states.push_back(HeuristicState(COLOR_JAB_FOR_ACE,              true));
      states.push_back(HeuristicState(BEST_WINNING,             true));
      states.push_back(HeuristicState(COLOR_LOW_HIGH,                 true));
      states.push_back(HeuristicState(DRAW_TRUMP,                     true));
      states.push_back(HeuristicState(PLAY_TO_JAB_LATER,              true));
      states.push_back(HeuristicState(PLAY_HIGHEST_COLOR_CARD_IN_GAME,true));
      states.push_back(HeuristicState(GET_TRICK_FOR_ANNOUNCEMENT,     true));

      this->heuristic_states_[HeuristicsMap::Key(HeuristicsMap::MARRIAGE_SILENT,
                                                 HeuristicsMap::CONTRA)]
        = this->heuristic_states_[HeuristicsMap::Key(HeuristicsMap::SOLI_COLOR,
                                                     HeuristicsMap::CONTRA)];
    } // silent marriage
  } // marriage

  this->fill_up_heuristic_states();
  this->leave_update();
  return ;
} // void Aiconfig::init_heuristic_states()

/**
 ** fills up the heuristic states so that all real heuristics are included
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
Aiconfig::fill_up_heuristic_states()
{
  this->enter_update();
  for (map<HeuristicsMap::Key, vector<HeuristicState> >::iterator
       m = this->heuristic_states_.begin();
       m != this->heuristic_states_.end();
       ++m) {
    vector<HeuristicState>& states = m->second;

    std::set<Heuristic> heuristics;
    for (int h = HEURISTIC_FIRST; h <= HEURISTIC_LAST; ++h)
      if (Aiconfig::is_real(static_cast<Heuristic>(h)))
        heuristics.insert(static_cast<Heuristic>(h));

    for (vector<HeuristicState>::const_iterator s = states.begin();
         s != states.end();
         ++s)
      heuristics.erase(s->heuristic);

    for (std::set<Heuristic>::const_iterator h = heuristics.begin();
         h != heuristics.end();
         ++h)
      states.push_back(HeuristicState(*h, false));

  } // for (m \in this->heuristic_states_)

  this->leave_update();
  return ;
} // void Aiconfig::fill_up_heuristic_states()

/**
 ** -> result
 **
 ** @param	key         the group key
 ** @param	heuristic   the heuristic type
 **
 ** @return	the value of the heuristic in the given context
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.3
 **/
bool
Aiconfig::value(HeuristicsMap::Key const& key,
                Heuristic const heuristic) const
{
  vector<HeuristicState> const&
    states = this->heuristic_states(key);

  for (vector<HeuristicState>::const_iterator s = states.begin();
       s != states.end();
       ++s)
    if (s->heuristic == heuristic)
      return s->active;

  return false;
} // bool Aiconfig::value(HeuristicsMap::Key key, Heuristic heuristic) const

/**
 ** -> result
 **
 ** @param	gametype_group     the game type group
 ** @param	playertype_group   the player type group
 ** @param	heuristic          the heuristic type
 **
 ** @return	the value of the heuristic 'type in the given context
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.3
 **/
bool
Aiconfig::value(HeuristicsMap::GametypeGroup const gametype_group,
                HeuristicsMap::PlayerTypeGroup const playertype_group,
                Heuristic const heuristic) const
{
  return this->value(HeuristicsMap::Key(gametype_group, playertype_group),
                     heuristic);
} // bool Aiconfig::value(HeuristicsMap::GametypeGroup gametype_group, HeuristicsMap::PlayerTypeGroup playertype_group, Heuristic heuristic) const

/**
 ** -> result
 **
 ** @param	type	the bool type
 **
 ** @return	the value of the aiconfig 'type'
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.6
 **/
bool
Aiconfig::value(TypeBool const type) const
{
  return this->bool_p[type - BOOL_FIRST];
} // bool Aiconfig::value(TypeBool) const;

/**
 ** -> result
 **
 ** @param	type	the unsigned type
 **
 ** @return	the value of the aiconfig 'type'
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.6
 **/
int
Aiconfig::value(TypeInt const type) const
{
  return this->int_p[type - INT_FIRST];
} // int Aiconfig::value(TypeInt) const;

/**
 ** -> result
 **
 ** @param	type	the card type
 **
 ** @return	the value of the aiconfig 'type'
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.6
 **/
Card
Aiconfig::value(TypeCard const type) const
{
  return this->card_p[type - CARD_FIRST];
} // Card Aiconfig::value(TypeCard const type) const

/**
 ** -> result
 **
 ** @param     type   config type
 **
 ** @return    minimal value for the type
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.2
 **/
int
Aiconfig::min(TypeInt const& type) const
{
  switch(type) {
  case REMEMBERTRICKS:
  case LASTFEHLCREATION:
  case LAST_TRICKS_WITHOUT_HEURISTICS:
  case FIRST_TRICK_FOR_TRUMP_POINTS_OPTIMIZATION:
    return 0;

  case LIMIT_THROW_FEHL:
  case LIMITQUEEN:
  case LIMITDOLLE:
    return 0;

  case ANNOUNCELIMIT:
  case ANNOUNCELIMITDEC:
  case ANNOUNCECONFIG:
  case ANNOUNCELIMITREPLY:
  case ANNOUNCECONFIGREPLY:
    return 0;

  case TAKEPOVERTY:
    return 0;

  case SINGLESOLO:
  case DOUBLESOLO:
  case TRIPLESOLO:
  case COLORSOLO:
  case MEATLESS:
    return 0;

  } // switch(type)

  return UINT_MAX;
} // int Aiconfig::min(TypeInt type) const

/**
 ** -> result
 **
 ** @param     type   config type
 **
 ** @return    maximum value for the type
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.2
 **/
int
Aiconfig::max(TypeInt const& type) const
{
  switch(type) {
  case REMEMBERTRICKS:
  case LASTFEHLCREATION:
  case LAST_TRICKS_WITHOUT_HEURISTICS:
  case FIRST_TRICK_FOR_TRUMP_POINTS_OPTIMIZATION:
    // when changing 'with nines' -> 'without nines' -> 'with nines'
    // it should keep '12'
    //return ::party.rule()(Rule::NUMBER_OF_TRICKS_IN_GAME);
    return static_cast<int>(::party.rule()(Rule::MAX_NUMBER_OF_TRICKS_IN_GAME));

  case LIMIT_THROW_FEHL:
  case LIMITQUEEN:
  case LIMITDOLLE:
    return 33;

  case ANNOUNCELIMIT:
  case ANNOUNCELIMITDEC:
  case ANNOUNCECONFIG:
  case ANNOUNCELIMITREPLY:
  case ANNOUNCECONFIGREPLY:

    return 1000;

  case TAKEPOVERTY:
    return 1000;

  case SINGLESOLO:
  case DOUBLESOLO:
  case TRIPLESOLO:
  case COLORSOLO:
  case MEATLESS:
    return 1000;

  } // switch(type)

  return UINT_MAX;
} // int Aiconfig::max(TypeInt type) const


/**
 ** -> result
 **
 ** @param     type   config type
 **
 ** @return    the valid cards
 **
 ** @author    Diether Knof
 **
 ** @version   0.7.2
 **/
vector<Card> const&
Aiconfig::valid_cards(TypeCard const type) const
{
  switch(type) {
  case LIMITTHROWING:
  case LIMITHIGH:

  case TRUMPLIMIT_NORMAL:
  case LOWEST_TRUMPLIMIT_NORMAL:
  case TRUMPLIMIT_SOLOCOLOR:
  case LOWEST_TRUMPLIMIT_SOLOCOLOR:
    {
      static vector<Card> valid_cards;
      if (valid_cards.empty()) {
        valid_cards.push_back(Card(Card::CLUB,    Card::QUEEN));
        valid_cards.push_back(Card(Card::SPADE,   Card::QUEEN));
        valid_cards.push_back(Card(Card::HEART,   Card::QUEEN));
        valid_cards.push_back(Card(Card::DIAMOND, Card::QUEEN));
        valid_cards.push_back(Card(Card::CLUB,    Card::JACK));
        valid_cards.push_back(Card(Card::SPADE,   Card::JACK));
        valid_cards.push_back(Card(Card::HEART,   Card::JACK));
        valid_cards.push_back(Card(Card::DIAMOND, Card::JACK));
      } // if (valid.cards.empty())

      return valid_cards;
    }
  case TRUMPLIMIT_SOLOJACK:
  case LOWEST_TRUMPLIMIT_SOLOJACK:
    {
      static vector<Card> valid_cards;
      if (valid_cards.empty()) {
        valid_cards.push_back(Card(Card::CLUB,    Card::JACK));
        valid_cards.push_back(Card(Card::SPADE,   Card::JACK));
        valid_cards.push_back(Card(Card::HEART,   Card::JACK));
        valid_cards.push_back(Card(Card::DIAMOND, Card::JACK));
      } // if (valid.cards.empty())

      return valid_cards;
    }
  case TRUMPLIMIT_SOLOQUEEN:
  case LOWEST_TRUMPLIMIT_SOLOQUEEN:
    {
      static vector<Card> valid_cards;
      if (valid_cards.empty()) {
        valid_cards.push_back(Card(Card::CLUB,    Card::QUEEN));
        valid_cards.push_back(Card(Card::SPADE,   Card::QUEEN));
        valid_cards.push_back(Card(Card::HEART,   Card::QUEEN));
        valid_cards.push_back(Card(Card::DIAMOND, Card::QUEEN));
      } // if (valid.cards.empty())

      return valid_cards;
    }
  case TRUMPLIMIT_SOLOKING:
  case LOWEST_TRUMPLIMIT_SOLOKING:
    {
      static vector<Card> valid_cards;
      if (valid_cards.empty()) {
        valid_cards.push_back(Card(Card::CLUB,    Card::KING));
        valid_cards.push_back(Card(Card::SPADE,   Card::KING));
        valid_cards.push_back(Card(Card::HEART,   Card::KING));
        valid_cards.push_back(Card(Card::DIAMOND, Card::KING));
      } // if (valid.cards.empty())

      return valid_cards;
    }
  case TRUMPLIMIT_SOLOJACKKING:
  case LOWEST_TRUMPLIMIT_SOLOJACKKING:
    {
      static vector<Card> valid_cards;
      if (valid_cards.empty()) {
        valid_cards.push_back(Card(Card::CLUB,    Card::KING));
        valid_cards.push_back(Card(Card::SPADE,   Card::KING));
        valid_cards.push_back(Card(Card::HEART,   Card::KING));
        valid_cards.push_back(Card(Card::DIAMOND, Card::KING));
        valid_cards.push_back(Card(Card::CLUB,    Card::JACK));
        valid_cards.push_back(Card(Card::SPADE,   Card::JACK));
        valid_cards.push_back(Card(Card::HEART,   Card::JACK));
        valid_cards.push_back(Card(Card::DIAMOND, Card::JACK));
      } // if (valid.cards.empty())

      return valid_cards;
    }
  case TRUMPLIMIT_SOLOJACKQUEEN:
  case LOWEST_TRUMPLIMIT_SOLOJACKQUEEN:
    {
      static vector<Card> valid_cards;
      if (valid_cards.empty()) {
        valid_cards.push_back(Card(Card::CLUB,    Card::QUEEN));
        valid_cards.push_back(Card(Card::SPADE,   Card::QUEEN));
        valid_cards.push_back(Card(Card::HEART,   Card::QUEEN));
        valid_cards.push_back(Card(Card::DIAMOND, Card::QUEEN));
        valid_cards.push_back(Card(Card::CLUB,    Card::JACK));
        valid_cards.push_back(Card(Card::SPADE,   Card::JACK));
        valid_cards.push_back(Card(Card::HEART,   Card::JACK));
        valid_cards.push_back(Card(Card::DIAMOND, Card::JACK));
      } // if (valid.cards.empty())

      return valid_cards;
    }
  case TRUMPLIMIT_SOLOQUEENKING:
  case LOWEST_TRUMPLIMIT_SOLOQUEENKING:
    {
      static vector<Card> valid_cards;
      if (valid_cards.empty()) {
        valid_cards.push_back(Card(Card::CLUB,    Card::KING));
        valid_cards.push_back(Card(Card::SPADE,   Card::KING));
        valid_cards.push_back(Card(Card::HEART,   Card::KING));
        valid_cards.push_back(Card(Card::DIAMOND, Card::KING));
        valid_cards.push_back(Card(Card::CLUB,    Card::QUEEN));
        valid_cards.push_back(Card(Card::SPADE,   Card::QUEEN));
        valid_cards.push_back(Card(Card::HEART,   Card::QUEEN));
        valid_cards.push_back(Card(Card::DIAMOND, Card::QUEEN));
      } // if (valid.cards.empty())

      return valid_cards;
    }
  case TRUMPLIMIT_SOLOKOEHLER:
  case LOWEST_TRUMPLIMIT_SOLOKOEHLER:
    {
      static vector<Card> valid_cards;
      if (valid_cards.empty()) {
        valid_cards.push_back(Card(Card::CLUB,    Card::KING));
        valid_cards.push_back(Card(Card::SPADE,   Card::KING));
        valid_cards.push_back(Card(Card::HEART,   Card::KING));
        valid_cards.push_back(Card(Card::DIAMOND, Card::KING));
        valid_cards.push_back(Card(Card::CLUB,    Card::QUEEN));
        valid_cards.push_back(Card(Card::SPADE,   Card::QUEEN));
        valid_cards.push_back(Card(Card::HEART,   Card::QUEEN));
        valid_cards.push_back(Card(Card::DIAMOND, Card::QUEEN));
        valid_cards.push_back(Card(Card::CLUB,    Card::JACK));
        valid_cards.push_back(Card(Card::SPADE,   Card::JACK));
        valid_cards.push_back(Card(Card::HEART,   Card::JACK));
        valid_cards.push_back(Card(Card::DIAMOND, Card::JACK));
      } // if (valid.cards.empty())

      return valid_cards;
    }
  case TRUMPLIMIT_MEATLESS:
  case LOWEST_TRUMPLIMIT_MEATLESS:
    {
      static vector<Card> valid_cards;
      if (valid_cards.empty()) {
        valid_cards.push_back(Card(Card::DIAMOND, Card::ACE));
        valid_cards.push_back(Card(Card::DIAMOND, Card::TEN));
        valid_cards.push_back(Card(Card::DIAMOND, Card::KING));
        valid_cards.push_back(Card(Card::DIAMOND, Card::QUEEN));
        valid_cards.push_back(Card(Card::DIAMOND, Card::JACK));
        valid_cards.push_back(Card(Card::DIAMOND, Card::NINE));
      } // if (valid.cards.empty())

      return valid_cards;
    }
  } // switch(type)

  {
    static vector<Card> valid_cards;
    return valid_cards;
  }
} // vector<Card> const& Aiconfig::valid_cards(TypeCard const type) const

/**
 ** sets 'type' to 'value'
 **
 ** @param	type	Aiconfigtype
 ** @param	value	new value
 **
 ** @return	'true', if the type is valid, otherwise 'false'
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.6
 **/
bool
Aiconfig::set(string const& type, string const& value)
{
  unsigned i;

#ifdef DEPRECATED
  // 0.7.6 replacing
  if (type == "fairplay hands") {
    this->set(TEAMS_KNOWN, !(   (value == "true")
                             || (value == "yes")
                             || (value == "1") ) );
    return true;
  }
  if (type == "fairplay teams") {
    this->set(HANDS_KNOWN, !(   (value == "true")
                             || (value == "yes")
                             || (value == "1")) );
    return true;
  }
#endif
  for (i = Aiconfig::BOOL_FIRST; i <= Aiconfig::BOOL_LAST; i++) {
    if (type == name(Aiconfig::TypeBool(i))) {
      this->set(Aiconfig::TypeBool(i), value);
      return true;
    } // if (type == name(Aiconfig::TypeBool(i)))
  } // (i \in BOOL)
  for (i = Aiconfig::INT_FIRST; i <= Aiconfig::INT_LAST; i++) {
    if (type == name(Aiconfig::TypeInt(i))) {
      this->set(Aiconfig::TypeInt(i), value);
      return true;
    } // if (type == name(Aiconfig::TypeInt(i)))
  } // (i \in INT)
  for (i = Aiconfig::CARD_FIRST; i <= Aiconfig::CARD_LAST; i++) {
    if (type == name(Aiconfig::TypeCard(i))) {
      this->set(Aiconfig::TypeCard(i), value);
      return true;
    } // if (type == name(Aiconfig::TypeCard(i)))
  } // (i \in CARD)

  return false;
} // bool Aiconfig::set(string const& type, string const& value)

/**
 ** activates/deactivates the heuristic in the given context
 **
 ** @param	gametype_group     the game type group
 ** @param	playertype_group   the player type group
 ** @param	heuristic          the heuristic type
 ** @param	value              new value
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.3
 **/
void
Aiconfig::set(HeuristicsMap::GametypeGroup const gametype_group,
              HeuristicsMap::PlayerTypeGroup const playertype_group,
              Heuristic const heuristic,
              string const& value)
{
  if (   (value != "true")
      && (value != "false")
      && (value != "yes")
      && (value != "no")
      && (value != "0")
      && (value != "1")) {
    cerr << "Aiconfig::set(GametypeGroup, PlayerTypeGroup, Heuristic, value)\n"
      << "  illegal value '" << value << "' for '" << heuristic << "', "
      << "must be a boolean ('true' or 'false' or 'yes' or 'no' or '1' or '0')."
      << '\n'
      << "  Taking 'false'."
      << endl;
  }
  this->set(gametype_group, playertype_group, heuristic,
            (   (value == "true")
             || (value == "yes")
             || (value == "1")) );

  return ;
} // void Aiconfig::set(HeuristicsMap::GametypeGroup gametype_group, HeuristicsMap::PlayerTypeGroup playertype_group, Heuristic heuristic, string value)

/**
 ** sets 'type' to 'value'
 **
 ** @param	type	Aiconfigtype
 ** @param	value	new value
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.6
 **/
void
Aiconfig::set(TypeBool const type, string const& value)
{
  if (   (value != "true")
      && (value != "false")
      && (value != "yes")
      && (value != "no")
      && (value != "0")
      && (value != "1")) {
    cerr << "Aiconfig::set(TypeBool, value)\n"
      << "  illegal value '" << value << "' for '" << type << "', "
      << "must be a boolean ('true' or 'false' or 'yes' or 'no' or '1' or '0')."
      << '\n'
      << "  Taking 'false'."
      << endl;
  }
  this->set(type, (  (   (value == "false")
                      || (value == "no")
                      || (value == "0") )
                   ? false
                   :	true) );

  return ;
} // void Aiconfig::set(const TypeBool type, string const& value)

/**
 ** sets 'type' to 'value'
 **
 ** @param	type	Aiconfigtype
 ** @param	value	new value
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.6
 **/
void
Aiconfig::set(TypeInt const type, string const& value)
{
  char* end_ptr;
  unsigned number = strtoul(value.c_str(), &end_ptr, 0);
  if (*end_ptr != '\0') {
    cerr << "Aiconfig::set(TypeInt, value)\n"
      << "illegal value '" << value << "' for '" << type << "', "
      << "must be a digit.\n"
      << "  Taking " << number << "."
      << endl;
  }

  this->set(type, number);

  return ;
} // void Aiconfig::set(const TypeInt type, string const& value)

/**
 ** sets 'type' to 'value'
 **
 ** @param	type	Aiconfigtype
 ** @param	value	new value
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.6
 **/
void
Aiconfig::set(TypeCard const type, string const& value)
{
  this->set(type, Card(value));

  return ;
} // void Aiconfig::set(const TypeCard type, string const& value)


/**
 ** activates/deactivates the heuristic in the given context
 **
 ** @param	key         the context key
 ** @param	heuristic   the heuristic type
 ** @param	value       new value
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.3
 **/
void
Aiconfig::set(HeuristicsMap::Key const& key,
              Heuristic const heuristic,
              bool const value)
{
  this->set(key.gametype_group, key.playertype_group, heuristic, value);
  return ;
} // void Aiconfig::set(HeuristicsMap::GametypeGroup key, Heuristic heuristic, bool value)

/**
 ** activates/deactivates the heuristic in the given context
 **
 ** @param	gametype_group     the game type group
 ** @param	playertype_group   the player type group
 ** @param	heuristic          the heuristic type
 ** @param	value              new value
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.3
 **/
void
Aiconfig::set(HeuristicsMap::GametypeGroup const gametype_group,
              HeuristicsMap::PlayerTypeGroup const playertype_group,
              Heuristic const heuristic,
              bool const value)
{
  this->enter_update();
  vector<HeuristicState>&
    states = this->heuristic_states(gametype_group, playertype_group);

  for (vector<HeuristicState>::iterator s = states.begin();
       s != states.end();
       ++s)
    if (s->heuristic == heuristic) {
      s->active = value;
      return;
    }
  states.push_back(HeuristicState(heuristic, value));

  this->leave_update();

  return ;
} // void Aiconfig::set(HeuristicsMap::GametypeGroup gametype_group, HeuristicsMap::PlayerTypeGroup playertype_group, Heuristic heuristic, bool value)

/**
 ** sets the heuristics map to the default one
 **
 ** @param	key     the context key
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.3
 **/
void
Aiconfig::set_to_default(HeuristicsMap::Key const& key)
{
  this->enter_update();
  this->heuristic_states(key)
    = this->heuristic_states(HeuristicsMap::Key::DEFAULT);
  this->leave_update();

  return ;
} // void Aiconfig::set_to_default(HeuristicsMap::Key key)

/**
 ** moves the heuristic to the given position
 ** all heuristics below are moved one position further
 **
 ** @param	key         the key of the heuristic group
 ** @param	heuristic   the heuristic type
 ** @param	pos         new position
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.3
 **/
void
Aiconfig::move(HeuristicsMap::Key const& key,
               Heuristic const heuristic,
               unsigned const pos)
{
  vector<HeuristicState>&
    states = this->heuristic_states(key);

  if (states[pos].heuristic == heuristic)
    return ;

  if (pos > states.size())
    return ;

  this->enter_update();

  vector<HeuristicState>::iterator s;
  for (s = states.begin();
       s != states.end();
       ++s)
    if (s->heuristic == heuristic)
      break;

  if (s == states.end()) {
    states.insert(states.begin() + pos, HeuristicState(heuristic, false));
  } else { // if !(s == states.end())
    HeuristicState const state = *s;
    states.erase(s);
    states.insert(states.begin() + pos, state);
  } // if !(s == states.end())

  this->leave_update();

  return ;
} // void Aiconfig::move(HeuristicsMap::Key key, Heuristic heuristic, unsigned pos)

/**
 ** sets the aiconfig 'type' to the value 'value'
 **
 ** @param	type	heuristic type
 ** @param	value	new value
 **
 ** @result	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.6
 **/
void
Aiconfig::set(TypeBool const type, bool const value)
{
  this->enter_update();
  this->bool_p[type - BOOL_FIRST] = value;
  this->leave_update();

  return ;
} // void Aiconfig::set(TypeBool, bool);

/**
 ** sets the aiconfig 'type' to the value 'value'
 **
 ** @param	type	heuristic type
 ** @param	value	new value
 **
 ** @result	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.6
 **/
void
Aiconfig::set(TypeInt const type, int const value)
{
  this->enter_update();
  this->int_p[type - INT_FIRST] = value;
  this->leave_update();

  return ;
} // void Aiconfig::set(TypeInt type, int value);

/**
 ** sets the aiconfig 'type' to the value 'value'
 **
 ** @param	type	heuristic type
 ** @param	value	new value
 **
 ** @result	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.6
 **/
void
Aiconfig::set(TypeCard const type, Card const value)
{
  this->enter_update();
  this->card_p[type - CARD_FIRST] = value;
  this->leave_update();

  return ;
} // void Aiconfig::set(TypeCard, card);

/**
 **
 ** @param	ostr	output stream
 **
 ** @result	-
 **
 ** @author	Borg Enders
 ** @author	Diether Knof
 ** 
 ** @version	0.7.4
 **/
void
Aiconfig::write(ostream& ostr) const
{
  if (this->difficulty() != CUSTOM) {
    if (Aiconfig::write_standard_type) {
      ostr << "difficulty = " << ::name(this->difficulty()) << '\n';
      return ;
    } else {
      ostr << "# difficulty = " << ::name(this->difficulty()) << '\n';
    }
  } // if (this->difficulty() != CUSTOM)

  std::ios_base::fmtflags const flags = ostr.flags();
  ostr << std::boolalpha;

#ifdef ALTERNATIVE
  // Do not use, since it will lead to problems when replaying bug reports.
  { // Only write the differences to a standard type.
    // Count the differences to the standard types.
    // Write only the differences if they are less than MAX_DIFFERENCES_TO_WRITE.
    unsigned const MAX_DIFFERENCES_TO_WRITE = 0;

    // the difficulty with the minimal differences
    Difficulty difficulty_min = CUSTOM;
    // the minimal differences
    unsigned differences_min = UINT_MAX;
    // the string with the differences
    string differences_min_string;

    for (int difficulty = DIFFICULTY_FIRST;
         difficulty <= DIFFICULTY_LAST;
         difficulty++) {
      if (difficulty == CUSTOM)
        continue;

      Aiconfig const& preset_aiconfig
        = Aiconfig::preset(static_cast<Difficulty>(difficulty));
      // the number of differences
      unsigned differences = 0;
      // output stream for the differences
      ostringstream differences_ostr;

      { // check the values
        for (unsigned t = BOOL_FIRST; t <= BOOL_LAST; ++t) {
          if (this->value(static_cast<TypeBool>(t))
              != preset_aiconfig.value(static_cast<TypeBool>(t))) {
            differences += 1;
            differences_ostr
              << "  "
              << static_cast<TypeBool>(t) << " = "
              << (this->value(static_cast<TypeBool>(t)) ? "true" : "false")
              << '\n'
              << "# "
              << static_cast<TypeBool>(t) << " = "
              << (preset_aiconfig.value(static_cast<TypeBool>(t))
                  ? "true" : "false")
              << '\n';
          } // if (value differs)
        } // for (t \in TypeBool)
        for (unsigned t = INT_FIRST; t <= INT_LAST; ++t) {
          if (this->value(static_cast<TypeInt>(t))
              != preset_aiconfig.value(static_cast<TypeInt>(t))) {
            differences += 1;
            differences_ostr
              << "  "
              << static_cast<TypeInt>(t) << " = "
              << this->value(static_cast<TypeInt>(t))
              << '\n'
              << "# "
              << static_cast<TypeInt>(t) << " = "
              << preset_aiconfig.value(static_cast<TypeInt>(t))
              << '\n';
          } // if (value differs)
        } // for (t \in TypeInt)
        for (unsigned t = CARD_FIRST; t <= CARD_LAST; ++t) {
          if (this->value(static_cast<TypeCard>(t))
              != preset_aiconfig.value(static_cast<TypeCard>(t))) {
            differences += 1;
            differences_ostr
              << "  "
              << static_cast<TypeCard>(t) << " = "
              << this->value(static_cast<TypeCard>(t))
              << '\n'
              << "# "
              << static_cast<TypeCard>(t) << " = "
              << preset_aiconfig.value(static_cast<TypeCard>(t))
              << '\n';
          } // if (value differs)
        } // for (t \in TypeCard)
      } // check the values
      if (   (differences >= MAX_DIFFERENCES_TO_WRITE)
          || (differences >= differences_min))
        continue;
      { // check the ai types
        for (unsigned t = 0;
             t < ::party.rule()(Rule::MAX_NUMBER_OF_TRICKS_IN_GAME);
             t++) {
          if (this->aitype(t) != preset_aiconfig.aitype(t)) {
            differences += 1;
            differences_ostr
              << "  "
              << "type         = " << setw(2) << t << ": "
              << this->aitype(t) << '\n'
              << "# "
              << "type         = " << setw(2) << t << ": "
              << preset_aiconfig.aitype(t) << '\n';
          } // if (value differs)
        } // for (t \in number of tricks)
        for (unsigned t = 0;
             t < ::party.rule()(Rule::MAX_NUMBER_OF_TRICKS_IN_GAME);
             t++) {
          if (this->rating(t) != preset_aiconfig.rating(t)) {
            differences += 1;
            differences_ostr
              << "  "
              << "rating       = " << setw(2) << t << ": "
              << this->rating(t) << '\n'
              << "# "
              << "rating       = " << setw(2) << t << ": "
              << preset_aiconfig.rating(t) << '\n';
          } // if (value differs)
        } // for (t \in number of tricks)
        for (unsigned t = 0;
             t < ::party.rule()(Rule::MAX_NUMBER_OF_TRICKS_IN_GAME);
             t++) {
          if (this->future_limit(t) != preset_aiconfig.future_limit(t)) {
            differences += 1;
            differences_ostr
              << "  "
              << "future limit = " << setw(2) << t << ": "
              << this->future_limit(t) << '\n'
              << "# "
              << "future limit = " << setw(2) << t << ": "
              << preset_aiconfig.future_limit(t) << '\n';
          } // if (value differs)
        } // for (t \in number of tricks)
      } // check the ai types
      if (   (differences >= MAX_DIFFERENCES_TO_WRITE)
          || (differences >= differences_min))
        continue;
      { // check the heuristics
        vector<HeuristicsMap::Key> const& keys = Aiconfig::keys();
        bool first_difference = true;

        for (vector<HeuristicsMap::Key>::const_iterator key = keys.begin();
             key != keys.end();
             ++key) {
          if (this->heuristic_states(*key)
              != preset_aiconfig.heuristic_states(*key)) {
            differences += 5;


            { // write the differences
              if (first_difference) {
                differences_ostr << "heuristics" << '\n'
                  << "{\n";
                first_difference = false;
              } // if (first_difference)

              differences_ostr
                << "  "
                << key->gametype_group << " - "
                << key->playertype_group << '\n'
                << "  {\n";
              for (vector<HeuristicState>::const_iterator
                   h = this->heuristic_states(*key).begin();
                   h != this->heuristic_states(*key).end();
                   ++h)
                differences_ostr
                  << "    "
                  << h->heuristic << " = "
                  << (h->active ? "true" : "false") << '\n';
              differences_ostr << "  }\n";
            } // write the differences
          } // if (heuristic states differ)
        } // for (key \in keys)

        if (!first_difference) {
          // there was a difference
          differences_ostr << "}\n";
        } // if (!first_difference)

      } // check the heuristics

      if (differences < differences_min) {
        // has found a 'nearer' difficulty
        difficulty_min = static_cast<Difficulty>(difficulty);
        differences_min = differences;
        differences_min_string = differences_ostr.str();
      } // if (differences < differences_min)
    } // for (difficulty)

    if (differences_min <= MAX_DIFFERENCES_TO_WRITE) {
      // write only the differences

      // write the nearest difficulty and differences
      ostr << "difficulty = " << ::name(difficulty_min) << '\n';
      ostr << "# differences = " << differences_min << '\n';
      ostr << '\n' << differences_min_string;

      return ;
    } // if (difficulty_min <= MAX_DIFFERENCES_TO_WRITE)
  } // only write the differences to a standard type
#endif // #ifndef RELEASE

  for (unsigned i = 0; i < this->aitype_p.size(); ++i)
    ostr << "type         = " << setw(2) << i << ": "
      << this->aitype(i) << '\n';
  for (unsigned i = 0; i < this->aitype_p.size(); ++i)
    ostr << "future limit = " << setw(2) << i << ": "
      << this->future_limit(i) << '\n';
  for (unsigned i = 0; i < this->aitype_p.size(); ++i)
    ostr << "rating       = " << setw(2) << i << ": "
      << this->rating(i) << '\n';
  ostr << '\n';
  ostr << '\n';

  ostr << "# bool\n";
  for (int t = BOOL_FIRST; t <= BOOL_LAST; ++t)
    ostr << static_cast<TypeBool>(t) << " = "
      << (this->value(static_cast<TypeBool>(t)) ? "true" : "false") << '\n';
  ostr << '\n';

  ostr << "# unsigned\n";
  for (int t = INT_FIRST; t <= INT_LAST; ++t)
    ostr << static_cast<TypeInt>(t) << " = "
      << this->value(static_cast<TypeInt>(t)) << '\n';
  ostr << '\n';

  ostr << "# card\n";
  for (int t = CARD_FIRST; t <= CARD_LAST; ++t)
    ostr << static_cast<TypeCard>(t) << " = "
      << this->value(static_cast<TypeCard>(t)) << '\n';

  ostr << '\n';
  ostr << "#heuristics" << '\n';
  ostr << "heuristics" << '\n'
    << "{\n";
  for (map<HeuristicsMap::Key, vector<HeuristicState> >::const_iterator
       s = this->heuristic_states_.begin();
       s != this->heuristic_states_.end();
       ++s) {
    ostr << "  "
      << s->first.gametype_group << " - " << s->first.playertype_group << '\n'
      << "  {\n";
    for (vector<HeuristicState>::const_iterator h = s->second.begin();
         h != s->second.end();
         ++h)
      ostr << "    "
        << h->heuristic << " = " << (h->active ? "true" : "false") << '\n';
    ostr << "  }\n";
  } // for (s \in this->heuristic_states_)
  ostr << "}\n";

  ostr.flags(flags);

  return ;
} // void Aiconfig::write(ostream& ostr) const

/**
 ** loads the infos from the file
 **
 ** @param	filename	file to load the aiconfig from
 **
 ** @return	whether the loading was successful
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.6
 **/
bool
Aiconfig::load(string const& filename)
{
  {
    ifstream istr(filename.c_str());
    if (istr.good()) {
      this->reset_to_hardcoded();
      return this->read(istr);
    }
  }
  {
    ifstream istr((::setting(Setting::PRIVATE_DATA_DIRECTORY)
                   + "/" + ::setting(Setting::AI_DIRECTORY)
                   + "/" + filename).c_str());
    if (istr.good()) {
      this->reset_to_hardcoded();
      return this->read(istr);
    }
  }
  {
    ifstream istr((::setting(Setting::PUBLIC_DATA_DIRECTORY)
                   + "/" + ::setting(Setting::AI_DIRECTORY)
                   + "/" + filename).c_str());
    if (istr.good()) {
      this->reset_to_hardcoded();
      return this->read(istr);
    }
  }

  return false;
} // bool Aiconfig::load(string const& filename)

/**
 ** reads the infos from the stream
 **
 ** @param	istr	stream with the infos
 **
 ** @return	whether the reading was successful
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
bool
Aiconfig::read(istream& istr)
{
  this->enter_update();

  unsigned depth = 0;

  this->reset_to_hardcoded();

  // read the configuration
  while (istr.good()) {
    Config config;
    istr >> config;
    if (!istr.good())
      break;

    // finished with the config file
    if ((config.name == "") && (config.value == ""))
      break;

    if (config.separator) {
      // a setting
      if (config.name == "difficulty") {
        // a preset difficulty
        int difficulty;
        for (difficulty = DIFFICULTY_FIRST;
             difficulty <= DIFFICULTY_LAST;
             difficulty++) {
          if (config.value == ::name(static_cast<Difficulty>(difficulty))) {
            this->set_to_difficulty(static_cast<Difficulty>(difficulty));
            break;
          }
        }
        if (difficulty > DIFFICULTY_LAST)
          cerr << "Aiconfig::read()\n"
            << "  unknown difficulty '" << config.value << "'"
            << endl;
      } else if (config.name == "type") {
        try {
          stringstream istr(config.value);

          // tricknumber
          unsigned trickno;
          istr >> trickno;
          if (!istr.good()) {
            throw config.value;
          }

          // separator
          if (istr.peek() != ':')
            throw config.value;
          istr.get();

          while (std::isspace(istr.peek())
                 && istr.good())
            istr.get();

          if (!istr.good())
            throw config.value;

          // aitype
          string aitype;
          std::getline(istr, aitype);
          this->set_aitype(trickno, aitype_from_name(aitype));
        } catch (string value) {
          cerr << "aiconfig: "
            << "unknown aitype '" << value << "'."
            << endl;
        } // try
      } else if (config.name == "future limit") {
        try {
          stringstream istr(config.value);

          // tricknumber
          unsigned trickno;
          istr >> trickno;
          if (!istr.good()) {
            throw config.value;
          }

          // separator
          if (istr.peek() != ':') {
            throw config.value;
          }
          istr.get();

          while (std::isspace(istr.peek())
                 && istr.good())
            istr.get();

          if (!istr.good()) {
            throw config.value;
          }

          // future limit
          unsigned future_limit;
          istr >> future_limit;
          if (istr.fail()) {
            throw config.value;
          }

          this->set_future_limit(trickno, future_limit);
        } // try
        catch (string value) {
          cerr << "aiconfig: "
            << "illegal future limit '" << value << "'."
            << endl;
        }
      } else if (config.name == "rating") {
        try {
          stringstream istr(config.value);

          // tricknumber
          unsigned trickno;
          istr >> trickno;
          if (!istr.good()) {
            throw config.value;
          }

          // separator
          if (istr.peek() != ':') {
            throw config.value;
          }
          istr.get();

          while (std::isspace(istr.peek())
                 && istr.good())
            istr.get();

          if (!istr.good()) {
            throw config.value;
          }

          // rating
          string rating;
          std::getline(istr, rating);

          {
            int t;
            for (t = Rating::TYPE_FIRST; t <= Rating::TYPE_LAST; t++)
              if (rating == ::name(Rating::Type(t))) {
                this->set_rating(trickno, Rating::Type(t));
                break;
              }
            if (t > Rating::TYPE_LAST) {
              throw config.value;
            }
          }

        } // try
        catch (string value) {
          cerr << "rating type: "
            << "unknown rating type '" << value << "'."
            << endl;
        }
      } else {
        if (!this->set(config.name, config.value)) {
          cerr << "aiconfig: "
            << "unknown config '" << config.name << "'."
            << endl;
        }
      } // if (config.name == "type")
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
          cerr << "Aiconfig: found a '}' without a '{' before.\n"
            << "Finish reading the the file."
            << endl;
          break;
        } // if (depth == 0)
        depth -= 1;
        if (depth == 0)
          break;
      } else if(config.name == "heuristics") {
        this->read_heuristics(istr);
      } else if(config.name == "") {
        cerr << "Aiconfig: "
          << "Ignoring line \'" << config.value << "\'.\n";
      } else {
        cerr << "Aiconfig: "
          << "type '" << config.name << "' unknown.\n"
          << "Ignoring it.\n";
      } // if (config.name == .)
    } // config.separator
  } // while (istr.good())

  this->leave_update();
  return istr.good();
} // bool Aiconfig::read(istream& istr)

/**
 ** reads the heuristics
 **
 ** @param	istr   input stream
 **
 ** @return	whether the reading was successful
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.3
 **/
bool
Aiconfig::read_heuristics(istream& istr)
{
  this->enter_update();
  unsigned depth = 0;
  string line;
  while (istr.good()) {
    std::getline(istr, line);
    if (!istr.good())
      break;

    while (   !line.empty()
           && isspace(*line.begin()))
      line.erase(line.begin());

    if (*(line.end() - 1) == '\r')
      line.erase(line.end() - 1);

    if (line.empty())          { // empty line
    } else if (line[0] == '#') { // comment
    } else if (line == "{")    { // block open
      depth += 1;
    } else if (line == "}")    { // block close 
      if (depth == 0) {
        cerr << "Aiconfig: found a '}' without a '{' before.\n"
          << "Finish reading the the file."
          << endl;
        break;
      } // if (depth == 0)
      depth -= 1;
      if (depth == 0)
        break;
    } else                     { // gametype group - playertype group
      string::size_type const pos = line.find('-');
      if (pos == string::npos) {
        cerr << "unknown line '" << line << "'\n"
          << "ignoring it" << endl;
        continue;
      } // if (pos == string::npos)

      string gametype_group_string(line, 0, pos);
      while (   !gametype_group_string.empty()
             && isspace(*gametype_group_string.rbegin()))
        gametype_group_string.erase(gametype_group_string.size() - 1);
      string playertype_group_string(line, pos + 1, string::npos);
      while (   !playertype_group_string.empty()
             && isspace(*playertype_group_string.begin()))
        playertype_group_string.erase(playertype_group_string.begin());

      HeuristicsMap::GametypeGroup const gametype_group
        = HeuristicsMap::GametypeGroup_from_name(gametype_group_string);
      HeuristicsMap::PlayerTypeGroup const playertype_group
        = HeuristicsMap::PlayerTypeGroup_from_name(playertype_group_string);

      unsigned depth2 = 0;
      vector<HeuristicState>& states
        = this->heuristic_states_[HeuristicsMap::Key(gametype_group,
                                                     playertype_group)];
      states.clear();
      while (istr.good()) {
        Config config;
        istr >> config;
        if (!istr.good())
          break;

        if (config.separator) {
          Heuristic const heuristic = Heuristic_from_name(config.name);
          if (is_real(heuristic)) {
            bool const active = (   (config.value == "true")
                                 || (config.value == "yes")
                                 || (config.value == "1"));
            states.push_back(HeuristicState(heuristic, active));
          } // if (is_real(heuristic))
        } else { // if (config.separator)
          if(config.name == "{") {
            depth2 += 1;
          } else if(config.name == "}") {
            if (depth2 == 0) {
              cerr << "Aiconfig: found a '}' without a '{' before.\n"
                << "Finish reading the the file."
                << endl;
              break;
            } // if (depth2 == 0)
            depth2 -= 1;
            if (depth2 == 0)
              break;
          } else if(config.name == "") {
            cerr << "Aiconfig: "
              << "Ignoring line \'" << config.value << "\'.\n";
          } else {
            cerr << "Aiconfig: "
              << "type '" << config.name << "' unknown.\n"
              << "Ignoring it.\n";
          } // if (config.name == .)
        } // config.separator
      } // while (istr.good())
    } // if (line == ...)
  } // while (istr.good())

  this->fill_up_heuristic_states();

  this->leave_update();
  return istr.good();
} // bool Aiconfig::read_heuristics(istream& istr)

/**
 ** save the infos in the file
 **
 ** @param	filename	file to save the aiconfig
 **
 ** @return	whether the saving was successful
 **
 ** @author	Diether Knof
 **
 ** @version	0.5.4
 **/
bool
Aiconfig::save(string const& filename) const
{
  string filename_tmp = filename + ".tmp";
  ofstream ostr(filename_tmp.c_str());
  if (!ostr.good()) {
    ::ui->information(::translation("Error::Aiconfig::save: Error opening temporary file %stmpfile%. Aborting saving.", filename_tmp), INFORMATION::PROBLEM);
    return false;
  }

  ostr << "# FreeDoko ai configuration (" << *::version << ")\n"
    << '\n';
  bool const write_standard_type_bak = Aiconfig::write_standard_type;
  Aiconfig::write_standard_type = false;
  this->write(ostr);
  Aiconfig::write_standard_type = write_standard_type_bak;

  if (!ostr.good()) {
    ::ui->information(::translation("Error::Aiconfig::save: Error saving in temporary file %stmpfile%. Keeping temporary file (for bug tracking).", filename_tmp), INFORMATION::PROBLEM);
    return false;
  }
  ostr.close();

#ifdef WINDOWS
  unlink(filename.c_str());
#endif
  if (rename(filename_tmp.c_str(), filename.c_str())) {
    ::ui->information(::translation("Error::Aiconfig::save: Could not rename temporary file %stmpfile% to requested file %sfile%. Keeping temporary file.", filename_tmp, filename), INFORMATION::PROBLEM);
    return false;
  }

  return true;
} // bool Aiconfig::save(string const& filename) const

/**
 ** comparison of two aiconfigs
 **
 ** @param	aiconfig_a   first aiconfig
 ** @param	aiconfig_b   second aiconfig
 **
 ** @return	whether the two aiconfigs are equal
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.2
 **/
bool
operator==(Aiconfig const& aiconfig_a, Aiconfig const& aiconfig_b)
{
  return aiconfig_a.equal(aiconfig_b);
} // bool operator==(Aiconfig aiconfig_a, Aiconfig aiconfig_b)

/**
 ** comparison of two aiconfigs
 **
 ** @param	aiconfig_a   first aiconfig
 ** @param	aiconfig_b   second aiconfig
 **
 ** @return	whether the two aiconfigs are different
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.2
 **/
bool
operator!=(Aiconfig const& aiconfig_a, Aiconfig const& aiconfig_b)
{
  return !(aiconfig_a == aiconfig_b);
} // bool operator!=(Aiconfig aiconfig_a, Aiconfig aiconfig_b)

/**
 ** comparison of two heuristic states
 **
 ** @param	heuristic_state_a   first heuristic state
 ** @param	heuristic_state_b   second heuristic state
 **
 ** @return	whether the two heuristic states are equal
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.2
 **/
bool
operator==(Aiconfig::HeuristicState const& heuristic_state_a,
           Aiconfig::HeuristicState const& heuristic_state_b)
{
  return (   (heuristic_state_a.heuristic
              == heuristic_state_b.heuristic)
          && (heuristic_state_a.active
              == heuristic_state_b.active) );
} // bool operator==(Aiconfig::HeuristicState heuristic_state_a, Aiconfig::HeuristicState heuristic_state_b)

/**
 ** -> result
 **
 ** @param	heuristic   heuristic type
 **
 ** @return	the name of 'type'
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.8
 **/
string
name(Aiconfig::Heuristic const heuristic)
{
  switch(heuristic) {
  case Aiconfig::NO_HEURISTIC:
    return "no heuristic";
  case Aiconfig::MANUAL:
    return "manual";
  case Aiconfig::BUG_REPORT:
    return "bug report";
  case Aiconfig::NETWORK:
    return "network";
  case Aiconfig::ONLY_VALID_CARD:
    return "only valid card";
  case Aiconfig::PLAY_TO_MARRY:
    return "play to marry";
  case Aiconfig::PLAY_TO_GET_MARRIED:
    return "play to get married";
  case Aiconfig::CHOOSE_ACE:
    return "choose ace";
  case Aiconfig::CHOOSE_TEN:
    return "choose ten";
  case Aiconfig::CHOOSE_SINGLE_ACE:
    return "choose single ace";
  case Aiconfig::CHOOSE_FOR_COLOR_TRICK:
    return "choose for color trick";
  case Aiconfig::JAB_COLOR_OVER_FOX:
    return "jab color over fox";
  case Aiconfig::START_WITH_COLOR:
    return "start with color";
  case Aiconfig::START_WITH_LOW_COLOR:
    return "start with low color";
  case Aiconfig::START_WITH_LOW_TRUMP:
    return "start with low trump";
  case Aiconfig::RETRY_COLOR:
    return "retry color";
  case Aiconfig::PLAY_COLOR_FOR_PARTNER:
    return "play color for partner";
  case Aiconfig::TRY_COLOR_FOR_PARTNER:
    return "try color for partner";
  case Aiconfig::PLAY_COLOR_FOR_PARTNER_ACE:
    return "play color for partner ace";
  case Aiconfig::PLAY_BAD_COLOR:
    return "play bad color";
  case Aiconfig::SERVE_COLOR_TRICK:
    return "serve color trick";
  case Aiconfig::SERVE_TRUMP_TRICK:
    return "serve trump trick";
  case Aiconfig::CHOOSE_PFUND:
    return "choose pfund";
  case Aiconfig::CHOOSE_PFUND_POVERTY:
    return "choose pfund poverty";
  case Aiconfig::CHOOSE_PFUND_BEFORE_PARTNER:
    return "choose pfund before partner";
  case Aiconfig::JAB_FOR_ACE:
    return "jab for ace";
  case Aiconfig::CREATE_FEHL:
    return "create fehl";
  case Aiconfig::BEST_WINNING:
    return "best winning card";
  case Aiconfig::LOW_HIGH:
    return "play low high";
  case Aiconfig::PLAY_TRUMP:
    return "play trump";
  case Aiconfig::PLAY_FOR_TEAM:
    return "play for team";
  case Aiconfig::JAB_FOX:
    return "jab fox";
  case Aiconfig::TRY_FOR_DOPPELKOPF:
    return "try for doppelkopf";
  case Aiconfig::PLAY_FOR_PARTNER_WORRIES:
    return "play for partner worries";
  case Aiconfig::PARTNER_BACKHAND_DRAW_TRUMP:
    return "partner backhand draw trump";
  case Aiconfig::DRAW_TRUMP:
    return "draw trump";
  case Aiconfig::PLAY_TO_JAB_LATER:
    return "play to jab later";
  case Aiconfig::PLAY_HIGHEST_COLOR_CARD_IN_GAME:
    return "play highest color card in game";
  case Aiconfig::GET_TRICK_FOR_ANNOUNCEMENT:
    return "get trick for announcement";
  case Aiconfig::GRAB_TRICK:
    return "grab trick";
  case Aiconfig::LET_TRICK_PASS:
    return "let trick pass";

  case Aiconfig::POVERTY_SPECIAL_PLAY_PFUND:
    return "poverty: special: play pfund";
  case Aiconfig::POVERTY_SPECIAL_GIVE_NO_POINTS:
    return "poverty: special: give no points";
  case Aiconfig::POVERTY_SPECIAL_OFFER_PFUND:
    return "poverty: special: offer pfund";
  case Aiconfig::POVERTY_RE_TRUMP_COLOR_TRICK_HIGH:
    return "poverty: re: trump color trick high";
  case Aiconfig::POVERTY_RE_PLAY_TRUMP:
    return "poverty: re: play trump";
  case Aiconfig::POVERTY_CONTRA_PLAY_COLOR:
    return "poverty: contra: play color";
  case Aiconfig::POVERTY_CONTRA_TRUMP_COLOR_TRICK_HIGH:
    return "poverty: contra: trump color trick high";
  case Aiconfig::POVERTY_LEAVE_TO_PARTNER:
    return "poverty: contra: leave to partner";
  case Aiconfig::POVERTY_OVERJAB_RE:
    return "poverty: contra: overjab re";
  case Aiconfig::POVERTY_BEST_WINNING_CARD:
    return "poverty: best winning card";

  case Aiconfig::PLAY_COLOR_IN_SOLO:
    return "play color in solo";
  case Aiconfig::MEATLESS_PLAYHIGHESTCOLOR:
    return "meatless: play highest color";
  case Aiconfig::PICTURE_SECONDBESTTRUMP:
    return "picture: second best trump";
  case Aiconfig::COLOR_JAB_FOR_ACE:
    return "color: jab for ace";
  case Aiconfig::COLOR_LOW_HIGH:
    return "color: play low high";
  case Aiconfig::CHOOSE_BEST_CARD:
    return "choose best card";
  } // switch(heuristic)

  return "";
} // string name(Aiconfig::Heuristic heuristic)

/**
 ** -> result
 **
 ** @param	name   name of the heuristic
 **
 ** @return	the heuristic according to the name
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.3
 **/
Aiconfig::Heuristic
Aiconfig::Heuristic_from_name(string const& name)
{
#ifdef WORKAROUND
  // name till 0.7.5
  if (name == "color: choose ace")
    return CHOOSE_ACE;
#endif
#ifdef WORKAROUND
  // name till 0.7.10
  if (name == "color: best winning")
    return BEST_WINNING;
#endif
  for (int h = HEURISTIC_FIRST; h <= HEURISTIC_LAST; ++h)
    if (name == ::name(static_cast<Heuristic>(h)))
      return static_cast<Heuristic>(h);

  return NO_HEURISTIC;
} // static Aiconfig::Heuristic Aiconfig::Heuristic_from_name(string name)

/**
 ** -> result
 **
 ** @param	difficulty    difficulty
 **
 ** @return	the name of 'difficulty'
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.2
 **/
string
name(Aiconfig::Difficulty const difficulty)
{
  switch(difficulty) {
  case Aiconfig::CUSTOM:
    return "custom";
  case Aiconfig::NOVICE:
    return "novice";
  case Aiconfig::STANDARD_DEFENSIVE:
    return "standard defensive";
  case Aiconfig::STANDARD_OFFENSIVE:
    return "standard offensive";
  case Aiconfig::PROFI:
    return "profi";
  case Aiconfig::PROFI_AGGRESSIVE:
    return "profi aggressive";
  case Aiconfig::PROFI_UNFAIR:
    return "profi unfair";
  } // switch(difficulty)

  return "";
} // string name(Aiconfig::Difficulty difficulty)

/**
 ** -> result
 **
 ** @param	type	bool type
 **
 ** @return	the name of 'type'
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.6
 **/
string
name(Aiconfig::TypeBool const type)
{
  switch(type) {
  case Aiconfig::TRUSTING:
    return "trusting";
  case Aiconfig::AGGRESSIVE:
    return "aggressive";
  case Aiconfig::TEAMS_KNOWN:
    return "teams known";
  case Aiconfig::HANDS_KNOWN:
    return "hands known";
  case Aiconfig::ESTIMATE_HANDS:
    return "estimate hands";
  case Aiconfig::FEHLCREATIONONFIRSTCARD:
    return "first card fehl creation";
  } // switch(type)

  return "";
} // string name(Aiconfig::TypeBool type)

/**
 ** -> result
 **
 ** @param	type	unsigned type
 **
 ** @return	the name of 'type'
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.6
 **/
string
name(Aiconfig::TypeInt const type)
{
  switch(type) {
  case Aiconfig::REMEMBERTRICKS:
    return "remember tricks";
  case Aiconfig::LIMIT_THROW_FEHL:
    return "limit throw fehl";
  case Aiconfig::LIMITQUEEN:
    return "limit queen";
  case Aiconfig::LIMITDOLLE:
    return "limit dolle";
  case Aiconfig::LASTFEHLCREATION:
    return "last fehlcreation";
  case Aiconfig::LAST_TRICKS_WITHOUT_HEURISTICS:
    return "last tricks without heuristics";
  case Aiconfig::FIRST_TRICK_FOR_TRUMP_POINTS_OPTIMIZATION:
    return "first trick for trump points optimization";
  case Aiconfig::ANNOUNCELIMIT:
    return "announce limit";
  case Aiconfig::ANNOUNCELIMITDEC:
    return "announce limit decrement";
  case Aiconfig::ANNOUNCECONFIG:
    return "announce config";
  case Aiconfig::ANNOUNCELIMITREPLY:
    return "announce limit reply";
  case Aiconfig::ANNOUNCECONFIGREPLY:
    return "announce config reply";

  case Aiconfig::TAKEPOVERTY:
    return "take poverty";
  case Aiconfig::SINGLESOLO:
    return "handvalue single solo";
  case Aiconfig::DOUBLESOLO:
    return "handvalue double solo";
  case Aiconfig::TRIPLESOLO:
    return "handvalue trible solo";
  case Aiconfig::COLORSOLO:
    return "handvalue color solo";
  case Aiconfig::MEATLESS:
    return "handvalue meatless";
  } // switch(type)

  return "";
} // string name(Aiconfig::TypeInt type)

/**
 ** -> result
 **
 ** @param	type	card type
 **
 ** @return	the name of 'type'
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.6
 **/
string
name(Aiconfig::TypeCard const type)
{
  switch(type) {
  case Aiconfig::LIMITTHROWING:
    return "limit throwing";
  case Aiconfig::LIMITHIGH:
    return "limit high";
  case Aiconfig::TRUMPLIMIT_SOLOCOLOR:
    return "trump limit color";
  case Aiconfig::TRUMPLIMIT_SOLOJACK:
    return "trump limit jack";
  case Aiconfig::TRUMPLIMIT_SOLOQUEEN:
    return "trump limit queen";
  case Aiconfig::TRUMPLIMIT_SOLOKING:
    return "trump limit king";
  case Aiconfig::TRUMPLIMIT_SOLOJACKKING:
    return "trump limit jack king";
  case Aiconfig::TRUMPLIMIT_SOLOJACKQUEEN:
    return "trump limit jack queen";
  case Aiconfig::TRUMPLIMIT_SOLOQUEENKING:
    return "trump limit queen king";
  case Aiconfig::TRUMPLIMIT_SOLOKOEHLER:
    return "trump limit koehler";
  case Aiconfig::TRUMPLIMIT_MEATLESS:
    return "trump limit meatless";
  case Aiconfig::TRUMPLIMIT_NORMAL:
    return "trump limit normal";
  case Aiconfig::LOWEST_TRUMPLIMIT_SOLOCOLOR:
    return "lowest trump limit color";
  case Aiconfig::LOWEST_TRUMPLIMIT_SOLOJACK:
    return "lowest trump limit jack";
  case Aiconfig::LOWEST_TRUMPLIMIT_SOLOQUEEN:
    return "lowest trump limit queen";
  case Aiconfig::LOWEST_TRUMPLIMIT_SOLOKING:
    return "lowest trump limit king";
  case Aiconfig::LOWEST_TRUMPLIMIT_SOLOJACKKING:
    return "lowest trump limit jack king";
  case Aiconfig::LOWEST_TRUMPLIMIT_SOLOJACKQUEEN:
    return "lowest trump limit jack queen";
  case Aiconfig::LOWEST_TRUMPLIMIT_SOLOQUEENKING:
    return "lowest trump limit queen king";
  case Aiconfig::LOWEST_TRUMPLIMIT_SOLOKOEHLER:
    return "lowest trump limit koehler";
  case Aiconfig::LOWEST_TRUMPLIMIT_MEATLESS:
    return "lowest trump limit meatless";
  case Aiconfig::LOWEST_TRUMPLIMIT_NORMAL:
    return "lowest trump limit normal";
  } // switch(type)

  return "";
} // string name(Aiconfig::TypeCard type)

/**
 ** writes the aiconfig into the output stream
 **
 ** @param	ostr       stream to write in
 ** @param	aiconfig   aiconfig to write
 **
 ** @return	output stream
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.6
 **/
ostream&
operator<<(ostream& ostr, Aiconfig const& aiconfig)
{
  aiconfig.write(ostr);

  return ostr;
} // ostream& operator<<(ostream& ostr, const Aiconfig& aiconfig)

/**
 ** -> result
 **
 ** @param	aitype	ai type
 **
 ** @return	the name of 'aitype'
 **/
string
name(AiType const aitype)
{
  switch(aitype) {
  case AiType::NO_CHOOSEBESTCARD:
    return "no choosebestcard";
  case AiType::RANDOM:
    return "random";
  case AiType::VIRTUAL_GAMES:
    return "virtual games";
  case AiType::GAMETREE:
    return "gametree";
  case AiType::GAMETREE_WITH_HEURISTICS:
    return "gametree with heuristics";
  case AiType::GAMETREE_FOR_TEAM:
    return "gametree for team";
  case AiType::MONTE_CARLO:
    return "monte carlo";
  case AiType::MONTE_CARLO_JAB_OR_SERVE:
    return "monte carlo - jab or serve";
  } // switch(aitype)

  return "";
} // string name(AiType aitype)

/**
 ** -> result
 **
 ** @param	name   name of the aitype
 **
 ** @return	the aitype
 **/
AiType
aitype_from_name(string const& name)
{
  for (auto a : AiTypeALL)
    if (::name(a) == name)
      return a;
  throw name;
  return AiType::NO_CHOOSEBESTCARD;
} // AiType aitype_from_name(string name)

/**
 ** writes the aitype into the output stream
 **
 ** @param    ostr    output stream
 ** @param    aitype  aitype
 **
 ** @return   output stream
 **/
ostream&
operator<<(ostream& ostr, AiType const aitype)
{
  return (ostr << ::name(aitype));
}
