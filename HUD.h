#ifndef __HUD_H__
#define __HUD_H__

#include <OpenGL/gl.h>
#include <list>

class HUD
{
	static int screen_width, screen_height;

	std::list<GLuint> textures;

	void tick() const;
	void draw_stats() const;
	void draw_bottom_banner() const;
	void draw_button(GLuint texture, int i) const;

public:
	HUD();
	static void init(int screen_width, int screen_height);

	static HUD& instance();
};

#endif
