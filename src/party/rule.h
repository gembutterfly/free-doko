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

#ifndef CLASS_RULE_HEADER
#define CLASS_RULE_HEADER

#include "../basistypes.h"
#include "../game/specialpoint.h"
#include "../card/card.h"
#include "../misc/translation.h"

namespace COUNTING_NS {
  enum Counting {
    PLUS,
    MINUS,
    PLUSMINUS
  };
} // namespace COUNTING_NS 
using COUNTING_NS::Counting;

/**
 **
 ** @brief      controls the rules of the game/party
 **
 ** @author     Diether Knof
 **
 **/
class Rule {
  public:
    enum Ruleset {
      DDV,
      FREEDOKO,
      DOKOLOUNGE_DDV,
      DOKOLOUNGE_SONDERREGELN,
      DOKOLOUNGE_LERNTISCH
    }; // enum Ruleset
    enum TypeBool {
      FIRST,
      BOOL_FIRST = FIRST,

      // mutate the rules after each game
      // default: false
      // is always 'false' in a release
      MUTATE = BOOL_FIRST,
      // if it is show, if a card is valid
      // default: true
      SHOW_IF_VALID,
      // if the hands of the other players can be seen
      // default: false
      SHOW_ALL_HANDS,
      // the number of tricks in the trickpiles are shown
      // default: true
      SHOW_TRICKS_IN_TRICKPILES,
      // if the points of the trickpiles are shown
      // default: true
      SHOW_TRICKPILES_POINTS,
      // whether the (known) teams are shown in the game (the team icons)
      // default: true
      SHOW_KNOWN_TEAMS_IN_GAME,
      // whether the soloplayer is shown in the game
      // (that is the team icon like the gametype in solo games,
      // the marriage- and poverty icon an so on)
      // default: true
      SHOW_SOLOPLAYER_IN_GAME,

      // if the player can get a hint for the card to be played
      // default: true
      CARD_HINT,
      // if the player can take back the last trick
      // default: false
      TAKE_BACK_TRICKS,

      // with or without 9
      // default: true
      WITH_NINES,

      // bock rounds (t.i. the points are doubled)
      // default: true
      BOCK,
      // append bock rounds (or sum them up)
      // default: false
      BOCK_APPEND,
      // bock after 120-120 points
      // default: true
      BOCK_120,
      // bock after a lost solo game
      // default: false
      BOCK_SOLO_LOST,
      // bock after a lost re/contra game (only re/contra is announced)
      // default: false
      BOCK_RE_LOST,
      // bock after a real heart trick
      // default: false
      BOCK_HEART_TRICK,
      // bock after a black game (a team got no trick)
      // default: false
      BOCK_BLACK,

      // throw with nines (-> MIN_NUMBER_OF_THROWING_NINES)
      // default: true
      THROW_WITH_NINES,
      // throw with kings (-> MIN_NUMBER_OF_THROWING_KINGS)
      // default: false
      THROW_WITH_KINGS,
      // throw with nines and kings (-> MIN_NUMBER_OF_THROWING_NINES_AND_KINGS)
      // default: false
      THROW_WITH_NINES_AND_KINGS,
      // throw with too many points (-> MIN_RICHNESS_FOR_THROWING)
      // default: false
      THROW_WITH_RICHNESS,

      // poverty (<= 3 trump)
      // default: true
      POVERTY,
      // to shift the trump (or give the cards again)
      // default: true
      POVERTY_SHIFT,
      // to shift only the trump
      // default: false
      POVERTY_SHIFT_ONLY_TRUMP,
      // fox do not count for the poverty trump number
      // default: false
      POVERTY_FOX_DO_NOT_COUNT,
      // shift the fox extra
      // default: false
      POVERTY_FOX_SHIFT_EXTRA,
      // shift the fox open
      // default: false
      POVERTY_FOX_SHIFT_OPEN,
      // to throw with at max one trump
      // default: false
      THROW_WITH_ONE_TRUMP,

      // to throw when the diamond ace is the highest trump
      // default: false
      THROW_WHEN_FOX_HIGHEST_TRUMP,

