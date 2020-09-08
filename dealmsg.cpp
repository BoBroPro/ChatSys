#include<string>
#include<stdio.h>
#include<algorithm>
#include"dealmsg.hpp"
#include"dealloginedmsg.hpp"
#include"ultoa.hpp"
#include<iterator>
#include"puserfrommapID.hpp"
#include"otherfunc.hpp"
#include"CacheUID.hpp"

using namespace std;

int mergestrIDstr(string* str1, IDTp ID, string str2){
    char resstr[20];
    if(ultoa(resstr, sizeof(resstr),ID) < 0){
        return -1;
    };
    (*str1).append(string(resstr)).append(str2);
    return 0;
}

static string userinfo2str(User* puser){
    string res;
    char IDstr[20];
    ultoa(IDstr,20,puser->getID());
    cout << "flaguserinfo2str ID: " << IDstr<<endl;
    res.append(string(IDstr)).append(", ").append(puser->getname());
    if(puser->getsts() == LOGINED || puser->getsts() == PEERSET){
        res.append(", Logined\n");
    }
    else{
        res.append(", NoLogin\n");
    }
    return res;
}

static bool equalchars(const char* str1, const char* str2, int str2len){
    int i = 0;
    int n = str2len;
    for(i = 0; i < n && str1[i] == str2[i]; ++i){}
    return i == n;
}


static void addfrd(int sockfd, User* puser, map<IDTp, User*>* pusersbyID, IDTp frdID){
    string msg;
    map<IDTp, User*>::iterator it;
    User* pfrdinmap;
    User frdinDb; 
    User* pfrd;
    int where;
     
    auto itmyaddfrd = puser->findIDinaddfrds(frdID);
    if(itmyaddfrd != puser->getpaddfrds()->end()){
        msg = move(string("the user is already in your friend request list\n"));
        write(sockfd, msg.c_str(), msg.size());
        return ;
    }
    // check where peer has been your friend yet or not.
    auto itmyfrd = puser->findIDinfrds(frdID);
    if(itmyfrd != puser->getpfrds()->end()){
        msg = move(string("the peer is already your friend\n"));
        write(sockfd, msg.c_str(), msg.size());
        return;
    }
    

    where = findUser(frdID, pusersbyID, &pfrdinmap, &frdinDb);
    // no this user
    if(where < 0){
        msg = string("this user(ID:"); 
        mergestrIDstr(&msg, frdID,") doesn't exists\n");
        write(sockfd, msg.c_str(), msg.size());
        return;
    }
    //in the map
    if(where == 0){
        pfrd = pfrdinmap;
    }
    // int the redis or mysql where == 1 or where == 2;
    else{
        pfrd = &frdinDb;
    }

    // check whether you are in the peer's friend list
    auto itpeerfrd = pfrd->findIDinfrds(puser->getID());
    if(itpeerfrd != pfrd->getpfrds()->end()){ // in the peer's friend list.
        puser->getpfrds()->push_back(frdID); // add in your friend list not friend request list.
        CacheUID::push(*puser);        
        //writeOrModifyUserRedis("127.0.0.1", 6379,*puser);
        updateMySQLUser(*puser);
    }
    else{
        auto itpeerfrd = pfrd->findIDinvrfyfrd(puser->getID());
        if(itpeerfrd == pfrd->getpvrfyfrds()->end()){
            //add peer's friend verificaion queue;
            pfrd->getpvrfyfrds()->push_back(puser->getID());
        }
        puser->getpaddfrds()->push_back(frdID);
        CacheUID::push(*puser);        
        //writeOrModifyUserRedis("127.0.0.1", 6379,*puser);
        CacheUID::push(*pfrd);        
        //writeOrModifyUserRedis("127.0.0.1", 6379,*pfrd);
        updateMySQLUser(*puser);
        updateMySQLUser(*pfrd);
    }
    // send message
    msg = string("a freind(ID: ");
    mergestrIDstr(&msg,frdID, ",");
    msg.append(pfrd->getname()).append(") requst has been sent\n");
    write(sockfd, msg.c_str(),msg.size()); 
    return;
}

