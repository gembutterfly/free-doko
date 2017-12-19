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

#include "os.h"
#include "gameplay.h"
#include "party_points.h"
#include "seed.h"
#include "bug_report.h"
#include "bug_report_replay.h"
#include "auto_bug_report.h"
#ifdef USE_NETWORK
#include "network_log.h"
#endif
#include "chatter.h"

#include "../misc/translations.h"

using namespace OS_TYPE;

// Nearly all functions of OS are virtual (all but OSType OS::type())

/**
 ** Constructor
 ** (to be called from a child class)
 **
 ** @param	type	the OSType (default: UNNAMED)
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **/
OS::OS(OS_TYPE::OSType const type) :
type_(type),
  party_(NULL),
  game_(NULL),
  trick_(NULL),
  bug_report_replay_(NULL),
  status_message_(),
  busy_(false),
  progress_(0),
  progress_max_(0),
  messages_shown_()
{ }

/**
 ** Constructor
 **
 ** @param	os	the OS
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **/
OS::OS(OS const& os) :
type_(os.type_),
  party_(os.party_),
  game_(os.game_),
  trick_(os.trick_),
  bug_report_replay_(os.bug_report_replay_),
  status_message_(os.status_message_),
  busy_(os.busy_),
  progress_(os.progress_),
  progress_max_(os.progress_max_),
  messages_shown_(os.messages_shown_)
{ }

/**
 ** copy operator
 **
 ** @param	os	the OS
 **
 ** @return	the os
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **/
OS&
OS::operator=(OS const& os)
{
  this->type_ = os.type_;
  this->party_ = os.party_;
  this->game_ = os.game_;
  this->trick_ = os.trick_;
  this->bug_report_replay_ = os.bug_report_replay_;
  this->status_message_ = os.status_message_;
  this->busy_ = os.busy_;
  this->progress_ = os.progress_;
  this->progress_max_ = os.progress_max_;
  this->messages_shown_ = os.messages_shown_;

  return *this;
}

/**
 ** Destructor
 **
 ** @param	-
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **/
OS::~OS()
{ }

/**
 ** create a new os
 ** (it should just call the new-operator of the child class)
 **
 ** @param	type	type of the OS to create
 **
 ** @return	pointer of OS
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **/
OS*
new_OS(OSType const type)
{
  switch (type) {
  case UNNAMED:
    return new OS(UNNAMED);
  case GAMEPLAY:
    return new OS_NS::Gameplay;
  case PARTY_POINTS:
    return new OS_NS::PartyPoints;
  case SEED:
    return new OS_NS::Seed;
  case OS_BUG_REPORT:
    return new OS_NS::BugReport;
  case OS_BUG_REPORT_REPLAY:
    DEBUG_ASSERTION(false,
                    "OS* new_OS(type):\n"
                    "  call with 'type' == 'BUG_REPORT_REPLAY'.\n"
                    "  this os needs an argument (the bug report)");
    return NULL;
  case AUTO_BUG_REPORT:
    return new OS_NS::AutoBugReport;
  case SOUND:
    DEBUG_ASSERTION(false,
                    "OS 'SOUND' not yet implemented");
    return NULL;
#ifdef USE_NETWORK
  case SERVER:
    DEBUG_ASSERTION(false,
                    "OS* new_OS(type):\n"
                    "  call with 'type' == 'SERVER'.");
    return NULL;
  case NETWORK_LOG:
    return new OS_NS::NetworkLog;
  case NETWORK_PARSER:
    DEBUG_ASSERTION(false,
                    "OS* new_OS(type):\n"
                    "  call with 'type' == 'NETWORK_PARSER'.");
    return NULL;
#endif
  case CHATTER:
    return new OS_NS::Chatter;
  } // switch(type)

  DEBUG_ASSERTION(false,
                  "OS* new_OS(type):\n"
                  "  call with unknown type '" << type << "'");
  return NULL;
} // OS* new_OS(OSType const type)

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	type of the OS
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **/
OSType
OS::type() const
{ return this->type_; }

/**
 ** the party is opened
 **
 ** @param	party   the party that is opened
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **/
void
OS::party_open(Party const& party)
{
  if (this->party_ == &party)
    return;
  DEBUG_ASSERTION(!this->party_,
                  "OS::party_open(party):\n"
                  "  party already set");
  this->party_ = &party;
  return ;
}

/**
 ** the party will close
 **
 ** @param	-
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **/
void
OS::party_close()
{
  this->party_ = NULL;
  return ;
}


/**
 ** a game is opened
 **
 ** @param	game	the game that is opened
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **/
void
OS::game_open(Game const& game)
{
  DEBUG_ASSERTION(!this->game_,
                  "OS::game_open(game):\n"
                  "  game already set");
  this->game_ = &game;
  return ;
}

