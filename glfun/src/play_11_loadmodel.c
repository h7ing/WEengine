#include "play_11_loadmodel.h"
#include <stdio.h>
#include <ufbx.h>
#include <GL/glew.h>
#include <cglm/cglm.h>

#include <cgltf.h>
#include <cgltf_write.h>

#include "hengine.h"
#include "hegl_util.h"

// H version

typedef struct HTransform {
	vec3 position;
	vec3 rotation;
	vec3 scale;
	mat4 matrix;
} HTransform;

typedef struct HCommonMaterial {
	GLuint shader;
	// GLuint vao;
	// GLuint modelview;
	// GLuint normalmatrix;
	// GLuint viewmatrix;
	// vec3 color;
	// vec3 ambient;
	// GLuint diffuse;
	// GLuint specular;
	// float shininess;
} HCommonMaterial;

typedef struct HMesh {
	float *positions;
	GLuint vao;
	GLuint vbo;
	float *normals;
	unsigned int *indices;
	GLuint ebo;
	size_t vertexcount;
	int elementtype;
	int elementoffset;
	// normal
	// texcoord
} HMesh;


typedef struct HObject {
	HTransform transform;
	HCommonMaterial material;
	HMesh mesh;
} HObject;

typedef struct HObjectList {
	HObject *objectarray;
	size_t size;
	size_t count;
} HObjectList;

typedef struct HScene {
	HObjectList objects;
} HScene;


static HScene myscene = {0};


static void load_fbx() {
	ufbx_load_opts opts = {
		.load_external_files = true,
		.ignore_missing_external_files = true,
		.generate_missing_normals = true,

		.target_axes = {
			.right = UFBX_COORDINATE_AXIS_POSITIVE_X,
			.up = UFBX_COORDINATE_AXIS_POSITIVE_Y,
			.front = UFBX_COORDINATE_AXIS_POSITIVE_Z,
		},
		.target_unit_meters = 1.0f,
	};
	ufbx_error error;

	char *path = he_asset_gen_path("models/teapot.obj");

	ufbx_scene *scene = ufbx_load_file(path, &opts, &error);

	free(path);

	if (!scene) {
		printf("Failed to load fbx");
		exit(1);
	}

	ufbx_free_scene(scene);

	printf("load fbx finish\n");
}


static void load_gltf() {
	cgltf_options options = {0};
	cgltf_data* data = NULL;

	char *path = he_asset_gen_path("models/treepine.glb");

	cgltf_result result = cgltf_parse_file(&options, path, &data);

	if (result == cgltf_result_success)
		result = cgltf_load_buffers(&options, data, path);

	if (result == cgltf_result_success)
		result = cgltf_validate(data);

	printf("load gltf Result: %d\n", result);

	if (result == cgltf_result_success)
	{
		printf("Type: %u\n", data->file_type);
		printf("Meshes: %u\n", (unsigned)data->meshes_count);
		// printf("json:\n%s", data->json);
	}

	cgltf_free(data);

	free(path);

	printf("load gltf finish\n");
}


typedef struct GLTFDataList {
	struct GLTFDataList *next;
	cgltf_data *data;
} GLTFDataList;

static GLTFDataList gltfdatalist = {0};

GLTFDataList *create_gltfdata() {
	GLTFDataList *prev = &gltfdatalist;
	GLTFDataList *leaf = prev->next;
	while (leaf) {
		leaf = leaf->next;
		prev = leaf;
	}

	leaf = malloc(sizeof(GLTFDataList));

	leaf->next = NULL;
	leaf->data = NULL;

	prev->next = leaf;

	return leaf;
}

static void free_gltfdata() {

	GLTFDataList *leaf = gltfdatalist.next;
	GLTFDataList *next;

	while (leaf) {
		next = leaf->next;
		if (leaf->data) {
			cgltf_free(leaf->data);
		}
		free(leaf);
		leaf = next;
	}

	if (gltfdatalist.data) {
		cgltf_free(leaf->data);
	}
}

