#ifndef __DAL__DAL_H__
#define __DAL__DAL_H__

#define B_NS_DAL namespace dal {
#define E_NS_DAL }

#include "dal/DataAccessInterface.h"

B_NS_DAL

void layer_offline();
DataAccessInterface *get();

E_NS_DAL

#endif
