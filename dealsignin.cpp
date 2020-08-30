#include<cstdio>
#include"utility.hpp"
#include"IDandname.hpp"
using namespace std;
int dealsignin(int sockfd, User* puser, char* str, size_t n){
    IDTp IDtmp;
    char passwd[100];
    const char* msg; 

    if(IDandname(str, n, &IDtmp, passwd, 100) < 0){
        msg = "check the format. ID must be numberic and try again\n";
    }

    if(strcmp(passwd, puser->getpasswd().c_str()) == 0){
        msg = "sign in successfully!\n";
        puser->setsts(LOGINED);
    }
    else{
        msg = "wrong password, please try again.\n";
    }
    write(sockfd, msg, strlen(msg));
    return 0;
}