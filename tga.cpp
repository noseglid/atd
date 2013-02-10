#include "tga.h"
#include "ILException.h"

#include <tga.h>

/* `TGA_ERROR` is buggy and may use uninitialized values, override it. */
#define TGA_ERROR(tga, code)

#include <iostream>


unsigned char *
read_tga(const char *path, unsigned int *width, unsigned int *height, unsigned int *bpp)
{
	TGA *tga = TGAOpen((char*)path, (char*)"r");
	if (NULL == tga || tga->last != TGA_OK) {
		throw IL::Exception(std::string("Could not open file: ") + path);
	}

	TGAData tdata;
	tdata.flags = TGA_IMAGE_DATA | TGA_IMAGE_ID | TGA_RGB;
	if (TGA_OK != TGAReadImage(tga, &tdata)) {
		throw IL::Exception(std::string("Could not parse TGA file: ") + path);
	}

	if (bpp)    *bpp    = tga->hdr.depth;
	if (width)  *width  = tga->hdr.width;
	if (height) *height = tga->hdr.height;

	TGAClose(tga);

	return tdata.img_data;
}
