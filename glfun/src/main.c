#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <GL/glew.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#include "hengine.h"
#include "learnopengl.h"
#include "hegl.h"

static SDL_Window* window = NULL;
static int window_flags = SDL_WINDOW_OPENGL;

int main(int argc, char** argv) {

    he_init();


    // init the window

    int err = SDL_InitSubSystem(SDL_INIT_VIDEO);
    if (err == -1) {
        printf("err: init video\n");
        abort();
    }

    float displayscale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
    // if (displayscale > 1.0f + FLT_EPSILON)
        window_flags |= SDL_WINDOW_HIGH_PIXEL_DENSITY;

    int displayw = 800 * displayscale;
    int displayh = 600 * displayscale;

    window = SDL_CreateWindow("glfun", displayw, displayh, window_flags);

    SDL_GetWindowSizeInPixels(window, &displayw, &displayh);

    he_set_window_size(displayw, displayh);

    SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    // SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    // SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    // SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // mac

    // SDL_SetWindowMouseGrab(window, SDL_TRUE);
    // SDL_HideCursor();

    SDL_GLContext context = SDL_GL_CreateContext(window);

    if (context == NULL) {
        printf("Could not create SDL context: %s\n", SDL_GetError());
        abort();
    }

    const GLubyte* glversion = glGetString(GL_VERSION);
    printf("opengl version:%s\n", glversion);

    SDL_GL_SetSwapInterval(1);

    glViewport(0, 0, displayw, displayh);

    hegl_init();

    Uint64 startms = SDL_GetTicks();
    Uint64 lastms = startms;
    Uint64 curms = startms;
    float delta;

    // the main loop

    SDL_Event event;
    int running = 1;
    int wireframemode = 0;
    int drawmode = 1;
    int submode = 0;
    int op_rotate = 0;
    int op_translate = 0;
    int op_translate_x = 0;
    int op_translate_y = 0;
    int op_translate_z = 0;
    int op_forward = 0;
    int op_right = 0;
    float op_rotate_x;
    float op_rotate_y;
    float op_zoom;
    HEInput *input = he_get_input();


    while (running) {

        // frame begin
        curms = SDL_GetTicks();
        delta = (float)(curms - lastms) / 1000.0f;
        he_set_delta(delta);
        lastms = curms;

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_EVENT_MOUSE_MOTION:
                op_rotate_x = event.motion.xrel;
                op_rotate_y = event.motion.yrel;
                break;
            case SDL_EVENT_MOUSE_WHEEL:
                op_zoom = event.wheel.y;
                break;
            case SDL_EVENT_KEY_DOWN:
                if (event.key.keysym.sym == SDLK_f) {
                    wireframemode = !wireframemode;
                    hegl_toggle_wireframe(wireframemode);
                }
                else if (event.key.keysym.sym == SDLK_1) {
                    if (event.key.keysym.mod == SDL_KMOD_LSHIFT)
                        drawmode = 11;
                    else
                        drawmode = 1;
                }
                else if (event.key.keysym.sym == SDLK_2) {
                    if (event.key.keysym.mod == SDL_KMOD_LSHIFT)
                        drawmode = 12;
                    else
                        drawmode = 2;
                }
                else if (event.key.keysym.sym == SDLK_3) {
                    drawmode = 3;
                }
                else if (event.key.keysym.sym == SDLK_4) {
                    drawmode = 4;
                    submode++;
                    if (submode > 3) submode = 1;
                }
                else if (event.key.keysym.sym == SDLK_5) {
                    drawmode = 5;
                    submode++;
                    if (submode > 2) submode = 1;
                }
                else if (event.key.keysym.sym == SDLK_6) {
                    drawmode = 6;
                    play_06_toggle_mode();
                }
                else if (event.key.keysym.sym == SDLK_7) {
                    drawmode = 7;
                }
                else if (event.key.keysym.sym == SDLK_8) {
                    drawmode = 8;
                }
                else if (event.key.keysym.sym == SDLK_9) {
                    drawmode = 9;
                }
                else if (event.key.keysym.sym == SDLK_0) {
                    drawmode = 10;
                    submode++;
                    if (submode > 5) submode = 1;
                }
                else if (event.key.keysym.sym == SDLK_w) {
                    op_rotate_y = -0.25f;
                    input->axisy = 1;
                }
                else if (event.key.keysym.sym == SDLK_s) {
                    op_rotate_y = 0.25f;
                    input->axisy = -1;
                }
                else if (event.key.keysym.sym == SDLK_a) {
                    op_rotate_x = -0.25f;
                    input->axisx = -1;
                }
                else if (event.key.keysym.sym == SDLK_d) {
                    op_rotate_x = 0.25f;
                    input->axisx = 1;
                }

                else if (event.key.keysym.sym == SDLK_UP) {
                    op_translate = 1;
                    op_translate_y = 1;
                    input->y = 1;
                }
                else if (event.key.keysym.sym == SDLK_DOWN) {
                    op_translate = -1;
                    op_translate_y = -1;
                    input->y = -1;
                }
                else if (event.key.keysym.sym == SDLK_LEFT) {
                    op_translate = -2;
                    op_translate_x = -1;
                    input->x = -1;
                }
                else if (event.key.keysym.sym == SDLK_RIGHT) {
                    op_translate = 2;
                    op_translate_x = 1;
                    input->x = 1;
                }
                else if (event.key.keysym.sym == SDLK_i) {
                    op_translate_z = -1;
                    input->z = -1;
                }
                else if (event.key.keysym.sym == SDLK_k) {
                    op_translate_z = 1;
                    input->z = 1;
                }
                else if (event.key.keysym.sym == SDLK_r) {
                    play_06_math_restore();
                }
                break;
            case SDL_EVENT_KEY_UP:
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    running = 0;
                }
                else if (event.key.keysym.sym == SDLK_COMMA) {
                    op_rotate = -1;
                }
                else if (event.key.keysym.sym == SDLK_PERIOD) {
                    op_rotate = 1;
                }
                // else {
                //     op_rotate = 0;
                //     op_translate = 0;
                // }
                break;
            case SDL_EVENT_QUIT:
                running = 0;
                break;
            }
        }

        // rendering
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // use the clear color

        switch (drawmode) {
        case 1:
            hegl_play_draw_a_triangle();
            // hegl_play_01_draw_triangle();
            break;
        case 2:
            hegl_play_02_draw_rectangle();
            break;
        case 3:
            hegl_play_03_draw_two_triangle();
            break;
        case 4:
            if (submode == 1) hegl_play_04_draw_different_triangles();
            else if (submode == 2) hegl_play_04_draw_pyramid();
            else if (submode == 3) play_04_draw_with_vao_color();
            break;
        case 5:
            if (submode == 1) play_05_draw_with_texture();
            else if (submode == 2) play_05_draw_two_textures();
            break;
        case 6:
            play_06_math(op_rotate, op_translate);
            break;
        case 7:
            play_07_camera(op_translate, op_rotate_x, op_rotate_y, op_zoom);
            break;
        case 8:
            // play_08_color(op_rotate_x, op_rotate_y, op_translate_x, op_translate_y);
            play_08_material(op_translate_x, op_translate_y);
            break;
        case 9:
            play_09_lightmap(op_translate_x, op_translate_y, op_translate_z);
            break;
        case 10:
            play_10_basiclight(submode, op_translate_x, op_translate_y, op_translate_z);
            break;
        case 11:
            play_11_loadmodel();
            break;
        case 12:
            play_12_update_gltf();
            break;
        default:
            break;
        }

        SDL_GL_SwapWindow(window);

        // frame end

        op_rotate = 0;
        op_translate = 0;
        op_rotate_x = op_rotate_y = op_zoom = 0;
        op_translate_x = op_translate_y = op_translate_z = 0;
        he_reset_input();
    }

    hegl_fin();

    SDL_DestroyWindow(window);

    he_fin();

    printf("fin\n");

    return 0;
}
