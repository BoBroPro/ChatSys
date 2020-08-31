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
enum sign{
    SIGN_IN,
    SIGN_UP
};

class User{

    //friend bool  operator<(User u1, User u2);
    //friend bool operator==(User u1, User u2);
public:
    User() : ID(0), name(""), ipaddr(""),peeruser(NULL),sts(NONE){}
    User(IDTp ID_, std::string passwd):ID(ID_), name(""), ipaddr(""),peeruser(NULL),sts(NONE){}
    //User(int sockfd_):sockfd(sockfd_){};
    User(const User& user) = delete;
    User(User&& user){
        this->ID = user.ID;
        this->name = user.name;
        this->ipaddr = user.ipaddr;
        this->peeruser = user.peeruser;
        this->sts = user.sts;
 //       this->sockfd = user.sockfd;
    }
    //User operator=(const User& user) = delete;
    //virtual ~User(){};
    


private:
    IDTp ID;
    std::string passwd;
    std::string passwdchck;
    std::string name; 
    std::string ipaddr;
    User* peeruser;
    state sts;    
    int sockfd;
    

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
};

class Friend{
public:
    void setID(IDTp ID_){
        ID = ID_;
    }
    IDTp getID(){
        return ID;
    }

    void setOnlineSts(bool isOnline_){
        isOnline = isOnline_;
    }
    bool getOnlineSts(){
        return isOnline;
    }

private:
    IDTp ID;
    bool isOnline;

};
/*
bool operator<(User u1, User u2){
    return u1.sockfd < u2.sockfd;
}

bool operator==(User u1, User u2){
    return u1.sockfd == u2.sockfd;
}
*/

#endif