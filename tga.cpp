#include "tga.h"
#include "Exception.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-writable-strings"
#pragma clang diagnostic ignored "-Wunused-variable"

#include <tga.h>

/* `TGA_ERROR` is buggy and may use uninitialized values, override it. */
#undef TGA_ERROR
#define TGA_ERROR(tga, code)

#pragma clang diagnostic pop

#include <iostream>


unsigned char *
read_tga(const char *path, unsigned int *width, unsigned int *height, unsigned int *bpp)
{
	TGA *tga = TGAOpen((char*)path, (char*)"r");
	if (NULL == tga || tga->last != TGA_OK) {
		throw Exception(std::string("Could not open file: ") + path);
	}

	TGAData tdata;
	tdata.flags = TGA_IMAGE_DATA | TGA_IMAGE_ID | TGA_RGB;
	if (TGA_OK != TGAReadImage(tga, &tdata)) {
		throw Exception(std::string("Could not parse TGA file: ") + path);
	}

	if (bpp)    *bpp    = tga->hdr.depth;
	if (width)  *width  = tga->hdr.width;
	if (height) *height = tga->hdr.height;

	TGAClose(tga);

	return tdata.img_data;
}
