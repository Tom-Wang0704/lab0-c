#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(struct list_head));
    if (head != NULL)
        INIT_LIST_HEAD(head);
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (!l)
        return;
    struct list_head *node = l->next;
    while (node != l) {
        element_t *dele = list_entry(node, element_t, list);
        node = node->next;
        q_release_element(dele);
        // free(dele->value);
        // free(dele);
    }
    free(l);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *node = malloc(sizeof(element_t));
    if (!node)
        return false;
    node->value = strdup(s);
    if (!node->value) {
        free(node);
        return false;
    }

    node->list.next = head->next;
    node->list.prev = head;
    head->next->prev = &node->list;
    head->next = &node->list;

    // node->list.next = head->next;
    // node->list.prev = head;
    // node->list.next->prev = &node->list;
    // head->next = &node->list;

    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *node = malloc(sizeof(element_t));
    if (!node)
        return false;
    node->value = strdup(s);
    if (!node->value) {
        free(node);
        return false;
    }

    node->list.next = head;
    node->list.prev = head->prev;
    node->list.prev->next = &node->list;
    head->prev = &node->list;
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *node = list_entry(head->next, element_t, list);
    list_del(head->next);
    if (sp != NULL) {
        strncpy(sp, node->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }

    return node;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *node = list_entry(head->prev, element_t, list);
    list_del(head->prev);
    if (sp != NULL) {
        strncpy(sp, node->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }

    return node;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;

    int count = 0;
    struct list_head *ptr = head->next;
    while (ptr != head) {
        ++count;
        ptr = ptr->next;
    }
    return count;
}

/* Delete the middle node in queue */
// https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
bool q_delete_mid(struct list_head *head)
{
    if (!head || list_empty(head))
        return false;
    struct list_head *ptr = head->next;
    for (struct list_head *back = head->prev;
         back != ptr && back->prev != ptr;) {
        back = back->prev;
        ptr = ptr->next;
    }

    list_del(ptr);
    element_t *node = list_entry(ptr, element_t, list);
    q_release_element(node);
    return true;
}

/* Delete all nodes that have duplicate string */
// https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
bool q_delete_dup(struct list_head *head)
{
    if (!head || list_empty(head))
        return false;

    bool dup = false;
    element_t *ptr, *Next;
    list_for_each_entry_safe (ptr, Next, head, list) {
        if ((ptr->list.next != head) &&
            (strcmp(ptr->value, Next->value) == 0)) {
            list_del(&ptr->list);
            q_release_element(ptr);
            dup = true;
        } else if (dup) {
            list_del(&ptr->list);
            q_release_element(ptr);
            dup = false;
        }
    }
    return true;
}

/* Swap every two adjacent nodes */
// https://leetcode.com/problems/swap-nodes-in-pairs/
void q_swap(struct list_head *head)
{
    if (!head || list_empty(head))
        return;

    struct list_head *current = head->next;
    while (current != head && current->next != head) {
        struct list_head *Next = current->next;
        // list_del(current);
        // list_add(current, Next);
        list_move(current, Next);
        current = current->next;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head))
        return;

    struct list_head *ptr = head->next;
    struct list_head *back = head->prev;
    while (ptr != back) {
        struct list_head *temp = ptr->prev;
        list_del(ptr);
        list_add(ptr, back);
        list_del(back);
        list_add(back, temp);
        if (back->next == ptr)
            break;
        temp = back->next;
        back = ptr->prev;
        ptr = temp;
    }
}

struct list_head *mergeSort(struct list_head *start, struct list_head *end)
{
    if (start == end)
        return start;

    struct list_head *slow = start, *fast = start;
    while (fast->next && fast->next->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    fast = slow->next;
    slow->next = NULL;

    struct list_head *left = mergeSort(start, slow);
    struct list_head *right = mergeSort(fast, end);

    struct list_head *new_head = start, **ptr = &new_head;

    for (element_t *l, *r; left && right; ptr = &(*ptr)->next) {
        l = list_entry(left, element_t, list);
        r = list_entry(right, element_t, list);
        struct list_head **Next;
        Next = strcmp(l->value, r->value) < 0 ? &left : &right;
        *ptr = *Next;
        (*Next) = (*Next)->next;
    }

    *ptr = left ? left : right;
    return new_head;
}

/* Sort elements of queue in ascending order */
void q_sort(struct list_head *head)
{
    if (!head || list_empty(head))
        return;

    struct list_head *start = head->next, *end = head->prev;
    end->next = NULL;

    head->next = mergeSort(start, end);
    head->next->prev = head;

    for (end = head->next; end->next; end = end->next)
        end->next->prev = end;

    end->next = head;
    head->prev = end;
}