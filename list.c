//
// Created by Raghav Mittal on 2018-03-18.
//

// list.c
// This file contains the implementations of methods declared in list.h

/* Libraries included */
#include "list.h"
#include <assert.h>
#include <stddef.h>
#include <stdio.h>

/* Global Structures */
NODE listNodes[100];
LIST listHeads[10];
LIST* headNext = NULL;
NODE* nodeNext = NULL;
int flag = 0;

/* Functions Implementations */
// Makes a new empty list and returns its reference on success.
// Returns a NULL pointer on failure.
LIST* ListCreate() {
    // Check if this is the first call to ListCreate()
    if(flag == 0) {
        // Set up the pointers to create a free pool of nodes
        listNodes[0].prev = NULL;
        for(int i = 0; i < 99; i++) {
            listNodes[i].next = &listNodes[i+1];
            //assert(listNodes[i].next = &listNodes[i+1]);
            listNodes[i+1].prev = &listNodes[i];
            //assert(listNodes[i+1].prev = &listNodes[i]);
        }
        listNodes[99].next = NULL;
        // Create a free pool of lists
        listHeads[0].backward = NULL;
        for(int j = 0; j < 9; j++) {
            listHeads[j].forward = &listHeads[j+1];
            assert(listHeads[j].forward == &listHeads[j+1]);
            listHeads[j+1].backward = &listHeads[j];
            assert(listHeads[j+1].backward == &listHeads[j]);
        }
        listHeads[9].forward = NULL;
        nodeNext = &listNodes[99];
        headNext = &listHeads[9];
        flag = 1;
    }
    // if a free list is available allocate it, else return NULL
    if(headNext != NULL) {
        LIST* newList = headNext;
        headNext = headNext->backward;
        if(headNext != NULL) headNext->forward = NULL;

        newList->numberOfItems = 0;
        newList->head = NULL;
        newList->current = NULL;
        newList->status = -1;
        return newList;
    }
    return NULL;
}

// Returns the number of items in list.
int ListCount(LIST* list) {
    if(list != NULL) return list->numberOfItems;
    return -1;
}

// Returns the pointer to the first item in the list and makes the first item the current item.
void* ListFirst(LIST* list) {
    if(list == NULL || list->head == NULL) {
        return NULL;
    }
    list->current = list->head;
    list->status = 0;
    return list->head->data;
}

// Returns the pointer to the last item in the list and makes the last item the current item.
void* ListLast(LIST* list) {
    if(list == NULL || list->tail == NULL) {
        return NULL;
    }
    list->current = list->tail;
    list->status = 0;
    return list->tail->data;
}

// Advances list's current item by one, and returns the pointer to the new current item.
// If the operation advances the current item beyond the end of the list, a NULL pointer is returned.
void* ListNext(LIST* list) {
    // Boundary conditions
    if(list == NULL || list->head == NULL) return NULL;
    if(list->status == 0) { // Case1: list->current inside the list
        if(list->current != list->tail) {
            list->current = list->current->next;
            return list->current->data;
        }
        list->current = NULL;
        list->status = 1;
        return NULL;
    }
    else if(list->status == -1) { // Case2: list->current before the start of the list
        list->current = list->head;
        list->status = 0;
        return list->head->data;
    }
    else { // Case3: list->current beyond the end of the list
        return NULL;
    }
}

// Backs up list's current item by one, and returns a pointer to the new current item.
// If this operation backs up the current item beyond the start of the list, a NULL pointer is returned.
void* ListPrev(LIST* list) {
    // Boundary conditions
    if(list == NULL || list->head == NULL) return NULL;
    // Case1: list->current inside the list
    if(list->status == 0) {
        if(list->current != list->head) {
            list->current = list->current->prev;
            return list->current->data;
        }
        list->current = NULL;
        list->status = -1;
        return NULL;
    }
    else if(list->status > 0) { // Case2: list->current before the start of the list
        list->current = list->tail;
        list->status = 0;
        return list->tail->data;
    }
    else { // Case3: list->current beyond the end of the list
        return NULL;
    }
}