/**
 ** the game will close
 **
 ** @param	-
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **/
void
OS::game_close()
{
  this->game_ = NULL;
  return ;
}


/**
 ** a new trick is opened
 **
 ** @param	trick	trick that is opened
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **/
void
OS::trick_open(Trick const& trick)
{
  DEBUG_ASSERTION(!this->trick_,
                  "OS::trick_open(trick):\n"
                  "  trick already set");
  this->trick_ = &trick;
  return ;
}

/**
 ** the trick is closed
 **
 ** @param	-
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **/
void
OS::trick_close()
{
  this->trick_ = NULL;
  return ;
}

/**
 ** a bug report replay is opened
 **
 ** @param	bug_report_replay	bug report replay that is opened
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.8
 **/
void
OS::bug_report_replay_open(OS_NS::BugReportReplay const& bug_report_replay)
{
#ifdef OUTDATED
  // When a bug report is opened via dnd, a new party is started.
  DEBUG_ASSERTION(!this->game_,
                  "OS::bug_report_replay_open(bug_report_replay):\n"
                  "  'bug_report_replay' already set");
#endif
  this->bug_report_replay_ = &bug_report_replay;
  return ;
}

/**
 ** the bug report replay will close
 **
 ** @param	-
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.8
 **/
void
OS::bug_report_replay_close()
{
  this->bug_report_replay_ = NULL;
  return ;
}

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	the latest status message
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **/
Translator::Translation
OS::status_message() const
{
  if (this->status_message_.empty())
    return ::translation("");

  return this->status_message_.back();
} // Translator::Translation OS::status_message() const

/**
 ** adds the status message
 **
 ** @param	status_message	the new status message
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **/
void
OS::add_status_message(Translator::Translation const& status_message)
{
  this->status_message_.push_back(status_message);
  this->status_message_changed(status_message);

  return ;
} // void OS::add_status_message(Translator::Translation const& status_message)

/**
 ** remove the status message
 **
 ** @param	status_message	the status message that is to be removed
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **/
void
OS::remove_status_message(Translator::Translation const& status_message)
{
  vector<Translator::Translation>::iterator pos = std::find(this->status_message_.begin(),
                                                            this->status_message_.end(),
                                                            status_message);

  DEBUG_ASSERTION((pos != this->status_message_.end()),
                  "OS::remove_status_message(status_message):\n"
                  "  message '" << status_message << "' "
                  "not found in the list");

  if (pos != this->status_message_.end())
    pos = this->status_message_.erase(pos);

  if (pos == this->status_message_.end())
    this->status_message_changed(this->status_message());

  return ;
} // void OS::remove_status_message(Translator::Translation const& status_message)

/**
 ** the program is busy
 **
 ** @param	-
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **/
void
OS::set_busy()
{
  this->busy_ = true;
  return ;
}

/**
 ** the program is not busy any more
 **
 ** @param	-
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **/
void
OS::set_not_busy()
{
  this->busy_ = false;
  return ;
}

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	the progress
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **/
double
OS::progress() const
{
  if (this->progress_max_ == 0)
    return 1;

  return (this->progress_ / this->progress_max_);
} // double OS::progress() const

/**
 ** sets the maximal progress
 **
 ** @param	progress_max	the maximal progress
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **/
void
OS::set_max_progress(double const progress_max)
{
  DEBUG_ASSERTION((this->progress_max_ == 0),
                  "OS::set_max_progress(progress_max):\n"
                  "  'this->progress_max_' is already set: "
                  << this->progress_max_ << ".");

  this->progress_max_ = progress_max;
  this->progress_changed(this->progress());

  return ;
} // void OS::set_max_progress(double const progress_max)

/**
 ** adds 'progress' to the progress
 **
 ** @param	progress	the progress
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **/
void
OS::add_progress(double const progress)
{
  this->progress_ += progress;
  DEBUG_ASSERTION((this->progress() <= 1),
                  "OS::add_progress(progress):\n"
                  "  'progress' is greater than 1");

  this->progress_changed(this->progress());

  return ;
} // void OS::add_progress(double const progress)

/**
 ** the progress is finished
 **
 ** @param	-
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.6.7
 **/
void
OS::finish_progress()
{
  DEBUG_ASSERTION((this->progress() != 1),
                  "OS::finish_progress():\n"
                  "  'progress' is not equal to 1");

  this->progress_finished();

  return ;
} // void OS::finish_progress()

/**
 ** adds the message to the shown messages set
 **
 ** @param	message   message to add
 **
 ** @return	whether the message is added to the set (if false: the message was already in the set)
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.8
 **/
bool
OS::add_to_messages_shown(Translator::Translation const& message)
{
  return this->messages_shown_.insert(message).second;
} // bool OS::add_to_messages_shown(Translator::Translation const& message)
