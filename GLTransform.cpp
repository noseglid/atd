#include "GLTransform.h"
#include "Math.h"

#include <OpenGL/gl.h>

void
GLTransform::billboard()
{
	Matrix4 m;
	glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat*)&m);

	m.c1r1 = m.c2r2 = m.c3r3 = 1.0f;
	m.c1r2 = m.c1r3 = 0.0f;
	m.c2r1 = m.c2r3 = 0.0f;
	m.c3r1 = m.c3r2 = 0.0f;

	glLoadMatrixf((GLfloat*)&m);
}
