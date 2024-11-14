#include <stdio.h>
#include "vaderUtility.h"
#include "lexiconUtility.h"

int main() {

    initHashTable();

    readVader();

    //printTable();

    char test[1024] = "not not funny";

    printf("%f\n", sentimentCalculation(test));

    return 0;
}