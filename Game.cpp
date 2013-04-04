#include "Game.h"
#include "Debug.h"
#include "Model.h"
#include "Text.h"

#include <sys/time.h>

Game&
Game::instance()
{
	static Game instance;
	return instance;
}

Game::Game() : running(true)
{
}

void
Game::handle_event(const SDL_Event& ev)
{
	switch (ev.type) {
		case SDL_QUIT:
			stop();
			return;

		case SDL_KEYDOWN:
			keyboard_event = ev.key;
			emit("keydown");
			break;

		case SDL_KEYUP:
			keyboard_event = ev.key;
			emit("keyup");
			break;

		case SDL_MOUSEBUTTONDOWN:
			mouse_button_event = ev.button;
			emit("mousedown");
			break;
		case SDL_MOUSEBUTTONUP:
			mouse_button_event = ev.button;
			emit("mouseup");
			break;

		case SDL_MOUSEMOTION:
			mouse_motion_event = ev.motion;
			emit("mousemotion");
			break;
	}
}

void
Game::init(Map *map)
{
	camera = new Camera();
	camera->set_limits(
		0.0f, (float)map->get_width(),
		0.5f, 8.0f,
		0.0f, (float)map->get_height()
	);
}

void
Game::run()
{
	SDL_Event ev;
	struct timeval now;

	gettimeofday(&start_time, NULL);
	while (running) {
		while (SDL_PollEvent(&ev)) {
			handle_event(ev);
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		camera->orientate();

		glPushMatrix();
		{
			GLfloat pos[] = { 0.2, 0.5, 0.5, 0.0 };
			glLightfv(GL_LIGHT0, GL_POSITION, pos);
		}
		glPopMatrix();

		gettimeofday(&now, NULL);
		elapsed = now.tv_sec - start_time.tv_sec + ((now.tv_usec - start_time.tv_usec) / 1000000.0f);

		glEnable(GL_DEPTH_TEST);
		emit("tick");
		glDisable(GL_DEPTH_TEST);
		emit("tick_nodepth");

		SDL_GL_SwapBuffers();
	}
}

void
Game::stop()
{
	running = false;
}

float
Game::get_elapsed() const
{
	return elapsed;
}

SDL_MouseMotionEvent
Game::last_mouse_motion_event() const
{
	return mouse_motion_event;
}

SDL_MouseButtonEvent
Game::last_mouse_button_event() const
{
	return mouse_button_event;
}

SDL_KeyboardEvent
Game::last_keyboard_event() const
{
	return keyboard_event;
}
