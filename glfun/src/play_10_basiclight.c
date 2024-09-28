#include "play_10_basiclight.h"
#include <math.h>
#include <GL/glew.h>
#include <cglm/cglm.h>
#include "hengine.h"
#include "hegl_util.h"

typedef struct Transform {
	vec3 position;
	vec3 rotation;
	vec3 scale;
	mat4 modelmatrix;
} Transform;

typedef struct Camera {
	Transform transform;
	vec3 direction;
	vec3 right;
	vec3 up;
	float fovy; // rad
	mat4 viewmatrix;
	mat4 projmatrix;
	mat4 matrix;
	// mat4 modelviewmatrix;
	// mat4 normalmatrix;
} Camera;

typedef struct LightMaterial {
	vec3 color;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	GLuint shader;
	GLuint vao;
	GLuint vbo;
	GLuint mvp;
} LightMaterial;

typedef struct DirectionalLight {
	vec3 direction;
	Transform transform;
	LightMaterial material;
} DirectionalLight;

typedef struct PointLight {
	Transform transform;
	LightMaterial material;
	float kc;
	float kl;
	float kq;
} PointLight;

typedef struct SpotLight {
	Transform transform;
	vec3 direction;
	float outer;
	float inner;
	float kc;
	float kl;
	float kq;
	LightMaterial material;
} SpotLight;

typedef struct ObjectMaterial {
	GLuint shader;
	GLuint vao;
	GLuint modelview;
	GLuint normalmatrix;
	GLuint viewmatrix;
	vec3 color;
	vec3 ambient;
	GLuint diffuse;
	GLuint specular;
	float shininess;
} ObjectMaterial;

typedef struct Object {
	Transform transform;
	ObjectMaterial material;
} Object;

typedef struct ObjectLeaf {
	Object *object;
	struct ObjectLeaf *next;
} ObjectLeaf;

typedef struct Scene {
	Camera camera;
	DirectionalLight directional;
	PointLight pointlight;
	SpotLight spotlight;

	Object object;
	ObjectLeaf *objects;

} Scene;


// mesh

