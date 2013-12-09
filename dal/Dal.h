#ifndef __DAL__DAL_H__
#define __DAL__DAL_H__

#include "dal/DataAccessInterface.h"

namespace dal {

void layer_offline();
DataAccessInterface *get();

}

#endif
