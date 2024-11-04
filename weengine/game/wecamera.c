#include "wecamera.h"
#include <stddef.h>
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

	glm_vec3_copy((vec3){0,0,1.0f}, c->node->transform.position);

	glm_vec3_copy((vec3){0,0,-1.0f}, c->target);
	c->fov_y = 63.4f * M_PI / 180.0f; /* 35mm lens */
	int width, height;
	WEworld_get_window_size(we_world, &width, &height);
	c->aspect = (float)width / (float)height;
	c->near_z = 0.1f;
	c->far_z = 100.0f;

	glm_lookat(c->node->transform.position, c->target, GLM_YUP, c->view);
	glm_perspective(c->fov_y, c->aspect, c->near_z, c->far_z, c->projection);
}