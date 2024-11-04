#include "wenode.h"
#include <string.h>
#include "wecomponent.h"

struct WEcomponent *WEnode_get_comp(struct WEnode *n, const char *name) {
	for (struct WEcomponent *c = n->comps; c; c = c->next) {
		if (strcmp(c->name, name) == 0)
			return c;
	}

	return NULL;
}

struct WEnode *WEnode_new() {
	struct WEnode *n = calloc(1, sizeof(*n));
	glm_vec3_zero(n->transform.position);
	glm_quat_identity(n->transform.rotation);
	glm_vec3_one(n->transform.scale);
	return n;
}

void WEnode_list_add(struct WEnode *list, struct WEnode *n) {
	while (list->next)
		list = list->next;
	list->next = n;
}