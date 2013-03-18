#ifndef __IMAGELOADER_H__
#define __IMAGELOADER_H__

#define BEGIN_NAMESPACE_IL namespace IL {
#define END_NAMESPACE_IL }

#include <string>
#include <map>
#include <OpenGL/gl.h>

BEGIN_NAMESPACE_IL

class GL
{
	typedef unsigned char *(*ilfn)(
		const char *path,
		unsigned int *width,
		unsigned int *height,
		unsigned int *bpp
	);

public:
	enum FILETYPE {
		GUESS,
		JPEG,
		TGA
	};

	static GLuint texture(std::string path, FILETYPE t = GUESS);

private:
	GL();
	GL& operator=(const GL& rhs);

	static ilfn guess_ilfn(const std::string& path);
};

END_NAMESPACE_IL

#endif
