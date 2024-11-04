#ifndef WERENDERER_H
#define WERENDERER_H

struct WErenderer {
	struct {
		int wireframe : 1;
	} mode;
};

void WErenderer_init_opengl(struct WErenderer *r);

void WErenderer_toggle_wireframe(struct WErenderer *r);

void WErenderer_draw(struct WErenderer *r);

#endif