#include<cstdio>
#include"utility.hpp"
#include"IDandname.hpp"
#include"dealsignin.hpp"
#include"hiredis/hiredis.h"
#include"ultoa.hpp"
#include"otherfunc.hpp"
#include"CacheUID.hpp"
using namespace std;


static void succlogin(User*puser, map<int, User*>::iterator iterfd){
    string msg = string("Sign in successfully.\n");
    puser->setsts(LOGINED);
    delete iterfd->second; 
    iterfd->second= puser;
    puser->setsockfd(iterfd->first);
    write(iterfd->first, msg.c_str(), msg.size());
    if(!puser->getpmsgsnotread()->empty()){
        msg = "Offline messages:\n";
        write(iterfd->first, msg.c_str(), msg.size());
        puser->readmsg(); 
    }
    
    writeOrModifyUserRedis("127.0.0.1", 6379,*puser);
    updateMySQLUser(*puser);
    msg = "You can input \"!!to <ID>\"to Chat with user identified by ID number.\n";
    write(iterfd->first, msg.c_str(), msg.size());
    return;
}


int dealsignin(map<int, User*>::iterator iterfd, map<int, User*>* pusersbysockfd, map<IDTp, User*>* pusersbyID, char* str, size_t n){
    IDTp IDtmp;
    char passwd[100];
    const char* msg; 
    User *ptargtuserinmap, targtuserinDb, *ptargtuser;
    int where;
    if(IDandname(str, n, &IDtmp, passwd, 100) < 0){
        msg = "check the format. ID must be numberic and try again\n";
        write(iterfd->first, msg, strlen(msg));
        return 0;
    }
    where = findUser(IDtmp, pusersbyID,&ptargtuserinmap, &targtuserinDb);
    if(where < 0){
        msg = "the user doesn't exist, please choose \"sign in\" or \"sign up\"\n";
        iterfd->second->setsts(CONNECTED);
        write(iterfd->first, msg, strlen(msg));
        return 0;
    } 
    else if(where == 0){ // in map
        ptargtuser = ptargtuserinmap;
    }
    else{
        ptargtuser = &targtuserinDb;
    }

    if(strcmp(passwd, ptargtuser->getpasswd().c_str()) == 0){

        if(ptargtuser->getpasswd() == string(passwd)){
            User* puser = new User(move(*ptargtuser));
            pusersbyID->emplace(IDtmp, puser);
            succlogin(puser, iterfd);
            return 0;
        }
        else{
            msg = "wrong password, please try again.\n";
            write(iterfd->first, msg, strlen(msg));
            return 0; 
        }
    }

    return 0;
}