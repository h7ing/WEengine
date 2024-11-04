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

extern struct WEworld *we_world;

static SDL_Window *the_window = NULL;
static int window_flags = SDL_WINDOW_OPENGL;

static void we_init_window();
static void we_destroy_window();


void we_init() {

    we_log("we init\n");

    we_init_window();

    WEworld_init(we_world);
    we_world->the_window = the_window;
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

    the_window = SDL_CreateWindow("hoka", 800, 600, window_flags);

    SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    // SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    // SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    // SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // mac

    SDL_GLContext context = SDL_GL_CreateContext(the_window);

    if (context == NULL) {
        we_log("Could not create SDL context: %s\n", SDL_GetError());
        abort();
    }

    SDL_GL_SetSwapInterval(1);
}

void we_destroy_window() {
    SDL_DestroyWindow(the_window);
}

void we_mainloop() {
    SDL_Event event;
    int running = 1;

    while (running) {

        // frame begin

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_EVENT_KEY_DOWN:
                if (event.key.keysym.sym == SDLK_w) {
                    if (event.key.keysym.mod == SDL_KMOD_LSHIFT) {
                        WErenderer_toggle_wireframe(we_world->the_renderer);
                    }
                }
                // else if (event.key.keysym.sym == SDLK_1) {
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

        WErenderer_draw(we_world->the_renderer);

        SDL_GL_SwapWindow(the_window);

        // frame end
    }
}

