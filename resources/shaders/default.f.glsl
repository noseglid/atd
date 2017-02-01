#version 120

varying vec3 normal, lightDir, halfVector;
varying vec4 diffuse, ambient;
varying vec4 pos;

uniform sampler2D tex;

float fog_factor(float z);

void main()
{
  vec3 n = normal;
  float NdotL = max(dot(n, lightDir), 0.0);
  vec4 texel = texture2D(tex, gl_TexCoord[0].st);

  vec4 color = ambient;

  if (NdotL > 0.0) {
    color += diffuse * NdotL;
    vec3 halfV = normalize(halfVector);
    float NdotHV = max(dot(n, halfV), 0.0);
    color += gl_FrontMaterial.specular *
             gl_LightSource[0].specular *
             pow(NdotHV, gl_FrontMaterial.shininess);
  }

  gl_FragColor = mix(gl_Fog.color, texel * color, fog_factor(pos.z));
}
