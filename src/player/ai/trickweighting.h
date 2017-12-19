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

#ifndef CLASS_TRICKWEIGHTING_HEADER
#define CLASS_TRICKWEIGHTING_HEADER

#include "../../card/hand.h"
#include "ai.h"

class VirtualGamesInterface;
class Player;


class TrickWeighting {
  public:
    // returns the weighting of the trick for 'team'
    static int modi(VirtualGamesInterface const& vgi,
                    Trick const& trick,
                    Team const team,
                    HandCard const& card);
    static int backhand(VirtualGamesInterface const& vgi,
                        HandCard const& c,
                        Game const& game_virt);
  private:
    static int modi_dolle_swines(VirtualGamesInterface const& vgi,
                                 Trick const& trick,
                                 Team const team,
                                 HandCard const& card, Team winnerteam, Ai const& ai);
    static int modi_queen(VirtualGamesInterface const& vgi,
                          Trick const& trick,
                          Team const team,
                          HandCard const& card, Team winnerteam, Ai const& ai);
    static int modi_jack(VirtualGamesInterface const& vgi,
                          Trick const& trick,
                          Team const team,
                          HandCard const& card, Team winnerteam, Ai const& ai);

    static int modi_soli_meat_less(VirtualGamesInterface const& vgi,
                                   Trick const& trick,
                                   Team const team,
                                   HandCard const& card,  Team winnerteam, Ai const& ai);

    static int modi_marriage(VirtualGamesInterface const& vgi,
                                       Trick const& trick,
                                       Team const team,
                                       HandCard const& card,  Team winnerteam, Ai const& ai);


    static int modi_fox(VirtualGamesInterface const& vgi,
                                       Trick const& trick,
                                       Team const team,
                                       HandCard const& card,  Team winnerteam, Ai const& ai);

    static int modi_special_points(VirtualGamesInterface const& vgi,
                                          Trick const& trick,
                                          Team const team,
                                          HandCard const& card,  Team winnerteam, Ai const& ai);

    static int modi_no_trump(VirtualGamesInterface const& vgi,
                                              Trick const& trick,
                                              Team const team,
                                              HandCard const& card,  Team winnerteam, Ai const& ai);


    static int modi_picture_soli(VirtualGamesInterface const& vgi,
                                              Trick const& trick,
                                              Team const team,
                                              HandCard const& card,  Team winnerteam, Ai const& ai);

    static int modi_other_trumps(VirtualGamesInterface const& vgi,
        Trick const& trick,
        Team const team,
        HandCard const& card,  Team winnerteam, Ai const& ai);

    static int modi_force_trump(VirtualGamesInterface const& vgi,
            Trick const& trick,
            Team const team,
            HandCard const& card,  Team winnerteam, Ai const& ai);

    static int modi_poverty(VirtualGamesInterface const& vgi,
              Trick const& trick,
              Team const team,
              HandCard const& card,  Team winnerteam, Ai const& ai);

}; // class TrickWeighting

#endif //headerguard