static float vertices_light[] = {
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

static float vertices[] = {
	// positions          // normals           // texture coords
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
};

static Scene scene10;


static void update_view_matrix(Camera* camera) {
	glm_look(camera->transform.position, camera->direction, GLM_YUP, camera->viewmatrix);
}

static void update_proj_matrix(Camera* camera) {
	glm_perspective(camera->fovy, he_get_window_aspect(), 0.1f, 100.0f, camera->projmatrix);
}

static void update_model_matrix(Transform *transform) {
	glm_mat4_identity(transform->modelmatrix);

	glm_translate(transform->modelmatrix, transform->position);

	glm_rotate_y(transform->modelmatrix, transform->rotation[1], transform->modelmatrix);
	glm_rotate_x(transform->modelmatrix, transform->rotation[0], transform->modelmatrix);
	glm_rotate_z(transform->modelmatrix, transform->rotation[2], transform->modelmatrix);

	glm_scale(transform->modelmatrix, transform->scale);
}

static Object *create_object(Scene *scene) {
	Object *object = malloc(sizeof(Object));

	object->transform.position[0] = (float)(rand()%10) - 5.0f;
	object->transform.position[1] = (float)(rand()%10) - 5.0f;
	object->transform.position[2] = (float)(rand()%10) - 5.0f;
	// object->transform.position[2] = 0;
	glm_vec3_divs(object->transform.position, 2.0f, object->transform.position);
	object->transform.rotation[0] = (float)(rand()%360)/180.0f * M_PI;
	object->transform.rotation[1] = (float)(rand()%360)/180.0f * M_PI;
	object->transform.rotation[2] = (float)(rand()%360)/180.0f * M_PI;

	glm_vec3_copy((vec3){1.0f,1.0f,1.0f}, object->transform.scale);

	glGenVertexArrays(1, &object->material.vao);
	glBindVertexArray(object->material.vao);

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);


	glm_vec3_copy((vec3){1.0f,1.0f,1.0f}, object->material.color);
	glm_vec3_copy((vec3){0.2f,0.2f,0.2f}, object->material.ambient);
	object->material.diffuse = hegl_create_texture("textures/container2.png");
	object->material.specular = hegl_create_texture("textures/container2_specular.png");
	object->material.shininess = 128.0f * 2.0f;

	GLuint shader = hegl_create_shader_program("shaders/s10_object.vert", "shaders/s10_object_lit0.frag");
	object->material.shader = shader;

	glUseProgram(object->material.shader);

	int tex = 0;
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, object->material.diffuse);
	glUniform1i(glGetUniformLocation(object->material.shader, "material.diffuse"), tex);

	tex++;
	glActiveTexture(GL_TEXTURE0 + tex);
	glBindTexture(GL_TEXTURE_2D, object->material.specular);
	glUniform1i(glGetUniformLocation(object->material.shader, "material.specular"), tex);

	glUniform3fv(glGetUniformLocation(object->material.shader, "material.color"), 1, object->material.color);

	glUniform3fv(glGetUniformLocation(object->material.shader, "material.ambient"), 1, object->material.ambient);

	glUniform1f(glGetUniformLocation(object->material.shader, "material.shininess"), object->material.shininess);

	glUniformMatrix4fv(glGetUniformLocation(object->material.shader, "projmatrix"), 1, GL_FALSE, &scene->camera.projmatrix[0][0]);

	object->material.modelview = glGetUniformLocation(object->material.shader, "modelview");
	object->material.normalmatrix = glGetUniformLocation(object->material.shader, "normalmatrix");
	object->material.viewmatrix = glGetUniformLocation(object->material.shader, "viewmatrix");

	glUniform3fv(glGetUniformLocation(object->material.shader, "light0.direction"), 1, scene->directional.direction);

	glUniform3fv(glGetUniformLocation(shader, "light0.ambient"), 1, scene->directional.material.ambient);
	glUniform3fv(glGetUniformLocation(shader, "light0.diffuse"), 1, scene->directional.material.diffuse);
	glUniform3fv(glGetUniformLocation(shader, "light0.specular"), 1, scene->directional.material.specular);

	// default point light
	glUniform3fv(glGetUniformLocation(shader, "light1.position"), 1, scene->pointlight.transform.position);
	glUniform3fv(glGetUniformLocation(shader, "light1.ambient"), 1, scene->pointlight.material.ambient);
	glUniform3fv(glGetUniformLocation(shader, "light1.diffuse"), 1, scene->pointlight.material.diffuse);
	glUniform3fv(glGetUniformLocation(shader, "light1.specular"), 1, scene->pointlight.material.specular);
	glUniform1f(glGetUniformLocation(shader, "light1.kc"), scene->pointlight.kc);
	glUniform1f(glGetUniformLocation(shader, "light1.kl"), scene->pointlight.kl);
	glUniform1f(glGetUniformLocation(shader, "light1.kq"), scene->pointlight.kq);

	// spotlight
	glUniform3fv(glGetUniformLocation(shader, "light2.position"), 1, scene->spotlight.transform.position);
	glUniform3fv(glGetUniformLocation(shader, "light2.direction"), 1, scene->spotlight.direction);
	glUniform3fv(glGetUniformLocation(shader, "light2.ambient"), 1, scene->spotlight.material.ambient);
	glUniform3fv(glGetUniformLocation(shader, "light2.diffuse"), 1, scene->spotlight.material.diffuse);
	glUniform3fv(glGetUniformLocation(shader, "light2.specular"), 1, scene->spotlight.material.specular);
	glUniform1f(glGetUniformLocation(shader, "light2.kc"), scene->spotlight.kc);
	glUniform1f(glGetUniformLocation(shader, "light2.kl"), scene->spotlight.kl);
	glUniform1f(glGetUniformLocation(shader, "light2.kq"), scene->spotlight.kq);
	glUniform1f(glGetUniformLocation(shader, "light2.inner"), scene->spotlight.inner);
	glUniform1f(glGetUniformLocation(shader, "light2.outer"), scene->spotlight.outer);

	return object;
}