// Returns a pointer to the current item in the list.
void* ListCurr(LIST* list) {
    if(list == NULL || list->current == NULL) {
        return NULL;
    }
    return list->current->data;
}

// Adds the new item to the list directly after the current item and makes item the current item.
// If the current pointer is before the start of the list, the item is added at the start.
// If the current pointer is beyond the end of the list, the item added at the end.
// Returns 0 on success, -1 on failure.
int ListAdd(LIST* list, void* item) {
    // Check if list or item is NULL (not allocated)
    if(list == NULL || item == NULL) return -1;
    // Check if free nodes are available
    if(nodeNext == NULL) return -1;

    // Allocate a free node
    NODE* newNode = nodeNext;
    nodeNext = nodeNext->prev;
    newNode->data = item;
    if(nodeNext != NULL) nodeNext->next = NULL;

    // Case1: ListCount(list) == 1
    if(list->head == NULL) {
        newNode->prev = NULL;
        list->head = newNode;
        list->current = newNode;
        list->tail = newNode;
        list->status = 0;
        list->numberOfItems = 1;
        return 0;
    }
    else {
        if(list->status == 0) { // Case2: list->current is inside the list
            if(list->current != list->tail) { // Case2->1:list->current is not tail
                newNode->next = list->current->next;
                newNode->next->prev = newNode;
                newNode->prev = list->current;
                list->current->next = newNode;
                list->current = newNode;
            }
            else { // Case2->2: general case
                list->tail = newNode;
                newNode->prev = list->current;
                list->current->next = newNode;
                list->current = newNode;
            }
            list->numberOfItems++;
            return 0;
        }
        else { // Case3: list->current is before the start of the list
            if(list->status == -1) {
                list->head->prev = newNode;
                newNode->next = list->head;
                newNode->prev = NULL;
                list->head = newNode;
                list->current = newNode;
                list->numberOfItems++;
                return 0;
            }
            else { // Case4: list->current is beyond the end of the list
                list->tail->next = newNode;
                newNode->prev = list->tail;
                list->tail = newNode;
                list->current = newNode;
                list->numberOfItems++;
                return 0;
            }
        }
    }
}

// Adds item to the list directly before the current item and makes the new item the current item.
// If the current pointer is before the start of the list, the item is added at the start.
// If the current pointer is beyond the end of the list, the item added at the end.
// Returns 0 on success, -1 on failure.
int ListInsert(LIST* list, void* item) {
    // Check if list is NULL (not allocated)
    if(list == NULL || item == NULL) return -1;
    // no free node available
    if(nodeNext == NULL) return -1;

    // Allocate free node
    NODE* newNode = nodeNext;
    nodeNext = nodeNext->prev;
    newNode->data = item;
    if(nodeNext != NULL) nodeNext->next = NULL;

    // Case1: ListCount(list) == 1
    if(list->head == NULL) {
        newNode->prev = NULL;
        list->head = newNode;
        list->current = newNode;
        list->tail = newNode;
        list->status = 0;
        list->numberOfItems = 1;
        return 0;
    }
    else {
        if(list->status == 0) { // Case2: list->current is inside the list
            if(list->current != list->head) { // Case2->1: list->current is not the head
                newNode->prev = list->current->prev;
                newNode->next = list->current;
                newNode->prev->next = newNode;
                list->current->prev = newNode;
                list->current = newNode;
            }
            else { // Case2->2: general case
                newNode->prev = NULL;
                newNode->next = list->head;
                list->head->prev = newNode;
                list->head = newNode;
                list->current = newNode;
            }
            list->numberOfItems++;
            return 0;
        }
        else {
            if(list->status == -1) { // Case3: list->current before the start of the list
                list->head->prev = newNode;
                newNode->next = list->head;
                newNode->prev = NULL;
                list->head = newNode;
                list->current = newNode;
                list->numberOfItems++;
                return 0;
            }
            else { // Case4: list->current beyond the end of the list
                list->tail->next = newNode;
                newNode->prev = list->tail;
                list->tail = newNode;
                list->current = newNode;
                list->numberOfItems++;
                return 0;
            }
        }
    }
}

