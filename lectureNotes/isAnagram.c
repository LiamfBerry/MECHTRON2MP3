#include<stdio.h>
#include<stdbool.h>
#include<string.h>

bool isAnagram(char* s, char* t) {
    //bool isAnagram(char s[], char t[]) same

    if (strlen(s)!=strlen(t)) {
        return false;
    }
    int size = strlen(s); //can add value for each letter and see if they are the same
    //then remove 1 for each that is the same, if the final result is 0 they are anagrams

    int charCount[26] = {0};

    for (int i=0; i<size; i++) {
        charCount[s[i]-'a']++; //finds corresponding index of alphabet then adds 1
        charCount[t[i]-'a']--; //substracts 1
    }

    for (int i=0; i<26; i++) {
        if (charCount[i]!=0) {
            return false;
        }
    }

    return true;
}

int main() {
    char s[] = "anagram"; 
    char t[] = "nagaram"; 

    printf("%s\n", isAnagram(s,t)? "true":"false");
    

}