#include "play_01_triangle.h"
#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <SDL3/SDL_opengl.h>

#define BUF_SIZE 512
char buff[BUF_SIZE];

static GLuint vao = 0;
static GLuint vbo = 0;
static GLuint shader = 0;

void hegl_init_shader() {

	const char * vert = "#version 330 core\n"
	"layout (location = 0) in vec3 aPos;\n"
	"void main() { gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0); }";

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vert, NULL);
	glCompileShader(vs);

	GLint success;

	glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vs, BUF_SIZE, NULL, buff);
		printf("gl compile vertex err:%s\n", buff);
		abort();
	}


	const char *frag = "#version 330 core\n"
	"out vec4 fragColor;\n"
	"void main() { fragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f); }";

	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &frag, NULL);
	glCompileShader(fs);

	glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fs, BUF_SIZE, NULL, buff);
		printf("gl compile fragment err:%s\n", buff);
		abort();
	}


	shader = glCreateProgram();
	glAttachShader(shader, vs);
	glAttachShader(shader, fs);
	glLinkProgram(shader);

	glGetProgramiv(shader, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shader, BUF_SIZE, NULL, buff);
		printf("err create shader program:%s", buff);
		abort();
	}

	glDeleteShader(vs);
	glDeleteShader(fs);
}

void hegl_init_a_triangle() {

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	float vertices[] = {
		0, 0.5f, 0,
		-0.5f, -0.5f, 0,
		0.5f, -0.5f, 0
	};

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void hegl_play_draw_a_triangle() {
	if (vbo == 0) {
		hegl_init_shader();
		hegl_init_a_triangle();
	}

	glUseProgram(shader);

	glBindVertexArray(vao);

	glDrawArrays(GL_TRIANGLES, 0, 3);
}


static GLuint vbo_01;

void hegl_play_01_draw_triangle() {
	if (!vbo_01) {
		float vertices[] = {
		-0.5f, -0.25f, 0,
		0, 0.25f, 0,
		0.5f, -0.25f, 0,
		};

		glGenBuffers(1, &vbo_01);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_01);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(0);

		if (!shader)
			hegl_init_shader();
	}

	glUseProgram(shader);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_01);
	glDrawArrays(GL_TRIANGLES, 0, 3);
}