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

#ifndef CLASS_EXCEPTION_HEADER
#define CLASS_EXCEPTION_HEADER

#include <string>
#include <exception>

class StdException : public std::exception {
  public:
    StdException() throw() :
      std::exception(),
      message_()
      { }
    StdException(std::string const& message) throw() :
      std::exception(),
      message_(message)
      { }
    StdException(StdException const& std_exception) throw() :
      std::exception(),
      message_(std_exception.message_)
      { }


    virtual ~StdException() throw() { }

    virtual char const* what() const throw()
    { return message_.c_str(); }

    virtual std::string const& message() const throw()
    { return message_; }

  private:
    std::string const message_;
  private: // unused
    StdException& operator=(StdException const&);
}; // class StdException : public std::exception

/**
 ** An exception thrown when a reading (of data from an istream) failed
 **
 ** @author	Diether Knof
 **/
class ReadException : public StdException {
  public:
    ReadException() throw() :
      StdException()
      { }
    ReadException(std::string const& message) throw() :
      StdException(message)
      { }
    ReadException(ReadException const& exception) throw() :
      StdException(exception)
    { }

    ~ReadException() throw() { }

  private: // unused
    ReadException& operator=(ReadException const&);

}; // class ReadException : public StdException

#endif // #ifndef CLASS_EXCEPTION_HEADER
