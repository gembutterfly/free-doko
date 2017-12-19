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
 **********************************************************************/

#pragma once

#include "../card/card.h"
#include "heuristicsmap.h"
#include "rating/rating.h"

#include "../misc/translation.h"

#include <map>

enum class AiType {
  NO_CHOOSEBESTCARD,
  RANDOM,
  VIRTUAL_GAMES,
  GAMETREE,
  GAMETREE_WITH_HEURISTICS,
  GAMETREE_FOR_TEAM,
  MONTE_CARLO,
  MONTE_CARLO_JAB_OR_SERVE
}; // enum AiType
constexpr AiType AiTypeALL[] = {
  AiType::NO_CHOOSEBESTCARD,
  AiType::RANDOM,
  AiType::VIRTUAL_GAMES,
  AiType::GAMETREE,
  AiType::GAMETREE_WITH_HEURISTICS,
  AiType::GAMETREE_FOR_TEAM,
  AiType::MONTE_CARLO,
  AiType::MONTE_CARLO_JAB_OR_SERVE
};
AiType aitype_from_name(string const& name);

string name(AiType aitype);
ostream& operator<<(ostream& ostr, AiType aitype);

class Aiconfig {
  public:
    // whether to write the standard type (all the whole configuration)
    static bool write_standard_type;

  public:
    enum Difficulty {
      DIFFICULTY_FIRST = 0,

      CUSTOM = DIFFICULTY_FIRST,
      NOVICE,
      STANDARD_DEFENSIVE,
      STANDARD_OFFENSIVE,
      PROFI,
      PROFI_AGGRESSIVE,
      PROFI_UNFAIR,

      DIFFICULTY_LAST = PROFI_UNFAIR
    }; // enum Difficulty

    enum Heuristic {
      HEURISTIC_FIRST,

      NO_HEURISTIC = HEURISTIC_FIRST,
      MANUAL,
      BUG_REPORT,
      NETWORK,
      ONLY_VALID_CARD,
      PLAY_TO_MARRY,
      PLAY_TO_GET_MARRIED,
      CHOOSE_ACE,
      CHOOSE_TEN,
      CHOOSE_SINGLE_ACE,
      CHOOSE_FOR_COLOR_TRICK,
      JAB_COLOR_OVER_FOX,

      START_WITH_COLOR,
      START_WITH_LOW_COLOR,
      START_WITH_LOW_TRUMP,
      RETRY_COLOR,
      PLAY_COLOR_FOR_PARTNER,
      TRY_COLOR_FOR_PARTNER,
      PLAY_COLOR_FOR_PARTNER_ACE,
      PLAY_BAD_COLOR,
      SERVE_COLOR_TRICK,
      SERVE_TRUMP_TRICK,
      CHOOSE_PFUND,
      CHOOSE_PFUND_POVERTY,
      CHOOSE_PFUND_BEFORE_PARTNER,
      JAB_FOR_ACE,
      CREATE_FEHL,
      BEST_WINNING,
      LOW_HIGH,
      PLAY_TRUMP,
      PLAY_FOR_TEAM,
      JAB_FOX,
      TRY_FOR_DOPPELKOPF,
      PLAY_FOR_PARTNER_WORRIES,
      PARTNER_BACKHAND_DRAW_TRUMP,
      DRAW_TRUMP,
      PLAY_TO_JAB_LATER,
      PLAY_HIGHEST_COLOR_CARD_IN_GAME,
      GET_TRICK_FOR_ANNOUNCEMENT,
      GRAB_TRICK,
      LET_TRICK_PASS,

      HEURISTIC_POVERTY_FIRST,
      POVERTY_SPECIAL_PLAY_PFUND = HEURISTIC_POVERTY_FIRST,
      POVERTY_SPECIAL_GIVE_NO_POINTS,
      POVERTY_SPECIAL_OFFER_PFUND,
      POVERTY_RE_TRUMP_COLOR_TRICK_HIGH,
      POVERTY_RE_PLAY_TRUMP,
      POVERTY_CONTRA_PLAY_COLOR,
      POVERTY_CONTRA_TRUMP_COLOR_TRICK_HIGH,
      POVERTY_LEAVE_TO_PARTNER,
      POVERTY_OVERJAB_RE,
      POVERTY_BEST_WINNING_CARD,
      HEURISTIC_POVERTY_LAST = POVERTY_BEST_WINNING_CARD,

