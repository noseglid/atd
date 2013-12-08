#version 120

float
fog_factor(float z)
{
  return clamp((gl_Fog.start - z) / (gl_Fog.end - gl_Fog.start), 0.0, 1.0);
}
