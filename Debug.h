#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <iostream>

#define __WHERE                "[" << __FILE__ << ":" << std::dec << __LINE__ << "] "
#define __OUTPUT(stream, args) stream << __WHERE << args << std::endl

#ifdef DEBUGDATA

#define DBGDATA(args)          __OUTPUT(std::cout, "DATA: " << std::endl << args)

#else

#define DBGDATA(args)

#endif

#ifdef DEBUG

#define DBG(args)              __OUTPUT(std::cout, "DEBUG: " << args)
#define DBGWRN(args)           __OUTPUT(std::cout, "WARNING: " << args)
#define DBGERR(args)           __OUTPUT(std::cerr, "ERROR: " << args)

#else

#define DBG(args)
#define DBGWRN(args)
#define DBGQRY(args)
#define DBGERR(arg)

#endif

#endif
