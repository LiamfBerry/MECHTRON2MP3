#include <stdio.h>
#include <stdint.h>

int64_t tribonacci(int n) {
    if (n==0) {
        return 0;
    }
    if (n==1||n==2) {
        return 1;
    }
    int64_t Tn = tribonacci(n-1)+tribonacci(n-2)+tribonacci(n-3);
    return Tn;
}             

int64_t tribonacci2(int n) {

    int64_t Tn_3=0,Tn_2=1,Tn_1=1;
    int64_t Tn=0;

    if (n==0) {
        return 0;
    }
    if(n==1||n==2) {
        return 1;
    }
  
    for (int i=3;i<=n;i++) {
        Tn = Tn_1 + Tn_2 + Tn_3;
        Tn_3=Tn_2;
        Tn_2=Tn_1;
        Tn_1=Tn;
    }
    return Tn;
}

int main() {
    int n = 5;
    int64_t result = tribonacci2(n);

    printf("%ld\n",result);
}

