#version 330 core
layout (location = 0) in vec3 a_pos;
// layout (location = 1) in vec3 a_col;
layout (location = 1) in vec2 a_tex;
layout (location = 2) in mat4 a_mvp;

// out vec3 v_col;
out vec2 v_tex;

// uniform mat4 u_vpm;

void main() {
  gl_Position = a_mvp * vec4(a_pos, 1.0);

  // v_col = a_col;
  v_tex = a_tex;
}