// Adds item to the end of the list and makes the new item the current one.
// Returns 0 on success, -1 on failure.
int ListAppend(LIST* list, void* item) {
    // Boundary conditions
    if(list == NULL || item == NULL) return -1;
    // no free node available
    if(nodeNext == NULL) return -1;

    // Allocate a free node
    NODE* newNode = nodeNext;
    nodeNext = nodeNext->prev;
    newNode->data = item;
    if(nodeNext != NULL) nodeNext->next = NULL;

    // Case1: ListCount(list) == 1
    if(list->head == NULL) {
        newNode->prev = NULL;
        list->head = newNode;
        list->tail = newNode;
        list->current = newNode;
        list->status = 0;
        list->numberOfItems++;
        return 0;
    }
    // Case2: General case
    list->tail->next = newNode;
    newNode->prev = list->tail;
    list->tail = newNode;
    list->current = newNode;
    list->numberOfItems++;
    return 0;
}

// Adds item to the front of the list and makes the new item the current one.
// Returns 0 on success, -1 on failure.
int ListPrepend(LIST* list, void* item) {
    // Boundary conditions
    if(list == NULL || item == NULL) return -1;
    // No free node available
    if(nodeNext == NULL) return -1;

    // Allocate a new Node
    NODE* newNode = nodeNext;
    nodeNext = newNode->prev;
    newNode->data = item;
    if(nodeNext != NULL) nodeNext->next = NULL;

    // Case1: ListCount(list) == 1
    if(list->head == NULL) {
        newNode->prev = NULL;
        list->head = newNode;
        list->tail = newNode;
        list->current = newNode;
        list->status = 0;
        list->numberOfItems++;
        return 0;
    }
    // Case2: General case
    list->head->prev = newNode;
    newNode->next = list->head;
    newNode->prev = NULL;
    list->head = newNode;
    list->current = newNode;
    list->numberOfItems++;
    return 0;
}

// Return current item and take it out of list. Make the next item the current one.
void* ListRemove(LIST* list) {
    // Check if list or head are NULL
    if(list == NULL || list->head == NULL) return NULL;

    // Case1: Current node is NULL
    if(list->current == NULL) {
        if(list->status == -1) {
            list->current = list->head;
            list->status = 0;
        }
        else if(list->status == 1) {
            list->current = list->tail;
            list->status = 0;
        }
        return NULL;
    }

    // Case2: ListCount(list) == 1
    NODE* freeNode = list->current;
    void* item = freeNode->data;
    if(list->current == list->head && list->current == list->tail) {
        list->current = NULL;
        list->head = NULL;
        list->tail = NULL;
        list->status = -1;
    }
    else { // Case3: head to be removed from the list
        if(list->current == list->head) {
            list->current = list->current->next;
            list->head = list->current;
            list->head->prev = NULL;
        } // Case4: tail to be removed from the list
        else if(list->current == list->tail) {
            list->tail = list->tail->prev;
            list->tail->next = NULL;
            list->current = list->tail;
        } // Case5: General case
        else {
            list->current = list->current->next;
            list->current->prev = freeNode->prev;
            freeNode->prev->next = list->current;
        }
    }
    // Add the available node to the free pool
    freeNode->data = NULL;
    if(nodeNext != NULL) {
        freeNode->prev = nodeNext;
        nodeNext->next = freeNode;
    }
    else {
        freeNode->prev = NULL;
    }
    nodeNext = freeNode;
    freeNode->next = NULL;
    list->numberOfItems--;
    return item;
}

