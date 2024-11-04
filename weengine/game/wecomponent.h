#ifndef WECOMPONENT_H
#define WECOMPONENT_H

struct WEcomponent {
	const char *name;
	struct WEcomponent *next;
};

#endif