      PLAY_COLOR_IN_SOLO,

      MEATLESS_PLAYHIGHESTCOLOR,
      PICTURE_SECONDBESTTRUMP,

      COLOR_JAB_FOR_ACE,
      COLOR_LOW_HIGH,

      CHOOSE_BEST_CARD, // for the database

      HEURISTIC_LAST = CHOOSE_BEST_CARD
    }; // enum Heuristic
    static unsigned const HEURISTIC_NUMBER;
    static Heuristic Heuristic_from_name(string const& name);
    static bool is_real(Heuristic const h);
    static bool is_general(Heuristic const h);
    static bool is_poverty(Heuristic const h);
    static bool is_solo(Heuristic const h);
    static bool is_solo_color(Heuristic const h);
    static bool is_solo_picture(Heuristic const h);
    static bool is_solo_meatless(Heuristic const h);

    struct HeuristicState {
      HeuristicState(Heuristic const heuristic, bool const active) :
        heuristic(heuristic), active(active) { }

      Heuristic heuristic;
      bool active;
    }; // struct HeuristicState

    enum TypeBool {
      BOOL_FIRST,

      TRUSTING = BOOL_FIRST, // whether the ai is trusting

      AGGRESSIVE,

      TEAMS_KNOWN,   // whether all teams are known
      HANDS_KNOWN,   // whether all hands are known
      ESTIMATE_HANDS, // whether to estimate the hands of the other players

      FEHLCREATIONONFIRSTCARD,

      BOOL_LAST = FEHLCREATIONONFIRSTCARD
    }; // enum TypeBool
    static unsigned const BOOL_NUMBER;

    enum TypeInt {
      INT_FIRST = BOOL_LAST + 1,

      REMEMBERTRICKS = INT_FIRST, // how many tricks are remembered

      LIMIT_THROW_FEHL, // maximum points for playing a fehl
      LIMITQUEEN, // mininum points per trick for jabbing with a queen
      LIMITDOLLE, // minimum points per trick for jabbing with dollen or higher

      LASTFEHLCREATION, // last trick to create a fehl color
      LAST_TRICKS_WITHOUT_HEURISTICS, // how many tricks at the end of the game no heuristics are used
      FIRST_TRICK_FOR_TRUMP_POINTS_OPTIMIZATION,

      ANNOUNCELIMIT, // points of hand for first announcement 
      ANNOUNCELIMITDEC, // points of hand you can lose between two announcements
      ANNOUNCECONFIG, // Parameter for Playerconfiguration of announcements
      ANNOUNCELIMITREPLY, // points of hand for first Reply
      ANNOUNCECONFIGREPLY, // Decrement for each announcment made, when replying 

      TAKEPOVERTY,

      SINGLESOLO, // minimum cardvalue for a solo with only one picture trump
      DOUBLESOLO, // minimum cardvalue for a solo with two pictures trump
      TRIPLESOLO, // minimum cardvalue for a solo with three pictures trump
      COLORSOLO, // minimun cardvalue for playing a colorsolo
      MEATLESS, // minimun cardvalue for playing a fleischlosen

      INT_LAST = MEATLESS
    }; // enum TypeInt
    static unsigned const INT_NUMBER;

    enum TypeCard {
      CARD_FIRST = INT_LAST + 1,

      LIMITTHROWING = CARD_FIRST, // minimum team mate card for not over jabbing

      TRUMPLIMIT_SOLOCOLOR,
      TRUMPLIMIT_SOLOJACK,
      TRUMPLIMIT_SOLOQUEEN,
      TRUMPLIMIT_SOLOKING,
      TRUMPLIMIT_SOLOJACKKING,
      TRUMPLIMIT_SOLOJACKQUEEN,
      TRUMPLIMIT_SOLOQUEENKING,
      TRUMPLIMIT_SOLOKOEHLER,
      TRUMPLIMIT_MEATLESS,
      TRUMPLIMIT_NORMAL,