      // whether you can make the announcement
      // default: true
      MARRIAGE_FIRST_FOREIGN,
      // whether you can make the announcement
      // default: true
      MARRIAGE_FIRST_COLOR,
      // whether you can make the announcement
      // default: true
      MARRIAGE_FIRST_SINGLE_COLOR,
      // whether you can make the announcement
      // default: true
      MARRIAGE_FIRST_TRUMP,
      // whether the first trick decides the marriage, also if it goes to the bride
      // default: false
      MARRIAGE_FIRST_ONE_DECIDES,
      // whether the marriage goes before the poverty
      // default: false
      MARRIAGE_BEFORE_POVERTY,

      // if a player has both diamond kings, he can decide, with whom he builds
      // a team, when he is playing the first diamond king
      // default: false
      GENSCHER,

      // if the solo games are allowed
      // default: true
      SOLO,
      // default: true
      SOLO_JACK,
      // default: true
      SOLO_QUEEN,
      // default: true
      SOLO_KING,
      // default: true
      SOLO_QUEEN_JACK,
      // default: true
      SOLO_KING_JACK,
      // default: true
      SOLO_KING_QUEEN,
      // default: true
      SOLO_KOEHLER,
      // default: true
      SOLO_COLOR,
      // default: true
      SOLO_MEATLESS,
      // default: false
      THROWING_BEFORE_SOLO,

      // the hearts tens are trump
      // default: true
      DOLLEN,
      // in the same trick is the second hearts ten is greater than the first
      // default: true
      DOLLEN_SECOND_OVER_FIRST,
      // ... but not in the last trick
      // default: false
      DOLLEN_CONTRARY_IN_LAST_TRICK,
      // if swines are announced the first dolle goes over the second always
      // default: false
      DOLLEN_FIRST_OVER_SECOND_WITH_SWINES,

      // both diamond ace are the highest
      // default: true
      SWINES,
      // when to make the announcement
      // (start of the game, or when the first is played)
      // dafault: true
      SWINES_ANNOUNCEMENT_BEGIN,
      // when the first diamond ace is home, the second becomes a Schwein
      // default: false
      SWINE_ONLY_SECOND,
      // when there are swines in a solo
      // default: true
      SWINES_IN_SOLO,
      // when there are swines in a poverty
      // default: true
      SWINES_IN_POVERTY,
      // both diamond nine are above the Schweine
      // default: false
      HYPERSWINES,
      // when to make the announcement
      // (start of the game, or when the first is played)
      // default: true
      HYPERSWINES_ANNOUNCEMENT_BEGIN,
      // when there are hyperswines in a solo
      // default: true
      HYPERSWINES_IN_SOLO,
      // when there are hyperswines in a povert
      // default: true
      HYPERSWINES_IN_POVERTY,
      // whether it is allowed that a player has swines and hyperswines
      // default: false
      SWINES_AND_HYPERSWINES_JOINT,

      // catching the diamond ace of the other team: +1 point
      // default: true
      EXTRAPOINT_CATCH_FOX,
      // catching the diamond ace of the other team in the last trick: +1 point
      // default: false
      EXTRAPOINT_CATCH_FOX_LAST_TRICK,
      // the fox gets the last trick: +1 point
      // default: false
      EXTRAPOINT_FOX_LAST_TRICK,
      // the double_fox gets the last trick: +1 points
      // (+ the point from 'fox_last_trick')
      // default: false
      EXTRAPOINT_DOUBLE_FOX_LAST_TRICK,

      // club jack gets the last trick: +1 point
      // default: true
      EXTRAPOINT_CHARLIE,
      // club jack of the other team is catched in the last trick: +1 point
      // default: true
      EXTRAPOINT_CATCH_CHARLIE,
      // both club jack of the same team get the last trick: +2 point
      // default: false
      EXTRAPOINT_DOUBLE_CHARLIE,
      // both club jack of the same team is catched in the last trick: +2 point
      // default: false
      EXTRAPOINT_CATCH_DOUBLE_CHARLIE,
      // jab a charlie only with a diamond queen
      // default: false
      EXTRAPOINT_CATCH_CHARLIE_ONLY_WITH_DIAMOND_QUEEN,

