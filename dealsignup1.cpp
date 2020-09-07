#include<cstdio>
#include"IDandname.hpp"
#include"dealsignup1.hpp"
#include"otherfunc.hpp"
using namespace std;

int dealsignup1(int sockfd, User* puser, map<IDTp, User*>* pusersbyID, char* str, size_t n){
    IDTp IDtmp;
    char passwd[100];
    string msg; 
    User* puserinmap, userinredis, userinMySQL;

    int where= 0;


    if(IDandname(str, n, &IDtmp, passwd, 100) < 0){
        msg = move(string("check the format. ID must be numberic and try again\n"));
        goto writefd;
    }
    else{
        where = findUser(IDtmp, pusersbyID, &puserinmap, &userinredis);
        if(where >= 0) {
            msg = move(string("the user has been signed up earlier,input\"sign in\" or \"sign up\"\n"));
            puser->setsts(CONNECTED);
            goto writefd;
        }

        puser->setID(IDtmp);
        puser->setpasswdchck(string(passwd));
        puser->setsts(INSIGNUP2);
        msg = "please input password again.\n";
        goto writefd;
    }

writefd:
    write(sockfd, msg.c_str(), msg.size());
    return 0;
}