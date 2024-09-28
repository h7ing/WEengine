#ifndef WESCENE_H
#define WESCENE_H

struct WEworld;

struct WEobject;

struct WEscene {
	int order;
	struct WEobject *owner;
};

void WEscene_load_dummy(struct WEworld *w);

#endif
