#ifndef __TEXT__DISPATCHER_H__
#define __TEXT__DISPATCHER_H__

#include "text/Text.h"
#include "gl/glm.h"

#include <OpenGL/gl.h>

B_NS_TEXT

class Dispatcher
{
  struct WorldText
  {
    GLuint texture;
    glm::vec3 pos, delta;
    utils::Color color;
    int width, height;
  };

  std::vector<WorldText> scrollings;

  Dispatcher();
  void operator=(Dispatcher const&);
  Dispatcher(const Dispatcher&);

  void tick();

public:

  static Dispatcher& instance();
  void clear();

  void scrolling(
    const std::string& text,
    const glm::vec3& pos,
    utils::Color clr = utils::colors::white
  );

  static void overlay(
    const std::string& text,
    const int& x,
    const int& y,
    utils::Color clr = utils::colors::white,
    const float& ptsize = 16.0,
    bool offbottom = true,
    bool offleft = true
  );

};

E_NS_TEXT

#endif
