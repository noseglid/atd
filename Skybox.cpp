#include "Skybox.h"
#include "ImageLoader.h"
#include "Camera.h"
#include "gl/glm.h"
#include "Debug.h"

#include <OpenGL/glu.h>

Skybox::Skybox()
{
  back   = IL::GL::texture("skybox/back.png");
  front  = IL::GL::texture("skybox/front.png");
  left   = IL::GL::texture("skybox/left.png");
  right  = IL::GL::texture("skybox/right.png");
  top    = IL::GL::texture("skybox/top.png");
  bottom = IL::GL::texture("skybox/bottom.png");
}

void
Skybox::draw() const
{
  glPushMatrix();

  glLoadIdentity();
  Camera& c = Camera::instance();
  glm::mat4 matrix = glm::lookAt(glm::vec3(0.0f), c.get_direction(), glm::vec3(0.0, 1.0, 0.0));
  glMultMatrixf(&matrix[0][0]);

  glPushAttrib(GL_ENABLE_BIT);
  glEnable(GL_TEXTURE_2D);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_LIGHTING);
  glDisable(GL_BLEND);

  glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

  /* Render the front quad */
  glBindTexture(GL_TEXTURE_2D, front);
  glBegin(GL_QUADS);
    glTexCoord2f(1, 1); glVertex3f(-0.5f, -0.5f, -0.5f);
    glTexCoord2f(0, 1); glVertex3f( 0.5f, -0.5f, -0.5f);
    glTexCoord2f(0, 0); glVertex3f( 0.5f,  0.5f, -0.5f);
    glTexCoord2f(1, 0); glVertex3f(-0.5f,  0.5f, -0.5f);
  glEnd();

  /* Render the left quad */
  glBindTexture(GL_TEXTURE_2D, left);
  glBegin(GL_QUADS);
    glTexCoord2f(1, 1); glVertex3f( 0.5f, -0.5f, -0.5f);
    glTexCoord2f(0, 1); glVertex3f( 0.5f, -0.5f,  0.5f);
    glTexCoord2f(0, 0); glVertex3f( 0.5f,  0.5f,  0.5f);
    glTexCoord2f(1, 0); glVertex3f( 0.5f,  0.5f, -0.5f);
  glEnd();

  /* Render the back quad */
  glBindTexture(GL_TEXTURE_2D, back);
  glBegin(GL_QUADS);
    glTexCoord2f(1, 1); glVertex3f(  0.5f, -0.5f,  0.5f );
    glTexCoord2f(0, 1); glVertex3f( -0.5f, -0.5f,  0.5f );
    glTexCoord2f(0, 0); glVertex3f( -0.5f,  0.5f,  0.5f );
    glTexCoord2f(1, 0); glVertex3f(  0.5f,  0.5f,  0.5f );
  glEnd();

  /* Render the right quad */
  glBindTexture(GL_TEXTURE_2D, right);
  glBegin(GL_QUADS);
    glTexCoord2f(1, 1); glVertex3f( -0.5f, -0.5f,  0.5f );
    glTexCoord2f(0, 1); glVertex3f( -0.5f, -0.5f, -0.5f );
    glTexCoord2f(0, 0); glVertex3f( -0.5f,  0.5f, -0.5f );
    glTexCoord2f(1, 0); glVertex3f( -0.5f,  0.5f,  0.5f );
  glEnd();

  /* Render the top quad */
  glBindTexture(GL_TEXTURE_2D, top);
  glBegin(GL_QUADS);
    glTexCoord2f(1, 0); glVertex3f( -0.5f,  0.5f,  0.5f );
    glTexCoord2f(1, 1); glVertex3f( -0.5f,  0.5f, -0.5f );
    glTexCoord2f(0, 1); glVertex3f(  0.5f,  0.5f, -0.5f );
    glTexCoord2f(0, 0); glVertex3f(  0.5f,  0.5f,  0.5f );
  glEnd();

  /* Render the bottom quad */
  glBindTexture(GL_TEXTURE_2D, bottom);
  glBegin(GL_QUADS);
    glTexCoord2f(1, 0); glVertex3f( -0.5f, -0.5f, -0.5f );
    glTexCoord2f(1, 1); glVertex3f( -0.5f, -0.5f,  0.5f );
    glTexCoord2f(0, 1); glVertex3f(  0.5f, -0.5f,  0.5f );
    glTexCoord2f(0, 0); glVertex3f(  0.5f, -0.5f, -0.5f );
  glEnd();

  glPopAttrib();
  glPopMatrix();

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
  glEnable(GL_BLEND);
}
