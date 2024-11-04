#ifndef WESCENE_H
#define WESCENE_H

#include "weobject.h"

struct WEworld;

struct WEnode;

struct WEcamera;

struct WEscene {
	int order;
	struct WEobject obj;

	struct WEnode *nodes;

	struct WEcamera *camera;
};

void WEscene_load_dummy(struct WEworld *w);

#endif
