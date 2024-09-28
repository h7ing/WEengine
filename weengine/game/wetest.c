#include "wetest.h"

#include <string.h>
#include <cglm.h>
#include "wearray.h"
#include "weutil.h"


//---

typedef struct WTid {
    char *key;
    // char *guid;
} WTid;



//---

struct WTcomponent;

typedef struct WTnode {
    WTid id;
    char *name;

    struct WTnode *parent;
    struct WEarray children;

    struct WEarray components;

} WTnode;

WTnode *WTnode_new();
void WTnode_delete(WTnode *node);
void WTnode_add_com(struct WTnode *n, struct WTcomponent *c);
void WTnode_add_component(struct WTnode *n, const char *typename);


typedef struct WTscene {
    WTid id;
    char *name;
    WTnode *root_node;

    struct WEarray nodes;
} WTscene;

WTscene *WTscene_new();
WTscene *WTscene_create_engine_scene();

void WTscene_delete(WTscene *s);


typedef struct WTworld {
    WTid id;
    struct WEarray *scenes;
} WTworld;

void WTworld_delete(WTworld *world);

WTnode *we_create_world_camera_node();

//---

struct WTcomponent {
    const char *typename;
    void (*del_fn)(void *);
    WTnode *owner;
    void *body;
};

void WTcomponent_delete(struct WTcomponent *c) {
    c->del_fn(c->body);
    free(c);
}

struct WTcomponent_type_t {
    const char *typename;
    struct WTcomponent *(* new_fn)(WTnode *);
};

//---

typedef struct WTcamera {
    WTid id;

    mat4 projection_matrix;
} WTcamera;

WTcamera *WTcamera_new(WTnode *node);
void WTcamera_delete(void *cam);

struct WTcomponent *WTcamera_component_new(WTnode *n) {
    struct WTcomponent *c = malloc(sizeof(struct WTcomponent));
    struct WTcamera *cam = malloc(sizeof(struct WTcamera));

    c->typename = "camera";
    c->body = cam;
    c->del_fn = WTcamera_delete;
    c->owner = n;

    return c;
}

const struct WTcomponent_type_t we_component_type_list[] = {
    {"camera", WTcamera_component_new},
    {NULL, NULL}
};

//---

static WTworld *we_world;


//--- 

static void we_init_world();

void we_init_world() {
    WTworld *world = (WTworld *)malloc(sizeof(WTworld));

    world->scenes = WEarray_new((WEdelete_fn_t)WTscene_delete);
    WEarray_add(world->scenes, WTscene_create_engine_scene());

    we_world = world;
}

void WTworld_delete(WTworld *world) {

    WEarray_delete(world->scenes);

    free(world);

    we_log("world.destroy finish.\n");
}

WTnode *we_create_world_camera_node() {
    WTnode *node = WTnode_new();
    // WTcamera *cam = WTcamera_new(node);
    // WTcamera_component_new(node);

    WTnode_add_component(node, "camera");

    // WTnode_add_com(node, cam->component);

    return node;
}

void WTscene_delete(WTscene *s) {
    we_log("scene.destroy:%s\n", s->name);
    WE_FREE_AND_NULL(s->name);
    free(s);
}


WTcamera *WTcamera_new(WTnode *node) {
    WTcamera *cam = malloc(sizeof(WTcamera));
    // cam->component = (struct WTcomponent){ .typename="camera", .del_fn=WTcamera_delete, .body=cam, .owner=node };
    // WTnode_add_com(node, &cam->component);

    return cam;
}

void WTcamera_delete(void *cam) {
    free(cam);
}

WTnode *WTnode_new() {
    WTnode *node = (WTnode *)malloc(sizeof(WTnode));
    WEarray_init(&node->components, (WEdelete_fn_t)WTcomponent_delete);
    return node;
}

void WTnode_delete(WTnode *n) {
    free(n);
}

void WTnode_add_com(struct WTnode *n, struct WTcomponent *c) {
    WEarray_add(&n->components, c);
}

void WTnode_add_component(struct WTnode *n, const char *typename) {
    const struct WTcomponent_type_t *comtype;

    for (comtype = we_component_type_list; comtype->new_fn; comtype++) {
        if (strcmp(comtype->typename, typename) == 0) {
            struct WTcomponent *c = comtype->new_fn(n);
            WEarray_add(&n->components, c);
            return;
        }
    }
}

WTscene *WTscene_new() {
    WTscene *scene = calloc(1, sizeof(WTscene));
    return scene;
}

WTscene *WTscene_create_engine_scene() {
    WTscene *scene = WTscene_new();
    scene->name = WEstring_new("WTscene");

    WEarray_init(&scene->nodes, (WEdelete_fn_t)WTnode_delete);
    WEarray_add(&scene->nodes, we_create_world_camera_node());

    we_log("WTscene_create_engine_scene:%s, finish.\n", scene->name);
    return scene;
}