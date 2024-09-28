#pragma once

#include <stdlib.h>
#include <cglm/cglm.h>


struct HENode;
struct HEScene;
struct HEWorld;
struct HETransform;
struct HERawBuffer;


typedef struct HETransform {
	vec3 translation;
	vec4 rotation; // 四元数
	vec3 scale;
	mat4 matrix;

} HETransform;


typedef struct HERawBuffer {
	void *key;
	void *data; // copyed from gltf raw buffer
	size_t data_size;
	struct HERawBuffer *next;
} HERawBuffer;


typedef struct HEBufferLayout {
	void *buffer;
	size_t buffer_size;
	int stride;
	size_t count;
} HEBufferLayout;


typedef struct HEOpenGLData {
	unsigned int vao;
	unsigned int vbo;
	unsigned int ebo;
	unsigned int shader;
} HEOpenGLData;


typedef struct HEDrawElement {
	unsigned int *indices;

	int vertex_count;

	HEBufferLayout position_buf;
	HEBufferLayout normal_buf;
	HEBufferLayout texcoord_buf;
	HEBufferLayout index_buf;

	HEOpenGLData gldata;
} HEDrawElement;


typedef struct HEMesh {
	HEDrawElement *elements;
	int element_count;
} HEMesh;


typedef struct HENode {
	struct HENode *parent;
	struct HENode *prev;
	struct HENode *next;
	struct HENode *child;

	struct HETransform transform; // local
	struct HEMesh *mesh;
} HENode;


typedef struct HEScene {
	HENode *root; // 有个默认的根节点，不显示；实际直接显示下一级的子节点
} HEScene;


typedef struct HEWorld {
	HEScene *scene; // 先支持一个场景

	HERawBuffer *buffer_list;
} HEWorld;


// mesh

HEMesh *mesh_create(int elementcount);


// node op

HENode *node_get_tail(HENode *node);

void node_attach_tail(HENode *parent, HENode *child);

HENode *node_create();

void node_destroy(HENode *node);


// scene

HEScene *scene_create();

void scene_destroy(HEScene *scene);


// world

HEWorld *world_create();

void world_destroy(HEWorld *world);

HERawBuffer *world_get_gltf_buffer(HEWorld *world, void *key, size_t size);



// // vector

// typedef void (*dealloc_func_ptr) (void *);

// typedef struct HEVector {
// 	size_t capacity;
// 	size_t size;
// 	void *elements;
// 	dealloc_func_ptr dealloc_func;
// } HEVector;

// void hevec_new(dealloc_func_ptr dealloc);

// void hevec_pushback();
