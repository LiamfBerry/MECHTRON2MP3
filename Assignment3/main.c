#include <stdio.h>
#include <string.h>
#include "vaderUtility.h"
#include "lexiconUtility.h"

int main() {

    //Initialize hash table as NULL
    initHashTable();

    //Reads through lexicon to insert entries into hash table
    readVader();

    //Prints Table to check if the structure is correct (see printTable(); function from lexicon.c to see how it should display)
    //printTable();

    //Source array of test cases 
    char *source[] = {"VADER is smart, handsome, and funny.", "VADER is smart, handsome, and funny!", "VADER is very smart, handsome, and funny.", 
                        "VADER is VERY SMART, handsome, and FUNNY.", "VADER is VERY SMART, handsome, and FUNNY!!!", "VADER is VERY SMART, uber handsome, and FRIGGIN FUNNY!!!",
                        "VADER is not smart, handsome, nor funny.", "At least it isn't a horrible book.", "The plot was good, but the characters are uncompelling and the dialog is not great.",
                        "Make sure you :) or :D today!", "Not bad at all"};

    //Get number of test cases in source array
    int len = sizeof(source)/sizeof(*source);

    //Iterate through each test case and dislay the compounded sentiment score
    for (int i = 0; i < len; i++) {
        printf("The Sentence, '%s', has a compounded sentiment of : %f\n", source[i], sentimentCalculation(source[i]));
    }

    //Free memory in table
    freeTable();

    return 0;
}


//Things to do still:
/*
Write up 

Negative amplifier "pretty much" handling

Maybe make vaderSentiment.c look more organized or try to optimize if possible.
*/