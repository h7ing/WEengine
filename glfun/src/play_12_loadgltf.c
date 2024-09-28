#include "play_12_loadgltf.h"

#include <string.h>
#include <GL/glew.h>

#include <cgltf_write.h>
#include <cgltf.h>

#include "hegl.h"
#include "hengine.h"
#include "hegeometry.h"


HEWorld *world = NULL;


static void init_world() {
	if (!world) {
		world = world_create();
	}
}

///

static void drawelement_load_renderer(HEDrawElement *element) {

	// OpenGL中的array可理解为data，并不是数据结构意义上的array

	glGenVertexArrays(1, &element->gldata.vao);
	glBindVertexArray(element->gldata.vao);

	// vbo
	// todo:这里可能可以把gltf加载的buffer统一传递，通过bufferview的参数设置attrib
	// mark:现在以单个模型文件为单位加载，使用的buffer数据不与其他模型通用
	// 如果整个场景也以模型文件格式保存，则其中的buffer数据如何处理？
	// todo: 参考一下godot的实现
	// https://docs.godotengine.org/zh-cn/4.x/tutorials/assets_pipeline/importing_3d_scenes/available_formats.html
	// https://docs.godotengine.org/zh-cn/4.x/_images/importing_3d_scenes_available_formats_gltf_editor.webp
	
	// 先申请buffer的id，可理解为创建一个buffer的实例
	glGenBuffers(1, &element->gldata.vbo);

	// opengl是不能直接设置buffer id的数据的，需要指明buffer id对应的目标存储类型
	glBindBuffer(GL_ARRAY_BUFFER, element->gldata.vbo);

	// 向目标存储类型填数据，目标需要先绑定id
	glBufferData(GL_ARRAY_BUFFER, element->position_buf.buffer_size, element->position_buf.buffer, GL_STATIC_DRAW);


	// attrib
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, element->position_buf.stride, 0);


	// ebo
	glGenBuffers(1, &element->gldata.ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element->gldata.ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, element->index_buf.buffer_size, element->index_buf.buffer, GL_STATIC_DRAW);

	// shader
	element->gldata.shader = hegl_create_shader_program("shaders/triangle.vert", "shaders/triangle.frag");
}

static void drawelement_init_by_gltfdata(HEDrawElement *element, cgltf_primitive *primitive) {

	// indices
	if (primitive->indices) {

		cgltf_accessor *accessor = primitive->indices;

		// 索引数据只能是unsigned int，无需判断数据类型
		int indexBufferSize = accessor->count * sizeof(unsigned int);
		element->indices = malloc(indexBufferSize);
		memcpy(element->indices, cgltf_buffer_view_data(accessor->buffer_view), indexBufferSize);

		element->vertex_count = accessor->count;


		HERawBuffer *hebuffer = world_get_gltf_buffer(world, accessor->buffer_view->buffer->data, accessor->buffer_view->buffer->size);


		element->index_buf.count = accessor->count;
		element->index_buf.buffer = (unsigned char *)hebuffer->data + accessor->offset; // 直接加上起始偏移
		element->index_buf.buffer_size = hebuffer->data_size - accessor->offset;
		element->index_buf.stride = accessor->stride;


	}

	for (int i = 0; i < primitive->attributes_count; i++) {

		cgltf_attribute *attribute = &primitive->attributes[i];

		// 复制一下gltf的buffer数据
		HERawBuffer *hebuffer = world_get_gltf_buffer(world, attribute->data->buffer_view->buffer->data, attribute->data->buffer_view->buffer->size);

		HEBufferLayout *tHEBufferLayout = NULL;

		switch (attribute->type) {

			case cgltf_attribute_type_position:
				tHEBufferLayout = &element->position_buf;
				break;

			case cgltf_attribute_type_normal:
				break;

			case cgltf_attribute_type_texcoord:
				break;

			case cgltf_attribute_type_color:
				break;

			// todo: other attribute

			default:
				break;
		}

		if (tHEBufferLayout) {
			tHEBufferLayout->count = attribute->data->count;
			tHEBufferLayout->stride = attribute->data->stride;
			tHEBufferLayout->buffer = (unsigned char *)hebuffer->data + attribute->data->offset; // 直接加上起始偏移
			tHEBufferLayout->buffer_size = hebuffer->data_size - attribute->data->offset;
		}

		// primitivedata->material

	}

}

static void node_init_by_gltfdata(HENode *node, cgltf_node *nodedata) {

	// trs

	if (nodedata->has_translation) {

		glm_vec3_copy(nodedata->translation, node->transform.translation);

	}

	if (nodedata->has_rotation) {
		glm_quat_copy(nodedata->rotation, node->transform.rotation);
	}

	if (nodedata->has_scale) {
		glm_vec3_copy(nodedata->scale, node->transform.scale);
	}


	// mark: gltf node matrix is column-major, same as glm matrix
	// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-node

	cgltf_node_transform_local(nodedata, &node->transform.matrix[0][0]);


	// mesh

	if (nodedata->mesh && nodedata->mesh->primitives_count > 0) {

		node->mesh = mesh_create(nodedata->mesh->primitives_count);

		for (int i = 0; i < nodedata->mesh->primitives_count; i++) {

			cgltf_primitive *primitivedata = &nodedata->mesh->primitives[i];

			drawelement_init_by_gltfdata(&node->mesh->elements[i], primitivedata);

		}

	}

}


///

static HENode *create_node_by_gltfdata(cgltf_node *data) {

	HENode *gameNode = node_create();

	node_init_by_gltfdata(gameNode, data);

	for (int i = 0; i < data->children_count; i++) {

		HENode *child = create_node_by_gltfdata(data->children[i]);

		node_attach_tail(gameNode, child);

	}

	return gameNode;

}


static HEScene *create_scene_by_gltfdata(cgltf_data *data) {
	// mark: 先只支持一个场景

	HEScene *gameScene = scene_create();

	if (data->scene) {

		for (int i = 0; i < data->scene->nodes_count; i++) {

			cgltf_node *nodedata = data->scene->nodes[i];

			HENode *node = create_node_by_gltfdata(nodedata);

			node_attach_tail(gameScene->root, node);

		}


	}


	return gameScene;

}

///

static void load_gltf() {
	cgltf_options options = {0};
	cgltf_data* data = NULL;
	char *path = he_asset_gen_path("models/triangle.gltf");
	cgltf_result result = cgltf_parse_file(&options, path, &data);

	if (result == cgltf_result_success)
		result = cgltf_load_buffers(&options, data, path);

	if (result == cgltf_result_success)
		result = cgltf_validate(data);

	if (result == cgltf_result_success) {

		world->scene = create_scene_by_gltfdata(data);
	}

	free(path);
	cgltf_free(data);
}


///

static void draw_node(HENode *node) {

	HENode *child = node->child;

	while (child) {
		draw_node(child);

		child = child->next;
	}

	// todo: draw node

	// mesh -> primitives -> vertices,normals,texcoords

}

static void draw_world() {
	HENode *node = world->scene->root->child;

	while (node) {

		draw_node(node);

		node = node->next;

	}

}

///

static void play_12_fin() {
	printf("play 12 fin\n");
	world_destroy(world);
}

void play_12_update_gltf() {
	static int inited = 0;

	if (!inited) {
		inited = 1;

		init_world();

		hegl_registfin(play_12_fin);

		load_gltf();
	}

	draw_world();
}