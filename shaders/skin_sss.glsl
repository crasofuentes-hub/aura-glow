precision highp float;

uniform sampler2D u_cameraTexture;
uniform sampler2D u_faceAlpha;     // 0..1
uniform vec3 u_makeupColor;        // RGB 0..1
uniform float u_opacity;           // 0..1
uniform float u_sss;               // 0..1

varying vec2 v_texCoord;

float luminance(vec3 c){ return dot(c, vec3(0.299,0.587,0.114)); }

void main(){
  vec4 base = texture2D(u_cameraTexture, v_texCoord);
  float m = texture2D(u_faceAlpha, v_texCoord).r;

  float d = luminance(base.rgb);
  vec3 sss = vec3(1.0, 0.2, 0.1) * (1.0 - d) * u_sss;

  // soft-light-ish blend (cheap)
  vec3 blend = mix(base.rgb, (1.0 - 2.0*u_makeupColor)*base.rgb*base.rgb + 2.0*u_makeupColor*base.rgb, u_opacity);

  vec3 outc = mix(base.rgb, blend + sss, m);
  gl_FragColor = vec4(outc, base.a);
}
