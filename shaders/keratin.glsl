precision highp float;

uniform sampler2D u_cameraTexture;
uniform sampler2D u_hairAlpha;     // 0..1
uniform sampler2D u_flow;          // tangent/flow map (optional stub)
uniform vec3 u_lightDir;           // normalized
uniform float u_shininess;         // 0..1

varying vec2 v_texCoord;

void main(){
  vec4 base = texture2D(u_cameraTexture, v_texCoord);
  float a = texture2D(u_hairAlpha, v_texCoord).r;

  // Stub tangent (if no flow map, assume vertical hair growth)
  vec3 T = vec3(0.0, 1.0, 0.0);

  float dotTL = dot(T, normalize(u_lightDir));
  float sinTL = sqrt(max(1.0 - dotTL*dotTL, 0.0));
  float spec = pow(sinTL, 64.0 + 192.0*u_shininess) * u_shininess;

  vec3 outc = base.rgb + vec3(1.0)*spec*a;
  gl_FragColor = vec4(outc, base.a);
}
