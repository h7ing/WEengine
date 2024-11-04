#include "wescene.h"
#include <stdlib.h>
#include "weworld.h"
#include "wecamera.h"
#include "wenode.h"
#include "weutil.h"
#include "wemesh.h"
#include "wegeometry.h"

void WEscene_load_dummy(struct WEworld *w) {

	struct WEscene *s = calloc(1, sizeof(*s));
	w->the_scene = s;

	/* root node */
	do {
		struct WEnode *n = WEnode_new();
		n->scene = s;
		n->parent = NULL;
		n->next = NULL;

		s->nodes = n;
	} while (0);

	/* camera node */
	do {
		struct WEnode *cn = WEnode_new();
		cn->scene = s;
		cn->next = NULL;

		struct WEcamera *cam = WEcamera_new();
		cam->node = cn;
		WEcamera_init_lens_normal_35(cam);

		s->camera = cam;

		w->the_camera = cam;

		WEnode_list_add(s->nodes, cn);
	} while(0);


	/* test geometry */
	do {

		struct WEgeometry *geo = WEgeometry_loadfromfile("models/triangle.gltf");
		if (geo) {

			struct WEnode *n = WEnode_new();
			n->scene = s;
			n->next = NULL;

			WEnode_list_add(s->nodes, n);

		}

		w->the_test_geometry = geo;

	} while (0);
	
}

void WEscene_clear(struct WEscene *s) {
	do {
		WE_FREE_AND_NULL(s->camera);
	} while (0);

	for (struct WEnode *n = s->nodes; n; ) {
		struct WEnode *p = n->next;
		free(n);
		n = p;
	}
}
