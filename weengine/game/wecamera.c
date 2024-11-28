#include "wecamera.h"
#include <stddef.h>
#include <cglm.h>
#include "weworld.h"
#include "wenode.h"

struct WEcamera *WEcamera_get(struct WEnode *n) {
	char *c = (char *)WEnode_get_comp(n, "camera");
	if (c) {
		struct WEcamera *camera = (struct WEcamera *)(c - offsetof(struct WEcamera, com));

		return camera;
	}

	return NULL;
}

struct WEcamera *WEcamera_new() {
	struct WEcamera *c = calloc(1, sizeof(*c));
	c->com.name = "camera";
	return c;
}

void WEcamera_init_lens_normal_35(struct WEcamera *c) {

	glm_vec3_copy((vec3){0,100.0f,100.0f}, c->node->transform.position);

	glm_vec3_copy((vec3){0,50.0f,-1.0f}, c->target);
	c->fov_y = 63.4f * M_PI / 180.0f; /* 35mm lens */
	int width, height;
	WEworld_get_window_size(we_world, &width, &height);
	c->aspect = (float)width / (float)height;
	c->near_z = 0.1f;
	c->far_z = 1000.0f;

	glm_lookat(c->node->transform.position, c->target, GLM_YUP, c->view);
	glm_perspective(c->fov_y, c->aspect, c->near_z, c->far_z, c->projection);
	// glm_ortho(-100.0f, 100.0f, -100.0f, 100.0f, c->near_z, c->far_z, c->projection);

	c->projection_mode = 0;
	c->target_projmode = 0;

	// glm_mat4_identity(c->view);
	// glm_mat4_identity(c->projection);
}

void WEcamera_set_projection_anim(struct WEcamera *c, int toproj) {
	c->target_projmode = toproj;
	c->animprogress = 0;
}

void
WEmath_mat4_add(mat4 m1, mat4 m2, mat4 dest) {
  float a00 = m1[0][0], a01 = m1[0][1], a02 = m1[0][2], a03 = m1[0][3],
        a10 = m1[1][0], a11 = m1[1][1], a12 = m1[1][2], a13 = m1[1][3],
        a20 = m1[2][0], a21 = m1[2][1], a22 = m1[2][2], a23 = m1[2][3],
        a30 = m1[3][0], a31 = m1[3][1], a32 = m1[3][2], a33 = m1[3][3],

        b00 = m2[0][0], b01 = m2[0][1], b02 = m2[0][2], b03 = m2[0][3],
        b10 = m2[1][0], b11 = m2[1][1], b12 = m2[1][2], b13 = m2[1][3],
        b20 = m2[2][0], b21 = m2[2][1], b22 = m2[2][2], b23 = m2[2][3],
        b30 = m2[3][0], b31 = m2[3][1], b32 = m2[3][2], b33 = m2[3][3];

  dest[0][0] = a00 + b00;
  dest[0][1] = a01 + b01;
  dest[0][2] = a02 + b02;
  dest[0][3] = a03 + b03;
  dest[1][0] = a10 + b10;
  dest[1][1] = a11 + b11;
  dest[1][2] = a12 + b12;
  dest[1][3] = a13 + b13;
  dest[2][0] = a20 + b20;
  dest[2][1] = a21 + b21;
  dest[2][2] = a22 + b22;
  dest[2][3] = a23 + b23;
  dest[3][0] = a30 + b30;
  dest[3][1] = a31 + b31;
  dest[3][2] = a32 + b32;
  dest[3][3] = a33 + b33;
}

/* mark: 在透视和正交投影之间动态变换 */
int WEcamera_anim_switch_projection(struct WEworld *w) {
	struct WEcamera *c = w->the_camera;

	if (c->target_projmode == c->projection_mode) {
		return 1;
	}

	c->animprogress += w->curdelta;

	if (c->animprogress >= 1.0f) {
		c->animprogress = 1.0f;
		c->projection_mode = c->target_projmode;
	}

	if (c->target_projmode == 0) {
		// to persp
		// Mproj = (1-t)Mortho + (t)Mpersp

		glm_perspective(c->fov_y, c->aspect, c->near_z, c->far_z, c->persp);
		glm_mat4_scale(c->persp, c->animprogress);

		glm_ortho(-100.0f, 100.0f, -100.0f, 100.0f, c->near_z, c->far_z, c->ortho);
		glm_mat4_scale(c->ortho, 1.0f - c->animprogress);

	} else {
		// to ortho:
		// Mproj = (1-t)Mpersp + (t)Mortho

		glm_perspective(c->fov_y, c->aspect, c->near_z, c->far_z, c->persp);
		glm_mat4_scale(c->persp, 1.0f - c->animprogress);

		glm_ortho(-100.0f, 100.0f, -100.0f, 100.0f, c->near_z, c->far_z, c->ortho);
		glm_mat4_scale(c->ortho, c->animprogress);
	}

	WEmath_mat4_add(c->persp, c->ortho, c->projection);

	return 0;
}