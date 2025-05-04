/**
 * libchess
 * Jack O'Connor 2025
 * src/list.c
 **/

#include <stdlib.h>

#include "list.h"


/* Node Functions */


/**
 * Create a Node structure.
 *
 * @param   v       Value (Number or String).
 * @param   next    Pointer to next Node structure.
 * @param   prev    Pointer to previous Node structure.
 *
 * @return  Pointer to new Node structure (must be deleted later).
 **/
Node *      node_create(void *data, Node *next, Node *prev) {

    Node *n = (Node *) calloc(1, sizeof(Node));
    if (n) {
        n->data = data;
        n->next = next;
        n->prev = prev;
    }

    return n;
}

/**
 * Delete Node structure and its contents.
 *
 * @param   n       Pointer to Node structure.
 * @param   release Whether or not to free the string value.
 **/
void        node_delete(Node *n, bool release) {
    if (release) free(n->data);
    free(n);
}



/* List Functions */

/**
 * Create a List structure.
 *
 * @return  Pointer to new List structure (must be deleted later).
 **/
List *	    list_create() {

    List *l = (List *) calloc(1, sizeof(List));
    if (l) {
        l->sentinel.next = &l->sentinel;
        l->sentinel.prev = &l->sentinel;
    }
    return l;
}

/**
 * Delete List structure.
 *
 * @param   l       Pointer to List structure.
 * @param   release Whether or not to release the string values.
 **/
void	    list_delete(List *l, bool release) {

    Node *n = l->sentinel.next;
    while (n != &l->sentinel) {
        Node *old = n;
        n = n->next;
        node_delete(old, release);
    }
    free(l);
}

/**
 * Add new Value to back of List structure.
 *
 * @param   l       Pointer to List structure.
 * @param   v       Value to add to back of List structure.
 **/
void        list_append(List *l, void *data) {

    Node *new = node_create(data, &l->sentinel, l->sentinel.prev);
    new->prev->next = new;
    l->sentinel.prev = new;
    l->size++;
}

/**
 * Remove Value at specified index from List structure.
 *
 * @param   l       Pointer to List structure.
 * @param   index   Index of Value to remove from List structure.
 *
 * @return  Value at index in List structure (-1 if index is out of bounds).
 **/
void *      list_pop(List *l, size_t index) {

    Node *curr = l->sentinel.next;
    for (size_t i = 0; i < l->size; i++) {
        if (curr == &l->sentinel) break; // Hopefully redundant
        if (i == index) {
            void *data = curr->data;
            curr->next->prev = curr->prev;
            curr->prev->next = curr->next;
            node_delete(curr, false);
            l->size--;
            return data;
        }
        curr = curr->next;
    }

    return NULL;
}
