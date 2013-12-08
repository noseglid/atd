#version 120

varying vec3 normal, lightDir, halfVector;
varying vec4 diffuse, ambient;
varying vec4 vertex, pos;

void main(void)
{
  lightDir = normalize(vec3(gl_LightSource[0].position));
  halfVector = normalize(vec3(gl_LightSource[0].halfVector.xyz));

  diffuse  = gl_FrontMaterial.diffuse * gl_LightSource[0].diffuse;
  ambient  = gl_FrontMaterial.ambient * gl_LightSource[0].ambient;
  ambient += gl_FrontMaterial.ambient * gl_LightModel.ambient;

  gl_TexCoord[0] = gl_MultiTexCoord0;
  gl_FrontColor = gl_Color;

  gl_Position = ftransform();
  normal = gl_NormalMatrix * gl_Normal;
  pos = gl_Position;
  vertex = gl_Vertex;
}
