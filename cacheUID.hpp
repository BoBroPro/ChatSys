#ifndef CACHEUID_HPP
#define CACHEUID_HPP

#include"utility.hpp"
#include<queue>
#include<vector>
#include"otherfunc.hpp"

using namespace std;

class TimeUID{
    public:
    TimeUID();
    TimeUID(time_t tt, IDTp ID_):t(tt), ID(ID_){}
    bool operator > (const TimeUID& tu )const{
        return t > tu.t;
    }
    bool operator < (const TimeUID& tu )const{
        return t < tu.t;
    }

    time_t t;
    IDTp ID;
};

class CacheUID{

    enum{ CASHSIZE  = 2, };

public:
    static void push(const User& user){
        if(userpq.size() >= CASHSIZE){
            User targtuser;
            if(findinredis("127.0.0.1", 6379,userpq.top().ID, targtuser) < 0){
                //nothing
            }
            else{
                delUserRedis("127.0.0.1", 6379, userpq.top().ID);
            }
            userpq.pop(); 
        }
        userpq.emplace(time(NULL), user.getID());
        writeOrModifyUserRedis("127.0.0.1", 6379,user);
        return;
    }

private:
    static priority_queue<TimeUID, vector<TimeUID>, greater<TimeUID>> userpq;     
};

#endif