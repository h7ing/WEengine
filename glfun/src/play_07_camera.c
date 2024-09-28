#include "play_07_camera.h"
#include <math.h>
#include <stdint.h>
#include <float.h>
#include <GL/glew.h>
#include <cglm/cglm.h>
#include "hegl_util.h"
#include "hengine.h"


typedef struct DrawElement {
	GLuint shader;
	GLuint tex;
	GLuint vao;
	GLuint vbo;
	GLuint translocation;
} DrawElement; // aka Material


typedef struct Transform {
	vec3 position;
	vec3 direction;
	vec3 rotation;
	mat4 transform; // not good name
} Transform;


typedef struct Camera {
	Transform transform;
	mat4 projection;
	mat4 mvp;
	vec3 target;
	vec3 up;
	vec3 right;
	float fov;
	float speedtranslate;
	float speedrotate;
} Camera;


static DrawElement element07;
static Camera mycamera;


static vec3 positions[] = {
	{0.0f,  0.0f,  0.0f},
	{2.0f,  5.0f, -15.0f},
	{-1.5f, -2.2f, -2.5f},
	{-3.8f, -2.0f, -12.3f},
	{2.4f, -0.4f, -3.5f},
	{-1.7f,  3.0f, -7.5f},
	{1.3f, -2.0f, -2.5f},
	{1.5f,  2.0f, -2.5f},
	{1.5f,  0.2f, -1.5f},
	{-1.3f,  1.0f, -1.5f},
};

Transform **cubes;


static void init_07_shader() {

	DrawElement *ele = &element07;

	ele->shader = hegl_create_shader_program("shaders/vert_06.vert", "shaders/frag_06_01.frag");

	ele->tex = hegl_create_texture("textures/texture.png");

	glUseProgram(ele->shader);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, ele->tex);

	glUniform1i(glGetUniformLocation(ele->shader, "tex"), 0);

	ele->translocation = glGetUniformLocation(ele->shader, "transform");
}

static void init_07_vertices() {
	DrawElement *ele = &element07;

	glGenVertexArrays(1, &ele->vao);
	glBindVertexArray(ele->vao);

	// cubes
	float vertices[] = {
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};

	glGenBuffers(1, &ele->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, ele->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

	// 0: vec3 aPos
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), 0);
	glEnableVertexAttribArray(0);

	// 1: vec2 aCoord
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (const void *)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
}

static void MyLookAt(vec3 campos, vec3 targetpos, vec3 camup, mat4 dest) {
	// u,v,w must be normalized
	// t -> view up
	// w = -g -> -direction -> eye - target
	// u = t x w
	// v = w x u

	vec3 w;
	glm_vec3_sub(campos, targetpos, w);
	glm_normalize(w);

	vec3 u = GLM_VEC3_ZERO_INIT;
	glm_vec3_crossn(camup, w, u); // need normalise

	vec3 v = GLM_VEC3_ZERO_INIT;
	glm_vec3_cross(w, u, v);

	glm_mat4_identity(dest);
	glm_translate(dest, (vec3){-campos[0],-campos[1],-campos[2]});

	// glm按列保存矩阵，相当于转置一下
	glm_mat4_mul((mat4){
		{u[0],v[0],w[0],0},
		{u[1],v[1],w[1],0},
		{u[2],v[2],w[2],0},
		{0,0,0,1.0f}}, dest, dest);
}

static void MyLookAtDirection(vec3 campos, vec3 direction, vec3 camup, mat4 dest) {
	// u,v,w must be normalized
	// t -> view up
	// w = -g -> -direction
	// u = t x w
	// v = w x u

	vec3 w;
	glm_vec3_negate_to(direction, w);

	vec3 u = GLM_VEC3_ZERO_INIT;
	glm_vec3_crossn(camup, w, u); // need normalise

	vec3 v = GLM_VEC3_ZERO_INIT;
	glm_vec3_cross(w, u, v);

	glm_mat4_identity(dest);
	glm_translate(dest, (vec3){-campos[0],-campos[1],-campos[2]});

	// glm按列保存矩阵，相当于转置一下
	glm_mat4_mul((mat4){
		{u[0],v[0],w[0],0},
		{u[1],v[1],w[1],0},
		{u[2],v[2],w[2],0},
		{0,0,0,1.0f}}, dest, dest);
}


static void init_07_mvp() {
	Camera *cam = &mycamera;

	glm_vec3_copy((vec3){0,0,0}, cam->transform.position);
	glm_vec3_copy((vec3){0,0,-1.0f}, cam->transform.direction);
	glm_vec3_copy((vec3){0, -M_PI_2, 0}, cam->transform.rotation);
	glm_vec3_add(cam->transform.position, cam->transform.direction, cam->target);

	glm_vec3_copy(GLM_YUP, cam->up);
	glm_lookat(cam->transform.position, cam->target, cam->up, cam->transform.transform);

	glm_cross(GLM_YUP, cam->transform.direction, cam->right);
	glm_normalize(cam->right);

	cam->fov = M_PI/4.0f;
	glm_perspective(cam->fov, 8.0f/6.0f, 0.1f, 100.0f, cam->projection);


	cam->speedtranslate = 2.0f;
	cam->speedrotate = M_PI / 180.0f;
}


