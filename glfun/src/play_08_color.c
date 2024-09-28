#include "play_08_color.h"
#include <math.h>
#include <cglm/cglm.h>
#include <GL/glew.h>
#include "hengine.h"
#include "hegl_util.h"


typedef struct Transform {
	vec3 position;
	vec3 direction;
	vec3 rotation;
	vec3 scale;
	mat4 mat;
	mat4 matinv; // inverse
	mat3 norm;
} Transform;


typedef struct Material {
	GLuint shader;
	GLuint vao;
	GLuint vbo;
	vec3 basecolor;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
} Material;


typedef struct Camera {
	Transform transform;
	float fovy; // in rad
	mat4 projection;
	mat4 view;
	mat4 pv;
	mat4 mvp;
	mat4 modelview;
} Camera;


typedef struct Light {
	Transform transform;
	Material material;
	vec3 speed;
} Light;


typedef struct RenderObject {
	Transform transform;
	Material material;
} RenderObject;


// typedef struct RenderObjectListHead {
// 	struct RenderObjectListHead *next;
// 	RenderObject *object;
// } RenderObjectListHead;


typedef struct Scene {
	Camera camera;
	Light light;

	// RenderObjectListHead objects;
	RenderObject object;

	int isbasic;
} Scene;


static float vertices_cube[] = {
    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,

    -0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,

    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,

     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,

    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f, -0.5f,

    -0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f
};

static float vertices_obj[] = {
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
};



static Scene scene08;


static void update_transform_matrix(Transform *transform) {
	// 计算变换矩阵：
	// 先缩放，后旋转，再平移 : T*R*S*p
	// 所以实际计算的顺序是：平移-旋转-缩放

	glm_translate_to(GLM_MAT4_IDENTITY, transform->position, transform->mat);

	// // 数学上的顺序为先绕x轴旋转后y轴最后z轴：Rz*Ry*Rx*p
	// glm_rotate_z(transform->mat, transform->rotation[2]*M_PI/180.0f, transform->mat);
	// glm_rotate_y(transform->mat, transform->rotation[1]*M_PI/180.0f, transform->mat);
	// glm_rotate_x(transform->mat, transform->rotation[0]*M_PI/180.0f, transform->mat);

	// 数学上的顺序：Ry*Rx*Rz*p
	glm_rotate_y(transform->mat, transform->rotation[1]*M_PI/180.0f, transform->mat);
	glm_rotate_x(transform->mat, transform->rotation[0]*M_PI/180.0f, transform->mat);
	glm_rotate_z(transform->mat, transform->rotation[2]*M_PI/180.0f, transform->mat);

	glm_scale(transform->mat, transform->scale);

	glm_mat4_inv(transform->mat, transform->matinv);
	glm_mat4_pick3t(transform->matinv, transform->norm);
}

static void init_transform(Transform *transform, vec3 position, vec3 scale, vec3 rotate) {
	glm_vec3_copy(rotate, transform->rotation);
	// glm_rotate_z(transform->mat, rotate[2]/180.0f*M_PI, transform->mat);
	// glm_rotate_y(transform->mat, rotate[1]/180.0f*M_PI, transform->mat);
	// glm_rotate_x(transform->mat, rotate[0]/180.0f*M_PI, transform->mat);

	glm_vec3_copy(position, transform->position);
	// glm_translate_to(GLM_MAT4_IDENTITY, position, transform->mat);

	glm_vec3_copy(scale, transform->scale);
	// glm_scale(transform->mat, scale);

	update_transform_matrix(transform);
}

static void init_material(Material *material, vec3 color, vec3 ambient, vec3 diffuse, vec3 specular, float shininess) {
	glm_vec3_copy(color, material->basecolor);
	glm_vec3_copy(ambient, material->ambient);
	glm_vec3_copy(diffuse, material->diffuse);
	glm_vec3_copy(specular, material->specular);
	material->shininess = shininess;
}


