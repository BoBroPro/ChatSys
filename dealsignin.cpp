#include<cstdio>
#include"utility.hpp"
#include"IDandname.hpp"
#include"dealsignin.hpp"
#include"hiredis/hiredis.h"
#include"ultoa.hpp"
#include"otherfunc.hpp"

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
    msg = "You can input \"!!to <ID>\"to Chat with user identified by ID number.\n";
    write(iterfd->first, msg.c_str(), msg.size());
    return;
}


int dealsignin(map<int, User*>::iterator iterfd, map<int, User*>* pusersbysockfd, map<IDTp, User*>* pusersbyID, char* str, size_t n){
    IDTp IDtmp;
    char passwd[100];
    const char* msg; 
    map<IDTp,User*>::iterator it;

    if(IDandname(str, n, &IDtmp, passwd, 100) < 0){
        msg = "check the format. ID must be numberic and try again\n";
        goto writefd;
    }
    
    for(it = pusersbyID->begin(); it != pusersbyID->end(); ++it){
        if(it->first == IDtmp){
            //cout <<"IDtmp: "<< IDtmp<<endl;
            //cout << "real ID: " << it->second->getID()<<endl;
            //cout <<"the new passwd: " << passwd<<endl;
            //cout << "the older passwd: " << it->second->getpasswd()<<endl;
            if(strcmp(passwd, it->second->getpasswd().c_str()) == 0){
                succlogin(it->second, iterfd);
                return 0;
            }
            else{
                msg = "wrong password, please try again.\n";
                goto writefd;
            }
        }
    }
    if(it == pusersbyID->end()){
        User user;
        if(findinredis("127.0.0.1",6379,IDtmp,user) < 0){ // not find the user in redis
            //mysql
        }
        else{
            if(user.getpasswd() == string(passwd)){
                User* puser = new User(move(user));
                pusersbyID->emplace(IDtmp, puser);
                cout << "sign in: " <<iterfd->first <<" " <<puser->getID()<<endl;
                succlogin(puser, iterfd);
                return 0;
            }
            else{
                msg = "wrong password, please try again.\n";
                cout<<"in redis" <<endl;
                goto writefd;
            }
        }
    }


    msg = "the user doesn't exist\n";
    goto writefd;
    

writefd:
    write(iterfd->first, msg, strlen(msg));
    return 0;
}