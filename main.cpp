#include "Menu.h"
#include "Debug.h"
#include "Exception.h"
#include "Shutdown.h"
#include "engine/Engine.h"
#include "engine/Video.h"
#include "ui/UI.h"
#include "ui/TitleMenu.h"

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_ttf.h>
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

void
init_SDL()
{
  DBG("Initing SDL...");
  if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
    throw Exception("Unable to initiate SDL");
  }

  SDL_EnableUNICODE(SDL_TRUE);
  SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

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

  engine::Video::instance().set_resolution(1024, 768);

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

  engine::resolution res = engine::Video::instance().get_resolution();
  glViewport(0, 0, (GLsizei)res.width, (GLsizei)res.height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45, (double)res.width / (double)res.height, 0.1, 100.0);
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

int
main(int argc, char *argv[])
{
  srand(time(NULL));
  int exitcode = EXIT_SUCCESS;

  try {
    init_SDL();
    init_OpenGL();

    ui::TitleMenu::instance().show(200, ui::Menu::ANIM_LEFT);

    engine::Engine::instance().run();

  } catch (Json::Exception& e) {
    DBGERR("json error: " << e.what());
    exitcode = EXIT_FAILURE;
  } catch (Shutdown& e) {
    DBG("Game shutdown gracefully: " << e.what());
  } catch (Exception& e) {
    DBGERR("Game ended: " << e.what());
    exitcode = EXIT_FAILURE;
  } catch (...) {
    DBGERR("unknown error.");
    exitcode = EXIT_FAILURE;
  }

  SDL_FreeSurface(surface);
  SDL_Quit();

  return exitcode;
}
