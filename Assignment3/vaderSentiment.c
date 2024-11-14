#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "lexiconUtility.h"



//Checks if a word us capitalized and returns a 1.5 times multplier if it is
//Also converts to lower case so that it can be properly checked by the hash lookup function
float checkCaps(char *word) {

    int len = strlen(word);

    float amplifier = 1.5; //flag Assumes it is all caps to start until proven otherwise

    //If any lette is non-capitalized then amplifer is not applied
    for (int i = 0; i < len; i++) {
        //Also checks if any character is non-alpha numeric as the lower and upper case of these are the same so it can't have special characters to apply multiplier
        if (!isupper(word[i]) || !isalnum(word[i])) {
            amplifier = 1;
        }
        //Convert to lower case
        word[i] = tolower(word[i]);
    }

    return amplifier;
}



//Checks for positive and negative amplifiers by comparing strings
float intensifiers(char *word) {

    //List of provided positive and negative amplifiers
    char *positive_amplifiers[] ={"absolutely", "completely", "extremely", "really", "so", "totally", "very", "particularly", "exceptionally", "incredibly", "remarkably"};
    char *negative_amplifiers[] = {"barely", "hardly", "scarcely", "somewhat", "mildly", "slightly", "partially", "fairly", "pretty much"};

    //Gets lengths of arrays in case any new intensifers are to be added
    int len_positive = sizeof(positive_amplifiers)/sizeof(*positive_amplifiers);
    int len_negative = sizeof(negative_amplifiers)/sizeof(*negative_amplifiers);
    
    float boost_factor = 0.293;

    //Positive string compare which is also considers capitalization just in case
    for (int i = 0; i < len_positive; i++) {
        if (strcasecmp(word, positive_amplifiers[i]) == 0) { //Ensures compare is same case
            return boost_factor;
        }
    }
    //Negative string compare which is also considers capitalization just in case
    for (int i = 0; i < len_negative; i++) {
        if (strcasecmp(word, negative_amplifiers[i]) == 0) {
            return boost_factor;
        }
    }
    return 0;
}



//Checks for reversal words that could change the sign of a sentiment word by comparing strings
float negations(char *word) {

    char *negations[] = {"not", "isn't", "doesn't", "wasn't", "shouldn't", "won't", "cannot", "can't", "nor", "neither", "without", "lack", "missing", "don't"};

    //Gets lengths of arrays in case any new negations are to be added
    int len_negations = sizeof(negations)/sizeof(*negations);

    float negation = -0.5;

    //Negations string compare which is also considers capitalization just in case
    for (int i = 0; i < len_negations; i++) {
        if (strcasecmp(word, negations[i]) == 0) {
            return negation;
        }
    }
    return 1;
}



//Calculate punctuation count to apply appropriate multiplier boost
float punctuationBoost(char punctuation, float punctuation_count, float sentiment) {
    //If punctuation is an intensifer (for our simplified case this is just an exclimation point)
    if (punctuation == '!') {
        //If the punctuation count multiplier has a megnitude of under 3 (3 exclimations max, any more don't add to sentiment)
        if (fabs(punctuation_count) < 3) {
            //If overall sentiment is negative the punctuation count should polarize it more to the negative side
            if (sentiment < 0) {
                punctuation_count--;
            }
            //If overall sentiment is positive or zero the punctuation count should polarize it more to the positive side
            else {
                punctuation_count++; //THIS meant to be positive for positive and negative for negative
            } 
        }
    }
    return punctuation_count;
}



//Checks compounding of negations (e.g. not not funny) also just checks for any reversals that should be stored
//Void function with pointers so that we can update it in the main function without needing to return a value
void negationCompoundStatus(char *word_temp, int *sequential_reversal, float *reversal) {

    //If the negations are sequential then compound them
    if (*sequential_reversal != 0) {
        //Compounds for semantics like not not funny which should be positive but much weaker
        *reversal *= negations(word_temp);
    }
    else {
        //This will trigger if this is the first negation in the sequence 
        *reversal = negations(word_temp);
        *sequential_reversal = 1; //Update flag to indicate a potential sequence 
    }
}



//If the reset_buffer flag triggers then this function resets all stored sentiment meaning sentiment from previous words is no longer carried over
void resetStoredSentiment(int *reset_buffer, float *add_to_sentiment, float *reversal) {
    //if flag for the buffer gets to zero
    if (*reset_buffer == 0) {
        //Reset all stored setiment values to default
        *add_to_sentiment = 0;
        *reversal = 1;
        *reset_buffer = 0;
    } 
}



