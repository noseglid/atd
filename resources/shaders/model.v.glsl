#version 120

varying vec3 normal, lightDir, halfVector;
varying vec4 diffuse, ambient;

attribute vec4 boneids;  /* No support for more than 4 bones */
attribute vec4 weights;  /* No support for more than 4 bones */
uniform mat4 bones[100]; /* No support for more than 100 bones */

void main(void)
{
  lightDir = normalize(vec3(gl_LightSource[0].position));
  halfVector = normalize(vec3(gl_LightSource[0].halfVector.xyz));

  diffuse  = gl_FrontMaterial.diffuse * gl_LightSource[0].diffuse;
  ambient  = gl_FrontMaterial.ambient * gl_LightSource[0].ambient;
  ambient += gl_FrontMaterial.ambient * gl_LightModel.ambient;

  gl_TexCoord[0] = gl_MultiTexCoord0;

  mat4 deform  = bones[int(boneids[0])] * weights[0];
       deform += bones[int(boneids[1])] * weights[1];
       deform += bones[int(boneids[2])] * weights[2];
       deform += bones[int(boneids[3])] * weights[3];

  gl_Position = gl_ModelViewProjectionMatrix * deform * gl_Vertex;
  normal = normalize(gl_NormalMatrix * mat3(deform) * gl_Normal);
}
