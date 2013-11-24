#version 120

varying vec3 normal, lightDir, halfVector;
varying vec4 diffuse, ambient;

uniform sampler2D tex;

void main()
{
  vec3 n = normalize(normal);
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

  if (length(texel.xyz) == 0.0) {
    /* If we dont have a texture, set the texel color to white */
    texel = vec4(1.0);
  }

  gl_FragColor = texel * color;
}
