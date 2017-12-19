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

#ifndef CLASS_GAMEPLAY_ACTION_POVERTY_HEADER
#define CLASS_GAMEPLAY_ACTION_POVERTY_HEADER

#include "pure.h"
#include "player1.h"
#include "base_cards.h"


GAMEPLAY_ACTION_BASE_CARDS(GameplayAction::PovertyShift, PovertyShift, POVERTY_SHIFT)
GAMEPLAY_ACTION_PLAYER1(GameplayAction::PovertyAccepted, PovertyAccepted, POVERTY_ACCEPTED)
GAMEPLAY_ACTION_BASE_CARDS(GameplayAction::PovertyReturned, PovertyReturned, POVERTY_RETURNED)
GAMEPLAY_ACTION_PLAYER1(GameplayAction::PovertyDenied, PovertyDenied, POVERTY_DENIED)
GAMEPLAY_ACTION_PURE(GameplayAction::PovertyDeniedByAll, PovertyDeniedByAll, POVERTY_DENIED_BY_ALL)

#endif // #ifndef CLASS_GAMEPLAY_ACTION_POVERTY_HEADER
