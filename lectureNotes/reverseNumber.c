#include <stdio.h>

int reverseNumber(int num) {

    int reverse = 0;

    while (num>0) {

        int digit = num % 10;
        reverse *= 10;
        reverse += digit;

        num /= 10;
    }
    return reverse;

}
int main() {
    int num = 0;
    scanf("%d",&num);
    printf("%d\n", reverseNumber(num));
}