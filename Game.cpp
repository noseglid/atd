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
	std::string signal;
	switch (ev.type) {
		case SDL_QUIT:
			stop();
			return;

		case SDL_KEYDOWN:
			signal = "keydown";
			break;

		case SDL_KEYUP:
			signal = "keyup";
			break;

		case SDL_MOUSEBUTTONDOWN:
			signal = "mousedown";
			break;

		case SDL_MOUSEBUTTONUP:
			signal = "mouseup";
			break;

		case SDL_MOUSEMOTION:
			signal = "mousemotion";
			break;
	}

	emit(signal, GameEvent(elapsed, ev));
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
		usleep(300000);
		while (SDL_PollEvent(&ev)) {
			handle_event(ev);
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		camera->orientate();

		GLfloat pos[] = { 0.2, 0.5, 0.5, 0.0 };
		glLightfv(GL_LIGHT0, GL_POSITION, pos);

		gettimeofday(&now, NULL);
		elapsed = now.tv_sec - start_time.tv_sec;
		if (now.tv_usec < start_time.tv_usec) {
			elapsed += (now.tv_usec - start_time.tv_usec) / 1000000.0f;
		} else {
			elapsed -= (start_time.tv_usec - now.tv_usec) / 1000000.0f;
		}

		glEnable(GL_DEPTH_TEST);
		emit("tick", GameEvent(elapsed));

		glDisable(GL_DEPTH_TEST);
		emit("tick_nodepth", GameEvent(elapsed));

		SDL_GL_SwapBuffers();
	}
}

void
Game::stop()
{
	running = false;
}
