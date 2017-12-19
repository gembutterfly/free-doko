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

#include "ui.h"

#include <unistd.h>

#include "ui.dummy.h"
#ifdef USE_UI_TEXT
#include "text/ui.h"
#endif // #ifdef USE_UI_TEXT
#ifdef USE_UI_GTKMM
#include "gtkmm/ui.h"
#endif // #ifdef USE_UI_GTKMM

#include "../card/hand.h"
#include "../misc/setting.h"
#include "../misc/translations.h"
#include "../misc/bug_report.h"
#include "../os/bug_report_replay.h"

using namespace UI_TYPE;


/**
 **
 ** Constructor
 ** (to be called from a child class)
 **
 ** @param	type	the UIType
 **
 ** @return	-
 **
 ** @version	0.6.7
 **
 ** @author	Diether Knof
 **
 **/
UI::UI(UI_TYPE::UIType const type) :
type_(type),
  party_(NULL),
  game_(NULL),
  trick_(NULL),
  status_message_(),
  busy_(false),
  progress_(0),
  progress_max_(0),
  messages_shown_()
{ return ; }

/**
 **
 ** Destructor
 **
 ** @param	-
 **
 ** @return	-
 **
 ** @version	0.6.7
 **
 ** @author	Diether Knof
 **
 **/
UI::~UI()
{ return ; }

/**
 **
 ** create a new ui
 ** (it should just call the new-operator of the child class)
 **
 ** @param	type	type of the UI to create
 **
 ** @return	pointer of UI
 **
 ** @version	0.6.7
 **
 ** @author	Diether Knof
 **
 **/
UI*
UI::new_(UI_TYPE::UIType const type)
{
  UI* ui;

  switch (type) {
  case DUMMY:
    ui = new UI_Dummy;
    break;
#ifdef USE_UI_TEXT
  case TEXT:
    ui = new UI_Text;
    break;
#endif // #ifdef USE_UI_TEXT
#ifdef USE_UI_GTKMM
  case GTKMM_DOKO:
    ui = new UI_GTKMM_NS::UI_GTKMM;
    break;
#endif // #ifdef USE_UI_GTKMM
  default:
    ui = NULL;
  } // switch(type)

  return ui;
} // static UI* UI::new_(UIType const type)

/**
 ** sleeps for 'sleep_usec' micro seconds
 **
 ** @param	sleep_usec  micro seconds to sleep
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.8
 **/
void
UI::sleep(unsigned const sleep_msec)
{
#ifdef LINUX
  usleep(1000 * sleep_msec);
#endif
  this->update();
} // virtual void sleep(unsigned sleep_msec)

/**
 ** wait a bit (sleep for 20 msec)
 **
 ** @param	-
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.13
 **/
void
UI::wait()
{
  if (!(FAST_PLAY & FAST_NS::PAUSE))
    this->sleep(20);
  else
    this->update();
} // void UI::wait()
/**
 **
 ** -> result
 **
 ** @param	-
 **
 ** @return	the party
 **
 ** @version	0.6.8
 **
 ** @author	Diether Knof
 **
 **/
Party&
UI::party()
{
  return *this->party_;
}

/**
 **
 ** the party is opened
 **
 ** @param	party	party that is opened
 **
 ** @return	-
 **
 ** @version	0.6.7
 **
 ** @author	Diether Knof
 **
 **/
void
UI::party_open(Party& party)
{
  DEBUG_ASSERTION(!this->party_,
                  "UI::party_open(party):\n"
                  "  party already set");
  this->party_ = &party;
  return ;
}

/**
 **
 ** the party will close
 **
 ** @param	-
 **
 ** @return	-
 **
 ** @version	0.6.7
 **
 ** @author	Diether Knof
 **
 **/
void
UI::party_close()
{
  this->party_ = NULL;
  return ;
}

/**
 **
 ** -> result
 **
 ** @param	-
 **
 ** @return	the game
 **
 ** @version	0.6.8
 **
 ** @author	Diether Knof
 **
 **/
Game&
UI::game()
{
  return *this->game_;
}

/**
 **
 ** a game is opened
 **
 ** @param	game	game that is opened
 **
 ** @return	-
 **
 ** @version	0.6.7
 **
 ** @author	Diether Knof
 **
 **/
void
UI::game_open(Game& game)
{
  DEBUG_ASSERTION(!this->game_,
                  "UI::game_open(game):\n"
                  "  game already set");
  this->game_ = &game;
  return ;
}

