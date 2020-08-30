#include<string>
#include<cstring>
#include<stdio.h>
#include<algorithm>
#include"dealmsg.hpp"
#include"dealloginedmsg.hpp"
#include"ultoa.hpp"

using namespace std;


int dealmsg(int sockfd, User* puser, map<unsigned long, User*>* pusersbyID,  char* str, size_t n){
    char msgbuf[100];
    if(n > 4){
        if(str[0] == '!' && str[1] == '!' && (str[2] == 'T' || str[2] == 't')&&(str[3] == 'O' || str[3] == 'o')){
            
            dealloginedmsg(sockfd, puser, pusersbyID, str, n);
            return 0;
        }
        else{
            goto msg;
        }
    }

msg:
    // find peeruser
    char*p = msgbuf;
    const char* strtmp;
    int peerusersockfd = puser->getpeeruser()->getsockfd();
    int cnt = ultoa(msgbuf, sizeof(msgbuf), puser->getID());
    p = msgbuf + cnt;
    memcpy(p, ",", 1);
    p++;
    int namelen = strlen(puser->getname().c_str());
    memcpy(p,puser->getname().c_str(), namelen);
    p += namelen;
    strtmp = ":  ";
    memcpy(p,strtmp,strlen(strtmp));
    p+=strlen(strtmp);
    memcpy(p, str, n);
    write(peerusersockfd, msgbuf, n+cnt+1+namelen+strlen(strtmp));    
    return 0;
}

