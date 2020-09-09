#include<string>
#include<cstring>
#include<stdio.h>
#include<algorithm>
#include"checkonline.hpp"
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
static string onlinemsg(User& userfound, User& user, string IDstr){
    string msg;
    user.setpeerID(userfound.getpeerID());
    user.setppeeronline(&userfound);

    user.setsts(PEERSET);
    msg = move(string("Find the user:("));
    msg.append(IDstr).append(",").append(userfound.getname()).append("), and you can chat now!\n");
    return msg;
}

static string offlinemsg(User& userfound, User& user, string IDstr){
    string msg;
    user.setpeerID(userfound.getID());
    user.setsts(PEERSET);
    msg = move(string("Find the user:(")).append(IDstr);
    msg.append("), but the peer doesn't sign in.\nYou can still send messages, which will be shown when the user signs in\n");
    return msg;
}




int checkonline(int sockfd, User* puser, map<unsigned long, User*>* pusersbyID,  char* str, size_t n){

    const char* strtmp;
    string msg;
    char IDbuf[20];
    string IDstr;
    User *puserinmap, userinDb, *ppeeruser;
    int where;
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

            where = findUser(IDtmp,pusersbyID,&puserinmap,&userinDb);
            if(where < 0){
                msg = string("This user does not exist\n");
            }
            else if(0 == where){
                msg = onlinemsg(*puserinmap, *puser, IDstr);
    
            }
            else {
                msg = offlinemsg(userinDb,*puser, IDstr);
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