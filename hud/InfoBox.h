#ifndef __HUD__INFOBOX_H__
#define __HUD__INFOBOX_H__

#include "hud/HUD.h"
#include "Game.h"
#include "Text.h"

#include <string>
#include <list>
#include <de.h>

BEGIN_NS_HUD

#define BOX_PADDING 3.0f
#define BOX_MARGIN 3.0f

class InfoBox
{
public:
  enum SNAP {
    SNAP_BOTLEFT,
    SNAP_TOPLEFT,
    SNAP_TOPRIGHT,
    SNAP_BOTRIGHT
  };

private:
  struct boxcontent {
    int x, y;
    float r, g, b;
    float ptsize;
    std::string text;
  };

  de::Emitter<GameEvent>::id_t mousemotion_emit;

  int left, top, width, height;
  std::vector<boxcontent> content;
  SNAP snap;
  bool followmouse;

  int inputx, inputy;
  float input_ptsize, input_indent;
  float inputr, inputg, inputb;

  void set_bounding_box();

  template <class T>
  void add(T input, int dx = 0)
  {
    boxcontent entry;
    entry.x      = inputx + ((inputx == BOX_PADDING) ? input_indent : 0);
    entry.y      = inputy;
    entry.r      = inputr;
    entry.g      = inputg;
    entry.b      = inputb;
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

  struct color
  {
    float r, g, b;
    color(float r, float g, float b) : r(r), g(g), b(b) {}
  };

  static color red;
  static color green;
  static color blue;
  static color white;
  static color black;
  static color orange;
  static color purple;

  /**
   * Creates a new infobox. Content can be added with operator<<() and it can be shown
   * with draw().
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

  InfoBox& operator<<(color c)
  {
    inputr = c.r;
    inputg = c.g;
    inputb = c.b;
    return *this;
  }

  InfoBox& operator<<(std::string text)
  {
    size_t prepos = 0, pos = 0;
    while (std::string::npos != (pos = text.find('\n', prepos))) {
      std::string line = text.substr(prepos, pos);
      prepos = pos + 1; // Just ahead of the located '\n'

      if (!line.empty()) add(line);
      linebreak();
    }

    if (prepos < text.size()) {
      /* The text did not end with newline.
         Take rest and add it without linebreak. */
      std::string line = text.substr(prepos);
      add(line);
    }

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

END_NS_HUD

#endif
