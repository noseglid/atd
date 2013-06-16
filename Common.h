#ifndef __COMMON_H__
#define __COMMON_H__

#define PTRDEL(x) do { if (NULL != x) { delete (x); x = NULL; } } while(0)

#endif
