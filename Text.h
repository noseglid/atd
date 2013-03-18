#ifndef __TEXT_H__
#define __TEXT_H__

#include "Listener.h"
#include "Math.h"

#include <string>
#include <vector>

#include <OpenGL/gl.h>
#include <SDL/SDL_ttf.h>

struct WorldText
{
	GLuint texture;
	Vector3 pos, delta;
	Vector3 color;
	int width, height;
};

class Text : public Listener
{
	static Text instance;
	static TTF_Font *font;
	static int screen_width, screen_height;

	static std::vector<WorldText> scrollings;

	Text();
	Text& operator=(const Text& rhs);

	static void enable2D();
	static void disable2D();

	static GLuint create_texture(const std::string& text, int& w, int& h);

public:

	void event(const float& elapsed);

	static void init(const int& screen_width, const int& screen_height);

	static void overlay(const std::string& text, const int& x, const int&y, bool offbottom = true);
	static void scrolling(const std::string& text, const Vector3& pos, Vector3 color = Vector3(1.0f, 1.0f, 1.0f));

};

#endif
