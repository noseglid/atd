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
public:

  static GLuint texture(std::string path);

private:
  GL();
  GL& operator=(const GL& rhs);
};

END_NAMESPACE_IL

#endif
