#include "vgfx/render.h"
#include "vgfx/util/camera_3d.h"
#include "vgfx/vgfx.h"

const usize WINDOW_WIDTH = 800;
const usize WINDOW_HEIGHT = 600;
const char *WINDOW_TITLE = "cgame";

const char *FRAG_SHADER_PATH = "res/shader/base.frag";
const char *VERT_SHADER_PATH = "res/shader/base.vert";
const char *TEST_FONT_PATH = "res/font/FiraCode-Medium.ttf";
const char *TEST_TEXTURE_PATH = "res/dummy.png";

static VGFX_Camera *s_camera = NULL;
static bool s_editor_mode = false;

int main(i32 argc, char *argv[]) {

  // VGFX setup
  vgfx_initialize();

  VSTD_String title = vstd_string_format("%s | %s", WINDOW_TITLE, PKG_VERSION);

  VGFX_Window *window = vgfx_window_new(&(VGFX_WindowDescriptor){
      .title = title.ptr,
      .size = {(i32)WINDOW_WIDTH, (i32)WINDOW_HEIGHT},
      .vsync = false,
      .resizable = true,
      .decorated = true,
  });

  vstd_string_free(&title);

  // Base Shader program
  VGFX_Shader base_shaders[2];

  {
    // Vertex shader
    VSTD_String vertex_shader_source = vstd_fs_read_file(VERT_SHADER_PATH);

    VGFX_Shader vertex_shader = vgfx_shader_new(
        GL_VERTEX_SHADER, (const char **)&vertex_shader_source.ptr);

    vstd_string_free(&vertex_shader_source);

    base_shaders[0] = vertex_shader;

    // Fragment shader
    VSTD_String fragment_shader_source = vstd_fs_read_file(FRAG_SHADER_PATH);

    VGFX_Shader fragment_shader = vgfx_shader_new(
        GL_FRAGMENT_SHADER, (const char **)&fragment_shader_source.ptr);

    vstd_string_free(&fragment_shader_source);

    base_shaders[1] = fragment_shader;
  }

  VGFX_ShaderProgram base_program = vgfx_shader_program_new(base_shaders, 2);

  // Base Shader program
  VGFX_Shader text_shaders[2];

  {
    // Vertex shader
    VSTD_String vertex_shader_source =
        vstd_fs_read_file("res/shader/text.vert");

    VGFX_Shader vertex_shader = vgfx_shader_new(
        GL_VERTEX_SHADER, (const char **)&vertex_shader_source.ptr);

    vstd_string_free(&vertex_shader_source);

    text_shaders[0] = vertex_shader;

    // Fragment shader
    VSTD_String fragment_shader_source =
        vstd_fs_read_file("res/shader/text.frag");

    VGFX_Shader fragment_shader = vgfx_shader_new(
        GL_FRAGMENT_SHADER, (const char **)&fragment_shader_source.ptr);

    vstd_string_free(&fragment_shader_source);

    text_shaders[1] = fragment_shader;
  }

  VGFX_ShaderProgram text_program = vgfx_shader_program_new(text_shaders, 2);

  // Load font
  VGFX_Font *font = vgfx_font_new(TEST_FONT_PATH, VGFX_FONT_DEFAULT_SIZE);

  // Load texture
  VGFX_Texture2D *texture =
      vgfx_texture_new(TEST_TEXTURE_PATH, GL_REPEAT, GL_LINEAR);

  // Setup camera
  s_camera = vgfx_camera_new(glm_rad(0), WINDOW_WIDTH, WINDOW_HEIGHT, 0.0f,
                             1000.0f, VGFX_CameraModeOrthographic);

  // Setup model matrix
  VGFX_Glyph *glyph = &font->glyphs[63];

  mat4 model;
  glm_mat4_identity(model);
  glm_scale(model, (vec3){glyph->size[0], glyph->size[1], 1.0f});

  f32 x = glyph->offset;
  f32 w = glyph->size[0] / font->size[0];

  // Setup render pipeline
  f32 vertices[] = {
      0.5f, 0.5f, 0.0f,  1.0f,  0.0f, 0.0f,  x + w, 0.0f, 0.5f, -0.5f, 0.0f,
      0.0f, 1.0f, 0.0f,  x + w, 1.0f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,  1.0f,
      x,    1.0f, -0.5f, 0.5f,  0.0f, 1.0f,  1.0f,  1.0f, x,    0.0f,
  };

  u32 indices[] = {0, 1, 3, 1, 2, 3};

  // Changing
  // VPos -> 0
  // VCol -> 2
  // VUv  -> 3

  VGFX_Buffer vbuff = vgfx_buffer_new(&(VGFX_BufferDesc){
      .type = GL_ARRAY_BUFFER,
      .usage = GL_STATIC_DRAW,
      .size = sizeof(vertices),
      .data = vertices,
  });

  VGFX_Buffer ibuff = vgfx_buffer_new(&(VGFX_BufferDesc){
      .type = GL_ELEMENT_ARRAY_BUFFER,
      .usage = GL_STATIC_DRAW,
      .size = sizeof(indices),
      .data = indices,
  });

  VGFX_VertexArray va = vgfx_vertex_array_new();

  vgfx_vertex_array_layout(va, &(VGFX_VertexLayout){
                                   .buffer = vbuff,
                                   .update = 0,
                                   .attrib =
                                       {
                                           [0] = {3, GL_FLOAT, GL_FALSE},
                                           [1] = {3, GL_FLOAT, GL_FALSE},
                                           [2] = {2, GL_FLOAT, GL_FALSE},
                                       },
                               });

  vgfx_vertex_array_index_buffer(va, ibuff);

  printf("Temporary.\n");

  // u32 vao;
  // glGenVertexArrays(1, &vao);

  // u32 vbo;
  // glGenBuffers(1, &vbo);

  // u32 ebo;
  // glGenBuffers(1, &ebo);

  // glBindVertexArray(vao);

  // glBindBuffer(GL_ARRAY_BUFFER, vbo);
  // glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // usize stride = sizeof(f32) * 8;

  // printf("STRIDE :: %lu\n", stride);

  // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, (i32)stride, (void *)0);
  // glEnableVertexAttribArray(0);

  // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, (i32)stride,
  //                       (void *)(sizeof(f32) * 3));
  // glEnableVertexAttribArray(1);

  // glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, (i32)stride,
  //                       (void *)(sizeof(f32) * 6));

  // glEnableVertexAttribArray(2);

  // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
  //              GL_STATIC_DRAW);

  // glBindVertexArray(0);
  // glBindBuffer(GL_ARRAY_BUFFER, 0);
  // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  f64 dt, last_frame;

  while (!vgfx_window_get_window_close(window)) {
    // Time and delta time
    f64 time = glfwGetTime();

    dt = time - last_frame;
    last_frame = time;

    // Update camera view
    vgfx_camera_update_view(s_camera);

    // Calculate mvp matrix
    mat4 mvp;
    vgfx_camera_get_matrix(s_camera, mvp);
    glm_mat4_mul(mvp, model, mvp);

    // Render the scene
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(text_program);

    vgfx_shader_program_uniform_f1(text_program, "u_time", (f32)time);
    vgfx_shader_program_uniform_i1(text_program, "u_texture", 0);
    vgfx_shader_program_uniform_mat4fv(text_program, "u_mvp", false,
                                       &mvp[0][0]);

    // vgfx_texture_bind(texture, 0);
    vgfx_texture_handle_bind(font->handle, 0);

    // glBindVertexArray(vao);
    glBindVertexArray(va);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    // vgfx_texture_unbind(texture);
    vgfx_texture_handle_unbind(0);
    glUseProgram(0);

    vgfx_window_swap_buffers(window);
    vgfx_window_poll_events(window);
  }

  // Delete render pipeline
  // glDeleteVertexArrays(1, &vao);
  // glDeleteBuffers(1, &vbo);
  // glDeleteBuffers(1, &ebo);

  // Free resources
  vgfx_texture_free(texture);
  vgfx_font_free(font);
  vgfx_shader_program_free(base_program);
  vgfx_shader_program_free(text_program);
  vgfx_camera_free(s_camera);

  // Free the vgfx
  vgfx_window_free(window);
  vgfx_terminate();

  return 0;
}