static void init_scene(Scene* scene) {

	// camera
	do {
		Camera *camera = &scene->camera;
		glm_vec3_copy((vec3) { 1.0f, 1.0f, 5.0f }, camera->transform.position);
		glm_vec3_copy((vec3){0,-M_PI_2,0}, camera->transform.rotation);
		// glm_vec3_negate_to(scene->camera.transform.position, scene->camera.direction);
		camera->direction[0] = cosf(camera->transform.rotation[0]) * cosf(camera->transform.rotation[1]);
		camera->direction[1] = sinf(camera->transform.rotation[0]);
		camera->direction[2] = cosf(camera->transform.rotation[0]) * sinf(camera->transform.rotation[1]);
		glm_normalize(camera->direction);
		glm_cross(GLM_YUP, camera->direction, camera->right);
		glm_normalize(camera->right);
		glm_cross(camera->right, camera->direction, camera->up);
		scene->camera.fovy = M_PI_4;

		update_view_matrix(&scene->camera);
		update_proj_matrix(&scene->camera);
	} while (0);


	// directional light
	do {
		DirectionalLight *directional = &scene->directional;
		glm_vec3_copy((vec3){0.2f, 0.0f, 2.0f}, directional->transform.position);
		glm_vec3_copy((vec3){0.2f, 0.2f, 0.2f}, directional->transform.scale);
		glm_vec3_zero(directional->transform.rotation);

		glm_vec3_negate_to(directional->transform.position, directional->direction);
		glm_normalize(directional->direction);

		glGenVertexArrays(1, &directional->material.vao);
		glBindVertexArray(directional->material.vao);

		GLuint vbo;
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_light), vertices_light, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);


		vec3 color = {1.0f, 0, 0};
		glm_vec3_copy(color, directional->material.color);
		glm_vec3_copy(color, directional->material.specular);
		glm_vec3_copy((vec3){1.0f,1.0f,1.0f}, directional->material.ambient);
		glm_vec3_copy((vec3){1.0f,1.0f,1.0f}, directional->material.diffuse);
		// glm_vec3_copy((vec3){1.0f,1.0f,1.0f}, directional->material.specular);
		directional->material.shader = hegl_create_shader_program("shaders/s08_light.vert", "shaders/s08_light.frag");
		glUseProgram(directional->material.shader);
		glUniform3fv(glGetUniformLocation(directional->material.shader, "lightColor"), 1, directional->material.color);
		directional->material.mvp = glGetUniformLocation(directional->material.shader, "mvp");

	} while(0);


	// point light
	do
	{
		PointLight *light = &scene->pointlight;
		Transform *trans = &light->transform;
		glm_vec3_copy((vec3){1.2f, 1.0f, 2.0f}, trans->position);
		glm_vec3_copy((vec3){0.2f, 0.2f, 0.2f}, trans->scale);
		glm_vec3_zero(trans->rotation);


		LightMaterial *material = &light->material;

		glm_vec3_copy((vec3){0,1.0f,0}, material->color);


		glm_vec3_copy((vec3){0,0,0}, material->ambient);
		glm_vec3_copy((vec3){1.0f,1.0f,1.0f}, material->diffuse);
		glm_vec3_copy((vec3){1.0f,1.0f,1.0f}, material->specular);

		light->kc = 1.0f;
		light->kl = 0.09f;
		light->kq = 0.032f;

		material->shader = hegl_create_shader_program("shaders/s08_light.vert", "shaders/s08_light.frag");
		glUseProgram(material->shader);
		glUniform3fv(glGetUniformLocation(material->shader, "lightColor"), 1, material->color);
		material->mvp = glGetUniformLocation(material->shader, "mvp");

		glGenVertexArrays(1, &material->vao);
		glBindVertexArray(material->vao);

		glGenBuffers(1, &material->vbo);
		glBindBuffer(GL_ARRAY_BUFFER, material->vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_light), vertices_light, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(0);
	} while (0);


	// spot light
	do
	{
		Camera *camera = &scene->camera;
		SpotLight *light = &scene->spotlight;
		LightMaterial *material = &light->material;

		// child to camera
		glm_vec3_copy(camera->transform.position, light->transform.position);
		glm_vec3_copy(camera->direction, light->direction);

		glm_vec3_copy((vec3){0,0,0}, material->ambient);
		glm_vec3_copy((vec3){1.0f,1.0f,1.0f}, material->diffuse);
		glm_vec3_copy((vec3){1.0f,1.0f,1.0f}, material->specular);

		light->outer = cosf(15.0f * M_PI / 180.0f);
		light->inner = cosf(10.0f * M_PI / 180.0f);

		light->kc = 1.0f;
		light->kl = 0.09f;
		light->kq = 0.032f;

	} while (0);
	


	// object
	do
	{
		Object *object = &scene->object;

		glm_vec3_copy((vec3){0,0,0}, object->transform.position);
		glm_vec3_copy((vec3){0,0,0}, object->transform.rotation);
		glm_vec3_copy((vec3){1.0f,1.0f,1.0f}, object->transform.scale);

		glGenVertexArrays(1, &object->material.vao);
		glBindVertexArray(object->material.vao);

		GLuint vbo;
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);


		glm_vec3_copy((vec3){1.0f,1.0f,1.0f}, object->material.color);
		glm_vec3_copy((vec3){0.2f,0.2f,0.2f}, object->material.ambient);
		object->material.diffuse = hegl_create_texture("textures/container2.png");
		object->material.specular = hegl_create_texture("textures/container2_specular.png");
		object->material.shininess = 128.0f * 2.0f;

		GLuint shader = hegl_create_shader_program("shaders/s10_object.vert", "shaders/s10_object_lit0.frag");
		object->material.shader = shader;

		glUseProgram(object->material.shader);

		int tex = 0;
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, object->material.diffuse);
		glUniform1i(glGetUniformLocation(object->material.shader, "material.diffuse"), tex);

		tex++;
		glActiveTexture(GL_TEXTURE0 + tex);
		glBindTexture(GL_TEXTURE_2D, object->material.specular);
		glUniform1i(glGetUniformLocation(object->material.shader, "material.specular"), tex);

		glUniform3fv(glGetUniformLocation(object->material.shader, "material.color"), 1, object->material.color);

		glUniform3fv(glGetUniformLocation(object->material.shader, "material.ambient"), 1, object->material.ambient);

		glUniform1f(glGetUniformLocation(object->material.shader, "material.shininess"), object->material.shininess);

		glUniformMatrix4fv(glGetUniformLocation(object->material.shader, "projmatrix"), 1, GL_FALSE, &scene->camera.projmatrix[0][0]);

		object->material.modelview = glGetUniformLocation(object->material.shader, "modelview");
		object->material.normalmatrix = glGetUniformLocation(object->material.shader, "normalmatrix");
		object->material.viewmatrix = glGetUniformLocation(object->material.shader, "viewmatrix");

		glUniform3fv(glGetUniformLocation(object->material.shader, "light0.direction"), 1, scene->directional.direction);

		glUniform3fv(glGetUniformLocation(shader, "light0.ambient"), 1, scene->directional.material.ambient);
		glUniform3fv(glGetUniformLocation(shader, "light0.diffuse"), 1, scene->directional.material.diffuse);
		glUniform3fv(glGetUniformLocation(shader, "light0.specular"), 1, scene->directional.material.specular);

		// default point light
		glUniform3fv(glGetUniformLocation(shader, "light1.position"), 1, scene->pointlight.transform.position);
		glUniform3fv(glGetUniformLocation(shader, "light1.ambient"), 1, scene->pointlight.material.ambient);
		glUniform3fv(glGetUniformLocation(shader, "light1.diffuse"), 1, scene->pointlight.material.diffuse);
		glUniform3fv(glGetUniformLocation(shader, "light1.specular"), 1, scene->pointlight.material.specular);
		glUniform1f(glGetUniformLocation(shader, "light1.kc"), scene->pointlight.kc);
		glUniform1f(glGetUniformLocation(shader, "light1.kl"), scene->pointlight.kl);
		glUniform1f(glGetUniformLocation(shader, "light1.kq"), scene->pointlight.kq);

		// spotlight
		glUniform3fv(glGetUniformLocation(shader, "light2.position"), 1, scene->spotlight.transform.position);
		glUniform3fv(glGetUniformLocation(shader, "light2.direction"), 1, scene->spotlight.direction);
		glUniform3fv(glGetUniformLocation(shader, "light2.ambient"), 1, scene->spotlight.material.ambient);
		glUniform3fv(glGetUniformLocation(shader, "light2.diffuse"), 1, scene->spotlight.material.diffuse);
		glUniform3fv(glGetUniformLocation(shader, "light2.specular"), 1, scene->spotlight.material.specular);
		glUniform1f(glGetUniformLocation(shader, "light2.kc"), scene->spotlight.kc);
		glUniform1f(glGetUniformLocation(shader, "light2.kl"), scene->spotlight.kl);
		glUniform1f(glGetUniformLocation(shader, "light2.kq"), scene->spotlight.kq);
		glUniform1f(glGetUniformLocation(shader, "light2.inner"), scene->spotlight.inner);
		glUniform1f(glGetUniformLocation(shader, "light2.outer"), scene->spotlight.outer);

	} while (0);


	// objects
	ObjectLeaf *lastleaf = malloc(sizeof(ObjectLeaf));
	lastleaf->next = NULL;
	scene->objects = lastleaf;

	for (int i = 0; i < 5; i++) {
		Object *obj = create_object(scene);
		ObjectLeaf *leaf;
		if (i == 0) {
			leaf = lastleaf;
		} else {
			leaf = malloc(sizeof(ObjectLeaf));
			leaf->next = NULL;
			lastleaf->next = leaf;
			lastleaf = leaf;
		}

		leaf->object = obj;
	}
}

