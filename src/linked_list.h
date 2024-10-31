#ifndef LINKED_LIST_H

#define LINKED_LIST_H


enum NODETYPE {
    HEAD, LITERAL, LABEL
};

typedef struct Node {
    const char *string;
    char type;
    struct Node *next;
} Node;

void linked_list_add(Node *head, Node *node);
void linked_list_print(Node *head);
void linked_list_free(const Node *head);

#endif
