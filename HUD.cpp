#include "HUD.h"
#include "Debug.h"
#include "Game.h"
#include "GLTransform.h"
#include "ImageLoader.h"

#define HUD_BUTTON_WIDTH 80.0f
#define HUD_BUTTON_HEIGHT 64.0f

#define BOTTOM_BAR_OFFSET 3.0f
#define BOTTOM_BAR_PADDING 3.0f
#define BOTTOM_BAR_HEIGHT HUD_BUTTON_HEIGHT + 2 * BOTTOM_BAR_PADDING

int HUD::screen_width  = 0;
int HUD::screen_height = 0;

HUD&
HUD::instance()
{
	static HUD instance;
	return instance;
}

HUD::HUD()
{
	Game::instance().on("tick_nodepth", std::bind(&HUD::tick, this));
	textures.push_back(IL::GL::texture("tower_hero.jpg"));
	textures.push_back(IL::GL::texture("tower_basic1.jpg"));
	textures.push_back(IL::GL::texture("tower_basic2.jpg"));
	textures.push_back(IL::GL::texture("tower_basic3.jpg"));
}

void
HUD::draw_bottom_banner() const
{
	glBegin(GL_TRIANGLE_STRIP);
		glVertex2f(
			BOTTOM_BAR_OFFSET,
			BOTTOM_BAR_OFFSET + BOTTOM_BAR_HEIGHT
		);

		glVertex2f(
			BOTTOM_BAR_OFFSET,
			BOTTOM_BAR_OFFSET
		);

		glVertex2f(
			HUD::screen_width - BOTTOM_BAR_OFFSET,
			BOTTOM_BAR_OFFSET + BOTTOM_BAR_HEIGHT
		);

		glVertex2f(
			HUD::screen_width - BOTTOM_BAR_OFFSET,
			BOTTOM_BAR_OFFSET
		);
	glEnd();
}

void
HUD::draw_button(GLuint texture, int i) const
{
	glBindTexture(GL_TEXTURE_2D, texture);

	glBegin(GL_TRIANGLE_STRIP);
	  /* Top Left */
		glTexCoord2f(0.0f, 1.0f);
		glVertex2f(
			(i + 1) * BOTTOM_BAR_PADDING + i * (HUD_BUTTON_WIDTH + BOTTOM_BAR_PADDING) + BOTTOM_BAR_OFFSET,
			BOTTOM_BAR_OFFSET + HUD_BUTTON_HEIGHT + BOTTOM_BAR_PADDING
		);

		/* Bottom Left */
		glTexCoord2f(0.0f, 0.0f);
		glVertex2f(
			(i + 1) * BOTTOM_BAR_PADDING + i * (HUD_BUTTON_WIDTH + BOTTOM_BAR_PADDING) + BOTTOM_BAR_OFFSET,
			BOTTOM_BAR_OFFSET + BOTTOM_BAR_PADDING
		);

		/* Top Right */
		glTexCoord2f(1.0f, 1.0f);
		glVertex2f(
			(i + 1) * (HUD_BUTTON_WIDTH + 2 * BOTTOM_BAR_PADDING) + BOTTOM_BAR_OFFSET,
			BOTTOM_BAR_OFFSET + HUD_BUTTON_HEIGHT + BOTTOM_BAR_PADDING
		);

		/* Bottom Right */
		glTexCoord2f(1.0f, 0.0f);
		glVertex2f(
			(i + 1) * (HUD_BUTTON_WIDTH + 2 * BOTTOM_BAR_PADDING) + BOTTOM_BAR_OFFSET,
			BOTTOM_BAR_OFFSET + BOTTOM_BAR_PADDING
		);
	glEnd();
}

void
HUD::tick() const
{
	glBindTexture(GL_TEXTURE_2D, 0);
	GLTransform::enable2D();

	glColor4f(0.5f, 0.5f, 0.5f, 0.6f);
	draw_bottom_banner();

	glColor3f(1.0f, 1.0f, 1.0f);
	int i = 0;
	for (GLuint tex : textures) {
		draw_button(tex, i++);
	}


	GLTransform::disable2D();
}

void
HUD::init(int screen_width, int screen_height)
{
	HUD::screen_width  = screen_width;
	HUD::screen_height = screen_height;
	HUD::instance();
}
