#include "play_02_rectangle.h"
#include <stdlib.h>
#include <stdio.h>
#include <GL/glew.h>
#include <SDL3/SDL_opengl.h>
#include "hegl_util.h"

static GLuint vao2;
static GLuint vbo2;
static GLuint ebo2;
static GLuint shader2;

void init_02_shader() {
	const char * vert = "#version 330 core\n"
		"layout (location = 0) in vec3 aPos;\n"
		"void main() { gl_Position = vec4(aPos.xyz, 1.0f); }";

	const char *frag = "#version 330 core\n"
		"out vec4 fragColor;\n"
		"void main() { fragColor = vec4(0.3f, 0.4f, 0.5f, 1.0f); }";

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vert, NULL);
	glCompileShader(vs);
	hegl_check_shader_compile_error(vs);

	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &frag, NULL);
	glCompileShader(fs);
	hegl_check_shader_compile_error(fs);

	shader2 = glCreateProgram();
	glAttachShader(shader2, vs);
	glAttachShader(shader2, fs);
	glLinkProgram(shader2);
	glDeleteShader(vs);
	glDeleteShader(fs);
	hegl_check_shader_link_error(shader2);
}

void init_02_rectangle() {
	glGenVertexArrays(1, &vao2);
	glGenBuffers(1, &vbo2);
	glGenBuffers(1, &ebo2);

	glBindVertexArray(vao2);


	float vertices[] = {
		-0.5f,0.5f, 0,
		0.5f, 0.5f, 0,
		0.5f, -0.5f, 0,
		-0.5f, -0.5f, 0,
	};

	glBindBuffer(GL_ARRAY_BUFFER, vbo2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


	GLuint indices[] = {
		0,1,3,
		1,2,3
	};

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo2);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


	GLuint idx = 0; // in vert shader: layout (location = 0)
	int size = 3; // the number of components the vertex attribute, here is vec3, 3 componets
	GLsizei stride = 3 * sizeof(float); // byte offset between attributes, here an attribute is vec3, no gap
	// pointer is the offset of the first data in buffer
	glVertexAttribPointer(idx, size, GL_FLOAT, GL_FALSE, stride, 0);
	glEnableVertexAttribArray(idx);


	glBindVertexArray(0);
}

void hegl_play_02_draw_rectangle() {
	if (!shader2) {
		init_02_shader();
		init_02_rectangle();
	}

	glUseProgram(shader2);
	glBindVertexArray(vao2);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
}