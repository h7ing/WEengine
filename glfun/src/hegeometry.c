#include "hegeometry.h"

#include <string.h>

// 

HEMesh *mesh_create(int elementcount) {
	HEMesh *mesh = malloc(sizeof(HEMesh));
	mesh->element_count = elementcount;
	if (elementcount > 0) {
		mesh->elements = calloc(elementcount, sizeof(HEDrawElement));
	} else {
		mesh->elements = NULL;
	}
	return mesh;
}

HENode *node_create() {
	HENode *node = calloc(1, sizeof(HENode));
	glm_mat4_identity(node->transform.matrix);

	glm_quat_identity(node->transform.rotation);
	glm_vec3_one(node->transform.scale);

	return node;
}



static void drawelement_destroy(HEDrawElement *element) {
	free(element->indices);
}


static void mesh_destroy(HEMesh *mesh) {
	if (mesh->elements) {
		for (int i = 0; i < mesh->element_count; i++) {
			drawelement_destroy(&mesh->elements[i]);
		}
		free(mesh->elements);
	}
	free(mesh);
}


static void node_destroy_self(HENode *node) {
	if (node->mesh) {
		mesh_destroy(node->mesh);
	}
	free(node);
}

void node_destroy(HENode *node) {
	if (node->child) {
		node_destroy(node->child);
	}

	HENode *sibling = node->next;
	HENode *nextnode;
	HENode *prevnode;
	while (sibling) {
		nextnode = sibling->next;
		prevnode = sibling->prev;
		if (sibling->child) {
			node_destroy(sibling->child);
		}
		node_destroy_self(sibling);
		sibling = nextnode;
	}

	node_destroy_self(node);
}

HEScene *scene_create() {
	HEScene *scene = calloc(1, sizeof(HEScene));
	scene->root = node_create();
	return scene;
}

void scene_destroy(HEScene *scene) {

	node_destroy(scene->root);

	free(scene);
}

static void buffer_destroy(HERawBuffer *bufferlist) {
	HERawBuffer *cur = bufferlist;
	HERawBuffer *next;
	while (cur) {
		next = cur->next;
		free(cur->data);
		cur = next;
	}
}

void world_destroy(HEWorld *world) {
	if (world) {
		if (world->scene) {

			scene_destroy(world->scene);
		}

		if (world->buffer_list) {
			buffer_destroy(world->buffer_list);
		}

		free(world);
		world = NULL;
	}
}

HEWorld *world_create() {
	HEWorld *world = calloc(1, sizeof(HEWorld));
	return world;
}

HERawBuffer *world_get_gltf_buffer(HEWorld *world, void *key, size_t size) {
	HERawBuffer *buffernode = world->buffer_list;
	HERawBuffer *tail = buffernode;

	while (buffernode) {
		if (buffernode->key == key) {
			return buffernode;
		}

		tail = buffernode;
		buffernode = buffernode->next;
	}

	HERawBuffer *copybuffer = malloc(sizeof(HERawBuffer));
	copybuffer->data = malloc(size);
	memcpy(copybuffer->data, key, size);
	copybuffer->key = key;
	copybuffer->next = NULL;
	copybuffer->data_size = size;

	if (tail) {
		tail->next = copybuffer;
	} else {
		world->buffer_list = copybuffer;
	}

	return copybuffer;
}


// node op

HENode *node_get_tail(HENode *node) {
	HENode *child = node->child;

	while (child) {
		if (child->next) {
			child = child->next;
		} else {
			return child;
		}
	}

	return child;
}

void node_attach_tail(HENode *parent, HENode *child) {
	child->parent = parent;

	HENode *tail = node_get_tail(parent);
	if (!tail) {
		parent->child = child;
	} else {
		tail->next = child;
		child->prev = tail;
	}
}