static void create_scene_by_gltf(cgltf_data *data) {

	myscene.objects.objectarray = calloc(data->nodes_count, sizeof(HObject));

	// todo: multi scenes
	// data->scene // the default scene
	// data->scene->nodes_count

	myscene.objects.size = data->nodes_count; // total nodes in all scenes (not very sure ?)
	myscene.objects.count = 0;

	for (int i = 0; i < data->nodes_count; i++) {
		HObject *obj = &myscene.objects.objectarray[i];

		obj->material.shader = hegl_create_shader_program("shaders/triangle.vert", "shaders/triangle.frag");

		glGenVertexArrays(1, &obj->mesh.vao);
		glBindVertexArray(obj->mesh.vao);

		cgltf_node *node = &data->nodes[i];

		for (int j = 0; j < node->mesh->primitives_count; j++) {
			cgltf_primitive *primitive = &node->mesh->primitives[j];

			// mark: indices (ebo) is done
			// 加载索引数据
			if (primitive->indices) {
				cgltf_accessor *idx_acc = primitive->indices;
				glGenBuffers(1, &obj->mesh.ebo);
				int target = GL_ELEMENT_ARRAY_BUFFER;
				if (idx_acc->buffer_view->type == cgltf_buffer_view_type_indices) {
					target = GL_ELEMENT_ARRAY_BUFFER;
				} else if (idx_acc->buffer_view->type == cgltf_buffer_view_type_vertices) {
					target = GL_ARRAY_BUFFER;
				}
				glBindBuffer(target, obj->mesh.ebo);

				glBufferData(target, idx_acc->buffer_view->size, cgltf_buffer_view_data(idx_acc->buffer_view), GL_STATIC_DRAW);

				obj->mesh.vertexcount = idx_acc->count;
				obj->mesh.elementtype = GL_FLOAT;//cgltf_int_from_component_type(idx_acc->component_type);
				obj->mesh.elementoffset = idx_acc->offset;
			}

			// mark: vertex data
			// todo: no need to check primitive type
			if (cgltf_primitive_type_triangles == primitive->type) {

				for (int k = 0; k < primitive->attributes_count; k++) {

					cgltf_attribute *attr = &primitive->attributes[k];

					if (attr->type == cgltf_attribute_type_position) {

						glGenBuffers(1, &obj->mesh.vbo);
						glBindBuffer(GL_ARRAY_BUFFER, obj->mesh.vbo);

						obj->mesh.vertexcount = attr->data->count;

						if (attr->data->is_sparse) {

						} else {
							cgltf_buffer_view *bufv = attr->data->buffer_view;

							int target;
							if (bufv->type == cgltf_buffer_view_type_vertices) {
								target = GL_ARRAY_BUFFER;
							} else if (bufv->type == cgltf_buffer_view_type_indices) {
								target = GL_ELEMENT_ARRAY_BUFFER;
							}

							glBufferData(target, bufv->size, cgltf_buffer_view_data(bufv), GL_STATIC_DRAW);

							cgltf_accessor *acc = attr->data;

							int theoffset = attr->data->offset;

							glVertexAttribPointer(0, 
								cgltf_num_components(acc->type), 
								GL_FLOAT, // cgltf_int_from_component_type(acc->component_type), 
								GL_FALSE, 
								bufv->stride, 
								(void *)(acc->offset));

							glEnableVertexAttribArray(0);
						}

					}

				}

			}
		}
	}

	myscene.objects.count = data->nodes_count;

}


static void init_primitive(cgltf_primitive *primitive) {

}


static void init_mesh(HObject *obj, cgltf_mesh *mesh) {

	// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#meshes

	// ```json
	// {
	//     "meshes": [
	//         {
	//             "primitives": [
	//                 {
	//                     "attributes": {
	//                         "NORMAL": 23,
	//                         "POSITION": 22,
	//                         "TANGENT": 24,
	//                         "TEXCOORD_0": 25
	//                     },
	//                     "indices": 21,
	//                     "material": 3,
	//                     "mode": 4
	//                 }
	//             ]
	//         }
	//     ]
	// }
	// ```

	// Q: 一个mesh对应几个obj？ A: 一个mesh对应一个obj：一个node只有一个mesh,一个node设计上对应一个object
	// Q: 一个primitive对应几个obj？ A: primitive应当对应一个draw_element，一个primitive有一个material


	for (int j = 0; j < mesh->primitives_count; j++) {

		cgltf_primitive *primitive = &mesh->primitives[j];


		// 加载索引数据
		// When indices property is not defined, the number of vertex indices to render is defined by count of attribute accessors (with the implied values from range [0..count));
		// when indices property is defined, the number of vertex indices to render is defined by count of accessor referred to by indices.

		if (primitive->indices) {
			cgltf_accessor *idx_acc = primitive->indices;
			glGenBuffers(1, &obj->mesh.ebo);
			int target = GL_ELEMENT_ARRAY_BUFFER;
			if (idx_acc->buffer_view->type == cgltf_buffer_view_type_indices) {
				target = GL_ELEMENT_ARRAY_BUFFER;
			} else if (idx_acc->buffer_view->type == cgltf_buffer_view_type_vertices) {
				target = GL_ARRAY_BUFFER;
			}
			glBindBuffer(target, obj->mesh.ebo);

			glBufferData(target, idx_acc->buffer_view->size, cgltf_buffer_view_data(idx_acc->buffer_view), GL_STATIC_DRAW);

			obj->mesh.vertexcount = idx_acc->count;
			obj->mesh.elementtype = GL_FLOAT;//cgltf_int_from_component_type(idx_acc->component_type);
			obj->mesh.elementoffset = idx_acc->offset;
		}

		// mark: vertex data
		// 对应 primitive 的 mode
		if (cgltf_primitive_type_triangles == primitive->type) {

			for (int k = 0; k < primitive->attributes_count; k++) {

				cgltf_attribute *attr = &primitive->attributes[k];

				if (attr->type == cgltf_attribute_type_position) {

					glGenBuffers(1, &obj->mesh.vbo);
					glBindBuffer(GL_ARRAY_BUFFER, obj->mesh.vbo);

					obj->mesh.vertexcount = attr->data->count;

					if (attr->data->is_sparse) {

					} else {
						cgltf_buffer_view *bufv = attr->data->buffer_view;

						int target;
						if (bufv->type == cgltf_buffer_view_type_vertices) {
							target = GL_ARRAY_BUFFER;
						} else if (bufv->type == cgltf_buffer_view_type_indices) {
							target = GL_ELEMENT_ARRAY_BUFFER;
						}

						glBufferData(target, bufv->size, cgltf_buffer_view_data(bufv), GL_STATIC_DRAW);

						cgltf_accessor *acc = attr->data;

						int theoffset = attr->data->offset;

						glVertexAttribPointer(0, 
							cgltf_num_components(acc->type), 
							GL_FLOAT,//cgltf_int_from_component_type(acc->component_type), 
							GL_FALSE, 
							bufv->stride, 
							(void *)(acc->offset));

						glEnableVertexAttribArray(0);
					}

				}

			}

		}
	}

}

