#ifndef __DEBUG_H__
#define __DEBUG_H__

#if DEBUG == 1

#define __WHERE                "[" << __FILE__ << ":" << std::dec << __LINE__ << "] "
#define __OUTPUT(stream, args) stream << __WHERE << args << std::endl

#include <iostream>
#include "gl/glm.h"

#define BLUE    "\033[0;34m"
#define BBLUE   "\033[1;34m"
#define RED     "\033[0;31m"
#define BRED    "\033[1;31m"
#define YELLOW  "\033[0;33m"
#define BYELLOW "\033[1;33m"
#define RESET   "\033[0m"

#define DBG(args)              __OUTPUT(std::cout, BBLUE   "[DEBUG] "   BLUE   << args << RESET)
#define DBGWRN(args)           __OUTPUT(std::cout, BYELLOW "[WARNING] " YELLOW << args << RESET)
#define DBGERR(args)           __OUTPUT(std::cerr, BRED    "[ERROR] "   RED    << args << RESET)

std::ostream& operator<<(std::ostream& out, const glm::vec2& vec);
std::ostream& operator<<(std::ostream& out, const glm::vec3& vec);

#else

#define DBG(args)
#define DBGWRN(args)
#define DBGERR(arg)

#endif

#endif
