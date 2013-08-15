#ifndef __METAMANAGER_H__
#define __METAMANAGER_H__

#include "engine/Engine.h"

#include <SDL/SDL.h>
#include <vector>

class MetaManager
{
  std::vector<engine::Engine::id_t> events;

  float current_fps;
  float last_measure;
  bool draw_meta;

  void tick(const engine::Event& ev);
  void keydown(const engine::Event& ev);

public:
  MetaManager();
  ~MetaManager();
};

#endif
