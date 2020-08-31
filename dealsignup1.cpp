#include<cstdio>
#include"IDandname.hpp"
#include"dealsignup1.hpp"

using namespace std;

int dealsignup1(int sockfd, User* puser, map<IDTp, User*>* pusersbyID, char* str, size_t n){
    IDTp IDtmp;
    char passwd[100];
    const char* msg; 

    if(IDandname(str, n, &IDtmp, passwd, 100) < 0){
        msg = "check the format. ID must be numberic and try again\n";
        goto writefd;
    }
    else{
        for(auto it = pusersbyID->begin(); it != pusersbyID->end(); ++it){
            if(it->first == IDtmp){
                msg = "user exist,please input \"sign in or sign up\"again\n";
                puser->setsts(CONNECTED);
                goto writefd;
            }
        } 

        puser->setID(IDtmp);
        puser->setpasswdchck(string(passwd));
        puser->setsts(INSIGNUP2);
        msg = "please input password again.\n";
        goto writefd;
    }

writefd:
    write(sockfd, msg, strlen(msg));
    return 0;
}