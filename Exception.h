#ifndef __EXCEPTION_H__
#define __EXCEPTION_H__

#include <exception>
#include <string>

class Exception : public std::exception
{
	std::string _what;
public:
	Exception(std::string msg) throw() : _what(msg) {}
	~Exception() throw() {}

	const char *what() const throw() {
		return _what.c_str();
	}
};

#endif
