#include "Game.h"
#include "Debug.h"
#include "Listener.h"
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
Game::run_ticks()
{
	for (std::pair<ListenerPriority, ListenerEntry*> entry : listeners) {
		auto event_list = entry.second->events;
		if (event_list.end() == find(event_list.begin(), event_list.end(), LE_TICK)) {
			continue;
		}

		glPushMatrix();
		entry.second->listener->event(elapsed);
		glPopMatrix();
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


	for (std::pair<ListenerPriority, ListenerEntry*> entry : listeners) {
		auto event_list = entry.second->events;
		if (event_list.end() == find(event_list.begin(), event_list.end(), ev.type)) {
			continue;
		}

		Listener *l = entry.second->listener;
		if (ke)  l->event(*ke);
		if (mme) l->event(*mme);
		if (mbe) l->event(*mbe);
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
			GLfloat pos[] = { 0.2, 0.5, 0.5, 0.0 };
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
Game::register_listener(Listener *listener, enum ListenerPriority priority)
{
	ListenerEntry *entry = new ListenerEntry;
	entry->listener = listener;
	entry->priority = priority;
	listeners.insert(std::make_pair(priority, entry));

	DBG("Registered listener '" << typeid(*listener).name() << "'");
	return *entry;
}
