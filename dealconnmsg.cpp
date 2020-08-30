#include<stdio.h>
#include<string>
#include"dealconnmsg.hpp"
#include"IDandname.hpp"

using namespace std;

using mp = map<unsigned long, User*>;

int dealconnmsg(int sockfd, User* puser, mp* pusersbyID,  char* str, size_t n){

    char namebuftmp[19];
    unsigned long IDtmp;
    int reterr = IDandname(str,n, &IDtmp, namebuftmp, sizeof(namebuftmp));

    const char* againbuf;
    if(reterr < 0){
        if(reterr = -1){
            againbuf = "ID is not a number\n";
        }
        else if(reterr == -2){
            againbuf = "name is too long\n";
        }
        else{
            //nothing
        }
        //memcpy(sendline, againbuf, strlen(againbuf));
        write(sockfd, againbuf, strlen(againbuf));
    }
    else{
        puser->setID(IDtmp);
        puser->setname(string(namebuftmp));
        puser->setsts(LOGINED);
        puser->setsockfd(sockfd);
        pusersbyID->emplace(IDtmp, puser);
        

        const char* msgtmp = "input peer ID, such as \"!!to 12345\" starting with \"!!to\"\n";
        write(sockfd, msgtmp, strlen(msgtmp));
    }
    return 0;
}