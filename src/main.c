#include "vgfx/util/camera_3d.h"
#include "vgfx/vgfx.h"

const usize WINDOW_WIDTH = 800;
const usize WINDOW_HEIGHT = 600;
const char *WINDOW_TITLE = "cgame";

static VGFX_Camera3D *s_camera = NULL;

void key_callback(VGFX_WindowHandle *window, i32 key, i32 scancode, i32 action,
                  i32 mode) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    s_camera->editor_mode = !s_camera->editor_mode;

    if (s_camera->editor_mode) {
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    } else {
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
  }
}

void cursor_pos_callback(VGFX_WindowHandle *window, f64 x, f64 y) {
  vgfx_camera3d_respond_cursor(s_camera, (f32)x, (f32)y);
}

void camera_movement(VGFX_Camera3D *camera, VGFX_Window *window, f64 dt) {
  if (glfwGetKey(window->handle, GLFW_KEY_W)) {
    vgfx_camera3d_respond_key(camera, GLFW_KEY_W, dt);
  }
  if (glfwGetKey(window->handle, GLFW_KEY_S)) {
    vgfx_camera3d_respond_key(camera, GLFW_KEY_S, dt);
  }

  if (glfwGetKey(window->handle, GLFW_KEY_D)) {
    vgfx_camera3d_respond_key(camera, GLFW_KEY_D, dt);
  }
  if (glfwGetKey(window->handle, GLFW_KEY_A)) {
    vgfx_camera3d_respond_key(camera, GLFW_KEY_A, dt);
  }
}

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

  vgfx_glew_initialize();

  // Set mouse movement callback
  glfwSetCursorPosCallback(window->handle, cursor_pos_callback);

  // Set key callback
  glfwSetKeyCallback(window->handle, key_callback);

  // Get maximum vertex attribute count
  i32 max_attribs;
  glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max_attribs);

  printf("DEBUG: Maximum available vertex attribute count: %d\n", max_attribs);

  // Vertex shader
  const char *vertex_shader_path = "res/shader/base.vert";
  String vertex_shader_source = std_fs_read_file(vertex_shader_path);

  VGFX_Shader vertex_shader = vgfx_shader_new(
      GL_VERTEX_SHADER, (const char **)&vertex_shader_source.ptr);

  std_string_free(&vertex_shader_source);

  // Fragment shader
  const char *fragment_shader_path = "res/shader/base.frag";
  String fragment_shader_source = std_fs_read_file(fragment_shader_path);

  VGFX_Shader fragment_shader = vgfx_shader_new(
      GL_FRAGMENT_SHADER, (const char **)&fragment_shader_source.ptr);

  std_string_free(&fragment_shader_source);

  // Shader program
  VGFX_Shader shaders[] = {vertex_shader, fragment_shader};
  VGFX_ShaderProgram program = vgfx_shader_program_new(shaders, 2);

  // Load texture
  const char *texture_path = "res/dummy.png";
  VGFX_Texture2D *texture =
      vgfx_texture_new(texture_path, GL_REPEAT, GL_LINEAR);

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

  // Enable depth test
  glEnable(GL_DEPTH_TEST);

  // Enable blending
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  f64 dt, last_frame;

  while (!vgfx_window_get_window_close(window)) {
    // Time and delta time
    f64 time = glfwGetTime();

    dt = time - last_frame;
    last_frame = time;

    // Camera movement
    camera_movement(s_camera, window, dt);

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

    glfwSwapBuffers(window->handle);
    glfwPollEvents();
  }

  // Delete render pipeline
  glDeleteVertexArrays(1, &vao);
  glDeleteBuffers(1, &vbo);
  glDeleteBuffers(1, &ebo);

  // Free resources
  vgfx_texture_free(texture);
  vgfx_shader_program_free(program);

  // Free the vgfx
  vgfx_camera3d_free(s_camera);
  vgfx_window_free(window);
  vgfx_terminate();

  return 0;
}
