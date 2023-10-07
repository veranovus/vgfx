#version 330 core
out vec4 frag_color;

in float v_shader;
in float v_texture;
in vec2 v_tex;
in vec4 v_col;

uniform float u_time;
uniform sampler2D u_texture[16];

void main() {
  float temp = u_time;

  int shader = int(v_shader);
  int index = int(v_texture);
  
  vec4 tex_color = texture(u_texture[index], v_tex);
    
  if (tex_color.a == 0) {
    discard;
  }
  
  frag_color = tex_color * v_col;
}
