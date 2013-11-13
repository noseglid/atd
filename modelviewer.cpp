#include "Debug.h"
#include "Exception.h"
#include "Model.h"
#include "Camera.h"
#include "GLTransform.h"
#include "engine/Engine.h"
#include "engine/Video.h"
#include "text/Stream.h"

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_ttf.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>

#include <unistd.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <libgen.h>

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

  if (TTF_Init() < 0) {
    throw Exception("Could not initiate SDL TTF library.");
  }

  engine::Video::instance().set_resolution(1024, 768);

  const SDL_version *v;
  v = SDL_Linked_Version();
  DBG("SDL version: " << (int)v->major << "." << (int)v->minor << "." << (int)v->patch);
  v = TTF_Linked_Version();
  DBG("SDL TTF version: " << (int)v->major << "." << (int)v->minor << "." << (int)v->patch);

  SDL_WM_SetCaption("ATD Model viewer", NULL);
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
  gluPerspective(45, (double)res.width / (double)res.height, 0.1, 1000.0);
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

  glLineWidth(3.0f);
  glPointSize(6.0f);
}

int
main(int argc, char *argv[])
{
  srand(time(NULL));
  int exitcode = EXIT_SUCCESS;

  if (2 != argc) {
    DBGERR("Usage: " << argv[0] << " <model-file>");
    exit(EXIT_FAILURE);
  }

  try {
    init_SDL();
    init_OpenGL();

    Model *m = Model::load(argv[1]);
    engine::Engine& e = engine::Engine::instance();
    Camera::instance().set_position();

    text::Stream helptext;
    helptext
      << "Showing model: " << utils::colors::green << basename(argv[1]) << utils::colors::white << "\n"
      << "Number vertices: " << m->get_vertex_count() << "\n"
      << "Zoom with mouse wheel\n\n"
      << utils::colors::yellow << "q" << utils::colors::white << " to quit\n"
      << utils::colors::yellow << "b" << utils::colors::white << " to toggle bones\n"
      << utils::colors::yellow << "r" << utils::colors::white << " to toggle rotation\n"
      << utils::colors::yellow << "a" << utils::colors::white << " to animation\n";

    bool bones = false, rotate = false, animate = false;
    e.on("tick", [m, &bones, &rotate, &animate](engine::Event e) {
      static int rotation = 0;
      if (rotate) rotation = (rotation + 2) % 360;
      glPushMatrix();
      glRotatef(rotation, 0.0f, 1.0f, 0.0f);
      m->draw(animate ? e.elapsed : 0.0f, 1.0f, bones);
      glPopMatrix();
    });

    e.on("tick_nodepth", [helptext](engine::Event e) {
        GLTransform::enable2D();
        glTranslatef(10.0f, 0.0f, 0.0f);
        helptext.draw();
        GLTransform::disable2D();
    });

    e.on("keydown", [&bones, &rotate, &animate](engine::Event e) {
      switch (e.ev.key.keysym.sym) {
        case SDLK_q: engine::Engine::instance().stop(); break;
        case SDLK_b: bones   = !bones;   break;
        case SDLK_r: rotate  = !rotate;  break;
        case SDLK_a: animate = !animate; break;
        default: /* Avoid warning about unhandled keys */ break;
      }
    });

    e.run();
  } catch (Exception& e) {
    DBGERR("Exit: " << e.what());
    exitcode = EXIT_FAILURE;
  } catch (std::exception& e) {
    DBGERR("exception: " << e.what());
  } catch (...) {
    DBGERR("unknown error.");
    exitcode = EXIT_FAILURE;
  }

  SDL_FreeSurface(surface);
  SDL_Quit();

  return exitcode;
}
