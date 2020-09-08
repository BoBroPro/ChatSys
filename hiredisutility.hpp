#ifndef HIREDISUTILITY_HPP
#define HIREDISUTILITY_HPP

#include"hiredis/hiredis.h"
#include"hiredis/read.h"
#include"hiredis/alloc.h"
#include"hiredis/async.h"
#include"hiredis/sds.h"
#include<iostream>
#include<stdarg.h>
#include<queue>
#include<vector>

typedef unsigned short PortTp;

class CHiredis{

public:
    CHiredis(std::string ipaddr_, PortTp port_):ipaddr(ipaddr_),port(port_),err(0){
        predisContext = redisConnect(ipaddr.c_str(),port);
        if(predisContext->err !=0){
            err = -1;
        }
    };
    CHiredis(const CHiredis & ) = delete;
    CHiredis& operator=(const CHiredis&) = delete;
    CHiredis(CHiredis && oldchiredis){
        ipaddr =  std::move(oldchiredis.ipaddr);
        port = std::move(oldchiredis.port);
        predisContext  = std::move(oldchiredis.predisContext);
        err = std::move(oldchiredis.err);
    }
    ~CHiredis(){
        redisFree(predisContext);
    };

    void set(const std::string& key, const std::string& val){
        std::cout <<"set:\n" << key <<std::endl<<val<<std::endl;
        redisCommand(predisContext,"set %s %s", key.c_str(), val.c_str());
    }
    std::string get(const std::string& key){
        const char* argv[2];
        size_t argvlen[2];
        argv[0] = "get";
        argvlen[0] = 3;
        argv[1] = key.c_str();
        argvlen[1] = key.size();

        predisReply = (redisReply*)redisCommandArgv(predisContext, 2, argv, argvlen);
        std::string val;
        if(predisReply->type != REDIS_REPLY_NIL){
            val = std::string(predisReply->str,predisReply->len);
        }
        else{
            err = -2;
        }
        std::cout << "get:\n" << val<<std::endl;
        freeReplyObject(predisReply);
        return val;
    }

    void getset(const std::string& key, const std::string& val){
        std::cout <<"getset:\n"<< key << std::endl << val<<std::endl;
        redisCommand(predisContext, "GETSET %s %s",key.c_str(), val.c_str());
    }

    void del(const std::string& key){
        std::cout <<"delete:\n"<< key << std::endl;
        redisCommand(predisContext, "DEL %s",key.c_str());

    }

    int geterr(){
        return err;
    }

private:
    std::string ipaddr;
    PortTp port;
    redisContext* predisContext;
    redisReply* predisReply;
    int err;
};






#endif