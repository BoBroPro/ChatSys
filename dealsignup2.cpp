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
        msg = "please input your name\n";
        puser->setsts(INSIGNUP3);
        puser->setpasswd(string(passwd));
        cout << "the passwd: " << string(passwd)<<endl;
    }
    else{
        msg = "password inconsistency. input ID and password again\n";
        puser->setsts(INSIGNUP1); 
    }
    write(sockfd, msg, strlen(msg));
    return 0;
}