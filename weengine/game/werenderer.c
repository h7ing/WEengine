#include "werenderer.h"
#include <GL/glew.h>
#include "weworld.h"
#include "weutil.h"
#include "wegeometry.h"
#include "wemesh.h"

/* test funcs */
void WErenderer_draw_test_geometry(struct WErenderer *r);


void WErenderer_init_opengl(struct WErenderer *r) {
    we_log("init opengl begin.\n");

    const GLubyte* glversion = glGetString(GL_VERSION);
    we_log("opengl version:%s\n", glversion);

	int width, height;
	WEworld_get_window_size(we_world, &width, &height);
    glViewport(0, 0, width, height);

    glewExperimental = GL_TRUE;

	GLenum glewError = glewInit();

	if (glewError != GLEW_OK) {
		abort();
	}

	we_log("glew version: %s\n", glewGetString(GLEW_VERSION));

	int the_glmax_vertex_attribs;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &the_glmax_vertex_attribs);

	we_log("gl max vertex attribs:%d\n", the_glmax_vertex_attribs);


	glEnable(GL_DEPTH_TEST);

    we_log("init opengl end.\n");
}

void WErenderer_toggle_wireframe(struct WErenderer *r) {
	r->mode.wireframe = !r->mode.wireframe;
	if (r->mode.wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void WErenderer_draw(struct WErenderer *r) {
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT); // use the clear color

	WErenderer_draw_test_geometry(r);
}

void WErenderer_draw_test_geometry(struct WErenderer *r) {

	struct WEgeometry *geo = we_world->the_test_geometry;

	if (!geo) return;

	for (int i = 0; i < geo->mesh_cnt; i++) {
		struct WEmesh *mesh = &(geo->meshes[i]);

		glUseProgram(mesh->shader);
		glBindVertexArray(mesh->vao);
		glDrawElements(GL_TRIANGLES, mesh->indices_cnt, GL_UNSIGNED_INT, mesh->indices);
	}
}