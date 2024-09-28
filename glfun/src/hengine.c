#include "hengine.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cgltf_write.h>
#include <cgltf.h>

#include "stb_image.h"


void he_init() {
	printf("hoka engine init\n");

	stbi_set_flip_vertically_on_load(1);
}

void he_fin() {
	printf("hoka engine finish\n");
}


char * he_asset_gen_path(const char *filename) {
	#ifdef _MSC_VER
	const char *prepath = "../../assets/";
	#else
	const char *prepath = "../assets/";
	#endif
	size_t pathsize = strlen(prepath) + strlen(filename) + 1;
	char *path = (char *)malloc(pathsize);
	snprintf(path, pathsize, "%s%s", prepath, filename);
	return path;
}

char * he_asset_load_content(const char *relativepath) {
	char *path = he_asset_gen_path(relativepath);
	FILE *fp = fopen(path, "rb");
	free(path);

	if (fp == NULL) {
		printf("file not loaded:%s\n", relativepath);
		return NULL;
	}

	fseek(fp,0,SEEK_END);
	fpos_t filesize;
	fgetpos(fp, &filesize);
	fseek(fp,0,SEEK_SET);

	char *content = malloc(filesize+1);
	content[filesize] = '\0';
	fread(content, sizeof(char), filesize, fp);
	fclose(fp);

	return content;
}


unsigned char * he_asset_load_texture(const char *relativepath, int *width, int *height, int *channels) {
	char *path = he_asset_gen_path(relativepath);

	// int width, height, nrChannels;
	unsigned char *data = stbi_load(path, width, height, channels, 0);

	free(path);

	return data;
}

void he_asset_free_image(void *image) {
	stbi_image_free(image);
}


static float he_world_delta;

void he_set_delta(float delta) {
	he_world_delta = delta;
}

float he_get_delta() {
	return he_world_delta;
}


static int window_width, window_height;
static float window_aspect;

void he_set_window_size(int w, int h) {
	window_width = w;
	window_height = h;
	window_aspect = (float)w / (float)h;
}

float he_get_window_aspect() {
	return window_aspect;
}


static HEInput input;

HEInput *he_get_input() {
	return &input;
}

void he_reset_input() {
	memset(&input, 0, sizeof(input));
}