      // the second dolle jabs the first
      // default: false
      EXTRAPOINT_DOLLE_JABS_DOLLE,
      // if there is a heart trick
      // default: false
      EXTRAPOINT_HEART_TRICK,

      // whether an announcement gives the information, which team the player is
      // default: false
      KNOCKING,
      // whether an announcement can be made without the ones before being
      // in the limit
      // default: false
      ANNOUNCEMENT_LIMIT_ONLY_FOR_CURRENT,
      // whether the announcement can be made till the trick is full
      // that is the played (open) card does count, too
      // default: false
      ANNOUNCEMENT_TILL_FULL_TRICK,
      // if the winner of the first trick has to make an announcement,
      // when the trick has at least thirty points
      // default: true
      ANNOUNCEMENT_FIRST_TRICK_THIRTY_POINTS,
      // the duty announcement in the first trick has only got to be made
      // when no announcement has been made so far
      // default: false
      ANNOUNCEMENT_FIRST_TRICK_THIRTY_POINTS_ONLY_FIRST,
      // the duty announcement in the first trick has only got to be made
      // when it is a re/contra
      // default: true
      ANNOUNCEMENT_FIRST_TRICK_THIRTY_POINTS_ONLY_RE_CONTRA,
      // the duty announcement in the first trick has also to be made
      // in a marriage
      // default: true
      ANNOUNCEMENT_FIRST_TRICK_THIRTY_POINTS_IN_MARRIAGE,
      // whether you can reply with re/contra to an announcement
      // default: true
      ANNOUNCEMENT_REPLY,
      // whether with an announcement re/contra 120 points must be get and the replies also set the points to get
      // default: true
      ANNOUNCEMENT_STRICT_LIMIT,

      // whether the announcement re/contra doubles the points
      // default: true
      ANNOUNCEMENT_RE_DOUBLES,

      // whether the announcement contra doubles the point 'against re'
      // default: false
      ANNOUNCEMENT_CONTRA_DOUBLES_AGAINST_RE,

      // whether the numbe of rounds is limited
      // default: true
      NUMBER_OF_ROUNDS_LIMITED,
      // whether the points (for the tournament) are limited
      // default: false
      POINTS_LIMITED,

      // offer the duty solo to the other players
      // default: false
      OFFER_DUTY_SOLO,

      // whether the soloplayer of a lust-solo plays the first card
      // default: true
      LUSTSOLO_PLAYER_LEADS,

      // whether solo games always count 3* points (also when lost)
      // default: false
      SOLO_ALWAYS_COUNTS_TRIPLE,

      BOOL_LAST = SOLO_ALWAYS_COUNTS_TRIPLE
    }; // enum TypeBool
    static unsigned const BOOL_NUMBER;

    enum TypeUnsigned {
      UNSIGNED_FIRST = BOOL_LAST + 1,

      // the number of unsupported rules (interesting for network games)
      // default: 0
      UNSUPPORTED = UNSIGNED_FIRST,

      // bock multiplier
      // default: 2
      BOCK_MULTIPLIER,

      // the number of tricks, that are allowed to be seen
      // default: 1
      SHOW_TRICKS_NUMBER,

      // minimum number of nines for throwing
      // default: 5
      MIN_NUMBER_OF_THROWING_NINES,
      // minimum number of kings for throwing
      // default: 5
      MIN_NUMBER_OF_THROWING_KINGS,
      // minimum number of nines and kings for throwing
      // default: 7
      MIN_NUMBER_OF_THROWING_NINES_AND_KINGS,

      // minimum number of points for throwing
      // default: 70
      MIN_RICHNESS_FOR_THROWING,

      // the number of tricks, after which you have to play alone
      // if you have not found a partner
      MARRIAGE_DETERMINATION,

