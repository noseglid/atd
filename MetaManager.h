#ifndef __METAMANAGER_H__
#define __METAMANAGER_H__

#include "engine/Event.h"

#include <SDL/SDL.h>

class MetaManager
{
  float current_fps;
  float last_measure;
  bool draw_meta;

public:
  MetaManager();
  void tick(const engine::Event& ev);
  void keydown(const engine::Event& ev);
};

#endif
