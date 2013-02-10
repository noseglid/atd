#include "ImageLoader.h"
#include "Debug.h"
#include "ILException.h"
#include "jpeg.h"
#include "tga.h"

#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <iostream>
#include <fstream>
#include <algorithm>

BEGIN_NAMESPACE_IL

GLuint
GL::texture(std::string path, FILETYPE t)
{
	DBG("Loading texture from: " << path);
	std::string trimmed(path.begin(), remove_if(path.begin(), path.end(), [](char c) {
		return isspace(c);
	}));

	path = trimmed;
	if (path.substr(0, 1) == "/") {
		/*
		Some exporters erraneously make '/' the first char even though it's relative
		See if this is the case and make relative if so
		*/
		std::string ptest = path.substr(1);
		std::ifstream ftest(ptest.c_str());
		if (!ftest.good()) {
			throw Exception("Could not find file: " + path);
		}

		path = ptest;
	}

	GLuint texid;
	glGenTextures(2, &texid);
	glBindTexture(GL_TEXTURE_2D, texid);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	ilfn ptr;
	switch (t) {
		case GUESS: ptr = guess_ilfn(path); break;
		case JPEG:  ptr = read_jpeg;        break;
		case TGA:   ptr = read_tga;         break;
	}

	unsigned int width, height, bpp;
	unsigned char *data = ptr(path.c_str(), &width, &height, &bpp);

	GLuint format = (bpp >= 32) ? GL_RGBA : GL_RGB;
	GLuint components = (format == GL_RGB) ? 3 : 4;
	gluBuild2DMipmaps(GL_TEXTURE_2D, components, width, height, format, GL_UNSIGNED_BYTE, data);
	free(data);

	return texid;
}

GL::ilfn
GL::guess_ilfn(const std::string& path)
{
	std::string ext = path.substr(path.find_last_of(".") + 1);
	if ("jpeg" == ext || "jpg" == ext) {
		return read_jpeg;
	} else if ("tga" == ext) {
		return read_tga;
	} else {
		throw Exception("Unknown file extension: " + ext);
	}
}

END_NAMESPACE_IL
