#include "hoka.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <GL/glew.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>

#include <cglm.h>

#include "weengine.h"
#include "wecamera.h"


//---

extern struct WEworld *we_world;

static SDL_Window *the_window = NULL;
static int window_flags = SDL_WINDOW_OPENGL;

static void we_init_window();
static void we_destroy_window();


void we_init() {

    we_log("we init\n");

    we_init_window();

    WEworld_init(we_world, the_window);
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

    float curdelta = 0;
    float fixdelta = 1.0/30.0f;
    float targetdelta = fixdelta;
    clock_t lastclock = clock();
    clock_t curclock = lastclock;

    while (running) {

        // frame begin

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_EVENT_KEY_DOWN:
                if (event.key.keysym.sym == SDLK_w) {
                    if (event.key.keysym.mod == SDL_KMOD_LSHIFT) {
                        // shift+w = toggle wireframe
                        WErenderer_toggle_wireframe(we_world->the_renderer);
                    }
                }
                else if (event.key.keysym.sym == SDLK_p) {
                    if (event.key.keysym.mod == SDL_KMOD_LSHIFT) {
                        // todo: shift+p = anim switch camera projection to perspective
                        WEcamera_set_projection_anim(we_world->the_camera, 0);
                        WEworld_append_task(we_world, WEcamera_anim_switch_projection);
                    }
                }
                else if (event.key.keysym.sym == SDLK_o) {
                    if (event.key.keysym.mod == SDL_KMOD_LSHIFT) {
                        // todo: shift+o = anim switch camera projection to ortho
                        WEcamera_set_projection_anim(we_world->the_camera, 1);
                        WEworld_append_task(we_world, WEcamera_anim_switch_projection);
                    }
                }
                break;
            case SDL_EVENT_KEY_UP:
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    running = 0;
                }
                break;
            case SDL_EVENT_QUIT:
                running = 0;
                break;
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                break;
            case SDL_EVENT_MOUSE_WHEEL:
                break;
            case SDL_EVENT_MOUSE_MOTION:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    // float x = event.motion.xrel;
                    // float y = event.motion.yrel;
                    // // printf("Mouse left button is down and moved to (%f, %f)\n", x, y);

                    // printf("\033[0;0H");
                    // printf("Mouse left button is down and moved to (%f, %f)\n", x, y);
                }
                break;
            }
        }

        curclock = clock();
        curdelta += (double)(curclock - lastclock) / CLOCKS_PER_SEC;
        lastclock = curclock;

        if (curdelta >= targetdelta) {
            // printf("\033[0;0H"); // 光标移动到0行0列
            // printf("\033[K"); // 清除光标行后的内容
            // printf("delta:%f\n", curdelta);

            we_world->curdelta = curdelta;

            WEworld_execute_tasks(we_world);

            WErenderer_draw(we_world->the_renderer);

            curdelta -= targetdelta;

            SDL_GL_SwapWindow(the_window);
        }

        // frame end
    }
}

