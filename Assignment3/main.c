#include <stdio.h>
#include "vaderUtility.h"
#include "lexiconUtility.h"

int main() {

    initHashTable();

    readVader();

    //printTable();

    char test[256] = ":D :-) ;] ";

    printf("%f\n", sentimentCalculation(test));

    return 0;
}