#include "wegeometry.h"
#include <stdlib.h>
#include "cgltf.h"
#include "weasset.h"
#include "wemesh.h"
#include "wehashmap.h"

struct WEgeometry *WEgeometry_new() {
	struct WEgeometry *geo = calloc(1, sizeof(*geo));
	return geo;
}

void WEgeometry_delete(struct WEGeometry *geo) {
	free(geo);
}

struct WEgeometry *WEgeometry_loadfromfile(char *path) {
	struct WEgeometry *geo = WEgeometry_new();

	char *fullpath = WEasset_gen_fullpath(path);

	cgltf_options options = {0};
	cgltf_data* data = NULL;

	cgltf_result result = cgltf_parse_file(&options, fullpath, &data);

	if (result == cgltf_result_success)
		result = cgltf_load_buffers(&options, data, fullpath);

	if (result == cgltf_result_success)
		result = cgltf_validate(data);

	if (result == cgltf_result_success) {
		WEgeometry_init_gltf(geo, data);
	}

	free(fullpath);
	cgltf_free(data);

	return geo;
}

void WEgeometry_init_gltf(struct WEgeometry *geo, struct cgltf_data *d) {

	struct WEhashmap *meshmap = WEhashmap_new();

	geo->mesh_cnt = d->meshes_count;
	geo->meshes = malloc(geo->mesh_cnt * sizeof(struct WEmesh));

	for (int i = 0; i < d->meshes_count; i++) {
		struct cgltf_mesh *meshdata = &(d->meshes[i]);
		struct WEmesh *mesh = &(geo->meshes[i]);

		/* todo: copy meshes */

		WEhashmap_add(meshmap, meshdata, mesh);
	}


	for (int i = 0; i < d->scene->nodes_count; i++) {
		cgltf_node *n = d->scene->nodes[i];

		if (!n->parent) {
			/* todo: parse from root node */
			/* 从根节点开始解析 */

			/* todo: 改进节点列表结构: geometrynodelist_add() or create()
			 */

			struct WEgeometrynode *geonode = calloc(1, sizeof(*geonode));

			if (!geo->nodes) {
				geo->nodes = geonode;
			} else {
				struct WEgeometrynode *prev = geo->nodes;
				while (prev->next) {
					prev = prev->next;
				}

				prev->next = geonode;
				geonode->prev = prev;
			}

			WEgeometrynode_init_gltf(geonode, geo, n, meshmap);
		}
	}

	WEhashmap_delete(meshmap);
}

void WEgeometrynode_init_gltf(struct WEgeometrynode *geonode, struct WEgeometry *geo, struct cgltf_node *d, struct WEhashmap *mashmap) {

	/* node的transform可能用trs向量表示，也可能用matrix表示 */
	if (d->has_matrix) {
		/* 都是column major，直接拷贝 */
		glm_mat4_copy(d->matrix, geonode->transform.matrix);

		vec4 t;
		mat4 r;

		glm_decompose(geonode->transform.matrix, t, r, geonode->transform.scale);

		glm_vec3(t, geonode->transform.position);

		glm_mat4_quat(r, geonode->transform.rotation);

	} else {
		if (d->has_translation) {
			glm_vec3_copy(d->translation, geonode->transform.position);
		}

		if (d->has_rotation) {
			glm_quat_copy(d->rotation, geonode->transform.rotation);
		}

		if (d->has_scale) {
			glm_vec3_copy(d->scale, geonode->transform.scale);
		}
	}


	if (d->mesh) {
		geonode->mesh = WEhashmap_find(mashmap, d->mesh);
	}

	geonode->children = calloc(d->children_count, sizeof(geonode->children[0]));

	for (int i = 0; i < d->children_count; i++) {

		/* todo: 优化列表的初始化 */

		struct WEgeometrynode *child = calloc(1, sizeof(*child));
		child->parent = geonode;

		struct WEgeometrynode *prev = geonode->children[0];

		while (prev && prev->next) {
			prev = prev->next;
		}

		child->prev = prev;

		if (prev) {
			prev->next = child;
		} else {
			geonode->children[0] = child;
		}


		WEgeometrynode_init_gltf(child, geo, d->children[i], mashmap);

	}
}