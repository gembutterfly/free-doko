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

#ifndef HEADER_GTKMM_CONSTANTS
#define HEADER_GTKMM_CONSTANTS

#include "../constants.h"

#ifdef USE_UI_GTKMM

#define ALPHA_THRESHOLD 50


#include <gtkmm/base.h>

// version checking
// Copied from gtkmm/base.h (GTK_VERSION_GE).
#ifndef GTKMM_VERSION_GE
#define GTKMM_VERSION_GE(major,minor)  ((GTKMM_MAJOR_VERSION>major)||(GTKMM_MAJOR_VERSION==major)&&(GTKMM_MINOR_VERSION>=minor))
#endif


#include <glibmm.h>
// these are not defined p.e. for the internet tablet
#ifndef GLIBMM_EXCEPTIONS_ENABLED
#define GLIBMM_EXCEPTIONS_ENABLED
#endif
#ifndef GLIBMM_DEFAULT_SIGNAL_HANDLERS_ENABLED
#define GLIBMM_DEFAULT_SIGNAL_HANDLERS_ENABLED
#endif
#ifndef GLIBMM_PROPERTIES_ENABLED
#define GLIBMM_PROPERTIES_ENABLED
#endif


// for relative spaces

// used for:
// * border in notebooks
#define EM * 18
// used for:
// * spacing in boxes
#define EX * 12

#endif // #ifdef USE_UI_GTKMM

#endif // #ifndef HEADER_GTKMM_CONSTANTS
