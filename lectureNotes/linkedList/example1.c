#include <stdio.h>
#include <stdlib.h>

struct node {
    int num;
    struct node *next;
} *head;

void createNodeList(int n) {

    struct node *tmp, *fnNode;

    int num = 5, i;

    head = (struct node*)malloc(sizeof(struct node));

    if (head == NULL) {
        printf("Memory cannot be allocated");
        exit(1);
    }
    head->num = num;
    head->next = NULL;
    tmp = head;

    for (i = 2; i <= n; i++) {
        fnNode = (struct node *)malloc(sizeof(struct node));

        if (fnNode == NULL) {
            printf("Memory cannot be allocated");
            exit(1);
        }
        fnNode->num = num;
        fnNode->next = NULL;

        tmp->next = fnNode;
        tmp = tmp->next;
    }
}

void displayList() {
    struct node *tmp;

    if (head == NULL) {
        printf("List is empty");
        exit(1);
    }
    tmp = head;

}

int main() {

    int n = 5;


}