static void draw_scene_directional(Scene *scene) {
	Camera *cam = &scene->camera;
	DirectionalLight *directional = &scene->directional;

	do {
		update_model_matrix(&directional->transform);
		glm_mat4_mul(cam->viewmatrix, directional->transform.modelmatrix, cam->matrix);
		glm_mat4_mul(cam->projmatrix, cam->matrix, cam->matrix);

		glBindVertexArray(directional->material.vao);
		glUseProgram(directional->material.shader);
		glUniformMatrix4fv(directional->material.mvp, 1, GL_FALSE, &cam->matrix[0][0]);

		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
	} while(0);

	do {

		Object *obj = &scene->object;

		update_model_matrix(&obj->transform);

		glm_mat4_mul(cam->viewmatrix, obj->transform.modelmatrix, cam->matrix);

		glBindVertexArray(obj->material.vao);
		GLuint shader = obj->material.shader;
		glUseProgram(shader);
		glUniformMatrix4fv(obj->material.modelview, 1, GL_FALSE, &scene->camera.matrix[0][0]);
		glUniformMatrix4fv(obj->material.viewmatrix, 1, GL_FALSE, &scene->camera.viewmatrix[0][0]);

		// normal matrix = M^(-T)
		// glm_mat4_inv(scene->camera.matrix, scene->camera.matrix);
		// glm_mat4_transpose(scene->camera.matrix);
		// glUniformMatrix4fv(obj->material.normalmatrix, 1, GL_FALSE, &scene->camera.matrix[0][0]);

		// or
		// not sure
		glm_mat4_inv(scene->camera.matrix, scene->camera.matrix);
		glUniformMatrix4fv(obj->material.normalmatrix, 1, GL_TRUE, &scene->camera.matrix[0][0]);

		glUniform3fv(glGetUniformLocation(shader, "light0.direction"), 1, scene->directional.direction);
		glUniform1f(glGetUniformLocation(shader, "light0.enable"), 1.0f);

		glUniform1f(glGetUniformLocation(shader, "light1.enable"), 0);
		glUniform1f(glGetUniformLocation(shader, "light2.enable"), 0);

		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
	} while(0);
}

