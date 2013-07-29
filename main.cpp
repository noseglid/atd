#include "Game.h"
#include "IO.h"
#include "Text.h"
#include "hud/InfoBar.h"
#include "hud/ButtonBar.h"
#include "Map.h"
#include "Player.h"
#include "MetaManager.h"
#include "KeyboardHandler.h"
#include "CreepManager.h"
#include "TowerManager.h"

#include <SDL/SDL.h>
#include <SDL_image.h>
#include <Rocket/Core.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>

#include <pjson.hpp>
#include <unistd.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

static SDL_Surface *surface;
static int screen_width = 1024, screen_height = 768;

Map *map;
MetaManager *meta_manager;
KeyboardHandler *keyboard;
CreepManager *creep_manager;
TowerManager *tower_manager;

void
init_ATD()
{
  DBG("Initing ATD...");

  Text::init(screen_width, screen_height);
  HUD::HUD::init(screen_width, screen_height);

  Json::Value levelspec = Json::deserialize(IO::file_get_contents("levels/test.map"));
  Map::instance().load(levelspec);

  TowerManager::instance().set_faction(Faction::Faction::SAGES);
  CreepManager::instance().setup(levelspec);

  keyboard      = new KeyboardHandler();
  meta_manager  = new MetaManager();

  Game::instance();
  Player::instance().alter_gold(12000);
  Player::instance().alter_lives(10);
}

void
init_SDL()
{
  DBG("Initing SDL...");
  if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
    throw std::exception();
  }

  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  surface = SDL_SetVideoMode(screen_width, screen_height, 16,
    SDL_OPENGL | SDL_SWSURFACE | SDL_NOFRAME);
  if (NULL == surface) {
    throw Exception("Could not set video modes.");
  }

  if (Mix_Init(MIX_INIT_OGG) < 0) {
    throw Exception("Could not initiate SDL mixer file formats.");
  }
  if (-1 == Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 4096)) {
    throw Exception("Could not initiate SDL Mixer library.");
  }
  if (TTF_Init() < 0) {
    throw Exception("Could not initiate SDL TTF library.");
  }
  if (IMG_Init(IMG_INIT_JPG) < 0) {
    throw Exception("Could not initiate SDL Image library.");
  }

  const SDL_version *v;
  v = SDL_Linked_Version();
  DBG("SDL version: " << (int)v->major << "." << (int)v->minor << "." << (int)v->patch);
  v = Mix_Linked_Version();
  DBG("SDL Mixer version: " << (int)v->major << "." << (int)v->minor << "." << (int)v->patch);
  v = TTF_Linked_Version();
  DBG("SDL TTF version: " << (int)v->major << "." << (int)v->minor << "." << (int)v->patch);
  v = IMG_Linked_Version();
  DBG("SDL Image version: " << (int)v->major << "." << (int)v->minor << "." << (int)v->patch);

  SDL_WM_SetCaption("ATD", NULL);
}

void
init_OpenGL()
{
  DBG("Initing OpenGL...");
  DBG("OpenGL version: "  << glGetString(GL_VERSION));
  DBG("OpenGL vendor: "   << glGetString(GL_VENDOR));
  DBG("OpenGL renderer: " << glGetString(GL_RENDERER));
  DBG("OpenGL shading: "  << glGetString(GL_SHADING_LANGUAGE_VERSION));

  glEnable(GL_CULL_FACE);
  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_POINT_SMOOTH);
  glEnable(GL_NORMALIZE);
  glShadeModel(GL_SMOOTH);
  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glViewport(0, 0, (GLsizei)screen_width, (GLsizei)screen_height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45, (double)screen_width / (double)screen_height, 0.1, 100.0);
  glMatrixMode(GL_MODELVIEW);

  glEnable(GL_LIGHTING);
  GLfloat lmodel_ambient[] = { 0.2, 0.2, 0.2, 1.0 };
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);

  glEnable(GL_LIGHT0);

  GLfloat ambient[] = { 0.2, 0.2, 0.2, 1.0 };
  GLfloat diffuse[] = { 0.8, 0.8, 0.8, 1.0 };
  GLfloat specular[] = { 1.0, 1.0, 1.0, 1.0 };
  glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
}

int main(int argc, char *argv[])
{
  srand(time(NULL));

  try {
    init_SDL();
    init_OpenGL();
    init_ATD();

    Game::instance().run();

  } catch (Json::Exception& e) {
    DBGERR("json error: " << e.what());
    exit(EXIT_FAILURE);
  } catch (Exception& e) {
    DBGERR("Game ended: " << e.what());
    exit(EXIT_FAILURE);
  } catch (...) {
    DBGERR("unknown error.");
    exit(EXIT_FAILURE);
  }

  SDL_Quit();

  return EXIT_SUCCESS;
}
