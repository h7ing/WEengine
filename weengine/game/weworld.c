#include "weworld.h"
#include <string.h>
#include "weutil.h"
#include "weobject.h"

static struct WEworld the_world;

struct WEworld *we_world = &the_world;


struct WEworld *we_get_the_world() {
	return we_world;
}


void WEworld_init(struct WEworld *w) {
	memset(w, 0, sizeof(*w));

	w->objects = calloc(1, sizeof(*w->objects));
}

void WEworld_clear(struct WEworld *w) {
	WEobject_list_clear(w->objects);
	WE_FREE_AND_NULL(w->objects);
}
