#ifndef WEGEOMETRY_H
#define WEGEOMETRY_H

#include "wetransform.h"

struct cgltf_data;
struct cgltf_node;

struct WEnode;
struct WEmesh;
struct WEtransform;
struct WEhashmap;

struct WEgeometrynode {
	struct WEgeometrynode *parent;
	struct WEgeometrynode **children;
	struct WEgeometrynode *prev;
	struct WEgeometrynode *next;
	int children_cnt;

	struct WEmesh *mesh;
	struct WEtransform transform;
};

struct WEgeometry {
	struct WEmesh* meshes;
	int mesh_cnt;

	struct WEgeometrynode *nodes;
};

struct WEgeometry *WEgeometry_new();

void WEgeometry_delete(struct WEgeometry *geo);

struct WEgeometry *WEgeometry_loadfromfile(char *path);

void WEgeometry_init_gltf(struct WEgeometry *geo, struct cgltf_data *d);

void WEgeometrynode_init_gltf(struct WEgeometrynode *geonode, struct WEgeometry *geo, struct cgltf_node *d, struct WEhashmap *mashmap);

#endif