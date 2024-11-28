#ifndef WEMESH_H
#define WEMESH_H

#include "cglm.h"
#include "wecomponent.h"

struct cgltf_data;
struct cgltf_mesh;

struct WEvertex {
	vec3 position;
	vec3 normal;
	vec2 texcoord;
};

struct WEmaterial;

struct WEmeshprimitive {
	struct WEvertex *vertices;
	size_t vertices_cnt;

	int *indices;
	size_t indices_cnt;

	struct WEmaterial *material;

	unsigned int vao;
	unsigned int vbo;
	unsigned int ebo;
};

struct WEmesh {
	struct WEcomponent com;
	struct WEnode *node;

	struct WEmeshprimitive *primitives;
	int primitives_cnt;
};

void WEmesh_init_gltf(struct WEmesh *dest, struct cgltf_data *d, struct cgltf_mesh *m);

void WEmesh_build_renderdata(struct WEmeshprimitive *meshprim);

#endif