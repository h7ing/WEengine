#include "weworld.h"
#include <string.h>
#include <SDL3/SDL.h>
#include "weutil.h"
#include "weobject.h"
#include "wescene.h"
#include "werenderer.h"

static struct WEworld the_world;

struct WEworld *we_world = &the_world;


struct WEworld *we_get_the_world() {
	return we_world;
}


void WEworld_init(struct WEworld *w) {
	memset(w, 0, sizeof(*w));

	// w->objects = calloc(1, sizeof(*w->objects));

	w->the_renderer = calloc(1, sizeof(*w->the_renderer));
	WErenderer_init_opengl(w->the_renderer);


	WEscene_load_dummy(w);
}

void WEworld_clear(struct WEworld *w) {
	// WEobject_list_clear(w->objects);
	// WE_FREE_AND_NULL(w->objects);

	WE_FREE_AND_NULL(w->the_scene);

	WE_FREE_AND_NULL(w->the_renderer);
}

void WEworld_get_window_size(struct WEworld *w, int *width, int *height) {
	SDL_GetWindowSize(w->the_window, width, height);
}
