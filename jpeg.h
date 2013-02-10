#ifndef __JPEG_H__
#define __JPEG_H__

#include <iostream>

unsigned char *read_jpeg(const char *path, unsigned int *width = NULL, unsigned int *height = NULL, unsigned int *bpp = NULL);

#endif
