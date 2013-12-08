#version 120

uniform sampler2D tex;
varying vec4 pos;

float fog_factor(float z);

void main(void)
{
  gl_FragColor = mix(gl_Fog.color, texture2D(tex, gl_TexCoord[0].st), fog_factor(pos.z));
}