/**
 **
 ** the game will close
 **
 ** @param	-
 **
 ** @return	-
 **
 ** @version	0.6.7
 **
 ** @author	Diether Knof
 **
 **/
void
UI::game_close()
{
  this->game_ = NULL;
  return ;
}

/**
 **
 ** a new trick is opened
 **
 ** @param	trick	trick that is opened
 **
 ** @return	-
 **
 ** @version	0.6.7
 **
 ** @author	Diether Knof
 **
 **/
void
UI::trick_open(Trick const& trick)
{
  DEBUG_ASSERTION(!this->trick_,
                  "UI::trick_open(trick):\n"
                  "  trick already set");
  this->trick_ = &trick;
  return ;
}

/**
 **
 ** the trick is closed
 **
 ** @param	-
 **
 ** @return	-
 **
 ** @version	0.6.7
 **
 ** @author	Diether Knof
 **
 **/
void
UI::trick_close()
{
  this->trick_ = NULL;
  return ;
}

/**
 **
 ** a bug report replay is opened
 **
 ** @param	bug_report_replay	bug report replay that is opened
 **
 ** @return	-
 **
 ** @version	0.6.8
 **
 ** @author	Diether Knof
 **
 **/
void
UI::bug_report_replay_open(OS_NS::BugReportReplay const& bug_report_replay)
{
#ifdef OUTDATED
  // When a bug report is opened via dnd, a new party is started.
  DEBUG_ASSERTION(!this->game_,
                  "UI::bug_report_replay_open(bug_report_replay):\n"
                  "  'bug_report_replay' already set");
#endif
  this->bug_report_replay_ = &bug_report_replay;
  return ;
}

/**
 **
 ** the bug report replay will close
 **
 ** @param	-
 **
 ** @return	-
 **
 ** @version	0.6.8
 **
 ** @author	Diether Knof
 **
 **/
void
UI::bug_report_replay_close()
{
  this->bug_report_replay_ = NULL;
  return ;
}

/**
 **
 ** -> result
 **
 ** @param	-
 **
 ** @return	the latest status message
 **
 ** @version	0.6.7
 **
 ** @author	Diether Knof
 **
 **/
Translator::Translation const&
UI::status_message() const
{
  if (this->status_message_.empty()) {
    static Translator::Translation const translation_empty = ::translation("");
    return translation_empty;
  }

  return this->status_message_.back();
} // Translator::Translation UI::status_message() const

/**
 **
 ** adds the status message
 **
 ** @param	status_message	the new status message
 **
 ** @return	-
 **
 ** @version	0.6.7
 **
 ** @author	Diether Knof
 **
 **/
void
UI::add_status_message(Translator::Translation const& status_message)
{
  this->status_message_.push_back(status_message);
  this->status_message_changed(status_message);

  return ;
} // void UI::add_status_message(Translator::Translation status_message)

/**
 **
 ** remove the status message
 **
 ** @param	status_message	the status message that is to be removed
 **
 ** @return	-
 **
 ** @version	0.6.7
 **
 ** @author	Diether Knof
 **
 **/
void
UI::remove_status_message(Translator::Translation const& status_message)
{
  vector<Translator::Translation>::iterator pos
    = std::find(this->status_message_.begin(),
                this->status_message_.end(),
                status_message);

  DEBUG_ASSERTION((pos != this->status_message_.end()),
                  "UI::remove_status_message(status_message):\n"
                  "  message '" << status_message << "' "
                  "not found in the list");

  if (pos != this->status_message_.end())
    pos = this->status_message_.erase(pos);

  if (pos == this->status_message_.end())
    this->status_message_changed(this->status_message());

  return ;
} // void UI::remove_status_message(Translator::Translation status_message)

/**
 **
 ** the program is busy
 **
 ** @param	-
 **
 ** @return	-
 **
 ** @version	0.6.7
 **
 ** @author	Diether Knof
 **
 **/
void
UI::set_busy()
{
  this->busy_ = true;
  return ;
}

/**
 **
 ** the program is not busy any more
 **
 ** @param	-
 **
 ** @return	-
 **
 ** @version	0.6.7
 **
 ** @author	Diether Knof
 **
 **/
void
UI::set_not_busy()
{
  this->busy_ = false;
  return ;
}

/**
 **
 ** -> result
 **
 ** @param	-
 **
 ** @return	the progress
 **
 ** @version	0.6.7
 **
 ** @author	Diether Knof
 **
 **/
