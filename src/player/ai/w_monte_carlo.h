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

#ifndef CLASS_W_MONTE_CARLO_HEADER
#define CLASS_W_MONTE_CARLO_HEADER

#include "../../card/hand.h"
#include "../rating/rating.h"
#include "../aiconfig.h"

class VirtualGamesInterface;
class Player;
class Trick;

/// create for each card a virtual game and look, how many points the ai makes
class WMonteCarlo {
  using Weighting = long long int;
  static HandCard card_with_max_weighting(map<HandCard, Weighting> const&
                                          weighting);

  public:
  WMonteCarlo(VirtualGamesInterface const& vgi,
              AiType ai_type,
              unsigned future_limit,
              Rating::Type rating_type);
  WMonteCarlo() = delete;
  WMonteCarlo(WMonteCarlo const&) = delete;
  WMonteCarlo& operator=(WMonteCarlo const&) = delete;
  ~WMonteCarlo() = default;

  Card best_card();

  private:

  // calculate the weightings for the cards of the hand
  map<HandCard, Weighting> weightings_calc();

  // the cards to check
  HandCards cards() const;
  void play_till_end(Game& game_virt) const;
  Weighting weighting(Game const& game_virt, HandCard const& card) const;

  private:
  VirtualGamesInterface const* vgi = nullptr;
  AiType const ai_type;
  unsigned const future_limit = 0;
  Rating::Type const rating_type;
}; // class WMonteCarlo 

#endif // Header Guard
