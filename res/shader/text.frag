#version 330 core
out vec4 frag_color;

in vec3 v_col;
in vec2 v_tex;

uniform float u_time;
uniform sampler2D u_texture;

void main() {
  // TODO: You can remove this on Release build
  float temp = u_time;
    
  vec4 tex_color = texture(u_texture, v_tex);

  float aaf = fwidth(tex_color.r);
  float alpha = smoothstep(0.5 - aaf, 0.5 + aaf, tex_color.r);

  frag_color = vec4(v_col.rgb, alpha);
}
