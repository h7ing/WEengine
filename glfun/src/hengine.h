// hoka engine
// 2023 gwe

#pragma once

// data def

//

typedef struct HEInput {
    int x;
    int y;
    int z;
    int axisx;
    int axisy;
} HEInput;


// core

void he_init();
void he_fin();

void he_set_delta(float delta);
float he_get_delta();

void he_set_window_size(int w, int h);
float he_get_window_aspect();


// assets

// need free the path
char * he_asset_gen_path(const char *filename);

char * he_asset_load_content(const char *relativepath);

unsigned char * he_asset_load_texture(const char *relativepath, int *width, int *height, int *channels);
void he_asset_free_image(void *image);


// inputs

HEInput *he_get_input();

void he_reset_input();