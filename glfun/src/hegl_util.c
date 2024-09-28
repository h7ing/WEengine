#include "hegl_util.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "hengine.h"

void hegl_check_shader_status(GLuint id, GLenum st) {
	int status;
	glGetShaderiv(id, st, &status);

	if (!status) {

		GLint logSize;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &logSize);

		GLchar *infoLog = malloc(logSize);
		glGetShaderInfoLog(id, logSize, NULL, infoLog);
		printf("err:%s\n", infoLog);
		free(infoLog);

		abort();
	}
}

void hegl_check_shader_compile_error(GLuint id) {
	hegl_check_shader_status(id, GL_COMPILE_STATUS);
}

void hegl_check_shader_link_error(GLuint id) {

	int status;
	glGetProgramiv(id, GL_LINK_STATUS, &status);

	if (!status) {

		GLint logSize;
		glGetProgramiv(id, GL_INFO_LOG_LENGTH, &logSize);

		GLchar *infoLog = malloc(logSize);
		glGetProgramInfoLog(id, logSize, NULL, infoLog);
		printf("err:%s\n", infoLog);
		free(infoLog);

		abort();
	}
}

GLuint hegl_create_shader(const char *filename, GLenum type) {
	char *content = he_asset_load_content(filename);
	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &content, NULL);
	glCompileShader(shader);
	free(content);
	hegl_check_shader_compile_error(shader);

	return shader;
}

GLuint hegl_create_shader_program(const char *vert, const char *frag) {
	GLuint vs = hegl_create_shader(vert, GL_VERTEX_SHADER);
	GLuint fs = hegl_create_shader(frag, GL_FRAGMENT_SHADER);

	GLuint prog = glCreateProgram();
	glAttachShader(prog, vs);
	glAttachShader(prog, fs);
	glLinkProgram(prog);
	glDeleteShader(vs);
	glDeleteShader(fs);
	hegl_check_shader_link_error(prog);

	return prog;
}

GLuint hegl_create_texture(const char *path) {
	GLuint tex;

	int width, height, channels;
	char *img = he_asset_load_texture(path, &width, &height, &channels);
	GLenum format;
	if (channels == 4)
		format = GL_RGBA;
	else if (channels == 3)
		format = GL_RGB;

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, format, GL_UNSIGNED_BYTE, img);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	he_asset_free_image(img);

	return tex;
}


///


typedef struct FinList {
	func_fin *array;
	size_t count; // used count
	size_t size; // total mem size
} FinList;

static FinList finlist = {0};


void hegl_init() {
	glewExperimental = GL_TRUE;

	GLenum glewError = glewInit();

	if (glewError != GLEW_OK) {
		abort();
	}

	printf("glew version: %s\n", glewGetString(GLEW_VERSION));


	int nmaxvertexattribs;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nmaxvertexattribs);

	printf("gl max vertex attribs:%d\n", nmaxvertexattribs);


	glEnable(GL_DEPTH_TEST);


	finlist.size = 16;
	finlist.array = malloc(sizeof(func_fin) * finlist.size);
}

void hegl_toggle_wireframe(int mode) {
	if (mode)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}




void hegl_registfin(func_fin a_fin) {
	if (finlist.count >= finlist.size) {
		// err
		return;
	}

	finlist.array[finlist.count] = a_fin;
	finlist.count++;
}

void hegl_fin() {
	for (int i = 0; i < finlist.count; i++) {
		func_fin func = finlist.array[i];
		func();
	}

	free(finlist.array);
	memset(&finlist, 0, sizeof(FinList));
}