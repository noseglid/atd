#ifndef __IMAGELOADER_H__
#define __IMAGELOADER_H__

#define BEGIN_NAMESPACE_IL namespace IL {
#define END_NAMESPACE_IL }

#include "gl/glm.h"

#include <string>
#include <map>
#include <OpenGL/gl.h>

BEGIN_NAMESPACE_IL

class GL
{
public:

  static GLuint texture(std::string path, glm::ivec2& dimensions, bool set_searchdir = true);
  static GLuint texture(std::string path, bool set_searchdir = true);

private:
  GL();
  GL& operator=(const GL& rhs);
};

END_NAMESPACE_IL

#endif
