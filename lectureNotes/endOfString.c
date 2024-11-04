#include <stdio.h>

void findEndOfString(const char *str) {
    while (*str != '\0') {
        str++;
    }

    printf("End of string: %c\n", *(str-1));
}

int main() {
    const char myString[] = "Hello, this is a test string.";

    findEndOfString(myString);
}