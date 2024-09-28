#ifndef WEARRAY_H
#define WEARRAY_H

typedef void (*WEdelete_fn_t)(void *);

struct WEarray {
    int count;
    int size;
    void **elements;
    WEdelete_fn_t element_del_fn;
};

struct WEarray *WEarray_new(WEdelete_fn_t element_del_fn);
void WEarray_init(struct WEarray *arr, WEdelete_fn_t element_del_fn);
void WEarray_resize(struct WEarray *arr, int count);
void WEarray_add(struct WEarray *arr, void *element);
void *WEarray_get(struct WEarray *arr, int idx);
void WEarray_clear(struct WEarray *arr);
void WEarray_delete(struct WEarray *arr);

#endif