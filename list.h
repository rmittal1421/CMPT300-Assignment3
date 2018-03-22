//list.h
//This header file contains the prototypes for the list data structure

/* Structure Definitions */
typedef struct _listNode {
  void* data;
  struct _listNode* next;
  struct _listNode* prev;
} NODE;


typedef struct _listHead {
  int numberOfItems;
  int status;
  struct _listHead* forward;
  struct _listHead* backward;
  NODE* head;
  NODE* tail;
  NODE* current;
} LIST;


/*Prototypes*/
// Makes a new empty list and returns its reference on success.
// Returns a NULL pointer on failure.
LIST* ListCreate();

// Returns the number of items in list.
int ListCount(LIST* list);

// Returns the pointer to the first item in the list and makes the first item the current item.
void* ListFirst(LIST* list);

// Returns the pointer to the last item in the list and makes the last item the current item.
void* ListLast(LIST* list);

// Advances list's current item by one, and returns the pointer to the new current item.
// If the operation advances the current item beyond the end of the list, a NULL pointer is returned.
void* ListNext(LIST* list);

// Backs up list's current item by one, and returns a pointer to the new current item.
// If this operation backs up the current item beyond the start of the list, a NULL pointer is returned.
void* ListPrev(LIST* list);

// Returns a pointer to the current item in the list.
void* ListCurr(LIST* list);

// Adds the new item to the list directly after the current item and makes item the current item.
// If the current pointer is before the start of the list, the item is added at the start.
// If the current pointer is beyond the end of the list, the item added at the end.
// Returns 0 on success, -1 on failure.
int ListAdd(LIST* list, void* item);

// Adds item to the list directly before the current item and makes the new item the current item.
// If the current pointer is before the start of the list, the item is added at the start.
// If the current pointer is beyond the end of the list, the item added at the end.
// Returns 0 on success, -1 on failure.
int ListInsert(LIST* list, void* item);

// Adds item to the end of the list and makes the new item the current one.
// Returns 0 on success, -1 on failure.
int ListAppend(LIST* list, void* item);

// Adds item to the front of the list and makes the new item the current one.
// Returns 0 on success, -1 on failure.
int ListPrepend(LIST* list, void* item);

// Return current item and take it out of list. Make the next item the current one.
void* ListRemove(LIST* list);

// Adds list2 to the end of list1. The current pointer is set to the current pointer of list1.
// list2 no longer exists after the operation.
void ListConcat(LIST* list1, LIST* list2);

// Delete list.itemFree is a pointer to routine that frees an item.
// It should be invoked (within ListFree) as: (*itemFree)(itemToBeFreed);
void ListFree(LIST* list, void (*itemFree)());

// Return last item and take it out of list.
// Make new last item the current one.
void* ListTrim(LIST* list);

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
void* ListSearch(LIST* list, int (*comparator)(), void* comparisonArg);

