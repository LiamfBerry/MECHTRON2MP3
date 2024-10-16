#include <stdbool.h>
#include <stdio.h>

bool isPalindrome(int x) {
    
    if (x < 0 || (x % 10 == 0 && x != 0)) {
        return 0; 
    }

    int reversed = 0;
    while (x > reversed) {
        reversed = reversed * 10 + x % 10;
        x /= 10; 
    }

    return x == reversed || x == reversed / 10; // Handle odd-length numbers
}

int main() {
    int x = 121;
    printf("%s\n",isPalindrome(x)?"true":"false");
}