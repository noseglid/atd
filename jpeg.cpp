#include "jpeg.h"
#include "Exception.h"

#include <stdio.h>
#include <jpeglib.h>
#include <exception>

#include <iostream>

unsigned char *
read_jpeg(const char *path, unsigned int *width, unsigned int *height, unsigned int *bpp)
{
  struct jpeg_decompress_struct cinfo;
  struct jpeg_error_mgr jerr;
  FILE *fp = fopen(path, "r");
  if (NULL == fp) {
    throw Exception(std::string("Could not open JPEG file: ") + path);
  }

  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_decompress(&cinfo);
  jpeg_stdio_src(&cinfo, fp);
  jpeg_read_header(&cinfo, TRUE);
  jpeg_start_decompress(&cinfo);

  if (width)  *width  = cinfo.output_width;
  if (height) *height = cinfo.output_height;
  if (bpp)    *bpp    = cinfo.data_precision;

  size_t s = (*width) * (*height) * 3;
  unsigned char *data = new unsigned char[s];
  unsigned char *line;
  while (cinfo.output_scanline < *height) {
    line = data + s - ((cinfo.output_scanline + 1) * (*width) * 3);
    jpeg_read_scanlines(&cinfo, &line, 1);
  }

  jpeg_finish_decompress(&cinfo);
  jpeg_destroy_decompress(&cinfo);
  fclose(fp);

  return data;
}

