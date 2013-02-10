#include "BoxCreep.h"

#include <iostream>
#include <OpenGL/gl.h>

BoxCreep::BoxCreep(Map *map) : Creep(map)
{
}

void
BoxCreep::draw(const float& elapsed) const
{
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);

	glTranslatef(x, 0.25, y);
	glScalef(0.25, 0.25, 0.25);
	glColor3f(0.5, 0.5, 0.5);
  glBegin(GL_TRIANGLE_STRIP);
	{
		glNormal3f(0.0, 0.0, 1.0);
		glVertex3f(-0.5, -0.5, 0.5);
		glVertex3f(-0.5,  0.5, 0.5);
		glVertex3f( 0.5, -0.5, 0.5);
		glVertex3f( 0.5,  0.5, 0.5);
	}
  glBegin(GL_TRIANGLE_STRIP);
	{
		glNormal3f(-1.0, 0.0, 0.0);
		glVertex3f(-0.5, -0.5,  0.5);
		glVertex3f(-0.5,  0.5,  0.5);
		glVertex3f(-0.5, -0.5, -0.5);
		glVertex3f(-0.5,  0.5, -0.5);
	}
	glEnd();
  glBegin(GL_TRIANGLE_STRIP);
	{
		glNormal3f(0.0, 0.0, -1.0);
		glVertex3f(-0.5, -0.5, -0.5);
		glVertex3f(-0.5,  0.5, -0.5);
		glVertex3f( 0.5, -0.5, -0.5);
		glVertex3f( 0.5,  0.5, -0.5);
	}
	glEnd();
  glBegin(GL_TRIANGLE_STRIP);
	{
		glNormal3f(1.0, 0.0, 0.0);
		glVertex3f(0.5, -0.5, -0.5);
		glVertex3f(0.5,  0.5, -0.5);
		glVertex3f(0.5, -0.5,  0.5);
		glVertex3f(0.5,  0.5,  0.5);
	}
  glBegin(GL_TRIANGLE_STRIP);
	{
		glNormal3f(0.0, 1.0, 0.0);
		glVertex3f(-0.5, 0.5, -0.5);
		glVertex3f(-0.5, 0.5,  0.5);
		glVertex3f( 0.5, 0.5, -0.5);
		glVertex3f( 0.5, 0.5,  0.5);
	}
  glBegin(GL_TRIANGLE_STRIP);
	{
		glNormal3f(0.0, -1.0, 0.0);
		glVertex3f(-0.5, -0.5, -0.5);
		glVertex3f(-0.5, -0.5,  0.5);
		glVertex3f( 0.5, -0.5, -0.5);
		glVertex3f( 0.5, -0.5,  0.5);
	}
	glEnd();
}
