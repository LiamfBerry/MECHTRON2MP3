#ifndef LEXICON_H
#define LEXICON_H

#define TABLE_SIZE 5000 //Size of table 
#define ARRAY_SIZE 10 //Size of input array

//Entry structure 
typedef struct WordData {

    char *word; //Key 
    float value1; //Mean Sentiment score
    float value2; //Standard deviation
    int intArray[ARRAY_SIZE]; //Arrat of sentiment rating
    struct WordData *next; //Creates linked lists for collision entries

} WordData; //typedef alias for strucutre type

extern WordData *hashTable[TABLE_SIZE]; //Pointer to a pointer (creates table for which each entry will have the structure of the WordData) <-- Global variable

//Function Definitions
int hash(char *word);
void initHashTable();
void insert(char *word, float value1, float value2, int *intArray);
void readVader();
float lookup(char *word);
void printTable();
void freeTable();

#endif