static void init_node(HObject *obj, cgltf_node *src) {

	// https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#geometry

	// Any node MAY contain one mesh
	// 一个node只有一个mesh

	// todo: material 对应到 primitive
	obj->material.shader = hegl_create_shader_program("shaders/triangle.vert", "shaders/triangle.frag");

	glGenVertexArrays(1, &obj->mesh.vao);
	glBindVertexArray(obj->mesh.vao);

	cgltf_node *node = src;

	// todo: node有TRS数据

	if (node->mesh) {
		init_mesh(obj, node->mesh);
	}

}

// static void create_default_scene_by_gltf(cgltf_data *data) {

// 	cgltf_scene *scene = data->scene; // 默认场景

// 	myscene.objects.objectarray = calloc(scene->nodes_count, sizeof(HObject));

// 	myscene.objects.size = scene->nodes_count;
// 	myscene.objects.count = 0;

// 	for (int i = 0; i < scene->nodes_count; i++) {
// 		HObject *obj = &myscene.objects.objectarray[i];
// 		cgltf_node *node = &scene->nodes[i];

// 		init_node(obj, node);
// 	}

// 	myscene.objects.count = data->nodes_count;

// }


static void free_scene() {

	for (int i = 0; i < myscene.objects.count; i++) {
		HObject *object = &myscene.objects.objectarray[i];
		glDeleteVertexArrays(1, &object->mesh.vao);
		glDeleteBuffers(1, &object->mesh.vbo);
		glDeleteBuffers(1, &object->mesh.ebo);
		glDeleteShader(object->material.shader);
	}

	free(myscene.objects.objectarray);
}

static void load_gltf_simple_triangle() {
	cgltf_options options = {0};
	cgltf_data* data = NULL;
	char *path = he_asset_gen_path("models/triangle.gltf");
	cgltf_result result = cgltf_parse_file(&options, path, &data);

	if (result == cgltf_result_success)
		result = cgltf_load_buffers(&options, data, path);

	if (result == cgltf_result_success)
		result = cgltf_validate(data);

	if (result == cgltf_result_success) {

		GLTFDataList *adata = create_gltfdata();
		adata->data = data;

		create_scene_by_gltf(data);

		// cgltf_free(data);
	}

	free(path);
}

static void load_gltf_simple_sparse() {
	cgltf_options options = {0};
	cgltf_data* data = NULL;
	char *path = he_asset_gen_path("models/triangles.gltf");
	cgltf_result result = cgltf_parse_file(&options, path, &data);

	if (result == cgltf_result_success)
		result = cgltf_load_buffers(&options, data, path);

	if (result == cgltf_result_success)
		result = cgltf_validate(data);

	if (result == cgltf_result_success) {

		GLTFDataList *adata = create_gltfdata();
		adata->data = data;

		create_scene_by_gltf(data);

		// cgltf_free(data); // 最后统一释放
	}

	free(path);
}


static void draw_object(HObject *object) {
	glUseProgram(object->material.shader);
	glBindVertexArray(object->mesh.vao);

	if (object->mesh.ebo) {
		glDrawElements(GL_TRIANGLES, object->mesh.vertexcount, object->mesh.elementtype, (void *)object->mesh.elementoffset);
	}
	else {
		glDrawArrays(GL_TRIANGLES, 0, object->mesh.vertexcount);
		// glDrawElements(GL_TRIANGLES, object->mesh.vertexcount, GL_FLOAT, NULL);
	}
}


static void draw_gltf() {

	for (int i = 0; i < myscene.objects.count; i++) {
		draw_object(&myscene.objects.objectarray[i]);
	}
}


static void play_11_fin() {
	free_gltfdata();
	free_scene();
	printf("play 11 fin\n");
}


void play_11_loadmodel() {
	static int inited = 0;

	if (!inited) {
		inited = 1;
		hegl_registfin(play_11_fin);
		load_gltf_simple_triangle();
		// load_gltf_simple_sparse();
	}

	draw_gltf();
}