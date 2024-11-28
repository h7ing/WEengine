#include "werenderer.h"
#include <GL/glew.h>
#include "weworld.h"
#include "weutil.h"
#include "wegeometry.h"
#include "wemesh.h"
#include "wematerial.h"
#include "wecamera.h"

/* test funcs */
void WErenderer_draw_test_geometry(struct WErenderer *r);


void WErenderer_init_opengl(struct WErenderer *r) {
    we_log("init opengl begin.\n");

    const GLubyte* glversion = glGetString(GL_VERSION);
    we_log("opengl version:%s\n", glversion);

	int width, height;
	WEworld_get_window_size(we_world, &width, &height);
	// width = 800;
	// height = 600;
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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // use the clear color

	WErenderer_draw_test_geometry(r);
}

static GLuint vao3;

void init_03_vertex() {
	glGenVertexArrays(1, &vao3);
	glBindVertexArray(vao3);

	float vertices[] = {
		-0.75f, 0, 0,
		-0.25f, 0.5f, 0,
		-0.25f, -0.5f, 0,

		0.75f, 0, 0,
		0.25f, 0.5f, 0,
		0.25f, -0.5f, 0,
	};

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), NULL);
	glEnableVertexAttribArray(0);


	GLuint indices[] = {
		0,1,2,
		3,4,5
	};
	GLuint ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


	glBindVertexArray(0);
}

void WErenderer_draw_test_triangle() {
	if (!vao3) {
		init_03_vertex();
	}

	do
	{
		struct WEmaterial *themat = WEmaterial_get_default();
		glUseProgram(themat->shader);
		glBindVertexArray(vao3);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	} while (0);
}

void WErender_draw_test_allmeshes(struct WEgeometry *geo) {
	for (int i = 0; i < geo->mesh_cnt; i++) {
		struct WEmesh *mesh = &(geo->meshes[i]);
		for (int j = 0; j < mesh->primitives_cnt; j++) {
			struct WEmeshprimitive *meshprim = &(mesh->primitives[j]);
			glUseProgram(meshprim->material->shader);
			glBindVertexArray(meshprim->vao);
			glDrawElements(GL_TRIANGLES, meshprim->indices_cnt, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}
	}
}

void WErenderer_draw_node(struct WEgeometrynode *node) {
	struct WEmesh *mesh = node->mesh;

	for (int j = 0; j < mesh->primitives_cnt; j++) {
		struct WEmeshprimitive *meshprim = &(mesh->primitives[j]);

		unsigned int shader = meshprim->material->shader;
		glUseProgram(shader);

		// mvp
		unsigned int model = glGetUniformLocation(shader, "model");
		glUniformMatrix4fv(model, 1, GL_FALSE, &node->transform.matrix[0][0]);

		unsigned int view = glGetUniformLocation(shader, "view");
		glUniformMatrix4fv(view, 1, GL_FALSE, &we_world->the_camera->view[0][0]);

		unsigned int projection = glGetUniformLocation(shader, "projection");
		glUniformMatrix4fv(projection, 1, GL_FALSE, &we_world->the_camera->projection[0][0]);


		glBindVertexArray(meshprim->vao);

		if (meshprim->indices_cnt > 0)
			glDrawElements(GL_TRIANGLES, meshprim->indices_cnt, GL_UNSIGNED_INT, 0);
		else
			glDrawArrays(GL_TRIANGLES, 0, meshprim->vertices_cnt);

		glBindVertexArray(0);
	}
}

void WErenderer_draw_test_geometry(struct WErenderer *r) {

	struct WEgeometry *geo = we_world->the_test_geometry;

	if (!geo) return;

	// WErender_draw_test_allmeshes(geo);

	struct WEgeometrynode *node = geo->nodes;

	while (node) {

		if (node->mesh) {
			WErenderer_draw_node(node);
		}

		node = node->next;
	}

}