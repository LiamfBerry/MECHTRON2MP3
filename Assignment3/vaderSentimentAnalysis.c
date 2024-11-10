#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node {
    char *word;
    struct Node *next;
} Node;

Node* createNode(const char *word) {
    Node *newNode = (Node *)malloc(sizeof(Node));
    newNode->word = strdup(word);  // Copy the word to the node
    newNode->next = NULL;
    return newNode;
}

void appendNode(Node **head, const char *word) {
    Node *newNode = createNode(word);
    if (*head == NULL) {
        *head = newNode;
    } else {
        Node *current = *head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = newNode;
    }
}

void splitSentenceToLinkedList(Node **head, const char *sentence) {
    char *sentenceCopy = strdup(sentence);  // Duplicate the sentence for strtok to modify
    char *word = strtok(sentenceCopy, " ");
    while (word != NULL) {
        appendNode(head, word);
        word = strtok(NULL, " ");
    }
    free(sentenceCopy);  // Free the duplicate after use
}

void printList(Node *head) {
    Node *current = head;
    while (current != NULL) {
        printf("%s\n", current->word);
        current = current->next;
    }
    printf("NULL\n");
}

void freeList(Node *head) {
    Node *current = head;
    while (current != NULL) {
        Node *next = current->next;
        free(current->word);
        free(current);
        current = next;
    }
}

int main() {
    const char *sentence = "This is a sample sentence";
    Node *head = NULL;
    
    splitSentenceToLinkedList(&head, sentence);
    printList(head);
    
    freeList(head);
    return 0;
}
