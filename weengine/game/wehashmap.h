#ifndef WEHASHMAP_H
#define WEHASHMAP_H

#include <stdint.h>

#define WE_BUCKET_SIZE 100

struct WEhash_entry {
	void *key;
	void *val;
	struct WEhash_entry *next;
};

struct WEhashmap {
	struct WEhash_entry **items;
};

struct WEhashmap *WEhashmap_new();

void WEhashmap_delete(struct WEhashmap *hm);

void WEhashmap_add(struct WEhashmap *hm, void *key, void *val);

void *WEhashmap_find(struct WEhashmap *hm, void *key);

#endif