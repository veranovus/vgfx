#version 330 core
out vec4 frag_color;

// in vec3 v_col;
in vec2 v_tex;

uniform float u_time;
uniform sampler2D u_texture;

void main() {
  float diff = (sin(u_time) + 1.0) / 2.0;
  vec4 tex_color = texture(u_texture, v_tex);

  if (tex_color.a == 0) {
    discard;
  }
  frag_color = tex_color * diff;
}
