#include "play_03_two_triangle.h"
#include <GL/glew.h>
#include <SDL3/SDL_opengl.h>
#include "hegl_util.h"

static GLuint vao3;
static GLuint shader3;

void init_03_shader() {
	const char * const vert = "#version 330 core\n"
	"layout (location=0) in vec3 aPos;\n"
	"void main() { gl_Position = vec4(aPos.xyz, 1.0f); }";

	const char * const frag = "#version 330 core\n"
	"out vec4 fragColor;\n"
	"void main() { fragColor = vec4(0.7, 1.0, 1.0, 1.0); }";

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vert, NULL);
	glCompileShader(vs);
	hegl_check_shader_compile_error(vs);

	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &frag, NULL);
	glCompileShader(fs);
	hegl_check_shader_compile_error(fs);


	shader3 = glCreateProgram();
	glAttachShader(shader3, vs);
	glAttachShader(shader3, fs);
	glLinkProgram(shader3);
	glDeleteShader(vs);
	glDeleteShader(fs);
	hegl_check_shader_link_error(shader3);
}

void init_03_vertex() {
	glGenVertexArrays(1, &vao3);
	glBindVertexArray(vao3);

	float vertices[] = {
		-0.75f, 0, 0,
		-0.25f, 0.5f, 0,
		-0.25f, -0.5f, 0,

		0.75f, 0, 0,
		0.25f, 0.5f, 0,
		0.25f, -0.5f, 0,
	};

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), NULL);
	glEnableVertexAttribArray(0);


	GLuint indices[] = {
		0,1,2,
		3,4,5
	};
	GLuint ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


	glBindVertexArray(0);
}


void hegl_play_03_draw_two_triangle() {
	if (!shader3) {
		init_03_shader();
		init_03_vertex();
	}

	glUseProgram(shader3);
	glBindVertexArray(vao3);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
}