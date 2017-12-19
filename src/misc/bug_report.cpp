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

#include "constants.h"

#include "bug_report.h"
#include "../os/bug_report.h"
#include "../os/bug_report_replay.h"
#include "../party/party.h"
#include "../ui/ui.h"

#include "translations.h"


/**
 **
 ** create a bug report and informs the user
 **
 ** @param	message	the message for the bug report
 **
 ** @return	-
 **
 ** @version	0.5.4
 **
 ** @author	Diether Knof
 **
 **/
void
create_assertion_bug_report(string const& message)
{
  // always ouput the error message on stderr
  cerr << message << '\n';
  if (::bug_report_replay != NULL)
    cerr << "--\n"
      << ::translation("replayed bug report: %sbug report%", ::bug_report_replay->filename()) << '\n';

  static bool already_running = false;
  if (already_running) {
    cerr << ::translation("BugReport::automatic report creation failed")
      << '\n';

    cerr << ::translation("BugReport::write seed: %useed%",
                          ::party.seed())
      << '\n';
    cerr << ::translation("BugReport::write startplayer: %ustartplayer%",
                          ::party.startplayer())
      << '\n';

#ifdef ASSERTION_GENERATES_SEGFAULT
    {
      cerr << '\n'
        << ::translation("Creating segmentation fault.") << '\n';
      SEGFAULT;
    }
#endif // #ifdef ASSERTION_GENERATES_SEGFAULT

    exit(EXIT_FAILURE);
  } // if (already_running)
  already_running = true;

  ::ui->error(message);

  already_running = false;

  return ;
} // void create_assertion_bug_report(string const& message)

