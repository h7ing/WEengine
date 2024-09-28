#include "play_04_shader.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <GL/glew.h>
#include "hengine.h"
#include "hegl_util.h"

typedef struct HShaderProgram {
	GLuint vao;
	GLuint vbo;
	GLuint ebo;
	GLuint shader;
	int ecnt;
	float color[4];
	bool anim;
	float delta;
} HShaderProgram;

static HShaderProgram progs[4];

static bool b_play04inited = false;



void init_04_triangles() {
	float vertices[] = {
		-0.5f, -0.25f, 0,
		-0.25f, 0, 0,
		0, 0.25f, 0,
		0, -0.25f, 0,
		0.25f, 0, 0,
		0.5f, -0.25f, 0
	};

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


	GLuint indices[][3] = {
		{0, 1, 3},
		{1, 2, 4},
		{3, 4, 5},
		{1, 3, 4}
	};

	GLuint vaos[4];
	glGenVertexArrays(4, vaos);

	GLuint ebos[4];
	glGenBuffers(4, ebos);

	char buffer[20];
	for (int i = 0; i < 4; i++) {

		HShaderProgram *prog = &progs[i];
		prog->vao = vaos[i];
		prog->ebo = ebos[i];
		prog->ecnt = 3;

		glBindVertexArray(prog->vao);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, prog->ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[i]), indices[i], GL_STATIC_DRAW);

		prog->vbo = vbo;
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(0);

		snprintf(buffer, 20, "shaders/frag0%d.frag", i+1);
		prog->shader = hegl_create_shader_program("shaders/vert.vert", buffer);
	}
}

void hegl_play_04_draw_different_triangles() {
	if (!b_play04inited) {
		b_play04inited = true;
		init_04_triangles();
	}

	for (int i = 0; i < 4; i++) {
		HShaderProgram *prog = &progs[i];
		glUseProgram(prog->shader);
		glBindVertexArray(prog->vao);
		glDrawElements(GL_TRIANGLES, prog->ecnt, GL_UNSIGNED_INT, NULL);
	}
}


static HShaderProgram pyramids[4];

void init_pyramid() {
	GLuint prog = hegl_create_shader_program("shaders/vert.vert", "shaders/frag.frag");

	float vertices[] = {
		-0.5f, -0.433f, 0, 1.0f, 0, 0,
		-0.25f, 0, 0, 1.0f, 0, 0,
		0, 0.433f, 0, 0, 1.0f, 0,
		0, -0.433f, 0, 0, 1.0f, 0,
		0.25f, 0, 0, 0, 0, 1.0f,
		0.5f, -0.433f, 0, 0, 0, 1.0f,
	};

	GLuint indices[][3] = {
		{0,1,3},
		{1,2,4},
		{3,4,5},
		{1,3,4}
	};

	float colors[][4] = {
		{1.0f, 0, 0, 1.0f},
		{0, 1.0f, 0, 1.0f},
		{0, 0, 1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f, 1.0f},
	};

	GLuint vaos[4], vbo;

	glGenVertexArrays(4, vaos);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	for (int i = 0; i < 4; i++) {
		HShaderProgram *pyramid = &pyramids[i];
		pyramid->shader = prog;
		pyramid->vao = vaos[i];
		memcpy(pyramid->color, colors[i], sizeof(colors[i]));

		glBindVertexArray(pyramid->vao);

		glGenBuffers(1, &pyramid->ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pyramid->ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[i]), indices[i], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		// location 0 - aPos
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), NULL);
		glEnableVertexAttribArray(0);

		// location 1 - aColor
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 *sizeof(float), (void *)(3 * sizeof(float)) );
		glEnableVertexAttribArray(1);
	}

	pyramids[3].anim = true;
}

void hegl_play_04_draw_pyramid() {
	static bool inited = false;
	if (!inited) {
		inited = true;
		init_pyramid();
	}

	for (int i = 0; i < 4; i++) {
		HShaderProgram *obj = &pyramids[i];
		glUseProgram(obj->shader);
		glBindVertexArray(obj->vao);

		GLint location = glGetUniformLocation(obj->shader, "uColorRatio");
		if (obj->anim) {
			// float colors[3];
			// for (int j = 0; j < 3; j++) {
			// 	obj->color[j] += he_get_delta() * 2.0f;
			// 	colors[j] = sin(obj->color[j]) * 0.5f + 0.5f;
			// }
			// glUniform4f(location, colors[0], colors[1], colors[2], obj->color[3]);
			obj->delta += he_get_delta();
			float uColorRatio = sin(obj->delta) * 0.5f + 0.5f;
			glUniform1f(location, uColorRatio);
		}
		else {
			// glUniform4f(location, obj->color[0], obj->color[1], obj->color[2], obj->color[3]);
			glUniform1f(location, 1.0f);
		}

		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, NULL);
	}
}


GLuint mode_5_vao;
GLuint mode_5_shaderprog;

void hegl_init_vao_color() {
	float vertices[] = {
		0, 1.0f, 0, 1.0f, 0, 0,
		0.5f, 0.5f, 0, 0, 1.0f, 0,
		-0.5f, 0.5f, 0, 0, 0, 1.0f
	};

	GLuint vao, vbo, ebo;
	glGenVertexArrays(1, &vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(vao);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	mode_5_vao = vao;
	mode_5_shaderprog = hegl_create_shader_program("shaders/vert_pos_n_col.vert", "shaders/frag_pos_n_col.frag");
}

void play_04_draw_with_vao_color() {
	static int mode_vao_color_ready = 0;
	if (!mode_vao_color_ready) {
		mode_vao_color_ready = 1;
		hegl_init_vao_color();
	}
	
	glUseProgram(mode_5_shaderprog);
	glBindVertexArray(mode_5_vao);
	glDrawArrays(GL_TRIANGLES, 0, 3);
}