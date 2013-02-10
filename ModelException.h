#ifndef __MODELEXCEPTION_H__
#define __MODELEXCEPTION_H__

#include "Exception.h"

class ModelException : public Exception
{
public:
	ModelException(const std::string& msg) : Exception(msg) {}
};

#endif
