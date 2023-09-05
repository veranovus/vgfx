#version 330 core
out vec4 frag_color;

in vec3 v_col;

uniform float u_time;

void main() {
  float diff = (sin(u_time) + 1.0) / 2.0;
  frag_color = vec4(v_col.xyz * diff, 1.0);
}
