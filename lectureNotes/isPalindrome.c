#include <stdio.h>
#include <string.h>
#include <stdbool.h>

bool isPalindrome(char s[]) {

    int charCount[26] = {0};

    int size = strlen(s);

    for (int i=0;i<size/2-1;i++) {
        charCount[s[i]-'a']++;
        charCount[s[size-i-1]-'a']--;
        
    }

    for (int i=0;i<26;i++) {
        if (charCount[i]!=0) {
            return false;
        }
    }
    return true;

}

int main() {
    char s1[] = "woow";
    char s2[] = "racecar";

    printf("%s\n",isPalindrome(s1)? "true": "false"); 
    printf("%s\n",isPalindrome(s2)? "true": "false");
}