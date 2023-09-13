#include "camera_3d.h"

// VGFX_Camera3D
// -------------

VGFX_Camera3D *vgfx_camera3d_new(f32 fov, f32 width, f32 height, f32 near,
                                 f32 far) {
  VGFX_Camera3D *camera = (VGFX_Camera3D *)malloc(sizeof(VGFX_Camera3D));
  *camera = (VGFX_Camera3D){
      .camera = vgfx_camera_new(fov, width, height, near, far,
                                VGFX_CameraModePerspective),
      .yaw = -90.0f,
      .pitch = 0.0f,
      .speed = VGFX_CAMERA3D_DEFAULT_SPEED,
      .sensivity = VGFX_CAMERA3D_DEFAULT_SENSIVITY,
  };

  vgfx_camera3d_update_vectors(camera);

  return camera;
}

void vgfx_camera3d_free(VGFX_Camera3D *camera) {
  vgfx_camera_free(camera->camera);

  free(camera);
}

void vgfx_camera3d_handle_input(VGFX_Camera3D *camera,
                                const VGFX_Window *window, f64 dt,
                                bool editor_mode) {
  _vgfx_camera3d_key_input(camera, window, dt, editor_mode);
  _vgfx_camera3d_cursor_input(camera, window, editor_mode);

  vgfx_camera3d_update_vectors(camera);
}

// Helper functions
// ----------------

void vgfx_camera3d_update_vectors(VGFX_Camera3D *camera) {
  // Calculate front vector
  camera->camera->front[0] =
      cosf(glm_rad(camera->yaw)) * cosf(glm_rad(camera->pitch));
  camera->camera->front[1] = sinf(glm_rad(camera->pitch));
  camera->camera->front[2] =
      sinf(glm_rad(camera->yaw)) * cosf(glm_rad(camera->pitch));
  glm_vec3_normalize(camera->camera->front);

  // Calculate right and up vector
  glm_vec3_cross(camera->camera->front, camera->camera->world_up,
                 camera->camera->right);
  glm_vec3_normalize(camera->camera->right);

  glm_vec3_cross(camera->camera->right, camera->camera->front,
                 camera->camera->up);
  glm_vec3_normalize(camera->camera->up);
}

// Camera input functions
// ----------------------

void _vgfx_camera3d_key_input(VGFX_Camera3D *camera, const VGFX_Window *window,
                              f64 dt, bool editor_mode) {
  if (!editor_mode) {
    return;
  }

  const f32 velocity = camera->speed * (f32)dt;

  vec3 change;

  if (vgfx_window_get_key(window, VGFX_Key_W)) {
    glm_vec3_scale(camera->camera->front, velocity, change);
    glm_vec3_add(camera->camera->position, change, camera->camera->position);
  }
  if (vgfx_window_get_key(window, VGFX_Key_S)) {
    glm_vec3_scale(camera->camera->front, velocity, change);
    glm_vec3_sub(camera->camera->position, change, camera->camera->position);
  }

  if (vgfx_window_get_key(window, VGFX_Key_D)) {
    glm_vec3_scale(camera->camera->right, velocity, change);
    glm_vec3_add(camera->camera->position, change, camera->camera->position);
  }
  if (vgfx_window_get_key(window, VGFX_Key_A)) {
    glm_vec3_scale(camera->camera->right, velocity, change);
    glm_vec3_sub(camera->camera->position, change, camera->camera->position);
  }
}

void _vgfx_camera3d_cursor_input(VGFX_Camera3D *camera,
                                 const VGFX_Window *window, bool editor_mode) {
  if (!editor_mode) {
    return;
  }

  vec2 offset;
  bool moved;
  vstd_vector_iter(VGFX_WindowEvent, window->_events, {
    if (_$iter->type != VGFX_WindowEventType_Cursor) {
      continue;
    }

    moved = true;

    offset[0] = _$iter->cursor_offset[0];
    offset[1] = _$iter->cursor_offset[1];
  });

  if (!moved) {
    return;
  }

  camera->yaw += offset[0] * camera->sensivity;
  camera->pitch += offset[1] * camera->sensivity;

  if (camera->pitch > 89.0f) {
    camera->pitch = 89.0f;
  }
  if (camera->pitch < -89.0f) {
    camera->pitch = -89.0f;
  }
}