//Applies all logic and semantic nuances considered to prepare for sentiment calculation 
void sentimentLogic(char bridge_character, char *word_temp, float *intensifier, float *is_caps, float *add_to_sentiment, int *sequential_reversal, int *reset_buffer, float *reversal) {

    //Update ampifiers to current word
    *intensifier = intensifiers(word_temp); 
    *is_caps = checkCaps(word_temp);

    //This makes it so that precurser words are properly stored and increased with each additional precurser word until they can be applied and negated
    if (lookup(word_temp) == 0 ) {
        if (*intensifier != 0) {
            *add_to_sentiment += *intensifier * (*is_caps);
            *sequential_reversal = 0; //If the current word is an intensifer it cannot be an negation so this flag updates to say negations are not sequential and it won't compound
        }
        else if (negations(word_temp) != 1) {
            negationCompoundStatus(word_temp, sequential_reversal, reversal);
        }
        else {
            *sequential_reversal = 0; //If current negation is 1 then we know there isn't a sequence
        }
        *reset_buffer = 1;
    }
    //This statements purpose is to prevent the next one from running under this condition
    else if (bridge_character == ',' ) {
        *sequential_reversal = 0; //If the current character is an index, this represents a break in the sequential reversal pattern
    }  
    else if (*reset_buffer > 0 && lookup(word_temp) != 0) {
        (*reset_buffer)--;
        *sequential_reversal = 0; //If the current word has a sentiment value it cannot be an negation so this flag updates to say negations are not sequential and it won't compound
    }
}



//Calculates updated sentiment score from all modifers 
void sentimentUpdate(char *word_temp, float *sentiment, float is_caps, float reversal, float add_to_sentiment, float punctuation_count, float punctuation_boost) {

    //New sentiment is adding the capitlization of current word multiplied by negations from previous words multiplied by sentiment of word 
    //plus the stored sentiment which is intensifiers and caps from previous words multplied by the caps of the current word multiplied by sentiment of current word (adds stored sentiment logic)
    //plus the puncutation boost factor times the incidences of the punctuation less than or equal to three
    *sentiment += is_caps * reversal * lookup(word_temp)
                + add_to_sentiment * is_caps * lookup(word_temp) 
                + punctuation_count * punctuation_boost;
}



//Calculates componded sentiment with various logic and semantic rules applied
float sentimentCalculation(char *sentence) {

    //If there is no sentence just return 0
    if (sentence == NULL) {
        return 0;
    }
    //Initilize variables needed 
    int len = strlen(sentence);
    char word_temp[256] = {0}; //Stores each word in the sentence until sentiment is calculated (i.e. tokenization is only temporary to save memory)
    
    float compound = 0; //Initial compound score
    float sentiment = 0; //Initial sentiment
    int temp_index = 0; //Index for temporary word to properly stores sequentially

    float alpha = 15; //normalization constant

    float intensifier = 0; //Intensifiers variable 
    float is_caps = 1; //Caps amplifier variable
    float reversal = 1; //Reversal factor variable (stored factor)

    float add_to_sentiment = 0; //Stored sentiment multiplier for precurser words (stored factor)
    int sequential_reversal = 0; //Stored sentiment reversal multipliers which checks for compounding reversals (e.g. not not funny)
    
    int reset_buffer = 0; //Delays reset of stored sentiment modifers until they need to be applied (stored factor flag)

    float punctuation_boost = 0.292; //Punctuation booster factor variable
    float punctuation_count = 0; //Iterations of punctuation boosters

    //Iterate through input sentence
    for (int i = 0; i < len; i++) {
        if (sentence[i] != ' ' && sentence[i] != '!' && sentence[i] != ',' && sentence[i] != '.' && sentence[i] != '\0')  { //How to hand punctuation that appears in emoticons?
            word_temp[temp_index] = sentence[i]; //Parse each word 
            temp_index++;
            
        } 

        punctuation_count = punctuationBoost(sentence[i], punctuation_count, sentiment); //Handels punctuation boost from exclimation points
        
        if (sentence[i] == ' ' || sentence[i] == ',' || sentence[i] == '.') {
            if (temp_index > 0) {
                
                word_temp[temp_index] = '\0'; //Null-terminates the word
                
                sentimentLogic(sentence[i], word_temp, &intensifier, &is_caps, &add_to_sentiment, &sequential_reversal, &reset_buffer, &reversal); //Proccess sentence logic to prepare for sentiment calculation
                sentimentUpdate(word_temp, &sentiment, is_caps, reversal, add_to_sentiment, punctuation_count, punctuation_boost); //Caluculate and update sentiment
                resetStoredSentiment(&reset_buffer, &add_to_sentiment, &reversal); //Reset stored sentiment if flag is 0

            }
            memset(word_temp, 0, sizeof(word_temp)); //Reset temporary word holder
            temp_index = 0; //Reset temporary index for word holder
            punctuation_count = 0; //reset cause exclimations can be on multiple words
        } 
    }

    //Checks for last word which is missed in loop
    if (temp_index > 0) {
        word_temp[temp_index] = '\0'; //Null-terminates the word
        is_caps = checkCaps(word_temp);
        //Caluculate and update sentiment
        sentimentUpdate(word_temp, &sentiment, is_caps, reversal, add_to_sentiment, punctuation_count, punctuation_boost);
    }

    compound = sentiment / sqrt(pow(sentiment, 2) + alpha); //Calculate compound sentiment
    return compound;
}