static void draw_scene_point(Scene *scene) {
	Camera *cam = &scene->camera;
	PointLight *light = &scene->pointlight;

	do {
		update_model_matrix(&light->transform);
		glm_mat4_mul(cam->viewmatrix, light->transform.modelmatrix, cam->matrix);
		glm_mat4_mul(cam->projmatrix, cam->matrix, cam->matrix);

		glBindVertexArray(light->material.vao);
		glUseProgram(light->material.shader);
		glUniformMatrix4fv(light->material.mvp, 1, GL_FALSE, &cam->matrix[0][0]);

		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
	} while(0);

	do {
		Object *obj = &scene->object;

		update_model_matrix(&obj->transform);

		glm_mat4_mul(cam->viewmatrix, obj->transform.modelmatrix, cam->matrix);

		glBindVertexArray(obj->material.vao);
		GLuint shader = obj->material.shader;
		glUseProgram(shader);
		glUniformMatrix4fv(obj->material.modelview, 1, GL_FALSE, &scene->camera.matrix[0][0]);
		glUniformMatrix4fv(obj->material.viewmatrix, 1, GL_FALSE, &scene->camera.viewmatrix[0][0]);

		// normal matrix = M^(-T)
		// glm_mat4_inv(scene->camera.matrix, scene->camera.matrix);
		// glm_mat4_transpose(scene->camera.matrix);
		// glUniformMatrix4fv(obj->material.normalmatrix, 1, GL_FALSE, &scene->camera.matrix[0][0]);

		// or
		// not sure
		glm_mat4_inv(scene->camera.matrix, scene->camera.matrix);
		glUniformMatrix4fv(obj->material.normalmatrix, 1, GL_TRUE, &scene->camera.matrix[0][0]);

		// glUniform3fv(glGetUniformLocation(shader, "light0.direction"), 1, scene->directional.direction);
		glUniform1f(glGetUniformLocation(shader, "light0.enable"), 0);

		glUniform3fv(glGetUniformLocation(shader, "light1.position"), 1, scene->pointlight.transform.position);
		glUniform1f(glGetUniformLocation(shader, "light1.enable"), 1.0f);

		glUniform1f(glGetUniformLocation(shader, "light2.enable"), 0);

		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
	} while(0);
}

