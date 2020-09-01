#include<cstdio>
#include"utility.hpp"
#include"IDandname.hpp"
#include"dealsignin.hpp"

using namespace std;
int dealsignin(map<int, User*>::iterator iterfd, map<int, User*>* pusersbysockfd, map<IDTp, User*>* pusersbyID, char* str, size_t n){
    IDTp IDtmp;
    char passwd[100];
    const char* msg; 

    if(IDandname(str, n, &IDtmp, passwd, 100) < 0){
        msg = "check the format. ID must be numberic and try again\n";
        goto writefd;
    }
    for(auto it = pusersbyID->begin(); it != pusersbyID->end(); ++it){
        if(it->first == IDtmp){
            //cout <<"IDtmp: "<< IDtmp<<endl;
            //cout << "real ID: " << it->second->getID()<<endl;
            //cout <<"the new passwd: " << passwd<<endl;
            //cout << "the older passwd: " << it->second->getpasswd()<<endl;
            if(strcmp(passwd, it->second->getpasswd().c_str()) == 0){
                msg = "sign in successfully!\n";
                it->second->setsts(LOGINED);
                iterfd->second= it->second;
                it->second->setsockfd(iterfd->first);
                write(iterfd->first, msg, strlen(msg));
                it->second->readmsg(); 
                return 0;
            }
            else{
                msg = "wrong password, please try again.\n";

            }
            goto writefd;
        }
    }


    msg = "the user doesn't exist\n";
    goto writefd;
    
    

writefd:
    write(iterfd->first, msg, strlen(msg));
    return 0;
}