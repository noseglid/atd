#include "CreepManager.h"
#include "Game.h"
#include "Text.h"
#include "KeyboardHandler.h"
#include "MetaManager.h"
#include "Map.h"

#include <SDL/SDL.h>
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
CreepManager *creep_manager;
KeyboardHandler *keyboard;

void
init_ATD()
{
	map = new Map();
	map->load("levels/level1.map");

	meta_manager  = new MetaManager();
	creep_manager = new CreepManager(map);
	keyboard      = new KeyboardHandler();

	Game::instance().init();

}

void
init_SDL()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		throw std::exception();
	}

	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	surface = SDL_SetVideoMode(screen_width, screen_height, 16, SDL_OPENGL | SDL_HWSURFACE);
	if (NULL == surface) {
		throw std::exception();
	}

	SDL_WM_SetCaption("ATD", NULL);
}

void
init_OpenGL()
{
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_LINE_SMOOTH);
	glShadeModel(GL_SMOOTH);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glViewport(0, 0, (GLsizei)screen_width, (GLsizei)screen_height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, (double)screen_width / (double)screen_height, 0.1, 100.0);
	glMatrixMode(GL_MODELVIEW);
	glPointSize(3);

	glFogi(GL_FOG_MODE, GL_LINEAR);
	GLfloat fog_color[] = { 0.53, 0.81, 0.92, 1.0 };
	glFogfv(GL_FOG_COLOR, fog_color);
	glFogf(GL_FOG_DENSITY, 0.35f);
	glHint(GL_FOG_HINT, GL_DONT_CARE);
	glFogf(GL_FOG_START, 15.0f);
	glFogf(GL_FOG_END, 25.0f);
	glEnable(GL_FOG);

	glClearColor(fog_color[0], fog_color[1], fog_color[2], fog_color[3]);
	GLfloat ambient[] = { 0.2, 0.2, 0.2, 1.0 };
	GLfloat white_light[] = { 0.7, 0.7, 0.7, 1.0 };

	glEnable(GL_LIGHTING);
	glEnable(GL_NORMALIZE);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);

	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, white_light);
	glLightfv(GL_LIGHT0, GL_AMBIENT, white_light);
	glLightfv(GL_LIGHT0, GL_SPECULAR, white_light);

	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
}

int main(int argc, char *argv[])
{
	srand(time(NULL));

	try {
		init_SDL();
		init_OpenGL();
		Text::init(screen_width, screen_height);

		init_ATD();

		Game::instance().run();

	} catch (Json::Exception& e) {
		std::cout << "json error: " << e.what() << std::endl;
		exit(EXIT_FAILURE);
	} catch (std::exception& e) {
		std::cout << "error: " << e.what() << std::endl;
		exit(EXIT_FAILURE);
	} catch (...) {
		std::cout << "unknown error." << std::endl;
		exit(EXIT_FAILURE);
	}

	SDL_Quit();

	return EXIT_SUCCESS;
}