// Adds list2 to the end of list1. The current pointer is set to the current pointer of list1.
// list2 no longer exists after the operation.
void ListConcat(LIST* list1, LIST* list2) {
    // Check if eiter list is NULL (not allocated)
    if(list1 == NULL || list2 == NULL) return;

    // Case1: list2 is empty
    if(list2->head == NULL) {
        if(headNext != NULL) {
            list2->backward = headNext;
            headNext->forward = list2;
        }
        list2->forward = NULL;
        headNext = list2;
        return;
    } // Case2: list1 is empty
    if(list1->head == NULL) {
        list1->head = list2->head;
    } // Case3: general case
    else {
        list1->tail->next = list2->head;
        list1->tail = list2->tail;
    }
    list1->numberOfItems += list2->numberOfItems;
    list2->numberOfItems = 0;
    list2->status = -1;
    // Push the freed list back to the pool
    if(headNext != NULL) {
        list2->backward = headNext;
        headNext->forward = list2;
    }
    else list2->backward = NULL;
    headNext = list2;
    headNext->head = NULL;
    headNext->current = NULL;
    headNext->tail = NULL;
    headNext->forward = NULL;

    list2 = NULL;
    return;
}

// Delete list.itemFree is a pointer to routine that frees an item.
// It should be invoked (within ListFree) as: (*itemFree)(itemToBeFreed);
void ListFree(LIST* list, void (*itemFree)()) {
    // return if list is NULL
    if(list == NULL) return;

    if(list->head != NULL) {
        // Traverse through the list to free each allocated node
        NODE* curr = list->head;
        NODE* toFree = NULL;
        while(curr != NULL) {
            (*itemFree)(curr->data);
            toFree = curr;
            curr = curr->next;
            if(nodeNext != NULL) {
                toFree->prev = nodeNext;
                nodeNext->next = toFree;
            }
            else toFree->prev = NULL;
            toFree->next = NULL;
            toFree->data = NULL;
            nodeNext = toFree;
        }
        list->head = NULL;
        list->current = NULL;
        list->tail = NULL;
    }

    // Push the emptied list back to the pool of free lists.
    if(headNext != NULL) {
        list->backward = headNext;
        headNext->forward = list;
    }
    list->forward = NULL;
    headNext = list;
    return;
}

// Return last item and take it out of list.
// Make new last item the current one.
void* ListTrim(LIST* list) {
    // Check if the the list or its first element is NULL
    if(list == NULL || list->head == NULL) return NULL;

    // Free Node at the tail and return its data
    NODE* freeNode = list->tail;
    void* item = freeNode->data;

    // Case1: ListCount(list) == 1;
    if(list->head == list->tail) {
        list->current = NULL;
        list->head = NULL;
        list->tail = NULL;
        list->numberOfItems--;
        list->status = -1;
    } // Case2: ListCount(list) > 1
    else {
        list->tail = list->tail->prev;
        list->tail->next = NULL;
        list->current = list->tail;
    }
    // push the available node to the pool of free nodes
    freeNode->data = NULL;
    if(nodeNext != NULL) {
        freeNode->prev = nodeNext;
        nodeNext->next = freeNode;
    }
        // Check if this is the first available node
    else {
        freeNode->prev = NULL;
    }
    nodeNext = freeNode;
    freeNode->next = NULL;
    return item;
}

// Searches list starting at the current item until the end is reached or a match is found.
// In this context a match is determined by the comparator parameter.
// This parameter is a pointer to a routine that takes as its first argument an item pointer,
// and as its second argument comparisonArg.
// Comparator returns 0 if the item and comparisonArg don't match, or 1 if they do.
// Exactly what constitutes a match is upto the implementor of comparator.
// If a match is found, the current pointer is left at the matched item
// and the pointer to that item is returned.
// If no match is found, the current pointer is left beyond the end of the list
// and a NULL pointer is returned.
void* ListSearch(LIST* list, int (*comparator)(), void* comparisonArg) {
    if(list == NULL || list->head == NULL || comparisonArg == NULL) return NULL;
    list->current = list->head;
    int ret = -1;

    // Traverse through the list to find the match!!
    while(list->current != NULL) {
        ret = (*comparator)(list->current->data, comparisonArg);
        if(ret == 1)  {
            return list->current->data;
        }
        list->current = list->current->next;
    }
    return NULL;
}
