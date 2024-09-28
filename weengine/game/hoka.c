#include "hoka.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <GL/glew.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>

#include <cglm.h>

#include "weengine.h"


//---

// todo: refactor to window struct
static SDL_Window* window = NULL;
static int window_flags = SDL_WINDOW_OPENGL;

static void we_init_window();
static void we_destroy_window();

static void we_init_opengl();

static void we_render_opengl();


void we_init() {

    we_log("we init\n");

    we_init_window();

    we_init_opengl();

    WEworld_init(we_world);

    WEscene_load_dummy(we_world);
}

void we_startgame() {
    we_mainloop();
}

void we_fin() {

    we_destroy_window();

    we_log("we fin");
}

// --- private

void we_init_window() {
    int err = SDL_InitSubSystem(SDL_INIT_VIDEO);
    if (err == -1) {
        we_log("err: init video\n");
        abort();
    }

    window = SDL_CreateWindow("hoka", 800, 600, window_flags);

    SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    // SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    // SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    // SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // mac

    SDL_GLContext context = SDL_GL_CreateContext(window);

    if (context == NULL) {
        we_log("Could not create SDL context: %s\n", SDL_GetError());
        abort();
    }

    SDL_GL_SetSwapInterval(1);
}

void we_destroy_window() {
    SDL_DestroyWindow(window);
}

void we_init_opengl() {
    we_log("init opengl begin.\n");

    const GLubyte* glversion = glGetString(GL_VERSION);
    we_log("opengl version:%s\n", glversion);

    glViewport(0, 0, 800, 600);

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

void we_mainloop() {
    SDL_Event event;
    int running = 1;
    int wireframemode = 0;
    int drawmode = 1;



    while (running) {

        // frame begin

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_EVENT_KEY_DOWN:
                // if (event.key.keysym.sym == SDLK_w) {
                //     wireframemode = !wireframemode;
                //     hegl_toggle_wireframe(wireframemode);
                // }
                // else if (event.key.keysym.sym == SDLK_1) {
                //     drawmode = 1;
                //     hegl_prepare_triangle();
                // }
                break;
            case SDL_EVENT_KEY_UP:
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    running = 0;
                }
                break;
            case SDL_EVENT_QUIT:
                running = 0;
                break;
            }
        }

        we_render_opengl();

        SDL_GL_SwapWindow(window);

        // frame end
    }
}

void we_render_opengl() {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT); // use the clear color
}

