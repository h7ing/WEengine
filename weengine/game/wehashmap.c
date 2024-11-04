#include "wehashmap.h"
#include <stdlib.h>
#include "wehash.h"

static unsigned int WEhashmap_hashindex(void *key) {
	uint32_t hash = hash_ptr(key, 32); /* mark: bits暂时不知道是什么算法 */
	unsigned int index = hash % WE_BUCKET_SIZE;
	return index;
}

struct WEhashmap *WEhashmap_new() {
	struct WEhashmap *hm = malloc(sizeof(*hm));
	hm->items = calloc(WE_BUCKET_SIZE, sizeof(struct WEhash_entry *));
	return hm;
}

void WEhashmap_delete(struct WEhashmap *hm) {
	for (int i = 0; i < WE_BUCKET_SIZE; i++) {
		struct WEhash_entry *item = hm->items[i];
		while (item) {
			struct WEhash_entry *tmp = item;
			item = item->next;

			free(tmp);
		}
	}
	free(hm);
}

void WEhashmap_add(struct WEhashmap *hm, void *key, void *val) {
	unsigned int index = WEhashmap_hashindex(key);

	struct WEhash_entry *item = malloc(sizeof(*item));
	item->key = key;
	item->val = val;

	/* 插入到表头 */
	item->next = hm->items[index];
	hm->items[index] = item;
}

void *WEhashmap_find(struct WEhashmap *hm, void *key) {
	unsigned int index = WEhashmap_hashindex(key);

	struct WEhash_entry *item = hm->items[index];
	while (item) {
		if (item->key == key) {
			return item->val;
		}

		item = item->next;
	}

	return NULL;
}