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


void WEworld_init(struct WEworld *w, struct SDL_Window *window) {
	memset(w, 0, sizeof(*w));

	w->the_window = window;

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
	SDL_GetWindowSizeInPixels(w->the_window, width, height);
}

void WEworld_execute_tasks(struct WEworld *w) {
	struct WEtask_head *task = w->tasks;
	struct WEtask_head *prevtask = task;

	while (task) {
		int result = (task->fn)(w);
		if (result) {
			if (prevtask != task) {
				prevtask->next = task->next;
			} else {
				// 表头
				prevtask = task->next;
				w->tasks = prevtask;
			}

			free(task);

			if (prevtask)
				task = prevtask->next;
			else
				task = NULL;
		} else {
			prevtask = task;
			task = task->next;
		}
	}
}

void WEworld_append_task(struct WEworld *w, WEtask_t task) {

	struct WEtask_head *item = malloc(sizeof(*item));
	item->fn = task;
	item->next = NULL;

	struct WEtask_head *tail = w->tasks;

	if (!tail) {
		w->tasks = item;
		return;
	}

	while (tail->next) {
		tail = tail->next;
	}
	tail->next = item;
}
