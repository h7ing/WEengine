#ifndef WETRANSFORM_H
#define WETRANSFORM_H

#include "cglm.h"

struct WEtransform {
	vec3 position;
	vec4 rotation;
	vec3 scale;
	mat4 matrix;
};

#endif