      // announcements:
      // -1 for not allowed,
      // 1-12: as long as you have at least so many cards on the hand
      // default: 10
      ANNOUNCEMENT_NO_120,
      // default: 9
      ANNOUNCEMENT_NO_90,
      // default: 6
      ANNOUNCEMENT_NO_60,
      // default: 3
      ANNOUNCEMENT_NO_30,
      // default: 3
      ANNOUNCEMENT_NO_0,
      // the last time, an announcement can be made
      // default: 3
      ANNOUNCEMENT_LAST,

      // the number of rounds to play
      // default: 20
      NUMBER_OF_ROUNDS,

      // the number of points in a tournament
      // default: 40
      POINTS,
      // the number of duty soli
      // default: 0
      NUMBER_OF_DUTY_SOLI,
      // the number of duty color soli
      // default: 0
      NUMBER_OF_DUTY_COLOR_SOLI,
      // the number of duty picture soli
      // default: 0
      NUMBER_OF_DUTY_PICTURE_SOLI,

      // number of players
      // default: 4
      NUMBER_OF_PLAYERS,

      // number of players in a game
      // default: 4
      NUMBER_OF_PLAYERS_IN_GAME,

      UNSIGNED_LAST = NUMBER_OF_PLAYERS_IN_GAME,
    }; // enum TypeUnsigned
    static unsigned const UNSIGNED_NUMBER;

    // these values are calculated, not set
    enum TypeUnsignedExtra {
      UNSIGNED_EXTRA_FIRST = UNSIGNED_LAST + 1,

      // number of cards colors, values and cards
      // how often a card is in the game
      // 2
      NUMBER_OF_SAME_CARDS = UNSIGNED_EXTRA_FIRST,
      // the number of colors
      // 4
      NUMBER_OF_CARD_COLORS,
      // the number of colors, with trump as an extra color
      // 5
      NUMBER_OF_CARD_TCOLORS,
      // the number of card values
      // 5/6
      NUMBER_OF_CARD_VALUES,
      // the number of cards (each (color, value) exists two times)
      // 24
      NUMBER_OF_CARDS,

      // the number of teams (two)
      // 2
      NUMBER_OF_TEAMS,
      // the number of players in a team
      // (NUMBER_OF_PLAYERS_IN_GAME / NUMBER_OF_TEAMS)
      NUMBER_OF_PLAYERS_PER_TEAM,

      // the number of games per round
      // equals the number of players
      NUMBER_OF_GAMES_PER_ROUND,

      MAX_NUMBER_OF_TRICKS_IN_GAME,
      // NUMBER_OF_CARDS / NUMBER_OF_PLAYERS
      NUMBER_OF_TRICKS_IN_GAME,
      NUMBER_OF_CARDS_ON_HAND = NUMBER_OF_TRICKS_IN_GAME,

      // the points needed for a 'Doppelkopf' extrapoint
      // 40
      POINTS_FOR_DOPPELKOPF,

      // the maximum number of trumps for a poverty
      // 3
      MAX_NUMBER_OF_POVERTY_TRUMPS,

      UNSIGNED_EXTRA_LAST = MAX_NUMBER_OF_POVERTY_TRUMPS
    }; // enum TypeUnsignedExtra

    // ToDo: TypeCard (p.e. for swines card)

    enum TypeCounting {
      COUNTING = UNSIGNED_EXTRA_LAST + 1,
      LAST = COUNTING
    }; // enum TypeCounting

    // how to sum the points
    // default: plus
    // ***
    // PARTY_POINTS_SUMMING

  public:
    Rule();
    Rule(Rule const&);

    Rule& operator=(Rule const&);

    // returns, whether the rule is activ or not (or the value)
    bool operator()(TypeBool const& type) const;
    unsigned operator()(TypeUnsigned const& type) const;
    unsigned operator()(TypeUnsignedExtra const& type) const;
    Counting operator()(TypeCounting const& type) const;

    // returns, whether the gametype can be selected
    bool operator()(GameType const& game_type) const;
    // returns, whether the marriage selector can be selected
    bool operator()(MarriageSelector const& marriage_selector) const;
    // returns, whether the specialpoint is valid
    bool operator()(Specialpoint const& special_point) const;
    // returns the number of remaining cards for the announcement
    unsigned remaining_cards(Announcement const& announcement) const;

