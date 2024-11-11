#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

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
        key += (word[i]*i); //Ensures anagrams don't occupy the same key (avoids collisions)
        
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
    memcpy(entry->intArray, intArray, sizeof(intArray));
    entry->next = NULL;

    if (hashTable[index] != NULL) {
        entry->next = hashTable[index]; //Link new entry if collision occurs
    }

    hashTable[index] = entry; //hashTable spot gets assigned to entry structure

}

//See if it works
void printTable() {
    for (int i = 0; i < TABLE_SIZE; i++) {
        if (hashTable[i] != NULL) {
            WordData *entry = hashTable[i];
            while (entry != NULL) {
                printf("\t%i\t%s\t%.1f\t%.1f\t[", i, entry->word, entry->value1, entry->value2);
                for (int i = 0; i < ARRAY_SIZE; i++) {
                    printf("%d ", entry->intArray[i]);
                }
                printf("]");
                entry = entry->next;
            }
            printf("\n");
        }
    }
}


void readVader() {

    char word[256]; //array to store word
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
    while (fscanf(vader, "%255[^\t]\t%f\t%f\t[%d, %d, %d, %d, %d, %d, %d, %d, %d, %d]%*[^\n]", 
        word, &value1, &value2,
        &intArray[0],&intArray[1],&intArray[2],&intArray[3],&intArray[4],
        &intArray[5],&intArray[6],&intArray[7],&intArray[8],&intArray[9]) == 13) { //Ensures two values are received from each lne
        //printf("Inserting word: '%s' with value: %f\n", word, value1); //For debugging
        insert (word, value1, value2, intArray);
        fgetc(vader); //Moves past newline character

    }
    //close file when done
    fclose(vader);
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


//NOTE: This function should only be run if we already know the word is fully capitalized so we have to impliment a check for it
float checkCaps(char *word) {
    int len = strlen(word);

    int amplifier = 1.5; //flag to check if it's all capitals

    //If it isn't all then it's not intensified
    for (int i = 0; i < len; i++) {
        if (!isupper(word[i])) {
            amplifier = 1;
        }
        word[i] = tolower(word[i]);
    }

    return amplifier;
}

float intensifiers(char *word) {

    char *positive_amplifiers[] = {"absolutely", "completely", "extremely", "really", "so", "totally", "very", "particularly", "exceptionally", "incredibly", "remarkably"};
    char *negative_amplifiers[] = {"barely", "hardly", "scarcely", "somewhat", "mildly", "slightly", "partially", "fairly", "pretty much"};
    
    float boost_factor = 1.293;

    //Positive string compare
    for (int i = 0; i < 11; i++) {
        if (strcasecmp(word, positive_amplifiers[i]) == 0) { //Ensures compare is same case
            return boost_factor;
        }
    }
    //Negative string compare
    for (int i = 0; i < 9; i++) {
        if (strcasecmp(word, negative_amplifiers[i]) == 0) {
            return boost_factor;
        }
    }
    return 1.0;
}

float negations(char *word) {

    char *negations[] = {"not", "isn't", "doesn't", "wasn't", "shouldn't", "won't", "cannot", "can't", "nor", "neither", "without", "lack", "missing"};

    float negation = -0.5;

    //Negations string compare 
    for (int i = 0; i < 13; i++) {
        if (strcasecmp(word, negations[i]) == 0) {
            return negation;
        }
    }
    return 1;
}

float sentimentCalculation(char *sentence) {

    //If there is no sentence just return 0
    if (sentence == NULL) {
        return 0.0;
    }

    //Initilize variables needed 
    int len = strlen(sentence);
    char word_temp[256] = {0};
    float word_count = 1.0;

    float sentiment = 0.0;
    int temp_index = 0;

    float alpha  = 15; //normalization constant

    float amplifier = 1;

    int punctuation_count = 0;
    float punctuation_boost = 0.292;


    //Iterate through input sentence
    for (int i = 0; i < len; i++) {
        if (sentence[i] != ' ' && sentence[i] != '!' && sentence[i] != ',' && sentence[i] != '.' && sentence[i] != '\0')  { //How to hand punctuation that appears in emoticons?
            word_temp[temp_index] = sentence[i]; //Parse each word 
            temp_index++;
            
        }  
        //Handels punctuation boost from exclimation points
        //else if (sentence[i] == '!') {
            //punctuation_count++; //Is this meant to be positive for positive and negative for negative or just always positive?
        //}
        else {
            if (temp_index > 0) {
                
                word_temp[temp_index] = '\0'; //Null-terminates the word
                word_count++; //Increase word count

                sentiment += amplifier*checkCaps(word_temp)*lookup(word_temp) + punctuation_count*punctuation_boost; //Add to sentiment 
                amplifier = intensifiers(word_temp)*negations(word_temp); // Amplifer is set to the current word but won't be applied until the next word. Since amplifers don't appear in the lexicon this doesn't conflict
                
                memset(word_temp, 0, sizeof(word_temp)); //Reset temporary word holder
                temp_index = 0; //Reset temporary index for word holder
                punctuation_count = 0; //reset cause exclimations can be on multiple words 
                
            }
            
        } 
    }
    //Checks for last word which is missed in loop
    if (temp_index > 0) {
        word_temp[temp_index] = '\0'; //Null-terminates the word
        sentiment += amplifier * lookup(word_temp);
    }

    float compound = sentiment / sqrt(pow(sentiment, 2) + alpha);

    return compound;
}



int main() {

    initHashTable();

    readVader();

    char test[256] = "VADER is very smart, handsome, and funny.";

    printf("%f\n", sentimentCalculation(test));

    return 0;
}