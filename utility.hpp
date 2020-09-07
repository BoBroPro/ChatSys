#ifndef UTILITY_HPP
#define UTILITY_HPP

#include<iostream>
#include<sys/socket.h>
#include<sys/select.h>
#include<stdio.h>
#include<string.h>
#include<netinet/in.h>
#include <unistd.h>
#include<arpa/inet.h>
#include<errno.h>
#include<list>
#include<queue>
#include"json.hpp"
#include"hiredisutility.hpp"

using json = nlohmann::json;

typedef unsigned long IDTp;

enum {
    SERV_PORT = 60000U,
    LINSTENQ = 1024,
    MAXCONNUM = 1024,
    MAXBUFLEN = 1024,
};

enum state {
    NONE= 0,
    CONNECTED,
    CHOSESIGNINORUP,
    INSIGNIN,
    INSIGNUP1,
    INSIGNUP2,
    INSIGNUP3,
    LOGINED,
    PEERSET
};
/*
enum cmd{
    ADD_FRIEND,
    DELET_FRIEND,
    LIST_ONLINE_FRIEND,
};

*/

class User{

friend void to_json(json& j, const User& u);
friend void from_json(const json& j, User& u);
    //friend bool  operator<(User u1, User u2);
    //friend bool operator==(User u1, User u2);
public:
    User() : ID(0), name(""), ipaddr(""),peeruser(NULL),sts(NONE){}
    User(IDTp ID_, std::string passwd_):ID(ID_), passwd(passwd_), name(""), ipaddr(""),peeruser(NULL),sts(NONE){}
    //User(int sockfd_):sockfd(sockfd_){};
    User(const User& user) = delete;

    User(User&& user){
        this->ID = user.ID;
        this->name = user.name;
        this->ipaddr = user.ipaddr;
        this->peeruser = user.peeruser;
        this->passwd = user.passwd;
        this->sts = user.sts;
        this->sockfd = user.sockfd;
        this->friends = move(user.friends);
        this->vrfyfrds = move(user.vrfyfrds);
        this->addfrds = move(user.addfrds);
        this->msgsnotread = move(user.msgsnotread);
        std::cout << "move:" << user.getpasswd() <<std::endl;

    }
    User& operator=(const User& user) = delete;

    User& operator=(User&& user){
        this->ID = user.ID;
        this->name = user.name;
        this->ipaddr = user.ipaddr;
        this->peeruser = user.peeruser;
        this->passwd = user.passwd;
        this->sts = user.sts;
        this->sockfd = user.sockfd;
        this->friends = move(user.friends);
        this->vrfyfrds = move(user.vrfyfrds);
        this->addfrds = move(user.addfrds);
        this->msgsnotread = move(user.msgsnotread);
        std::cout << "move:" << user.getpasswd() <<std::endl;
        return *this;
    }
    virtual ~User(){};
    


private:
    IDTp ID;
    std::string passwd;
    std::string passwdchck;
    std::string name; 
    std::string ipaddr;
    User* peeruser;
    state sts;    
    
    int sockfd;
    std::list<IDTp> friends;
    std::list<IDTp> vrfyfrds;   
    std::list<IDTp> addfrds;   
    std::vector<std::string> msgsnotread;

    std::list<IDTp>::iterator findIDinlist(std::list<IDTp>&l, IDTp ID){
        std::list<IDTp>::iterator it;
        for(it = l.begin();it!=l.end(); ++it){
            if(*it == ID){
                break;
            }
        }
        return it;
    }


public:
    void setID(IDTp ID_){
        ID = ID_;
    }
    IDTp getID()const{
        return ID;
    }
    void setpasswd(std::string passwd_){
        passwd = passwd_;
    }
    std::string getpasswd(){
        return passwd;
    }
    void setpasswdchck(std::string passwdchck_){
        passwdchck = passwdchck_;
    }
    std::string getpasswdchck(){
        return passwdchck;
    }

    void setname(std::string name_){
        name = name_;
    }
    std::string getname()const{
        return name;
    }

    void setipaddr(std::string ipaddr_){
        ipaddr = ipaddr_;
    }
    std::string getipaddr()const{
        return ipaddr;
    }

    void setpeeruser(User* peeruser_){
        peeruser = peeruser_;
    }
    User* getpeeruser()const{
        return peeruser;
    }

    void setsts(state sts_){
        sts = sts_;
    }
    state getsts()const{
        return sts;
    }    

    void setsockfd(int sockfd_){
        sockfd = sockfd_;
    }
    int getsockfd()const{
        return sockfd;
    }

    std::list<IDTp>* getpfrds(){
        return &friends;
    }
    std::list<IDTp>* getpvrfyfrds(){
        return &vrfyfrds;
    }
    std::list<IDTp>* getpaddfrds(){
        return &addfrds;
    }
    std::vector<std::string>* getpmsgsnotread(){
        return &msgsnotread;
    } 

    void setfrds(const std::list<IDTp>& frds){
        friends  = frds;
    }
    void setvrfyfrds(const std::list<IDTp>& vrfyfrds_){
        vrfyfrds = vrfyfrds_;
    }
    void setaddfrds(const std::list<IDTp> &addfrds_){
        addfrds = addfrds_;
    }
    void setmsgsnotread(const std::vector<std::string>& msgsnotread_){
        msgsnotread = msgsnotread_;
    } 

    void readmsg(){
        for(auto it = msgsnotread.begin(); it != msgsnotread.end();++it){
            write(sockfd, it->c_str(), (*it).size());
        }
        msgsnotread.clear();
    }
    
    std::list<IDTp>::iterator findIDinfrds(IDTp ID){
        return findIDinlist(friends, ID);
    }

    std::list<IDTp>::iterator findIDinaddfrds(IDTp ID){
        return findIDinlist(addfrds, ID);
    }
    std::list<IDTp>::iterator findIDinvrfyfrd(IDTp ID){
        return findIDinlist(vrfyfrds, ID);
    }


};

#endif