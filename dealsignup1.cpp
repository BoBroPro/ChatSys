#include<cstdio>
#include"IDandname.hpp"
#include"dealsignup1.hpp"

using namespace std;

int dealsignup1(int sockfd, User* puser, char* str, size_t n){
    IDTp IDtmp;
    char passwd[100];
    const char* msg; 

    if(IDandname(str, n, &IDtmp, passwd, 100) < 0){
        msg = "check the format. ID must be numberic and try again\n";
    }
    else{
        puser->setpasswdchck(string(passwd));
        puser->setsts(INSIGNUP2);
        msg = "please input password again.\n";
    }
    write(sockfd, msg, strlen(msg));
    return 0;
}