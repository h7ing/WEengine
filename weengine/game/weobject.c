#include "weobject.h"
#include "weutil.h"

static void WEcomponent_clear(struct WEcomponent *cs) {
	
}

struct WEobject *WEobject_new() {
	struct WEobject *obj = calloc(1, sizeof(*obj));
	obj->ref.count = 1;
	return obj;
}

static void WEobject_clear(struct WEobject *obj) {
	WEcomponent_clear(obj->components);

	struct WEcomponent *cs = obj->components;
	while (cs) {
		struct WEcomponent *c = cs->next;
		free(cs);
		cs = c;
	}
	obj->components = NULL;
}

void WEobject_list_clear(struct WEobject_list *objs) {
	while (objs) {
		struct WEobject_list *p = objs;
		objs = objs->next;

		WEobject_release(p->item);
		free(p);
	}
}

int WEobject_release(struct WEobject *obj) {
	obj->ref.count--;
	if (obj->ref.count <= 0) {
		WEobject_free(obj);
		return 1;
	}

	return 0;
}

void WEobject_retain(struct WEobject *obj) {
	obj->ref.count++;
}

void WEobject_free(struct WEobject *obj) {
	WEobject_clear(obj);
	free(obj);
}
