#include "Hud.h"
#include "Text.h"

Hud::Hud()
{
}

void
Hud::tick()
{
	struct timeval now;
	gettimeofday(&now, NULL);
	float elapsed =
		now.tv_sec - start_time.tv_sec +
		((now.tv_usec - start_time.tv_usec) / 1000000.0f);

	static int frames = 0;
	static float lastelapsed = 0;
	frames++;

	if (frames == 10) {
		frames = 0;
		fps = 10.0f / (elapsed - lastelapsed);
		lastelapsed = elapsed;
	}


	if (display_meta) {
		std::stringstream ss;
		ss.precision(3);
		ss << "FPS: " << fps;
		Text::render(ss.str(), 10, screen_height - 30);
	}
}
