#include <stdio.h>
#include <stdlib.h>

#define TABLE_SIZE 10

typedef struct {
    int key; //Key word
    int value; //Stores this value
} HashEntry; //Initializes head entry

//HashEntry *hashTable[TABLE_SIZE]; //Pointer to a pointer (Create with a table size and each feature will have structure of HashEntry) <-global variable

int hashFunction(int key) { //keys: 1, 5, 15

    return key % TABLE_SIZE; //Index: 1, 5, 5

}

void insert(int key, int value, HashEntry **hashTable) {
    int index = hashFunction(key);

    if (hashTable[index] != NULL) { //Avoids memory leak
        free(hashTable[index]);
    }
    HashEntry *entry = (HashEntry *)malloc(sizeof(HashEntry));
    entry->key = key;
    entry-> value = value;
    hashTable[index] = entry; ///This index points to the node that is stored
}

int search(int key, HashEntry **hashTable) {
    int index = hashFunction(key);
    if (hashTable[index] != NULL && hashTable[index]->key == key) {
        return hashTable[index]->value;
    }
    return -1; //Key not found
}

int main() {

    //Memory allocation

    HashEntry **hashTable = (HashEntry **)malloc(TABLE_SIZE*sizeof(HashEntry *)); //Pointer to a pointer, and every element of created stack is a pointer.

    //Passing hashTable to functions

    insert(1, 100, hashTable);
    insert(5,200, hashTable); //(when deallocating it doesn't get free so there is a memory leak)
    insert(15,1200, hashTable); //Collision occurs here, doesn't overwrite you allocate another part of memory and the previous one is lost 

    printf("Value for key 1: %d\n", search(1, hashTable));
    printf("Value for key 2: %d\n", search(2, hashTable));
    printf("Value for key 15: %d\n", search(15, hashTable));

    for (int i = 0; i < TABLE_SIZE; i++) {
        if (hashTable[i] != NULL) {
            free(hashTable[i]);
        }
    }
    free(hashTable);

    return 0;
}