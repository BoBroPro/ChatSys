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

    char msg[50];
    const char* strtmp;
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
                it->second->setsts(PEERSET);

                strtmp = "Find the user:(";
                ntmp = strlen(strtmp);
                memcpy(msg, strtmp, ntmp);
                p = msg + ntmp;
                if((cnt = ultoa(p, sizeof(msg)-ntmp, IDtmp)) < 0){
                    // pending buf is to small.
                }
                else{
                    p += cnt; 
                    p[0] = ',';
                    p++;
                    const char* name = it->second->getname().c_str();
                    memcpy(p, name, strlen(name));
                    p += strlen(name);
                    strtmp = "), and you can chat now!\n";
                    memcpy(p, strtmp, strlen(strtmp));
                    p[strlen(strtmp)] = 0;
                }
            }
            else{
                strtmp = "This user is not logined\n";
                memcpy(msg, strtmp, strlen(strtmp));
                msg[strlen(strtmp)] = 0;
            }
            write(sockfd, msg, strlen(msg)+1);
            return 0;
        }
    }
    else{
        goto formatmsg;
    }

formatmsg:
    strtmp = "format of input error, (like\"!!to 1234\")"; 
    memcpy(msg, strtmp, strlen(strtmp)+1);
    write(sockfd, msg, strlen(msg)+1);
    return 0;
};