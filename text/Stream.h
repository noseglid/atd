#ifndef __TEXT__STREAM_H__
#define __TEXT__STREAM_H__

#include "text/Text.h"
#include "gl/glm.h"

#include <sstream>

B_NS_TEXT

class Stream
{
  struct entry {
    int x, y;
    utils::Color clr;
    float ptsize;
    std::string text;
  };

  int inputx, inputy;
  float input_ptsize, input_indent;
  utils::Color clr;

  std::vector<entry> content;

  void linebreak();

  template <class T>
  void add(T input)
  {
    entry e;
    e.x      = inputx + ((inputx == 0) ? input_indent : 0);
    e.y      = inputy;
    e.clr    = clr;
    e.ptsize = input_ptsize;

    std::stringstream ss;
    ss << input;
    e.text  = ss.str();

    int w;
    text::Text::size(e.text, &w, NULL, e.ptsize);
    inputx = e.x + w;

    content.push_back(e);
  }

public:

  struct size
  {
    float ptsize;
    size(float ptsize) : ptsize(ptsize) {}
  };

  struct indent
  {
    float indentsize;
    indent(float indentsize) : indentsize(indentsize) {}
  };

  template <class T>
  struct colornumber
  {
    T comparable;
    bool invert;
    colornumber(T comparable, bool invert = false) :
      comparable(comparable), invert(invert) {}
  };

  Stream();

  void clear();
  void draw() const;

  glm::ivec4 get_bounding_box() const;
  bool empty() const;

  Stream& operator<<(const char* c)
  {
    return *this << std::string(c);
  }

  Stream& operator<<(size s)
  {
    input_ptsize = s.ptsize;
    return *this;
  }

  Stream& operator<<(indent i)
  {
    input_indent = i.indentsize;
    return *this;
  }

  Stream& operator<<(utils::Color clr)
  {
    this->clr = clr;
    return *this;
  }

  template <class T>
  Stream& operator<<(colornumber<T> c)
  {
    utils::Color clr = utils::colors::white;
    if (0 < c.comparable) clr = c.invert ? utils::colors::red : utils::colors::green;
    if (0 > c.comparable) clr = c.invert ? utils::colors::green : utils::colors::red;

    return *this << clr << (0 < c.comparable ? "+" : "") << c.comparable;
  }

  Stream& operator<<(std::string text)
  {
    size_t prepos = 0, pos = 0;
    while (std::string::npos != (pos = text.find('\n', prepos))) {
      std::string line = text.substr(prepos, pos - prepos);
      prepos = pos + 1; // Just ahead of the located '\n'

      if (!line.empty()) add(line);
      linebreak();
    }

    /* Add the end of the string */
    std::string line = text.substr(prepos);
    if (!line.empty()) add(line);

    return *this;
  }

  template <class T>
  Stream& operator<<(T entry)
  {
    add(entry);
    return *this;
  }
};

E_NS_TEXT

#endif
