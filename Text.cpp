#include "Text.h"

TTF_Font *Text::font    = NULL;
int Text::screen_width  = 0;
int Text::screen_height = 0;

#include <OpenGL/gl.h>
#include <SDL/SDL.h>

void
Text::init(const int& screen_width, const int& screen_height)
{
	if (TTF_Init() < 0) {
		throw std::exception();
	}

	if (!(Text::font = TTF_OpenFont("/Library/Fonts/Arial.ttf", 16))) {
		throw std::exception();
	}

	Text::screen_width  = screen_width;
	Text::screen_height = screen_height;
}

void
Text::render(const std::string& text, const int& x, const int& y)
{
	GLint polygon_mode[2];
	glGetIntegerv(GL_POLYGON_MODE, polygon_mode);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	enable2D();

	int w = screen_width - x;
	int h = TTF_FontHeight(Text::font) + 4;
	SDL_Color color = { 255, 255, 255 };

	SDL_Surface *intermediary = SDL_CreateRGBSurface(0, w, h, 32,
			0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);

	SDL_Surface *text_surface = TTF_RenderText_Solid(font, text.c_str(), color);

	SDL_BlitSurface(text_surface, 0, intermediary, 0);

	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, w, h, 0, GL_BGRA, GL_UNSIGNED_BYTE, intermediary->pixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, texture);
	glColor3f(1.0f, 1.0f, 1.0f);

	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f); glVertex2f(x,     y);
		glTexCoord2f(1.0f, 1.0f); glVertex2f(x + w, y);
		glTexCoord2f(1.0f, 0.0f); glVertex2f(x + w, y + h);
		glTexCoord2f(0.0f, 0.0f); glVertex2f(x,     y + h);
	glEnd();

	glFinish();

	disable2D();

	glPolygonMode(GL_FRONT, polygon_mode[0]);
	glPolygonMode(GL_BACK, polygon_mode[1]);
	SDL_FreeSurface(text_surface);
	SDL_FreeSurface(intermediary);
	glDeleteTextures(1, &texture);
}


void
Text::enable2D()
{
	int vport[4];
	glGetIntegerv(GL_VIEWPORT, vport);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	glOrtho(0, vport[2], 0 , vport[3], -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();


	glLoadIdentity();
}

void
Text::disable2D()
{
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}