static void init_scene(Scene *scene) {

	// init camera
	do {
		glm_vec3_copy((vec3){1.5f,1.5f,5.0f}, scene->camera.transform.position);
		glm_vec3_negate_to(scene->camera.transform.position, scene->camera.transform.direction);
		glm_normalize(scene->camera.transform.direction);
		// glm_vec3_copy((vec3){0,0,-1.0f}, scene->camera.transform.direction);
		scene->camera.fovy = M_PI_4;
		glm_perspective(scene->camera.fovy, he_get_window_aspect(), 0.1f, 100.0f, scene->camera.projection);
	} while (0);
	

	// init light
	do {
		init_transform(&scene->light.transform, (vec3){1.2f, 1.0f, 2.0f}, (vec3){0.2f, 0.2f, 0.2f}, GLM_VEC3_ZERO);
		init_material(&scene->light.material, (vec3){1.0f, 1.0f, 1.0f}, (vec3){1.0f,1.0f,1.0f}, (vec3){1.0f,1.0f,1.0f}, (vec3){1.0f,1.0f,1.0f}, 0);

		glm_vec3_copy((vec3){0.25f,0.25f,0.25f}, scene->light.speed);

		scene->light.material.shader = hegl_create_shader_program("shaders/s08_light.vert", "shaders/s08_light.frag");

		glGenVertexArrays(1, &scene->light.material.vao);
		glBindVertexArray(scene->light.material.vao);

		glGenBuffers(1, &scene->light.material.vbo);
		glBindBuffer(GL_ARRAY_BUFFER, scene->light.material.vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_cube), vertices_cube, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(0);
	} while (0);


	// init objects
	do {
		RenderObject *object = &scene->object;
		init_transform(&object->transform, GLM_VEC3_ZERO, (vec3){1.0f,1.0f,1.0f}, (vec3){0, 0, 0});

		//obsidian	0.05375	0.05	0.06625	0.18275	0.17	0.22525	0.332741	0.328634	0.346435	0.3
		// obsidian 90,55,177 -> 
		init_material(&object->material, (vec3){0.35f, 0.214f, 0.69f}, (vec3){0.05375f,0.05f,0.06625f}, (vec3){0.18275f,0.18275f,0.17f}, (vec3){0.22525f,0.332741f,0.328634f}, 0.3f*128.0f);

		if (scene->isbasic)
			object->material.shader = hegl_create_shader_program("shaders/s08_object.vert", "shaders/s08_object.frag");
		else
			object->material.shader = hegl_create_shader_program("shaders/s08_object_material.vert", "shaders/s08_object_material.frag");

		glGenVertexArrays(1, &object->material.vao);
		glBindVertexArray(object->material.vao);

		glGenBuffers(1, &scene->light.material.vbo);
		glBindBuffer(GL_ARRAY_BUFFER, scene->light.material.vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_obj), vertices_obj, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

	} while (0);
}


