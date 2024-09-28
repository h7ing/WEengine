#ifndef WEOBJECT_H
#define WEOBJECT_H

#include "weid.h"

struct WEcomponent;

struct WErefcount {
	int count;
};

struct WEobject {
	struct WEid id;
	struct WErefcount ref;

	struct WEcomponent *components;
};

struct WEobject_list {
	struct WEobject *item;
	struct WEobject_list *next;
};

struct WEcomponent {
	const char *name;
	void *body;
	struct WEobject *owner;
	struct WEcomponent *next;
};

struct WEobject *WEobject_new();
void WEobject_free(struct WEobject *obj);
void WEobject_retain(struct WEobject *obj);
int WEobject_release(struct WEobject *obj);


void WEobject_list_clear(struct WEobject_list *objs);

#endif