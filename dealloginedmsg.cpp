#include<string>
#include<cstring>
#include<stdio.h>
#include<algorithm>
#include"dealloginedmsg.hpp"
#include"extractID.hpp"
#include"ultoa.hpp"
#include"otherfunc.hpp"

using namespace std;


/*
int main(){
    char arr[10];
    int cnt = ultoa(arr, 10, 123);
    cout << "cnt: " << cnt << endl<< "arr: " << arr<<endl;
}
*/
static string onlinemsg(User& userfound, User* puser, string IDstr){
    string msg;
    puser->setpeeruser(&userfound);
    userfound.setpeeruser(puser);
    puser->setsts(PEERSET);
    userfound.setsts(PEERSET);
    msg = move(string("Find the user:("));
    msg.append(IDstr).append(",").append(userfound.getname()).append("), and you can chat now!\n");
    return msg;
}

static string offlinemsg(User& userfound, User* puser, string IDstr){
    string msg;
    puser->setpeeruser(&userfound);
    puser->setsts(PEERSET);
    msg = move(string("Find the user:(")).append(IDstr);
    msg.append("), but the peer doesn't sign in.\nYou can still send messages, which will be shown when the user signs in\n");
    return msg;
}




int dealloginedmsg(int sockfd, User* puser, map<unsigned long, User*>* pusersbyID,  char* str, size_t n){

    const char* strtmp;
    string msg;
    char IDbuf[20];
    string IDstr;
    
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
            if((ultoa(IDbuf, sizeof(IDbuf), IDtmp)) < 0){
                // pending buf is to small.
            }
            IDstr=string(IDbuf);

            auto it = pusersbyID->find(IDtmp);
            if(it == pusersbyID->end()){
                User user;
                if(findinredis("127.0.0.1",6379,IDtmp, user) < 0){
                    //find in MySQL....
                    msg = string("This user does not exist\n");
                }
                else{
                    User* puserfound = new User(move(user));
                    pusersbyID->emplace(puserfound->getID(),puserfound);
                    msg = offlinemsg(*puserfound,puser, IDstr);
                }
            }
            else{
                msg = onlinemsg(*it->second, puser, IDstr);
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