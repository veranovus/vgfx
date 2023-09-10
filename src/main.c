#include "vgfx/util/camera_3d.h"
#include "vgfx/vgfx.h"

/*
 * [X] TODO : Refactor `VGFX_Camera3D` to utilize new input system.
 * [_] TODO : Add more callback functions to `VGFX_Window`.
 * [X] TODO : Improve `VGFX_Input`.
 */

const usize WINDOW_WIDTH = 800;
const usize WINDOW_HEIGHT = 600;
const char *WINDOW_TITLE = "cgame";

const char *FRAG_SHADER_PATH = "res/shader/base.frag";
const char *VERT_SHADER_PATH = "res/shader/base.vert";
const char *TEST_TEXTURE_PATH = "res/dummy.png";

static VGFX_Camera3D *s_camera = NULL;
static bool s_editor_mode = false;

int main(i32 argc, char *argv[]) {

  // VGFX setup
  vgfx_initialize();

  String title = std_string_format("%s | %s", WINDOW_TITLE, PKG_VERSION);

  VGFX_Window *window = vgfx_window_new((VGFX_WindowDescriptor){
      .title = title.ptr,
      .width = WINDOW_WIDTH,
      .height = WINDOW_HEIGHT,
      .vsync = false,
      .resizable = true,
      .decorated = true,
  });

  std_string_free(&title);

  // Get maximum vertex attribute count
  i32 max_attribs;
  glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max_attribs);

  printf("DEBUG: Maximum available vertex attribute count: %d\n", max_attribs);

  // Vertex shader
  String vertex_shader_source = std_fs_read_file(VERT_SHADER_PATH);

  VGFX_Shader vertex_shader = vgfx_shader_new(
      GL_VERTEX_SHADER, (const char **)&vertex_shader_source.ptr);

  std_string_free(&vertex_shader_source);

  // Fragment shader
  String fragment_shader_source = std_fs_read_file(FRAG_SHADER_PATH);

  VGFX_Shader fragment_shader = vgfx_shader_new(
      GL_FRAGMENT_SHADER, (const char **)&fragment_shader_source.ptr);

  std_string_free(&fragment_shader_source);

  // Shader program
  VGFX_Shader shaders[] = {vertex_shader, fragment_shader};
  VGFX_ShaderProgram program = vgfx_shader_program_new(shaders, 2);

  // Load texture
  VGFX_Texture2D *texture =
      vgfx_texture_new(TEST_TEXTURE_PATH, GL_REPEAT, GL_LINEAR);

  // Setup camera
  s_camera = vgfx_camera3d_new(glm_rad(45.0f), (f32)WINDOW_WIDTH,
                               (f32)WINDOW_HEIGHT, 0.01f, 100.0f);

  // Setup model matrix
  mat4 model;
  glm_mat4_identity(model);
  glm_rotate(model, glm_rad(-30.0f), (vec3){1.0f, 0.0f, 0.0f});

  // Setup render pipeline
  f32 vertices[] = {
      0.5f, 0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  1.0f,  1.0f, 0.5f, -0.5f, 0.0f,
      0.0f, 1.0f, 0.0f,  1.0f, 0.0f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,  1.0f,
      0.0f, 0.0f, -0.5f, 0.5f, 0.0f, 1.0f,  1.0f,  1.0f, 0.0f, 1.0f,
  };
  u32 indices[] = {0, 1, 3, 1, 2, 3};

  u32 vao;
  glGenVertexArrays(1, &vao);

  u32 vbo;
  glGenBuffers(1, &vbo);

  u32 ebo;
  glGenBuffers(1, &ebo);

  glBindVertexArray(vao);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  usize stride = sizeof(f32) * 8;

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, (i32)stride, (void *)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, (i32)stride,
                        (void *)(sizeof(f32) * 3));
  glEnableVertexAttribArray(1);

  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, (i32)stride,
                        (void *)(sizeof(f32) * 6));
  glEnableVertexAttribArray(2);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  f64 dt, last_frame;

  while (!vgfx_window_get_window_close(window)) {
    // Time and delta time
    f64 time = glfwGetTime();

    dt = time - last_frame;
    last_frame = time;

    // Set s_editor_mode
    if (vgfx_input_is_key_pressed(window->input, VGFX_Key_ESCAPE)) {
      s_editor_mode = !s_editor_mode;

      if (!s_editor_mode) {
        glfwSetInputMode(window->handle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
      } else {
        glfwSetInputMode(window->handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
      }
    }

    // Camera movement
    vgfx_camera3d_handle_input(s_camera, window->input, dt, s_editor_mode);

    // Update camera view
    vgfx_camera_update_view(s_camera->camera);

    // Calculate mvp matrix
    mat4 mvp;
    vgfx_camera_get_matrix(s_camera->camera, mvp);
    glm_mat4_mul(mvp, model, mvp);

    // Render the scene
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(program);

    vgfx_shader_program_uniform_f1(program, "u_time", (f32)time);
    vgfx_shader_program_uniform_i1(program, "u_texture", 0);
    vgfx_shader_program_uniform_mat4fv(program, "u_mvp", false, &mvp[0][0]);

    vgfx_texture_bind(texture, 0);

    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    vgfx_texture_unbind(texture);
    glUseProgram(0);

    vgfx_window_swap_buffers(window);
    vgfx_window_poll_events(window);
  }

  // Delete render pipeline
  glDeleteVertexArrays(1, &vao);
  glDeleteBuffers(1, &vbo);
  glDeleteBuffers(1, &ebo);

  // Free resources
  vgfx_texture_free(texture);
  vgfx_shader_program_free(program);
  vgfx_camera3d_free(s_camera);

  // Free the vgfx
  vgfx_window_free(window);
  vgfx_terminate();

  return 0;
}
