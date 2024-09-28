/* code gen by duckduckgo ai */

#ifndef WEHASHTABLE_H
#define WEHASHTABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TABLE_SIZE 100

typedef struct Entry {
    char *key;
    int value;
    struct Entry *next;
} Entry;

typedef struct HashTable {
    Entry **entries;
} HashTable;

// 创建哈希表
HashTable *create_table() {
    HashTable *table = malloc(sizeof(HashTable));
    table->entries = malloc(sizeof(Entry *) * TABLE_SIZE);
    for (int i = 0; i < TABLE_SIZE; i++) {
        table->entries[i] = NULL;
    }
    return table;
}

// 哈希函数
unsigned int hash(const char *key) {
    unsigned int hash = 0;
    while (*key) {
        hash = (hash << 5) + *key++; // 31 * hash + c
    }
    return hash % TABLE_SIZE;
}

// 插入键值对
void insert(HashTable *table, const char *key, int value) {
    unsigned int index = hash(key);
    Entry *new_entry = malloc(sizeof(Entry));
    new_entry->key = strdup(key);
    new_entry->value = value;
    new_entry->next = table->entries[index];
    table->entries[index] = new_entry;
}

// 查找值
int search(HashTable *table, const char *key) {
    unsigned int index = hash(key);
    Entry *entry = table->entries[index];
    while (entry) {
        if (strcmp(entry->key, key) == 0) {
            return entry->value;
        }
        entry = entry->next;
    }
    return -1; // 未找到
}

// 释放哈希表
void free_table(HashTable *table) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        Entry *entry = table->entries[i];
        while (entry) {
            Entry *temp = entry;
            entry = entry->next;
            free(temp->key);
            free(temp);
        }
    }
    free(table->entries);
    free(table);
}

int test_hashtable() {
    HashTable *table = create_table();
    insert(table, "key1", 1);
    insert(table, "key2", 2);
    
    printf("key1: %d\n", search(table, "key1"));
    printf("key2: %d\n", search(table, "key2"));
    
    free_table(table);
    return 0;
}

#endif