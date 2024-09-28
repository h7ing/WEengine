#include "play_06_math.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <GL/glew.h>
#include <cglm/cglm.h>
#include "hegl_util.h"
#include "hengine.h"

typedef struct DrawElementSix {
	GLuint vao;
	GLuint vbo;
	GLuint ebo;
	GLuint shader;
	GLuint tex;
	GLuint location_transform;

	mat4 trans_identity;
	mat4 trans_simple;

	mat4 trans_model;
	mat4 trans_view;
	mat4 trans_projection;

	GLuint location_model;
	GLuint location_view;
	GLuint location_projection;

} DrawElementSix;

static DrawElementSix element;
static DrawElementSix element_mvp;
static DrawElementSix element_cube;
static DrawElementSix element_cubes;


static void play_06_reset_trans(DrawElementSix *ele, float cameraz);


static void play_06_init_simpleelement() {
	element.shader = hegl_create_shader_program("shaders/vert_06.vert", "shaders/frag_06_01.frag");

	int width, height, channels;
	unsigned char *tex = he_asset_load_texture("textures/texture.png", &width, &height, &channels);
	glGenTextures(1, &element.tex);
	glBindTexture(GL_TEXTURE_2D, element.tex);
	glTexImage2D(GL_TEXTURE_2D,
			0,
			GL_RGBA,
			width,
			height,
			0,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			tex);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	he_asset_free_image(tex);

	glUseProgram(element.shader);
	// glActiveTexture(GL_TEXTURE0); // no need for just one
	glUniform1i(glGetUniformLocation(element.shader, "tex"), 0);
	element.location_transform = glGetUniformLocation(element.shader, "transform");

	glm_mat4_identity(element.trans_identity);

	glm_mat4_identity(element.trans_simple);
	// glm_scale(element.trans_simple, (vec3){2.0f,2.0f,2.0f});
	// glm_rotate(element.trans_simple, 45.0f, (vec3){0,0,1.0f});
	// glm_translate(element.trans_simple, (vec3){0.25f, 0.25f, 0});

	glGenVertexArrays(1, &element.vao);
	glBindVertexArray(element.vao);

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	element.vbo = vbo;

	float vertices[] = {
		-0.25f, 0.25f, 0, 0, 1.0f,
		0.25f, 0.25f, 0, 1.0f, 1.0f,
		0.25f, -0.25f, 0, 1.0f, 0,
		-0.25f, -0.25f, 0, 0, 0
	};

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


	GLsizei stride = 5 * sizeof(float);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (const void *)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);


	GLuint indices[] = {
		0,1,3,
		1,2,3
	};

	GLuint ebo;
	glGenBuffers(1, &ebo);
	element.ebo = ebo;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
}

