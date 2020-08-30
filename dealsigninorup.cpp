#include<stdio.h>
#include<string>
#include"dealsigninorup.hpp"

using namespace std;

int dealsigninorup(int sockfd, User* puser, char* str, size_t n){
    const char* msg;
    
    if(strcmp(str, "sign in\n") == 0){
        msg = "please input ID and password,divided by \",\" for sign in\n";
        puser->setsts(INSIGNIN);
    }

    else if(strcmp(str, "sign up\n") == 0){
        msg = "please input ID and password,divided by \",\" for sign up\n";
        puser->setsts(INSIGNUP1);
    }
    else{
        msg = "input error! please input \"sign in\" or \"sign up\"\n";
    }
    write(sockfd,msg,strlen(msg));
    return 0;
}