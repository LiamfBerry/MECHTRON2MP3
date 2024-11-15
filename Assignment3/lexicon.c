#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TABLE_SIZE 5000 //Size of table 
#define ARRAY_SIZE 10



//Entry Structure 
typedef struct WordData {

    char *word; //Key 
    float value1; //Mean Sentiment score
    float value2; //Standard deviation
    int intArray[ARRAY_SIZE]; //Arrat of sentiment rating
    struct WordData *next; //Creates linked lists for collision entries

} WordData; //typedef alias for strucutre type

WordData *hashTable[TABLE_SIZE]; //Pointer to a pointer (creates table for which each entry will have the structure of the WordData) <-- Global variable



//Creates a key based off the input word
int hash(char *word) {
    int key = 0;
    int len = strlen(word);

    //Adds value of unsigned char multiplied by index for each character in word to create a key
    for (int i = 0; i < len; i++) { 
        key += ((unsigned char)(word[i])*i); //Ensures anagrams don't occupy the same key (avoids collisions)
        
    }
    return key % TABLE_SIZE; //Normalize to table size
}



//Initilaize all entries to NULL to start
void initHashTable() {
    for (int i = 0; i < TABLE_SIZE; i++) {
        hashTable[i] = NULL;
    }
}



//Inserts entries in hash table
void insert (char *word, float value1, float value2, int *intArray) {

    int index = hash(word); //Keys map to the index in the table

    //Initializes entry by allocating approprate memory to it
    WordData *entry = (WordData *)malloc(sizeof(WordData));
    if (entry == NULL) {
        printf("Memory allocation has failed\n");
        exit(1);
    }

    //Dynamic memory allocation to each word to make code more flexible to longer words while using less memory overall.
    entry->word = (char *)malloc((strlen(word)+1)*sizeof(char)); //Allocates memory to word dynamically note: +1 accounts for \0 terminator 
    if (entry->word == NULL ) {
        printf("memory allocation has failed\n");
        free(entry);
        exit(1);
    }

    strcpy(entry->word, word); //Copy the input word into the entry->word spot
    entry->value1 = value1; //Assign average sentiment score
    entry->value2 = value2; //Assign standard deviation
    memcpy(entry->intArray, intArray, ARRAY_SIZE*sizeof(int)); //Copy the array of values into the entry->intArray spot with a size of 10 integer entries
    entry->next = NULL; //Initialize linked node as NULL until told otherwise

    //If the spot in the table is aleady occupied (i.e. collision occurs) then move populated entry to linked node and populate empty header node with entry
    if (hashTable[index] != NULL) {
        entry->next = hashTable[index]; //Link new entry if collision occurs
    }
    hashTable[index] = entry; //hashTable spot gets assigned to entry structure
}



//Reads vader_lexicon.txt file and formats data into entries to be inserted into hash table
void readVader() {

    char word[256]; //array to store word
    char line[512]; //Store each line in lexicon
    float value1 = 0; //the average sentiment score variable
    float value2 = 0; //Standard deviation score variable
    int intArray[ARRAY_SIZE]; //Initialize array  

    //Opens lexicon
    FILE *vader = fopen("vader_lexicon.txt", "r");

    if (vader == NULL) {
        printf("File cannot be opened\n");
        exit(1);
    }

    //Scan through each line and look for the charcters in the word until a tab space is encountered then find the floating point number after the tabspace as well as the standard deviation and the list values
    //After found they are stored in the defined pointer memory adrresses for each element 
    while (fgets(line, sizeof(line), vader)) {
        if (sscanf(line, "%255[^\t] %f %f [%d, %d, %d, %d, %d, %d, %d, %d, %d, %d]", 
            word, &value1, &value2,
            &intArray[0],&intArray[1],&intArray[2],&intArray[3],&intArray[4],
            &intArray[5],&intArray[6],&intArray[7],&intArray[8],&intArray[9]) == 13) {//Ensures values are received from each line which is 13 values
    
            insert (word, value1, value2, intArray); //Insert found values from text file into hash table
        }
        else {
            printf("Line has failed\n");
        }
    }
    //close file when done
    fclose(vader);
}



//Find entries in hash table: O(1) time complexity 
float lookup(char *word) {

    int index = hash(word); //Keys map to the index in the table

    //Intialize entry as the entry from the index in the table
    WordData *entry = hashTable[index];

    //Word does not exist in lexicon
    if (entry == NULL) {
        return 0;
    }

    //If the input and entry words are the same then return the average sentiment
    while ( entry != NULL) {
        if (strcmp(entry->word, word) == 0) {
            return entry->value1;
        }
        //If words are not the same then go to the next node if its linked 
        entry = entry->next;
    }
    //Word does not exist in lexicon but collision occurs 
    return 0;

}



//outputs as a table with appropriate values
void printTable() {
    for (int i = 0; i < TABLE_SIZE; i++) {
        if (hashTable[i] != NULL) {
            WordData *entry = hashTable[i];
            while (entry != NULL) {
                //Print each value in a line
                printf("%i %s %.1f %.1f [", i, entry->word, entry->value1, entry->value2);
                for (int i = 0; i < ARRAY_SIZE; i++) {
                    printf("%d ", entry->intArray[i]);
                }
                printf("]\t");
                //Put linked entries in same line if they exist 
                entry = entry->next;
            }
            printf("\n");
        }
    }
}



//Frees memory hash table uses
void freeTable() {
    //For each entry in the table, if it isn't NULL iterate through each linked strucutre and free each until linked-list in entry is free
    for (int i = 0; i < TABLE_SIZE; i++) {
        WordData *entry = hashTable[i];
        while (entry) {
            WordData *toFree = entry;
            entry = entry->next;
            free(toFree->word);
            free(toFree);
        
        }
    }
}