      LOWEST_TRUMPLIMIT_SOLOCOLOR,
      LOWEST_TRUMPLIMIT_SOLOJACK,
      LOWEST_TRUMPLIMIT_SOLOQUEEN,
      LOWEST_TRUMPLIMIT_SOLOKING,
      LOWEST_TRUMPLIMIT_SOLOJACKKING,
      LOWEST_TRUMPLIMIT_SOLOJACKQUEEN,
      LOWEST_TRUMPLIMIT_SOLOQUEENKING,
      LOWEST_TRUMPLIMIT_SOLOKOEHLER,
      LOWEST_TRUMPLIMIT_MEATLESS,
      LOWEST_TRUMPLIMIT_NORMAL,

      LIMITHIGH, // minimum team mate card for throwing a ten or ace into this trick

      CARD_LAST = LIMITHIGH
    }; // enum TypeCard
    static unsigned const CARD_NUMBER;

  public:
    // returns a preset ai
    static Aiconfig const& preset(Difficulty const difficulty);
    // returns all (interesting) keys for the heuristic configuration
    static vector<HeuristicsMap::Key> const& keys();

  public:
    friend class Ai;

  public:
    Aiconfig();
    Aiconfig(Difficulty const difficulty);
    Aiconfig(istream& istr);
    Aiconfig(istream& istr, Difficulty const difficulty);
    Aiconfig(Aiconfig const& aiconfig);
    Aiconfig& operator=(Aiconfig const& aiconfig);
    ~Aiconfig();

    // set the aiconfig to the default values
    void reset_to_hardcoded(unsigned const no = UINT_MAX);

    // (re)sets the aiconfig to the given difficulty
    void set_to_difficulty(Difficulty const difficulty);
    // the rule has changed
    void rule_changed(int const type, void const* const old_value);
  private:
    // update the difficulty
    void update_difficulty();
  public:
    // whether the aiconfig is in a (nested) update
    bool in_update() const;
    // enter a (nested) update
    void enter_update();
    // leave a (nested) update
    void leave_update();
    // leave a (nested) update and set the difficulty manual
    void leave_update(Difficulty const difficulty);


    // whether the aiconfig is equal
    bool equal(Aiconfig const& aiconfig) const;

    AiType const& aitype(unsigned const trickno) const;
    void set_aitype(unsigned const trickno, AiType const& aitype);
    void set_aitype_for_all_tricks(AiType const& aitype);
    unsigned const future_limit(unsigned const trickno) const;
    void set_future_limit(unsigned const trickno, unsigned const future_limit);
    void set_future_limit_for_all_tricks(unsigned const future_limit);
    Rating::Type const& rating(unsigned const trickno) const;
    void set_rating(unsigned const trickno, Rating::Type const& rating);
    void set_rating_for_all_tricks(Rating::Type const& rating);

    // returns the heuristic states
    vector<HeuristicState> const&
      heuristic_states(HeuristicsMap::Key const& key) const;
    vector<HeuristicState> const&
      heuristic_states(HeuristicsMap::GametypeGroup const gametype_group,
                       HeuristicsMap::PlayerTypeGroup const playertype_group
                      ) const;
  private:
    vector<HeuristicState>&
      heuristic_states(HeuristicsMap::Key const& key);
    vector<HeuristicState>&
      heuristic_states(HeuristicsMap::GametypeGroup const gametype_group,
                       HeuristicsMap::PlayerTypeGroup const playertype_group
                      );
  public:
    // returns the heuristics
    vector<Heuristic> heuristics(HeuristicsMap::GametypeGroup const
                                 gametype_group,
                                 HeuristicsMap::PlayerTypeGroup const
                                 playertype_group
                                ) const;
    // returns the heuristics
    vector<Heuristic> heuristics(Player const& player) const;

    // returns the value
    bool value(HeuristicsMap::Key const& key,
               Heuristic const heuristic) const;
    bool value(HeuristicsMap::GametypeGroup const gametype_group,
               HeuristicsMap::PlayerTypeGroup const playertype_group,
               Heuristic const heuristic) const;
    bool value(TypeBool const type) const;
    int value(TypeInt const type) const;
    Card value(TypeCard const type) const;

    // the minimum and maximum value
    int min(TypeInt const& type) const;
    int max(TypeInt const& type) const;

    // the valid cards for the type
    vector<Card> const& valid_cards(TypeCard const type) const;

    // sets the value
    bool set(string const& type, string const& value);

