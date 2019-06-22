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

typedef struct {
    char *key;
    int val;
    struct node *next;
} node;

typedef struct {
    int size;
    struct node **list;
} table;

struct table *createTable(int size);
int hashCode(struct table *t,char *key);
void insert(struct table *t,char *key,int val);
int lookup(struct table *t,char *key);
#endif /* HASHMAP_H_ */