static void acptfrd(int sockfd, User* puser, map<IDTp, User*>* pusersbyID, IDTp frdID){

    //list<IDTp>* pvrfyfrds = puser->getpvrfyfrds();
    User *pfrdinmap, *pfrd, frdinredis;
    int where = 0;
    list<IDTp>::iterator itfrd, itaddfrd, itvrfyfrd;
    string msg;

    //check vrfyfriend list 
    itvrfyfrd = puser->findIDinvrfyfrd(frdID);
    // found nothing
    if(itvrfyfrd == puser->getpvrfyfrds()->end()){
        msg = string("the friend witvrfyfrdsh ID:");
        mergestrIDstr(&msg, frdID, " is not in your friend request list\n");
        write(sockfd,msg.c_str(), msg.size());
        return ;
    }
    puser->getpvrfyfrds()->erase(itvrfyfrd);

    // check self addfriend list
    itfrd = puser->findIDinaddfrds(frdID);
    if(itfrd != puser->getpaddfrds()->end()){
        puser->getpaddfrds()->erase(itfrd);
    }
    // add the frdID in friend list;
    //check friend list
    itfrd = puser->findIDinfrds(frdID);
    if(itfrd == puser->getpfrds()->end()){
        puser->getpfrds()->push_back(frdID);
    }

    where = findUser(frdID, pusersbyID, &pfrdinmap, &frdinredis);
    if(where < 0){ // no this user;
        msg = string("the user with ID:");
        mergestrIDstr(&msg, frdID, " doesn't exit\n");
        write(sockfd,msg.c_str(), msg.size());
        return ;
    }
    if(0 == where){ // in the map;
        pfrd = pfrdinmap;
    }
    else{ // in the redis or mysql
        pfrd = &frdinredis;
    }

    itaddfrd = pfrd->findIDinaddfrds(puser->getID());
    if(itaddfrd != pfrd->getpaddfrds()->end()){
        pfrd->getpaddfrds()->erase(itaddfrd);
    }

    itfrd = pfrd->findIDinvrfyfrd(puser->getID());
    if(itfrd != pfrd->getpvrfyfrds()->end()) {
        pfrd->getpvrfyfrds()->erase(itfrd);
    }

    itfrd = pfrd->findIDinfrds(puser->getID());
    if(itfrd == pfrd->getpfrds()->end()){
        // the peer pushs the user into the friend list.
        pfrd->getpfrds()->push_back(puser->getID());
    }

    CacheUID::push(*puser);        
    CacheUID::push(*pfrd);        
    //writeOrModifyUserRedis("127.0.0.1", 6379,*puser);
    //writeOrModifyUserRedis("127.0.0.1", 6379,*pfrd);
    updateMySQLUser(*puser);
    updateMySQLUser(*pfrd);

    msg = string("has accepted the friend(");
    mergestrIDstr(&msg, frdID,",");
    msg.append(pfrd->getname()).append(") request\n");
    write(sockfd, msg.c_str(), msg.size());
    return;
}

static void dltfrd(int sockfd, User* puser, map<IDTp, User*>* pusersbyID, IDTp frdID){
    list<IDTp>*pfrds = puser->getpfrds();
    list<IDTp>*ppeerfrds;
    list<IDTp>::iterator itme, itpeer;
    string msg;

    auto itmyfrd = puser->findIDinfrds(frdID);
    if(itmyfrd == puser->getpfrds()->end()){
        msg = string("err, the user(");
        mergestrIDstr(&msg, frdID,",");
        msg.append(") is not your friend\n");
        write(sockfd, msg.c_str(), msg.size());
        return ;
    }

    pfrds->erase(itmyfrd);
    CacheUID::push(*puser);        
    //writeOrModifyUserRedis("127.0.0.1", 6379,*puser); 
    updateMySQLUser(*puser);
    return ;
}

static void listfrdonline(int sockfd, User*puser, map<IDTp, User*>* pusersbyID){
    list<IDTp>* pfrds = puser->getpfrds();
    string userinfo;
    for(auto it = pfrds->begin(); it != pfrds->end(); ++ it){
        map<IDTp, User*>::iterator itusersbyID;
        if((itusersbyID = pusersbyID->find(*it)) != pusersbyID->end() && \
         itusersbyID->second->getsts()==LOGINED){

            userinfo = userinfo2str(itusersbyID->second);
            write(sockfd, userinfo.c_str(), strlen(userinfo.c_str()));
        }
    }
    return;
}



static void listfriends(int sockfd, list<IDTp>*pfrds, map<IDTp, User*>* pusersbyID){
    string userinfo;
    User userinredis, *puserinmap, *pfrd;
    int where = 0; 
    for(auto it = pfrds->begin(); it != pfrds->end(); ++it){
        where = findUser(*it, pusersbyID, &puserinmap, &userinredis);
        if(where < 0){
            string msg("the user(");
            mergestrIDstr(&msg, *it, ")doesn't exist\nand deleted it in your friend list\n");
            write(sockfd, msg.c_str(),msg.size()); 
            continue;
        }
        else if(where ==0 ){ // in the map
            pfrd = puserinmap;
            cout << " find: ID:" <<pfrd->getID()<<" in map" <<endl;
        }
        else{ // in the redis or mysql;
            pfrd = &userinredis; 
            CacheUID::push(*pfrd);        
            cout << " find: ID:" <<pfrd->getID()<<" in redis or MySQL" <<endl;
        }

        userinfo = userinfo2str(pfrd);
        write(sockfd, userinfo.c_str(), strlen(userinfo.c_str()));
    }
    return;
}

