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
      .editor_mode = false,
      ._cursor_last = {0.0f, 0.0f},
  };

  vgfx_camera3d_update_vectors(camera);

  return camera;
}

void vgfx_camera3d_free(VGFX_Camera3D *camera) {
  vgfx_camera_free(camera->camera);

  free(camera);
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

void vgfx_camera3d_respond_key(VGFX_Camera3D *camera, i32 key, f64 dt) {
  if (!camera->editor_mode) {
    return;
  }

  const f32 velocity = camera->speed * (f32)dt;

  vec3 change;

  if (key == GLFW_KEY_W) {
    glm_vec3_scale(camera->camera->front, velocity, change);
    glm_vec3_add(camera->camera->position, change, camera->camera->position);
  }
  if (key == GLFW_KEY_S) {
    glm_vec3_scale(camera->camera->front, velocity, change);
    glm_vec3_sub(camera->camera->position, change, camera->camera->position);
  }

  if (key == GLFW_KEY_D) {
    glm_vec3_scale(camera->camera->right, velocity, change);
    glm_vec3_add(camera->camera->position, change, camera->camera->position);
  }
  if (key == GLFW_KEY_A) {
    glm_vec3_scale(camera->camera->right, velocity, change);
    glm_vec3_sub(camera->camera->position, change, camera->camera->position);
  }
}

void vgfx_camera3d_respond_cursor(VGFX_Camera3D *camera, f32 x, f32 y) {
  f32 x_offset = (x - camera->_cursor_last[0]) * camera->sensivity;
  f32 y_offset = (camera->_cursor_last[1] - y) * camera->sensivity;

  camera->_cursor_last[0] = x;
  camera->_cursor_last[1] = y;

  if (!camera->editor_mode) {
    return;
  }

  camera->yaw += x_offset;
  camera->pitch += y_offset;

  if (camera->pitch > 89.0f) {
    camera->pitch = 89.0f;
  }
  if (camera->pitch < -89.0f) {
    camera->pitch = -89.0f;
  }

  vgfx_camera3d_update_vectors(camera);
}
