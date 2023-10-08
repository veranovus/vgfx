#version 330 core
out vec4 frag_color;

in float v_texture;
in vec2 v_tex;
in vec4 v_col;

uniform float u_time;
uniform sampler2D u_texture[16];

void main() {
  float temp = u_time;

  int index = int(v_texture);
    
  vec4 tex_color = texture(u_texture[index], v_tex);

  float aaf = fwidth(tex_color.r);
  float alpha = smoothstep(0.5 - aaf, 0.5 + aaf, tex_color.r);

  if (alpha == 0) {
    discard;
  }

  frag_color = vec4(vec3(1.0f), alpha) * v_col;
}
