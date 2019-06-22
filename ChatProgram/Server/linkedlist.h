/*
 * linkedlist.h
 *
 *  Created on: Jun 21, 2019
 *      Author: john
 */

#ifndef LINKEDLIST_H_
#define LINKEDLIST_H_
/* A linked list node */
struct Node {
	// Any data type can be stored in this node
	void *data;

	struct Node *next;
};
void push(struct Node **head_ref, void *new_data, size_t data_size);
void printList(struct Node *node, void (*fptr)(void*));
void printChar(void *n);
void printInt(void *n);
void printFloat(void *f);
#endif /* LINKEDLIST_H_ */