    // returns, whether the dependencies of the rule are met
    bool dependencies(TypeBool const& type) const;
    bool dependencies(TypeUnsigned const& type) const;

    // returns the setting of the rule
    bool value(TypeBool const& type) const;
    unsigned value(TypeUnsigned const& type) const;
    Counting value(TypeCounting const& type) const;
    Counting const& counting() const;

    // the minimum and maximum value
    unsigned min(TypeUnsigned const& type) const;
    unsigned max(TypeUnsigned const& type) const;

    // Remark:
    // these are vectors and not sets because of compilation problems
    // all card colors
    vector<Card::Color> const& card_colors() const;
    // all card values (depending on rule 'with nines') in descending order 
    vector<Card::Value> const& card_values() const;
    // all cards (depending on rule 'with nines')
    vector<Card> const& cards() const;

    // sets the rule
    void set(TypeBool type, bool value);
    void set(TypeBool const& type, string const& value);
    void set(TypeUnsigned const& type, unsigned const& value);
    void set(TypeUnsigned const& type, string const& value);
    void set(TypeCounting const& type, Counting const& value);
    void set(TypeCounting const& type, string const& value);

    bool set(string const& type, string const& value);

    // set to a ruleset
    void set_to(Ruleset const& ruleset);

    // mutate the rule
    void mutate();

    // load the rules
    bool load(string const& filename);
    bool read(istream& istr);
    // save the rules in the given file
    bool save(string const& filename) const;

    // output of the rules
    void write(ostream& ostr) const;

  private:
    // set to the default values
    void reset_to_hardcoded();
  public:
    // set to the values for the seed statistics
    void reset_to_seed_statistics();

  private:
    vector<bool> bool_p;
    vector<unsigned> unsigned_p;
    Counting counting_;

}; // class Rule

// conversion in a string
string name(Rule::TypeBool const& type);
string name(Rule::TypeUnsigned const& type);
string name(Rule::TypeUnsignedExtra const& type);
string name(Rule::TypeCounting const& type);
string name(Counting const& counting);
#ifdef WORKAROUND
// 0.7.0 name changed
string name_old(Counting const& counting);
#endif

// output of the name
ostream& operator<<(ostream& ostr, Rule::TypeBool const& type);
ostream& operator<<(ostream& ostr, Rule::TypeUnsigned const& type);
ostream& operator<<(ostream& ostr, Rule::TypeUnsignedExtra const& type);
ostream& operator<<(ostream& ostr, Rule::TypeCounting const& type);
ostream& operator<<(ostream& ostr, Counting const& counting);

// output of the Rule
ostream& operator<<(ostream& ostr, Rule const& rule);

// comparison of two rules (value)
bool operator==(Rule const& rule1, Rule const& rule2);
bool operator!=(Rule const& rule1, Rule const& rule2);

inline
Translator::Translation translation(Rule::TypeBool const& type)
{ return Translator::Translation(::translator,
                                 "Rule::"
                                 + ::name(type)); }
inline
Translator::Translation translation(Rule::TypeBool const& type, string const& text)
{ return Translator::Translation(::translator,
                                 "Rule::"
                                 + ::name(type)
                                 + (text.empty() ? text : " -- " + text)); }
inline
Translator::Translation translation(Rule::TypeUnsigned const& type)
{ return Translator::Translation(::translator,
                                 "Rule::"
                                 + ::name(type)); }
inline
Translator::Translation translation(Rule::TypeUnsigned const& type, string const& text)
{ return Translator::Translation(::translator,
                                 "Rule::"
                                 + ::name(type)
                                 + (text.empty() ? text : " -- " + text)); }
inline
Translator::Translation translation(Rule::TypeCounting const& type)
{ return Translator::Translation(::translator,
                                 "Rule::"
                                 + ::name(type)); }
inline
Translator::Translation translation(Counting const& counting)
{ return Translator::Translation(::translator,
                                 "Counting::"
                                 + ::name(counting)); }

#endif // #ifndef CLASS_RULE_HEADER
