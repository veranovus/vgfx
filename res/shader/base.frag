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

  if (index < 0) {
    frag_color = v_col;
  } else {
    frag_color = texture(u_texture[index], v_tex) * v_col;
  }

  if (frag_color.a == 0) {
    discard;
  }
}
