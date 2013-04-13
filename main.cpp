#include "Game.h"
#include "Text.h"
#include "HUD.h"
#include "Map.h"
#include "Player.h"
#include "MetaManager.h"
#include "KeyboardHandler.h"
#include "CreepManager.h"
#include "TowerManager.h"

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
KeyboardHandler *keyboard;
CreepManager *creep_manager;
TowerManager *tower_manager;

void
init_ATD()
{
	DBG("Initing ATD...");

	Text::init(screen_width, screen_height);
	HUD::init(screen_width, screen_height);

	map = new Map();
	map->load("levels/test.map");

	tower_manager = new TowerManager(map);
	CreepManager::instance().init(map);

	keyboard      = new KeyboardHandler();
	meta_manager  = new MetaManager();

	Game::instance().init(map);
	Player::instance().alter_gold(300);
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
	surface = SDL_SetVideoMode(screen_width, screen_height, 16, SDL_OPENGL | SDL_HWSURFACE);
	if (NULL == surface) {
		throw std::exception();
	}

	SDL_WM_SetCaption("ATD", NULL);
}

void
init_OpenGL()
{
	DBG("Initing OpenGL...");
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

	glEnable(GL_LIGHTING);
	GLfloat ambient[] = { 0.2, 0.2, 0.2, 1.0 };
	GLfloat diffuse[] = { 0.4, 0.4, 0.4, 1.0 };
	GLfloat specular[] = { 0.8, 0.8, 0.8, 1.0 };

	GLfloat lmodel_ambient[] = { 0.1, 0.1, 0.1, 1.0 };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);

	glEnable(GL_LIGHT0);
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
