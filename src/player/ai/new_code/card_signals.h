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

#ifndef CARD_SIGNALS_HEADER
#define CARD_SIGNALS_HEADER

#include "../basistypes.h"

#ifndef RELEASE
// #define CARD_SIGNALS_DEBUG
#endif

namespace CARD_SIGNALS {

  // Card signals to be used within the card weighting process (see CardsInformation::OfPlayer).
  // The signals do not interfere with the non-signal weightings. This way it's possible to
  // fall back to the non-signal weighting once a signal turns out to be fake.

  // to preserve non-signal weightings, each signal is represented by a range of weightings
  const int SIGNAL_RANGE = 10000;
  // the actual range is ]signal - SIGNAL_EPSILON .. signal + SIGNAL_EPSILON]
  const int SIGNAL_EPSILON = SIGNAL_RANGE / 2;

  // the possible card signals
  enum CardSignal {
    // player has signalled that he does not have the card
    SIGNAL_NONE      = -2 * SIGNAL_RANGE,
    // player has signalled that he does not have both copies of the card
    SIGNAL_MAX_ONE   = -1 * SIGNAL_RANGE,
    // no signal has been given
    SIGNAL_NO_SIGNAL =  0 * SIGNAL_RANGE,
    // player has signalled to have exactly one copy of the card
    SIGNAL_ONE       =  1 * SIGNAL_RANGE,
    // player has signalled to have at least one copy of the card
    SIGNAL_MIN_ONE   =  2 * SIGNAL_RANGE,
    // player has signalled to have both copies of the card
    SIGNAL_BOTH      =  3 * SIGNAL_RANGE
  };


  // as a result, there are maximum and minimum weightings which should never be exceeded
  const int MAX_WEIGHTING = SIGNAL_BOTH + SIGNAL_EPSILON;
  const int MIN_WEIGHTING = SIGNAL_NONE - SIGNAL_EPSILON + 1;

  // returns the signal represented by the given weighting
  CardSignal weighting_to_signal(int weighting);

  // returns a weighting representing the given signal, preserving non-signal weightings in old_weighting
  int signal_to_weighting(CardSignal signal, int old_weighting);

  // the number of cards signalled by weighting
  unsigned signalled_cards(int weighting);

  // strips off a signal and returns the remaining non-signal weighting
  int strip_signal(int weighting);

  // adds new_weighting (either a signal or a non-signal weighting, not both) to old_weighting,
  // preserves non-signal weightings in old_weighting, prevents inadvertently changing a signal
  // and makes shure that the result isn't greater than MAX_WEIGHTING or lesser than MIN_WEIGHTING
  int add_weighting(int old_weighting, int new_weighting);

  // adjusts a weighting (including signals) when the first copy of the corresponding card
  // has been played or its owner is known (first_card_owner determines if it's the weighting
  // of the card's player/owner or that of another player)
  int adjust_weighting_for_second_card(int weighting, bool first_card_owner);

  // non-signal weighting which represents the assumption that a player has one copy of a card
  const int SINGLE_CARD_WEIGHT = 100;

  // Processes a card's weightings to calculate the effective weightings used either to
  // distribute the card or to estimate the number of copies in a player's hand
  vector<pair<int,int> > process_weightings(vector<int> const& weightings,
                                            int remaining,
                                            int musthave_player,
                                            bool for_distribution = true);


#ifdef CARD_SIGNALS_DEBUG

  // generates a list of the effective weightings for all possible combinations of card-signals
  // and non-signal weightings in the given range and writes it to the given output stream
  // see implementation for details
  void test_process_weightings(ostream &out, int lower_bound, int upper_bound, unsigned step_size);

#endif

  // converts a weighting to a string
  string weighting_to_string(int weighting, unsigned non_signal_width = 0, bool signal_and_weighting = true);

  // converts an effective weighting to a string
  string effective_weighting_to_string(int effective);

} // namespace CARD_SIGNALS

using CARD_SIGNALS::CardSignal;

string name(CardSignal const& signal);
WRITE_NAME(CardSignal)

#endif  // #ifndef CARD_SIGNALS_HEADER
