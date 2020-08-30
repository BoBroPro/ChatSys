#include"utility.hpp"

int ultoa(char* str, size_t n, unsigned long num){
    int i = 0;
    int j =0;
    int cnt = 0;
    int d = 0;
    while(num){
        d = num%10;
        num /= 10;
        if(i >= n){
            return -1;
        }
        str[i] = '0' + d;
        ++i;
    }
    cnt = i;
    for(i = 0, j = cnt-1; i < j; i++, j--){
        char ch;
        ch = str[i];
        str[i] = str[j];
        str[j] = ch;
    }

    return cnt; // return the number of character copied to the string
}