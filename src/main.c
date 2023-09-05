#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vstd/std.h>

#include "vgfx/shader.h"

const usize WINDOW_WIDTH = 800;
const usize WINDOW_HEIGHT = 600;
const char *WINDOW_TITLE = "cgame";

void framebuffer_size_callback(GLFWwindow *window, i32 w, i32 h) {
  glViewport(0, 0, w, h);
}

int main(i32 argc, char *argv[]) {

  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);

  String title = std_string_format("%s | %s", WINDOW_TITLE, PKG_VERSION);

  GLFWwindow *window =
      glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, title.ptr, NULL, NULL);
  if (!window) {
    fprintf(stderr, "ERROR: Failed to create GLFW Window.");

    glfwTerminate();
    abort();
  }

  glfwMakeContextCurrent(window);

  if (glewInit() != GLEW_OK) {
    fprintf(stderr, "ERROR: Glew failed to initialize.");

    glfwTerminate();
    abort();
  }

  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

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

  f32 vertices[] = {0.5,  0.5,  0.0, 0.5,  -0.5, 0.0,
                    -0.5, -0.5, 0.0, -0.5, 0.5,  0.0};
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

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(f32) * 3, (void *)0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  while (!glfwWindowShouldClose(window)) {
    glClearColor(0.1, 0.1, 0.1, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(program);
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();

  return 0;
}
