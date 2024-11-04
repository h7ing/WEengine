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
};

extern struct WEworld *we_world;

struct WEworld *we_get_the_world();

void WEworld_init(struct WEworld *world);

void WEworld_clear(struct WEworld *world);

void WEworld_get_window_size(struct WEworld *w, int *width, int *height);

#endif