static void draw_scene_spotlight(Scene *scene) {
	Camera *camera = &scene->camera;

	ObjectLeaf *leaf = scene->objects;
	do {
		Object *obj = &scene->object;
		// Object *obj = leaf->object;
		leaf = leaf->next;

		update_model_matrix(&obj->transform);

		glm_mat4_mul(camera->viewmatrix, obj->transform.modelmatrix, camera->matrix);

		glBindVertexArray(obj->material.vao);
		GLuint shader = obj->material.shader;
		glUseProgram(shader);
		glUniformMatrix4fv(obj->material.modelview, 1, GL_FALSE, &camera->matrix[0][0]);
		glUniformMatrix4fv(obj->material.viewmatrix, 1, GL_FALSE, &scene->camera.viewmatrix[0][0]);

		// normal matrix = M^(-T)
		// glm_mat4_inv(scene->camera.matrix, scene->camera.matrix);
		// glm_mat4_transpose(scene->camera.matrix);
		// glUniformMatrix4fv(obj->material.normalmatrix, 1, GL_FALSE, &scene->camera.matrix[0][0]);

		// or
		// not sure
		glm_mat4_inv(camera->matrix, camera->matrix);
		glUniformMatrix4fv(obj->material.normalmatrix, 1, GL_TRUE, &camera->matrix[0][0]);

		glUniform3fv(glGetUniformLocation(shader, "light0.direction"), 1, scene->directional.direction);
		glUniform1f(glGetUniformLocation(shader, "light0.enable"), 0);

		glUniform3fv(glGetUniformLocation(shader, "light1.position"), 1, scene->pointlight.transform.position);
		glUniform1f(glGetUniformLocation(shader, "light1.enable"), 0);

		glUniform3fv(glGetUniformLocation(shader, "light2.position"), 1, scene->spotlight.transform.position);
		glUniform3fv(glGetUniformLocation(shader, "light2.direction"), 1, scene->spotlight.direction);
		glUniform1f(glGetUniformLocation(shader, "light2.enable"), 1);

		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
	} while(0);
}

static void draw_scene_alllights(Scene *scene) {
	Camera *cam = &scene->camera;
	DirectionalLight *directional = &scene->directional;

	do {
		update_model_matrix(&directional->transform);
		glm_mat4_mul(cam->viewmatrix, directional->transform.modelmatrix, cam->matrix);
		glm_mat4_mul(cam->projmatrix, cam->matrix, cam->matrix);

		glBindVertexArray(directional->material.vao);
		glUseProgram(directional->material.shader);
		glUniformMatrix4fv(directional->material.mvp, 1, GL_FALSE, &cam->matrix[0][0]);

		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
	} while(0);


	PointLight *light = &scene->pointlight;
	do {
		update_model_matrix(&light->transform);
		glm_mat4_mul(cam->viewmatrix, light->transform.modelmatrix, cam->matrix);
		glm_mat4_mul(cam->projmatrix, cam->matrix, cam->matrix); // mvp

		glBindVertexArray(light->material.vao);
		glUseProgram(light->material.shader);
		glUniformMatrix4fv(light->material.mvp, 1, GL_FALSE, &cam->matrix[0][0]);

		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
	} while(0);



	// ObjectLeaf *leaf = scene->objects;
	do {

		Object *obj = &scene->object;
		// Object *obj = leaf->object;

		update_model_matrix(&obj->transform);

		glm_mat4_mul(cam->viewmatrix, obj->transform.modelmatrix, cam->matrix);

		glBindVertexArray(obj->material.vao);
		GLuint shader = obj->material.shader;
		glUseProgram(shader);
		glUniformMatrix4fv(obj->material.modelview, 1, GL_FALSE, &scene->camera.matrix[0][0]);
		glUniformMatrix4fv(obj->material.viewmatrix, 1, GL_FALSE, &scene->camera.viewmatrix[0][0]);

		// normal matrix = M^(-T)
		// glm_mat4_inv(scene->camera.matrix, scene->camera.matrix);
		// glm_mat4_transpose(scene->camera.matrix);
		// glUniformMatrix4fv(obj->material.normalmatrix, 1, GL_FALSE, &scene->camera.matrix[0][0]);

		// or
		// not sure
		glm_mat4_inv(scene->camera.matrix, scene->camera.matrix);
		glUniformMatrix4fv(obj->material.normalmatrix, 1, GL_TRUE, &scene->camera.matrix[0][0]);

		glUniform3fv(glGetUniformLocation(shader, "light0.direction"), 1, scene->directional.direction);
		glUniform1f(glGetUniformLocation(shader, "light0.enable"), 1.0f);

		glUniform3fv(glGetUniformLocation(shader, "light1.position"), 1, scene->pointlight.transform.position);
		glUniform1f(glGetUniformLocation(shader, "light1.enable"), 1.0f);

		glUniform3fv(glGetUniformLocation(shader, "light2.position"), 1, scene->spotlight.transform.position);
		glUniform3fv(glGetUniformLocation(shader, "light2.direction"), 1, scene->spotlight.direction);
		glUniform1f(glGetUniformLocation(shader, "light2.enable"), 1.0f);

		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

		// leaf = leaf->next;
	} while(0);
}

