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

/*
 *    all debug outputs are printed for standard to cerr
 *    to use any other stream just define DEBUG_STREAM
 *
 *    all assert outputs are printed for standard to cerr
 *    to use any other stream just define ASSERT_STREAM
 *
 *    all memcheck outputs are printed for standard to cerr
 *    to use any other stream just define MEMCHECK_STREAM
 * 
 *    all exception outputs are printed for standard to cerr
 *    to use any other stream just define  EXCEPTION_STREAM
 *
 *    for redirection of all three outputs to one stream
 *    just define one stream DEBUG_ALL_STREAM
 *
 *    to suppress output just define DEBUG_NO_OUTPUT
 *
 *    For tracking of function calls:
 *      
 *      at the start of each function use DEBUG_CALLING(predicate, output_text) (predicate toogles output),
 *      DEBUG_CALL( output_text ) or just DEBUG_CAL
 *
 *      use instead of return always DEBUG_RETURNING(value, predicate, text) (predicate toogles output),
 *      DEBUG_RETURN(text , value ) or just DEBUG_RET( value )
 *      and for functions without return value DEBUG_RETURNING_N(predicate, text) (predicate toogles output),
 *      DEBUG_RETURN_N(text) or just DEBUG_RET_N()
 *
 *    For producing more output:
 *      use DEBUG_PRINTING( predicate, output_text )(predicate toogles output), or just
 *      DEBUG_PRINT(output_text)
 * 
 *
 *    to suppress memory checks just define DEBUG_NO_MEMCHECK
 * 
 *    for memory checks use DEBUG_MEMCHECK( pointer ), this 
 *    terminates for null pointers the programm with exit(1)
 *
 *    to suppress asserts just define DEBUG_NO_ASSERTS
 *
 *    for asserts use DEBUG_ASSERTION ( predicate, text) or 
 *    just  DEBUG_ASSERT( predicate ) both terminates the
 *    programm for !predicate with exit(2)
 * 
 *     For Exceptions use DEBUG_TROW( exception_type, exception_constructor ) 
 *     and in the catch block for each DEBUG_THROW a DEBUG_CAUGHT()
 *
 */

#ifndef DEBUG_HEADER_BE
#define DEBUG_HEADER_BE

#include <fstream>
#include <iostream>
using std::cout;
using std::cerr;
using std::endl;
#include <sstream>
using std::ostringstream;
#include <string>
using std::string;

#include <cstdlib>
using std::set_terminate;

#ifndef VOID
#define VOID
#endif

#define critical_check(test, text) DEBUG_ASSERTION(test, text)
#define MEMORY_CHECK(pointer, text) DEBUG_MEMCHECK(pointer)
#define ASSERT(test, text) DEBUG_ASSERTION(test, text)


//#define DEBUG_NO


//#define DEBUG_NO_OUTPUT

//#define DEBUG_NO_MEMCHECK

//#define DEBUG_NO_ASSERTS

#ifdef DEBUG_NO
#undef DEBUG_NO_OUTPUT
#define DEBUG_NO_OUTPUT
#undef DEBUG_NO_MEMCHECK
#define DEBUG_NO_MEMCHECK
#undef DEBUG_NO_ASSERTS
#define DEBUG_NO_ASSERTS
#endif

#ifdef DEBUG_ALL_STREAM
#define DEBUG_STREAM DEBUG_ALL_STREAM
#define MEMCHECK_STREAM DEBUG_ALL_STREAM
#define ASSERT_STREAM DEBUG_ALL_STREAM
#define EXCEPTION_STREAM DEBUG_ALL_STREAM
#endif

#ifndef DEBUG_STREAM
#define DEBUG_STREAM cout
#endif

#ifndef ASSERT_STREAM
#define ASSERT_STREAM cerr
#endif

#ifndef EXCEPTION_STREAM
#define EXCEPTION_STREAM cerr
#endif

#ifndef MEMCHECK_STREAM
#define MEMCHECK_STREAM cerr
#endif

#ifndef DEBUG_NO_OUTPUT

// these macros have to be called with a trailing ';'

#define DEBUG_CALLING(predicate, text)                       \
if (predicate) {                                   \
  CIndent::PrintIndent();                          \
    CIndent::IncIndent();                            \
    DEBUG_STREAM<<"CALL:   "<<text<<endl;              \
} else

#define DEBUG_CALL(text) DEBUG_CALLING(true, text)

#define DEBUG_CAL()  DEBUG_CALL("FUNCTION CALL")

#define DEBUG_RETURNING(value, predicate, text)                \
if (predicate) {                                     \
  CIndent::DecIndent();                              \
    CIndent::PrintIndent();                            \
    DEBUG_STREAM << "RETURN: " << text << endl;        \
    return value;                                      \
} else                                               \
return value

#define DEBUG_RETURN(value, text) DEBUG_RETURNING(value, true, text) 

#define DEBUG_RET( value ) DEBUG_RETURN(value, "LEAVING FUNCTION")    

#define DEBUG_RETURNING_N(predicate, text) DEBUG_RETURNING(VOID, predicate, text)
/*
#define DEBUG_RETURNING_N(predicate, text)                       \
if (predicate) {                                       \
CIndent::DecIndent();                                \
CIndent::PrintIndent();                              \
DEBUG_STREAM<<"RETURN: "<<text<<endl;                \
}                                                      \
return;
*/

