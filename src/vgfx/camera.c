#include "camera.h"

/*****************************************************************************
 * - VGFX Camera
 * */

VGFX_Camera *vgfx_camera_new(f32 fov, f32 width, f32 height, f32 near, f32 far,
                             VGFX_CameraMode mode) {
  VGFX_Camera *camera = (VGFX_Camera *)malloc(sizeof(VGFX_Camera));
  *camera = (VGFX_Camera){
      .position = {0.0f, 0.0f, 0.0f},
      .front = {0.0f, 0.0f, -1.0f},
      .world_up = {0.0f, 1.0f, 0.0f},
  };

  // Set camera's up vector
  glm_vec3_copy(camera->world_up, camera->up);

  // Set camera's right vector
  glm_vec3_cross(camera->front, camera->up, camera->right);
  glm_vec3_normalize(camera->right);

  // Create projection matrix
  vgfx_camera_update_projection(camera, fov, width, height, near, far, mode);

  // Create view matrix
  glm_lookat(camera->position, camera->front, camera->up, camera->view);

  return camera;
}

void vgfx_camera_free(VGFX_Camera *camera) { free(camera); }

/*****************************************************************************
 * - Camera Projection & View Functions
 * */

void vgfx_camera_update_projection(VGFX_Camera *camera, f32 fov, f32 width,
                                   f32 height, f32 near, f32 far,
                                   VGFX_CameraMode mode) {
  f32 w = width / 2.0f;
  f32 h = height / 2.0f;

  camera->fov = fov;
  camera->aspect_ratio = width / height;
  camera->near = near;
  camera->far = far;
  camera->mode = mode;

  switch (camera->mode) {
  case VGFX_CameraModePerspective:
    glm_perspective(camera->fov, camera->aspect_ratio, camera->near,
                    camera->far, camera->projection);

    camera->position[2] = 3.0f;
    break;
  case VGFX_CameraModeOrthographic:
    glm_ortho(-w, w, -h, h, camera->near, camera->far, camera->projection);

    camera->position[2] = camera->far - 0.1f;
    break;
  }
}

void vgfx_camera_update_view(VGFX_Camera *camera) {
  vec3 target;
  glm_vec3_add(camera->position, camera->front, target);
  glm_lookat(camera->position, target, camera->up, camera->view);
}

/*****************************************************************************
 * - Camera Helper Functions
 * */

void vgfx_camera_get_matrix(VGFX_Camera *camera, mat4 dest) {
  glm_mat4_mul(camera->projection, camera->view, dest);
}
