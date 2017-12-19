/**********************************************************************
 *
 *   Some routines for effective Debugging
 *
 *   Copyright (C) 2001-2017  by Borg Enders
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
 *    Borg Enders  borg@borgsoft.de
 *
 *********************************************************************/

#include <iostream>
#include "debug.h"

//initiliaze of CIndent
int CIndent::i_indent = 0;

// initialize the pointer to the debug function
DebugFunction debug_function = NULL;
