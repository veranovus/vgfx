#version 330 core
out vec4 frag_color;

// in vec3 v_col;
in vec2 v_tex;

uniform float u_time;
uniform sampler2D u_texture;

void main() {
  float diff = (sin(u_time) + 1.0) / 2.0;
  frag_color = vec4(texture(u_texture, v_tex).xyz, diff);
}