double
UI::progress() const
{
  if (this != ::ui)
    return ::ui->progress();

  if (this->progress_max_ == 0)
    return 1;

  return (this->progress_ / this->progress_max_);
} // double UI::progress() const

/**
 ** -> result
 **
 ** @param	-
 **
 ** @return	whether the ui is in a progress (t.i. progress_max is set)
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
bool
UI::in_progress() const
{
  if (this != ::ui)
    return ::ui->in_progress();

  return (this->progress_max_ > 0);
} // bool UI::in_progress() const

/**
 **
 ** sets the maximal progress
 **
 ** @param	progress_max	the maximal progress
 **
 ** @return	-
 **
 ** @version	0.6.7
 **
 ** @author	Diether Knof
 **
 **/
void
UI::set_max_progress(double const progress_max)
{
  if (this != ::ui) {
    ::ui->set_max_progress(progress_max);
    return ;
  }

  DEBUG_ASSERTION((this->progress_max_ == 0),
                  "UI::set_max_progress(progress_max):\n"
                  "  'this->progress_max_' is already set: "
                  << this->progress_max_ << ".");

  this->progress_max_ = progress_max;
  this->progress_ = 0;
  this->progress_changed(this->progress());

  return ;
} // void UI::set_max_progress(double const progress_max)

/**
 ** sett 'progress' to the progress
 **
 ** @param	progress	the progress
 **
 ** @return	-
 **/
void
UI::set_progress(double const progress)
{
  if (this != ::ui) {
    ::ui->add_progress(progress);
    return ;
  }

  DEBUG_ASSERTION(progress <= this->progress_max_,
                  "UI::set_progress(" << progress << ")\n"
                  "The progress is greater then the maximum one (" << this->progress_max_ << ")");
  DEBUG_ASSERTION(progress >= this->progress_,
                  "UI::set_progress(" << progress << ")\n"
                  "The progress is less then the current one (" << this->progress_ << ")");
  this->progress_ = progress;

  this->progress_changed(this->progress());
} // void UI::add_progress(double const progress)

/**
 ** adds 'progress' to the progress
 **
 ** @param	progress	the progress
 **
 ** @return	-
 **/
void
UI::add_progress(double const progress)
{
  if (this != ::ui) {
    ::ui->add_progress(progress);
    return ;
  }

  this->progress_ += progress;

#ifdef DKNOF
  DEBUG_ASSERTION((this->progress() <= 1.000001),
                  "UI::add_progress(progress):\n"
                  "  'progress' is greater than 1: "
                  << this->progress());
#else
  if (this->progress() > 1.000001)
    cerr << "UI::add_progres():\n"
      << "  progress is " << this->progress() << ", greater than 1" << endl;
  else
#endif
    if (this->progress() > 1 - 0.000001)
      this->progress_ = this->progress_max_;

  this->progress_changed(this->progress());

  return ;
} // void UI::add_progress(double const progress)

/**
 ** the progress is finished
 **
 ** @param	set_full   whether to set to full before finishing the progress
 **
 ** @return	-
 **
 ** @author	Diether Knof
 **
 ** @version	0.7.6
 **/
void
UI::finish_progress(bool const set_full)
{
  if (this != ::ui) {
    ::ui->finish_progress();
    return ;
  }

#ifndef RELEASE
#ifdef POSTPONED
  DEBUG_ASSERTION((this->progress() == 1),
                  "UI::finish_progress():\n"
                  "  'progress' is not equal to 1: "
                  << this->progress()
                  << ", diff: " << this->progress() - 1);
#else
  if (this->progress() != 1)
    cerr << "UI::finish_progres()\n"
      << "  progress is " << this->progress() << ", not 1" << endl;
#endif
#endif
  if (set_full) {
    this->progress_ = this->progress_max_;
    this->progress_changed(this->progress());
  } // if (set_full)

  this->progress_finished();
  this->progress_max_ = 0;
  this->progress_ = 0;

  return ;
} // void UI::finish_progress(bool set_full = true)

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
UI::add_to_messages_shown(Translator::Translation const& message,
                               INFORMATION::Type const type)
{
  if (std::find(this->messages_shown_.begin(), this->messages_shown_.end(),
                Message(message, type))
      != this->messages_shown_.end())
    return false;

  this->messages_shown_.push_back(Message(message, type));
  return true;
} // bool UI::add_to_messages_shown(Translator::Translation message, INFORMATION::Type type)
