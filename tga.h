#ifndef __TGA_H__
#define __TGA_H__

#include <iostream>

unsigned char *read_tga(const char *path, unsigned int *width = NULL, unsigned int *height = NULL, unsigned int *bpp = NULL);

#endif
