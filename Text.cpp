#include "Text.h"
#include "Math.h"
#include "Game.h"
#include "Debug.h"
#include "GLTransform.h"

#include <SDL/SDL.h>

TTF_Font *Text::font    = NULL;
int Text::screen_width  = 0;
int Text::screen_height = 0;
Text Text::instance = Text();
std::vector<WorldText> Text::scrollings = std::vector<WorldText>();

Text::Text()
{
	Game::instance().register_listener(this, LP_HIGH) << LE_TICK;
}

void
Text::init(const int& screen_width, const int& screen_height)
{
	if (TTF_Init() < 0) {
		throw std::exception();
	}

	if (!(Text::font = TTF_OpenFont("/Library/Fonts/Arial.ttf", 16))) {
		throw std::exception();
	}

	TTF_SetFontStyle(Text::font, TTF_STYLE_BOLD);
	TTF_SetFontOutline(Text::font, 0);

	Text::screen_width  = screen_width;
	Text::screen_height = screen_height;
}

GLuint
Text::create_texture(const std::string& text, int& w, int& h)
{
	SDL_Color color = { 255, 255, 255 };

	TTF_SizeText(Text::font, text.c_str(), &w, &h);
	SDL_Surface *intermediary = SDL_CreateRGBSurface(0, w, h, 32,
			0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);

	SDL_Surface *text_surface = TTF_RenderText_Solid(font, text.c_str(), color);
	SDL_BlitSurface(text_surface, 0, intermediary, 0);
	SDL_FreeSurface(text_surface);

	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, w, h, 0, GL_BGRA, GL_UNSIGNED_BYTE, intermediary->pixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	SDL_FreeSurface(intermediary);

	return texture;
}

void
Text::event(const float& elapsed)
{
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

	auto it = scrollings.begin();
	while (it != scrollings.end()) {
		WorldText& wt = *it;

		glPushMatrix();
		glBindTexture(GL_TEXTURE_2D, wt.texture);
		glTranslatef(wt.pos.x + wt.delta.x, wt.pos.y + wt.delta.y, wt.pos.z + wt.delta.z);
		GLTransform::billboard();
		glColor3f(wt.color.x, wt.color.y, wt.color.z);
		float height = 0.25f;
		float width = (float)wt.width/(float)wt.height * height;
		glBegin(GL_TRIANGLE_STRIP);
			glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f, 0.1f + height, 0.0f);
			glTexCoord2f(0.0f, 1.0f); glVertex3f(0.0f, 0.1f, 0.0f);
			glTexCoord2f(1.0f, 0.0f); glVertex3f(width, 0.1f + height, 0.0f);
			glTexCoord2f(1.0f, 1.0f); glVertex3f(width, 0.1f, 0.0f);
		glEnd();
		glPopMatrix();
		wt.delta.y += 0.02;

		it = (wt.delta.length() >= 1.5f) ? scrollings.erase(it) : it + 1;
	}
	glEnable(GL_DEPTH_TEST);
}

void
Text::overlay(const std::string& text, const int& x, const int& y, bool offbottom)
{
	enable2D();
	glEnable(GL_TEXTURE_2D);

	int w, h;
	GLuint texture = Text::create_texture(text, w, h);
	glBindTexture(GL_TEXTURE_2D, texture);
	glColor3f(1.0f, 1.0f, 1.0f);

	float offset = (offbottom) ? 2 * y : screen_height;
	glBegin(GL_TRIANGLE_STRIP);
		glTexCoord2f(0.0f, 0.0f); glVertex2f(x,     offset - y + h);
		glTexCoord2f(0.0f, 1.0f); glVertex2f(x,     offset - y);
		glTexCoord2f(1.0f, 0.0f); glVertex2f(x + w, offset - y + h);
		glTexCoord2f(1.0f, 1.0f); glVertex2f(x + w, offset - y);
	glEnd();
	glDeleteTextures(1, &texture);

	disable2D();
}

void
Text::scrolling(const std::string& text, const Vector3& pos, Vector3 color)
{
	WorldText wt;
	wt.texture = Text::create_texture(text, wt.width, wt.height);
	wt.pos     = pos;
	wt.color   = color;

	scrollings.push_back(wt);
}

void
Text::enable2D()
{
	int vport[4];
	glGetIntegerv(GL_VIEWPORT, vport);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	glOrtho(0, vport[2], 0, vport[3], -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glLoadIdentity();
	glDisable(GL_LIGHTING);
}

void
Text::disable2D()
{
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glEnable(GL_LIGHTING);
}
