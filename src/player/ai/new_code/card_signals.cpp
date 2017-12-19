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

#include "card_signals.h"
#include "constants.h"
#include <sstream>

namespace CARD_SIGNALS {


/**
 ** -> return
 **
 ** @param     weighting    the weighting to check
 **
 ** @return    the signal represented by the given weighting
 **
 ** @author    Tassilo Kirsch
 **
 ** @version   0.7.6
 **/
CardSignal
weighting_to_signal(int weighting)
{
  if (weighting > (SIGNAL_BOTH - SIGNAL_EPSILON))
    return SIGNAL_BOTH;
  if (weighting > (SIGNAL_MIN_ONE - SIGNAL_EPSILON))
    return SIGNAL_MIN_ONE;
  if (weighting > (SIGNAL_ONE - SIGNAL_EPSILON))
    return SIGNAL_ONE;
  if (weighting <= (SIGNAL_NONE + SIGNAL_EPSILON))
    return SIGNAL_NONE;
  if (weighting <= (SIGNAL_MAX_ONE + SIGNAL_EPSILON))
    return SIGNAL_MAX_ONE;
  return SIGNAL_NO_SIGNAL;
} // CardSignal weighting_to_signal(...)


/**
 ** -> return
 **
 ** @param     signal           the signal to be set
 ** @param     old_weighting    the old weighting
 **
 ** @return    returns a weighting representing the given signal, preserving non-signal weightings in old_weighting
 **
 ** @author    Tassilo Kirsch
 **
 ** @version   0.7.6
 **/
int
signal_to_weighting(CardSignal signal, int old_weighting)
{
  return signal + strip_signal(old_weighting);
} // int signal_to_weighting(...)



string
weighting_to_string(int weighting, unsigned non_signal_width)
{
  stringstream out;

  if ((weighting < MIN_WEIGHTING) || (weighting > MAX_WEIGHTING)) {

    // invalid weighting
    out << "??? " << setw(non_signal_width) << weighting;

  } else {

    CardSignal s = weighting_to_signal(weighting);
    int w = strip_signal(weighting);

    out << setw(non_signal_width ? 4 : 0);
    switch (s) {
    case SIGNAL_NONE:
      out << "[--]";
      break;
    case SIGNAL_MAX_ONE:
      out << "[- ";
      break;
    case SIGNAL_NO_SIGNAL:
      out << "";
      break;
    case SIGNAL_ONE:
      out << "[+-]";
      break;
    case SIGNAL_MIN_ONE:
      out << "[+ ]";
      break;
    case SIGNAL_BOTH:
      out << "[++]";
      break;
    }

    out << " " << setw(non_signal_width);
    if (w > 0)
      out << showpos;
    out << w;
  } // valid weighting

  return out.str();

} // weighting_to_string(...)


/**
 ** strips off a signal and returns the remaining non-signal weighting
 **
 ** @param     weighting    the weighting to check
 **
 ** @return    the non-signal portion of weighting
 **
 ** @author    Tassilo Kirsch
 **
 ** @version   0.7.6
 **/
int
strip_signal(int weighting)
{
  return weighting - weighting_to_signal(weighting);
} // int strip_signal(...)


/**
 ** adds new_weighting to old_weighting and returns the sum
 ** - non-signal weightings in old_weighting will be preserved,
 ** - prevents inadvertently changing a signal
 ** - makes shure that the result isn't greater than MAX_WEIGHTING
 ** - if new_weighting represents a signal, its non-signal portion will be stripped off
 **
 ** @param     old_weighting    the old weighting
 ** @param     new_weighting    the weighting to be added
 **
 ** @return    returns the resulting sum of old_weighting and new_weighting
 **
 ** @author    Tassilo Kirsch
 **
 ** @version   0.7.6
 **/
int
add_weighting(int old_weighting, int new_weighting)
{
  CardSignal signal = weighting_to_signal(new_weighting);
  if (signal != SIGNAL_NO_SIGNAL)
    // we're setting a signal
    new_weighting = signal_to_weighting(signal, old_weighting);
  else {
    // we're setting a non-signal weighting
    new_weighting += old_weighting;
    if (new_weighting > MAX_WEIGHTING)
      new_weighting = MAX_WEIGHTING;
    if (new_weighting < MIN_WEIGHTING)
      new_weighting = MIN_WEIGHTING;
    if (weighting_to_signal(new_weighting) != weighting_to_signal(old_weighting))
      // we've inadvertently changed the signal,
      new_weighting = old_weighting;
  }

  return new_weighting;
} // int add_weighting(...)


/**
 ** -> return
 **
 ** @param     weighting    the weighting to check
 **
 ** @return    the number of cards signalled by weighting
 **
 ** @author    Tassilo Kirsch
 **
 ** @version   0.7.6
 **/
unsigned
signalled_cards(int weighting)
{
  switch (weighting_to_signal(weighting)) {
  case SIGNAL_NONE:
  case SIGNAL_MAX_ONE:
  case SIGNAL_NO_SIGNAL:
    return 0;
  case SIGNAL_ONE:
  case SIGNAL_MIN_ONE:
    return 1;
  case SIGNAL_BOTH:
    return 2;
  }
  return UINT_MAX;
} // unsigned signalled_csrds(...)


/**
 ** adjusts a weighting (including signals) when the first card corresponding to
 ** the weighting has been played or its owner is known
 **
 ** @param     weighting           the weighting to adjust
 ** @param     first_card_owner    adjust the weighting for the player/owner of the first card
 **
 ** @return    weighting adjusted for only one card left
 **
 ** @author    Tassilo Kirsch
 **
 ** @version   0.7.6
 **/
int
adjust_weighting_for_second_card(int weighting, bool first_card_owner)
{
  // nothing to do if the card hasn't been weighted yet
  if (!weighting)
    return 0;

  CardSignal signal = weighting_to_signal(weighting);

  if (weighting > signal)
    // halve positive weightings
    weighting = signal + strip_signal(weighting) / 2;

  if (first_card_owner) {
    // player has signalled not to have both cards, change signal to no card
    if (signal == SIGNAL_MAX_ONE)
      return signal_to_weighting(SIGNAL_NONE, weighting);
    // SIGNAL_NONE or no signal with a negative weighting, reset weighting
    else if (weighting < 0)
      return 0;
    // player has signalled both cards, change signal to one card
    else if (signal == SIGNAL_BOTH)
      return signal_to_weighting(SIGNAL_ONE, weighting);
    // player has signalled at least one card, remove the signal
    else if (signal == SIGNAL_MIN_ONE)
      return strip_signal(weighting);
    // player has signalled exactly one card, reset weighting
    else if (signal == SIGNAL_ONE)
      return 0;
  } else {
    // another player has signalled both cards, remove the signal
    if (signal == SIGNAL_BOTH)
      return strip_signal(weighting);
    // another player has signalled at least one card, change signal to one card
    else if (signal == SIGNAL_MIN_ONE)
      return signal_to_weighting(SIGNAL_ONE, weighting);
    // another player has signalled not to have both cards, remove the signal
    else if (signal == SIGNAL_MAX_ONE)
      return strip_signal(weighting);
  }

  return weighting;
} // int adjust_weighting_for_second_card(...)


/**
 ** Processes a card's weightings to calculate the effective weightings used either to distribute the
 ** card (for_distribution == true, see CardsInformation::OfPlayer::estimate_hand) or to estimate the
 ** number of copies in a player's hand (for_distribution == false, see estimate_numberof functions).
 **
 ** The vector 'weightings' holds the players' weightings of the card to process. Only the first 3
 ** elements will be processed. Elements with a value of INT_MIN will be ignored.
 **
 ** There are 3 possible situations for the number of remaining copies of the card:
 ** (1) Both copies are still in the game => remaining == 2, musthave_player == -1
 ** (2) Only one copy is left => remaining == 1, musthave_player == -1
 ** (3) Both copies are still in the game, but the owner of one of them is already known.
 **     In this case, use remaining == 1 and set musthave_player to the index of the owner's
 **     weighting in 'weightings'. If the owner's weighting isn't an element of 'weightings' (e.g.
 **     if it's the analysing AI), any number that's neither a valid index nor -1 will do.
 **     The weightings will then be adjusted (see adjust_weighting_for_second_card).
 **
 ** The function returns a vector holding the players' effective weightings in pairs
 ** .first:  effective weighting for the first (or only) copy
 ** .second: effective weighting for the second copy
 **
 ** The results are encoded as follows:
 ** INT_MIN:          ignore
 ** MAX_WEIGHTING:    determined by card-signals: player has a copy
 ** MIN_WEIGHTING:    determined by card-signals: player does not have a copy
 ** all other values: effective non-signal weighting

 ** @param     weightings        the player's weightings of the card to process
 ** @param     remaining         number of remaining copies of the card to process
 ** @param     musthave_player   'weightings'-index of the player who must have a card, -1 if no such player exists
 ** @param     for_distribution  process for card distribution or numberof-estimation
 **
 ** @return    the players' effective weightings
 **
 ** @author    Tassilo Kirsch
 **
 ** @version   0.7.6
 **/
vector<pair<int,int> >
process_weightings(vector<int> const& weightings,
                   int remaining,
                   int musthave_player,
                   bool for_distribution) // = true
{
  // the resulting effective weightings to be used to distribute the cards
  // INT_MIN is used internally as a flag meaning "use non-signal weighting",
  // if it's returned, its meaning changes to "to be ignored"
  vector<pair<int,int> > effective(3, make_pair(INT_MIN, INT_MIN));

  unsigned const candidates = min(3u, weightings.size());

  // check if there's something to do
  if ((candidates == 0) || (remaining < 1) || (remaining > 2))
    return effective;

  // check number of valid weightings to process
  int singleton = -1;
  for (unsigned i = 0; i < candidates; ++i)
    if (weightings[i] != INT_MIN)
      if (singleton == -1)
        // first valid weighting
        singleton = i;
      else {
        // more than one valid weighting => do process
        singleton = INT_MAX;
        break;
      }

  if (singleton == -1)
    // no valid weighting found
    return effective;
  else if (singleton != INT_MAX) {
    // only one player in the mix, he has all remaining copies
    effective[0] = make_pair(MAX_WEIGHTING, MAX_WEIGHTING);
    return effective;
  }

  // the card signals
  int max_sig = INT_MIN;          // highest signal
  int min_sig = INT_MAX;          // lowest signal
  unsigned min_i, max_i;          // indices of the highest and lowest signals
  vector<int> s(3, SIGNAL_NONE);  // the signals
  vector<int> w(3, INT_MIN);      // the non-signal weightings

  // extract card signals
  for (unsigned i = 0; i < candidates; ++i) {
    w[i] = weightings[i];
    if (w[i] != INT_MIN) {

      // adjust weighting if a player has a must_have card
      if (musthave_player != -1)
        w[i] = adjust_weighting_for_second_card(w[i], i == musthave_player);

      // separate signal and non-signal weighting
      s[i] = weighting_to_signal(w[i]);
      w[i] = strip_signal(w[i]);

    }

    if (s[i] > max_sig) {
      max_sig = s[i];
      max_i = i;
    }
    if (s[i] < min_sig) {
      min_sig = s[i];
      min_i = i;
    }
  }

  // process signals
  // <thedoctor>I'm sorry... I'm so sorry</thedoctor>

/*
the signal combination and the resulting effective weightings

signals:
--: SIGNAL_NONE
- : SIGNAL_MAX_ONE
0 : SIGNAL_NO_SIGNAÖ
+ : SIGNAL_ONE
+1: SIGNAL_MIN_ONE
++: SIGNAL_BOTH

results:
?: use non-signal weightings
0: no card  (MIN_WEIGHTING)
1: one card (MAX_WEIGHTING)

columns:
                      effective weightings for
signal combination => only copy    first copy second copy
*/


  if (min_sig == max_sig) {
/*
-- -- -- => ?,?,?    ?,?,?  ?,?,?
-  -  -  => ?,?,?    ?,?,?  ?,?,?
0  0  0  => ?,?,?    ?,?,?  ?,?,?
+  +  +  => ?,?,?    ?,?,?  ?,?,?
+1 +1 +1 => ?,?,?    ?,?,?  ?,?,?
++ ++ ++ => ?,?,?    ?,?,?  ?,?,?
*/
    // everything is already set, nothing to do here
  } else {

    // the signal with the middle value
    int mid_sig = s[3 - min_i - max_i];

    if ((max_sig == SIGNAL_BOTH) && (mid_sig < SIGNAL_BOTH)) {
/*
-- -- ++ => 0,0,1    0,0,1  0,0,1
-- -  ++ => 0,0,1    0,0,1  0,0,1
-- 0  ++ => 0,0,1    0,0,1  0,0,1
-- +  ++ => 0,0,1    0,0,1  0,0,1
-- +1 ++ => 0,0,1    0,0,1  0,0,1
-  -  ++ => 0,0,1    0,0,1  0,0,1
-  0  ++ => 0,0,1    0,0,1  0,0,1
-  +  ++ => 0,0,1    0,0,1  0,0,1
-  +1 ++ => 0,0,1    0,0,1  0,0,1
0  0  ++ => 0,0,1    0,0,1  0,0,1
0  +  ++ => 0,0,1    0,0,1  0,0,1
0  +1 ++ => 0,0,1    0,0,1  0,0,1
+  +  ++ => 0,0,1    0,0,1  0,0,1
+  +1 ++ => 0,0,1    0,0,1  0,0,1
+1 +1 ++ => 0,0,1    0,0,1  0,0,1
*/
      for (unsigned i = 0; i < s.size(); ++i)
        if (s[i] == SIGNAL_BOTH)
          effective[i] = make_pair(MAX_WEIGHTING, MAX_WEIGHTING);
        else
          effective[i] = make_pair(MIN_WEIGHTING, MIN_WEIGHTING);

    } else if (mid_sig == SIGNAL_BOTH) {
/*
-- ++ ++ => 0,?,?    0,?,?  0,?,?
-  ++ ++ => 0,?,?    0,?,?  0,?,?
0  ++ ++ => 0,?,?    0,?,?  0,?,?
+  ++ ++ => 0,?,?    0,?,?  0,?,?
+1 ++ ++ => 0,?,?    0,?,?  0,?,?
*/
      for (unsigned i = 0; i < s.size(); ++i)
        if (s[i] != SIGNAL_BOTH)
          effective[i] = make_pair(MIN_WEIGHTING, MIN_WEIGHTING);

    } else if (remaining == 1) {

      if (min_sig == SIGNAL_ONE) {
/*
+  +  +1 => ?,?,?
+  +1 +1 => ?,?,?
*/
        // everything is already set, nothing to do here
      } else if (   (mid_sig < SIGNAL_ONE)
                 && (mid_sig < max_sig)
                 && (max_sig >= SIGNAL_NO_SIGNAL)
                )
      {
/*
-- -- 0  => 0,0,1
-- -- +  => 0,0,1
-- -- +1 => 0,0,1
-- -  0  => 0,0,1
-- -  +  => 0,0,1
-- -  +1 => 0,0,1
-- 0  +  => 0,0,1
-- 0  +1 => 0,0,1
-  -  0  => 0,0,1
-  -  +  => 0,0,1
-  -  +1 => 0,0,1
-  0  +  => 0,0,1
-  0  +1 => 0,0,1
0  0  +  => 0,0,1
0  0  +1 => 0,0,1
*/
        for (unsigned i = 0; i < s.size(); ++i)
          if (s[i] == max_sig)
            effective[i].first = MAX_WEIGHTING;
          else
            effective[i].first = MIN_WEIGHTING;
      } else if (mid_sig == SIGNAL_NONE) {
/*
-- -- -  => ?,?,1
*/
        for (unsigned i = 0; i < s.size(); ++i)
          if (s[i] == max_sig)
            effective[i].first = MAX_WEIGHTING;
      } else {
/*
-- -  -  => 0,?,?
-- 0  0  => 0,?,?
-- +  +  => 0,?,?
-- +  +1 => 0,?,?
-- +1 +1 => 0,?,?
-  0  0  => 0,?,?
-  +  +  => 0,?,?
-  +  +1 => 0,?,?
-  +1 +1 => 0,?,?
0  +  +  => 0,?,?
0  +  +1 => 0,?,?
0  +1 +1 => 0,?,?
*/
        for (unsigned i = 0; i < s.size(); ++i)
          if (s[i] == min_sig)
            effective[i].first = MIN_WEIGHTING;
      }

    } else {

      // remaining == 2

      // if each of two players gets a copy, we need
      // this to alternate between first and second
      bool toggle = false;

      switch (min_sig) {

      case SIGNAL_NONE:
        switch (mid_sig) {

        case SIGNAL_NONE:
/*
-- -- -  => ?,?,1    0,0,1  ?,?,0
-- -- 0  => 0,0,1    0,0,1  0,0,1
-- -- +  => 0,0,1    0,0,1  ?,?,0
-- -- +1 => 0,0,1    0,0,1  0,0,1
*/
          for (unsigned i = 0; i < s.size(); ++i)
            if (s[i] == SIGNAL_NONE) {
              effective[i].first = MIN_WEIGHTING;
              if ((max_sig == SIGNAL_NO_SIGNAL) || (max_sig == SIGNAL_MIN_ONE))
                effective[i].second = MIN_WEIGHTING;
            } else {
              effective[i].first = MAX_WEIGHTING;
              if ((max_sig == SIGNAL_NO_SIGNAL) || (max_sig == SIGNAL_MIN_ONE))
                effective[i].second = MAX_WEIGHTING;
              else
                effective[i].second = MIN_WEIGHTING;
            }
          break;

        case SIGNAL_MAX_ONE:
/*
-- -  -  => 0,?,?    0,?,?  0,?,?
-- -  0  => 0,0,1    0,0,1  0,?,?
-- -  +  => 0,0,1    0,0,1  0,1,0
-- -  +1 => 0,0,1    0,0,1  0,?,?
*/
          for (unsigned i = 0; i < s.size(); ++i)
            if (s[i] == SIGNAL_NONE) {
              effective[i].first = MIN_WEIGHTING;
              effective[i].second = MIN_WEIGHTING;
            } else if (max_sig != SIGNAL_MAX_ONE) {
              if (s[i] == SIGNAL_MAX_ONE) {
                effective[i].first = MIN_WEIGHTING;
                if (max_sig == SIGNAL_ONE)
                  effective[i].second = MAX_WEIGHTING;
              } else {
                effective[i].first = MAX_WEIGHTING;
                if (max_sig == SIGNAL_ONE)
                  effective[i].second = MIN_WEIGHTING;
              }
            }
          break;

        case SIGNAL_NO_SIGNAL:
/*
-- 0  0  => 0,?,?    0,?,?  0,?,?
-- 0  +  => 0,0,1    0,0,1  0,1,0
-- 0  +1 => 0,0,1    0,0,1  0,?,?
*/
          for (unsigned i = 0; i < s.size(); ++i)
            if (s[i] == SIGNAL_NONE) {
              effective[i].first = MIN_WEIGHTING;
              effective[i].second = MIN_WEIGHTING;
            } else if (max_sig != SIGNAL_NO_SIGNAL) {
              if (s[i] == SIGNAL_NO_SIGNAL) {
                effective[i].first = MIN_WEIGHTING;
                if (max_sig == SIGNAL_ONE)
                  effective[i].second = MAX_WEIGHTING;
              } else if (s[i] == SIGNAL_ONE) {
                effective[i].first = MAX_WEIGHTING;
                effective[i].second = MIN_WEIGHTING;
              } else {
                effective[i].first = MAX_WEIGHTING;
              }
            }
          break;

        case SIGNAL_ONE:
/*
-- +  +  => 0,?,?    0,1,0  0,0,1
-- +  +1 => 0,?,?    0,1,0  0,0,1
*/
          for (unsigned i = 0; i < s.size(); ++i)
            if (s[i] == SIGNAL_NONE) {
              effective[i].first = MIN_WEIGHTING;
              effective[i].second = MIN_WEIGHTING;
            } else {
              if (toggle) {
                effective[i].first = MIN_WEIGHTING;
                effective[i].second = MAX_WEIGHTING;
              } else {
                effective[i].first = MAX_WEIGHTING;
                effective[i].second = MIN_WEIGHTING;
              }
              toggle = true;
            }
          break;

        case SIGNAL_MIN_ONE:
/*
-- +1 +1 => 0,?,?    0,1,1  0,?,?
*/
          for (unsigned i = 0; i < s.size(); ++i)
            if (s[i] == SIGNAL_NONE)
              effective[i].first = MIN_WEIGHTING;
            else
              effective[i].first = MAX_WEIGHTING;
          break;

        }
        break;

      case SIGNAL_MAX_ONE:
        switch (mid_sig) {

        case SIGNAL_MAX_ONE:
/*
-  -  0  => 0,0,1    0,0,1  ?,?,?
-  -  +  => 0,0,1    0,0,1  ?,?,0
-  -  +1 => 0,0,1    0,0,1  ?,?,?
*/
          for (unsigned i = 0; i < s.size(); ++i)
            if (s[i] == SIGNAL_MAX_ONE) {
              effective[i].first = MIN_WEIGHTING;
            } else {
              effective[i].first = MAX_WEIGHTING;
              if (s[i] == SIGNAL_ONE)
                effective[i].second = MIN_WEIGHTING;
            }
          break;

        case SIGNAL_NO_SIGNAL:
/*
-  0  0  => 0,?,?    0,?,?  ?,?,?
-  0  +  => 0,0,1    0,0,1  ?,?,0
-  0  +1 => 0,0,1    0,0,1  ?,?,?
*/
          for (unsigned i = 0; i < s.size(); ++i)
            if (s[i] == SIGNAL_MAX_ONE) {
              effective[i].first = MIN_WEIGHTING;
            } else if (max_sig != SIGNAL_NO_SIGNAL) {
              if (s[i] == SIGNAL_NO_SIGNAL)
                effective[i].first = MIN_WEIGHTING;
              else {
                effective[i].first = MAX_WEIGHTING;
                if (s[i] == SIGNAL_ONE)
                  effective[i].second = MIN_WEIGHTING;
              }
            }
          break;

        case SIGNAL_ONE:
/*
-  +  +  => 0,?,?    0,1,0  0,0,1
-  +  +1 => 0,?,?    0,1,1  ?,0,?
*/
          for (unsigned i = 0; i < s.size(); ++i)
            if (max_sig == SIGNAL_MIN_ONE) {
              if (s[i] == SIGNAL_MAX_ONE) {
                effective[i].first = MIN_WEIGHTING;
              } else {
                effective[i].first = MAX_WEIGHTING;
                if (s[i] == SIGNAL_ONE)
                  effective[i].second = MIN_WEIGHTING;
              }
            } else if (s[i] == SIGNAL_MAX_ONE) {
              effective[i].first = MIN_WEIGHTING;
              effective[i].second = MIN_WEIGHTING;
            } else {
              if (toggle) {
                effective[i].first = MIN_WEIGHTING;
                effective[i].second = MAX_WEIGHTING;
              } else {
                effective[i].first = MAX_WEIGHTING;
                effective[i].second = MIN_WEIGHTING;
              }
              toggle = true;
            }
          break;

        case SIGNAL_MIN_ONE:
/*
-  +1 +1 => 0,?,?    0,1,1  ?,?,?
*/
          for (unsigned i = 0; i < s.size(); ++i)
            if (s[i] == SIGNAL_MAX_ONE) {
              effective[i].first = MIN_WEIGHTING;
            } else {
              effective[i].first = MAX_WEIGHTING;
            }
          break;
        }
        break;

      case SIGNAL_NO_SIGNAL:
        switch (mid_sig) {

        case SIGNAL_NO_SIGNAL:
/*
0  0  +  => 0,0,1    0,0,1  ?,?,0
0  0  +1 => 0,0,1    0,0,1  ?,?,?
*/
          for (unsigned i = 0; i < s.size(); ++i)
            if (s[i] == SIGNAL_NO_SIGNAL)
              effective[i].first = MIN_WEIGHTING;
            else {
              effective[i].first = MAX_WEIGHTING;
              if (s[i] == SIGNAL_ONE)
                effective[i].second = MIN_WEIGHTING;
            }
          break;

        case SIGNAL_ONE:
/*
0  +  +  => 0,?,?    0,1,0  0,0,1
0  +  +1 => 0,?,?    0,1,1  ?,0,?
*/
          for (unsigned i = 0; i < s.size(); ++i)
            if (max_sig == SIGNAL_ONE) {
              if (s[i] == SIGNAL_NO_SIGNAL) {
                effective[i].first = MIN_WEIGHTING;
                effective[i].second = MIN_WEIGHTING;
              } else {
                if (toggle) {
                  effective[i].first = MIN_WEIGHTING;
                  effective[i].second = MAX_WEIGHTING;
                } else {
                  effective[i].first = MAX_WEIGHTING;
                  effective[i].second = MIN_WEIGHTING;
                }
                toggle = true;
              }
            } else {
              if (s[i] == SIGNAL_NO_SIGNAL) {
                effective[i].first = MIN_WEIGHTING;
              } else {
                effective[i].first = MAX_WEIGHTING;
                if (s[i] == SIGNAL_ONE)
                  effective[i].second = MIN_WEIGHTING;
              }
            }
          break;

        case SIGNAL_MIN_ONE:
/*
0  +1 +1 => 0,?,?    0,1,1  ?,?,?
*/
          for (unsigned i = 0; i < s.size(); ++i)
            if (s[i] == SIGNAL_NO_SIGNAL)
              effective[i].first = MIN_WEIGHTING;
            else
              effective[i].first = MAX_WEIGHTING;
          break;
        }
        break;

      case SIGNAL_ONE:
        switch (mid_sig) {

        case SIGNAL_ONE:
/*
+  +  +1 => ?,?,?    0,0,1  ?,?,?
*/
          for (unsigned i = 0; i < s.size(); ++i)
            if (s[i] == SIGNAL_ONE) {
              effective[i].first = MIN_WEIGHTING;
            } else {
              effective[i].first = MAX_WEIGHTING;
            }
          break;

        case SIGNAL_MIN_ONE:
/*
+  +1 +1 => ?,?,?    ?,?,?  0,?,?
*/
          for (unsigned i = 0; i < s.size(); ++i)
            if (s[i] == SIGNAL_ONE) {
              effective[i].second = MIN_WEIGHTING;
            }
          break;

        }
        break;
      } // switch (min_sig)

    } // remaining == 2

  } // min_sig != max_sig

  // process the non-signal weightings

  // how many players to weight
  int weight_first  = 0;
  int weight_second = 0;
  // min/max weighting
  int min_w = INT_MAX;
  int max_w = INT_MIN;

  // calculate number of players to weight and the min and max weighting
  for (unsigned i = 0; i < w.size(); ++i)
    if (w[i] != INT_MIN) {
      if (effective[i].first == INT_MIN) {
        ++weight_first;
        if (w[i] > max_w) {
          max_w = w[i];
          max_i = i;
        }
        if (w[i] < min_w) {
          min_w = w[i];
          min_i = i;
        }
      }
      if (effective[i].second == INT_MIN)
        ++weight_second;
    }

  if (!weight_first && !weight_second)
    // cards fully determined by signals, we're done
    return effective;

  bool single_weighting = (remaining == 1) || (weight_first != weight_second);

  if (weight_first == 2) {

    // only two players can have the card(s)
    if (max_w == min_w) {
      int max_eff = (max_w < SINGLE_CARD_WEIGHT / 2) ? SINGLE_CARD_WEIGHT / 2 : max_w;
      for (unsigned i = 0; i < w.size(); ++i)
        if ((w[i] != INT_MIN) && (effective[i].first == INT_MIN))
          if (single_weighting)
            effective[i].first = max_eff;
          else
            if (for_distribution)
              effective[i] = make_pair(max_eff, max_eff / 2);
            else if (max_i == i)
              effective[i] = make_pair(max_eff, 0);
            else
              effective[i] = make_pair(0, max_eff);

    } else {
      if (single_weighting) {
        effective[max_i].first = max_w - min_w;
        effective[min_i].first = min_w - max_w;
      } else {
        effective[max_i] = make_pair(max_w - min_w, (max_w - min_w) / 2);
        effective[min_i] = make_pair(min_w - max_w, (min_w - max_w) / 2);
      }
    }

  } else if ((weight_first == 3) && (max_w == min_w)) {

    // all three players have the same weighting
    for (unsigned i = 0; i < w.size(); ++i)
      if (single_weighting)
        effective[i].first = 0;
      else
        effective[i] = make_pair(SINGLE_CARD_WEIGHT / 3, -SINGLE_CARD_WEIGHT / 3);

  } else if (weight_first == 3) {

    // the middle value
    int mid_i = 3 - min_i - max_i;
    int mid_w = w[mid_i];

    vector<int> norm_w(w);
    vector<int> bonus(3, 0);

    // normalize weightings
    for (unsigned i = 0; i < w.size(); ++i) {
      // max => 0
      norm_w[i] = w[i] - max_w;
      if ((max_w > 0) && (min_w < 0) && (w[i] > 0))
        // increase difference between positive and negative weightings
        bonus[i] = w[i];
    }

    int norm_min = norm_w[min_i];
    int norm_mid = norm_w[mid_i];
    int max_eff = bonus[max_i] - norm_min;

    if (mid_w == max_w) {
      // case #1: max, max, min
      if (single_weighting) {
        effective[max_i].first = max_eff;
        effective[mid_i].first = max_eff;
        effective[min_i].first = norm_min;
      } else {
        if (for_distribution) {
          effective[max_i] = make_pair(max_eff, max_eff / 2);
          effective[mid_i] = make_pair(max_eff, max_eff / 2);
          effective[min_i] = make_pair(norm_min, norm_min / 2);
        } else {
          effective[max_i] = make_pair(max_eff, bonus[max_i]);
          effective[mid_i] = make_pair(bonus[mid_i], max_eff);
          effective[min_i] = make_pair(norm_min, norm_min);
        }
      }
    } else if (mid_w == min_w) {
      // case #2: max, min, min
      if (single_weighting) {
        effective[max_i].first = max_eff;
        effective[mid_i].first = norm_min;
        effective[min_i].first = norm_min;
      } else {
        if (for_distribution) {
          effective[max_i] = make_pair(max_eff, max_eff / 2);
          effective[mid_i] = make_pair(norm_min, norm_min / 2);
          effective[min_i] = make_pair(norm_min, norm_min / 2);
        } else {
          effective[max_i] = make_pair(max_eff, max_eff / 2);
          effective[mid_i] = make_pair(norm_min, norm_min / 2);
          effective[min_i] = make_pair(norm_min / 2, norm_min);
        }
      }
    } else {
      // case #3: max, mid, min
      if (single_weighting) {
        effective[max_i].first = bonus[max_i] - norm_mid;
        effective[mid_i].first = bonus[mid_i] + norm_mid;
        effective[min_i].first = norm_min;
      } else {
        effective[max_i] = make_pair(max_eff, bonus[max_i] - norm_mid);
        effective[mid_i] = make_pair(bonus[mid_i] + norm_mid, bonus[mid_i] + (norm_mid - norm_min));
        effective[min_i] = make_pair(norm_min - norm_mid, norm_min);
      }
    }

  } // weight first card

  if ((remaining == 1) || !weight_second || (weight_second == weight_first))
    // no second card to weight or it has already been weighted, we're done
    return effective;


  // reset min/max weighting and the corresponing indices
  min_w = INT_MAX;
  max_w = INT_MIN;

  // calculate number of players to weight and the min and max weighting
  for (unsigned i = 0; i < w.size(); ++i)
    if (w[i] != INT_MIN) {
      if (effective[i].second == INT_MIN) {
        if (w[i] > max_w) {
          max_w = w[i];
          max_i = i;
        }
        if (w[i] < min_w) {
          min_w = w[i];
          min_i = i;
        }
      }
    }

  if (weight_second == 2) {

    // only two players can have the card(s)
    if (max_w == min_w) {
      int max_eff = (max_w < SINGLE_CARD_WEIGHT / 2) ? SINGLE_CARD_WEIGHT / 2 : max_w;
      for (unsigned i = 0; i < w.size(); ++i)
        if ((w[i] != INT_MIN) && (effective[i].second == INT_MIN))
          effective[i].second = max_eff;

    } else {
      effective[max_i].second = max_w - min_w;
      effective[min_i].second = min_w - max_w;
    }

  } else if ((weight_second == 3) && (max_w == min_w)) {

    // all three players have the same weighting
    for (unsigned i = 0; i < w.size(); ++i)
      effective[i].second = 0;

  } else if (weight_second == 3) {

    // the middle value
    int mid_i = 3 - min_i - max_i;
    int mid_w = w[mid_i];

    vector<int> norm_w(w);
    vector<int> bonus(3, 0);

    // normalize weightings
    for (unsigned i = 0; i < w.size(); ++i) {
      // max => 0
      norm_w[i] = w[i] - max_w;
      if ((max_w > 0) && (min_w < 0) && (w[i] > 0))
        // increase difference between positive and negative weightings
        bonus[i] = w[i];
    }

    int norm_min = norm_w[min_i];
    int norm_mid = norm_w[mid_i];
    int max_eff = bonus[max_i] - norm_min;

    if (mid_w == max_w) {
      // case #1: max, max, min
      effective[max_i].second = max_eff;
      effective[mid_i].second = max_eff;
    } else if (mid_w == min_w) {
      // case #2: max, min, min
      effective[max_i].second = max_eff;
      effective[mid_i].second = norm_min;
    } else {
      // case #3: max, mid, min
      effective[max_i].second = bonus[max_i] - norm_mid;
      effective[mid_i].second = bonus[mid_i] + norm_mid;
    }
    effective[min_i].second = norm_min;

  } // weight second card


  return effective;

} // void process_weightings(...)


#ifdef CARD_SIGNALS_DEBUG

/////////////
// TESTING //
/////////////

CardSignal
int_to_signal(int i)
{
  switch (i) {
  case 0:
    return SIGNAL_NONE;
  case 1:
    return SIGNAL_MAX_ONE;
  case 2:
    return SIGNAL_NO_SIGNAL;
  case 3:
    return SIGNAL_ONE;
  case 4:
    return SIGNAL_MIN_ONE;
  case 5:
    return SIGNAL_BOTH;
  }
}

string
shortname(int signal)
{
  using namespace CARD_SIGNALS;

  switch (signal) {
  case SIGNAL_NONE:
    return "--";
  case SIGNAL_MAX_ONE:
    return "- ";
  case SIGNAL_NO_SIGNAL:
    return "0 ";
  case SIGNAL_ONE:
    return "+ ";
  case SIGNAL_MIN_ONE:
    return "+1";
  case SIGNAL_BOTH:
    return "++";
  }

  DEBUG_ASSERTION(false,
		  "::name(CardSignal):\n"
		  "  signal not found: " << static_cast<int>(signal));

  return "";
} // string shortname(...)

string
code(int c)
{
  using namespace CARD_SIGNALS;

  switch (c) {
  case MAX_WEIGHTING:
    return "1";
  case MIN_WEIGHTING:
    return "0";
  default:
    return "?";
  }
} // string shortname(...)


int
encode(int c)
{
  switch (c) {
  case MAX_WEIGHTING:
    return MAX_WEIGHTING;
  case MIN_WEIGHTING:
    return MIN_WEIGHTING;
  default:
    return 0;
  }
} // string shortname(...)


string
result(int w)
{
  stringstream out;

  switch (w) {
  case MAX_WEIGHTING:
    return "MAX";
  case MIN_WEIGHTING:
    return "MIN";
  case INT_MIN:
    return "???";
  case INT_MAX:
    return "XXX";
  default:
    out << w;
    return out.str();
  }
}

int
estimate_numberof(int p, int remaining, vector<pair<int,int> > const& w)
{
  int n = 0;
  if (w[p].first == MAX_WEIGHTING)
    ++n;
  else if ((w[p].first > MIN_WEIGHTING) && (w[p].first >= SINGLE_CARD_WEIGHT)) {
    bool has_card = true;
    for (unsigned i = 0; i < w.size(); ++i)

      if ((i != p) && (w[p].first < w[i].first + (3-remaining) * SINGLE_CARD_WEIGHT)) {
        has_card = false;
        break;
      }
    n += has_card;
  }

  if (remaining > 1)
    if (w[p].second == MAX_WEIGHTING)
      ++n;
    else if ((w[p].second > MIN_WEIGHTING)  && (w[p].second >= SINGLE_CARD_WEIGHT)) {
      bool has_card = true;
      for (unsigned i = 0; i < w.size(); ++i)
        if ((i != p) && (w[p].second < w[i].second + SINGLE_CARD_WEIGHT)) {
          has_card = false;
          break;
        }
      n += has_card;
    }

  return n;
}

void
show_results(ostream &out, vector<int> const& w, int width)
{
  vector<pair<int,int> > effective = process_weightings(w, 1, -1);

  for (int i = 0; i < 3; ++i)
    out << (i == 0 ? " " : ",") << setw(width) << result(effective[i].first);
  out << " #";
  for (int i = 0; i < 3; ++i)
    out << (i == 0 ? "" : ",") << estimate_numberof(i, 1, effective);

  effective = process_weightings(w, 2, -1);
  out << "   ";
  for (int i = 0; i < 3; ++i)
    out << (i == 0 ? " " : ",") << setw(width) << result(effective[i].first);
  out << " ";
  for (int i = 0; i < 3; ++i)
    out << (i == 0 ? " " : ",") << setw(width) << result(effective[i].second);
  effective = process_weightings(w, 2, -1, false);
  out << " #";
  for (int i = 0; i < 3; ++i)
    out << (i == 0 ? "" : ",") << estimate_numberof(i, 2, effective);
  out << " ";
  for (int i = 0; i < 3; ++i)
    out << (i == 0 ? " " : ",") << setw(width) << result(effective[i].first);
  out << " ";
  for (int i = 0; i < 3; ++i)
    out << (i == 0 ? " " : ",") << setw(width) << result(effective[i].second);
  out << endl;

}



/**
 ** generates a list of the effective weightings for all possible combinations of card-signals
 ** and non-signal weightings in the given range and writes it to the given output stream.
 **
 ** @param     &out          output stream
 ** @param     lower_bound   lower bound of the non-signal weightings range to be tested
 ** @param     upper_bound   upper bound of the non-signal weightings range to be tested
 ** @param     step_size     step size for the non-signal weightings
 **
 ** @return    -
 **
 ** @author    Tassilo Kirsch
 **
 ** @version   0.7.6
 **/
void
test_process_weightings(ostream &out, int lower_bound, int upper_bound, unsigned step_size)
{

/*
First a list of all possible signal combinations and the type of the resulting effective weightings is generated:

card signal combinations
------------------------
-- -- -- => ?,?,?    ?,?,?  ?,?,?
-- -- -  => ?,?,1    0,0,1  ?,?,0
-- -- 0  => 0,0,1    0,0,1  0,0,1
-- -- +  => 0,0,1    0,0,1  ?,?,0
-- -- +1 => 0,0,1    0,0,1  0,0,1
-- -- ++ => 0,0,1    0,0,1  0,0,1
[...]

signals:
--: SIGNAL_NONE
- : SIGNAL_MAX_ONE
0 : SIGNAL_NO_SIGNAÖ
+ : SIGNAL_ONE
+1: SIGNAL_MIN_ONE
++: SIGNAL_BOTH

results:
?: use non-signal weightings
0: no card  (MIN_WEIGHTING)
1: one card (MAX_WEIGHTING)

columns:
                      effective weightings for
signal combination => only copy    first copy second copy


Then for each card signal combination a list is generated which shows all possible combinations of the non-signal
weightings between lower_bound and upper_bound, using step_size:

-- +  +  => 0,?,?    0,1,0  0,0,1
---------------------------------
-210 -210 -210 =>  MIN,  50,  50 #0,0,0     MIN, MAX, MIN   MIN, MIN, MAX #0,1,1   MIN, MAX, MIN   MIN, MIN, MAX
-210 -210 -140 =>  MIN, -70,  70 #0,0,0     MIN, MAX, MIN   MIN, MIN, MAX #0,1,1   MIN, MAX, MIN   MIN, MIN, MAX
-210 -210  -70 =>  MIN,-140, 140 #0,0,1     MIN, MAX, MIN   MIN, MIN, MAX #0,1,1   MIN, MAX, MIN   MIN, MIN, MAX
-210 -210    0 =>  MIN,-210, 210 #0,0,1     MIN, MAX, MIN   MIN, MIN, MAX #0,1,1   MIN, MAX, MIN   MIN, MIN, MAX
-210 -210   70 =>  MIN,-280, 280 #0,0,1     MIN, MAX, MIN   MIN, MIN, MAX #0,1,1   MIN, MAX, MIN   MIN, MIN, MAX
-210 -210  140 =>  MIN,-350, 350 #0,0,1     MIN, MAX, MIN   MIN, MIN, MAX #0,1,1   MIN, MAX, MIN   MIN, MIN, MAX
-210 -210  210 =>  MIN,-420, 420 #0,0,1     MIN, MAX, MIN   MIN, MIN, MAX #0,1,1   MIN, MAX, MIN   MIN, MIN, MAX
[...]

-- -  +  => 0,0,1    0,0,1  0,1,0
---------------------------------
 ALL  ALL  ALL =>  MIN, MIN, MAX #0,0,1     MIN, MIN, MAX   MIN, MAX, MIN #0,1,1   MIN, MIN, MAX   MIN, MAX, MIN

range: -210, 210, step = 70

MIN: no card  (MIN_WEIGHTING)
MAX: one card (MAX_WEIGHTING)

columns:
weighting combination (ALL ALL ALL means that the results are fully determined by signals)
each player's effective weighting if only one copy of the card is left
each player's estimated number of copies only one copy of the card is left (starts with '#')
each player's effective weighting if both copies of the card are left
each player's estimated number of copies if both copies of the card are left(starts with '#')
each player's effective weighting if both copies of the card are left using for_distribution == false

If different signal combinations have identical results, the non-signal list will be generated only for the
first signal combination and subsequent combinations will be listed as

-- -  +1 => 0,0,1    0,0,1  0,?,? => same as -- -  0

WARNING: select range and stepping carefully!
With steps = ((upper_bound - lower_bound) / step_size) + 1 the number of lines for each non-signal list is
(3 + steps - 1)! / (3! * (steps-1)!) and there are 56 possible card signal combinations...

*/

  out << endl
      << "card signal combinations" << endl
      << "------------------------" << endl;

  vector<int> w(3);

  for (int p0 = 0; p0 < 6; ++p0)
    for (int p1 = p0; p1 < 6; ++p1)
      for (int p2 = p1; p2 < 6; ++p2) {
        w[0] = int_to_signal(p0);
        w[1] = int_to_signal(p1);
        w[2] = int_to_signal(p2);

        out << shortname(w[0]) << " " << shortname(w[1]) << " " << shortname(w[2]) << " =>";

        vector<pair<int, int> > effective;
        effective = process_weightings(w, 1, -1);
        for (int i = 0; i < 3; ++i)
          out << (i == 0 ? " " : ",") << code(effective[i].first);

        effective = process_weightings(w, 2, -1);
        out << "   ";
        for (int i = 0; i < 3; ++i)
          out << (i == 0 ? " " : ",") << code(effective[i].first);
        out << " ";
        for (int i = 0; i < 3; ++i)
          out << (i == 0 ? " " : ",") << code(effective[i].second);

        out << endl;
      }

  out << endl;
  out << endl;

  int step = step_size;

  lower_bound = strip_signal(lower_bound);
  upper_bound = strip_signal(upper_bound);
  if (lower_bound > upper_bound)
    std::swap(lower_bound, upper_bound);

  // check non-signal stepping
  if (   !step_size
      || (step < 0) // unsigned -> int overflow
     )
  {
    out << "invalid stepping" << endl;
    return;
  }

  if ((upper_bound - lower_bound) / step > 100)
  {
    out << "too many combinations to process" << endl;
    return;
  }


  int width = 3; // minimun width, see function result()
  if (lower_bound <= -1000)
    width = 5;
  else if ((lower_bound <= -100) || (upper_bound >= 1000))
    width = 4;

  vector<vector<int> > types;

  for (int s0 = 0; s0 < 6; ++s0)
    for (int s1 = s0; s1 < 6; ++s1)
      for (int s2 = s1; s2 < 6; ++s2) {
        w[0] = int_to_signal(s0);
        w[1] = int_to_signal(s1);
        w[2] = int_to_signal(s2);

        out << shortname(w[0]) << " " << shortname(w[1]) << " " << shortname(w[2]) << " =>";
        vector<int> t;

        bool fully_determined = true;

        vector<pair<int, int> > effective;
        effective = process_weightings(w, 1, -1);
        for (int i = 0; i < 3; ++i) {
          out << (i == 0 ? " " : ",") << code(effective[i].first);
          t.push_back(encode(effective[i].first));
          if (!encode(effective[i].first))
            fully_determined = false;
        }
        effective = process_weightings(w, 2, -1);
        for (int i = 0; i < 3; ++i)  {
          t.push_back(encode(effective[i].first));
          if (!encode(effective[i].first))
            fully_determined = false;
          t.push_back(encode(effective[i].second));
          if (!encode(effective[i].second))
            fully_determined = false;
        }
        out << "   ";
        for (int i = 0; i < 3; ++i)
          out << (i == 0 ? " " : ",") << code(effective[i].first);
        out << " ";
        for (int i = 0; i < 3; ++i)
          out << (i == 0 ? " " : ",") << code(effective[i].second);

        bool is_new = true;
        for (int i = 0; i < types.size(); ++i) {
          bool equal = true;
          for (int j = 0; j < 9; ++j)
            if (types[i][j] != t[j]) {
              equal = false;
              break;
            }
          if (equal) {
            out << " => same as " << shortname(types[i][9]) << " " << shortname(types[i][10]) << " " << shortname(types[i][11]);
            is_new = false;
            break;
          }
        }

        out << endl;

        if (!is_new) {
          out << endl;
          continue;
        }

        t.push_back(w[0]);
        t.push_back(w[1]);
        t.push_back(w[2]);
        types.push_back(t);

        out << "---------------------------------" << endl;

        if (fully_determined) {

          out << setw(width) << "ALL" << " " << setw(width) << "ALL" << " " << setw(width) << "ALL" << " =>";
          show_results(out, w, width);

        } else {


          for (int p0 = lower_bound; p0 <= upper_bound; p0 += step)
            for (int p1 = p0; p1 <= upper_bound; p1 += step)
              for (int p2 = p1; p2 <= upper_bound; p2 += step) {
                w[0] = p0 + int_to_signal(s0);
                w[1] = p1 + int_to_signal(s1);
                w[2] = p2 + int_to_signal(s2);
                out << setw(width) << p0 << " " << setw(width) << p1 << " " << setw(width) << p2 << " =>";
                show_results(out, w, width);
              }

        }
        out << endl;
        out << endl;
      }

  out << endl;

  return;

}

#endif // CARD_SIGNALS_DEBUG


/**
 ** converts a weighting to a string
 ** if non_signal_width is greater than 0, an internal alignment is used, with the signal
 ** code to the left and the non-signal portion of the weighting to the right
 **
 ** SIGNAL_NONE      -> "[--]"
 ** SIGNAL_MAX_ONE   -> "[- ]"
 ** SIGNAL_NO_SIGNAL -> "" (blank)
 ** SIGNAL_ONE       -> "[+-}"
 ** SIGNAL_MIN_ONE   -> "[+ ]"
 ** SIGNAL_BOTH      -> "[++]"
 ** illegal signal   -> "???"
 **
 ** @param     weighting               the weighting
 ** @param     non_signal_width        output width of the non-signal portion of the weighting
 ** @param     signal_and_weighting    true:  always show signal and non-signal portion of the weighting
 **                                    false: show signal only if a signal is set, show non-signal portion only if no signal is set
 **
 ** @return    string representation of the weighting
 **
 ** @author    Tassilo Kirsch
 **
 ** @version   0.7.6
 **/
string
weighting_to_string(int weighting, unsigned non_signal_width, bool signal_and_weighting)
{
  stringstream out;

  if ((weighting < MIN_WEIGHTING) || (weighting > MAX_WEIGHTING)) {

    // invalid weighting
    out << "??? " << setw(non_signal_width) << weighting;

  } else {

    CardSignal s = weighting_to_signal(weighting);
    int w = strip_signal(weighting);

    if (signal_and_weighting || (s != SIGNAL_NO_SIGNAL)) {
      out << setw(non_signal_width ? 4 + signal_and_weighting : 0) << left;
      switch (s) {
      case SIGNAL_NONE:
        out << "[--]";
        break;
      case SIGNAL_MAX_ONE:
        out << "[- ]";
        break;
      case SIGNAL_NO_SIGNAL:
        out << "";
        break;
      case SIGNAL_ONE:
        out << "[+-]";
        break;
      case SIGNAL_MIN_ONE:
        out << "[+ ]";
        break;
      case SIGNAL_BOTH:
        out << "[++]";
        break;
      }
    }

    if (signal_and_weighting || (s == SIGNAL_NO_SIGNAL)) {
      out << setw(non_signal_width);
      if (w > 0)
        out << showpos;
      out << right << w;
    }

  } // valid weighting

  return out.str();

} // string weighting_to_string(...)


/**
 ** converts an effective weighting to a string
 ** MAX_WEIGHTING -> "[+]"
 ** MIN_WEIGHTING -> "[-]"
 ** illegal value -> "???"
 ** other values  -> number
 **
 ** @param     effective    the effective weighting
 **
 ** @return    string representation of an effective weighting
 **
 ** @author    Tassilo Kirsch
 **
 ** @version   0.7.6
 **/
string
effective_weighting_to_string(int effective)
{

  switch (effective) {
  case MAX_WEIGHTING:
    return "[+]";
  case MIN_WEIGHTING:
    return "[-]";
  case INT_MIN:
  case INT_MAX:
    return "???";
  default: {
    stringstream out;
    if (effective > 0)
      out << showpos;
    out << effective;
    return out.str();
  }
  }
} // string effective_weighting_to_string(...)

} // namespace CARD_SIGNALS

/**
 ** -> return
 **
 ** @param     signal    the card signal
 **
 ** @return    the name of the card signal
 **
 ** @author    Tassilo Kirsch
 **
 ** @version   0.7.6
 **/
string
name(CardSignal const& signal)
{
  using namespace CARD_SIGNALS;

  switch (signal) {
  case SIGNAL_NONE:
    return "signal none";
  case SIGNAL_MAX_ONE:
    return "signal max one";
  case SIGNAL_NO_SIGNAL:
    return "no signal";
  case SIGNAL_ONE:
    return "signal one";
  case SIGNAL_MIN_ONE:
    return "signal min one";
  case SIGNAL_BOTH:
    return "signal both";
  }

  DEBUG_ASSERTION(false,
		  "::name(CardSignal):\n"
		  "  signal not found: " << static_cast<int>(signal));

  return "";
} // string name(...)