static void init_07_cubes() {
	int cubecnt = sizeof(positions) / sizeof(positions[0]);
	cubes = malloc(sizeof(Transform *) * (cubecnt+1));
	cubes[cubecnt] = 0;

	for (int i = 0; i < cubecnt; i++) {
		Transform *t = malloc(sizeof(Transform));
		cubes[i] = t;
		glm_vec3_copy(positions[i], t->position);

		glm_mat4_identity(t->transform);
		glm_translate(t->transform, t->position);
	}
}

static float kpitchlimit[] = {
	M_PI_2 - FLT_EPSILON,
	-M_PI_2 + FLT_EPSILON
};

static void update_camera(Camera *cam, int op_translate, float op_rotate_x, float op_rotate_y, float op_zoom) {
	int nochange = 0;
	switch (op_translate) {
		case 1:
			// cam->transform.position[2] -= cam->speedtranslate * he_get_delta();
			glm_vec3_muladds(cam->transform.direction, cam->speedtranslate * he_get_delta(), cam->transform.position);
			break;
		case -1:
			// cam->transform.position[2] += cam->speedtranslate * he_get_delta();
			glm_vec3_mulsubs(cam->transform.direction, cam->speedtranslate * he_get_delta(), cam->transform.position);
			break;
		case 2:
			// cam->transform.position[0] += cam->speedtranslate * he_get_delta();
			// actually right is left
			glm_vec3_mulsubs(cam->right, cam->speedtranslate * he_get_delta(), cam->transform.position);
			break;
		case -2:
			// cam->transform.position[0] -= cam->speedtranslate * he_get_delta();
			glm_vec3_muladds(cam->right, cam->speedtranslate * he_get_delta(), cam->transform.position);
			break;
		default:
			nochange = 1;
			break;
	}

	cam->transform.rotation[0] -= op_rotate_y * cam->speedrotate; // pitch
	if (cam->transform.rotation[0] > kpitchlimit[0]) cam->transform.rotation[0] = kpitchlimit[0];
	if (cam->transform.rotation[0] < kpitchlimit[1]) cam->transform.rotation[0] = kpitchlimit[1];
	cam->transform.rotation[1] += op_rotate_x * cam->speedrotate; // yaw
	// if (cam->transform.rotation[1] > kpitchlimit[0]) cam->transform.rotation[1] = kpitchlimit[0];
	// if (cam->transform.rotation[1] < kpitchlimit[1]) cam->transform.rotation[1] = kpitchlimit[1];
	cam->transform.direction[0] = cosf(cam->transform.rotation[0]) * cosf(cam->transform.rotation[1]);
	cam->transform.direction[1] = sinf(cam->transform.rotation[0]);
	cam->transform.direction[2] = cosf(cam->transform.rotation[0]) * sinf(cam->transform.rotation[1]);
	glm_normalize(cam->transform.direction);

	glm_cross(GLM_YUP, cam->transform.direction, cam->right);
	glm_normalize(cam->right);

	// glm_cross(cam->transform.direction, cam->right, cam->up);
	// glm_normalize(cam->up);

	int calway = 1;

	//if (!nochange) {
	if (calway == 1) {
		// glm_vec3_add(cam->transform.position, cam->transform.direction, cam->target);
		// glm_lookat(cam->transform.position, cam->target, cam->up, cam->transform.transform);

		glm_look(cam->transform.position, cam->transform.direction, cam->up, cam->transform.transform);
	} else if (calway == 2) {
		glm_vec3_add(cam->transform.position, cam->transform.direction, cam->target);
		MyLookAt(cam->transform.position, cam->target, cam->up, cam->transform.transform);
	} else if (calway == 3) {
		MyLookAtDirection(cam->transform.position, cam->transform.direction, cam->up, cam->transform.transform);
	}
	//}

	if (op_zoom > FLT_EPSILON || op_zoom < -FLT_EPSILON) {
		cam->fov -= op_zoom * M_PI / 90.0f;
		printf("fovy:%f(%f)\n", cam->fov, cam->fov / M_PI * 180.0f);
		glm_perspective(cam->fov, 8.0f/6.0f, 0.1f, 100.0f, cam->projection);
	}
}


static void init_07_camera() {
	init_07_shader();
	init_07_vertices();
	init_07_mvp();
	init_07_cubes();
}

void play_07_camera(int op_translate, float op_rotate_x, float op_rotate_y, float op_zoom) {
	static int inited = 0;
	if (!inited) {
		inited = 1;
		init_07_camera();
	}

	DrawElement *ele = &element07;
	Camera *cam = &mycamera;

	update_camera(cam, op_translate, op_rotate_x, op_rotate_y, op_zoom);

	glUseProgram(ele->shader);
	glBindVertexArray(ele->vao);

	// int cubecnt = sizeof(cubes) / sizeof(cubes[0]);
	for (int i = 0; cubes[i]; i++) {
		Transform *cube = cubes[i];

		// transform formulation: P*V*M*p
		// because use column major(列优先), 相当于先转置了一下, 所以实际计算时按转置的顺序
		// (ABC)^T = C^T * B^T * A^T
		glm_mat4_mul(cam->projection, cam->transform.transform, cam->mvp);
		glm_mat4_mul(cam->mvp, cube->transform, cam->mvp);
		// or same form:
		// glm_mat4_mulN((mat4 *[]){&cam->projection, &cam->transform.transform, &cube->transform}, 3, cam->mvp);
		glUniformMatrix4fv(ele->translocation, 1, GL_FALSE, &cam->mvp[0][0]);

		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
}