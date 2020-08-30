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

enum {
    SERV_PORT = 60000U,
    LINSTENQ = 1024,
    MAXCONNUM = 1024,
    MAXBUFLEN = 1024,

};


enum state {
    NONE= 0,
    CONNECTED,
    LOGINED,
    PEERSET
};


class User{

    //friend bool  operator<(User u1, User u2);
    //friend bool operator==(User u1, User u2);
public:
    User():ID(0),name(""),ipaddr(""),peeruser(NULL),sts(NONE){}
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
    

    void setID(unsigned long ID_){
        ID = ID_;
    }
    void setname(std::string name_){
        name = name_;
    }
    void setipaddr(std::string ipaddr_){
        ipaddr = ipaddr_;
    }

    void setpeeruser(User* peeruser_){
        peeruser = peeruser_;
    }
    void setsts(state sts_){
        sts = sts_;
    }
    void setsockfd(int sockfd_){
        sockfd = sockfd_;
    }

    unsigned long getID()const{
        return ID;
    }
    std::string getname()const{
        return name;
    }
    std::string getipaddr()const{
        return ipaddr;
    }
    User* getpeeruser()const{
        return peeruser;
    }

    state getsts()const{
        return sts;
    }    
    int getsockfd()const{
        return sockfd;
    }

private:
    unsigned long ID;
    std::string name; 
    std::string ipaddr;
    User* peeruser;
    state sts;    
    int sockfd;
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