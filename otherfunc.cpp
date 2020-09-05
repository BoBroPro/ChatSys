#include"otherfunc.hpp"
#include"utility.hpp"
#include"ultoa.hpp"
#include"puserfrommapID.hpp"

using namespace std;

void to_json(json& j, const User& u){

    j = json{
        {"ID", u.ID},
        {"passwd", u.passwd}, 
        {"name", u.name}, 
        {"friends", u.friends}, 
        {"vrfyfrds", u.vrfyfrds}, 
        {"addfrds", u.addfrds}, 
        {"msgsnotread", u.msgsnotread}
    };
    return ;
}
void from_json(const json& j, User& u){

    j.at("ID").get_to(u.ID);
    j.at("passwd").get_to(u.passwd);
    j.at("name").get_to(u.name);
    j.at("friends").get_to(u.friends);
    j.at("vrfyfrds").get_to(u.vrfyfrds);
    j.at("addfrds").get_to(u.addfrds);
    j.at("msgsnotread").get_to(u.msgsnotread);
    return;
}

int findinredis(string ipaddr,PortTp port, IDTp ID, User& outuser){
        CHiredis credisConnect(ipaddr, port);
        string IDkey = "ID";
        char IDbuf[20] = {0};
        ultoa(IDbuf, sizeof(IDbuf),ID);
        IDkey.append(IDbuf);
        string val = credisConnect.get(IDkey);
        if(val.size() != 0){
            json j = json::parse(val);
            outuser = move(j.get<User>());
            return 0; 
        }
        else{
            return -1;
        }
}

string redisIDkey(IDTp ID){

    string rediskey = string("ID");
    char IDbuf[20];
    ultoa(IDbuf, sizeof(IDbuf), ID);
    rediskey.append(string(IDbuf));
    return rediskey;

}

int writeRedisUser(const string& ipaddr, PortTp port, const User& user){
    CHiredis chiredis(ipaddr, port);
    if(chiredis.geterr()<0){
        return -1;
    }
    string key = redisIDkey(user.getID());
    json j(user);
    string val = j.dump(4);
    chiredis.set(key, val);
    return 0;
}

int modifyRedisUser(const string& ipaddr, PortTp port, const User& user){
    CHiredis chiredis(ipaddr, port);
    if(chiredis.geterr()<0){
        return -1;
    }
    string key = redisIDkey(user.getID());
    json j(user);
    string val = j.dump(4);
    chiredis.getset(key, val);
    return 0;
}

int findUser(IDTp frdID, map<IDTp, User*>* pusersbyID/*and redis*/, User** outppusermap, User* outpuserRedis){
    if(puserfrommapID(frdID, pusersbyID, outppusermap) < 0){
        if(findinredis("127.0.0.1", 6379,frdID, *outpuserRedis) < 0){
            return -1;
        }
        else{

            return 1; // find in the redis
        }
    }
    else{
        return 0;  //find int he map
    }
}
