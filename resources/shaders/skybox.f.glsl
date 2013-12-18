#version 120

uniform sampler2D tex;
varying vec4 vertex;

void main(void)
{
  float p = vertex.y + 0.5;
  float cutoff_start = 0.45;
  float cutoff_end   = 0.65;
  float m = (clamp(p, cutoff_start, cutoff_end) - cutoff_start) / (cutoff_end - cutoff_start);

  gl_FragColor = mix(gl_Fog.color, texture2D(tex, gl_TexCoord[0].st), m);
}
