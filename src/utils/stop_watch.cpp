//////////////////////////////////////////////////////////////////////////////
//
//   Copyright (C) 2006  by Diether Knof
//
//   This program is free software; you can redistribute it and/or 
//   modify it under the terms of the GNU General Public License as 
//   published by the Free Software Foundation; version 2 of 
//   the License.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details. 
//   You can find this license in the file 'gpl.txt'.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 59 Temple Place, Suite 330, Boston, 
//   MA  02111-1307  USA
//
//  Contact:
//    Diether Knof dknof@gmx.de
//
//////////////////////////////////////////////////////////////////////////////

#include "stop_watch.h"

#include <iostream>
using namespace std;

namespace DK {
namespace Utils {

/** constructor
 ** starts the stop watch
 **/
StopWatch::StartStopProxy::StartStopProxy(StopWatch& stop_watch) :
  stop_watch(&stop_watch)
{ this->stop_watch->start(); }

/** destructor
 ** stops the stop watch
 **/
StopWatch::StartStopProxy::~StartStopProxy()
{ this->stop_watch->stop(); }

/**
 ** -> result
 **
 ** @param     start_time   start time
 **
 ** @return    difference from 'start_time' to 'now'
 **            '0' on error
 **
 ** @author    Diether Knof
 **
 ** @version   2006-10-29
 **/
MSecCounter
StopWatch::diff_to_now(clock_t const start_time) {
  clock_t const stop_time = clock();
  if (stop_time == static_cast<clock_t>(-1)) {
    cerr << "StopWatch::diff_to_now()\n"
      << "  could not get time"
      << endl;
    return 0;
  }

  if (start_time > stop_time) {
    cerr << "StopWatch::diff_to_now()\n"
      << "  overflow: "
      << "start time = " << start_time
      << " > " << stop_time << " = stop time"
      << endl;
    return 0;
  }

  // under Microsoft Windows 'CLOCKS_PER_SEC' is 1000,
  // under GNU/Linux (Posix) 1000000,
  // under FreeBSD < 1000
  // So in order to do integer calculation we have to split the code

  // The variable is needed because 'CLOCKS_PER_SEC' need not to be a
  // constant (so no '#if CLOCKS_PER_SEC < 1000' can be used).
  // But with a constant in FreeBSD 'CLOCKS_PER_SEC / 1000' equals 0,
  // this leads to the warning division with zero (g++-3.4.4).
  clock_t clocks_per_sec = CLOCKS_PER_SEC;
  if (clocks_per_sec < 1000) {
    return ((stop_time - start_time) * (1000 / clocks_per_sec));
  } else {
    return ((stop_time - start_time) / (clocks_per_sec / 1000));
  }
} // static MSecCounter StopWatch::diff_to_now(clock_t start_time)


/**
 ** constructor
 **
 ** @param     -
 **
 ** @return    -
 ** 
 ** @author    Diether Knof
 **
 ** @version   2006-10-29
 **/
StopWatch::StopWatch() :
  start_time(static_cast<clock_t>(-1)),
  msec_(0),
  depth(0),
  calls_(0)
{ }

/**
 ** copy constructor
 **
 ** @param     stop_watch
 **
 ** @return    -
 ** 
 ** @author    Diether Knof
 **
 ** @version   2009-01-02
 **/
StopWatch::StopWatch(StopWatch const& stop_watch) :
  start_time(stop_watch.start_time),
  msec_(stop_watch.msec_),
  depth(stop_watch.depth),
  calls_(stop_watch.calls_)
{ }

/**
 ** destructor
 **
 ** @param     -
 **
 ** @return    -
 ** 
 ** @author    Diether Knof
 **
 ** @version   2006-10-29
 **/
StopWatch::~StopWatch()
{
#ifdef DKNOF
  if (this->depth > 0) {
    cerr << "StopWatch::~StopWatch()\n"
      << "  depth is not 0: " << this->depth
      << endl;
#ifndef RELEASE
    abort();
#endif
  }
#endif
} // StopWatch::~StopWatch()

/**
 ** ->result
 **
 ** @param    -
 **
 ** @return   a start stop proxy
 **/
std::unique_ptr<StopWatch::StartStopProxy>
StopWatch::start_stop_proxy()
{ return std::make_unique<StartStopProxy>(*this); }

/**
 ** start the time counter
 **
 ** @param     -
 **
 ** @return    -
 ** 
 ** @author    Diether Knof
 **
 ** @version   2006-10-29
 **/
void
StopWatch::start()
{
  if (this->depth == 0)
    this->start_time = clock();

  if (this->start_time == static_cast<clock_t>(-1)) {
    cerr << "StopWatch::start()\n"
      << "  clock could not be started"
      << endl;
    return ;
  }

  this->depth += 1;
  this->calls_ += 1;
  return ;
} // void StopWatch::start()

/**
 ** stop the time counter
 **
 ** @param     -
 **
 ** @return    -
 ** 
 ** @author    Diether Knof
 **
 ** @version   2006-10-29
 **/
void
StopWatch::stop()
{
  if (   (this->start_time == static_cast<clock_t>(-1))
      || (this->depth == 0) ) {
#ifdef DKNOF
    cerr << "StopWatch::stop()\n"
      << "clock not started"
      << endl;
#ifndef RELEASE
    //  (*reinterpret_cast<int*>(NULL) = 0);
#endif
#endif
    return ;
  }

  this->depth -= 1;

  if (this->depth == 0) {
    this->msec_ += StopWatch::diff_to_now(this->start_time);

    this->start_time = static_cast<clock_t>(-1);
  }

  return ;
} // void StopWatch::stop()

/**
 ** reset the counter
 **
 ** @param     -
 **
 ** @return    -
 ** 
 ** @author    Diether Knof
 **
 ** @version   2006-10-29
 **/
void
StopWatch::reset()
{
  this->start_time = static_cast<clock_t>(-1);
  this->msec_ = 0;
  this->depth = 0;
  return ;
} // void StopWatch::reset()

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    whether the stop watch is running
 ** 
 ** @author    Diether Knof
 **
 ** @version   2006-10-29
 **/
bool
StopWatch::running() const
{
  return (this->start_time != static_cast<clock_t>(-1));
} // bool StopWatch::running() const

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    the stopped time in miliseconds
 ** 
 ** @author    Diether Knof
 **
 ** @version   2006-10-29
 **/
MSecCounter
StopWatch::msec() const
{
  if (this->start_time != static_cast<clock_t>(-1))
    return this->msec_ + StopWatch::diff_to_now(this->start_time);
  else
    return this->msec_;
} // MSecCounter StopWatch::msec() const

/**
 ** -> result
 **
 ** @param     -
 **
 ** @return    how many calls (of start) has been made
 ** 
 ** @author    Diether Knof
 **
 ** @version   2014-03-24
 **/
int
StopWatch::calls() const
{
  return this->calls_;
} // int StopWatch::calls() const

} // namespace Utils
} // namespace DK