    void set(HeuristicsMap::GametypeGroup const gametype_group,
             HeuristicsMap::PlayerTypeGroup const playertype_group,
             Heuristic const heuristic,
             string const& value);
    void move(HeuristicsMap::Key const& key,
              Heuristic const heuristic,
              unsigned const pos);
    void set(Heuristic const heuristic, string const& value);
    void set(TypeBool const type, string const& value);
    void set(TypeInt const type, string const& value);
    void set(TypeCard const type, string const& value);

    void set(HeuristicsMap::Key const& key,
             Heuristic const heuristic,
             bool const value);
    void set(HeuristicsMap::GametypeGroup const gametype_group,
             HeuristicsMap::PlayerTypeGroup const playertype_group,
             Heuristic const heuristic,
             bool const value);
    void set_to_default(HeuristicsMap::Key const& key);
    void set(Heuristic const heuristic, bool const value);
    void set(TypeBool const type, bool const value);
    void set(TypeInt const type, int const value);
    void set(TypeCard const type, Card const value);

    bool load(string const& filename);
    bool read(istream& istr);
    bool read_heuristics(istream& istr);
    bool save(string const& filename) const;

    // output of the settings
    void write(ostream& ostr) const;

  private:
    // initializes the heuristic states
    void init_heuristic_states();
    // fills the heuristic states with all heuristics
    void fill_up_heuristic_states();

  private:
    PRIV_VAR_R(Difficulty, difficulty);

    vector<AiType> aitype_p;
    vector<Rating::Type> rating_p;
    vector<unsigned> future_limit_p;

    map<HeuristicsMap::Key, vector<HeuristicState> > heuristic_states_;

    vector<bool> bool_p;
    vector<int> int_p;
    vector<Card> card_p;

    // the level of the internal update
    unsigned update_level_;

}; // class Aiconfig

namespace HeuristicsMap {
typedef std::map<Key, vector<Aiconfig::Heuristic> > Map;
}; // namespace HeuristicsMap

// whether the aitype has support for rating
bool supports_rating(AiType const type);

// whether the two aiconfigs are equal
bool operator==(Aiconfig const& aiconfig_a, Aiconfig const& aiconfig_b);
// whether the two aiconfigs are different
bool operator!=(Aiconfig const& aiconfig_a, Aiconfig const& aiconfig_b);

// whether the two heuristic states are equal
bool operator==(Aiconfig::HeuristicState const& heuristic_state_a,
                Aiconfig::HeuristicState const& heuristic_state_b);

string name(Aiconfig::Difficulty const difficulty);
WRITE_NAME(Aiconfig::Difficulty)

  // conversion in a string
  string name(Aiconfig::Heuristic const heuristic);

  string name(Aiconfig::TypeBool const type);
  string name(Aiconfig::TypeInt const type);
  string name(Aiconfig::TypeCard const type);

  // output of the name
WRITE_NAME(Aiconfig::Heuristic)
WRITE_NAME(Aiconfig::TypeBool)
WRITE_NAME(Aiconfig::TypeInt)
WRITE_NAME(Aiconfig::TypeCard)

  // output of the Aiconfig
  ostream& operator<<(ostream& ostr, Aiconfig const& aiconfig);


inline
Translator::Translation translation(Aiconfig::Difficulty const& difficulty)
{ return Translator::Translation(::translator,
                                 "AiConfig::Difficulty::"
                                 + ::name(difficulty)); }
inline
Translator::Translation translation(Aiconfig::Heuristic const& heuristic)
{ return Translator::Translation(::translator,
                                 "AiConfig::Heuristic::"
                                 + ::name(heuristic)); }
inline
Translator::Translation translation_description(Aiconfig::Heuristic const& heuristic)
{ return Translator::Translation(::translator,
                                 "AiConfig::Heuristic::Description::"
                                 + ::name(heuristic)); }
inline
Translator::Translation translation(Aiconfig::TypeBool const& type)
{ return Translator::Translation(::translator,
                                 "AiConfig::"
                                 + ::name(type)); }
inline
Translator::Translation translation(Aiconfig::TypeInt const& type)
{ return Translator::Translation(::translator,
                                 "AiConfig::"
                                 + ::name(type)); }
inline
Translator::Translation translation(Aiconfig::TypeCard const& type)
{ return Translator::Translation(::translator,
                                 "AiConfig::"
                                 + ::name(type)); }
inline
Translator::Translation translation(AiType const& aitype)
{ return Translator::Translation(::translator,
                                 "AiConfig::AiType::"
                                 + ::name(aitype)); }
