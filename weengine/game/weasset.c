#include "weasset.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

char *WEasset_gen_fullpath(char *path) {
  #ifdef _MSC_VER
	const char *prepath = "../../assets/";
  #else
	const char *prepath = "../assets/";
  #endif
	size_t pathsize = strlen(prepath) + strlen(path) + 1;
	char *fullpath = (char *)malloc(pathsize);
	snprintf(fullpath, pathsize, "%s%s", prepath, path);
	return fullpath;
}

char *WEasset_load_content(const char *relativepath) {
	char *path = WEasset_gen_fullpath(relativepath);
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
