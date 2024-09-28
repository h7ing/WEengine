#include "wearray.h"
#include <stdlib.h>

struct WEarray *WEarray_new(WEdelete_fn_t element_del_fn) {
    struct WEarray *arr = malloc(sizeof(struct WEarray));
    WEarray_init(arr, element_del_fn);
    return arr;
}

void WEarray_init(struct WEarray *arr, WEdelete_fn_t element_del_fn) {
    arr->size = 0;
    arr->count = 4;
    arr->elements = calloc(arr->count, sizeof(void *));
    arr->element_del_fn = element_del_fn;
}

void WEarray_resize(struct WEarray *arr, int count) {
    if (arr->count == count) return;

    arr->count = count;
    arr->elements = realloc(arr->elements, count * sizeof(void *));
}

void WEarray_add(struct WEarray *arr, void *element) {
    if (arr->size >= arr->count) {
        WEarray_resize(arr, arr->count * 2);
    }

    arr->size++;

    arr->elements[arr->size - 1] = element;
}

void *WEarray_get(struct WEarray *arr, int idx) {
    if (idx >= arr->size) return NULL;
    return arr->elements[idx];
}

void WEarray_clear(struct WEarray *arr) {
    if (arr->element_del_fn) {
        for (int i = 0; i < arr->size; i++) {
            arr->element_del_fn(arr->elements[i]);
        }
    }

    free(arr->elements);
    arr->count = 0;
    arr->size = 0;
}

void WEarray_delete(struct WEarray *arr) {
    WEarray_clear(arr);
    free(arr);
}
