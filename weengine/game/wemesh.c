#include "wemesh.h"
#include <stdlib.h>
#include "cgltf.h"
#include "weasset.h"

struct WEmesh *WEmesh_new() {
	struct WEmesh *m = malloc(sizeof(*m));
	m->com.name = "mesh";
	m->com.next = NULL;
	return m;
}

void WEmesh_init_gltf(struct WEmesh *m, struct cgltf_data *d) {
	/* todo */
}