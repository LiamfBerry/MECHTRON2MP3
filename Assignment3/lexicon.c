#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TABLE_SIZE 5000 //Size of table 
#define ARRAY_SIZE 10

typedef struct WordData {

    char *word; //Key 
    float value1; //Mean Sentiment score
    float value2; //Standard deviation
    int intArray[ARRAY_SIZE]; //Arrat of sentiment rating
    struct WordData *next; //Creates linked lists for collision entries

} WordData;

WordData *hashTable[TABLE_SIZE]; //Pointer to a pointer (creates table for which each entry will have the structure of the WordData) <-- Global variable

//Creates a key based off the input word
int hash(char *word) {
    int key = 0;
    int len = strlen(word);

    for (int i = 0; i < len; i++) { 
        key += ((unsigned char)(word[i])*i); //Ensures anagrams don't occupy the same key (avoids collisions)
        
    }
    return key % TABLE_SIZE; //Normalize to table size
}

void initHashTable() {
    //Initilaize all entries to NULL to start
    for (int i = 0; i < TABLE_SIZE; i++) {
        hashTable[i] = NULL;
    }
}

//Inserts entries in hash table
void insert (char *word, float value1, int value2, int *intArray) {

    int index = hash(word); //Keys map to the index in the table

    //printf("Inserting %s at %d\n", word, index);

    //Initializes entry
    WordData *entry = (WordData *)malloc(sizeof(WordData));
    if (entry == NULL) {
        printf("Memory allocation has failed\n");
        exit(1);
    }
    entry->word = (char *)malloc((strlen(word)+1)*sizeof(char)); //Allocates memory to word dynamically note: +1 accounts for \0 terminator 
    if (entry->word == NULL ) {
        printf("memory allocation has failed\n");
        free(entry);
        exit(1);
    }

    strcpy(entry->word, word); //Copy the input word into the entry->word spot
    entry->value1 = value1; //Assign sentiment score
    entry->value2 = value2;
    memcpy(entry->intArray, intArray, ARRAY_SIZE*sizeof(int));
    entry->next = NULL;

    if (hashTable[index] != NULL) {
        entry->next = hashTable[index]; //Link new entry if collision occurs
    }

    hashTable[index] = entry; //hashTable spot gets assigned to entry structure

}

float lookup(char *word) {

    int index = hash(word); //Keys map to the index in the table

    WordData *entry = hashTable[index];

    //Word does not exist in lexicon
    if (entry == NULL) {
        return 0.0;
    }

    //If values are the same 
    while ( entry != NULL) {
        if (strcmp(entry->word, word) == 0) {
            return entry->value1;
        }
        entry = entry->next;
    }
    //Word does not exist in lexicon but collision occurs 
    return 0.0;

    
    
}

//See if it works
void printTable() {
    for (int i = 0; i < TABLE_SIZE; i++) {
        if (hashTable[i] != NULL) {
            WordData *entry = hashTable[i];
            while (entry != NULL) {
                printf("%i %s %.1f %.1f [", i, entry->word, entry->value1, entry->value2);
                for (int i = 0; i < ARRAY_SIZE; i++) {
                    printf("%d ", entry->intArray[i]);
                }
                printf("]\t");
                entry = entry->next;
            }
            printf("\n");
        }
    }
}


void readVader() {

    char word[256]; //array to store word
    char line[512]; //Store each line 
    float value1 = 0; //the sentiment score
    float value2 = 0;
    int intArray[10];

    //Opens lexicon
    FILE *vader = fopen("vader_lexicon.txt", "r");

    if (vader == NULL) {
        printf("File cannot be opened\n");
        exit(1);
    }

    //Scan through each line and look for the charcters in the word and the floating point integer after the tabspace as well as the standard deviation and the list values
    while (fgets(line, sizeof(line), vader)) {
        if (sscanf(line, "%255[^\t] %f %f [%d, %d, %d, %d, %d, %d, %d, %d, %d, %d]", 
            word, &value1, &value2,
            &intArray[0],&intArray[1],&intArray[2],&intArray[3],&intArray[4],
            &intArray[5],&intArray[6],&intArray[7],&intArray[8],&intArray[9]) == 13) {//Ensures two values are received from each lne
            //printf("Inserting word: '%s' with value: %f\n", word, value1); //For debugging
            insert (word, value1, value2, intArray);
        }
        else {
            printf("Line has failed\n");
        }
    }
    //close file when done
    fclose(vader);
}

void freeTable() {
    for (int i = 0; i < TABLE_SIZE; i++) {
        WordData *entry = hashTable[i];
        WordData *temp = entry;
        while (temp) {
            WordData *toFree = temp;
            temp = temp->next;
            free(toFree);
        }
        free(entry);
    }
}