#define DEBUG_RETURN_N( text )  DEBUG_RETURNING_N( true, text)

#define DEBUG_RET_N() DEBUG_RETURN_N("LEAVING FUNCTION") 

#define DEBUG_PRINTING( predicate, text)    \
if (predicate) {                  \
  CIndent::PrintIndent();          \
    DEBUG_STREAM<<text<<endl;        \
} else

#define DEBUG_PRINT( text) DEBUG_PRINTING(false, text)

#endif

#ifdef DEBUG_NO_OUTPUT

#define DEBUG_CALLING(predicate, text)

#define DEBUG_CALL( text )

#define DEBUG_CAL( )

#define DEBUG_RETURNING(value, predicate, text) \
return value

#define DEBUG_RETURN(value, text) \
return value

#define DEBUG_RET(value) \
return value

#define DEBUG_RETURNING_N(predicate, text) \
return

#define DEBUG_RETURN_N(text) \
return

#define DEBUG_RET_N() \
return

#define DEBUG_PRINTING(predicate, text )

#define DEBUG_PRINT(text)

#endif

#ifndef DEBUG_NO_MEMCHECK

#define DEBUG_MEMCHECK(pointer)                              \
if ( NULL == (pointer) ) {                            \
  MEMCHECK_STREAM << "ERROR(file " << __FILE__           \
    << ", line " << __LINE__               \
    << "): not enough memory " << endl;    \
    exit(1);                                            \
} else

#endif

#ifdef DEBUG_NO_MEMCHECK

#define DEBUG_MEMCHECK(pointer) ((void) 0)

#endif

#ifndef DEBUG_NO_ASSERTS

#define DEBUG_ASSERTION(predicate, text)	\
  if (!(predicate)) {				\
    ostringstream ostr;				\
    ostr << "ASSERT(file " << __FILE__		\
         << ", line " << __LINE__		\
         << "): " << text << endl;		\
    if (::debug_function != NULL)		\
      (*debug_function)(ostr.str());		\
    else					\
      ASSERT_STREAM << ostr.str();		\
    exit(2);					\
  } else


#define DEBUG_ASSERT( predicate )   DEBUG_ASSERTION(predicate, "failed")

#endif

#ifdef DEBUG_NO_ASSERTS

#define DEBUG_ASSERTION(predicate, text)

#define DEBUG_ASSERT(predicate)

#endif                                             

class CIndent
{
  public:
    static inline void PrintIndent() { for ( int i = 0 ; i < i_indent ; i++ ) DEBUG_STREAM<<"  " ; } ;
    static inline void IncIndent()   { i_indent++; } ;
    static inline void DecIndent()   { i_indent--; } ;

  private:
    static int i_indent;
};

typedef void (*DebugFunction)(string const&);
extern DebugFunction debug_function;

// Ausgabe der History einer ungefangenen Exception auf fs::error
class UncaughtException
{
	public:
	  UncaughtException( const char* file, const int line, const char* what )
	    : file_(file), line_(line), what_(what)
      {}
	  const char* file_;
	  const int line_;
	  const char* what_;
};

namespace Debug {
  static  UncaughtException* uncaughtException_ = NULL;
}

class ExceptionTerminateHandler {
  public:
    static void set( const char* file, const int line, const char* what )
    {
      delete Debug::uncaughtException_;
      Debug::uncaughtException_ = new UncaughtException( file, line, what );
      set_terminate( &terminate );
    } // static void set(const char* file, const int line, const char* what)

    static void unset()
    {
      if( Debug::uncaughtException_ != NULL )
	delete Debug::uncaughtException_;
      Debug::uncaughtException_ = NULL;
    } // static void unset
#ifdef WORKAROUND
    // Wofuer ist das 'protected' da?
#else
  protected:
#endif
    // neue terminate Funktion, die what, CPOS und Bugreport ausgibt
    // fuer eine nicht gefangene Exception
    static void
      terminate()
      {   	
	ostringstream ostr;	
	if( Debug::uncaughtException_ != NULL )
	{
	  ostr << "EXCEPTION( file " << Debug::uncaughtException_->file_
	    << ", line " << Debug::uncaughtException_->line_ 
	    << "): " << Debug::uncaughtException_->what_ << endl;
	} else {
	  ostr << "EXCEPTION( unknown )" << endl;
        }

	if (::debug_function != NULL)		
	  (*debug_function)(ostr.str());		
	else					
	  EXCEPTION_STREAM << ostr.str();		
	abort();
      } // static void terminate()
}; // class ExceptionTerminateHandler

#define DEBUG_THROW( DEBUG_THROW_type, DEBUG_THROW_constructor )     \
do{                                                                  \
  DEBUG_THROW_type DEBUG_THROW_exception = DEBUG_THROW_constructor;  \
    ExceptionTerminateHandler::set( __FILE__, __LINE__,              \
				    DEBUG_THROW_exception.what() );  \
  if (false)                                                          \
    cout << "exception: " __FILE__ << '#' << __LINE__ << '\n'        \
      << "  " << DEBUG_THROW_exception.what() << endl;               \
  throw( DEBUG_THROW_exception );                                    \
} while( false )

#define DEBUG_CAUGHT()           \
  ExceptionTerminateHandler::unset()
  /* moved int to debug.cpp-file

  //initiliaze of CIndent
  int CIndent::i_indent=0;
  */

#endif // #ifndef DEBUG_HEADER_BE
