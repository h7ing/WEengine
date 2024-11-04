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