static void sendmsg(int sockfd, User& user, User& peeruser,  char* str, size_t n){
    string msg;
    char IDbuf[20];
    ultoa(IDbuf, sizeof(IDbuf), user.getID());
    string IDstr = string(IDbuf);
    msg = msg.append(IDstr).append(",").append(user.getname()).append(": ");
    for(int i = 0; i< n; ++i){
        msg.push_back(str[i]);
    }
    if(peeruser.getsts()!=LOGINED && peeruser.getsts()!=PEERSET){
        peeruser.getpmsgsnotread()->push_back(msg);
        // modify user data in  redis
        CacheUID::push(peeruser);        
        //writeOrModifyUserRedis("127.0.0.1", 6379,peeruser);
        updateMySQLUser(peeruser);
        return ;
    }

    int peerusersockfd = peeruser.getsockfd();
    if(peeruser.getsts() == LOGINED){
        peeruser.setpeeruser(&user);
        peeruser.setsts(PEERSET);
    }

    write(peerusersockfd, msg.c_str(), msg.size()+1);    

    return;
} 

int dealmsg(int sockfd, User* puser, map<unsigned long, User*>* pusersbyID,  char* str, size_t n){
    string msg;
    const char* addfriend = "addfriend\n";
    const char* dltfriend = "dltfriend\n";
    const char* listfriend = "listfriend\n";
    const char* acptfriend = "acptfriend\n";
    const char* listfriendonline = "listfriendonline\n";
    const char* showcmd = "showcmd\n";
    const char* listvrfyfriend = "listvrfyfriend\n";
    const char* listaddfriend= "listaddfriend\n";
    int i = 0; 
    IDTp frdID;
    User* ppeeruser = puser->getpeeruser();
    if(n > 2){
        if(str[0] == '!' && str[1] == '!'){
            if(n > 4 && equalchars(str+2, "to", 2)){
                dealloginedmsg(sockfd, puser, pusersbyID, str, n);
            }
            else if(n > 2+strlen(addfriend) && equalchars(str+2, addfriend,strlen(addfriend)-1)){
                frdID = atol(str+2+strlen(addfriend));
                addfrd(sockfd, puser, pusersbyID,frdID);
            }
            else if(n > 2+strlen(dltfriend) && equalchars(str+2, dltfriend,strlen(dltfriend)-1)){
                frdID = atol(str+2+strlen(dltfriend));
                dltfrd(sockfd, puser, pusersbyID,frdID);
            }
            else if(n >= 2+strlen(listfriend) && equalchars(str+2,listfriend,strlen(listfriend))){
                listfriends(sockfd,puser->getpfrds(), pusersbyID);
            }
            else if(n > 2+strlen(acptfriend) && equalchars(str+2,acptfriend, strlen(acptfriend)-1)){
                frdID = atol(str+2+strlen(addfriend));
                acptfrd(sockfd, puser, pusersbyID,frdID);
            }
            else if(n >= 2+strlen(listfriendonline) && equalchars(str+2,listfriendonline, strlen(listfriendonline))){
                listfrdonline(sockfd, puser, pusersbyID);
            }
            else if(n >= 2+strlen(listvrfyfriend) && equalchars(str+2,listvrfyfriend, strlen(listvrfyfriend))){
                listfriends(sockfd, puser->getpvrfyfrds(), pusersbyID);
            }
            else if(n >= 2+strlen(listaddfriend) && equalchars(str+2,listaddfriend, strlen(listaddfriend))){
                for(auto i : *(puser->getpaddfrds())){
                    cout << "ID: "<< i<<endl;
                }
                listfriends(sockfd, puser->getpaddfrds(), pusersbyID);
            }
            else if(n >= 2+strlen(showcmd) && equalchars(str+2, showcmd, strlen(showcmd))){
                write(sockfd, showcmd, strlen(showcmd));
                write(sockfd, "to\n", strlen("to\n"));
                write(sockfd, addfriend, strlen(addfriend));
                write(sockfd, dltfriend, strlen(dltfriend));
                write(sockfd, acptfriend, strlen(acptfriend));
                write(sockfd, listfriend, strlen(listfriend));
                write(sockfd, listfriendonline, strlen(listfriendonline));
                write(sockfd, listvrfyfriend, strlen(listvrfyfriend));
                write(sockfd, listaddfriend, strlen(listaddfriend));
            }
            else{
                msg = std::move(string("err: no this command!\n"));
                write(sockfd, msg.c_str(), msg.size());
            }
            return 0;
        }
        else{
            goto sendmsg;
        }
    }
    else{
        goto sendmsg;
    }


sendmsg:    
    if(puser->getsts() != PEERSET){
        msg = string("input peer ID, such as \"!!to 12345\" starting with \"!!to\"\n");
        write(sockfd,msg.c_str(), msg.size());
        return 0;
    }

    sendmsg(sockfd, *puser, *puser->getpeeruser(), str, n);
    return 0;
}

