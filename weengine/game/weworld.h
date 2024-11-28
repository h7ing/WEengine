#ifndef WEWORLD_H
#define WEWORLD_H

struct WEobject_list;

struct WEscene;

struct WEcamera;

struct WErenderer;

struct WEgeometry;

struct SDL_Window;


struct WEworld {
	// struct WEobject_list *objects;

	struct SDL_Window *the_window;

	struct WEscene *the_scene;

	struct WEcamera *the_camera;

	struct WErenderer *the_renderer;


	/* test */
	struct WEgeometry *the_test_geometry;


	struct WEtask_head *tasks;


	float curdelta;
};

extern struct WEworld *we_world;

struct WEworld *we_get_the_world();

void WEworld_init(struct WEworld *world, struct SDL_Window *);

void WEworld_clear(struct WEworld *world);

void WEworld_get_window_size(struct WEworld *w, int *width, int *height);

void WEworld_execute_tasks(struct WEworld *w);


typedef int (*WEtask_t)(struct WEworld *);

struct WEtask_head {
	WEtask_t fn;
	struct WEtask_head *next;
};

void WEworld_append_task(struct WEworld *w, WEtask_t task);

#endif