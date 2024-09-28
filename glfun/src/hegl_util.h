#pragma once
#include <GL/glew.h>


///

void hegl_check_shader_status(GLuint id, GLenum st);

void hegl_check_shader_compile_error(GLuint id);
void hegl_check_shader_link_error(GLuint id);

GLuint hegl_create_shader_program(const char *vert, const char *frag);

// path: relative to assets/
GLuint hegl_create_texture(const char *path);


///

void hegl_init();

void hegl_toggle_wireframe(int mode);

void hegl_fin();


typedef void (*func_fin) ();

void hegl_registfin(func_fin a_fin);