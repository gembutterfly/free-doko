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


// All players are AI,
// there is no pause,
// no 'full trick'- and no 'game finished'-window
// change it with the '-F' option
int FAST_PLAY = 0;
int const FAST_NS::ALL = (  PAUSE
			  | PLAYER
			  | PARTY_START
			  | GAMEPLAY_INFORMATION
			  | GAME_FINISHED
			  | SHOW_ALL_HANDS
			  | HIDE_BUG_REPORT_WINDOW
			  | QUIT_WHEN_FINISHED
			  );


#if defined(DKNOF) && !defined(RELEASE)

// 3: 161411 different games
//unsigned SEED_START = 3;

//     18: marriage     0
//  12700: ?
// 248603: poverty	2	(3) (1 Karo As)
// 228801 0: poverty	3	(2)
// 217152: poverty	3 + 1   (0)
// 174565: poverty      2 + 2   (2) (ohne Neunen)
// 194192: poverty      3       (0) mit Schweine
// 189395: poverty      0       (2) 2: Schweine, 1: Hyperschweine
// 241161: swines	0
//  64100: hyperswines	3 2
//  91528: schweine + hyperschweine 0
// 117325: genscher	2
// 266129: 30 Punkte	2
// 166266: 5 Neuner     0
//  58230: club solo    0
// 245286: two spade aces  3
// 148579: five nines, next marriage
// 110184: marriage             (0)
//  95319: double poverty: (0 + 1) (je 1 Karo As)
// 23883: Königen-Damen-Solo: schwarz.
//unsigned SEED_START = UINT_MAX;
//172808:	schlechte Teambestimmung
unsigned const SEED_START = 3;
unsigned const STARTPLAYER = 0;
#endif // #ifdef DKNOF && !RELEASE

#if defined(RELEASE) || !defined(DKNOF)
unsigned const SEED_START = UINT_MAX;
unsigned const STARTPLAYER = 0;
#endif // !#ifndef RELEASE
