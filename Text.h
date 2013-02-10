#ifndef __TEXT_H__
#define __TEXT_H__

#include <SDL/SDL_ttf.h>
#include <string>

class Text
{
	Text();
	Text& operator=(const Text& rhs);

	static void enable2D();
	static void disable2D();

	static TTF_Font *font;
	static int screen_width, screen_height;

public:

	static void init(const int& screen_width, const int& screen_height);
	static void render(const std::string& text, const int& x, const int&y);

};

#endif
