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

 /**********************************************************************
 *
 *  Additional contact for this file:
 *    Tassilo Kirsch  straightdog@taks.de
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


using namespace HEURISTICS_UTILS;

#define LOCKING_DEBUG

#ifdef LOCKING_DEBUG

#define SETLOCK(name) \
locked = locked || card_locked; \
if (!hi.game().isvirtual()) { \
  if (first_lock) { \
    cout << "#" << hi.no() << " " << heuristic_name << ":"; \
    first_lock = false; \
  } else \
    cout << ","; \
  cout << " " << name << (card_locked ? "(done)" : "(failed)"); \
}

#define LOCKRETURN \
if (!hi.game().isvirtual() && !first_lock) \
  cout << endl; \
return locked

#else

#define SETLOCK(name) locked = locked || card_locked
#define LOCKRETURN return locked

#endif



/**
 ** locks special cards like foxes and dollen in a normal game
 **
 ** @param     trick     trick to check
 ** @param     hi        heuristic interface
 **
 ** @return    true if at least one card has been successfully locked
 **
 ** @author    Tassilo Kirsch
 **
 ** @version   0.7.6
 **/
bool
Heuristics::lock_special_cards(Trick const& trick, HeuristicInterface const& hi)
{
  // @heuristic::name   lock special cards
  // @heuristic::idea   don't let choose best card lead with a dolle or play a fox or single charlie

  if (!GAMETYPE::is_normal(hi.game().type()))
    return false;

  string const heuristic_name = "lock special cards";

  bool locked = false;
  bool first_lock = true;

  // don't play a fox unless it's endangered (cf. has_endangered_fox)
  if (   hi.game().rule()(Rule::EXTRAPOINT_CATCH_FOX)
      && hi.hand().contains(Card::FOX)
      && !has_endangered_fox(trick, hi)
     )
  {
    bool card_locked = hi.hand().lock(Card::FOX);
    SETLOCK("fox");
  }

  // don't lead with a dolle if it's guaranteed to win the trick but our partner
  // doesn't know our team yet and therefore cannot play a pfund
  if (   trick.isstartcard()
      && !partner_knows_me(hi)
      && hi.hand().contains(Card::DOLLE)
      && cardWillWinTheTrick(trick, hi, HandCard(hi.hand(), Card::DOLLE))
     )
  {
    bool card_locked = hi.hand().lock(Card::DOLLE);
    SETLOCK("dolle");
  }

  // don't play a single charlie if the hand has more trumps than plain suit cards
  if (   hi.game().rule()(Rule::EXTRAPOINT_CHARLIE)
      && (hi.hand().numberof(Card::CHARLIE) == 1)
      && (hi.hand().numberoftrumps() > hi.hand().cardsnumber() / 2)
     )
  {
    bool card_locked = hi.hand().lock(Card::CHARLIE);
    SETLOCK("charlie");
  }

  LOCKRETURN;

} // lock_special_cards(...)


/**
 ** locks all cards from suits that are traps for the partner
 **
 ** @param     trick     trick to check
 ** @param     hi        heuristic interface
 **
 ** @return    true if at least one card has been successfully locked
 **
 ** @author    Tassilo Kirsch
 **
 ** @version   0.7.6
 **/
bool
Heuristics::lock_plain_suit_traps(Trick const& trick, HeuristicInterface const& hi)
{
  // @heuristic::name   lock plain suit traps
  // @heuristic::idea   don't let choose best card lead with a suit that's a trap for the partner

  string const heuristic_name = "lock plain suit trap";

  bool locked = false;
  bool first_lock = true;

  for (vector<Card::Color>::const_iterator
       c = hi.game().rule().card_colors().begin();
       c != hi.game().rule().card_colors().end();
       ++c)
  {
    // ace is always a plain suit card (unless the suit itself is trump)
    if (is_plain_suit_trap(trick, hi, Card(*c, Card::ACE))) {
      bool card_locked = hi.hand().lock(*c);
      SETLOCK(*c);
    }
  }

  LOCKRETURN;

} // lock_plain_suit_traps(...)

