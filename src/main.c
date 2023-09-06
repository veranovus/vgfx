#include "vgfx/vgfx.h"

const usize WINDOW_WIDTH = 800;
const usize WINDOW_HEIGHT = 600;
const char *WINDOW_TITLE = "cgame";

int main(i32 argc, char *argv[]) {

  vgfx_initialize();

  String title = std_string_format("%s | %s", WINDOW_TITLE, PKG_VERSION);

  VGFX_Window *window = vgfx_window_new((VGFX_WindowDescriptor){
      .title = title.ptr,
      .width = WINDOW_WIDTH,
      .height = WINDOW_HEIGHT,
      .vsync = false,
      .resizable = true,
  });

  std_string_free(&title);

  vgfx_glew_initialize();

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

  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);

  // Load texture
  const char *texture_path = "res/dummy.png";
  VGFX_Texture2D *texture =
      vgfx_texture_new(texture_path, GL_REPEAT, GL_LINEAR);

  // Setup camera
  VGFX_Camera *camera =
      vgfx_camera_new(glm_rad(90.0f), (f32)WINDOW_WIDTH, (f32)WINDOW_HEIGHT,
                      0.01f, 100.0f, VGFX_CameraModePerspective);

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

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, (i32) stride, (void *)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, (i32) stride,
                        (void *)(sizeof(f32) * 3));
  glEnableVertexAttribArray(1);

  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, (i32) stride,
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

  while (!glfwWindowShouldClose(window)) {
    f64 time = glfwGetTime();

    dt = time - last_frame;
    last_frame = time;

    const f32 camera_speed = 5.0f * (f32) dt;

    if (glfwGetKey(window, GLFW_KEY_W)) {
      vec3 add;
      glm_vec3_scale(camera->front, camera_speed, add);
      glm_vec3_add(camera->position, add, camera->position);
    }
    if (glfwGetKey(window, GLFW_KEY_S)) {
      vec3 add;
      glm_vec3_scale(camera->front, camera_speed, add);
      glm_vec3_sub(camera->position, add, camera->position);
    }

    if (glfwGetKey(window, GLFW_KEY_D)) {
      vec3 add;
      glm_cross(camera->front, camera->up, add);
      glm_normalize(add);
      glm_vec3_scale(add, camera_speed, add);
      glm_vec3_add(camera->position, add, camera->position);
    }
    if (glfwGetKey(window, GLFW_KEY_A)) {
      vec3 add;
      glm_cross(camera->front, camera->up, add);
      glm_normalize(add);
      glm_vec3_scale(add, camera_speed, add);
      glm_vec3_sub(camera->position, add, camera->position);
    }

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    vec3 target;
    glm_vec3_add(camera->position, camera->front, target);
    glm_lookat(camera->position, target, camera->up, camera->view);

    mat4 mvp;
    vgfx_camera_get_matrix(camera, mvp);
    glm_mat4_mul(mvp, model, mvp);

    glUseProgram(program);

    vgfx_shader_program_uniform_f1(program, "u_time", (f32) time);
    vgfx_shader_program_uniform_i1(program, "u_texture", 0);
    vgfx_shader_program_uniform_mat4fv(program, "u_mvp", false, &mvp[0][0]);

    vgfx_texture_bind(texture, 0);

    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    vgfx_texture_unbind(texture);
    glUseProgram(0);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteVertexArrays(1, &vao);
  glDeleteBuffers(1, &vbo);
  glDeleteBuffers(1, &ebo);

  vgfx_texture_free(texture);
  vgfx_shader_program_free(program);

  vgfx_window_free(window);
  vgfx_terminate();

  return 0;
}
