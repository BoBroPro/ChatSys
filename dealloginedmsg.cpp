#include<string>
#include<cstring>
#include<stdio.h>
#include<algorithm>
#include"dealloginedmsg.hpp"
#include"extractID.hpp"
#include"ultoa.hpp"

using namespace std;


/*
int main(){
    char arr[10];
    int cnt = ultoa(arr, 10, 123);
    cout << "cnt: " << cnt << endl<< "arr: " << arr<<endl;
}
*/
int dealloginedmsg(int sockfd, User* puser, map<unsigned long, User*>* pusersbyID,  char* str, size_t n){

    const char* strtmp;
    string msg;
    char IDbuf[20];
    if(n < 2){
        goto formatmsg; // ID format error
    }
    if(str[0] == '!' && str[1] == '!'){
        unsigned long IDtmp;
        str = str + 2;
        if(extractID(str, n-2, &IDtmp) < 0){
            goto formatmsg;
        }
        else{
            auto it = pusersbyID->find(IDtmp);
            if(it != pusersbyID->end()){
                char* p; 
                int cnt, ntmp;

                puser->setpeeruser(it->second);
                it->second->setpeeruser(puser);
                puser->setsts(PEERSET);

                msg = string("Find the user:(");
                if((cnt = ultoa(IDbuf, sizeof(IDbuf), IDtmp)) < 0){
                    // pending buf is to small.
                }
                else{
                    msg.append(string(IDbuf)).append(",").append(it->second->getname());
                    if(it->second->getsts()!= PEERSET && it->second->getsts() != LOGINED){
                        msg.append("), but the peer doesnt sign in. you can still send messages and the peer will read the message untill signing in\n");
                    }
                    else{
                        it->second->setsts(PEERSET);
                        msg.append(string(IDbuf)).append(",").append(it->second->getname()).append("), and you can chat now!\n");
                    }
                }
            }
            else{
                msg = string("This user does not exist\n");
            }
            write(sockfd, msg.c_str(), msg.size());
            return 0;
        }
    }
    else{
        goto formatmsg;
    }

formatmsg:
    msg = move(string("format of input error, (like\"!!to 1234\")")); 
    write(sockfd, msg.c_str(), msg.size());
    return 0;
};