#ifndef __HUD__INFOBOX_H__
#define __HUD__INFOBOX_H__

#include "hud/HUD.h"
#include "utils/Color.h"
#include "engine/Engine.h"
#include "Text.h"

#include <string>
#include <sstream>
#include <list>
#include <de.h>

B_NS_HUD

#define BOX_PADDING 3.0f
#define BOX_MARGIN 3.0f

class InfoBox
{
public:
  enum SNAP {
    SNAP_BOTLEFT,
    SNAP_TOPLEFT,
    SNAP_TOPRIGHT,
    SNAP_BOTRIGHT,
    SNAP_CENTER
  };

private:
  struct boxcontent {
    int x, y;
    utils::Color clr;
    float ptsize;
    std::string text;
  };

  de::Emitter<engine::Event>::id_t mousemotion_emit;

  int left, top, width, height;
  std::vector<boxcontent> content;
  SNAP snap;
  bool followmouse;

  int inputx, inputy;
  float input_ptsize, input_indent;
  utils::Color clr;

  void set_bounding_box();

  template <class T>
  void add(T input, int dx = 0)
  {
    boxcontent entry;
    entry.x      = inputx + ((inputx == BOX_PADDING) ? input_indent : 0);
    entry.y      = inputy;
    entry.clr    = clr;
    entry.ptsize = input_ptsize;

    std::stringstream ss;
    ss << input;
    entry.text  = ss.str();

    int w;
    Text::size(entry.text, &w, NULL, entry.ptsize);
    inputx = entry.x + w;

    content.push_back(entry);
  }

  void linebreak()
  {
    inputx  = BOX_PADDING;
    inputy += Text::overlay_line_skip(input_ptsize);
  }

  InfoBox& operator=(const InfoBox& rhs);

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
    colornumber(T comparable, bool invert = false) : comparable(comparable), invert(invert) {}
  };

  /**
   * Creates a new infobox. Content can be added with operator<<(), it can be shown
   * with draw() and emptied with clear().
   *
   * @param snap          Where the box should be placed 0 - bottom left, 1 - top left,
   *                      2 - top right, 3 bottom right. If followmouse is true, then this
   *                      parameter decides where the mouse should snap to.
   *
   * @param followmouse   If the box should follow the mouse or not.
   */
  InfoBox(SNAP snap = SNAP_BOTLEFT, bool followmouse = false);
  ~InfoBox();

  InfoBox& operator<<(const char* c)
  {
    return *this << std::string(c);
  }

  InfoBox& operator<<(size s)
  {
    input_ptsize = s.ptsize;
    return *this;
  }

  InfoBox& operator<<(indent i)
  {
    input_indent = i.indentsize;
    return *this;
  }

  InfoBox& operator<<(utils::Color clr)
  {
    this->clr = clr;
    return *this;
  }

  template <class T>
  InfoBox& operator<<(colornumber<T> c)
  {
    utils::Color clr = utils::colors::white;
    if (0 < c.comparable) clr = c.invert ? utils::colors::red : utils::colors::green;
    if (0 > c.comparable) clr = c.invert ? utils::colors::green : utils::colors::red;

    return *this << clr << (0 < c.comparable ? "+" : "") << c.comparable;
  }

  InfoBox& operator<<(std::string text)
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

    set_bounding_box();
    return *this;
  }


  template <class T>
  InfoBox& operator<<(T entry)
  {
    add(entry);
    set_bounding_box();
    return *this;
  }

  void clear();

  void draw();
};

E_NS_HUD

#endif
