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

#ifndef DK_UTILS_STOP_WATCH_HEADER
#define DK_UTILS_STOP_WATCH_HEADER

#include <time.h>
#include <memory>

namespace DK {
namespace Utils {

typedef long unsigned MSecCounter ;

/**
 ** a class to check the runtime
 ** uses 'clock()'
 **
 ** @author   Diether Knof
 **
 ** @todo     thread savety (start/stop: either save the thread id or do not use)
 **/
class StopWatch {
  public:
    /** A proxy for simplier measuring
     ** Starts at construct, stop at destruct
     **/
    class StartStopProxy {
      public:
        StartStopProxy() = delete;
        StartStopProxy(StopWatch& stop_watch);
        ~StartStopProxy();
      private:
        StopWatch* const stop_watch;
    }; // class StartStopProxy

  private:
    // returns the time betwenn 'start_time' and 'now'
    static MSecCounter diff_to_now(clock_t const start_time);

  public:
    // constructor (current date)
    StopWatch();
    // copy constructor
    StopWatch(StopWatch const& stop_watch);
    // destructor
    ~StopWatch();

    // create a start stop proxy
    std::unique_ptr<StartStopProxy> start_stop_proxy();

    // start the time counter (the time is added)
    void start();
    // stop the time counter
    void stop();
    // reset the time counter
    void reset();

    // whether the stop watch is running
    bool running() const;

    // elapsed time in mili seconds
    long unsigned msec() const;
    // how many calls have been made
    int calls() const;

  private:
    // the time when 'add' was called
    clock_t start_time;
    // elapsed time in mili seconds
    MSecCounter msec_;
    // how many 'start's still remain
    int depth;

    // how many calls (of start) have been made
    int calls_;

  private: // unused
    StopWatch& operator=(StopWatch const&);
}; // class Version

} // namespace Utils
} // namespace DK

#endif // #ifndef DK_UTILS_STOP_WATCH_HEADER
