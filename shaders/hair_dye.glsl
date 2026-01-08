precision highp float;

uniform sampler2D u_cameraTexture;
uniform sampler2D u_hairAlpha;   // 0..1 alpha matte
uniform vec3 u_dyeColor;         // RGB 0..1
uniform float u_opacity;         // 0..1

varying vec2 v_texCoord;

float luminance(vec3 c){ return dot(c, vec3(0.299,0.587,0.114)); }

void main(){
  vec4 base = texture2D(u_cameraTexture, v_texCoord);
  float a = texture2D(u_hairAlpha, v_texCoord).r * u_opacity;

  float lum = luminance(base.rgb);
  vec3 dyed = mix(base.rgb, u_dyeColor * lum, a);

  gl_FragColor = vec4(dyed, base.a);
}
