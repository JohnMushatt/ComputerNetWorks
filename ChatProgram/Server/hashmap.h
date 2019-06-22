/*
 * hashmap.h
 *
 *  Created on: Jun 21, 2019
 *      Author: john
 */

#ifndef HASHMAP_H_
#define HASHMAP_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct entry_s {
	char *key;
	char *value;
	struct entry_s *next;
};

typedef struct entry_s entry_t;

struct hashtable_s {
	int size;
	struct entry_s **table;
};

typedef struct hashtable_s hashtable_t;

hashtable_t* ht_create(int size);

int ht_hash(hashtable_t *hashtable, char *key);

entry_t* ht_newpair(char *key, char *value);

void ht_set(hashtable_t *hashtable, char *key, char *value);

char* ht_get(hashtable_t *hashtable, char *key);
#endif /* HASHMAP_H_ */
