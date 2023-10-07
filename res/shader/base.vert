#version 330 core
layout (location = 0) in float a_texture;
layout (location = 1) in vec3 a_pos;
layout (location = 2) in vec2 a_tex;
layout (location = 3) in vec4 a_col;

uniform mat4 u_vpm;

out float v_texture;
out vec2 v_tex;
out vec4 v_col;

void main() {
  gl_Position = u_vpm * vec4(a_pos, 1.0);

  v_texture = a_texture;
  v_tex = a_tex;
  v_col = a_col;
}
