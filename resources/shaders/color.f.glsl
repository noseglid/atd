#version 120

varying vec4 pos;

vec4 fog_color();
float fog_factor(float z);

void main(void)
{
  gl_FragColor = mix(gl_Fog.color, gl_Color, fog_factor(pos.z));
}
