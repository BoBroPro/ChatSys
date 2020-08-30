#include"extractID.hpp"
#include"ctype.h"

int extractID(char* str, int n, unsigned long* ID){
    if(n < 5){
        return -1;
    }
    if((str[0] == 'T' || str[0] == 't') && (str[1] == 'O' || str[1] == 'o')){
        int i = 0;
        for(i = 2; i < n; i++){
            if(!isdigit(str[i])){
                continue;
            }
            else{
                break;
            }
        }
        if(i == n){
            return -1;
        }
        *ID = 0;
        for(; i< n; ++i){
            if(isdigit(str[i])){
                *ID = *ID * 10 + (str[i] -'0');
            }
            else{
                break;
            }
        }
        return 0;

    }                           
    else{
        return -1;
    }
}