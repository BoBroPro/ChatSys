#include"utility.hpp"
#include<queue>
#include<vector>
#include"otherfunc.hpp"

using namespace std;

class CacheUID{

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

    enum{ CASHSIZE  = 4, };

public:
    static void push(const User& user){
        if(userpq.size() >= CASHSIZE){
            userpq.pop(); 
            delUserRedis("127.0.0.1", 6379, user.getID());
        }
        userpq.emplace(time(NULL), user.getID());
        writeOrModifyUserRedis("127.0.0.1", 6379,user);
    }

private:
    static priority_queue<TimeUID, vector<TimeUID>, greater<TimeUID>> userpq;     

};
