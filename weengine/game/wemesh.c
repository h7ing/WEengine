#include "wemesh.h"
#include <stdlib.h>
#include <string.h>
#include <GL/glew.h>
#include "cgltf.h"
#include "cglm.h"
#include "weasset.h"
#include "wematerial.h"

void WEmeshprimitive_clear(struct WEmeshprimitive *prim) {
	glDeleteBuffers(1, &prim->vbo);
	glDeleteBuffers(1, &prim->ebo);
	glDeleteVertexArrays(1, &prim->vao);

	free(prim->vertices);
	free(prim->indices);
}

void WEmesh_delete(struct WEmesh *m) {
	for (int i = 0; i < m->primitives_cnt; i++) {
		WEmeshprimitive_clear(&m->primitives[i]);
	}
	free(m->primitives);
	free(m);
}

void WEmesh_init_gltf(struct WEmesh *dest, struct cgltf_data *d, struct cgltf_mesh *m) {

	dest->primitives_cnt = m->primitives_count;
	dest->primitives = malloc(dest->primitives_cnt * sizeof(*dest->primitives));
	printf("mesh primitive count:%d\n", m->primitives_count);


	for (int i = 0; i < m->primitives_count; i++) {

		struct cgltf_primitive *prim = &(m->primitives[i]);

		struct WEmeshprimitive *meshprim = &(dest->primitives[i]);

		// 现在先只做三角形
		// todo: support other primitive types
		if (prim->type != cgltf_primitive_type_triangles) {
			memset(meshprim, 0, sizeof(*meshprim));
			continue;
		}

		// strip or fan 的三角形数量 = 顶点数-2

		// 先根据顶点数分配内存
		for (int j = 0; j < prim->attributes_count; j++) {
			struct cgltf_attribute *attr = &(prim->attributes[j]);
			if (attr->type == cgltf_attribute_type_position) {
				meshprim->vertices_cnt = attr->data->count;
				meshprim->vertices = malloc(meshprim->vertices_cnt * sizeof(*meshprim->vertices));
				break;
			}
		}

		// 解析顶点数据
		for (int j = 0; j < prim->attributes_count; j++) {
			struct cgltf_attribute *attr = &(prim->attributes[j]);

			switch (attr->type) {
			case cgltf_attribute_type_position: { // gltf的单位是meter

				if (attr->data->is_sparse) {

					float *tmpbuffer = malloc(attr->data->count * 3 * sizeof(float));
					cgltf_accessor_unpack_floats(attr->data, tmpbuffer, attr->data->count * 3);

					for (int k = 0; k < attr->data->count; k++) {
						struct WEvertex *vertex = &(meshprim->vertices[k]);
						glm_vec3(tmpbuffer + k*3, vertex->position);
						// glm_vec3_divs(vertex->position, 10.0f, vertex->position);

						// printf("sparse vertex %d:%.1f,%.1f,%.1f\n", k, vertex->position[0], vertex->position[1], vertex->position[2]);
					}

					free(tmpbuffer);

				} else {
					for (int k = 0; k < attr->data->count; k++) {
						struct WEvertex *vertex = &(meshprim->vertices[k]);
						cgltf_accessor_read_float(attr->data, k, vertex->position, 3);
						// glm_vec3_divs(vertex->position, 100.0f, vertex->position);
						// printf("vertex %d:%.1f,%.1f,%.1f\n", k, vertex->position[0], vertex->position[1], vertex->position[2]);
					}
				}
			}
				
				break;

			case cgltf_attribute_type_normal:
				break;

			case cgltf_attribute_type_texcoord:
				break;

			/* todo: other attr types */
			
			default:
				break;
			}

			
		}

		// 解析索引数据
		if (prim->indices) {
			meshprim->indices_cnt = prim->indices->count;
			meshprim->indices = malloc(meshprim->indices_cnt * sizeof(*meshprim->indices));
			if (prim->indices->is_sparse) {
				// mark: error case
			} else {
				for (int k = 0; k < meshprim->indices_cnt; k++) {
					meshprim->indices[k] = cgltf_accessor_read_index(prim->indices, k);
					// printf("vertex index %d:%d\n", k, meshprim->indices[k]);
				}
			}
		} else {
			meshprim->indices_cnt = 0;
			meshprim->indices = NULL;
		}

		/* todo: support material */
		meshprim->material = WEmaterial_get_default();
		WEmesh_build_renderdata(meshprim);
	}
}

void WEmesh_build_renderdata(struct WEmeshprimitive *meshprim) {
	glGenVertexArrays(1, &meshprim->vao);
	glBindVertexArray(meshprim->vao);

	glGenBuffers(1, &meshprim->vbo);
	glGenBuffers(1, &meshprim->ebo);

	glBindBuffer(GL_ARRAY_BUFFER, meshprim->vbo);
	glBufferData(GL_ARRAY_BUFFER, meshprim->vertices_cnt * sizeof(*meshprim->vertices), meshprim->vertices, GL_STATIC_DRAW);


	if (meshprim->indices_cnt > 0) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshprim->ebo);
		int indicessize = meshprim->indices_cnt * sizeof(*meshprim->indices);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicessize, meshprim->indices, GL_STATIC_DRAW);
	}

	int stride = sizeof(*meshprim->vertices);

	// layout=0 vertex positions
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void *)offsetof(struct WEvertex, position));
	glEnableVertexAttribArray(0);

    // layout=1 vertex normals
    
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void *)offsetof(struct WEvertex, normal));
	glEnableVertexAttribArray(1);

    // layout=2 vertex texture coords
    
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void *)offsetof(struct WEvertex, texcoord));
	glEnableVertexAttribArray(2);



    glBindVertexArray(0);
}
