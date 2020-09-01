#include<string>
#include<stdio.h>
#include<algorithm>
#include"dealmsg.hpp"
#include"dealloginedmsg.hpp"
#include"ultoa.hpp"
#include<iterator>
#include"puserfrommapID.hpp"

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
    char msgbuf[50];
    int cnt = 0;
    string msg;
    map<IDTp, User*>::iterator it;
    User* ppeeruser;
    if(puserfrommapID(frdID, pusersbyID, &ppeeruser) < 0){
        msg = string("this user(ID:"); 
        mergestrIDstr(&msg, frdID,") doesn't exists\n");
        write(sockfd, msg.c_str(), msg.size());
        return;
    }

    puser->getpaddfrds()->push_back(frdID);
    //add peer's friend verificaion queue;
    ppeeruser->getpvrfyfrds()->push_back(puser->getID());

    // send message
    msg = string("a freind(ID: ");
    mergestrIDstr(&msg,frdID, ",");
    msg.append(ppeeruser->getname()).append(") requst has been sent\n");
    write(sockfd, msg.c_str(),msg.size()); 

    return;
}

static void acptfrd(int sockfd, User* puser, map<IDTp, User*>* pusersbyID, IDTp frdID){

    list<IDTp>* pvrfyfrds = puser->getpvrfyfrds();
    list<IDTp>* paddfrdsreq;
    User* ppeeruser;
    list<IDTp>::iterator it;
    string msg;
    for(it = pvrfyfrds->begin(); it != pvrfyfrds -> end();++it){
        // find the friend request
        if(*it == frdID){
            puser->getpfrds()->push_back(*it);
            pvrfyfrds->erase(it);
            break; 
        }
    }
    // found nothing
    if(it == pvrfyfrds->end()){
        msg = string("the friend with ID:");
        mergestrIDstr(&msg, frdID, " is not in your friend to be confirmed\n");
        write(sockfd,msg.c_str(), msg.size());
        return ;
    } 
    if((puserfrommapID(frdID, pusersbyID, &ppeeruser)) < 0){
        msg = string("the user with ID:");
        mergestrIDstr(&msg, frdID, " doesn't exit\n");
        write(sockfd,msg.c_str(), msg.size());
        return ;
    };
    paddfrdsreq = ppeeruser->getpaddfrds();

    for(it = paddfrdsreq->begin(); it!= paddfrdsreq->end(); ++it){
        if(*it == puser->getID()){
            paddfrdsreq->erase(it);
            ppeeruser->getpfrds()->push_back(puser->getID());

            msg = string("has accepted the friend(");
            mergestrIDstr(&msg, frdID,",");
            msg.append(ppeeruser->getname()).append(") request\n");
            write(sockfd, msg.c_str(), msg.size());

            break;
        } 
    }

    return;

}
static void dltfrd(int sockfd, User* puser, map<IDTp, User*>* pusersbyID, IDTp frdID){
    list<IDTp>*pfrds = puser->getpfrds();
    list<IDTp>*ppeerfrds;
    list<IDTp>::iterator it;
    User* ppeeruser;
    string msg;

    for(it = pfrds->begin(); it != pfrds->end(); ++it){
        if(*it == frdID){
            break; 
        }
    }
    if(it == pfrds->end()){
        msg = string("err, the user(");
        mergestrIDstr(&msg, frdID,",");
        msg.append(ppeeruser->getname()).append(") is not your friend\n");
        write(sockfd, msg.c_str(), msg.size());
        return ;
    }

    pfrds->erase(it);
    if((puserfrommapID(frdID, pusersbyID, &ppeeruser))< 0){
        msg = string("the user with ID:");
        mergestrIDstr(&msg, frdID, " doesn't exit\n");
        write(sockfd,msg.c_str(), msg.size());
        return ;
    };
    ppeerfrds= ppeeruser->getpfrds();
    for(it = ppeerfrds->begin(); it!=ppeerfrds->end(); ++it){
        if(*it == puser->getID()){
            ppeerfrds->erase(it);
            break;
        }
    }
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
    for(auto it = pfrds->begin(); it != pfrds->end(); ++ it){
        map<IDTp, User*>::iterator itusersbyID;
        if((itusersbyID = pusersbyID->find(*it)) != pusersbyID->end()){ 
            userinfo = userinfo2str(itusersbyID->second);
            write(sockfd, userinfo.c_str(), strlen(userinfo.c_str()));
        }
    }
    return;
}



int dealmsg(int sockfd, User* puser, map<unsigned long, User*>* pusersbyID,  char* str, size_t n){
    string msg;
    char IDbuf[20];
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
                listfriends(sockfd, puser->getpaddfrds(), pusersbyID);
            }
            else if(n >= 2+strlen(showcmd) && equalchars(str+2, showcmd, strlen(showcmd))){
                write(sockfd, addfriend, strlen(addfriend));
                write(sockfd, dltfriend, strlen(dltfriend));
                write(sockfd, listfriend, strlen(listfriend));
                write(sockfd, acptfriend, strlen(acptfriend));
                write(sockfd, listfriendonline, strlen(listfriendonline));
                write(sockfd, showcmd, strlen(showcmd));
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
            goto msg;
        }
    }
    else{
        goto msg;
    }

msg:
    // find peeruser
    if(puser->getsts() != PEERSET){
        msg = string("input peer ID, such as \"!!to 12345\" starting with \"!!to\"\n");
        write(sockfd,msg.c_str(), msg.size());
        return 0;
    }

    int peerusersockfd = puser->getpeeruser()->getsockfd();

    int cnt = ultoa(IDbuf, sizeof(IDbuf), puser->getID());
    msg = string(IDbuf).append(",").append(puser->getname()).append(": ");
    for(int i = 0; i< n; ++i){
        msg.push_back(str[i]);
    }
    cout << "peer stats: " << puser->getpeeruser()->getsts()<<endl;
    if(puser->getpeeruser()->getsts()!=LOGINED && puser->getpeeruser()->getsts()!=PEERSET){
        puser->getpeeruser()->getpmsgsnotread()->push(msg);
        cout << msg <<endl;
        cout << "push in the msg" <<endl;
    }
    else{
        cout << "jinlaile"<<endl;
        write(peerusersockfd, msg.c_str(), msg.size()+1);    
    }

    return 0;
}

