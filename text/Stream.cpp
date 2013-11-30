#include "text/Stream.h"
#include "text/Dispatcher.h"
#include "engine/Video.h"
#include "Debug.h"

B_NS_TEXT

Stream::Stream() : inputx(0.0f), inputy(0.0f), input_ptsize(18.0f),
  input_indent(0.0f), clr(1.0f, 1.0f, 1.0f)
{
}

void
Stream::linebreak()
{
  inputx  = 0;
  inputy += text::Text::line_skip(input_ptsize);
}

void
Stream::clear()
{
  content.clear();
  inputx = inputy = 0;
}

void
Stream::draw() const
{
  glm::ivec4 bb = get_bounding_box();
  for (entry e : content) {
    text::Dispatcher::overlay(
      e.text,
      e.x,
      bb.w - e.y,
      e.clr,
      e.ptsize
    );
  }
}


glm::ivec4
Stream::get_bounding_box() const
{
  glm::ivec4 bb;
  for (entry e : content) {
    int w, h;
    text::Text::size(e.text, &w, &h, e.ptsize);
    bb.w = std::max(bb.w, e.y + h);
    bb.z = std::max(bb.z, e.x + w);
  }

  return bb;
}

bool
Stream::empty() const
{
  return content.empty();
}

E_NS_TEXT
