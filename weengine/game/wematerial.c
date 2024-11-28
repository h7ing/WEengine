#include "wematerial.h"
#include <stdlib.h>
#include <stdio.h>
#include <GL/glew.h>
#include "weasset.h"

static struct WEmaterial *default_mat = NULL;

struct WEmaterial *WEmaterial_get_default() {
    if (!default_mat) {
        default_mat = malloc(sizeof(*default_mat));

        default_mat->shader = WEmaterial_create_shader_prog("shaders/default.vert", "shaders/default.frag");
    }

    return default_mat;
}

unsigned int WEmaterial_build_shader(const char *filename, unsigned int type) {
	char *content = WEasset_load_content(filename);
	unsigned int shader = glCreateShader(type);
	glShaderSource(shader, 1, &content, NULL);
	glCompileShader(shader);
	free(content);

	int status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

	if (!status) {

		GLint logSize;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logSize);

		GLchar *infoLog = malloc(logSize);
		glGetShaderInfoLog(shader, logSize, NULL, infoLog);
		printf("compile shader err:%s\n", infoLog);
		free(infoLog);

        glDeleteShader(shader);
        shader = 0;
	}

	return shader;
}

unsigned int WEmaterial_create_shader_prog(char *vertpath, char *fragpath) {
	unsigned int vs = WEmaterial_build_shader(vertpath, GL_VERTEX_SHADER);
    if (!vs) return 0;

	unsigned int fs = WEmaterial_build_shader(fragpath, GL_FRAGMENT_SHADER);
    if (!fs) return 0;

	unsigned int prog = glCreateProgram();
	glAttachShader(prog, vs);
	glAttachShader(prog, fs);
	glLinkProgram(prog);
	glDeleteShader(vs);
	glDeleteShader(fs);

    // check link status
    int status;
	glGetProgramiv(prog, GL_LINK_STATUS, &status);

	if (!status) {

		GLint logSize;
		glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &logSize);

		GLchar *infoLog = malloc(logSize);
		glGetProgramInfoLog(prog, logSize, NULL, infoLog);
		printf("link shader err:%s\n", infoLog);
		free(infoLog);

        glDeleteProgram(prog);

        return 0;
	}

	return prog;
}


