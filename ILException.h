#ifndef __ILEXCEPTION_H__
#define __ILEXCEPTION_H__

#include "ImageLoader.h"

#include <exception>
#include <iostream>

BEGIN_NAMESPACE_IL

class Exception : public std::exception
{
public:
	std::string msg;

	Exception(std::string msg)
	{
		this->msg = msg;
	}
	~Exception() throw() {}

	const char*
	what() const throw()
	{
		return this->msg.c_str();
	}

	friend std::ostream& operator<<(std::ostream out, const Exception& e)
	{
		return out << e.what();
	}
};

END_NAMESPACE_IL

#endif