static void draw_scene(Scene *scene) {
	do {
		glm_look(scene->camera.transform.position, scene->camera.transform.direction, GLM_YUP, scene->camera.view);
		glm_mat4_mul(scene->camera.projection, scene->camera.view, scene->camera.pv);
	} while(0);


	// draw light
	do {
		glm_mat4_mul(scene->camera.pv, scene->light.transform.mat, scene->camera.mvp);
		glUseProgram(scene->light.material.shader);
		glBindVertexArray(scene->light.material.vao);
		glUniformMatrix4fv(glGetUniformLocation(scene->light.material.shader, "mvp"), 1, GL_FALSE, &scene->camera.mvp[0][0]);
		glUniform3fv(glGetUniformLocation(scene->light.material.shader, "lightColor"), 1, scene->light.material.basecolor);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	} while (0);


	// draw objects
	do {
		glm_mat4_mul(scene->camera.pv, scene->object.transform.mat, scene->camera.mvp);
		glUseProgram(scene->object.material.shader);
		glBindVertexArray(scene->object.material.vao);
		glUniformMatrix4fv(glGetUniformLocation(scene->object.material.shader, "model"), 1, GL_FALSE, &scene->object.transform.mat[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(scene->object.material.shader, "view"), 1, GL_FALSE, &scene->camera.view[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(scene->object.material.shader, "mvp"), 1, GL_FALSE, &scene->camera.mvp[0][0]);
		glUniformMatrix3fv(glGetUniformLocation(scene->object.material.shader, "norm"), 1, GL_FALSE, &scene->object.transform.norm[0][0]);
		glUniform3f(glGetUniformLocation(scene->object.material.shader, "lightPos"), scene->light.transform.position[0], scene->light.transform.position[1], scene->light.transform.position[2]);
		glUniform3f(glGetUniformLocation(scene->object.material.shader, "camPos"), scene->camera.transform.position[0], scene->camera.transform.position[1], scene->camera.transform.position[2]);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	} while (0);
}

static void draw_scene_use_material(Scene *scene) {
	// camera
	do {
		glm_look(scene->camera.transform.position, scene->camera.transform.direction, GLM_YUP, scene->camera.view);
		glm_mat4_mul(scene->camera.projection, scene->camera.view, scene->camera.pv);
	} while(0);

	// draw light
	do {
		glm_mat4_mul(scene->camera.pv, scene->light.transform.mat, scene->camera.mvp);
		glUseProgram(scene->light.material.shader);
		glBindVertexArray(scene->light.material.vao);
		glUniformMatrix4fv(glGetUniformLocation(scene->light.material.shader, "mvp"), 1, GL_FALSE, &scene->camera.mvp[0][0]);
		glUniform3fv(glGetUniformLocation(scene->light.material.shader, "lightColor"), 1, scene->light.material.basecolor);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	} while (0);

	// draw objects
	do {
		glm_mat4_mul(scene->camera.pv, scene->object.transform.mat, scene->camera.mvp);
		glm_mat4_mul(scene->camera.view, scene->object.transform.mat, scene->camera.modelview);
		glUseProgram(scene->object.material.shader);
		glBindVertexArray(scene->object.material.vao);
		glUniformMatrix4fv(glGetUniformLocation(scene->object.material.shader, "model"), 1, GL_FALSE, &scene->object.transform.mat[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(scene->object.material.shader, "view"), 1, GL_FALSE, &scene->camera.view[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(scene->object.material.shader, "mvp"), 1, GL_FALSE, &scene->camera.mvp[0][0]);
		glUniformMatrix3fv(glGetUniformLocation(scene->object.material.shader, "norm"), 1, GL_FALSE, &scene->object.transform.norm[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(scene->object.material.shader, "modelview"), 1, GL_FALSE, &scene->camera.modelview[0][0]);

		glUniform3fv(glGetUniformLocation(scene->object.material.shader, "light.color"), 1, scene->light.material.basecolor);
		glUniform3fv(glGetUniformLocation(scene->object.material.shader, "light.position"), 1, scene->light.transform.position);
		glUniform3fv(glGetUniformLocation(scene->object.material.shader, "light.ambient"), 1, scene->light.material.ambient);
		glUniform3fv(glGetUniformLocation(scene->object.material.shader, "light.diffuse"), 1, scene->light.material.diffuse);
		glUniform3fv(glGetUniformLocation(scene->object.material.shader, "light.specular"), 1, scene->light.material.specular);

		glUniform3fv(glGetUniformLocation(scene->object.material.shader, "material.color"), 1, scene->object.material.basecolor);
		glUniform3fv(glGetUniformLocation(scene->object.material.shader, "material.ambient"), 1, scene->object.material.ambient);
		glUniform3fv(glGetUniformLocation(scene->object.material.shader, "material.diffuse"), 1, scene->object.material.diffuse);
		glUniform3fv(glGetUniformLocation(scene->object.material.shader, "material.specular"), 1, scene->object.material.specular);
		glUniform1f(glGetUniformLocation(scene->object.material.shader, "material.shininess"), scene->object.material.shininess);

		glDrawArrays(GL_TRIANGLES, 0, 36);
	} while (0);
}

static void update_transform_direction(Transform *transform) {
	transform->direction[0] = cosf(transform->rotation[0]) * cosf(transform->rotation[1]);
	transform->direction[1] = sinf(transform->rotation[0]);
	transform->direction[2] = cosf(transform->rotation[0]) * sinf(transform->rotation[1]);
	glm_normalize(transform->direction);
}


void play_08_color(float op_rotate_x, float op_rotate_y, int op_translate_x, int op_translate_y) {
	static int inited = 0;
	if (!inited) {
		inited = 1;

		scene08.isbasic = 1;
		init_scene(&scene08);
	}

	Scene *scene = &scene08;

	// if (fabsf(op_rotate_x) > FLT_EPSILON || fabsf(op_rotate_y) > FLT_EPSILON) {
	// 	scene->camera.transform.rotation[0] -= op_rotate_y * M_PI / 180.0f; // pitch
	// 	scene->camera.transform.rotation[1] += op_rotate_x * M_PI / 180.0f; // yaw
	// 	update_transform_direction(&scene->camera.transform);
	// }

	if (fabsf(op_rotate_x) > FLT_EPSILON || fabsf(op_rotate_y) > FLT_EPSILON) {
		scene->object.transform.rotation[1] += op_rotate_x;
		scene->object.transform.rotation[0] += op_rotate_y;

		// scene->object.transform.scale[0] += op_rotate_x;
		// scene->object.transform.scale[1] += op_rotate_y;

		update_transform_matrix(&scene->object.transform);
	}

	if (op_translate_x || op_translate_y) {
		glm_vec3_muladd(scene->light.speed, (vec3){op_translate_x, op_translate_y, 0} ,scene->light.transform.position);

		update_transform_matrix(&scene->light.transform);
	}

	draw_scene(&scene08);
}

static Scene scene09;

void play_08_material(int op_translate_x, int op_translate_y) {
	static int inited = 0;
	if (!inited) {
		inited = 1;

		scene09.isbasic = 0;
		init_scene(&scene09);
	}

	Scene *scene = &scene09;

	if (op_translate_x || op_translate_y) {
		glm_vec3_muladd(scene->light.speed, (vec3){op_translate_x, op_translate_y, 0} ,scene->light.transform.position);

		update_transform_matrix(&scene->light.transform);
	}

	draw_scene_use_material(scene);
}
