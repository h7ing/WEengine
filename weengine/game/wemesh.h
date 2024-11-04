#ifndef WEMESH_H
#define WEMESH_H

#include "cglm.h"
#include "wecomponent.h"

struct cgltf_data;

struct WEvertex {
	vec3 position;
	vec3 normal;
	vec2 texcoord;
};

struct WEmesh {
	struct WEcomponent com;
	struct WEnode *node;

	struct WEvertex *vertices;
	size_t vertices_cnt;

	int *indices;
	size_t indices_cnt;

	unsigned int shader;

	unsigned int vao;
	unsigned int vbo;
	unsigned int ebo;
};

struct WEmesh *WEmesh_new();

void WEmesh_init_gltf(struct WEmesh *m, struct cgltf_data *d);

#endif