static void draw_scene_allobj_alllights(Scene *scene) {
	Camera *cam = &scene->camera;
	DirectionalLight *directional = &scene->directional;

	do {
		update_model_matrix(&directional->transform);
		glm_mat4_mul(cam->viewmatrix, directional->transform.modelmatrix, cam->matrix);
		glm_mat4_mul(cam->projmatrix, cam->matrix, cam->matrix);

		glBindVertexArray(directional->material.vao);
		glUseProgram(directional->material.shader);
		glUniformMatrix4fv(directional->material.mvp, 1, GL_FALSE, &cam->matrix[0][0]);

		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
	} while(0);


	PointLight *light = &scene->pointlight;
	do {
		update_model_matrix(&light->transform);
		glm_mat4_mul(cam->viewmatrix, light->transform.modelmatrix, cam->matrix);
		glm_mat4_mul(cam->projmatrix, cam->matrix, cam->matrix); // mvp

		glBindVertexArray(light->material.vao);
		glUseProgram(light->material.shader);
		glUniformMatrix4fv(light->material.mvp, 1, GL_FALSE, &cam->matrix[0][0]);

		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
	} while(0);


	ObjectLeaf *leaf = scene->objects;
	do {
		// Object *obj = &scene->object;
		Object *obj = leaf->object;

		update_model_matrix(&obj->transform);

		glm_mat4_mul(cam->viewmatrix, obj->transform.modelmatrix, cam->matrix);

		glBindVertexArray(obj->material.vao);
		GLuint shader = obj->material.shader;
		glUseProgram(shader);
		glUniformMatrix4fv(obj->material.modelview, 1, GL_FALSE, &scene->camera.matrix[0][0]);
		glUniformMatrix4fv(obj->material.viewmatrix, 1, GL_FALSE, &scene->camera.viewmatrix[0][0]);

		// normal matrix = M^(-T)
		// glm_mat4_inv(scene->camera.matrix, scene->camera.matrix);
		// glm_mat4_transpose(scene->camera.matrix);
		// glUniformMatrix4fv(obj->material.normalmatrix, 1, GL_FALSE, &scene->camera.matrix[0][0]);

		// or
		// not sure
		glm_mat4_inv(scene->camera.matrix, scene->camera.matrix);
		glUniformMatrix4fv(obj->material.normalmatrix, 1, GL_TRUE, &scene->camera.matrix[0][0]);

		glUniform3fv(glGetUniformLocation(shader, "light0.direction"), 1, scene->directional.direction);
		glUniform1f(glGetUniformLocation(shader, "light0.enable"), 1.0f);

		glUniform3fv(glGetUniformLocation(shader, "light1.position"), 1, scene->pointlight.transform.position);
		glUniform1f(glGetUniformLocation(shader, "light1.enable"), 1.0f);

		glUniform3fv(glGetUniformLocation(shader, "light2.position"), 1, scene->spotlight.transform.position);
		glUniform3fv(glGetUniformLocation(shader, "light2.direction"), 1, scene->spotlight.direction);
		glUniform1f(glGetUniformLocation(shader, "light2.enable"), 1.0f);

		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

		leaf = leaf->next;
	} while(leaf);
}


static void move_directional(int x, int y, int z, DirectionalLight *light) {
	if (x || y || z) {
		glm_vec3_muladd((vec3){0.25f,0.25f,0.25f}, (vec3){x, y, z} , light->transform.position);

		glm_vec3_negate_to(light->transform.position, light->direction);
		glm_normalize(light->direction);

		update_model_matrix(&light->transform);
	}
}

static void move_pointlight(int x, int y, int z, PointLight *light) {
	if (x || y || z) {
		glm_vec3_muladd((vec3){0.25f,0.25f,0.25f}, (vec3){x, y, z} , light->transform.position);

		update_model_matrix(&light->transform);
	}
}