static void play_06_init_withmvp() {
	element_mvp.shader = hegl_create_shader_program("shaders/vert_06_mvp.vert", "shaders/frag_06_01.frag");
	element_mvp.tex = hegl_create_texture("textures/texture.png");

	glUseProgram(element_mvp.shader);
	glActiveTexture(element_mvp.tex);
	glBindTexture(GL_TEXTURE_2D, element_mvp.tex);
	glUniform1i(glGetUniformLocation(element_mvp.shader, "tex"), 0);


	// vertex
	float vertices[] = {
		-0.25f, 0.25f, 0, 0, 1.0f,
		0.25f, 0.25f, 0, 1.0f, 1.0f,
		0.25f, -0.25f, 0, 1.0f, 0,
		-0.25f, -0.25f, 0, 0, 0
	};

	GLuint indices[] = {
		0,1,3,
		1,2,3
	};


	glGenVertexArrays(1, &element_mvp.vao);
	glBindVertexArray(element_mvp.vao);


	glGenBuffers(1, &element_mvp.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, element_mvp.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (const void *)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);


	glGenBuffers(1, &element_mvp.ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_mvp.ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


	// transforms
	glm_mat4_identity(element_mvp.trans_model);
	glm_mat4_identity(element_mvp.trans_view);
	glm_translate_z(element_mvp.trans_view, -1.0f);
	glm_perspective(M_PI/4.0f, 8.0f/6.0f, 0.1f, 100.0f, element_mvp.trans_projection);

	element_mvp.location_model = glGetUniformLocation(element_mvp.shader, "model");
	element_mvp.location_view = glGetUniformLocation(element_mvp.shader, "view");
	element_mvp.location_projection = glGetUniformLocation(element_mvp.shader, "projection");
}


static void play_06_init_cube(DrawElementSix *ele) {
	ele->shader = hegl_create_shader_program("shaders/vert_06_mvp.vert", "shaders/frag_06_01.frag");
	ele->tex = hegl_create_texture("textures/texture.png");

	glUseProgram(ele->shader);
	glActiveTexture(ele->tex);
	glBindTexture(GL_TEXTURE_2D, ele->tex);
	glUniform1i(glGetUniformLocation(ele->shader, "tex"), 0);


	// cube
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


	glGenVertexArrays(1, &ele->vao);
	glBindVertexArray(ele->vao);


	glGenBuffers(1, &ele->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, ele->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (const void *)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);




	// transforms
	glm_mat4_identity(ele->trans_model);
	glm_mat4_identity(ele->trans_view);
	glm_translate_z(ele->trans_view, -1.0f);
	glm_perspective(M_PI/4.0f, 8.0f/6.0f, 0.1f, 100.0f, ele->trans_projection);

	ele->location_model = glGetUniformLocation(ele->shader, "model");
	ele->location_view = glGetUniformLocation(ele->shader, "view");
	ele->location_projection = glGetUniformLocation(ele->shader, "projection");
}



static vec3 cubes[] = {
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

// static float cube_rotates[] = {
// 	M_PI/5.0f,
// 	M_PI/5.0f,
// 	M_PI/5.0f,
// };

static float cube_rotate = 0;


static void play_06_init() {
	play_06_init_simpleelement();
	play_06_init_withmvp();
	play_06_init_cube(&element_cube);
	play_06_init_cube(&element_cubes);
	play_06_reset_trans(&element_cubes, -5.0f);
}

static void draw_06_basic(int rotate, int translate) {
	glUseProgram(element.shader);
	glUniformMatrix4fv(element.location_transform, 1, GL_FALSE, &element.trans_identity[0][0]);
	glBindVertexArray(element.vao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}


static vec3 simple_rotate_axis = {0, 0, -1.0f};
static vec3 simple_translate = GLM_VEC3_ZERO_INIT;

static void draw_06_simple_tranform(int rotate, int translate) {
	glUseProgram(element.shader);

	if (rotate) {
		glm_rotate(element.trans_simple, (float)rotate * glm_rad(5.0f), simple_rotate_axis);
	}

	if (translate) {
		glm_vec3_zero(simple_translate);
		switch (translate)
		{
		case 1: // up
			simple_translate[1] = 0.1f;
			break;
		case 2: // right
			simple_translate[0] = 0.1f;
			break;
		case -1: // down
			simple_translate[1] = -0.1f;
			break;
		case -2: // left
			simple_translate[0] = -0.1f;
			break;
		default:
			break;
		}
		glm_translate(element.trans_simple, simple_translate);
	}


	glUniformMatrix4fv(element.location_transform, 1, GL_FALSE, &element.trans_simple[0][0]);
	glBindVertexArray(element.vao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}


static void draw_06_with_mvp(int rotate, int translate) {
	switch (translate) {
		case 1: // up
			glm_translate_z(element_mvp.trans_view, 0.1f);
			break;
		case 2: // right
			glm_rotate(element_mvp.trans_model, M_PI_4/5.0f, GLM_YUP);
			break;
		case -1: // down
			glm_translate_z(element_mvp.trans_view, -0.1f);
			break;
		case -2: // left
			glm_rotate(element_mvp.trans_model, -M_PI_4/5.0f, GLM_YUP);
			break;
		default: break;
	}

	glUseProgram(element_mvp.shader);
	glUniformMatrix4fv(element_mvp.location_model, 1, GL_FALSE, &element_mvp.trans_model[0][0]);
	glUniformMatrix4fv(element_mvp.location_view, 1, GL_FALSE, &element_mvp.trans_view[0][0]);
	glUniformMatrix4fv(element_mvp.location_projection, 1, GL_FALSE, &element_mvp.trans_projection[0][0]);

	glBindVertexArray(element_mvp.vao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}



static void draw_06_cube(int rotate, int translate) {
	DrawElementSix *ele = &element_cube;
	switch (translate) {
		case 1: // up
			glm_translate_z(ele->trans_view, 0.1f);
			break;
		case 2: // right
			glm_rotate(ele->trans_model, M_PI_4/5.0f, GLM_YUP);
			break;
		case -1: // down
			glm_translate_z(ele->trans_view, -0.1f);
			break;
		case -2: // left
			glm_rotate(ele->trans_model, -M_PI_4/5.0f, GLM_YUP);
			break;
		default: break;
	}

	glUseProgram(ele->shader);
	glUniformMatrix4fv(ele->location_model, 1, GL_FALSE, &ele->trans_model[0][0]);
	glUniformMatrix4fv(ele->location_view, 1, GL_FALSE, &ele->trans_view[0][0]);
	glUniformMatrix4fv(ele->location_projection, 1, GL_FALSE, &ele->trans_projection[0][0]);

	glBindVertexArray(ele->vao);
	// glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}


static void draw_06_cubes(int rotate, int translate) {

	size_t cubecnt = sizeof(cubes) / sizeof(cubes[0]);

	DrawElementSix *ele = &element_cubes;
	switch (translate) {
		case 1: // up
			// glm_translate_z(ele->trans_view, 0.1f);
			glm_ortho(-4.0f, 4.0f, -3.0f, 3.0f, 0.1f, 100.0f, ele->trans_projection);
			break;
		case 2: // right
			// glm_rotate(ele->trans_model, M_PI_4/5.0f, GLM_YUP);
			cube_rotate += M_PI_4/5.0f;
			break;
		case -1: // down
			glm_translate_z(ele->trans_view, -0.1f);
			break;
		case -2: // left
			// glm_rotate(ele->trans_model, -M_PI_4/5.0f, GLM_YUP);
			cube_rotate -= M_PI_4/5.0f;
			break;
		default: break;
	}

	for (int i = 0; i < cubecnt; i++) {
		// vec3 *cube = &cubes[i];


		glUseProgram(ele->shader);

		glm_mat4_identity(ele->trans_model);
		glm_translate(ele->trans_model, cubes[i]);
		if (i%3 == 0) {
			glm_rotate(ele->trans_model, cube_rotate, GLM_YUP);
		}


		glUniformMatrix4fv(ele->location_model, 1, GL_FALSE, &ele->trans_model[0][0]);
		glUniformMatrix4fv(ele->location_view, 1, GL_FALSE, &ele->trans_view[0][0]);
		glUniformMatrix4fv(ele->location_projection, 1, GL_FALSE, &ele->trans_projection[0][0]);

		glBindVertexArray(ele->vao);
		glDrawArrays(GL_TRIANGLES, 0, 36);

	}

}


typedef void (*drawfunc)(int, int);

typedef struct loopnode {
	struct loopnode *next;
	drawfunc df;
} loopnode;

static loopnode *modelist = NULL;
static loopnode *curmode = NULL;

void play_06_toggle_mode() {
	if (!curmode) return;
	curmode = curmode->next;
}


void play_06_math(int rotate, int translate) {
	static int inited = 0;
	if (!inited) {
		inited = 1;
		play_06_init();

		loopnode *modes;
		loopnode *beginnode;

		// 1
		modelist = malloc(sizeof(loopnode));
		modelist->df = draw_06_simple_tranform;

		beginnode = modelist;
		modes = modelist;

		// 2
		modes->next = malloc(sizeof(loopnode));
		modes->next->df = draw_06_with_mvp;

		modes = modes->next;

		// 3
		modes->next = malloc(sizeof(loopnode));
		modes->next->df = draw_06_cube;

		modes = modes->next;

		// 4
		modes->next = malloc(sizeof(loopnode));
		modes->next->df = draw_06_cubes;

		modes = modes->next;

		// end
		modes->next = beginnode;

		curmode = modelist;
	}

	curmode->df(rotate, translate);
}


static void play_06_reset_trans(DrawElementSix *ele, float cameraz) {
	glm_mat4_identity(ele->trans_model);
	glm_mat4_identity(ele->trans_view);
	glm_translate_z(ele->trans_view, cameraz);
	glm_perspective(M_PI/4.0f, 8.0f/6.0f, 0.1f, 100.0f, ele->trans_projection);
}

void play_06_math_restore() {
	glm_mat4_identity(element.trans_simple);

	play_06_reset_trans(&element_mvp, -1.0f);
	play_06_reset_trans(&element_cube, -1.0f);
	play_06_reset_trans(&element_cubes, -5.0f);
}