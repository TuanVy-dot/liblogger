#include "linked_list.h"
#include <stdlib.h>

void linked_list_add(Node *head, Node *node) {
    Node *curr = head;
    while (curr -> next != NULL) {
        curr = curr -> next;
    }
        ;
    curr -> next = node;
    node->next = NULL;
}

void linked_list_print(Node *head) {
    Node *curr = head;
    /*printf("%s\n", head -> next -> string);*/
    while (curr) {
        printf("%s -> ", curr->string);
        curr = curr -> next;
    }
}

void linked_list_free(Node *head) {
    Node *curr = head;
    Node *prev = NULL;
    while (curr) {
        prev = curr;
        curr = curr -> next;
        free(prev);
    }
}
