#ifndef WENODE_H
#define WENODE_H

#include "weobject.h"
#include "wetransform.h"

struct WEscene;

struct WEcomponent;

struct WEgeometry;

struct WEnode {
	struct WEobject obj;
	struct WEtransform transform;

	struct WEscene *scene;

	struct WEnode *parent;
	struct WEnode *next;

	struct WEcomponent *comps;

	struct WEgeometry *geometry;
};

struct WEcomponent *WEnode_get_comp(struct WEnode *n, const char *name);

struct WEnode *WEnode_new();

void WEnode_list_add(struct WEnode *list, struct WEnode *n);

#endif