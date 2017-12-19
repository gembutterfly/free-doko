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

#ifndef HEADER_CONSTANTS
#define HEADER_CONSTANTS

// using the microsoft visual C++ compiler
#ifdef _MSC_VER
#define MS_VISUAL_C
#define WINDOWS
#endif

// C++ include

#include <typeinfo>
#include <iostream>
using std::ostream;
using std::istream;
using std::cout;
using std::clog;
using std::cerr;
using std::endl;
using std::flush;
#include <iomanip>
using std::setw;
#include <fstream>
using std::ifstream;
using std::ofstream;
#include <sstream>
using std::stringstream;
using std::istringstream;
using std::ostringstream;
#include <algorithm>
using std::min;
using std::max;

// for temporary output
// ToDo: use null_str in the release
#define COUT cout
#define CLOG cout << __FILE__ << '#' << __LINE__ << "  "

#define LOGreturn if(CLOG << "return" << endl, true) return


using std::pair;
#include <vector>
using std::vector;
#include <queue>
using std::queue;
#include <map>
using std::map;
#include <set>
using std::set;
#include <string>
using std::string;
using namespace std::literals::string_literals;
#include <memory>
using std::unique_ptr;
using std::make_unique;


// C functions
#include <cmath>
#include <climits>

#ifdef WINDOWS
#define USE_REGISTRY
#endif

#ifdef WINDOWS
#ifdef USE_REGISTRY
#include <windows.h>
#undef min
#undef max
#endif
#endif


#ifndef MS_VISUAL_C
#include <dirent.h>
#endif


#include "utils/version.h"


// Whether GTKMM can be used.
// When not defined, FreeDoko should compile and run without any GTKMM library
// this makro shall be defined through the compiler (in the makefile)
//#define GTKMM


// code that is outdated -- leave this macro undefined
#undef OUTDATED
// code, that is postponed -- leave this macro undefined
#undef POSTPONED

// code that is deprecated -- leave this macro defined
#define DEPRECATED

// code that is workaround -- leave this macro defined
#define WORKAROUND

// code, that is an alternative -- leave this macro undefined
#undef ALTERNATIVE


extern char const* const GPL_string;
extern DK::Utils::Version const* version;
extern std::vector<DK::Utils::Version const*> all_versions;
#define CONTACT "freedoko@users.sourceforge.net"
#define PROGRAM_NAME "FreeDoko"


// In the release-version there are some elements hidden
//#define RELEASE
//#define VERSION_DESCRIPTION "preversion"

#ifdef RELEASE
#define NO_CRITICAL_CHECKS
#endif


// whether to check the runtime
#if !defined(RELEASE) && !defined(USE_THREADS)
//#define CHECK_RUNTIME
#endif
#ifdef CHECK_RUNTIME
#include "runtime.h"
#endif


#ifndef USE_NETWORK
#ifdef USE_NETWORK_DOKOLOUNGE
#undef USE_NETWORK_DOKOLOUNGE
#endif
#endif
#if __cplusplus < 201103L
  //for (auto p : this->of_player_)
#ifdef USE_NETWORK_DOKOLOUNGE
#undef USE_NETWORK_DOKOLOUNGE
#endif
#endif


namespace FAST_NS {
  enum BIT {
    NONE	         = 0,
    PARTY_START          = 1,                         //   1 automatical start party
    PLAYER               = PARTY_START          << 1, //   2 change human players to ais
    RANDOM_AI            = PLAYER               << 1, //   4 set the players to the random ai
    PAUSE                = RANDOM_AI            << 1, //   8 skip pauses
    GAMEPLAY_INFORMATION = PAUSE                << 1, //  16 skip gameplay information dialogs
    GAME_FINISHED        = GAMEPLAY_INFORMATION << 1, //  32 skip game finished dialog
    PARTY_FINISHED        = GAME_FINISHED       << 1, //  64 skip party finished dialog
    SHOW_ALL_HANDS      = PARTY_FINISHED        << 1, // 128 show all hands
    HIDE_BUG_REPORT_WINDOW     = SHOW_ALL_HANDS << 1, // 256 do not show the bug report replay window
    QUIT_WHEN_FINISHED = HIDE_BUG_REPORT_WINDOW << 1, // 512 quit when finished
    SEED_INFO              = QUIT_WHEN_FINISHED << 1 // 1024 only write the seed on stdout
  }; // enum BIT
  extern int const ALL;
} // namespace FAST_NS
// All players are AI, there is no pause and no 'full trick'-window
extern int FAST_PLAY;

// the starting seed
extern unsigned const SEED_START;
// the starting player
extern unsigned const STARTPLAYER;

// some utils (RAND(int), MARK(), expand_filename(string), Null_Str
#include "utils.h"


#ifndef WINDOWS
// PlaySound exists only under MS Windows
#undef USE_SOUND_PLAYSOUND
#endif


// whether an assertion shall create a segmentation fault (for a core dump)
// (define it in 'Makefile.local')
//#define ASSERTION_GENERATES_SEGFAULT

#ifdef RELEASE
// if DEBUG_NO_OUTPUT is defined, debugging information are not written
#define DEBUG_NO_OUTPUT
#endif

// whether to use debug informations
#define DEBUG

#ifndef DEBUG
// if DEBUG_NO is defined, the internal checks are skipped
// this speeds up the program up to factor 4
#define DEBUG_NO
#endif

// debugging utils
#include "debug.h"

// creates a segmentation fault
#define SEGFAULT if (true) { cerr << "Created segmentation fault:\n" \
  << __FILE__ << " # " << __LINE__ << endl; \
  abort(); } else (void)0
//(*reinterpret_cast<volatile int*>(NULL) = 0); } else (void)0

#endif // #ifndef HEADER_CONSTANTS
