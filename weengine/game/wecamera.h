#ifndef WECAMERA_H
#define WECAMERA_H

#include "cglm.h"
#include "wecomponent.h"

struct WEnode;

struct WEcamera {
	struct WEcomponent com;
	struct WEnode *node;

	float fov_y; /* radian */
	float aspect; /* width / height */
	float near_z;
	float far_z;
	vec3 target;

	mat4 view;
	mat4 projection;
};

struct WEcamera *WEcamera_get(struct WEnode *n);

struct WEcamera *WEcamera_new();

void WEcamera_init_lens_normal_35(struct WEcamera *c);

#endif