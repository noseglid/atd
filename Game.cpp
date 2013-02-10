#include "Game.h"
#include "Debug.h"
#include "Listener.h"
#include "Model.h"

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
Game::run_ticks()
{
	for (ListenerEntry *entry : listeners) {
		auto event_list = entry->events;
		if (event_list.end() != find(event_list.begin(), event_list.end(), LE_TICK)) {
			glPushMatrix();
			entry->listener->event(elapsed);
			glPopMatrix();
		}
	}
}

void
Game::handle_event(const SDL_Event& ev)
{
	const SDL_KeyboardEvent *ke     = NULL;
	const SDL_MouseMotionEvent *mme = NULL;
	const SDL_MouseButtonEvent *mbe = NULL;

	switch (ev.type) {
		case SDL_QUIT:
			stop();
			return;

		case SDL_KEYDOWN:
		case SDL_KEYUP:
			ke = &ev.key;
			break;

		case SDL_MOUSEMOTION:
			mme = &ev.motion;
			break;

		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
			mbe = &ev.button;
			break;
	}


	for (ListenerEntry *entry : listeners) {
		auto event_list = entry->events;
		if (event_list.end() != find(event_list.begin(), event_list.end(), ev.type)) {
			if (ke) (entry->listener)->event(*ke);
			if (mme) (entry->listener)->event(*mme);
			if (mbe) (entry->listener)->event(*mbe);
		}
	}
}

void
Game::init()
{
	camera = new Camera();
}

void
Game::run()
{
	SDL_Event ev;
	struct timeval now;

	gettimeofday(&start_time, NULL);
	while(running) {
		while (SDL_PollEvent(&ev)) {
			handle_event(ev);
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		camera->orientate();

		glPushMatrix();
		{
			glTranslatef(0.0, 1.0, 0.0);
			GLfloat pos[] = { 0.0, 0.8, 1.0, 0.0 };
			glLightfv(GL_LIGHT0, GL_POSITION, pos);
		}
		glPopMatrix();

		gettimeofday(&now, NULL);
		elapsed = now.tv_sec - start_time.tv_sec + ((now.tv_usec - start_time.tv_usec) / 1000000.0f);

		run_ticks();

		SDL_GL_SwapBuffers();
	}
}

void
Game::stop()
{
	running = false;
}

ListenerEntry&
Game::register_listener(Listener *listener)
{
	ListenerEntry *entry = new ListenerEntry;
	entry->listener = listener;
	listeners.push_back(entry);

	DBG("Registered listener '" << typeid(*listener).name() << "'");
	return *entry;
}
