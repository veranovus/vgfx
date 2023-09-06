#include "camera.h"

// VGFX_Camera
// -----------

VGFX_Camera *vgfx_camera_new(f32 fov, f32 width, f32 height, f32 near, f32 far,
                             VGFX_CameraMode mode) {
  VGFX_Camera *camera = (VGFX_Camera *)malloc(sizeof(VGFX_Camera));

  *camera = (VGFX_Camera){
      .position = {0.0, 0.0, 0.0},
      .front = {0.0, 0.0, -1.0},
      .up = {0.0, 1.0, 0.0},
  };

  vgfx_camera_update_projection(camera, fov, width, height, near, far, mode);

  glm_lookat(camera->position, camera->front, camera->up, camera->view);

  return camera;
}

void vgfx_camera_free(VGFX_Camera *camera) { free(camera); }

// Camera helper functions
// -----------------------

void vgfx_camera_update_projection(VGFX_Camera *camera, f32 fov, f32 width,
                                   f32 height, f32 near, f32 far,
                                   VGFX_CameraMode mode) {
  f32 w = width / 2.0;
  f32 h = height / 2.0;

  camera->fov = fov;
  camera->aspect_ratio = width / height;
  camera->near = near;
  camera->far = far;
  camera->mode = mode;

  switch (camera->mode) {
  case VGFX_CameraModePerspective:
    glm_perspective(camera->fov, camera->aspect_ratio, camera->near,
                    camera->far, camera->projection);

    camera->position[2] = 3.0;
    break;
  case VGFX_CameraModeOrthographic:
    glm_ortho(-w, w, -h, h, camera->near, camera->far, camera->projection);

    camera->position[2] = camera->far - 0.1;
    break;
  }
}

void vgfx_camera_update_view(VGFX_Camera *camera) {
  fprintf(stderr, "ERROR: `@%s` is unimplemented.\n", __func__);
  abort();
}

// Camera helper functions
// -----------------------

void vgfx_camera_get_matrix(VGFX_Camera *camera, mat4 dest) {
  glm_mat4_mul(camera->projection, camera->view, dest);
}