static float PITCH_LIMIT0 = M_PI_2 - FLT_EPSILON;
static float PITCH_LIMIT1 = -M_PI_2 + FLT_EPSILON;
static float CAM_ROT_SPEED = M_PI * 2.0f / 180.0f;

static void move_camera(Camera *camera) {
	HEInput *input = he_get_input();
	bool changed = false;
	if (input->axisx || input->axisy) {
		// glm_vec3_muladd((vec3){0.125f,0.125f,0.125f}, (vec3){input->axisx, input->axisy, 0} , camera->direction);
		// glm_normalize(camera->direction);
		// update_view_matrix(camera);

		camera->transform.rotation[0] += input->axisy * CAM_ROT_SPEED; // pitch
		if (camera->transform.rotation[0] > PITCH_LIMIT0) camera->transform.rotation[0] = PITCH_LIMIT0;
		if (camera->transform.rotation[0] < PITCH_LIMIT1) camera->transform.rotation[0] = PITCH_LIMIT1;
		camera->transform.rotation[1] += input->axisx * CAM_ROT_SPEED; // yaw
		camera->direction[0] = cosf(camera->transform.rotation[0]) * cosf(camera->transform.rotation[1]);
		camera->direction[1] = sinf(camera->transform.rotation[0]);
		camera->direction[2] = cosf(camera->transform.rotation[0]) * sinf(camera->transform.rotation[1]);
		glm_normalize(camera->direction);
		glm_cross(GLM_YUP, camera->direction, camera->right);
		glm_normalize(camera->right);
		glm_cross(camera->right, camera->direction, camera->up);
		changed = true;
	}

	if (input->x) {
		// glm_vec3_muladd(camera->direction, (vec3){input->x, input->y, input->z} , camera->transform.position);
		glm_vec3_muladd(camera->right, (vec3){-input->x * 0.25f, -input->x * 0.25f, -input->x * 0.25f} , camera->transform.position);
		changed = true;
	}

	if (input->z) {
		glm_vec3_muladd(camera->direction, (vec3){-input->z * 0.25f, -input->z * 0.25f, -input->z * 0.25f} , camera->transform.position);
		changed = true;
	}

	if (input->y) {
		glm_vec3_muladd(camera->up, (vec3){-input->y * 0.25f, -input->y * 0.25f, -input->y * 0.25f} , camera->transform.position);
		changed = true;
	}

	if (changed) {
		update_view_matrix(camera);
	}
}

static void move_object(Object *object) {
	HEInput *input = he_get_input();
	if (input->axisx || input->axisy) {
		static float rotspeed = 5.0f/180.0f*M_PI;
		glm_vec3_muladd((vec3){rotspeed,rotspeed,rotspeed}, (vec3){-input->axisy, input->axisx, 0} , object->transform.rotation);
		// glm_normalize(object->transform.rotation);
		update_model_matrix(&object->transform);
	}

	if (input->x || input->y || input->z) {
		glm_vec3_muladd((vec3){0.25f,0.25f,0.25f}, (vec3){input->x, input->y, input->z} , object->transform.position);
		update_model_matrix(&object->transform);
	}
}




void play_10_basiclight(int lightmode, int op_move_x, int op_move_y, int op_move_z) {
	Scene* scene = &scene10;

	static int inited = 0;
	if (!inited) {
		inited = 1;
		init_scene(scene);
	}

	if (lightmode == 1) {
		move_directional(op_move_x, op_move_y, op_move_z, &scene->directional);
		draw_scene_directional(scene);
	}
	else if (lightmode == 2) {
		move_pointlight(op_move_x, op_move_y, op_move_z, &scene->pointlight);
		draw_scene_point(scene);
	}
	else if (lightmode == 3) {
		move_camera(&scene->camera);
		// move_object(&scene->object);
		glm_vec3_copy(scene->camera.direction, scene->spotlight.direction);
		glm_vec3_copy(scene->camera.transform.position, scene->spotlight.transform.position);
		draw_scene_spotlight(scene);
	}
	else if (lightmode == 4) {
		move_object(&scene->object);
		glm_vec3_copy(scene->camera.direction, scene->spotlight.direction);
		glm_vec3_copy(scene->camera.transform.position, scene->spotlight.transform.position);
		draw_scene_alllights(scene);
	}
	else if (lightmode == 5) {
		move_camera(&scene->camera);
		glm_vec3_copy(scene->camera.direction, scene->spotlight.direction);
		glm_vec3_copy(scene->camera.transform.position, scene->spotlight.transform.position);
		draw_scene_allobj_alllights(scene);
	}
}