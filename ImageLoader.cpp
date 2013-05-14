#include "ImageLoader.h"
#include "Debug.h"
#include "Exception.h"
#include "jpeg.h"
#include "tga.h"

#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <iostream>
#include <fstream>
#include <algorithm>

BEGIN_NAMESPACE_IL

static std::map<std::string, GLuint> loaded_textures;

GLuint
GL::texture(std::string path, FILETYPE t)
{
	path = "textures/" + path;
	DBG("Loading texture from: " << path);

	auto it = loaded_textures.find(path);
	if (it != loaded_textures.end()) {
		DBG("Using cached at texid: " << it->second);
		return it->second;
	}


	GLuint texid;
	glGenTextures(1, &texid);

	glBindTexture(GL_TEXTURE_2D, texid);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
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

	loaded_textures.insert(std::make_pair(path, texid));

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
