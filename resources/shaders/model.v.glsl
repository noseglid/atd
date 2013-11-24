#version 120

varying vec3 normal, lightDir, halfVector;
varying vec4 diffuse, ambient;

void main(void)
{
  normal = normalize(gl_NormalMatrix * gl_Normal);
  lightDir = normalize(vec3(gl_LightSource[0].position));
  halfVector = normalize(vec3(gl_LightSource[0].halfVector.xyz));

  diffuse  = gl_FrontMaterial.diffuse * gl_LightSource[0].diffuse;
  ambient  = gl_FrontMaterial.ambient * gl_LightSource[0].ambient;
  ambient += gl_FrontMaterial.ambient * gl_LightModel.ambient;

  gl_TexCoord[0] = gl_MultiTexCoord0;

  gl_Position = ftransform();
}
