#ifndef WEMATERIAL_H
#define WEMATERIAL_H

struct WEmaterial {
	unsigned int shader;

    int texture;
};

struct WEmaterial *WEmaterial_get_default();

unsigned int WEmaterial_build_shader(const char *filename, unsigned int type);
unsigned int WEmaterial_create_shader_prog(char *vertpath, char *fragpath);

#endif