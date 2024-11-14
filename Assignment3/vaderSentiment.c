#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "lexiconUtility.h"

//NOTE: This function should only be run if we already know the word is fully capitalized so we have to impliment a check for it
//non alpha numeric character is always caps and not so add flag for this
float checkCaps(char *word) {
    int len = strlen(word);

    float amplifier = 1.5; //flag to check if it's all capitals

    //If it isn't all then it's not intensified
    for (int i = 0; i < len; i++) {
        if (!isupper(word[i]) || !isalnum(word[i])) {
            amplifier = 1;
        }
        word[i] = tolower(word[i]);
    }

    return amplifier;
}

float intensifiers(char *word) {

    char *positive_amplifiers[] ={"absolutely", "completely", "extremely", "really", "so", "totally", "very", "particularly", "exceptionally", "incredibly", "remarkably"};
    char *negative_amplifiers[] = {"barely", "hardly", "scarcely", "somewhat", "mildly", "slightly", "partially", "fairly", "pretty much"};
    
    float boost_factor = 0.293;

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

    char *negations[] = {"not", "isn't", "doesn't", "wasn't", "shouldn't", "won't", "cannot", "can't", "nor", "neither", "without", "lack", "missing", "don't"};

    float negation = -0.5;

    //Negations string compare 
    for (int i = 0; i < 14; i++) {
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
    float add_to_sentiment = 0;
    float reversal = 1;
    int reset_buffer = 0;

    float punctuation_count = 0;
    float punctuation_boost = 0.292;


    //Iterate through input sentence
    for (int i = 0; i < len; i++) {
        if (sentence[i] != ' ' && sentence[i] != '!' && sentence[i] != ',' && sentence[i] != '.' && sentence[i] != '\0')  { //How to hand punctuation that appears in emoticons?
            word_temp[temp_index] = sentence[i]; //Parse each word 
            temp_index++;
            
        }  
        //Handels punctuation boost from exclimation points
        else if (sentence[i] == '!') {

            if (fabs(punctuation_count) < 3) {
                if (sentiment < 0) {
                    punctuation_count--;
                }
                else {
                    punctuation_count++; //THIS meant to be positive for positive and negative for negative
                } 
            }
        }
        else {
            if (temp_index > 0) {
                
                word_temp[temp_index] = '\0'; //Null-terminates the word
                word_count++; //Increase word count
                amplifier = intensifiers(word_temp) * checkCaps(word_temp); //Since intensifiers are allways precurser words this should work

                //This makes it so that precurser words are properly stored for one additional loop before being reset (i.e. without being overwritten it gets through a full loop then is reset on the second loop)
                if (lookup(word_temp) == 0 ) {
                    if (amplifier != 1) {
                        add_to_sentiment += amplifier;
                    }
                    else if (negations(word_temp) != 1) {
                        reversal *= negations(word_temp);
                    }
                    reset_buffer = 1;
                }
                else if (sentence[i] == ',' ) {
                    ; //This statements purpose is to prevent the next one from running under this condition
                }  
                else if (reset_buffer > 0 && lookup(word_temp) != 0) {
                    reset_buffer--;
                }
                     
              
                sentiment += amplifier * reversal * lookup(word_temp)
                           + amplifier * add_to_sentiment * lookup(word_temp) 
                           + punctuation_count * punctuation_boost; //Add to sentiment 
                printf("Amplifier %f\n", amplifier);
                printf("Add to sentiment %f\n", add_to_sentiment);
                printf("Reversal %f\n", reversal);
                printf("amplifier * reversal * lookup(word_temp) %f\n", amplifier * reversal * lookup(word_temp));
                printf("Sentiment %f\n", sentiment);
                
                if (reset_buffer == 0) {
                        add_to_sentiment = 0;
                        reversal = 1;
                        reset_buffer = 0;
                } 
            }
            memset(word_temp, 0, sizeof(word_temp)); //Reset temporary word holder
            temp_index = 0; //Reset temporary index for word holder
            punctuation_count = 0; //reset cause exclimations can be on multiple words
            
        } 
    }
    //Checks for last word which is missed in loop
    if (temp_index > 0) {
        word_temp[temp_index] = '\0'; //Null-terminates the word
        amplifier = intensifiers(word_temp) * negations(word_temp) * checkCaps(word_temp);
        sentiment += amplifier * reversal * lookup(word_temp) + amplifier * add_to_sentiment * lookup(word_temp) + punctuation_count * punctuation_boost; //Add to sentiment 
        printf("Amplifier %f\n", amplifier);
        printf("Add to sentiment %f\n", add_to_sentiment);
        printf("Reversal %f\n", reversal);
        printf("amplifier * reversal * lookup(word_temp) %f\n", amplifier * reversal * lookup(word_temp));
        printf("Sentiment %f\n", sentiment);
    }

    float compound = sentiment / sqrt(pow(sentiment, 2) + alpha);

    return compound;
}

//Check for typecasting

//uber and FRIGGIN, not needed to be included 
//not very funny so wait until there is a word it can be applied to <-- do this 
//^ seperate negations vs intensifiers since they are applied to the word differently  (DONE)

//not smart, handsome, or funny <-- needs bridging conditions 

//Do need logic for chaining
//Do need to stack intensifiersand precursers until they are applied (DONE)

//make Exclimations a function

//Remeber to impliment free function
