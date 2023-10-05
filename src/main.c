#include "vgfx/asset.h"
#include "vgfx/camera.h"
#include "vgfx/core.h"
#include "vgfx/gl.h"
#include "vgfx/os.h"
#include "vgfx/shader.h"
#include "vgfx/texture.h"
#include <time.h>

const usize WINDOW_WIDTH = 800;
const usize WINDOW_HEIGHT = 600;
const char *WINDOW_TITLE = "cgame";

const char *FRAG_SHADER_PATH = "res/shader/base.frag";
const char *VERT_SHADER_PATH = "res/shader/base.vert";
const char *TEST_FONT_PATH = "res/font/FiraCode-Medium.ttf";
const char *TEST_TEXTURE_PATH = "res/bunny.png";

static VGFX_Camera *s_camera = NULL;

int main(i32 argc, char *argv[]) {

  VGFX_UNUSED(argc);
  VGFX_UNUSED(argv);

  // VGFX setup
  VSTD_String title = vstd_string_format("%s | %s", WINDOW_TITLE, PKG_VERSION);

  VGFX_OS_WindowHandle win = vgfx_os_window_open(&(VGFX_OS_WindowDesc){
      .title = title.ptr,
      .width = WINDOW_WIDTH,
      .height = WINDOW_HEIGHT,
      .vsync = false,
      .resizable = true,
      .decorated = true,
      .visible = true,
  });

  vstd_string_free(&title);

  // Asset Server
  VGFX_AS_AssetServer *asset_server = vgfx_as_asset_server_new();

  // Load texture
  VGFX_AS_Asset *texture = vgfx_as_asset_server_load(
      asset_server, &(VGFX_AS_AssetDesc){
                        .type = VGFX_ASSET_TYPE_TEXTURE,
                        .texture_path = TEST_TEXTURE_PATH,
                        .texture_filter = GL_NEAREST,
                        .texture_wrap = GL_REPEAT,
                    });

  // Base Shader program
  VGFX_AS_Asset *program = vgfx_as_asset_server_load(
      asset_server, &(VGFX_AS_AssetDesc){
                        .type = VGFX_ASSET_TYPE_SHADER,
                        .shader_vert_path = VERT_SHADER_PATH,
                        .shader_frag_path = FRAG_SHADER_PATH,
                    });

  // Setup camera
  s_camera = vgfx_camera_new(glm_rad(45.0f), WINDOW_WIDTH, WINDOW_HEIGHT, 0.0f,
                             1000.0f, VGFX_CameraModeOrthographic);

  // Setup render pipeline
  const usize MAX_INSTANCE = 100000;

  f32 vertices[] = {
      0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.5f,  -0.5f, 0.0f, 1.0f, 1.0f,
      -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, -0.5f, 0.5f,  0.0f, 0.0f, 0.0f,
  };

  u32 indices[] = {0, 1, 3, 1, 2, 3};

  VGFX_GL_Buffer vbuff = vgfx_gl_buffer_create(GL_ARRAY_BUFFER);
  vgfx_gl_buffer_data(&vbuff, GL_STATIC_DRAW, sizeof(vertices), vertices);

  VGFX_GL_Buffer ibuff = vgfx_gl_buffer_create(GL_ELEMENT_ARRAY_BUFFER);
  vgfx_gl_buffer_data(&ibuff, GL_STATIC_DRAW, sizeof(indices), indices);

  VGFX_GL_Buffer dbuff = vgfx_gl_buffer_create(GL_ARRAY_BUFFER);
  vgfx_gl_buffer_data(&dbuff, GL_DYNAMIC_DRAW, sizeof(mat4) * MAX_INSTANCE,
                      NULL);

  VGFX_GL_VertexArray va = vgfx_gl_vertex_array_create();

  VGFX_GL_VertexAttribLayout layout0 = {
      .buffer = vbuff,
      .update_freq = 0,
      .attribs =
          {
              {3, GL_FLOAT, GL_FALSE},
              {2, GL_FLOAT, GL_FALSE},
          },
  };
  vgfx_gl_vertex_array_layout(&va, &layout0);

  VGFX_GL_VertexAttribLayout layout1 = {
      .buffer = dbuff,
      .update_freq = 1,
      .attribs =
          {
              {4, GL_FLOAT, GL_FALSE},
              {4, GL_FLOAT, GL_FALSE},
              {4, GL_FLOAT, GL_FALSE},
              {4, GL_FLOAT, GL_FALSE},
          },
  };
  vgfx_gl_vertex_array_layout(&va, &layout1);

  vgfx_gl_vertex_array_index_buffer(&va, &ibuff);

  mat4 *buff = (mat4 *)calloc(MAX_INSTANCE, sizeof(mat4));

  const usize INITIAL_OBJ_COUNT = 10000;

  typedef struct Object {
    vec3 pos;
    vec3 scl;
    vec2 dir;
  } Object;

  VSTD_Vector(Object) objs = vstd_vector_with_capacity(Object, MAX_INSTANCE);

  srand(time(NULL));

  for (usize i = 0; i < INITIAL_OBJ_COUNT; ++i) {
    f32 x = (WINDOW_WIDTH / 2.0f) - rand() % WINDOW_WIDTH;
    f32 y = (WINDOW_HEIGHT / 2.0f) - rand() % WINDOW_HEIGHT;

    f32 dirx = (rand() % 100 < 50) ? (rand() % 100) / -100.0f
                                   : (rand() % 100) / 100.0f;
    f32 diry = (rand() % 100 < 50) ? (rand() % 100) / -100.0f
                                   : (rand() % 100) / 100.0f;

    vstd_vector_push(Object, (&objs),
                     ((Object){.pos = {x, y, 0.0f},
                               .scl = {25.0f, 25.0f, 1.0f},
                               .dir = {dirx, diry}}));
  }

  bool run = true;

  volatile u32 fps_counter, fps;
  volatile f64 fps_timer = 0.0f;
  volatile f64 dt, last_frame;
  while (run) {
    // Time and delta time
    f64 time = glfwGetTime();

    dt = time - last_frame;
    last_frame = time;

    fps_timer += dt;
    fps_counter += 1;
    if (fps_timer > 1.0f) {
      fps_timer = 0;
      fps = fps_counter;
      fps_counter = 0;
      printf("FPS: %u\n", fps);
    }

    VSTD_Vector(VGFX_OS_Event) events = vgfx_os_events(win);
    vstd_vector_iter(VGFX_OS_Event, events, {
      if (_$iter->type == VGFX_OS_EVENT_TYPE_WINDOW_CLOSE) {
        run = false;
      }
    });

    // Update camera view
    vgfx_camera_update_view(s_camera);

    // Calculate mvp matrix
    mat4 vpm;
    vgfx_camera_get_matrix(s_camera, vpm);

    // Set instance data
    f32 ww = WINDOW_WIDTH / 2.0f;
    f32 wh = WINDOW_HEIGHT / 2.0f;
    usize instance_count = 0;
    vec2 tmp;
    const f32 speed = 100.0f * dt;

    for (usize i = 0; i < objs.len; ++i) {
      Object *obj = &vstd_vector_get(Object, objs, i);

      glm_vec2_scale(obj->dir, speed, tmp);

      if (obj->pos[0] + tmp[0] < -ww || obj->pos[0] + tmp[0] > ww) {
        obj->dir[0] = -obj->dir[0];
      } else {
        obj->pos[0] += tmp[0];
      }

      if (obj->pos[1] + tmp[1] < -wh || obj->pos[1] + tmp[1] > wh) {
        obj->dir[1] = -obj->dir[1];
      } else {
        obj->pos[1] += tmp[1];
      }

      glm_mat4_identity(buff[i]);
      glm_translate(buff[i], obj->pos);
      glm_scale(buff[i], obj->scl);
      glm_mat4_mul(vpm, buff[i], buff[i]);

      instance_count += 1;
    }

    vgfx_gl_buffer_sub_data(&dbuff, 0, sizeof(mat4) * instance_count, &buff[0]);

    // Get from handle
    VGFX_AS_Shader *shader;
    VGFX_ASSET_CAST(program, VGFX_ASSET_TYPE_SHADER, shader);

    // Render the scene
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    vgfx_gl_bind_shader_program(program);

    vgfx_shader_program_uniform_f1(shader->handle, "u_time", (f32)time);
    vgfx_shader_program_uniform_i1(shader->handle, "u_texture", 0);

    vgfx_gl_bind_texture_handle(texture, 0);

    glBindVertexArray(va.handle);

    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0,
                            instance_count);

    glBindVertexArray(0);
    vgfx_gl_unbind_texture_handle(0);
    vgfx_gl_unbind_shader_program();

    vgfx_os_window_swap_buffers(win);
    vgfx_os_poll_events();
  }

  // Delete vectors
  vstd_vector_free(Object, (&objs));
  free(buff);

  // Delete render pipeline
  vgfx_gl_vertex_array_delete(&va);
  vgfx_gl_buffer_delete(&vbuff);
  vgfx_gl_buffer_delete(&ibuff);
  vgfx_gl_buffer_delete(&dbuff);

  // Free resources
  // vgfx_texture_free(texture);
  vgfx_camera_free(s_camera);

  // Free the vgfx
  vgfx_as_asset_server_free(asset_server);
  vgfx_os_window_free(win);
  // vgfx_window_free(window);
  // vgfx_terminate();

  return 0;
}
