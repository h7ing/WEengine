#ifndef WECAMERA_H
#define WECAMERA_H

#include "cglm.h"
#include "wecomponent.h"

struct WEnode;

struct WEworld;

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


	mat4 persp;
	mat4 ortho;

	float animprogress;


	int projection_mode; // 0-persp 1-ortho
	int target_projmode;
};

struct WEcamera *WEcamera_get(struct WEnode *n);

struct WEcamera *WEcamera_new();

void WEcamera_init_lens_normal_35(struct WEcamera *c);

int WEcamera_anim_switch_projection(struct WEworld *w);
void WEcamera_set_projection_anim(struct WEcamera *c, int toproj);

#endif