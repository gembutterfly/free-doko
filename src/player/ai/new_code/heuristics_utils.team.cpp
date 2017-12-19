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
 ********************************************************************/

#include "constants.h"

#include "heuristics.h"
#include "heuristics_utils.h"

#include "HeuristicInterface.h"
#include "cards_information.h"
#include "cards_information.of_player.h"
#include "team_information.h"
#include "../../game/game.h"
#include "../../party/party.h"
#include "../../party/rule.h"
#include "../../card/trick.h"
#include "../../card/card_signals.h"

namespace HEURISTICS_UTILS {

/**
 ** checks if player p is a teammate
 **
 ** @param     hi      heuristic interface
 ** @param     p       player to check
 ** @param     strict  check only for known (and not for guessed) partners
 **
 ** @return    true if it's a teammate
 **
 ** @author    Tassilo Kirsch
 **
 ** @version   0.7.6
 **/
bool
is_teammate(HeuristicInterface const& hi, Player const& p, bool strict) // = false
{
  // in an undetermined marriage, all players are considered opponents
  if (hi.game().undetermined_marriage())
    return false;

  // don't guess
  if (strict)
    return hi.teamofplayer(p) == hi.team();

  Player const* partner = hi.team_information().guessed_partner();
  return    (partner && (p == *partner))        // normal
         || (hi.teamofplayer(p) == hi.team());  // solo
} // bool is_teammate(HeuristicInterface const& hi, Player const& p, bool strict)


/**
 ** checks if player number p is a teammate
 **
 ** @param     hi      heuristic interface
 ** @param     p       number of the player to check
 ** @param     strict  check only for known (and not for guessed) partners
 **
 ** @return    true if it's a teammate
 **
 ** @author    Tassilo Kirsch
 **
 ** @version   0.7.6
 **/
bool
is_teammate(HeuristicInterface const& hi, unsigned p, bool strict) // = false
{
  return is_teammate(hi, hi.game().player(p), strict);
} // bool is_teammate(HeuristicInterface const& hi, unsigned p, bool strict)


/**
 ** checks if player number p in trick t is a teammate
 **
 ** @param     hi      heuristic interface
 ** @param     t       trick
 ** @param     p       number of the trick's player to check
 ** @param     strict  check only for known (and not for guessed) partners
 **
 ** @return    true if it's a teammate
 **
 ** @author    Tassilo Kirsch
 **
 ** @version   0.7.6
 **/
bool
is_teammate(HeuristicInterface const& hi, Trick const& t, unsigned p, bool strict) // = false
{
  return is_teammate(hi, t.player_of_card(p), strict);
} // bool is_teammate(HeuristicInterface const& hi, Trick const& t, unsigned p, bool strict)


/**
 ** checks if player p is a possible teammate
 **
 ** @param     hi      heuristic interface
 ** @param     p       player to check
 **
 ** @return    true if it's a possible teammate
 **
 ** @author    Tassilo Kirsch
 **
 ** @version   0.7.6
 **/
bool
is_possible_teammate(HeuristicInterface const& hi, Player const& player)
{
/*
These grids show the results of is_possible_teammate() for all possible team distributions.
On the left is the team distribution, on the right are the results for all possible teams
of the player to check. There are more distributions in the grid for the contra player
because of the possibility of a silent marriage. The function also allows for the
distribuitions listed as never been set by TeamInformation::update_teams().

U:unknown +:maybe_re -:maybe_contra R:re C:contra f:false t:true

hi.team() == TEAM::RE
       U + - R C
       =========
C C R        t f  // normal game, teams cleared
C C C          f  // solo incl. silent marriage
+ - C    t f   f
U U C  t       f
U + -  f t f
+ - -    t f
U U U  t
================  distributions never set by TeamInformation::update_teams()
U + C  f t     f
U - C  t   f   f
+ + C    t     f
- - C      t   f
U U +  f t
U U -  t   f
U + +  f t
U - -  t   t
+ + -    t f
+ + +    t
- - -      t

hi.team() == TEAM::CONTRA
       U + - R C
       =========
R R C        f t  // normal game, teams cleared
C C R        f t  // solo incl. known silent marriage
U C R  f     f t
+ C R    f   f t
- C R      t f t
U U R  t     f
+ - R    f t f
U U C  f       t
U + C  f f     t
U - C  f   t   t
+ + C    f     t
+ - C    f t   t
- - C      t   t
U + -  f f t
+ + -    f t
U U U  t
================  distributions never set by TeamInformation::update_teams()
U + R  t f   f
U - R  f   t f
+ + R    t   f
- - R      t f    // never set but could be useful in a silent marriage
U U +  t f
U U -  f   t
U + +  t t
U - -  f   t
+ - -    f t      // never set but could be useful in a silent marriage
+ + +    t
- - -      t

*/

  using namespace TEAM;
  Team own = hi.team();
  Team t = hi.teamofplayer(player);

  // a real or a maybe partner is always a possible partner
  if (own == maybe_to_team(t))
    return true;

  // a known opponent cannot be a possible partner
  if (opposite(own) == t)
    return false;

  // still to check: unknown team or maybe-opponent

  // count the teams
  unsigned teamcount[MAYBE_CONTRA + 1] = {0};
  for (unsigned p = 0; p < hi.game().playerno(); ++p) {
    if (p == hi.no())
      continue;
    ++teamcount[hi.teamofplayer(p)];
  }

  if (teamcount[own] + teamcount[team_to_maybe(own)])
    // at least one possible partner is known, treat the player to check
    // as an opponent (because of the low probability of a silent marriage)
    return false;
  else {
    // no partners known
    if (t == team_to_maybe(opposite(own)))
      // if there's more than one maybe-opponent, they're possible partners
      return teamcount[t] > 1;
    else // if (t == UNKNOWN)
      // a player with an unknown team is a possible partner
      return true;
  }

} // bool is_possible_teammate(HeuristicInterface const& hi, Player const& player)


/**
 ** checks if player number p is a possible teammate
 **
 ** @param     hi      heuristic interface
 ** @param     p       number of the player to check
 **
 ** @return    true if it's a possible teammate
 **
 ** @author    Tassilo Kirsch
 **
 ** @version   0.7.6
 **/
bool
is_possible_teammate(HeuristicInterface const& hi, unsigned p)
{
  return is_possible_teammate(hi, hi.game().player(p));
} // bool is_possible_teammate(HeuristicInterface const& hi, unsigned p)


/**
 ** checks if player number p in trick t is a possible teammate
 **
 ** @param     hi      heuristic interface
 ** @param     t       trick
 ** @param     p       number of the trick's player to check
 **
 ** @return    true if it's a possible teammate
 **
 ** @author    Tassilo Kirsch
 **
 ** @version   0.7.6
 **/
bool
is_possible_teammate(HeuristicInterface const& hi, Trick const& t, unsigned p)
{
  return is_possible_teammate(hi, t.player_of_card(p));
} // bool is_possible_teammate(HeuristicInterface const& hi, Trick const& t, unsigned p)


/**
 ** checks if our partner knows or can guess our team
 **
 ** @param     hi      heuristic interface
 **
 ** @return    true if our partner knows or can guess our team
 **
 ** @author    Tassilo Kirsch
 **
 ** @version   0.7.6
 **/
bool
partner_knows_me(HeuristicInterface const& hi)
{
  Game const& game = hi.game();

  // in an undetermined marriage, only the bride is known to others
  if (game.undetermined_marriage())
    return hi.no() == game.soloplayer().no();

  if (::is_real(game.teaminfo(hi.no())))
    return true;

  // we're in a normal game with uncleared teams
  // we may have shown our team by playing a pfund for our partner

  // our partner
  Player const* partner = hi.team_information().guessed_partner();

  // we've only played for our team if we know our partner
  if (!partner)
    return false;

  // now check if our partner may have guessed who we are
  // implementation note: this follows the logic used by TeamInformation::update_teams()
  //                      any changes there should be reflected here
  vector<int> const& team_values = hi.team_information().team_values();
  int min_value = INT_MAX;
  int max_value = INT_MIN;
  int known = 0;
  for (unsigned p = 0; p < game.playerno(); ++p) {
    if (partner->no() == p)
      continue;
    if (!::is_real(game.teaminfo(p))) {
      if (team_values[p] < min_value)
        min_value = team_values[p];
      if (team_values[p] > max_value)
        max_value = team_values[p];
    } else
      ++known;
  } // for (p)

  if (known) {
    // our partner knows one of our opponents
    if (max_value - min_value < (hi.value(Aiconfig::TRUSTING) ? 10 : 20)) // *Value*
      return false;
    if (hi.team() == TEAM::RE)
      return team_values[hi.no()] == max_value;
    else
      return team_values[hi.no()] == min_value;
  }

  if (max_value - min_value < 10) // *Value*
    return false;

  // check if we're the only possible partner
  known = -1;
  for (unsigned p = 0; p < game.playerno(); ++p) {
    if ((partner->no() != p) && !::is_real(game.teaminfo(p))) {
      if (hi.team() == TEAM::RE) {
        if (team_values[p] >= max_value - (max_value - min_value) / 3)
          if (known > -1)
            return false;
          else
            known = p;
      } else {
        if (team_values[p] <= min_value + (max_value - min_value) / 3)
          if (known > -1)
            return false;
          else
            known = p;
      }
    }
  } // for (p)

  return known == hi.no();

} // partner_knows_me(...)


/**
 ** checks if all remaining player in the trick are of my team
 **
 ** @param     trick   trick to check
 ** @param     hi      Heuristic interface
 **
 ** @return    returns true if all remaining players are in the same team
 **
 ** @author    Borg Enders
 **
 ** @version   Beta
 **/
#if 0
bool
checkAllMyTeam( Trick const& t, HeuristicInterface const& hi )
{
  bool allmyteam = true;

  // take a look if all players coming in this trick are of mine team
  for( unsigned i = t.actcardno(); i < hi.game().playerno(); i++ )
    allmyteam = (   allmyteam
                 && ( hi.team() == hi.teamofplayer( t.player_of_card( i ) ) ));
  return allmyteam;
} // bool checkAllMyTeam( Trick t, HeuristicInterface hi )
#endif
bool
checkAllMyTeam( Trick const& t, HeuristicInterface const& hi )
{
  // check if there's an opponent sitting behind me
  for( unsigned i = t.actcardno() + 1; i < hi.game().playerno(); i++ )
    if (hi.team() != hi.teamofplayer(t.player_of_card(i)))
      return false;
  return true;
} // bool checkAllMyTeam( Trick t, HeuristicInterface hi )


/**
 ** checks if a solo player sits behind the player
 **
 ** @param     t    trick
 ** @param     hi   heuristics interface
 **
 ** @return    whether the solo player sits behind
 **
 ** @author    Borg Enders
 ** @author    Tassilo Kirsch (simplified)
 **
 ** @version   0.7.6
 **/
bool
soloPlayerBehind( Trick const& t, HeuristicInterface const& hi )
{
  return    GAMETYPE::is_solo(hi.game().type())
         && (t.cardno_of_player(hi.game().soloplayer()) > t.actcardno());
} // bool soloPlayerBehind( Trick const& t, HeuristicInterface const& hi )


/**
 ** calculates all points of own team in Game g
 **
 ** @param        hi        heuristic interface
 ** @param        g         current game
 **
 ** @return       points of own team in the game
 **
 ** @author    Borg Enders
 **
 ** @version      Beta
 **/
int
calcPointsOfOwnTeam( HeuristicInterface const& hi, Game const& g )
{
  if (!::in_running_game())
    return 0;

  int points_p = 0;

  for( unsigned int i = 0; i < g.playerno(); i++ )
  {
    if( hi.teamofplayer( g.player(i) ) == hi.team() )
    {
      points_p += g.points_of_player( g.player(i) );
    }
  } // for ( i < g.playerno() )

  Trick tr = g.trick_current();
  if( tr.isfull() )
    if( TEAM::maybe_to_team(hi.teamofplayer(tr.winnerplayer())) == hi.team() )
    {
      points_p += tr.points();
    }

  return points_p;
} // int calcPointsOfOwnTeam( HeuristicInterface hi, Game g )


/**
 ** calculates all points of opposite team in Game g
 **
 ** @param        hi        heuristic interface
 ** @param        g         current game
 **
 ** @return       points of other team in game
 **
 ** @author    Borg Enders
 **
 ** @version      Beta
 **/
int
calcPointsOfOppositeTeam( HeuristicInterface const& hi, Game const& g)
{
  if (!::in_running_game())
    return 0;

  int points_p = 0;

  for( unsigned int i = 0; i < g.playerno(); i++ )
  {
    if( hi.teamofplayer( g.player(i) ) != hi.team() )
    {
      points_p += g.points_of_player( g.player(i) );
    }
  } // for ( i < g.playerno() )

  Trick tr = g.trick_current();
  if( tr.isfull() )
    if( TEAM::maybe_to_team(hi.teamofplayer(tr.winnerplayer())) != hi.team() )
    {
      points_p += tr.points();
    }

  return points_p;
} // int calcPointsOfOppositeTeam( HeuristicInterface hi, Game g)


/**
 ** estimates the points for the teams based on the player's knowledge
 ** if the teams aren't known to the player, a worst case scenario is used
 **
 ** @param     hi        heuristic interface
 ** @param     own       points of the team of the player
 ** @param     opposite  points of the opposite team
 **
 ** @return    (void)
 **
 ** @author    Tassilo Kirsch
 **
 ** @version   0.7.6
 **/
void
estimate_team_points(HeuristicInterface const& hi, unsigned& own, unsigned& opposite)
{
  unsigned total = 0;
  unsigned ownpoints = 0;
  unsigned minpartner = 0;

  for (unsigned p = 0; p < hi.game().playerno(); ++p) {
    unsigned playerpoints = hi.game().points_of_player(p);
    total += playerpoints;
    if (p == hi.no())
      ownpoints += playerpoints;
    else if (hi.game().undetermined_marriage()) {
      // ignore teams, just choose the player who has the least points
      if (!minpartner || (playerpoints < minpartner))
        minpartner = playerpoints;
    } else if (hi.teamofplayer(p) == hi.team())
      ownpoints += playerpoints;
    else if (is_possible_teammate(hi, p))
      if (!minpartner || (playerpoints < minpartner))
        minpartner = playerpoints;
  }
  own = ownpoints + minpartner;
  opposite = total - own;
} // void estimate_team_points(...)


/**
 ** counts the number of tricks the player's partner will win by using his
 ** highest trumps, based on the knowledge found in handofplayer
 **
 ** @param     hi   heuristics interface
 **
 ** @return    number of guaranteed tricks
 **
 ** @author    Tassilo Kirsch
 **
 ** @version   0.7.6
 **/
unsigned
trump_tricks_guaranteed_by_partner(HeuristicInterface const& hi)
{
  // not useful in solos or during an undetermined marriage
  if (  !GAMETYPE::is_normal(hi.game().type())
      || hi.game().undetermined_marriage()
     )
    return 0;

  Player const* partner = hi.team_information().guessed_partner();
  if (!partner) {
    // take the club queen of an unknown re partner into account
    return   (hi.team() == TEAM::RE)
          && (hi.cards_information().highest_remaining_trump_of_others() == Card::CLUB_QUEEN);
  }

  unsigned tricks = 0;
  for (vector<Card>::const_reverse_iterator c = hi.game().trumps().rbegin();
       c != hi.game().trumps().rend();
       ++c)
  {
    unsigned remaining = hi.cards_information().remaining_others(*c);
    if (!remaining)
      continue;
    unsigned partner_may_have = hi.handofplayer(*partner).numberof(*c);
    if (!partner_may_have)
      break;
    unsigned partner_must_have = hi.cards_information().of_player(*partner).must_have(*c);
    if (partner_must_have) {
      tricks += partner_must_have;
      if (partner_must_have == remaining)
        continue;
    } else {
      if (partner_may_have < remaining)
        break;
    }
    unsigned max_opponent_may_have = 0;
    for (unsigned p = 0; p < hi.game().playerno(); ++p) {
      if ((p != hi.no()) && (p != partner->no())) {
        unsigned opponent_may_have = hi.handofplayer(p).numberof(*c);
        if (opponent_may_have > max_opponent_may_have)
          max_opponent_may_have = opponent_may_have;
      }
    }
    if (partner_may_have <= max_opponent_may_have)
      break;
    ++tricks;
    if (max_opponent_may_have)
      break;

  } // for (c \in hi.game().trumps())

  return tricks;

} // trump_tricks_guaranteed_by_partner(...)


/**
 ** checks if the player has an endangered fox on hand, i.e. if the player can't
 ** afford to wait for a better opportunity to play his fox(es).
 **
 ** @param     t    trick
 ** @param     hi   heuristics interface
 **
 ** @return    whether the player has an endangered fox
 **
 ** @author    Tassilo Kirsch
 **
 ** @version   0.7.6
 **/
bool
has_endangered_fox(Trick const& t, HeuristicInterface const& hi)
{
  HandCard const& fox = HandCard(hi.hand(), Card::FOX);
  if (!fox.isfox())
    return false;
  unsigned foxes = hi.hand().numberoftrumpaces();
  if (!foxes)
    return false;

  unsigned own_trumps = hi.hand().numberoftrumps();

  // we can wait for a good opportunity to play our foxes
  if (own_trumps - foxes > 3) // *value*
    return false;

  Player const* partner = hi.team_information().guessed_partner();

  // check if our partner can take care of our foxes
  unsigned partner_tricks = trump_tricks_guaranteed_by_partner(hi);
  if (partner_tricks) {
    if (t.isempty() || (partner && !t.has_played(*partner)))
      --partner_tricks;
    if (partner_tricks >= foxes)
      return false;
    foxes -= partner_tricks;
  }

  // check if we or our partner can take care of our foxes through trump length
  unsigned max_opponent_trumps = 0;
  for (unsigned p = 0; p < hi.game().playerno(); ++p) {
    if (p == hi.no())
      continue;
    unsigned trumps = hi.handofplayer(p).numberoftrumps();
    if (partner && (p == partner->no())) {
      if (trumps > own_trumps)
        own_trumps = trumps;
    } else {
      if (trumps > max_opponent_trumps)
        max_opponent_trumps = trumps;
    }
  }
  if (own_trumps - foxes >= max_opponent_trumps)
    return false;

  return true;
} // bool has_endangered_fox(...)


} // namespace HEURISTICS_UTILS

