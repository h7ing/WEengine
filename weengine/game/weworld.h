#ifndef WEWORLD_H
#define WEWORLD_H

struct WEobject_list;

struct WEworld {
	struct WEobject_list *objects;
};

extern struct WEworld *we_world;

struct WEworld *we_get_the_world();

void WEworld_init(struct WEworld *world);

void WEworld_clear(struct WEworld *world);

#endif