#include<cstdio>
#include"dealsignup2.hpp"

using namespace std;

int dealsignup2(int sockfd, User* puser, char* str, size_t n){
    char passwd[100];
    const char* msg; 
    int i;
    for(i = 0; i < n && str[i]!='\n' && str[i]!='\0'; ++i){
        passwd[i] = str[i];
    }
    passwd[i] = 0;
    if(strcmp(passwd, puser->getpasswdchck().c_str()) == 0){
        msg = "sign up successfully, and you have signed in\n";
        puser->setsts(LOGINED);
    }
    else{
        msg = "password inconsistency. input password again\n";
        puser->setsts(INSIGNUP1); 
    }
    write(sockfd, msg, strlen(msg));
    return 0;
}