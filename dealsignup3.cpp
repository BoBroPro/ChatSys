#include<cstdio>
#include"dealsignup3.hpp"
#include<map>
#include"hiredis/hiredis.h"
#include"hiredisutility.hpp"
#include"ultoa.hpp"
#include"json.hpp"
#include"otherfunc.hpp"
#include"CacheUID.hpp"

using namespace std;
using json = nlohmann::json;
int dealsignup3(int sockfd, User* puser, map<IDTp, User*>* pmp, char* str, size_t n){
    char nametmp[100];
    const char* msg; 
    int i;
    for(i = 0; i < n && str[i]!='\n' && str[i]!='\0'; ++i){
        nametmp[i] = str[i];
    }
    nametmp[i] = 0;
    puser->setname(string(nametmp));
    msg = "sign up successfully, and you have signed in.\nyou can input \"!!to <ID>\"to Chat with user identified by ID number.\n";
    puser->setsts(LOGINED);
    puser->setsockfd(sockfd);

    CacheUID::push(*puser);
    if(writeMySQLUser(*puser) < 0){

        cout << "Failed writing user data to MySQL"<< endl;
    }

    write(sockfd, msg, strlen(msg));
    return 0;
}