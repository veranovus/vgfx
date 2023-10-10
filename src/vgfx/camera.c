#include "camera.h"

// =============================================
//
//
// Camera
//
//
// =============================================

VGFX_RD_Camera *
vgfx_rd_camera_new(VGFX_RD_CameraDesc *desc) {

  VGFX_ASSERT_NON_NULL(desc);

  VGFX_RD_Camera *camera = (VGFX_RD_Camera*) malloc(sizeof(VGFX_RD_Camera));
  *camera = (VGFX_RD_Camera) {
    .mode = desc->mode,
    .position = {desc->position[0], desc->position[1], desc->position[2]},
    .front = {0.0f, 0.0f, -1.0f},
    .world_up = {0.0f, 1.0f, 0.0f},
  };

  // Set camera's up vector
  glm_vec3_copy(camera->world_up, camera->up);

  // Set camera's right vector
  glm_vec3_cross(camera->front, camera->up, camera->right);
  glm_vec3_normalize(camera->right);

  // Create projection matrix
  vgfx_rd_camera_projection_settings(camera, &desc->projection_settings);

  // Create view matrix
  glm_lookat(camera->position, camera->front, camera->up, camera->view);

  return camera;
}

void
vgfx_rd_camera_free(VGFX_RD_Camera *camera) {
  
  VGFX_ASSERT_NON_NULL(camera);

  free(camera);
}

void
vgfx_rd_camera_projection_settings(VGFX_RD_Camera *camera, VGFX_RD_CameraPS *ps) {
  
  VGFX_ASSERT_NON_NULL(camera);
  VGFX_ASSERT_NON_NULL(ps);

  camera->projection_settings = *ps;

  switch (camera->mode) {
  case VGFX_RD_CAMERA_MODE_ORTHO:
    glm_ortho(
        0, 
        camera->projection_settings.ortho_width, 
        0, 
        camera->projection_settings.ortho_height,
        camera->projection_settings.near, 
        camera->projection_settings.far, 
        camera->projection);
    break;
  case VGFX_RD_CAMERA_MODE_PERSP:
    glm_perspective(
        camera->projection_settings.persp_fov, 
        camera->projection_settings.persp_aspect_ratio, 
        camera->projection_settings.near,
        camera->projection_settings.far, 
        camera->projection);
    break;
  }
}

void
vgfx_rd_camera_update_view(VGFX_RD_Camera *camera) {
  
  VGFX_ASSERT_NON_NULL(camera);

  vec3 target;
  glm_vec3_add(camera->position, camera->front, target);
  glm_lookat(camera->position, target, camera->up, camera->view);
}

void
vgfx_rd_camera_combined_matrix(VGFX_RD_Camera *camera, mat4 dest) {

  VGFX_ASSERT_NON_NULL(camera);
  
  glm_mat4_mul(camera->projection, camera->view, dest);
}
