/**
 * libchess
 * Jack O'Connor 2025
 * include/list.h
 **/

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>


/* Node Structure */

typedef struct Node Node;
struct Node {
    void    *data;
    Node    *next;
    Node    *prev;
};

Node *	node_create(void *data, Node *next, Node *prev);
void	node_delete(Node *n, bool release);

/* List Structure */

typedef struct {
    Node    sentinel;
    size_t  size;
} List;

List *	    list_create();
void	    list_delete(List *l, bool release);

void        list_append(List *l, void *data);
void *      list_pop(List *l, size_t index);


